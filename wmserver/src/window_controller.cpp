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

#include "window_controller.h"
#include <ability_manager_client.h>
#include <parameters.h>
#include <power_mgr_client.h>
#include <rs_window_animation_target.h>
#include <transaction/rs_transaction.h>
#include "rs_window_animation_finished_callback.h"
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "wm_common.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowController"};
    static SkBitmap g_bitmap;

}

uint32_t WindowController::GenWindowId()
{
    return ++windowId_;
}

void WindowController::CreateDesWindowNodeAndShow(sptr<WindowNode>& desNode, sptr<WindowTransitionInfo>& toInfo)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetRequestRect(toInfo->GetWindowRect());
    property->SetWindowMode(toInfo->GetWindowMode());
    property->SetDisplayId(toInfo->GetDisplayId());
    property->SetWindowType(toInfo->GetWindowType());
    if (toInfo->GetShowFlagWhenLocked()) {
        property->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    auto winId = GenWindowId();
    property->SetWindowId(winId);
    sptr<WindowNode> node = new(std::nothrow) WindowNode(property);
    if (node == nullptr) {
        WLOGFE("malloc window node failed.");
        return;
    }
    node->abilityToken_ = toInfo->GetAbilityToken();
    WMError res = windowRoot_->SaveDesWindowNode(node);
    if (res != WMError::WM_OK) {
        WLOGFE("SaveDesWindowNode failed with ret %{public}d.", res);
        return;
    }
    res = windowRoot_->ShowInTransition(node);
    if (res != WMError::WM_OK) {
        WLOGFE("ShowInTransition failed with ret %{public}d.", res);
        return;
    }
    // draw starting Window
    Rect rect = node->GetWindowRect();
    const char *staringImgPath_ = "/etc/window/resources/starting_window.png";
    if (!surfaceDraw_.DecodeImageFile(staringImgPath_, g_bitmap)) {
        WLOGFE("load starting image failed");
        return;
    }
    surfaceDraw_.DrawBitmap(node->startingWinSurfaceNode_, rect, g_bitmap, 0xffffffff);
    // surfaceDraw_.DrawSurface(node->startingWinSurfaceNode_, rect);
    desNode = node;
}

static bool NeedMinimize(sptr<WindowNode> fromNode, sptr<WindowNode> toNode)
{
    if (fromNode == nullptr || toNode == nullptr) {
        return false;
    }
    if (fromNode->GetWindowId() == toNode->GetWindowId()) {
        return false;
    }
    return (toNode->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) &&
        (WindowHelper::IsAppWindow(toNode->GetWindowType())) && (fromNode->GetWindowMode() ==
        WindowMode::WINDOW_MODE_FULLSCREEN) && (WindowHelper::IsMainWindow(fromNode->GetWindowType()));
}

