/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <ability_manager_client.h>
#include <algorithm>
#include <cinttypes>
#include <ctime>
#include <display_power_mgr_client.h>
#include <power_mgr_client.h>

#include "common_event_manager.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_layout_policy_cascade.h"
#include "window_layout_policy_tile.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeContainer"};
    constexpr int WINDOW_NAME_MAX_LENGTH = 10;
    const std::string SPLIT_SCREEN_EVENT_NAME = "common.event.SPLIT_SCREEN";
    const char DISABLE_WINDOW_ANIMATION_PATH[] = "/etc/disable_window_animation";
    constexpr uint32_t MAX_BRIGHTNESS = 255;
}

WindowNodeContainer::WindowNodeContainer(DisplayId displayId, uint32_t width, uint32_t height, bool isMinimizedByOther)
    : displayId_(displayId), isMinimizedByOther_(isMinimizedByOther)
{
    displayRect_ = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    windowPair_ = new WindowPair(displayId_, appWindowNode_);
    layoutPolicys_[WindowLayoutMode::CASCADE] =
        new WindowLayoutPolicyCascade(displayRect_, displayId_,
            belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    layoutPolicys_[WindowLayoutMode::TILE] =
        new WindowLayoutPolicyTile(displayRect_, displayId_,
            belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    layoutPolicy_ = layoutPolicys_[WindowLayoutMode::CASCADE];
    layoutPolicy_->Launch();
    UpdateAvoidAreaFunc func = std::bind(&WindowNodeContainer::OnAvoidAreaChange, this, std::placeholders::_1);
    avoidController_ = new AvoidAreaController(displayId, func);
}

WindowNodeContainer::~WindowNodeContainer()
{
    Destroy();
}

void WindowNodeContainer::UpdateDisplayRect(uint32_t width, uint32_t height)
{
    WLOGFI("update display rect, w/h=%{public}u/%{public}u", width, height);
    displayRect_ = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    layoutPolicy_->LayoutWindowTree();
}

WMError WindowNodeContainer::MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node)
{
    std::vector<uint32_t> exceptionalIds = { node->GetWindowId() };
    std::vector<WindowMode> exceptionalModes = { WindowMode::WINDOW_MODE_FLOATING, WindowMode::WINDOW_MODE_PIP };
    return MinimizeAppNodeExceptOptions(false, exceptionalIds, exceptionalModes);
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
        if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
            sysBarNodeMap_[node->GetWindowType()] = node;
        }
    }
    node->parent_ = parentNode;
    windowPair_->UpdateIfSplitRelated(node);
    UpdateWindowTree(node);
    if (node->IsSplitMode() || node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        RaiseSplitRelatedWindowToTop(node);
    }
    UpdateRSTree(node, true);
    AssignZOrder();
    layoutPolicy_->AddWindowNode(node);
    if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        avoidController_->AvoidControl(node, AvoidControlType::AVOID_NODE_ADD);
        NotifyIfSystemBarRegionChanged();
    } else {
        NotifyIfSystemBarTintChanged();
    }
    std::vector<sptr<WindowVisibilityInfo>> infos;
    UpdateWindowVisibilityInfos(infos);
    DumpScreenWindowTree();
    NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_ADDED);
    WLOGFI("AddWindowNode windowId: %{public}u end", node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowNode(sptr<WindowNode>& node, WindowUpdateReason reason)
{
    if (!node->surfaceNode_) {
        WLOGFE("surface node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (WindowHelper::IsMainWindow(node->GetWindowType()) && WindowHelper::IsSwitchCascadeReason(reason)) {
        SwitchLayoutPolicy(WindowLayoutMode::CASCADE);
    }
    layoutPolicy_->UpdateWindowNode(node);
    if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        avoidController_->AvoidControl(node, AvoidControlType::AVOID_NODE_UPDATE);
        NotifyIfSystemBarRegionChanged();
    } else {
        NotifyIfSystemBarTintChanged();
    }
    DumpScreenWindowTree();
    WLOGFI("UpdateWindowNode windowId: %{public}u end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateSizeChangeReason(sptr<WindowNode>& node, WindowSizeChangeReason reason)
{
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        for (auto& childNode : appWindowNode_->children_) {
            if (childNode->IsSplitMode()) {
                childNode->GetWindowToken()->UpdateWindowRect(childNode->GetWindowRect(),
                    childNode->GetDecoStatus(), reason);
                childNode->ResetWindowSizeChangeReason();
                WLOGFI("Notify split window that the drag action is start or end, windowId: %{public}d, "
                    "reason: %{public}u", childNode->GetWindowId(), reason);
            }
        }
    } else {
        node->GetWindowToken()->UpdateWindowRect(node->GetWindowRect(), node->GetDecoStatus(), reason);
        node->ResetWindowSizeChangeReason();
        WLOGFI("Notify window that the drag action is start or end, windowId: %{public}d, "
            "reason: %{public}u", node->GetWindowId(), reason);
    }
}

void WindowNodeContainer::UpdateWindowTree(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    node->priority_ = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    RaiseInputMethodWindowPriorityIfNeeded(node);
    RaiseShowWhenLockedWindowIfNeeded(node);
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
    static const bool IsWindowAnimationEnabled = ReadIsWindowAnimationEnabledProperty();

    auto updateRSTreeFunc = [&]() {
        auto& dms = DisplayManagerServiceInner::GetInstance();
        if (isAdd) {
            dms.UpdateRSTree(displayId_, node->surfaceNode_, true);
            for (auto& child : node->children_) {
                if (child->currentVisibility_) {
                    dms.UpdateRSTree(displayId_, child->surfaceNode_, true);
                }
            }
        } else {
            dms.UpdateRSTree(displayId_, node->surfaceNode_, false);
            for (auto& child : node->children_) {
                dms.UpdateRSTree(displayId_, child->surfaceNode_, false);
            }
        }
    };

    if (IsWindowAnimationEnabled) {
        // default transition duration: 350ms
        static const RSAnimationTimingProtocol timingProtocol(350);
        // default transition curve: EASE OUT
        static const Rosen::RSAnimationTimingCurve curve = Rosen::RSAnimationTimingCurve::EASE_OUT;
        // add or remove window with transition animation
        RSNode::Animate(timingProtocol, curve, updateRSTreeFunc);
    } else {
        // add or remove window without animation
        updateRSTreeFunc();
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
    if (node == nullptr) {
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
    node->isCovered_ = true;
    std::vector<sptr<WindowVisibilityInfo>> infos = {new WindowVisibilityInfo(node->GetWindowId(),
        node->GetCallingPid(), node->GetCallingUid(), false)};
    for (auto& child : node->children_) {
        if (child->currentVisibility_) {
            child->currentVisibility_ = false;
            child->isCovered_ = true;
            infos.emplace_back(new WindowVisibilityInfo(child->GetWindowId(), child->GetCallingPid(),
                child->GetCallingUid(), false));
        }
    }
    UpdateRSTree(node, false);
    layoutPolicy_->RemoveWindowNode(node);
    windowPair_->HandleRemoveWindow(node);
    if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        avoidController_->AvoidControl(node, AvoidControlType::AVOID_NODE_REMOVE);
        NotifyIfSystemBarRegionChanged();
    } else {
        NotifyIfSystemBarTintChanged();
    }
    UpdateWindowVisibilityInfos(infos);
    DumpScreenWindowTree();
    NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_REMOVED);
    RcoveryScreenDefaultOrientationIfNeed();
    WLOGFI("RemoveWindowNode windowId: %{public}u end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::RcoveryScreenDefaultOrientationIfNeed()
{
    if (appWindowNode_->children_.empty()) {
        WLOGFI("appWindowNode_ child is empty in display  %{public}" PRIu64"", displayId_);
        DisplayManagerServiceInner::GetInstance().
            SetOrientationFromWindow(displayId_, Orientation::UNSPECIFIED);
    }
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
        if (focused) {
            WLOGFW("current focus window: windowId: %{public}d, windowName: %{public}s",
                id, node->GetWindowProperty()->GetWindowName().c_str());
        }
        sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo(node->GetWindowId(), node->GetDisplayId(),
            node->GetCallingPid(), node->GetCallingUid(), node->GetWindowType(), node->abilityToken_);
        WindowManagerAgentController::GetInstance().UpdateFocusChangeInfo(
            focusChangeInfo, focused);
    }
}

void WindowNodeContainer::UpdateActiveStatus(uint32_t id, bool isActive) const
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFE("cannot find active window id: %{public}d", id);
        return;
    }
    node->GetWindowToken()->UpdateActiveStatus(isActive);
}

void WindowNodeContainer::UpdateBrightness(uint32_t id, bool byRemoved)
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFE("cannot find active window id: %{public}d", id);
        return;
    }

    if (!byRemoved) {
        if (!WindowHelper::IsAppWindow(node->GetWindowType())) {
            return;
        }
    }
    WLOGFI("brightness: [%{public}f, %{public}f]", GetDisplayBrightness(), node->GetBrightness());
    if (node->GetBrightness() == UNDEFINED_BRIGHTNESS) {
        if (GetDisplayBrightness() != node->GetBrightness()) {
            WLOGFI("adjust brightness with default value");
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        }
        SetBrightnessWindow(INVALID_WINDOW_ID);
    } else {
        if (GetDisplayBrightness() != node->GetBrightness()) {
            WLOGFI("adjust brightness with value: %{public}u", ToOverrideBrightness(node->GetBrightness()));
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                ToOverrideBrightness(node->GetBrightness()));
            SetDisplayBrightness(node->GetBrightness());
        }
        SetBrightnessWindow(node->GetWindowId());
    }
}

