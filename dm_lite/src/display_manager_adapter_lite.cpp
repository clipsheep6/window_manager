/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "display_manager_adapter_lite.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager_lite.h"
#include "dm_common.h"
#include "scene_board_judgement.h"
#include "screen_manager_lite.h"
#include "window_manager_hilog.h"
#include "session_manager_lite.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerAdapterLite"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerAdapterLite)
WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManagerAdapterLite)

#define INIT_PROXY_CHECK_RETURN(ret) \
    do { \
        if (!InitDMSProxy()) { \
            WLOGFE("InitDMSProxy failed! "); \
            return ret; \
        } \
    } while (false)

sptr<DisplayInfo> DisplayManagerAdapterLite::GetDefaultDisplayInfo()
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDefaultDisplayInfo();
}

sptr<DisplayInfo> DisplayManagerAdapterLite::GetDisplayInfoByScreenId(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return  displayManagerServiceProxy_->GetDisplayInfoByScreen(screenId);
}

DMError DisplayManagerAdapterLite::DisableDisplaySnapshot(bool disableOrNot)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->DisableDisplaySnapshot(disableOrNot);
}

DMError ScreenManagerAdapterLite::SetScreenRotationLocked(bool isLocked)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    WLOGFI("DisplayManagerAdapterLite::SetScreenRotationLocked");
    return displayManagerServiceProxy_->SetScreenRotationLocked(isLocked);
}

DMError ScreenManagerAdapterLite::IsScreenRotationLocked(bool& isLocked)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    WLOGFI("DisplayManagerAdapterLite::IsScreenRotationLocked");
    return displayManagerServiceProxy_->IsScreenRotationLocked(isLocked);
}

bool ScreenManagerAdapterLite::SetSpecifiedScreenPower(ScreenId screenId,
    ScreenPowerState state, PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);
    return displayManagerServiceProxy_->SetSpecifiedScreenPower(screenId, state, reason);
}

bool ScreenManagerAdapterLite::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);
    return displayManagerServiceProxy_->SetScreenPowerForAll(state, reason);
}

ScreenPowerState ScreenManagerAdapterLite::GetScreenPower(ScreenId dmsScreenId)
{
    INIT_PROXY_CHECK_RETURN(ScreenPowerState::INVALID_STATE);
    return displayManagerServiceProxy_->GetScreenPower(dmsScreenId);
}

DMError ScreenManagerAdapterLite::SetOrientation(ScreenId screenId, Orientation orientation)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetOrientation(screenId, orientation);
}

DMError BaseAdapterLite::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->RegisterDisplayManagerAgent(displayManagerAgent, type);
}

DMError BaseAdapterLite::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerAdapterLite::WakeUpBegin(PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->WakeUpBegin(reason);
}

bool DisplayManagerAdapterLite::WakeUpEnd()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->WakeUpEnd();
}

bool DisplayManagerAdapterLite::SuspendBegin(PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SuspendBegin(reason);
}

bool DisplayManagerAdapterLite::SuspendEnd()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SuspendEnd();
}

bool DisplayManagerAdapterLite::SetDisplayState(DisplayState state)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SetDisplayState(state);
}

DisplayState DisplayManagerAdapterLite::GetDisplayState(DisplayId displayId)
{
    INIT_PROXY_CHECK_RETURN(DisplayState::UNKNOWN);

    return displayManagerServiceProxy_->GetDisplayState(displayId);
}

void DisplayManagerAdapterLite::NotifyDisplayEvent(DisplayEvent event)
{
    INIT_PROXY_CHECK_RETURN();

    displayManagerServiceProxy_->NotifyDisplayEvent(event);
}

