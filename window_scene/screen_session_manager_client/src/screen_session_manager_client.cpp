/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screen_session_manager_client.h"

#include <hitrace_meter.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <transaction/rs_transaction.h>
#include <transaction/rs_interfaces.h>
#include "dm_common.h"
#include "pipeline/rs_node_map.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerClient" };
std::mutex g_instanceMutex;
} // namespace

ScreenSessionManagerClient& ScreenSessionManagerClient::GetInstance()
{
    std::lock_guard<std::mutex> lock(g_instanceMutex);
    static sptr<ScreenSessionManagerClient> instance = nullptr;
    if (instance == nullptr) {
        instance = new ScreenSessionManagerClient();
    }
    return *instance;
}

void ScreenSessionManagerClient::ConnectToServer()
{
    if (screenSessionManager_) {
        WLOGFI("Success to get screen session manager proxy");
        return;
    }
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityMgr) {
        WLOGFE("Failed to get system ability mgr");
        return;
    }

    auto remoteObject = systemAbilityMgr->GetSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get display manager service");
        return;
    }

    screenSessionManager_ = iface_cast<IScreenSessionManager>(remoteObject);
    if (!screenSessionManager_) {
        WLOGFE("Failed to get screen session manager proxy");
        return;
    }
    screenSessionManager_->SetClient(this);
}

void ScreenSessionManagerClient::RegisterScreenConnectionListener(IScreenConnectionListener* listener)
{
    if (listener == nullptr) {
        WLOGFE("Failed to register screen connection listener, listener is null");
        return;
    }

    screenConnectionListener_ = listener;
    ConnectToServer();
    WLOGFI("Success to register screen connection listener");
}

bool ScreenSessionManagerClient::CheckIfNeedConnectScreen(ScreenId screenId, ScreenId rsId, const std::string& name)
{
    if (rsId == SCREEN_ID_INVALID) {
        WLOGFE("rsId is invalid");
        return false;
    }
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is nullptr");
        return false;
    }
    if (screenSessionManager_->GetScreenProperty(screenId).GetScreenType() == ScreenType::VIRTUAL) {
        if (name == "HiCar" || name == "SuperLauncher" || name == "CastEngine" || name == "DevEcoViewer") {
            WLOGFI("HiCar or SuperLauncher or CastEngine or DevEcoViewer, need to connect the screen");
            return true;
        } else {
            WLOGFE("ScreenType is virtual, no need to connect the screen");
            return false;
        }
    }
    return true;
}

void ScreenSessionManagerClient::OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
    ScreenId rsId, const std::string& name, bool isExtend)
{
    WLOGFI("screenId: %{public}" PRIu64 " screenEvent: %{public}d rsId: %{public}" PRIu64 " name: %{public}s",
        screenId, static_cast<int>(screenEvent), rsId, name.c_str());
    if (screenEvent == ScreenEvent::CONNECTED) {
        if (!CheckIfNeedConnectScreen(screenId, rsId, name)) {
            WLOGFE("There is no need to connect the screen");
            return;
        }
        ScreenSessionConfig config = {
            .screenId = screenId,
            .rsId = rsId,
            .name = name,
        };
        config.property = screenSessionManager_->GetScreenProperty(screenId);
        config.displayNode = screenSessionManager_->GetDisplayNode(screenId);
        sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
        screenSession->SetScreenCombination(screenSessionManager_->GetScreenCombination(screenId));
        {
            std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
            screenSessionMap_.emplace(screenId, screenSession);
        }
        screenSession->SetIsExtend(isExtend);
        if (screenConnectionListener_) {
            screenConnectionListener_->OnScreenConnected(screenSession);
            WLOGFI("screenId: %{public}" PRIu64 " density: %{public}f ",
                screenId, config.property.GetDensity());
            screenSession->SetScreenSceneDpi(config.property.GetDensity());
        }
        screenSession->Connect();
        return;
    }
    if (screenEvent == ScreenEvent::DISCONNECTED) {
        auto screenSession = GetScreenSession(screenId);
        if (!screenSession) {
            WLOGFE("screenSession is null");
            return;
        }
        screenSession->DestroyScreenScene();
        if (screenConnectionListener_) {
            screenConnectionListener_->OnScreenDisconnected(screenSession);
        }
        {
            std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
            screenSessionMap_.erase(screenId);
        }
        screenSession->Disconnect();
    }
}
void ScreenSessionManagerClient::OnScreenExtendChanged(ScreenId mainScreenId, ScreenId extendScreenId)
{
    auto screenSession = GetScreenSession(mainScreenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    WLOGI("mainScreenId=%{public}" PRIu64" extendScreenId=%{public}" PRIu64, mainScreenId, extendScreenId);
    screenSession->ScreenExtendChange(mainScreenId, extendScreenId);
}

sptr<ScreenSession> ScreenSessionManagerClient::GetScreenSession(ScreenId screenId) const
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        WLOGFE("Error found screen session with id: %{public}" PRIu64, screenId);
        return nullptr;
    }
    return iter->second;
}