void WindowNodeContainer::AssignZOrder()
{
    zOrder_ = 0;
    WindowNodeOperationFunc func = [this](sptr<WindowNode> node) {
        if (node->surfaceNode_ == nullptr) {
            WLOGE("AssignZOrder: surfaceNode is nullptr, window Id:%{public}u", node->GetWindowId());
            return false;
        }
        node->surfaceNode_->SetPositionZ(zOrder_);
        ++zOrder_;
        return false;
    };
    TraverseWindowTree(func, false);
}

WMError WindowNodeContainer::SetFocusWindow(uint32_t windowId)
{
    if (focusedWindow_ == windowId) {
        WLOGFI("focused window do not change, id: %{public}u", windowId);
        return WMError::WM_DO_NOTHING;
    }
    UpdateFocusStatus(focusedWindow_, false);
    focusedWindow_ = windowId;
    sptr<WindowNode> node = FindWindowNodeById(windowId);
    NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    UpdateFocusStatus(focusedWindow_, true);
    return WMError::WM_OK;
}

uint32_t WindowNodeContainer::GetFocusWindow() const
{
    return focusedWindow_;
}

WMError WindowNodeContainer::SetActiveWindow(uint32_t windowId, bool byRemoved)
{
    if (activeWindow_ == windowId) {
        WLOGFI("active window do not change, id: %{public}u", windowId);
        return WMError::WM_DO_NOTHING;
    }
    UpdateActiveStatus(activeWindow_, false);
    activeWindow_ = windowId;
    UpdateActiveStatus(activeWindow_, true);
    UpdateBrightness(activeWindow_, byRemoved);
    return WMError::WM_OK;
}

