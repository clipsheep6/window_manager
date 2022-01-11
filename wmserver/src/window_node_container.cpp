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
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowNodeContainer"};
}

WindowNodeContainer::WindowNodeContainer(uint64_t screenId, uint32_t width, uint32_t height,
    UpdateFocusStatusFunc focusCallback, UpdateSystemBarPropsFunc sysBarCallback) : 
        screenId_(screenId),
        focusStatusCallBack_(focusCallback),
        systemBarChangedCallBack_(sysBarCallback)
{
    struct RSDisplayNodeConfig config = {screenId};
    displayNode_ = RSDisplayNode::Create(config);
    displayRect_ = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    layoutPolicy_->UpdateDisplayInfo(displayRect_);
}

WindowNodeContainer::~WindowNodeContainer()
{
    Destroy();
}

WMError WindowNodeContainer::MinimizeOtherFullScreenAbility()
{
    if (appWindowNode_->children_.empty()) {
        return WMError::WM_OK;
    }
    for (auto iter = appWindowNode_->children_.begin(); iter < appWindowNode_->children_.end() - 1; ++iter) {
        if ((*iter)->GetWindowMode() != WindowMode::WINDOW_MODE_FULLSCREEN) {
            continue;
        }
        if  ((*iter)->abilityToken_ != nullptr) {
            WLOGFI("Find previous fullscreen window, notify ability to minimize");
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility((*iter)->abilityToken_);
        }
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (!node->surfaceNode_ || !displayNode_) {
        WLOGFE("surface node or display node is nullptr!");
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

    UpdateWindowTree(node);
    UpdateRSTree(node, true);
    AssignZOrder();
    layoutPolicy_->AddWindowNode(node);
    UpdateFocusWindow();
    NotifySystemBarIfChanged();
    WLOGFI("AddWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowNode(sptr<WindowNode>& node)
{
    if (!node->surfaceNode_ || !displayNode_) {
        WLOGFE("surface node or display node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    layoutPolicy_->UpdateWindowNode(node);
    NotifySystemBarIfChanged();
    WLOGFI("UpdateWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateWindowTree(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    node->priority_ = zorderPolicy_->GetWindowPriority(node->GetWindowType());
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
    if (displayNode_ == nullptr) {
        WLOGFE("displayNode_ is nullptr");
        return false;
    }
    if (isAdd) {
        displayNode_->AddChild(node->surfaceNode_, -1);
        for (auto& child : node->children_) {
            if (child->currentVisibility_) {
                displayNode_->AddChild(child->surfaceNode_, -1);
            }
        }
    } else {
        displayNode_->RemoveChild(node->surfaceNode_);
        for (auto& child : node->children_) {
            displayNode_->RemoveChild(child->surfaceNode_);
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
        if (child->surfaceNode_ != nullptr && displayNode_ != nullptr) {
            displayNode_->RemoveChild(child->surfaceNode_);
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
    for (auto& child : node->children_) {
        child->currentVisibility_ = false;
    }
    UpdateRSTree(node, false);
    UpdateFocusWindow();
    layoutPolicy_->RemoveWindowNode(node);
    NotifySystemBarIfChanged();
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
    displayNode_ = nullptr;
    return removedIds_;
}

sptr<WindowNode> WindowNodeContainer::FindRoot(WindowType type) const
{
    if (WindowHelper::IsAppWindow(type)) {
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

void WindowNodeContainer::UpdateFocusWindow()
{
    for (auto iter = appWindowNode_->children_.rbegin(); iter < appWindowNode_->children_.rend(); iter++) {
        if ((*iter)->GetWindowProperty()->GetFocusable()) {
            WLOGFI("find focused id %{public}d;", (*iter)->GetWindowId());
            SetFocusWindow((*iter)->GetWindowId());
            break;
        }
    }
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
        focusStatusCallBack_(node->GetWindowId(), node->abilityToken_, node->GetWindowType(),
            node->GetDisplayId(), focused);
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
        return WMError::WM_OK;
    }
    UpdateFocusStatus(focusedWindow_, false);
    focusedWindow_ = windowId;
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

void WindowNodeContainer::NotifySystemBarIfChanged()
{
    DumpScreenWindowTree();
    auto node = GetTopImmersiveNode();
    SystemBarProps props;
    if (node == nullptr) { // use default system bar
        for (auto it : sysBarPropMap_) {
            if (it.second == SystemBarProperty()) {
                continue;
            }
            sysBarPropMap_[it.first] = SystemBarProperty();
            props.push_back({it.first, SystemBarProperty()});
        }
    } else { // use node-defined system bar
        auto& sysBarPropMap = node->GetSystemBarProperty();
        for (auto it : sysBarPropMap_) {
            if (sysBarPropMap.find(it.first) == sysBarPropMap.end()) {
                return;
            }
            auto& prop = sysBarPropMap.find(it.first)->second;
            if (it.second == prop) {
                continue;
            }
            WLOGFI("Set systemBar prop winId: %{public}d, type: %{public}d" \
                "visible: %{public}d, Color: %{public}x | %{public}x",
                node->GetWindowId(), static_cast<int32_t>(it.first),
                prop.enable_, prop.backgroundColor_, prop.contentColor_);
            sysBarPropMap_[it.first] = prop;
            props.push_back({it.first, prop});
        }
    }
    systemBarChangedCallBack_(static_cast<int32_t>(screenId_), props);
}

void WindowNodeContainer::TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes)
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

void WindowNodeContainer::TraverseWindowNode(sptr<WindowNode>& node, std::vector<sptr<WindowNode>>& windowNodes)
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

void WindowNodeContainer::DumpScreenWindowTree()
{
    WLOGFI("-------- Screen %{public}llu dump window info begin---------", screenId_);
    WLOGFI("WinId Type Mode Flag ZOrd [   x    y    w    h]");
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    int zOrder = windowNodes.size();
    for (auto node : windowNodes) {
        Rect rect = node->GetLayoutRect();
        WLOGFI("%{public}5d %{public}4d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
            "%{public}4d %{public}4d]", node->GetWindowId(), node->GetWindowType(), node->GetWindowMode(),
            node->GetWindowFlags(), --zOrder, rect.posX_, rect.posY_, rect.width_, rect.height_);
    }
    WLOGFI("-------- Screen %{public}llu dump window info end  ---------", screenId_);
}

uint64_t WindowNodeContainer::GetScreenId() const
{
    return screenId_;
}

Rect WindowNodeContainer::GetDisplayRect() const
{
    return displayRect_;
}
}
}
