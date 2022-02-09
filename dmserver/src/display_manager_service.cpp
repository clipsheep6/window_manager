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

#include "display_manager_service.h"
#include "window_manager_service.h"

#include <cinttypes>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager_agent_controller.h"
#include "transaction/rs_interfaces.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerService)
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<DisplayManagerService>());

DisplayManagerService::DisplayManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true),
    abstractScreenController_(new AbstractScreenController(mutex_)),
    abstractDisplayController_(new AbstractDisplayController(mutex_))
{
}

void DisplayManagerService::OnStart()
{
    WLOGFI("DisplayManagerService::OnStart start");
    if (!Init()) {
        return;
    }
}

bool DisplayManagerService::Init()
{
    WLOGFI("DisplayManagerService::Init start");
    bool ret = Publish(this);
    if (!ret) {
        WLOGFW("DisplayManagerService::Init failed");
        return false;
    }
    abstractScreenController_->Init();
    abstractDisplayController_->Init(abstractScreenController_);
    WLOGFI("DisplayManagerService::Init success");
    return true;
}

void DisplayManagerService::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    displayChangeListener_ = listener;
    WLOGFI("IDisplayChangeListener registered");
}

void DisplayManagerService::NotifyDisplayStateChange(DisplayStateChangeType type)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnDisplayStateChange(type);
    }
}

DisplayId DisplayManagerService::GetDisplayIdFromScreenId(ScreenId screenId)
{
    return (DisplayId)screenId;
}

ScreenId DisplayManagerService::GetScreenIdFromDisplayId(DisplayId displayId)
{
    return (ScreenId)displayId;
}

DisplayId DisplayManagerService::GetDefaultDisplayId()
{
    ScreenId screenId = abstractDisplayController_->GetDefaultScreenId();
    WLOGFI("GetDefaultDisplayId %{public}" PRIu64"", screenId);
    return GetDisplayIdFromScreenId(screenId);
}

DisplayInfo DisplayManagerService::GetDisplayInfoById(DisplayId displayId)
{
    DisplayInfo displayInfo;
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    auto screenModeInfo = abstractDisplayController_->GetScreenActiveMode(screenId);
    displayInfo.id_ = displayId;
    displayInfo.width_ = screenModeInfo.GetScreenWidth();
    displayInfo.height_ = screenModeInfo.GetScreenHeight();
    displayInfo.freshRate_ = screenModeInfo.GetScreenFreshRate();
    return displayInfo;
}

ScreenId DisplayManagerService::CreateVirtualScreen(VirtualScreenOption option)
{
    WM_SCOPED_TRACE("dms:CreateVirtualScreen(%s)", option.name_.c_str());
    ScreenId screenId = abstractScreenController_->CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService::CreateVirtualScreen: Get virtualScreenId failed");
        return SCREEN_ID_INVALID;
    }
    return screenId;
}

DMError DisplayManagerService::DestroyVirtualScreen(ScreenId screenId)
{
    WLOGFI("DestroyVirtualScreen::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: virtualScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:DestroyVirtualScreen(%" PRIu64")", screenId);
    std::map<ScreenId, std::shared_ptr<RSDisplayNode>>::iterator iter = displayNodeMap_.find(screenId);
    if (iter == displayNodeMap_.end()) {
        WLOGFE("DisplayManagerService: displayNode is nullptr");
        return abstractScreenController_->DestroyVirtualScreen(screenId);
    }
    displayNodeMap_[screenId]->RemoveFromTree();
    WLOGFE("DisplayManagerService: displayNode remove from tree");
    displayNodeMap_.erase(screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    return abstractScreenController_->DestroyVirtualScreen(screenId);
}

std::shared_ptr<Media::PixelMap> DisplayManagerService::GetDispalySnapshot(DisplayId displayId)
{
    WM_SCOPED_TRACE("dms:GetDispalySnapshot(%" PRIu64")", displayId);
    std::shared_ptr<Media::PixelMap> screenSnapshot
        = abstractDisplayController_->GetScreenSnapshot(displayId);
    return screenSnapshot;
}


DMError DisplayManagerService::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    WLOGFI("GetScreenSupportedColorGamuts::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:GetScreenSupportedColorGamuts(%" PRIu64")", screenId);

    colorGamuts.clear();
    colorGamuts.push_back(COLOR_GAMUT_NATIVE);
    return DMError::DM_OK;
}

DMError DisplayManagerService::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    WLOGFI("GetScreenColorGamut::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:GetScreenColorGamut(%" PRIu64")", screenId);

    colorGamut = COLOR_GAMUT_NATIVE;
    return DMError::DM_OK;
}