void WindowController::NotifyAnimationTransition(sptr<WindowTransitionInfo>& fromInfo,
    sptr<WindowTransitionInfo>& toInfo, sptr<WindowNode> fromNode, sptr<WindowNode> toNode)
{

    sptr<RSWindowAnimationTarget> fromTarget = new RSWindowAnimationTarget();
    fromTarget->bundleName_ = fromInfo->GetBundleName();
    fromTarget->abilityName_ = fromInfo->GetAbilityName();
    if (fromNode != nullptr) {
        if (WindowHelper::IsAppWindow(fromNode->GetWindowType())) {
            fromTarget->surfaceNode_ = fromNode->leashWinSurfaceNode_;
        } else {
            fromTarget->surfaceNode_ = fromNode->surfaceNode_;
        }
    }

    sptr<RSWindowAnimationTarget> toTarget = new RSWindowAnimationTarget();
    toTarget->bundleName_ = toInfo->GetBundleName();
    toTarget->abilityName_ = toInfo->GetAbilityName();
    if (WindowHelper::IsAppWindow(toNode->GetWindowType())) {
        toTarget->surfaceNode_ = toNode->leashWinSurfaceNode_;
    } else {
        toTarget->surfaceNode_ = toNode->surfaceNode_;
    }
    if (NeedMinimize(fromNode, toNode)) { // from App To App
        fromTarget->actionType_ = WindowAnimationActionType::GO_BACKGROUND;
        toTarget->actionType_ = WindowAnimationActionType::GO_FOREGROUND;
        fromNode->isPlayAnimationHide_ = true;
        toNode->isPlayAnimationShow_ = true;
    } else {
        // from launcher/recent/aa to app
        fromTarget->actionType_ = WindowAnimationActionType::NO_CHANGE;
        toTarget->actionType_ = WindowAnimationActionType::GO_FOREGROUND;
        toNode->isPlayAnimationShow_ = true;
    }
    auto callback = [fromNode, toNode]() {
        if (NeedMinimize(fromNode, toNode)) {
            WM_SCOPED_TRACE_BEGIN("controller:Minimize  fromNode in transition");
            if (fromNode->abilityToken_ == nullptr) {
                WLOGFE("Target abilityToken is nullptr, windowId:%{public}u", fromNode->GetWindowId());
                return;
            }
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(fromNode->abilityToken_, false);
        }
    };
    // TODO: add time out check
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        callback);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return;
    }
    // windowAnimationController_->OnTransition(fromTarget, toTarget, finishedCallback);
}