void WindowNodeContainer::SetDisplayBrightness(float brightness)
{
    displayBrightness_ = brightness;
}

float WindowNodeContainer::GetDisplayBrightness() const
{
    return displayBrightness_;
}

void WindowNodeContainer::SetBrightnessWindow(uint32_t windowId)
{
    brightnessWindow_ = windowId;
}

uint32_t WindowNodeContainer::GetBrightnessWindow() const
{
    return brightnessWindow_;
}

uint32_t WindowNodeContainer::ToOverrideBrightness(float brightness)
{
    return static_cast<uint32_t>(brightness * MAX_BRIGHTNESS);
}

uint32_t WindowNodeContainer::GetActiveWindow() const
{
    return activeWindow_;
}

void WindowNodeContainer::HandleKeepScreenOn(const sptr<WindowNode>& node, bool requireLock)
{
    if (requireLock && node->keepScreenLock_ == nullptr) {
        // reset ipc identity
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        node->keepScreenLock_ = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(node->GetWindowName(),
            PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
    }
    if (node->keepScreenLock_ == nullptr) {
        return;
    }
    WLOGFI("handle keep screen on: [%{public}s, %{public}d]", node->GetWindowName().c_str(), requireLock);
    ErrCode res;
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (requireLock) {
        res = node->keepScreenLock_->Lock();
    } else {
        res = node->keepScreenLock_->UnLock();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    if (res != ERR_OK) {
        WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]", requireLock, res);
    }
}

bool WindowNodeContainer::IsAboveSystemBarNode(sptr<WindowNode> node) const
{
    int32_t curPriority = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    if ((curPriority > zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_STATUS_BAR)) &&
        (curPriority > zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_NAVIGATION_BAR))) {
        return true;
    }
    return false;
}

bool WindowNodeContainer::IsFullImmersiveNode(sptr<WindowNode> node) const
{
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    return mode == WindowMode::WINDOW_MODE_FULLSCREEN &&
        !(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
}

bool WindowNodeContainer::IsSplitImmersiveNode(sptr<WindowNode> node) const
{
    auto type = node->GetWindowType();
    return node->IsSplitMode() || type == WindowType::WINDOW_TYPE_DOCK_SLICE;
}

std::unordered_map<WindowType, SystemBarProperty> WindowNodeContainer::GetExpectImmersiveProperty() const
{
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };

    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (auto& node : rootNodes) {
        for (auto iter = node->children_.rbegin(); iter < node->children_.rend(); ++iter) {
            auto& sysBarPropMapNode = (*iter)->GetSystemBarProperty();
            if (IsAboveSystemBarNode(*iter)) {
                continue;
            }
            if (IsFullImmersiveNode(*iter)) {
                WLOGFI("Top immersive window id: %{public}d. Use full immersive prop", (*iter)->GetWindowId());
                for (auto it : sysBarPropMap) {
                    sysBarPropMap[it.first] = (sysBarPropMapNode.find(it.first))->second;
                }
                return sysBarPropMap;
            } else if (IsSplitImmersiveNode(*iter)) {
                WLOGFI("Top split window id: %{public}d. Use split immersive prop", (*iter)->GetWindowId());
                for (auto it : sysBarPropMap) {
                    sysBarPropMap[it.first] = (sysBarPropMapNode.find(it.first))->second;
                    sysBarPropMap[it.first].enable_ = false;
                }
                return sysBarPropMap;
            }
        }
    }

    WLOGFI("No immersive window on top. Use default systembar Property");
    return sysBarPropMap;
}

