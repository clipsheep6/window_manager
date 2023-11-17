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

#include "display_manager_adapter.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager.h"
#include "screen_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerAdapter"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerAdapter)
WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManagerAdapter)

#define INIT_PROXY_CHECK_RETURN(ret) \
    do { \
        if (!InitDMSProxy()) { \
            WLOGFE("InitDMSProxy failed!"); \
            return ret; \
        } \
    } while (false)

sptr<DisplayInfo> DisplayManagerAdapter::GetDefaultDisplayInfo()
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDefaultDisplayInfo();
}

sptr<DisplayInfo> DisplayManagerAdapter::GetDisplayInfoByScreenId(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return  displayManagerServiceProxy_->GetDisplayInfoByScreen(screenId);
}

std::shared_ptr<Media::PixelMap> DisplayManagerAdapter::GetDisplaySnapshot(DisplayId displayId, DmErrorCode* errorCode)
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDisplaySnapshot(displayId, errorCode);
}

DMError DisplayManagerAdapter::DisableDisplaySnapshot(bool disableOrNot)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->DisableDisplaySnapshot(disableOrNot);
}

DMError ScreenManagerAdapter::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetScreenSupportedColorGamuts(screenId, colorGamuts);
}

DMError ScreenManagerAdapter::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetScreenColorGamut(screenId, colorGamut);
}

DMError ScreenManagerAdapter::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenColorGamut(screenId, colorGamutIdx);
}

DMError ScreenManagerAdapter::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetScreenGamutMap(screenId, gamutMap);
}

DMError ScreenManagerAdapter::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenGamutMap(screenId, gamutMap);
}

DMError ScreenManagerAdapter::SetScreenColorTransform(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenColorTransform(screenId);
}

ScreenId ScreenManagerAdapter::CreateVirtualScreen(VirtualScreenOption option,
    const sptr<IDisplayManagerAgent>& displayManagerAgent)
{
    INIT_PROXY_CHECK_RETURN(SCREEN_ID_INVALID);

    WLOGFI("DisplayManagerAdapter::CreateVirtualScreen");
    return displayManagerServiceProxy_->CreateVirtualScreen(option, displayManagerAgent->AsObject());
}

DMError ScreenManagerAdapter::DestroyVirtualScreen(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    WLOGFI("DisplayManagerAdapter::DestroyVirtualScreen");
    return displayManagerServiceProxy_->DestroyVirtualScreen(screenId);
}

DMError ScreenManagerAdapter::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    if (surface == nullptr) {
        WLOGFE("Surface is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    WLOGFI("DisplayManagerAdapter::SetVirtualScreenSurface");
    return displayManagerServiceProxy_->SetVirtualScreenSurface(screenId, surface->GetProducer());
}

DMError ScreenManagerAdapter::SetVirtualMirrorScreenBufferRotation(ScreenId screenId, bool bufferRotation)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    WLOGFI("DisplayManagerAdapter::SetVirtualMirrorScreenBufferRotation");
    return displayManagerServiceProxy_->SetVirtualMirrorScreenBufferRotation(screenId, bufferRotation);
}

DMError ScreenManagerAdapter::SetScreenRotationLocked(bool isLocked)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    WLOGFI("DisplayManagerAdapter::SetScreenRotationLocked");
    return displayManagerServiceProxy_->SetScreenRotationLocked(isLocked);
}

DMError ScreenManagerAdapter::IsScreenRotationLocked(bool& isLocked)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    WLOGFI("DisplayManagerAdapter::IsScreenRotationLocked");
    return displayManagerServiceProxy_->IsScreenRotationLocked(isLocked);
}

bool ScreenManagerAdapter::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);
    return displayManagerServiceProxy_->SetScreenPowerForAll(state, reason);
}

ScreenPowerState ScreenManagerAdapter::GetScreenPower(ScreenId dmsScreenId)
{
    INIT_PROXY_CHECK_RETURN(ScreenPowerState::INVALID_STATE);
    return displayManagerServiceProxy_->GetScreenPower(dmsScreenId);
}

DMError ScreenManagerAdapter::SetOrientation(ScreenId screenId, Orientation orientation)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetOrientation(screenId, orientation);
}

DMError BaseAdapter::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->RegisterDisplayManagerAgent(displayManagerAgent, type);
}

DMError BaseAdapter::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerAdapter::WakeUpBegin(PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->WakeUpBegin(reason);
}

bool DisplayManagerAdapter::WakeUpEnd()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->WakeUpEnd();
}

bool DisplayManagerAdapter::SuspendBegin(PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SuspendBegin(reason);
}

bool DisplayManagerAdapter::SuspendEnd()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SuspendEnd();
}

bool DisplayManagerAdapter::SetDisplayState(DisplayState state)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SetDisplayState(state);
}

DisplayState DisplayManagerAdapter::GetDisplayState(DisplayId displayId)
{
    INIT_PROXY_CHECK_RETURN(DisplayState::UNKNOWN);

    return displayManagerServiceProxy_->GetDisplayState(displayId);
}

void DisplayManagerAdapter::NotifyDisplayEvent(DisplayEvent event)
{
    INIT_PROXY_CHECK_RETURN();

    displayManagerServiceProxy_->NotifyDisplayEvent(event);
}

bool DisplayManagerAdapter::SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze)
{
    INIT_PROXY_CHECK_RETURN(false);
    return displayManagerServiceProxy_->SetFreeze(displayIds, isFreeze);
}