void WindowController::NotifyWindowTransition(
    sptr<WindowTransitionInfo>& fromInfo, sptr<WindowTransitionInfo>& toInfo)
{
    // if (windowAnimationController_ == nullptr) {
    //     WLOGFI("not use transition animation because windowAnimationController_ not set.");
    //     return;
    // }
    auto desNode = windowRoot_->FindWindowNodeWithToken(toInfo->GetAbilityToken());
    if (desNode == nullptr) {
        if (toInfo->GetAbilityToken() == nullptr) { // back event
            WLOGFI("target window abilityToken is null");
        } else {
            CreateDesWindowNodeAndShow(desNode, toInfo);
        }
    } else {
        WMError res = windowRoot_->ShowInTransition(desNode);
        if (res != WMError::WM_OK) {
            WLOGFE("ShowInTransition failed with ret %{public}d.", res);
            return;
        }
    }
    if (desNode != nullptr) {
        WLOGFI("Id:%{public}u, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
            desNode->GetWindowId(), desNode->GetWindowRect().posX_, desNode->GetWindowRect().posY_,
            desNode->GetWindowRect().width_, desNode->GetWindowRect().height_);
        WLOGFI("Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
            desNode->GetWindowId(), desNode->GetRequestRect().posX_, desNode->GetRequestRect().posY_,
            desNode->GetRequestRect().width_, desNode->GetRequestRect().height_);
        StopBootAnimationIfNeed(desNode->GetWindowType());
        FlushWindowInfo(desNode->GetWindowId());
    }
    auto fromNode = windowRoot_->FindWindowNodeWithToken(fromInfo->GetAbilityToken());
    // recent/aa abilityToken is null
    if (desNode != nullptr) {
        NotifyAnimationTransition(fromInfo, toInfo, fromNode, desNode);
    }
    // TODO: process back event/ recent event
    return;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token)
{
    uint32_t parentId = property->GetParentId();
    if ((parentId != INVALID_WINDOW_ID) && !WindowHelper::IsSubWindow(property->GetWindowType())) {
        WLOGFE("create window failed, type is error");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    sptr<WindowNode> node = windowRoot_->FindWindowNodeWithToken(token);
    if (node == nullptr || (!WindowHelper::IsMainWindow(property->GetWindowType()))) {
        windowId = GenWindowId();
        sptr<WindowProperty> windowProperty = new WindowProperty(property);
        windowProperty->SetWindowId(windowId);
        node = new WindowNode(windowProperty, window, surfaceNode);
        node->abilityToken_ = token;
        UpdateWindowAnimation(node);
        return windowRoot_->SaveWindow(node);
    }
    windowId = node->GetWindowId();
    node->surfaceNode_ = surfaceNode;
    node->SetWindowToken(window);
    node->SetCallingPid();
    node->SetCallingUid();
    sptr<WindowProperty> windowProperty = new(std::nothrow) WindowProperty(property);
    if (windowProperty == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    // mode & type should same as pre property
    windowProperty->SetWindowId(node->GetWindowId());
    // using starting window rect if client rect is empty
    if (WindowHelper::IsEmptyRect(property->GetRequestRect())) { // for tile and cascade
        WLOGFI("Id:%{public}u, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
            node->GetWindowId(), node->GetWindowRect().posX_, node->GetWindowRect().posY_, node->GetWindowRect().width_, node->GetWindowRect().height_);
        WLOGFI("before Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
            node->GetWindowId(), node->GetRequestRect().posX_, node->GetRequestRect().posY_, node->GetRequestRect().width_, node->GetRequestRect().height_);
        windowProperty->SetRequestRect(node->GetRequestRect());
        windowProperty->SetWindowRect(node->GetWindowRect());
    }
    node->SetWindowProperty(windowProperty);
    WLOGFI("after set Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), node->GetRequestRect().posX_, node->GetRequestRect().posY_, node->GetRequestRect().width_, node->GetRequestRect().height_);
    return windowRoot_->SaveWindowWithWindowToken(node);
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->currentVisibility_ && !node->isPlayAnimationShow_) {
        WLOGFE("current window is visible, windowId: %{public}u", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    ReSizeSystemBarPropertySizeIfNeed(property);
    // using starting window rect if client rect is empty
    if (WindowHelper::IsEmptyRect(property->GetRequestRect())) { // for tile and cascade
        WLOGFI("Id:%{public}u, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
            node->GetWindowId(), node->GetWindowRect().posX_, node->GetWindowRect().posY_, node->GetWindowRect().width_, node->GetWindowRect().height_);
        WLOGFI("Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
            node->GetWindowId(), node->GetRequestRect().posX_, node->GetRequestRect().posY_, node->GetRequestRect().width_, node->GetRequestRect().height_);
        property->SetRequestRect(node->GetRequestRect());
        property->SetWindowRect(node->GetWindowRect());
    }
    node->GetWindowProperty()->CopyFrom(property);

    // Need 'check permission'
    // Need 'adjust property'
    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfo(property->GetWindowId());
    HandleTurnScreenOn(node);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        sysBarWinId_[node->GetWindowType()] = node->GetWindowId();
    }

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType()) && !node->isPlayAnimationHide_) {
        WM_SCOPED_TRACE_BEGIN("controller:MinimizeStructuredAppWindowsExceptSelf");
        res = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(node);
        WM_SCOPED_TRACE_END();
        if (res != WMError::WM_OK) {
            WLOGFE("Minimize other structured window failed");
            return res;
        }
    }
    StopBootAnimationIfNeed(node->GetWindowType());
    return WMError::WM_OK;
}

void WindowController::ReSizeSystemBarPropertySizeIfNeed(sptr<WindowProperty>& property)
{
    auto displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(property->GetDisplayId());
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is null");
        return;
    }
    uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR) {
        auto statusBarRectIter =
                systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].find(displayHeight);
        if (statusBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].end()) {
            property->SetWindowRect(statusBarRectIter->second);
        }
    } else if (property->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto navigationBarRectIter =
                systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].find(displayHeight);
        if (navigationBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].end()) {
            property->SetWindowRect(navigationBarRectIter->second);
        }
    }
    if (curDisplayInfo_.find(displayInfo->GetDisplayId()) == curDisplayInfo_.end()) {
        curDisplayInfo_[displayInfo->GetDisplayId()] = displayInfo;
    }
}

void WindowController::HandleTurnScreenOn(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (node->IsTurnScreenOn() && !PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGFI("handle turn screen on");
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
}

WMError WindowController::RemoveWindowNode(uint32_t windowId)
{
    WMError res = windowRoot_->RemoveWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfo(windowId);
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    DisplayId displayId = DISPLAY_ID_INVALID;
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node != nullptr) {
        displayId = node->GetDisplayId();
    }
    WMError res = windowRoot_->DestroyWindow(windowId, onlySelf);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfoWithDisplayId(displayId);
    return res;
}