DMError DisplayManagerService::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    WLOGFI("SetScreenColorGamut::ScreenId: %{public}" PRIu64 ", colorGamutIdx %{public}d", screenId, colorGamutIdx);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:SetScreenColorGamut(%" PRIu64")", screenId);

    return DMError::DM_OK;
}

DMError DisplayManagerService::GetScreenGamutsMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    WLOGFI("GetScreenGamutsMap::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:GetScreenGamutsMap(%" PRIu64")", screenId);

    gamutMap = GAMUT_MAP_CONSTANT;
    return DMError::DM_OK;
}

DMError DisplayManagerService::SetScreenGamutsMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    WLOGFI("SetScreenGamutsMap::ScreenId: %{public}" PRIu64 ", ScreenGamutMap %{public}u",
        screenId, static_cast<uint32_t>(gamutMap));
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:SetScreenGamutsMap(%" PRIu64")", screenId);

    return DMError::DM_OK;
}

DMError DisplayManagerService::SetScreenColorTransform(ScreenId screenId)
{
    WLOGFI("SetScreenColorTransform::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:SetScreenColorTransform(%" PRIu64")", screenId);

    return DMError::DM_OK;
}


void DisplayManagerService::OnStop()
{
    WLOGFI("ready to stop display service.");
}

bool DisplayManagerService::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().RegisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::WakeUpBegin(PowerStateChangeReason reason)
{
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::WakeUpEnd()
{
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::END);
}

bool DisplayManagerService::SuspendBegin(PowerStateChangeReason reason)
{
    displayPowerController_.SuspendBegin(reason);
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::SuspendEnd()
{
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::END);
}

bool DisplayManagerService::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("SetScreenPowerForAll");
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(
        state == DisplayPowerState::POWER_ON ? DisplayPowerEvent::DISPLAY_ON :
        DisplayPowerEvent::DISPLAY_OFF, EventStatus::END);
}

bool DisplayManagerService::SetDisplayState(DisplayState state)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_.SetDisplayState(state);
}

DisplayState DisplayManagerService::GetDisplayState(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_.GetDisplayState(displayId);
}

void DisplayManagerService::NotifyDisplayEvent(DisplayEvent event)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayPowerController_.NotifyDisplayEvent(event);
}

sptr<AbstractScreenController> DisplayManagerService::GetAbstractScreenController()
{
    return abstractScreenController_;
}

DMError DisplayManagerService::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds)
{
    if (mainScreenId == SCREEN_ID_INVALID || mirrorScreenIds.empty()) {
        WLOGFI("create mirror fail, screen is invalid. Screen :%{public}" PRIu64"", mainScreenId);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:MakeMirror");
    WLOGFI("create mirror. Screen: %{public}" PRIu64"", mainScreenId);
    std::shared_ptr<RSDisplayNode> displayNode =
        SingletonContainer::Get<WindowManagerService>().GetDisplayNode(mainScreenId);
    if (displayNode == nullptr) {
        WLOGFE("create mirror fail, cannot get DisplayNode");
        return DMError::DM_ERROR_NULLPTR;
    }
    NodeId nodeId = displayNode->GetId();
    for (ScreenId mirrorScreenId : mirrorScreenIds) {
        struct RSDisplayNodeConfig config = {mirrorScreenId, true, nodeId};
        displayNodeMap_[mirrorScreenId] = RSDisplayNode::Create(config);
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    transactionProxy->FlushImplicitTransaction();
    WLOGFI("create mirror. NodeId: %{public}" PRIu64"", nodeId);
    return DMError::DM_OK;
}

sptr<ScreenInfo> DisplayManagerService::GetScreenInfoById(ScreenId screenId)
{
    auto screen = abstractScreenController_->GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screen->ConvertToScreenInfo();
}

sptr<ScreenGroupInfo> DisplayManagerService::GetScreenGroupInfoById(ScreenId screenId)
{
    auto screenGroup = abstractScreenController_->GetAbstractScreenGroup(screenId);
    if (screenGroup == nullptr) {
        WLOGE("cannot find screenGroupInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenGroup->ConvertToScreenGroupInfo();
}

std::vector<sptr<ScreenInfo>> DisplayManagerService::GetAllScreenInfos()
{
    std::vector<ScreenId> screenIds = abstractScreenController_->GetAllScreenIds();
    std::vector<sptr<ScreenInfo>> screenInfos;
    for (auto screenId: screenIds) {
        auto screenInfo = GetScreenInfoById(screenId);
        if (screenInfo == nullptr) {
            WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
            continue;
        }
        screenInfos.emplace_back(screenInfo);
    }
    return screenInfos;
}

DMError DisplayManagerService::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    // todo: make expand
    return DMError::DM_OK;
}

bool DisplayManagerService::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    return abstractScreenController_->SetScreenActiveMode(screenId, modeId);
}
} // namespace OHOS::Rosen