void ScreenSessionManagerClient::OnPropertyChanged(ScreenId screenId,
    const ScreenProperty& property, ScreenPropertyChangeReason reason)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->PropertyChange(property, reason);
}

void ScreenSessionManagerClient::OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    if (screenSessionMap_.empty()) {
        WLOGFE("[UL_POWER]screenSessionMap_ is nullptr");
        return;
    }
    auto screenSession = screenSessionMap_.begin()->second;
    if (!screenSession) {
        WLOGFE("[UL_POWER]screenSession is null");
        return;
    }
    screenSession->PowerStatusChange(event, status, reason);
}

void ScreenSessionManagerClient::OnSensorRotationChanged(ScreenId screenId, float sensorRotation)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->SensorRotationChange(sensorRotation);
}

void ScreenSessionManagerClient::OnHoverStatusChanged(ScreenId screenId, int32_t hoverStatus, bool needRotate)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->HandleHoverStatusChange(hoverStatus, needRotate);
}

void ScreenSessionManagerClient::OnScreenOrientationChanged(ScreenId screenId, float screenOrientation)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->ScreenOrientationChange(screenOrientation);
}

void ScreenSessionManagerClient::OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->SetScreenRotationLocked(isLocked);
}

void ScreenSessionManagerClient::RegisterDisplayChangeListener(const sptr<IDisplayChangeListener>& listener)
{
    displayChangeListener_ = listener;
}

void ScreenSessionManagerClient::RegisterSwitchingToAnotherUserFunction(std::function<void()>&& func)
{
    switchingToAnotherUserFunc_ = func;
}

void ScreenSessionManagerClient::OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    }
}

void ScreenSessionManagerClient::OnUpdateFoldDisplayMode(FoldDisplayMode displayMode)
{
    displayMode_ = displayMode;
}

void ScreenSessionManagerClient::OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds, bool isBlackList)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnGetSurfaceNodeIdsFromMissionIds(missionIds, surfaceNodeIds, isBlackList);
    }
}

void ScreenSessionManagerClient::OnScreenshot(DisplayId displayId)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnScreenshot(displayId);
    }
}

void ScreenSessionManagerClient::OnImmersiveStateChanged(ScreenId screenId, bool& immersive)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnImmersiveStateChange(screenId, immersive);
    }
}

std::map<ScreenId, ScreenProperty> ScreenSessionManagerClient::GetAllScreensProperties() const
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    std::map<ScreenId, ScreenProperty> screensProperties;
    for (const auto& iter: screenSessionMap_) {
        if (iter.second == nullptr) {
            continue;
        }
        screensProperties[iter.first] = iter.second->GetScreenProperty();
    }
    return screensProperties;
}

FoldDisplayMode ScreenSessionManagerClient::GetFoldDisplayMode() const
{
    return displayMode_;
}

void ScreenSessionManagerClient::UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds,
    ScreenDirectionInfo directionInfo, ScreenPropertyChangeType screenPropertyChangeType)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->UpdateScreenDirectionInfo(screenId, directionInfo.screenRotation_, directionInfo.rotation_,
        directionInfo.phyRotation_, screenPropertyChangeType);
    screenSessionManager_->UpdateScreenRotationProperty(screenId, bounds, directionInfo.notifyRotation_,
        screenPropertyChangeType);

    // not need update property to input manager
    if (screenPropertyChangeType == ScreenPropertyChangeType::ROTATION_END ||
        screenPropertyChangeType == ScreenPropertyChangeType::ROTATION_UPDATE_PROPERTY_ONLY) {
        return;
    }
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    auto foldDisplayMode = screenSessionManager_->GetFoldDisplayMode();
    screenSession->SetPhysicalRotation(directionInfo.phyRotation_);
    screenSession->SetScreenComponentRotation(directionInfo.screenRotation_);
    screenSession->UpdateToInputManager(bounds, directionInfo.notifyRotation_, directionInfo.rotation_,
        foldDisplayMode);
    screenSession->UpdateTouchBoundsAndOffset(foldDisplayMode);
}

void ScreenSessionManagerClient::SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->SetDisplayNodeScreenId(displayNodeScreenId);
}