WMError WindowController::ResizeRect(uint32_t windowId, const Rect& rect, WindowSizeChangeReason reason)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING) {
        WLOGFE("fullscreen window could not resize");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    auto property = node->GetWindowProperty();
    node->SetWindowSizeChangeReason(reason);
    Rect lastRect = property->GetWindowRect();
    Rect newRect;
    if (reason == WindowSizeChangeReason::MOVE) {
        newRect = { rect.posX_, rect.posY_, lastRect.width_, lastRect.height_ };
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            if (windowRoot_->isVerticalDisplay(node)) {
                newRect.posX_ = lastRect.posX_;
            } else {
                newRect.posY_ = lastRect.posY_;
            }
        }
    } else if (reason == WindowSizeChangeReason::RESIZE) {
        newRect = { lastRect.posX_, lastRect.posY_, rect.width_, rect.height_ };
    } else if (reason == WindowSizeChangeReason::DRAG) {
        newRect = rect;
    }
    property->SetRequestRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_RECT);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    return windowRoot_->RequestFocus(windowId);
}

WMError WindowController::SetWindowMode(uint32_t windowId, WindowMode dstMode)
{
    WM_FUNCTION_TRACE();
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError ret = windowRoot_->SetWindowMode(node, dstMode);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel dstLevel)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFI("WindowEffect WindowController SetWindowBackgroundBlur level: %{public}u", dstLevel);
    node->SetWindowBackgroundBlur(dstLevel);
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::SetAlpha(uint32_t windowId, float dstAlpha)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFI("WindowEffect WindowController SetAlpha alpha: %{public}f", dstAlpha);
    node->SetAlpha(dstAlpha);

    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

void WindowController::NotifyDisplayStateChange(DisplayId displayId, DisplayStateChangeType type)
{
    WLOGFD("DisplayStateChangeType:%{public}u", type);
    switch (type) {
        case DisplayStateChangeType::BEFORE_SUSPEND: {
            windowRoot_->ProcessWindowStateChange(WindowState::STATE_FROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::BEFORE_UNLOCK: {
            windowRoot_->ProcessWindowStateChange(WindowState::STATE_UNFROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION: {
            ProcessDisplayChange(displayId, type);
            break;
        }
        case DisplayStateChangeType::CREATE: {
            windowRoot_->ProcessDisplayCreate(displayId);
            break;
        }
        case DisplayStateChangeType::DESTROY: {
            windowRoot_->ProcessDisplayDestroy(displayId);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
}

void WindowController::ProcessDisplayChange(DisplayId displayId, DisplayStateChangeType type)
{
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64 "", displayId);
        return;
    }

    switch (type) {
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION: {
            auto iter = curDisplayInfo_.find(displayId);
            if (iter != curDisplayInfo_.end()) {
                auto lastDisplayInfo = iter->second;
                uint32_t lastDisplayWidth = static_cast<uint32_t>(lastDisplayInfo->GetWidth());
                uint32_t lastDisplayHeight = static_cast<uint32_t>(lastDisplayInfo->GetHeight());
                auto statusBarNode = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR]);
                auto navigationBarNode =
                    windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR]);
                systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][lastDisplayWidth][lastDisplayHeight]
                    = statusBarNode->GetWindowProperty()->GetWindowRect();
                systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][lastDisplayWidth][lastDisplayHeight]
                    = navigationBarNode->GetWindowProperty()->GetWindowRect();
            }
            curDisplayInfo_[displayId] = displayInfo;
            windowRoot_->ProcessDisplayChange(displayInfo);
            // Remove 'sysBarWinId_' after SystemUI resize 'systembar'
            uint32_t width = static_cast<uint32_t>(displayInfo->GetWidth());
            uint32_t height = static_cast<uint32_t>(displayInfo->GetHeight() * SYSTEM_BAR_HEIGHT_RATIO);
            Rect newRect = { 0, 0, width, height };
            uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
            uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
            auto statusBarRectIter =
                systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].find(displayHeight);
            if (statusBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].end()) {
                newRect = statusBarRectIter->second;
            }
            ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR], newRect, WindowSizeChangeReason::DRAG);
            newRect = { 0, displayInfo->GetHeight() - static_cast<int32_t>(height), width, height };
            auto navigationBarRectIter =
                systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].find(displayHeight);
            if (navigationBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].end()) {
                newRect = navigationBarRectIter->second;
            }
            ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR], newRect, WindowSizeChangeReason::DRAG);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
    FlushWindowInfoWithDisplayId(displayId);
    WLOGFI("Finish ProcessDisplayChange");
}