void WindowNodeContainer::NotifyIfSystemBarTintChanged()
{
    WM_FUNCTION_TRACE();
    auto expectSystemBarProp = GetExpectImmersiveProperty();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) {
        auto expectProp = expectSystemBarProp.find(it.first)->second;
        if (it.second.prop_ == expectProp) {
            continue;
        }
        WLOGFI("System bar prop update, Type: %{public}d, Visible: %{public}d, Color: %{public}x | %{public}x",
            static_cast<int32_t>(it.first), expectProp.enable_, expectProp.backgroundColor_, expectProp.contentColor_);
        sysBarTintMap_[it.first].prop_ = expectProp;
        sysBarTintMap_[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap_[it.first]);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifyIfSystemBarRegionChanged()
{
    WM_FUNCTION_TRACE();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) { // split screen mode not support yet
        auto sysNode = sysBarNodeMap_[it.first];
        if (sysNode == nullptr || it.second.region_ == sysNode->GetWindowRect()) {
            continue;
        }
        const auto& newRegion = sysNode->GetWindowRect();
        sysBarTintMap_[it.first].region_ = newRegion;
        sysBarTintMap_[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap_[it.first]);
        WLOGFI("system bar region update, type: %{public}d" \
            "region: [%{public}d, %{public}d, %{public}d, %{public}d]",
            static_cast<int32_t>(it.first), newRegion.posX_, newRegion.posY_, newRegion.width_, newRegion.height_);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifySystemBarDismiss(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    if (node == nullptr) {
        WLOGE("could not find window");
        return;
    }
    SystemBarRegionTints tints;
    auto& sysBarPropMapNode = node->GetSystemBarProperty();
    for (auto it : sysBarPropMapNode) {
        it.second.enable_ = false;
        node->SetSystemBarProperty(it.first, it.second);
        WLOGFI("set system bar enable to false, id: %{public}u, type: %{public}d",
            node->GetWindowId(), static_cast<int32_t>(it.first));
        if (sysBarTintMap_[it.first].prop_.enable_) {
            sysBarTintMap_[it.first].prop_.enable_ = false;
            tints.emplace_back(sysBarTintMap_[it.first]);
            WLOGFI("notify system bar dismiss, type: %{public}d", static_cast<int32_t>(it.first));
        }
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifySystemBarTints()
{
    WM_FUNCTION_TRACE();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) {
        WLOGFD("system bar cur notify, type: %{public}d, " \
            "visible: %{public}d, color: %{public}x | %{public}x, " \
            "region: [%{public}d, %{public}d, %{public}d, %{public}d]",
            static_cast<int32_t>(it.first),
            sysBarTintMap_[it.first].region_.posX_, sysBarTintMap_[it.first].region_.posY_,
            sysBarTintMap_[it.first].region_.width_, sysBarTintMap_[it.first].region_.height_,
            sysBarTintMap_[it.first].prop_.enable_,
            sysBarTintMap_[it.first].prop_.backgroundColor_, sysBarTintMap_[it.first].prop_.contentColor_);
        tints.push_back(sysBarTintMap_[it.first]);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

bool WindowNodeContainer::IsTopWindow(uint32_t windowId, sptr<WindowNode>& rootNode) const
{
    if (rootNode->children_.empty()) {
        WLOGFE("root does not have any node");
        return false;
    }
    auto node = *(rootNode->children_.rbegin());
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

void WindowNodeContainer::RaiseOrderedWindowToTop(std::vector<sptr<WindowNode>>& orderedNodes,
    std::vector<sptr<WindowNode>>& windowNodes)
{
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end();) {
        uint32_t wid = (*iter)->GetWindowId();
        auto orderedIter = std::find_if(orderedNodes.begin(), orderedNodes.end(),
            [wid] (sptr<WindowNode> orderedNode) { return orderedNode->GetWindowId() == wid; });
        if (orderedIter != orderedNodes.end()) {
            iter = windowNodes.erase(iter);
        } else {
            iter++;
        }
    }
    for (auto iter = orderedNodes.begin(); iter != orderedNodes.end(); iter++) {
        UpdateWindowTree(*iter);
    }
    return;
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
        WLOGFI("raise window to top %{public}u", node->GetWindowId());
    }
}

void WindowNodeContainer::NotifyAccessibilityWindowInfo(const sptr<WindowNode>& node, WindowUpdateType type) const
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
        std::vector<sptr<WindowInfo>> windowList;
        GetWindowList(windowList);
        sptr<WindowInfo> windowInfo = new (std::nothrow) WindowInfo();
        sptr<AccessibilityWindowInfo> accessibilityWindowInfo = new (std::nothrow) AccessibilityWindowInfo();
        if (windowInfo != nullptr && accessibilityWindowInfo != nullptr) {
            windowInfo->wid_ = static_cast<int32_t>(node->GetWindowId());
            windowInfo->windowRect_ = node->GetWindowRect();
            windowInfo->focused_ = node->GetWindowId() == focusedWindow_;
            windowInfo->displayId_ = node->GetDisplayId();
            windowInfo->mode_ = node->GetWindowMode();
            windowInfo->type_ = node->GetWindowType();
            accessibilityWindowInfo->currentWindowInfo_ = windowInfo;
            accessibilityWindowInfo->windowList_ = windowList;
            WindowManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(accessibilityWindowInfo, type);
        }
    }
}

void WindowNodeContainer::GetWindowList(std::vector<sptr<WindowInfo>>& windowList) const
{
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    for (auto node : windowNodes) {
        sptr<WindowInfo> windowInfo = new WindowInfo();
        windowInfo->wid_ = static_cast<int32_t>(node->GetWindowId());
        windowInfo->windowRect_ = node->GetWindowRect();
        windowInfo->focused_ = node->GetWindowId() == focusedWindow_;
        windowInfo->displayId_ = node->GetDisplayId();
        windowInfo->mode_ = node->GetWindowMode();
        windowInfo->type_ = node->GetWindowType();
        windowList.emplace_back(windowInfo);
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
    for (auto& node : appWindowNode_->children_) {
        if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN && node->GetWindowToken() != nullptr) {
            // notify client
            node->GetWindowToken()->UpdateAvoidArea(avoidArea);
        }
    }
}

void WindowNodeContainer::DumpScreenWindowTree()
{
    WLOGFI("-------- display %{public}" PRIu64" dump window info begin---------", displayId_);
    WLOGFI("WindowName WinId Type Mode Flag ZOrd Orientation [   x    y    w    h]");
    uint32_t zOrder = zOrder_;
    WindowNodeOperationFunc func = [&zOrder](sptr<WindowNode> node) {
        Rect rect = node->GetWindowRect();
        const std::string& windowName = node->GetWindowName().size() < WINDOW_NAME_MAX_LENGTH ?
            node->GetWindowName() : node->GetWindowName().substr(0, WINDOW_NAME_MAX_LENGTH);
        WLOGI("DumpScreenWindowTree: %{public}10s %{public}5u %{public}4u %{public}4u %{public}4u %{public}4u " \
            "%{public}11u [%{public}4d %{public}4d %{public}4u %{public}4u]",
            windowName.c_str(), node->GetWindowId(), node->GetWindowType(), node->GetWindowMode(),
            node->GetWindowFlags(), --zOrder, static_cast<uint32_t>(node->GetRequestedOrientation()),
            rect.posX_, rect.posY_, rect.width_, rect.height_);
        return false;
    };
    TraverseWindowTree(func, true);
    WLOGFI("-------- display %{public}" PRIu64" dump window info end  ---------", displayId_);
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
        if (node->priority_ < topPriority &&
            !(node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED))) {
            node->GetWindowToken()->UpdateWindowState(state);
            HandleKeepScreenOn(node, state);
        }
    }
    for (auto& childNode : node->children_) {
        UpdateWindowState(childNode, topPriority, state);
    }
}