bool BaseAdapter::InitDMSProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject
            = systemAbilityManager->GetSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID);
        if (!remoteObject) {
            WLOGFE("Failed to get display manager service.");
            return false;
        }

        displayManagerServiceProxy_ = iface_cast<IDisplayManager>(remoteObject);
        if ((!displayManagerServiceProxy_) || (!displayManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system display manager services");
            return false;
        }

        dmsDeath_ = new(std::nothrow) DMSDeathRecipient(*this);
        if (dmsDeath_ == nullptr) {
            WLOGFE("Failed to create death Recipient ptr DMSDeathRecipient");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(dmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

DMSDeathRecipient::DMSDeathRecipient(BaseAdapter& adapter) : adapter_(adapter)
{
}

void DMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
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
    WLOGFI("dms OnRemoteDied");
    adapter_.Clear();
    SingletonContainer::Get<DisplayManager>().OnRemoteDied();
    SingletonContainer::Get<ScreenManager>().OnRemoteDied();
    return;
}


BaseAdapter::~BaseAdapter()
{
    WLOGFI("BaseAdapter destory!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayManagerServiceProxy_ = nullptr;
}

void BaseAdapter::Clear()
{
    WLOGFI("BaseAdapter Clear!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((displayManagerServiceProxy_ != nullptr) && (displayManagerServiceProxy_->AsObject() != nullptr)) {
        displayManagerServiceProxy_->AsObject()->RemoveDeathRecipient(dmsDeath_);
    }
    isProxyValid_ = false;
}

DMError ScreenManagerAdapter::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId,
                                         ScreenId& screenGroupId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
}

DMError ScreenManagerAdapter::StopMirror(const std::vector<ScreenId>& mirrorScreenIds)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->StopMirror(mirrorScreenIds);
}

DMError ScreenManagerAdapter::DisableMirror(bool disableOrNot)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->DisableMirror(disableOrNot);
}

sptr<ScreenInfo> ScreenManagerAdapter::GetScreenInfo(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    sptr<ScreenInfo> screenInfo = displayManagerServiceProxy_->GetScreenInfoById(screenId);
    return screenInfo;
}

std::vector<DisplayId> DisplayManagerAdapter::GetAllDisplayIds()
{
    INIT_PROXY_CHECK_RETURN(std::vector<DisplayId>());

    return displayManagerServiceProxy_->GetAllDisplayIds();
}

DMError DisplayManagerAdapter::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->HasPrivateWindow(displayId, hasPrivateWindow);
}

DMError DisplayManagerAdapter::HasImmersiveWindow(bool& immersive)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->HasImmersiveWindow(immersive);
}

sptr<DisplayInfo> DisplayManagerAdapter::GetDisplayInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDisplayInfoById(displayId);
}

sptr<CutoutInfo> DisplayManagerAdapter::GetCutoutInfo(DisplayId displayId)
{
    WLOGFI("DisplayManagerAdapter::GetCutoutInfo");
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);
    return displayManagerServiceProxy_->GetCutoutInfo(displayId);
}

DMError DisplayManagerAdapter::AddSurfaceNodeToDisplay(DisplayId displayId,
    std::shared_ptr<class RSSurfaceNode>& surfaceNode)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    return displayManagerServiceProxy_->AddSurfaceNodeToDisplay(displayId, surfaceNode);
}

DMError DisplayManagerAdapter::RemoveSurfaceNodeFromDisplay(DisplayId displayId,
    std::shared_ptr<class RSSurfaceNode>& surfaceNode)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);
    return displayManagerServiceProxy_->RemoveSurfaceNodeFromDisplay(displayId, surfaceNode);
}

bool DisplayManagerAdapter::IsFoldable()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->IsFoldable();
}

FoldStatus DisplayManagerAdapter::GetFoldStatus()
{
    INIT_PROXY_CHECK_RETURN(FoldStatus::UNKNOWN);

    return displayManagerServiceProxy_->GetFoldStatus();
}

FoldDisplayMode DisplayManagerAdapter::GetFoldDisplayMode()
{
    INIT_PROXY_CHECK_RETURN(FoldDisplayMode::UNKNOWN);

    return displayManagerServiceProxy_->GetFoldDisplayMode();
}

void DisplayManagerAdapter::SetFoldDisplayMode(const FoldDisplayMode mode)
{
    INIT_PROXY_CHECK_RETURN();

    return displayManagerServiceProxy_->SetFoldDisplayMode(mode);
}

sptr<FoldCreaseRegion> DisplayManagerAdapter::GetCurrentFoldCreaseRegion()
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetCurrentFoldCreaseRegion();
}

sptr<ScreenGroupInfo> ScreenManagerAdapter::GetScreenGroupInfoById(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenGroup id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetScreenGroupInfoById(screenId);
}

DMError ScreenManagerAdapter::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetAllScreenInfos(screenInfos);
}

DMError ScreenManagerAdapter::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                                         ScreenId& screenGroupId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->MakeExpand(screenId, startPoint, screenGroupId);
}

DMError ScreenManagerAdapter::StopExpand(const std::vector<ScreenId>& expandScreenIds)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->StopExpand(expandScreenIds);
}


void ScreenManagerAdapter::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    INIT_PROXY_CHECK_RETURN();

    displayManagerServiceProxy_->RemoveVirtualScreenFromGroup(screens);
}

DMError ScreenManagerAdapter::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenActiveMode(screenId, modeId);
}

DMError ScreenManagerAdapter::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetVirtualPixelRatio(screenId, virtualPixelRatio);
}

DMError ScreenManagerAdapter::MakeUniqueScreen(const std::vector<ScreenId>& screenIds)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->MakeUniqueScreen(screenIds);
}
} // namespace OHOS::Rosen