void WindowController::StopBootAnimationIfNeed(WindowType type) const
{
    if (WindowType::WINDOW_TYPE_DESKTOP == type) {
        WLOGFD("stop boot animation");
        system::SetParameter("persist.window.boot.inited", "1");
    }
}

WMError WindowController::SetWindowType(uint32_t windowId, WindowType type)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    property->SetWindowType(type);
    UpdateWindowAnimation(node);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_TYPE);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    WLOGFI("SetWindowType end");
    return res;
}

WMError WindowController::SetWindowFlags(uint32_t windowId, uint32_t flags)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    property->SetWindowFlags(flags);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_FLAGS);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    WLOGFI("SetWindowFlags end");
    return res;
}

WMError WindowController::SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetSystemBarProperty(type, property);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_OTHER_PROPS);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    WLOGFI("SetSystemBarProperty end");
    return res;
}

void WindowController::NotifySystemBarTints()
{
    windowRoot_->NotifySystemBarTints();
}

WMError WindowController::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (windowAnimationController_ != nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, Already had a controller!");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    windowAnimationController_ = controller;
    return WMError::WM_OK;
}

std::vector<Rect> WindowController::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    std::vector<Rect> avoidArea = windowRoot_->GetAvoidAreaByType(windowId, avoidAreaType);
    return avoidArea;
}

WMError WindowController::ProcessPointDown(uint32_t windowId, bool isStartDrag)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!node->currentVisibility_) {
        WLOGFE("this window is not visibile and not in window tree, windowId: %{public}u", windowId);
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    if (isStartDrag) {
        WMError res = windowRoot_->UpdateSizeChangeReason(windowId, WindowSizeChangeReason::DRAG_START);
        return res;
    }

    WLOGFI("process point down, windowId: %{public}u", windowId);
    WMError zOrderRes = windowRoot_->RaiseZOrderForAppWindow(node);
    WMError focusRes = windowRoot_->RequestFocus(windowId);
    windowRoot_->RequestActiveWindow(windowId);
    if (zOrderRes == WMError::WM_OK || focusRes == WMError::WM_OK) {
        FlushWindowInfo(windowId);
        WLOGFI("ProcessPointDown end");
        return WMError::WM_OK;
    }
    windowRoot_->FocusFaultDetection();
    return WMError::WM_ERROR_INVALID_OPERATION;
}

WMError WindowController::ProcessPointUp(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError res = windowRoot_->UpdateSizeChangeReason(windowId, WindowSizeChangeReason::DRAG_END);
    if (res != WMError::WM_OK) {
        return res;
    }
    return WMError::WM_OK;
}

void WindowController::MinimizeAllAppWindows(DisplayId displayId)
{
    windowRoot_->MinimizeAllAppWindows(displayId);
}

WMError WindowController::MaxmizeWindow(uint32_t windowId)
{
    WMError ret = SetWindowMode(windowId, WindowMode::WINDOW_MODE_FULLSCREEN);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    ret = windowRoot_->MaxmizeWindow(windowId);
    FlushWindowInfo(windowId);
    return ret;
}

WMError WindowController::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return windowRoot_->GetTopWindowId(mainWinId, topWinId);
}