void WindowNodeContainer::HandleKeepScreenOn(const sptr<WindowNode>& node, WindowState state)
{
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    if (state == WindowState::STATE_FROZEN) {
        HandleKeepScreenOn(node, false);
    } else if (state == WindowState::STATE_UNFROZEN) {
        HandleKeepScreenOn(node, node->IsKeepScreenOn());
    } else {
        // do nothing
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

void WindowNodeContainer::RaiseSplitRelatedWindowToTop(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    std::vector<sptr<WindowNode>> orderedPair = windowPair_->GetOrderedPair(node);
    RaiseOrderedWindowToTop(orderedPair, appWindowNode_->children_);
    AssignZOrder();
    return;
}

WMError WindowNodeContainer::RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (node == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }

    if (IsTopWindow(node->GetWindowId(), appWindowNode_) || IsTopWindow(node->GetWindowId(), aboveAppWindowNode_)) {
        WLOGFI("it is already top app window, id: %{public}u", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_TYPE;
    }

    if (WindowHelper::IsSubWindow(node->GetWindowType())) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_NULLPTR;
        }
        RaiseWindowToTop(node->GetWindowId(), parentNode->children_); // raise itself
        if (parentNode->IsSplitMode()) {
            RaiseSplitRelatedWindowToTop(parentNode);
        } else {
            RaiseWindowToTop(node->GetParentId(), parentNode->parent_->children_); // raise parent window
        }
    } else if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (node->IsSplitMode()) {
            RaiseSplitRelatedWindowToTop(node);
        } else {
            RaiseWindowToTop(node->GetWindowId(), node->parent_->children_);
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
    sptr<WindowNode> nextFocusableWindow;
    bool previousFocusedWindowFound = false;
    WindowNodeOperationFunc func = [windowId, &nextFocusableWindow, &previousFocusedWindowFound](
        sptr<WindowNode> node) {
        if (previousFocusedWindowFound && node->GetWindowProperty()->GetFocusable()) {
            nextFocusableWindow = node;
            return true;
        }
        if (node->GetWindowId() == windowId) {
            previousFocusedWindowFound = true;
        }
        return false;
    };
    TraverseWindowTree(func, true);
    return nextFocusableWindow;
}

sptr<WindowNode> WindowNodeContainer::GetNextActiveWindow(uint32_t windowId) const
{
    auto currentNode = FindWindowNodeById(windowId);
    if (currentNode == nullptr) {
        WLOGFE("cannot find window id: %{public}u by tree", windowId);
        return nullptr;
    }
    WLOGFI("current window: [%{public}u, %{public}u]", windowId, static_cast<uint32_t>(currentNode->GetWindowType()));
    if (WindowHelper::IsSystemWindow(currentNode->GetWindowType())) {
        for (auto& node : appWindowNode_->children_) {
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
                continue;
            }
            return node;
        }
        for (auto& node : belowAppWindowNode_->children_) {
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
                return node;
            }
        }
    } else if (WindowHelper::IsAppWindow(currentNode->GetWindowType())) {
        std::vector<sptr<WindowNode>> windowNodes;
        TraverseContainer(windowNodes);
        auto iter = std::find_if(windowNodes.begin(), windowNodes.end(), [windowId](sptr<WindowNode>& node) {
            return node->GetWindowId() == windowId;
            });
        if (iter == windowNodes.end()) {
            WLOGFE("could not find this window");
            return nullptr;
        }
        int index = std::distance(windowNodes.begin(), iter);
        for (size_t i = static_cast<size_t>(index) + 1; i < windowNodes.size(); i++) {
            if (windowNodes[i]->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
                continue;
            }
            return windowNodes[i];
        }
    } else {
        // do nothing
    }
    WLOGFE("could not get next active window");
    return nullptr;
}

