/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "window_node_container.h"

#include <algorithm>
#include <ability_manager_client.h>
#include <cinttypes>

#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_layout_policy_cascade.h"
#include "window_layout_policy_tile.h"
#include "window_manager_agent_controller.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_trace.h"
#include "common_event_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeContainer"};
    constexpr int WINDOW_NAME_MAX_LENGTH = 10;
}

WindowNodeContainer::WindowNodeContainer(DisplayId displayId, uint32_t width, uint32_t height) : displayId_(displayId)
{
    displayRect_ = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    layoutPolicys_[WindowLayoutMode::CASCADE] =
        new WindowLayoutPolicyCascade(displayRect_, displayId_,
            belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    layoutPolicys_[WindowLayoutMode::TILE] =
        new WindowLayoutPolicyTile(displayRect_, displayId_,
            belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    layoutPolicy_ = layoutPolicys_[WindowLayoutMode::CASCADE];
    layoutPolicy_->Launch();
    UpdateAvoidAreaFunc func = std::bind(&WindowNodeContainer::OnAvoidAreaChange, this, std::placeholders::_1);
    avoidController_ = new AvoidAreaController(func);
}

WindowNodeContainer::~WindowNodeContainer()
{
    Destroy();
}

WMError WindowNodeContainer::MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node)
{
    std::vector<uint32_t> exceptionalIds = { node->GetWindowId() };
    std::vector<WindowMode> exceptionalModes = { WindowMode::WINDOW_MODE_FLOATING, WindowMode::WINDOW_MODE_PIP };
    return MinimizeAppNodeExceptOptions(exceptionalIds, exceptionalModes);
}

WMError WindowNodeContainer::AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (!node->surfaceNode_) {
        WLOGFE("surface node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<WindowNode> root = FindRoot(node->GetWindowType());
    if (root == nullptr) {
        WLOGFE("root window node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->requestedVisibility_ = true;
    if (parentNode != nullptr) { // subwindow
        if (parentNode->parent_ != root) {
            WLOGFE("window type and parent window not match or try to add subwindow to subwindow, which is forbidden");
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        node->currentVisibility_ = parentNode->currentVisibility_;
    } else { // mainwindow
        parentNode = root;
        node->currentVisibility_ = true;
        for (auto& child : node->children_) {
            child->currentVisibility_ = child->requestedVisibility_;
        }
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
            node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
            sysBarNodeMap_[node->GetWindowType()] = node;
        }
    }
    node->parent_ = parentNode;

    if (node->IsSplitMode()) {
        WMError ret = EnterSplitWindowMode(node);
        if (ret != WMError::WM_OK) {
            WLOGFE("Add split window failed!");
            return ret;
        }
    }
    UpdateWindowTree(node);

    UpdateRSTree(node, true);
    AssignZOrder();
    layoutPolicy_->AddWindowNode(node);
    if (avoidController_->IsAvoidAreaNode(node)) {
        avoidController_->AddAvoidAreaNode(node);
        NotifyIfSystemBarRegionChanged();
    }
    NotifyIfSystemBarTintChanged();
    DumpScreenWindowTree();
    UpdateWindowStatus(node, WindowUpdateType::WINDOW_UPDATE_ADDED);
    WLOGFI("AddWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowNode(sptr<WindowNode>& node)
{
    if (!node->surfaceNode_) {
        WLOGFE("surface node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE);
    layoutPolicy_->UpdateWindowNode(node);
    if (avoidController_->IsAvoidAreaNode(node)) {
        avoidController_->UpdateAvoidAreaNode(node);
        NotifyIfSystemBarRegionChanged();
    }
    NotifyIfSystemBarTintChanged();
    DumpScreenWindowTree();
    WLOGFI("UpdateWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateWindowTree(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    node->priority_ = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    RaiseInputMethodWindowPriorityIfNeeded(node);
    auto parentNode = node->parent_;
    auto position = parentNode->children_.end();
    for (auto iter = parentNode->children_.begin(); iter < parentNode->children_.end(); ++iter) {
        if ((*iter)->priority_ > node->priority_) {
            position = iter;
            break;
        }
    }
    parentNode->children_.insert(position, node);
}

bool WindowNodeContainer::UpdateRSTree(sptr<WindowNode>& node, bool isAdd)
{
    WM_FUNCTION_TRACE();
    if (isAdd) {
        DisplayManagerServiceInner::GetInstance().UpdateRSTree(displayId_, node->surfaceNode_, true);
        for (auto& child : node->children_) {
            if (child->currentVisibility_) {
                DisplayManagerServiceInner::GetInstance().UpdateRSTree(displayId_, child->surfaceNode_, true);
            }
        }
    } else {
        DisplayManagerServiceInner::GetInstance().UpdateRSTree(displayId_, node->surfaceNode_, false);
        for (auto& child : node->children_) {
            DisplayManagerServiceInner::GetInstance().UpdateRSTree(displayId_, child->surfaceNode_, false);
        }
    }
    return true;
}

WMError WindowNodeContainer::DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds)
{
    WMError ret = RemoveWindowNode(node);
    if (ret != WMError::WM_OK) {
        WLOGFE("RemoveWindowNode failed");
        return ret;
    }
    node->surfaceNode_ = nullptr;
    windowIds.push_back(node->GetWindowId());

    for (auto& child : node->children_) { // destroy sub window if exists
        windowIds.push_back(child->GetWindowId());
        child->parent_ = nullptr;
        if (child->surfaceNode_ != nullptr) {
            WLOGFI("child surfaceNode set nullptr");
            child->surfaceNode_ = nullptr;
        }
    }
    node->children_.clear();
    return WMError::WM_OK;
}

WMError WindowNodeContainer::RemoveWindowNode(sptr<WindowNode>& node)
{
    if (node == nullptr || !node->surfaceNode_) {
        WLOGFE("window node or surface node is nullptr, invalid");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE);
    if (node->parent_ == nullptr) {
        WLOGFW("can't find parent of this node");
    } else {
        // remove this node from parent
        auto iter = std::find(node->parent_->children_.begin(), node->parent_->children_.end(), node);
        if (iter != node->parent_->children_.end()) {
            node->parent_->children_.erase(iter);
        } else {
            WLOGFE("can't find this node in parent");
        }
        node->parent_ = nullptr;
    }
    node->requestedVisibility_ = false;
    node->currentVisibility_ = false;
    node->hasDecorated = false;
    for (auto& child : node->children_) {
        child->currentVisibility_ = false;
    }

    if (node->IsSplitMode()) {
        WMError ret = ExitSplitWindowMode(node);
        if (ret != WMError::WM_OK) {
            WLOGFE("Remove split window failed!");
            return ret;
        }
    }

    UpdateRSTree(node, false);
    layoutPolicy_->RemoveWindowNode(node);
    if (avoidController_->IsAvoidAreaNode(node)) {
        avoidController_->RemoveAvoidAreaNode(node);
        NotifyIfSystemBarRegionChanged();
    }
    NotifyIfSystemBarTintChanged();
    DumpScreenWindowTree();
    UpdateWindowStatus(node, WindowUpdateType::WINDOW_UPDATE_REMOVED);
    WLOGFI("RemoveWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

const std::vector<uint32_t>& WindowNodeContainer::Destroy()
{
    removedIds_.clear();
    for (auto& node : belowAppWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    for (auto& node : appWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    return removedIds_;
}

sptr<WindowNode> WindowNodeContainer::FindRoot(WindowType type) const
{
    if (WindowHelper::IsAppWindow(type) || type == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        return appWindowNode_;
    }
    if (WindowHelper::IsBelowSystemWindow(type)) {
        return belowAppWindowNode_;
    }
    if (WindowHelper::IsAboveSystemWindow(type)) {
        return aboveAppWindowNode_;
    }
    return nullptr;
}

sptr<WindowNode> WindowNodeContainer::FindWindowNodeById(uint32_t id) const
{
    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (auto& rootNode : rootNodes) {
        for (auto& node : rootNode->children_) {
            if (node->GetWindowId() == id) {
                return node;
            }
            for (auto& subNode : node->children_) {
                if (subNode->GetWindowId() == id) {
                    return subNode;
                }
            }
        }
    }
    return nullptr;
}

void WindowNodeContainer::UpdateFocusStatus(uint32_t id, bool focused) const
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFW("cannot find focused window id:%{public}d", id);
    } else {
        node->GetWindowToken()->UpdateFocusStatus(focused);
        if (node->abilityToken_ == nullptr) {
            WLOGFI("abilityToken is null, window : %{public}d", id);
        }
        WindowManagerAgentController::GetInstance().UpdateFocusStatus(
            node->GetWindowId(), node->abilityToken_, node->GetWindowType(), node->GetDisplayId(), focused);
    }
}

void WindowNodeContainer::AssignZOrder()
{
    zOrder_ = 0;
    for (auto& node : belowAppWindowNode_->children_) {
        AssignZOrder(node);
    }
    for (auto& node : appWindowNode_->children_) {
        AssignZOrder(node);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        AssignZOrder(node);
    }
}

void WindowNodeContainer::AssignZOrder(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    auto iter = node->children_.begin();
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->priority_ < 0) {
            if ((*iter)->surfaceNode_) {
                (*iter)->surfaceNode_->SetPositionZ(zOrder_);
                ++zOrder_;
            }
        } else {
            break;
        }
    }
    if (node->surfaceNode_) {
        node->surfaceNode_->SetPositionZ(zOrder_);
        ++zOrder_;
    }
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->surfaceNode_) {
            (*iter)->surfaceNode_->SetPositionZ(zOrder_);
            ++zOrder_;
        }
    }
}

WMError WindowNodeContainer::SetFocusWindow(uint32_t windowId)
{
    if (focusedWindow_ == windowId) {
        WLOGFI("focused window do not change");
        return WMError::WM_DO_NOTHING;
    }
    UpdateFocusStatus(focusedWindow_, false);
    focusedWindow_ = windowId;
    sptr<WindowNode> node = FindWindowNodeById(windowId);
    UpdateWindowStatus(node, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    UpdateFocusStatus(focusedWindow_, true);
    return WMError::WM_OK;
}

uint32_t WindowNodeContainer::GetFocusWindow() const
{
    return focusedWindow_;
}

sptr<WindowNode> WindowNodeContainer::GetTopImmersiveNode() const
{
    if (appWindowNode_->children_.empty()) {
        return nullptr;
    }
    auto iter = appWindowNode_->children_.rbegin();
    for (; iter < appWindowNode_->children_.rend(); ++iter) {
        auto mode = (*iter)->GetWindowMode();
        auto flags = (*iter)->GetWindowFlags();
        if (mode == WindowMode::WINDOW_MODE_FULLSCREEN &&
            !(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID))) {
            return (*iter);
        }
    }
    return nullptr;
}

void WindowNodeContainer::NotifyIfSystemBarTintChanged()
{
    WM_FUNCTION_TRACE();
    auto node = GetTopImmersiveNode();
    SystemBarRegionTints tints;
    if (node == nullptr) { // use default system bar
        WLOGFI("no immersive window on top");
        for (auto it : sysBarTintMap_) {
            if (it.second.prop_ == SystemBarProperty()) {
                continue;
            }
            WLOGFI("system bar prop change to default");
            sysBarTintMap_[it.first].prop_ = SystemBarProperty();
            sysBarTintMap_[it.first].type_ = it.first;
            tints.emplace_back(sysBarTintMap_[it.first]);
        }
    } else { // use node-defined system bar
        WLOGFI("top immersive window id: %{public}d", node->GetWindowId());
        auto& sysBarPropMap = node->GetSystemBarProperty();
        for (auto it : sysBarTintMap_) {
            if (sysBarPropMap.find(it.first) == sysBarPropMap.end()) {
                continue;
            }
            auto& prop = sysBarPropMap.find(it.first)->second;
            if (it.second.prop_ == prop) {
                continue;
            }
            WLOGFI("system bar prop update winId: %{public}d, type: %{public}d" \
                "visible: %{public}d, Color: %{public}x | %{public}x",
                node->GetWindowId(), static_cast<int32_t>(it.first),
                prop.enable_, prop.backgroundColor_, prop.contentColor_);
            sysBarTintMap_[it.first].prop_ = prop;
            sysBarTintMap_[it.first].type_ = it.first;
            tints.emplace_back(sysBarTintMap_[it.first]);
        }
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifyIfSystemBarRegionChanged()
{
    WM_FUNCTION_TRACE();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) { // split screen mode not support yet
        auto sysNode = sysBarNodeMap_[it.first];
        if (sysNode == nullptr || it.second.region_ == sysNode->GetLayoutRect()) {
            continue;
        }
        auto& newRegion = sysNode->GetLayoutRect();
        sysBarTintMap_[it.first].region_ = newRegion;
        sysBarTintMap_[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap_[it.first]);
        WLOGFI("system bar region update, type: %{public}d" \
            "region: [%{public}d, %{public}d, %{public}d, %{public}d]",
            static_cast<int32_t>(it.first), newRegion.posX_, newRegion.posY_, newRegion.width_, newRegion.height_);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

bool WindowNodeContainer::IsTopAppWindow(uint32_t windowId) const
{
    auto node = *(appWindowNode_->children_.rbegin());
    if (node == nullptr) {
        WLOGFE("window tree does not have any node");
        return false;
    }

    for (auto iter = node->children_.rbegin(); iter < node->children_.rend(); iter++) {
        if ((*iter)->priority_ > 0) {
            return (*iter)->GetWindowId() == windowId;
        } else {
            break;
        }
    }

    return node->GetWindowId() == windowId;
}

void WindowNodeContainer::RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes)
{
    auto iter = std::find_if(windowNodes.begin(), windowNodes.end(),
                             [windowId](sptr<WindowNode> node) {
                                 return node->GetWindowId() == windowId;
                             });
    // raise app node window to top
    if (iter != windowNodes.end()) {
        sptr<WindowNode> node = *iter;
        windowNodes.erase(iter);
        UpdateWindowTree(node);
        WLOGFI("raise window to top %{public}d", node->GetWindowId());
    }
}

void WindowNodeContainer::UpdateWindowStatus(const sptr<WindowNode>& node, WindowUpdateType type) const
{
    if (node == nullptr) {
        WLOGFE("window node is null");
        return;
    }
    bool isNeedNotify = false;
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED:
            if (node->currentVisibility_) {
                isNeedNotify = true;
            }
            break;
        case WindowUpdateType::WINDOW_UPDATE_FOCUSED:
            if (node->GetWindowId() == focusedWindow_) {
                isNeedNotify = true;
            }
            break;
        case WindowUpdateType::WINDOW_UPDATE_REMOVED:
            isNeedNotify = true;
            break;
        default:
            break;
    }
    if (isNeedNotify) {
        sptr<WindowInfo> windowInfo = new WindowInfo();
        windowInfo->wid_ = node->GetWindowId();
        windowInfo->windowRect_ = node->GetLayoutRect();
        windowInfo->focused_ = node->GetWindowId() == focusedWindow_;
        windowInfo->mode_ = node->GetWindowMode();
        windowInfo->type_ = node->GetWindowType();
        WindowManagerAgentController::GetInstance().UpdateWindowStatus(windowInfo, type);
    }
}

void WindowNodeContainer::TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const
{
    for (auto& node : belowAppWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    for (auto& node : appWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    std::reverse(windowNodes.begin(), windowNodes.end());
}

void WindowNodeContainer::TraverseWindowNode(sptr<WindowNode>& node, std::vector<sptr<WindowNode>>& windowNodes) const
{
    if (node == nullptr) {
        return;
    }
    auto iter = node->children_.begin();
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->priority_ < 0) {
            windowNodes.emplace_back(*iter);
        } else {
            break;
        }
    }
    windowNodes.emplace_back(node);
    for (; iter < node->children_.end(); ++iter) {
        windowNodes.emplace_back(*iter);
    }
}

std::vector<Rect> WindowNodeContainer::GetAvoidAreaByType(AvoidAreaType avoidAreaType)
{
    return avoidController_->GetAvoidAreaByType(avoidAreaType);
}

void WindowNodeContainer::OnAvoidAreaChange(const std::vector<Rect>& avoidArea)
{
    layoutPolicy_->UpdateDefaultFoatingRect();
    for (auto& node : appWindowNode_->children_) {
        if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN && node->GetWindowToken() != nullptr) {
            // notify client
            node->GetWindowToken()->UpdateAvoidArea(avoidArea);
        }
    }
}

void WindowNodeContainer::DumpScreenWindowTree()
{
    WLOGFI("-------- Screen %{public}" PRIu64" dump window info begin---------", displayId_);
    WLOGFI("WindowName WinId Type Mode Flag ZOrd [   x    y    w    h]");
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    int zOrder = windowNodes.size();
    for (auto node : windowNodes) {
        Rect rect = node->GetLayoutRect();
        const std::string& windowName = node->GetWindowName().size() < WINDOW_NAME_MAX_LENGTH ?
            node->GetWindowName() : node->GetWindowName().substr(0, WINDOW_NAME_MAX_LENGTH);
        WLOGFI("%{public}10s %{public}5d %{public}4d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
            "%{public}4d %{public}4d]", windowName.c_str(), node->GetWindowId(), node->GetWindowType(),
            node->GetWindowMode(), node->GetWindowFlags(),
            --zOrder, rect.posX_, rect.posY_, rect.width_, rect.height_);
    }
    WLOGFI("-------- Screen %{public}" PRIu64" dump window info end  ---------", displayId_);
}

uint64_t WindowNodeContainer::GetScreenId() const
{
    return DisplayManagerServiceInner::GetInstance().GetRSScreenId(displayId_);
}

DisplayId WindowNodeContainer::GetDisplayId() const
{
    return displayId_;
}

Rect WindowNodeContainer::GetDisplayRect() const
{
    return displayRect_;
}

bool WindowNodeContainer::isVerticalDisplay() const
{
    return displayRect_.width_ < displayRect_.height_;
}

void WindowNodeContainer::NotifyWindowStateChange(WindowState state, WindowStateChangeReason reason)
{
    switch (reason) {
        case WindowStateChangeReason::KEYGUARD: {
            int32_t topPriority = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD);
            TraverseAndUpdateWindowState(state, topPriority);
            break;
        }
        default:
            return;
    }
}

void WindowNodeContainer::TraverseAndUpdateWindowState(WindowState state, int32_t topPriority)
{
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    for (auto& node : rootNodes) {
        UpdateWindowState(node, topPriority, state);
    }
}

void WindowNodeContainer::UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state)
{
    if (node == nullptr) {
        return;
    }
    if (node->parent_ != nullptr && node->currentVisibility_) {
        if (node->priority_ < topPriority) {
            node->GetWindowToken()->UpdateWindowState(state);
        }
    }
    for (auto& childNode : node->children_) {
        UpdateWindowState(childNode, topPriority, state);
    }
}

sptr<WindowNode> WindowNodeContainer::FindDividerNode() const
{
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end(); iter++) {
        if ((*iter)->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            return *iter;
        }
    }
    return nullptr;
}

void WindowNodeContainer::RaiseZOrderForSplitWindow(sptr<WindowNode>& node)
{
    auto divider = FindDividerNode();
    if (pairedWindowMap_.count(node->GetWindowId()) != 0 && (divider != nullptr)) {
        auto pairNode = pairedWindowMap_.at(node->GetWindowId()).pairNode_;
        // remove split related node from tree
        for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end();) {
            uint32_t wid = (*iter)->GetWindowId();
            if (wid == node->GetWindowId() ||
                wid == pairNode->GetWindowId() ||
                wid == divider->GetWindowId()) {
                iter = appWindowNode_->children_.erase(iter);
            } else {
                iter++;
            }
        }
        UpdateWindowTree(pairNode); // raise pair node
        UpdateWindowTree(node); // raise self
        UpdateWindowTree(divider); // devider
    } else {
        // raise self if not paired
        RaiseWindowToTop(node->GetWindowId(), appWindowNode_->children_);
    }
}

WMError WindowNodeContainer::RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (node == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    if (IsTopAppWindow(node->GetWindowId())) {
        WLOGFI("it is already top app window, id: %{public}d", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    if (WindowHelper::IsSubWindow(node->GetWindowType())) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_NULLPTR;
        }
        RaiseWindowToTop(node->GetWindowId(), parentNode->children_); // raise itself
        if (parentNode->IsSplitMode()) {
            RaiseZOrderForSplitWindow(parentNode);
        } else {
            RaiseWindowToTop(node->GetParentId(), appWindowNode_->children_); // raise parent window
        }
    } else if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (parentNode->IsSplitMode()) {
            RaiseZOrderForSplitWindow(node);
        } else {
            RaiseWindowToTop(node->GetWindowId(), appWindowNode_->children_);
        }
    } else {
        // do nothing
    }
    AssignZOrder();
    WLOGFI("RaiseZOrderForAppWindow finished");
    DumpScreenWindowTree();
    return WMError::WM_OK;
}

sptr<WindowNode> WindowNodeContainer::GetNextFocusableWindow(uint32_t windowId) const
{
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    auto iter = std::find_if(windowNodes.begin(), windowNodes.end(),
                             [windowId](sptr<WindowNode>& node) {
                                 return node->GetWindowId() == windowId;
                             });
    if (iter != windowNodes.end()) {
        int index = std::distance(windowNodes.begin(), iter);
        for (int i = index + 1; i < windowNodes.size(); i++) {
            if (windowNodes[i]->GetWindowProperty()->GetFocusable()) {
                return windowNodes[i];
            }
        }
    }
    WLOGFI("could not get next focusable window");
    return nullptr;
}

void WindowNodeContainer::MinimizeAllAppWindows()
{
    WMError ret =  MinimizeAppNodeExceptOptions();
    if (ret != WMError::WM_OK) {
        WLOGFE("Minimize all app window failed");
    }
    return;
}

void WindowNodeContainer::SendSplitScreenEvent(WindowMode mode)
{
    // should define in common_event_support.h and @ohos.commonEvent.d.ts
    WLOGFI("send split sceen event , trigger mode is %{public}d", mode);
    const std::string eventName = "common.event.SPLIT_SCREEN";
    AAFwk::Want want;
    want.SetAction(eventName);
    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        commonEventData.SetData("Secondary");
    } else {
        commonEventData.SetData("Primary");
    }
    EventFwk::CommonEventManager::PublishCommonEvent(commonEventData);
}