void WindowController::FlushWindowInfo(uint32_t windowId)
{
    WLOGFI("FlushWindowInfo");
    RSTransaction::FlushImplicitTransaction();
    inputWindowMonitor_->UpdateInputWindow(windowId);
}

void WindowController::FlushWindowInfoWithDisplayId(DisplayId displayId)
{
    WLOGFI("FlushWindowInfoWithDisplayId");
    RSTransaction::FlushImplicitTransaction();
    inputWindowMonitor_->UpdateInputWindowByDisplayId(displayId);
}

void WindowController::UpdateWindowAnimation(const sptr<WindowNode>& node)
{
    if (node == nullptr || node->surfaceNode_ == nullptr) {
        WLOGFE("windowNode or surfaceNode is nullptr");
        return;
    }

    uint32_t animationFlag = node->GetWindowProperty()->GetAnimationFlag();
    uint32_t windowId = node->GetWindowProperty()->GetWindowId();
    WLOGFI("windowId: %{public}u, animationFlag: %{public}u", windowId, animationFlag);
    if (animationFlag == static_cast<uint32_t>(WindowAnimation::DEFAULT)) {
        // set default transition effect for window: scale from 1.0 to 0.7, fade from 1.0 to 0.0
        static const auto effect = RSTransitionEffect::Create()->Scale(Vector3f(0.7f, 0.7f, 0.0f))->Opacity(0.0f);
        node->surfaceNode_->SetTransitionEffect(effect);
    } else {
        node->surfaceNode_->SetTransitionEffect(nullptr);
    }
}

WMError WindowController::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    WMError res = windowRoot_->SetWindowLayoutMode(displayId, mode);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfoWithDisplayId(displayId);
    return res;
}

WMError WindowController::UpdateProperty(sptr<WindowProperty>& property, PropertyChangeAction action)
{
    if (property == nullptr) {
        WLOGFE("property is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    uint32_t windowId = property->GetWindowId();
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("window: [%{public}s, %{public}u] update property for action: %{public}u", node->GetWindowName().c_str(),
        node->GetWindowId(), static_cast<uint32_t>(action));
    switch (action) {
        case PropertyChangeAction::ACTION_UPDATE_RECT: {
            node->SetDecoStatus(property->GetDecoStatus());
            ResizeRect(windowId, property->GetRequestRect(), property->GetWindowSizeChangeReason());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_MODE: {
            SetWindowMode(windowId, property->GetWindowMode());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_FLAGS: {
            SetWindowFlags(windowId, property->GetWindowFlags());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS: {
            auto& props = property->GetSystemBarProperty();
            for (auto& iter : props) {
                SetSystemBarProperty(windowId, iter.first, iter.second);
            }
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_FOCUSABLE: {
            node->SetFocusable(property->GetFocusable());
            windowRoot_->UpdateFocusableProperty(windowId);
            FlushWindowInfo(windowId);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TOUCHABLE: {
            node->SetTouchable(property->GetTouchable());
            FlushWindowInfo(windowId);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW: {
            node->SetCallingWindow(property->GetCallingWindow());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_ORIENTATION: {
            node->SetRequestedOrientation(property->GetRequestedOrientation());
            DisplayManagerServiceInner::GetInstance().
                SetOrientationFromWindow(node->GetDisplayId(), property->GetRequestedOrientation());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON: {
            node->SetTurnScreenOn(property->IsTurnScreenOn());
            HandleTurnScreenOn(node);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON: {
            node->SetKeepScreenOn(property->IsKeepScreenOn());
            windowRoot_->HandleKeepScreenOn(node->GetWindowId(), node->IsKeepScreenOn());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS: {
            node->SetBrightness(property->GetBrightness());
            windowRoot_->SetBrightness(node->GetWindowId(), node->GetBrightness());
            break;
        }
        default:
            break;
    }
    return WMError::WM_OK;
}

WMError WindowController::GetModeChangeHotZones(DisplayId displayId,
    ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config)
{
    return windowRoot_->GetModeChangeHotZones(displayId, hotZones, config);
}
} // namespace OHOS
} // namespace Rosen