void WindowNodeContainer::MinimizeAllAppWindows()
{
    WMError ret =  MinimizeAppNodeExceptOptions(true);
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE);
    if (ret != WMError::WM_OK) {
        WLOGFE("Minimize all app window failed");
    }
    return;
}

void WindowNodeContainer::MinimizeWindowFromAbility(const sptr<WindowNode>& node, bool fromUser)
{
    if (node->abilityToken_ == nullptr) {
        WLOGFW("Target abilityToken is nullptr, windowId:%{public}u", node->GetWindowId());
        return;
    }
    WLOGFI("minimize window fromUser:%{public}d, isMinimizedByOther:%{public}d", fromUser, isMinimizedByOther_);
    if (!fromUser && !isMinimizedByOther_) {
        return;
    }
    AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(node->abilityToken_, fromUser);
}

WMError WindowNodeContainer::MinimizeAppNodeExceptOptions(bool fromUser, const std::vector<uint32_t> &exceptionalIds,
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
        MinimizeWindowFromAbility(appNode, fromUser);
    }
    return WMError::WM_OK;
}

void WindowNodeContainer::ResetLayoutPolicy()
{
    layoutPolicy_->Reset();
}

WMError WindowNodeContainer::SwitchLayoutPolicy(WindowLayoutMode dstMode, bool reorder)
{
    WLOGFI("SwitchLayoutPolicy src: %{public}d dst: %{public}d reorder: %{public}d",
        static_cast<uint32_t>(layoutMode_), static_cast<uint32_t>(dstMode), static_cast<uint32_t>(reorder));
    if (dstMode < WindowLayoutMode::BASE || dstMode >= WindowLayoutMode::END) {
        WLOGFE("invalid layout mode");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (layoutMode_ != dstMode) {
        if (layoutMode_ == WindowLayoutMode::CASCADE) {
            layoutPolicy_->Reset();
            windowPair_->Clear();
        }
        layoutMode_ = dstMode;
        layoutPolicy_->Clean();
        layoutPolicy_ = layoutPolicys_[dstMode];
        layoutPolicy_->Launch();
        DumpScreenWindowTree();
    } else {
        WLOGFI("Current layout mode is already: %{public}d", static_cast<uint32_t>(dstMode));
    }
    if (reorder) {
        windowPair_->Clear();
        layoutPolicy_->Reorder();
        DumpScreenWindowTree();
    }
    NotifyIfSystemBarTintChanged();
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
        node->priority_ = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD) + 1;
    }
}

