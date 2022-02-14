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

#include "window_controller.h"
#include <transaction/rs_transaction.h>
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowController"};
}
uint32_t WindowController::GenWindowId()
{
    return ++windowId_;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
{
    uint32_t parentId = property->GetParentId();
    if ((parentId != INVALID_WINDOW_ID) && !WindowHelper::IsSubWindow(property->GetWindowType())) {
        WLOGFE("create window failed, type is error");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    windowId = GenWindowId();
    property->SetWindowId(windowId);

    // set default transition effect for window
    static auto effect = RSTransitionEffect::Create()->Scale({ 0.0f, 0.0f, 0.0f })->Opacity(0.0f);
    if (surfaceNode != nullptr) {
        surfaceNode->SetTransitionEffect(effect);
    }

    sptr<WindowNode> node = new WindowNode(property, window, surfaceNode);
    return windowRoot_->SaveWindow(node);
}

WMError WindowController::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->abilityToken_ = abilityToken;
    return WMError::WM_OK;
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetWindowProperty(property);
    // TODO: check permission
    // TODO: adjust property

    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(property->GetWindowId());

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        sysBarWinId_[node->GetWindowType()] = node->GetWindowId();
    }

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        WM_SCOPED_TRACE_BEGIN("controller:MinimizeStructuredAppWindowsExceptSelf");
        res = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(node);
        WM_SCOPED_TRACE_END();
        if (res != WMError::WM_OK) {
            WLOGFE("Minimize other structured window failed");
        }
    }
    return res;
}

WMError WindowController::RemoveWindowNode(uint32_t windowId)
{
    WMError res = windowRoot_->RemoveWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    DisplayId displayId = DISPLAY_ID_INVALD;
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node != nullptr) {
        displayId = node->GetDisplayId();
    }
    WMError res = windowRoot_->DestroyWindow(windowId, onlySelf);
    if (res != WMError::WM_OK) {
        return res;
    }
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
    auto property = node->GetWindowProperty();
    Rect lastRect = property->GetWindowRect();
    Rect newRect;
    node->SetWindowSizeChangeReason(reason);
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
        node->hasDecorated_ = false;
        newRect = { lastRect.posX_, lastRect.posY_, rect.width_, rect.height_ };
    } else if (reason == WindowSizeChangeReason::DRAG) {
        if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
            // fix rect in case of moving window when dragging
            newRect = WindowHelper::GetFixedWindowRectByMinRect(rect,
                property->GetWindowRect(), windowRoot_->isVerticalDisplay(node));
        } else {
            newRect = rect;
        }
    }

    property->SetWindowRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    return windowRoot_->RequestFocus(windowId);
}

WMError WindowController::SetWindowMode(uint32_t windowId, WindowMode dstMode)
{
    WM_FUNCTION_TRACE();
    auto node = windowRoot_->GetWindowNode(windowId);
    WindowMode srcMode = node->GetWindowMode();
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (srcMode == dstMode) {
        return WMError::WM_OK;
    }
    WMError res = WMError::WM_OK;
    node->SetWindowMode(dstMode);
    if ((srcMode == WindowMode::WINDOW_MODE_FULLSCREEN) && (dstMode == WindowMode::WINDOW_MODE_FLOATING)) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RECOVER);
    } else if (dstMode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::MAXIMIZE);
    } else {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RESIZE);
    }
    if (WindowHelper::IsSplitWindowMode(srcMode)) {
        // change split mode to other
        res = windowRoot_->ExitSplitWindowMode(node);
    } else if (!WindowHelper::IsSplitWindowMode(srcMode) && WindowHelper::IsSplitWindowMode(dstMode)) {
        // change other mode to split
        res = windowRoot_->EnterSplitWindowMode(node);
    }
    if (res != WMError::WM_OK) {
        node->GetWindowProperty()->ResumeLastWindowMode();
        return res;
    }
    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        // minimize other app window
        res = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(node);
        if (res != WMError::WM_OK) {
            return res;
        }
    }
    res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        WLOGFE("Set window mode failed, update node failed");
        return res;
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
            windowRoot_->NotifyWindowStateChange(WindowState::STATE_FROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::BEFORE_UNLOCK: {
            windowRoot_->NotifyWindowStateChange(WindowState::STATE_UNFROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::UPDATE_ROTATION: {
            const sptr<AbstractDisplay> abstractDisplay
                = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
            if (abstractDisplay == nullptr) {
                WLOGFE("get display failed displayId:%{public}" PRId64 "", displayId);
                return;
            }
            windowRoot_->NotifyDisplayChange(abstractDisplay);

            // TODO: Remove 'sysBarWinId_' after SystemUI resize 'systembar'
            uint32_t width = abstractDisplay->GetWidth();
            uint32_t height = abstractDisplay->GetHeight() * SYSTEM_BAR_HEIGHT_RATIO;
            Rect newRect = { 0, 0, width, height };
            ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR], newRect, WindowSizeChangeReason::DRAG);
            newRect = { 0, abstractDisplay->GetHeight() - height, width, height };
            ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR], newRect, WindowSizeChangeReason::DRAG);

            FlushWindowInfoWithDisplayId(displayId);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
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
    WMError res = windowRoot_->UpdateWindowNode(windowId);
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
    WMError res = windowRoot_->UpdateWindowNode(windowId);
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
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    WLOGFI("SetSystemBarProperty end");
    return res;
}

std::vector<Rect> WindowController::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    std::vector<Rect> avoidArea = windowRoot_->GetAvoidAreaByType(windowId, avoidAreaType);
    return avoidArea;
}

WMError WindowController::ProcessWindowTouchedEvent(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError zOrderRes = windowRoot_->RaiseZOrderForAppWindow(node);
    WMError focusRes = windowRoot_->RequestFocus(windowId);
    if (zOrderRes == WMError::WM_OK || focusRes == WMError::WM_OK) {
        FlushWindowInfo(windowId);
        WLOGFI("ProcessWindowTouchedEvent end");
        return WMError::WM_OK;
    }
    return WMError::WM_ERROR_INVALID_OPERATION;
}

void WindowController::MinimizeAllAppWindows(DisplayId displayId)
{
    windowRoot_->MinimizeAllAppWindows(displayId);
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

WMError WindowController::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    WMError res = windowRoot_->SetWindowLayoutMode(displayId, mode);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfoWithDisplayId(displayId);
    return res;
}
}
}