uint32_t ScreenSessionManagerClient::GetCurvedCompressionArea()
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return 0;
    }
    return screenSessionManager_->GetCurvedCompressionArea();
}

ScreenProperty ScreenSessionManagerClient::GetPhyScreenProperty(ScreenId screenId)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return {};
    }
    return screenSessionManager_->GetPhyScreenProperty(screenId);
}

__attribute__((no_sanitize("cfi"))) void ScreenSessionManagerClient::NotifyDisplayChangeInfoChanged(
    const sptr<DisplayChangeInfo>& info)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->NotifyDisplayChangeInfoChanged(info);
}

void ScreenSessionManagerClient::SetScreenPrivacyState(bool hasPrivate)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    WLOGFD("Begin calling the SetScreenPrivacyState() of screenSessionManager_, hasPrivate: %{public}d", hasPrivate);
    screenSessionManager_->SetScreenPrivacyState(hasPrivate);
    WLOGFD("End calling the SetScreenPrivacyState() of screenSessionManager_");
}

void ScreenSessionManagerClient::SetPrivacyStateByDisplayId(DisplayId id, bool hasPrivate)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    WLOGFD("Begin calling the SetPrivacyStateByDisplayId, hasPrivate: %{public}d", hasPrivate);
    screenSessionManager_->SetPrivacyStateByDisplayId(id, hasPrivate);
    WLOGFD("End calling the SetPrivacyStateByDisplayId");
}

void ScreenSessionManagerClient::SetScreenPrivacyWindowList(DisplayId id, std::vector<std::string> privacyWindowList)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    WLOGFD("Begin calling the SetScreenPrivacyWindowList(), id: %{public}" PRIu64, id);
    screenSessionManager_->SetScreenPrivacyWindowList(id, privacyWindowList);
    WLOGFD("End calling the SetScreenPrivacyWindowList()");
}

void ScreenSessionManagerClient::UpdateAvailableArea(ScreenId screenId, DMRect area)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->UpdateAvailableArea(screenId, area);
}

int32_t ScreenSessionManagerClient::SetScreenOffDelayTime(int32_t delay)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return 0;
    }
    return screenSessionManager_->SetScreenOffDelayTime(delay);
}

int32_t ScreenSessionManagerClient::SetScreenOnDelayTime(int32_t delay)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return 0;
    }
    return screenSessionManager_->SetScreenOnDelayTime(delay);
}

void ScreenSessionManagerClient::SetCameraStatus(int32_t cameraStatus, int32_t cameraPosition)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    return screenSessionManager_->SetCameraStatus(cameraStatus, cameraPosition);
}

void ScreenSessionManagerClient::NotifyFoldToExpandCompletion(bool foldToExpand)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->NotifyFoldToExpandCompletion(foldToExpand);
}

void ScreenSessionManagerClient::SwitchUserCallback(std::vector<int32_t> oldScbPids, int32_t currentScbPid)
{
    if (screenSessionManager_ == nullptr) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    if (oldScbPids.size() == 0) {
        WLOGFE("oldScbPids size 0");
        return;
    }
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    for (const auto& iter : screenSessionMap_) {
        auto displayNode = screenSessionManager_->GetDisplayNode(iter.first);
        if (displayNode == nullptr) {
            WLOGFE("display node is null");
            continue;
        }
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            displayNode->SetScbNodePid(oldScbPids, currentScbPid);
            transactionProxy->FlushImplicitTransaction();
        } else {
            displayNode->SetScbNodePid(oldScbPids, currentScbPid);
            WLOGFW("transactionProxy is null");
        }
        ScreenId screenId = iter.first;
        sptr<ScreenSession> screenSession = iter.second;
        if (screenSession == nullptr) {
            WLOGFE("screenSession is null");
            return;
        }
        ScreenProperty screenProperty = screenSession->GetScreenProperty();
        RRect bounds = screenProperty.GetBounds();
        float rotation = screenSession->ConvertRotationToFloat(screenSession->GetRotation());
        screenSessionManager_->UpdateScreenRotationProperty(screenId, bounds, rotation,
            ScreenPropertyChangeType::ROTATION_UPDATE_PROPERTY_ONLY);
    }
    WLOGFI("switch user callback end");
}

void ScreenSessionManagerClient::SwitchingCurrentUser()
{
    if (screenSessionManager_ == nullptr) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->SwitchUser();
    WLOGFI("switch to current user end");
}

FoldStatus ScreenSessionManagerClient::GetFoldStatus()
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return FoldStatus::UNKNOWN;
    }
    return screenSessionManager_->GetFoldStatus();
}