void WindowNodeContainer::ReZOrderShowWhenLockedWindows(const sptr<WindowNode>& node, bool up)
{
    WLOGFI("Keyguard change %{public}u, re-zorder showWhenLocked window", up);
    std::vector<sptr<WindowNode>> needReZOrderNodes;
    auto& srcRoot = up ? appWindowNode_ : aboveAppWindowNode_;
    auto& dstRoot = up ? aboveAppWindowNode_ : appWindowNode_;
    auto dstPriority = up ? zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD) + 1 :
        zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    for (auto iter = srcRoot->children_.begin(); iter != srcRoot->children_.end();) {
        if ((*iter)->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) {
            needReZOrderNodes.emplace_back(*iter);
            iter = srcRoot->children_.erase(iter);
        } else {
            iter++;
        }
    }

    for (auto& needReZOrderNode : needReZOrderNodes) {
        needReZOrderNode->priority_ = dstPriority;
        needReZOrderNode->parent_ = dstRoot;
        auto parentNode = needReZOrderNode->parent_;
        auto position = parentNode->children_.end();
        for (auto iter = parentNode->children_.begin(); iter < parentNode->children_.end(); ++iter) {
            if ((*iter)->priority_ > needReZOrderNode->priority_) {
                position = iter;
                break;
            }
        }
        parentNode->children_.insert(position, needReZOrderNode);
        WLOGFI("ShowWhenLocked window %{public}u re-zorder when keyguard change %{public}u",
            needReZOrderNode->GetWindowId(), up);
    }
}

void WindowNodeContainer::RaiseShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node)
{
    // if keyguard window show, raise show when locked windows
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        ReZOrderShowWhenLockedWindows(node, true);
        return;
    }

    // if show when locked window show, raise itself when exist keyguard
    if (!(node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED))) {
        return;
    }

    auto iter = std::find_if(aboveAppWindowNode_->children_.begin(), aboveAppWindowNode_->children_.end(),
                             [](sptr<WindowNode> node) {
        return node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD;
    });
    if (iter != aboveAppWindowNode_->children_.end()) {
        WLOGFI("ShowWhenLocked window %{public}u raise itself", node->GetWindowId());
        node->priority_ = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD) + 1;
        node->parent_ = aboveAppWindowNode_;
    }
}

void WindowNodeContainer::DropShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node)
{
    // if keyguard window hide, drop show when locked windows
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        ReZOrderShowWhenLockedWindows(node, false);
        AssignZOrder();
    }
}

void WindowNodeContainer::MoveWindowNode(sptr<WindowNodeContainer>& container)
{
    DisplayId from = container->GetDisplayId();
    WLOGFI("disconnect expand display: %{public}" PRId64 ", move window node to display: "
        "%{public}" PRId64 "", from, displayId_);
    for (auto& node : container->aboveAppWindowNode_->children_) {
        WLOGFI("aboveAppWindowNode_: move windowNode: %{public}u", node->GetWindowId());
        aboveAppWindowNode_->children_.push_back(node);
        layoutPolicy_->AddWindowNode(node);
    }
    for (auto& node : container->appWindowNode_->children_) {
        WLOGFI("appWindowNode_: move windowNode: %{public}u", node->GetWindowId());
        appWindowNode_->children_.push_back(node);
        layoutPolicy_->AddWindowNode(node);
    }
    for (auto& node : container->belowAppWindowNode_->children_) {
        WLOGFI("belowAppWindowNode_: move windowNode: %{public}u", node->GetWindowId());
        belowAppWindowNode_->children_.push_back(node);
        layoutPolicy_->AddWindowNode(node);
    }
}

void WindowNodeContainer::TraverseWindowTree(const WindowNodeOperationFunc& func, bool isFromTopToBottom) const
{
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    if (isFromTopToBottom) {
        std::reverse(rootNodes.begin(), rootNodes.end());
    }

    for (auto& node : rootNodes) {
        if (isFromTopToBottom) {
            for (auto iter = node->children_.rbegin(); iter != node->children_.rend(); ++iter) {
                if (TraverseFromTopToBottom(*iter, func)) {
                    return;
                }
            }
        } else {
            for (auto iter = node->children_.begin(); iter != node->children_.end(); ++iter) {
                if (TraverseFromBottomToTop(*iter, func)) {
                    return;
                }
            }
        }
    }
}