sptr<WindowNode> WindowNodeContainer::FindSplitPairNode(sptr<WindowNode>& triggerNode) const
{
    auto triggerMode = triggerNode->GetWindowMode();
    for (auto iter = appWindowNode_->children_.rbegin(); iter != appWindowNode_->children_.rend(); iter++) {
        if ((*iter)->GetWindowId() == triggerNode->GetWindowId()) {
            continue;
        }
        // Find Top FullScreen main winodow or top paired split mode app main window
        if ((*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN ||
            (triggerMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
            (*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) ||
            (triggerMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY &&
            (*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
            WLOGFI("Find pair node mode is: %{public}d", static_cast<uint32_t>((*iter)->GetWindowMode()));
            return *iter;
        }
    }
    return nullptr;
}

void WindowNodeContainer::MinimizeWindowFromAbility(const sptr<WindowNode>& node)
{
    if (node->abilityToken_ == nullptr) {
        WLOGFW("Target abilityToken is nullptr, windowId:%{public}u", node->GetWindowId());
        return;
    }
    AAFwk::AbilityManagerClient::GetInstance()->DoAbilityBackground(node->abilityToken_,
        static_cast<uint32_t>(WindowStateChangeReason::NORMAL));
}

WMError WindowNodeContainer::MinimizeAppNodeExceptOptions(const std::vector<uint32_t> &exceptionalIds,
                                                          const std::vector<WindowMode> &exceptionalModes)
{
    if (appWindowNode_->children_.empty()) {
        return WMError::WM_OK;
    }
    for (auto& appNode : appWindowNode_->children_) {
        // exclude exceptional window
        if (std::find(exceptionalIds.begin(), exceptionalIds.end(), appNode->GetWindowId()) != exceptionalIds.end() ||
            std::find(exceptionalModes.begin(), exceptionalModes.end(),
                appNode->GetWindowMode()) != exceptionalModes.end() ||
                appNode->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            continue;
        }
        // minimize window
        WLOGFI("minimize window, windowId:%{public}u", appNode->GetWindowId());
        MinimizeWindowFromAbility(appNode);
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::EnterSplitWindowMode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    WLOGFI("Enter split window mode: %{public}d", node->GetWindowId());
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE); // window split mode is belong to cascade
    auto pairNode = FindSplitPairNode(node);
    if (pairNode != nullptr) {
        WLOGFI("Window %{public}d find pair %{public}d", node->GetWindowId(), pairNode->GetWindowId());
        WMError ret = UpdateWindowPairInfo(node, pairNode);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_CREATE_DIVIDER, displayId_);
        std::vector<uint32_t> exceptionalIds;
        for (auto iter = pairedWindowMap_.begin(); iter != pairedWindowMap_.end(); iter++) {
            exceptionalIds.emplace_back(iter->first);
        }
        std::vector<WindowMode> exceptionalModes = { WindowMode::WINDOW_MODE_FLOATING, WindowMode::WINDOW_MODE_PIP };
        ret = MinimizeAppNodeExceptOptions(exceptionalIds, exceptionalModes);
        if (ret != WMError::WM_OK) {
            return ret;
        }
    } else {
        SendSplitScreenEvent(node->GetWindowMode());
    }
    return WMError::WM_OK;
}

void WindowNodeContainer::ResetLayoutPolicy()
{
    layoutPolicy_->Reset();
}

WMError WindowNodeContainer::ExitSplitWindowMode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    WLOGFI("exit split window mode %{public}d", node->GetWindowId());
    if (pairedWindowMap_.find(node->GetWindowId()) != pairedWindowMap_.end()) {
        WindowPairInfo info = pairedWindowMap_.at(node->GetWindowId());
        auto pairNode = info.pairNode_;
        pairNode->GetWindowProperty()->ResumeLastWindowMode();
        pairNode->GetWindowToken()->UpdateWindowMode(pairNode->GetWindowMode());
        node->GetWindowProperty()->ResumeLastWindowMode();
        node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
        pairedWindowMap_.erase(pairNode->GetWindowId());
        pairedWindowMap_.erase(node->GetWindowId());
        WLOGFI("Split out, Id[%{public}d, %{public}d], Mode[%{public}d, %{public}d]",
            node->GetWindowId(), pairNode->GetWindowId(),
            node->GetWindowMode(), pairNode->GetWindowMode());
    } else {
        WLOGFE("Split out, but can not find pair in map  %{public}d", node->GetWindowId());
        return WMError::WM_OK;
    }
    if (pairedWindowMap_.empty()) {
        SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    }
    ResetLayoutPolicy();
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowPairInfo(sptr<WindowNode>& triggerNode, sptr<WindowNode>& pairNode)
{
    float splitRatio = DEFAULT_SPLIT_RATIO;
    WindowMode triggerMode = triggerNode->GetWindowMode();
    WindowMode pairSrcMode = pairNode->GetWindowMode();
    if (pairSrcMode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        WindowMode pairDstMode = (triggerMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) ?
            WindowMode::WINDOW_MODE_SPLIT_SECONDARY : WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
        pairNode->SetWindowMode(pairDstMode);
        pairNode->GetWindowToken()->UpdateWindowMode(pairDstMode);
        WMError ret = UpdateWindowNode(pairNode);
        if (ret != WMError::WM_OK) {
            WLOGFE("Update window pair info failed");
            return ret;
        }
        WLOGFI("Pair FullScreen [%{public}d, %{public}d], Mode[%{public}d, %{public}d], splitRatio = %{public}f",
            triggerNode->GetWindowId(), pairNode->GetWindowId(), triggerMode, pairDstMode, splitRatio);
    } else {
        if (pairedWindowMap_.count(pairNode->GetWindowId()) != 0) {
            WindowPairInfo info = pairedWindowMap_.at(pairNode->GetWindowId());
            auto prevPairNode = info.pairNode_;
            WLOGFI("%{public}d node is paird , pre paired id is %{public}d,",
                pairNode->GetWindowId(), prevPairNode->GetWindowId());
            prevPairNode->GetWindowProperty()->ResumeLastWindowMode();
            prevPairNode->GetWindowToken()->UpdateWindowMode(prevPairNode->GetWindowMode());

            pairedWindowMap_.erase(prevPairNode->GetWindowId());
            pairedWindowMap_.erase(pairNode->GetWindowId());

            splitRatio = info.splitRatio_;
            WLOGFI("Pair Split [%{public}d, %{public}d], Mode[%{public}d, %{public}d], splitRatio = %{public}f",
                triggerNode->GetWindowId(), pairNode->GetWindowId(), triggerMode, pairSrcMode, splitRatio);
        } else {
            WLOGFI("%{public}d node is not paird", pairNode->GetWindowId());
        }
    }
    pairedWindowMap_.insert(std::pair<uint32_t, WindowPairInfo>(triggerNode->GetWindowId(),
        {pairNode, splitRatio}));
    pairedWindowMap_.insert(std::pair<uint32_t, WindowPairInfo>(pairNode->GetWindowId(),
        {triggerNode, 1 - splitRatio}));
    return WMError::WM_OK;
}

WMError WindowNodeContainer::SwitchLayoutPolicy(WindowLayoutMode mode, bool reorder)
{
    WLOGFI("SwitchLayoutPolicy src: %{public}d dst: %{public}d reorder: %{public}d",
        static_cast<uint32_t>(layoutMode_), static_cast<uint32_t>(mode), static_cast<uint32_t>(reorder));
    if (layoutMode_ != mode) {
        layoutMode_ = mode;
        layoutPolicy_->Clean();
        layoutPolicy_ = layoutPolicys_[mode];
        layoutPolicy_->Launch();
    } else {
        WLOGFI("curret layout mode is allready: %{public}d", static_cast<uint32_t>(mode));
    }
    if (reorder) {
        layoutPolicy_->Reorder();
    }
    return WMError::WM_OK;
}

void WindowNodeContainer::RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const
{
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        return;
    }
    auto iter = std::find_if(aboveAppWindowNode_->children_.begin(), aboveAppWindowNode_->children_.end(),
                             [](sptr<WindowNode> node) {
        return node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD;
    });
    if (iter != aboveAppWindowNode_->children_.end()) {
        WLOGFI("raise input method float window priority.");
        node->priority_ = WINDOW_TYPE_RAISED_INPUT_METHOD;
    }
}
}
}