bool BaseAdapterLite::InitDMSProxy()
{
#ifdef SCENE_BOARD_DISABLED
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to  get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject
            = systemAbilityManager->GetSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID);
        if (!remoteObject) {
            WLOGFE("Failed to  get display manager service.");
            return false;
        }
        displayManagerServiceProxy_ = iface_cast<IDisplayManager>(remoteObject);
        if ((!displayManagerServiceProxy_) || (!displayManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to  get system display manager services");
            return false;
        }

        dmsDeath_ = new(std::nothrow) DMSDeathRecipientLite(*this);
        if (dmsDeath_ == nullptr) {
            WLOGFE("Failed to  create death Recipient ptr DMSDeathRecipientLite");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(dmsDeath_)) {
            WLOGFE("Failed to  add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
#else
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        displayManagerServiceProxy_ = SessionManagerLite::GetInstance().GetScreenSessionManagerLiteProxy();
        if ((!displayManagerServiceProxy_) || (!displayManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get  system scene session manager services");
            return false;
        }

        dmsDeath_ = new (std::nothrow) DMSDeathRecipientLite(*this);
        if (!dmsDeath_) {
            WLOGFE("Failed to create  death Recipient ptr DMSDeathRecipientLite");
            return false;
        }
        sptr<IRemoteObject> remoteObject = displayManagerServiceProxy_->AsObject();
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(dmsDeath_)) {
            WLOGFE("Failed to add  death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
#endif
}

DMSDeathRecipientLite::DMSDeathRecipientLite(BaseAdapterLite& adapter) : adapter_(adapter)
{
}

void DMSDeathRecipientLite::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath  is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object  is null");
        return;
    }
    WLOGFI("dms  OnRemoteDied");
    adapter_.Clear();
    SingletonContainer::Get<DisplayManagerLite>().OnRemoteDied();
    SingletonContainer::Get<ScreenManagerLite>().OnRemoteDied();
    SingletonContainer::Get<SessionManagerLite>().ClearSessionManagerProxy();
    return;
}


BaseAdapterLite::~BaseAdapterLite()
{
    WLOGFI("BaseAdapterLite destory!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayManagerServiceProxy_ = nullptr;
}

void BaseAdapterLite::Clear()
{
    WLOGFI("BaseAdapterLite Clear!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((displayManagerServiceProxy_ != nullptr) && (displayManagerServiceProxy_->AsObject() != nullptr)) {
        displayManagerServiceProxy_->AsObject()->RemoveDeathRecipient(dmsDeath_);
    }
    isProxyValid_ = false;
}

sptr<ScreenInfo> ScreenManagerAdapterLite::GetScreenInfo(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screen   id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    sptr<ScreenInfo> screenInfo = displayManagerServiceProxy_->GetScreenInfoById(screenId);
    return screenInfo;
}

std::vector<DisplayId> DisplayManagerAdapterLite::GetAllDisplayIds()
{
    INIT_PROXY_CHECK_RETURN(std::vector<DisplayId>());

    return displayManagerServiceProxy_->GetAllDisplayIds();
}

DMError DisplayManagerAdapterLite::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->HasPrivateWindow(displayId, hasPrivateWindow);
}

sptr<DisplayInfo> DisplayManagerAdapterLite::GetDisplayInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDisplayInfoById(displayId);
}

bool DisplayManagerAdapterLite::IsFoldable()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->IsFoldable();
}

FoldStatus DisplayManagerAdapterLite::GetFoldStatus()
{
    INIT_PROXY_CHECK_RETURN(FoldStatus::UNKNOWN);

    return displayManagerServiceProxy_->GetFoldStatus();
}

FoldDisplayMode DisplayManagerAdapterLite::GetFoldDisplayMode()
{
    INIT_PROXY_CHECK_RETURN(FoldDisplayMode::UNKNOWN);

    return displayManagerServiceProxy_->GetFoldDisplayMode();
}

void DisplayManagerAdapterLite::SetFoldDisplayMode(const FoldDisplayMode mode)
{
    INIT_PROXY_CHECK_RETURN();

    return displayManagerServiceProxy_->SetFoldDisplayMode(mode);
}

sptr<FoldCreaseRegion> DisplayManagerAdapterLite::GetCurrentFoldCreaseRegion()
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetCurrentFoldCreaseRegion();
}

sptr<ScreenGroupInfo> ScreenManagerAdapterLite::GetScreenGroupInfoById(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenGroup id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetScreenGroupInfoById(screenId);
}

DMError ScreenManagerAdapterLite::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetAllScreenInfos(screenInfos);
}

DMError ScreenManagerAdapterLite::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenActiveMode(screenId, modeId);
}

DMError ScreenManagerAdapterLite::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetVirtualPixelRatio(screenId, virtualPixelRatio);
}
} // namespace OHOS::Rosen