SuperFoldStatus ScreenSessionManagerClient::GetSuperFoldStatus()
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return SuperFoldStatus::UNKNOWN;
    }
    return screenSessionManager_->GetSuperFoldStatus();
}

std::shared_ptr<Media::PixelMap> ScreenSessionManagerClient::GetScreenSnapshot(ScreenId screenId,
    float scaleX, float scaleY)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("get screen session is null");
        return nullptr;
    }
    return screenSession->GetScreenSnapshot(scaleX, scaleY);
}

DeviceScreenConfig ScreenSessionManagerClient::GetDeviceScreenConfig()
{
    if (!screenSessionManager_) {
        TLOGE(WmsLogTag::DMS, "screenSessionManager_ is null");
        return {};
    }
    return screenSessionManager_->GetDeviceScreenConfig();
}

sptr<ScreenSession> ScreenSessionManagerClient::GetScreenSessionById(const ScreenId id)
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    auto iter = screenSessionMap_.find(id);
    if (iter == screenSessionMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

ScreenId ScreenSessionManagerClient::GetDefaultScreenId()
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    auto iter = screenSessionMap_.begin();
    if (iter != screenSessionMap_.end()) {
        return iter->first;
    }
    return SCREEN_ID_INVALID;
}

bool ScreenSessionManagerClient::IsFoldable()
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return false;
    }
    return screenSessionManager_->IsFoldable();
}

void ScreenSessionManagerClient::SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio)
{
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screen session is null");
        return;
    }
    if (screenSession->isScreenGroup_) {
        WLOGFE("cannot set virtual pixel ratio to the combination. screen: %{public}" PRIu64, screenId);
        return;
    }
    screenSession->SetScreenSceneDpi(virtualPixelRatio);
}

void ScreenSessionManagerClient::UpdateDisplayHookInfo(int32_t uid, bool enable, const DMHookInfo& hookInfo)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->UpdateDisplayHookInfo(uid, enable, hookInfo);
}

void ScreenSessionManagerClient::OnFoldStatusChangedReportUE(const std::vector<std::string>& screenFoldInfo)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnScreenFoldStatusChanged(screenFoldInfo);
    }
}

void ScreenSessionManagerClient::UpdateDisplayScale(ScreenId id, float scaleX, float scaleY, float pivotX, float pivotY,
                                                    float translateX, float translateY)
{
    auto session = GetScreenSession(id);
    if (session == nullptr) {
        TLOGE(WmsLogTag::DMS, "session is null");
        return;
    }
    auto displayNode = session->GetDisplayNode();
    if (displayNode == nullptr) {
        TLOGE(WmsLogTag::DMS, "displayNode is null");
        return;
    }
    TLOGD(WmsLogTag::DMS, "scale [%{public}f, %{public}f] translate [%{public}f, %{public}f]", scaleX, scaleY,
          translateX, translateY);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
                      "ssmc:UpdateDisplayScale(ScreenId = %" PRIu64
                      " scaleX=%f, scaleY=%f, pivotX=%f, pivotY=%f, translateX=%f, translateY=%f",
                      id, scaleX, scaleY, pivotX, pivotY, translateX, translateY);
    session->SetScreenScale(scaleX, scaleY, pivotX, pivotY, translateX, translateY);
    session->PropertyChange(session->GetScreenProperty(), ScreenPropertyChangeReason::ACCESS_INFO_CHANGE);
}

void ScreenSessionManagerClient::ScreenCaptureNotify(ScreenId mainScreenId, int32_t uid, const std::string& clientName)
{
    sptr<ScreenSession> screenSession = GetScreenSession(mainScreenId);
    if (!screenSession) {
        WLOGFE("screen session is null");
        return;
    }
    WLOGFI("capture screenId: %{public}" PRIu64", uid=%{public}d", mainScreenId, uid);
    screenSession->ScreenCaptureNotify(mainScreenId, uid, clientName);
}

void ScreenSessionManagerClient::OnSuperFoldStatusChanged(ScreenId screenId, SuperFoldStatus superFoldStatus)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    WLOGI("screenId=%{public}" PRIu64 " superFoldStatus=%{public}d", screenId,
        static_cast<uint32_t>(superFoldStatus));
    screenSession->SuperFoldStatusChange(screenId, superFoldStatus);
}

void ScreenSessionManagerClient::OnSecondaryReflexionChanged(ScreenId screenId, uint32_t isSecondaryReflexion)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    WLOGI("screenId=%{public}" PRIu64 " isSecondaryReflexion=%{public}d", screenId, isSecondaryReflexion);
    screenSession->SecondaryReflexionChange(screenId, isSecondaryReflexion);
}
} // namespace OHOS::Rosen