bool WindowNodeContainer::TraverseFromTopToBottom(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const
{
    if (node == nullptr) {
        return false;
    }
    auto iterBegin = node->children_.rbegin();
    for (; iterBegin != node->children_.rend(); ++iterBegin) {
        if ((*iterBegin)->priority_ <= 0) {
            break;
        }
        if (func(*iterBegin)) {
            return true;
        }
    }
    if (func(node)) {
        return true;
    }
    for (; iterBegin != node->children_.rend(); ++iterBegin) {
        if (func(*iterBegin)) {
            return true;
        }
    }
    return false;
}

bool WindowNodeContainer::TraverseFromBottomToTop(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const
{
    if (node == nullptr) {
        return false;
    }
    auto iterBegin = node->children_.begin();
    for (; iterBegin != node->children_.end(); ++iterBegin) {
        if ((*iterBegin)->priority_ >= 0) {
            break;
        }
        if (func(*iterBegin)) {
            return true;
        }
    }
    if (func(node)) {
        return true;
    }
    for (; iterBegin != node->children_.end(); ++iterBegin) {
        if (func(*iterBegin)) {
            return true;
        }
    }
    return false;
}

void WindowNodeContainer::UpdateWindowVisibilityInfos(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    currentCoveredArea_.clear();
    WindowNodeOperationFunc func = [this, &infos](sptr<WindowNode> node) {
        if (node == nullptr) {
            return false;
        }
        Rect layoutRect = node->GetWindowRect();
        int32_t nodeX = std::max(0, layoutRect.posX_);
        int32_t nodeY = std::max(0, layoutRect.posY_);
        int32_t nodeXEnd = std::min(displayRect_.posX_ + static_cast<int32_t>(displayRect_.width_),
            layoutRect.posX_ + static_cast<int32_t>(layoutRect.width_));
        int32_t nodeYEnd = std::min(displayRect_.posY_ + static_cast<int32_t>(displayRect_.height_),
            layoutRect.posY_ + static_cast<int32_t>(layoutRect.height_));

        Rect rectInDisplay = {nodeX, nodeY,
                              static_cast<uint32_t>(nodeXEnd - nodeX), static_cast<uint32_t>(nodeYEnd - nodeY)};
        bool isCovered = false;
        for (auto& rect : currentCoveredArea_) {
            if (rectInDisplay.IsInsideOf(rect)) {
                isCovered = true;
                WLOGD("UpdateWindowVisibilityInfos: find covered window:%{public}u", node->GetWindowId());
                break;
            }
        }
        if (!isCovered) {
            currentCoveredArea_.emplace_back(rectInDisplay);
        }
        if (isCovered != node->isCovered_) {
            node->isCovered_ = isCovered;
            infos.emplace_back(new WindowVisibilityInfo(node->GetWindowId(), node->GetCallingPid(),
                node->GetCallingUid(), !isCovered));
            WLOGD("UpdateWindowVisibilityInfos: covered status changed window:%{public}u, covered:%{public}d",
                node->GetWindowId(), isCovered);
        }
        return false;
    };
    TraverseWindowTree(func, true);
    WindowManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(infos);
}

float WindowNodeContainer::GetVirtualPixelRatio() const
{
    return layoutPolicy_->GetVirtualPixelRatio();
}

bool WindowNodeContainer::ReadIsWindowAnimationEnabledProperty()
{
    if (access(DISABLE_WINDOW_ANIMATION_PATH, F_OK) == 0) {
        return false;
    }
    return true;
}

WMError WindowNodeContainer::SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode)
{
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WindowMode srcMode = node->GetWindowMode();
    if (srcMode == dstMode) {
        return WMError::WM_OK;
    }
    WMError res = WMError::WM_OK;
    if ((srcMode == WindowMode::WINDOW_MODE_FULLSCREEN) && (dstMode == WindowMode::WINDOW_MODE_FLOATING)) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RECOVER);
    } else if (dstMode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::MAXIMIZE);
    } else {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RESIZE);
    }
    node->SetWindowMode(dstMode);
    windowPair_->UpdateIfSplitRelated(node);
    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        // minimize other app window
        res = MinimizeStructuredAppWindowsExceptSelf(node);
        if (res != WMError::WM_OK) {
            return res;
        }
    }
    if (node->GetWindowToken() != nullptr) {
        node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
    }
    res = UpdateWindowNode(node, WindowUpdateReason::UPDATE_MODE);
    if (res != WMError::WM_OK) {
        WLOGFE("Set window mode failed, update node failed");
        return res;
    }
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS
