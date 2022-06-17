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

#include "window_adapter.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAdapter"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowAdapter)

#define INIT_PROXY_CHECK_RETURN(ret) \
    do { \
        if (!InitWMSProxy()) { \
            WLOGFE("InitWMSProxy failed!"); \
            return ret; \
        } \
    } while (false)

WMError WindowAdapter::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
    std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId, const sptr<IRemoteObject>& token)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->CreateWindow(window, windowProperty, surfaceNode, windowId, token);
}

WMError WindowAdapter::AddWindow(sptr<WindowProperty>& windowProperty)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->AddWindow(windowProperty);
}

WMError WindowAdapter::RemoveWindow(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->RemoveWindow(windowId);
}

WMError WindowAdapter::DestroyWindow(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->DestroyWindow(windowId);
}

WMError WindowAdapter::RequestFocus(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->RequestFocus(windowId);
}

void WindowAdapter::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN();

    return windowManagerServiceProxy_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

void WindowAdapter::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN();

    return windowManagerServiceProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent);
}

WMError WindowAdapter::GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->GetAccessibilityWindowInfo(windowInfo);
}

WMError WindowAdapter::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->SetWindowAnimationController(controller);
}

WMError WindowAdapter::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type, AvoidArea& avoidArea)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    avoidArea = windowManagerServiceProxy_->GetAvoidAreaByType(windowId, type);
    return WMError::WM_OK;
}

WMError WindowAdapter::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->SetWindowBackgroundBlur(windowId, level);
}

WMError WindowAdapter::SetAlpha(uint32_t windowId, float alpha)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->SetAlpha(windowId, alpha);
}

void WindowAdapter::ProcessPointDown(uint32_t windowId, bool isStartDrag)
{
    INIT_PROXY_CHECK_RETURN();

    return windowManagerServiceProxy_->ProcessPointDown(windowId, isStartDrag);
}

void WindowAdapter::ProcessPointUp(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN();

    return windowManagerServiceProxy_->ProcessPointUp(windowId);
}

void WindowAdapter::MinimizeAllAppWindows(DisplayId displayId)
{
    INIT_PROXY_CHECK_RETURN();

    windowManagerServiceProxy_->MinimizeAllAppWindows(displayId);
}

WMError WindowAdapter::ToggleShownStateForAllAppWindows()
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->ToggleShownStateForAllAppWindows();
}

WMError WindowAdapter::MaxmizeWindow(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->MaxmizeWindow(windowId);
}

WMError WindowAdapter::GetSystemConfig(SystemConfig& systemConfig)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->GetSystemConfig(systemConfig);
}

WMError WindowAdapter::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->GetModeChangeHotZones(displayId, hotZones);
}

bool WindowAdapter::InitWMSProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
        if (!remoteObject) {
            WLOGFE("Failed to get window manager service.");
            return false;
        }

        windowManagerServiceProxy_ = iface_cast<IWindowManager>(remoteObject);
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system window manager services");
            return false;
        }

        wmsDeath_ = new WMSDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

void WindowAdapter::ClearWindowAdapter()
{
    if ((windowManagerServiceProxy_ != nullptr) && (windowManagerServiceProxy_->AsObject() != nullptr)) {
        windowManagerServiceProxy_->AsObject()->RemoveDeathRecipient(wmsDeath_);
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isProxyValid_ = false;
}

void WMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    SingletonContainer::Get<WindowAdapter>().ClearWindowAdapter();
}

WMError WindowAdapter::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->GetTopWindowId(mainWinId, topWinId);
}

WMError WindowAdapter::SetWindowLayoutMode(WindowLayoutMode mode)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->SetWindowLayoutMode(mode);
}

WMError WindowAdapter::UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->UpdateProperty(windowProperty, action);
}

WMError WindowAdapter::NotifyWindowTransition(sptr<WindowTransitionInfo> from, sptr<WindowTransitionInfo> to)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->NotifyWindowTransition(from, to);
}

WMError WindowAdapter::UpdateAvoidAreaListener(uint32_t windowId, bool haveListener)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    return windowManagerServiceProxy_->UpdateAvoidAreaListener(windowId, haveListener);
}
} // namespace Rosen
} // namespace OHOS