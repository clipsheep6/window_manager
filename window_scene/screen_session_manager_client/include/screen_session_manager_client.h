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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_H

#include <map>
#include <mutex>

#include <common/rs_rect.h>

#include "display_change_listener.h"
#include "display_change_info.h"
#include "dm_common.h"
#include "session/screen/include/screen_session.h"
#include "interfaces/include/ws_common.h"
#include "wm_single_instance.h"
#include "zidl/screen_session_manager_client_stub.h"
#include "zidl/screen_session_manager_interface.h"

namespace OHOS::Rosen {
using ScreenInfoChangeClientListener = std::function<void(uint64_t)>;

class IScreenConnectionListener {
public:
    virtual void OnScreenConnected(const sptr<ScreenSession>& screenSession) = 0;
    virtual void OnScreenDisconnected(const sptr<ScreenSession>& screenSession) = 0;
};

class IScreenConnectionChangeListener : public RefBase {
public:
    virtual void OnScreenConnected(const sptr<ScreenSession>& screenSession) = 0;
    virtual void OnScreenDisconnected(const sptr<ScreenSession>& screenSession) = 0;
};

class ScreenSessionManagerClient : public ScreenSessionManagerClientStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenSessionManagerClient)

public:
    void RegisterScreenConnectionListener(IScreenConnectionListener* listener);
    void RegisterDisplayChangeListener(const sptr<IDisplayChangeListener>& listener);
    void RegisterScreenConnectionChangeListener(const sptr<IScreenConnectionChangeListener>& listener);

    sptr<ScreenSession> GetScreenSession(ScreenId screenId) const;
    std::map<ScreenId, ScreenProperty> GetAllScreensProperties() const;
    FoldDisplayMode GetFoldDisplayMode() const;

    void UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, ScreenDirectionInfo directionInfo,
        ScreenPropertyChangeType screenPropertyChangeType);
    uint32_t GetCurvedCompressionArea();
    ScreenProperty GetPhyScreenProperty(ScreenId screenId);
    void SetScreenPrivacyState(bool hasPrivate);
    void SetPrivacyStateByDisplayId(DisplayId id, bool hasPrivate);
    void SetScreenPrivacyWindowList(DisplayId id, std::vector<std::string> privacyWindowList);
    void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info);
    void OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    void OnScreenshot(DisplayId displayId) override;
    void OnImmersiveStateChanged(ScreenId screenId, bool& immersive) override;
    void OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds, bool isBlackList = false) override;
    void OnUpdateFoldDisplayMode(FoldDisplayMode displayMode) override;
    void UpdateAvailableArea(ScreenId screenId, DMRect area);
    void UpdateSuperFoldAvailableArea(ScreenId screenId, DMRect bArea, DMRect cArea);
    int32_t SetScreenOffDelayTime(int32_t delay);
    int32_t SetScreenOnDelayTime(int32_t delay);
    void SetCameraStatus(int32_t cameraStatus, int32_t cameraPosition);
    void NotifyFoldToExpandCompletion(bool foldToExpand);
    FoldStatus GetFoldStatus();
    SuperFoldStatus GetSuperFoldStatus();
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(ScreenId screenId, float scaleX, float scaleY);
    DeviceScreenConfig GetDeviceScreenConfig();
    sptr<ScreenSession> GetScreenSessionById(const ScreenId id);
    ScreenId GetDefaultScreenId();
    bool IsFoldable();
    void SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio) override;
    void UpdateDisplayHookInfo(int32_t uid, bool enable, const DMHookInfo& hookInfo);

    void RegisterSwitchingToAnotherUserFunction(std::function<void()>&& func);
    void SwitchingCurrentUser();
    void SwitchUserCallback(std::vector<int32_t> oldScbPids, int32_t currentScbPid) override;

    void OnFoldStatusChangedReportUE(const std::vector<std::string>& screenFoldInfo) override;

    void UpdateDisplayScale(ScreenId id, float scaleX, float scaleY, float pivotX, float pivotY, float translateX,
                            float translateY);
protected:
    ScreenSessionManagerClient() = default;
    virtual ~ScreenSessionManagerClient() = default;

private:
    void ConnectToServer();
    bool CheckIfNeedConnectScreen(ScreenId screenId, ScreenId rsId, const std::string& name);
    void OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
        ScreenId rsId, const std::string& name, bool isExtend) override;
    void OnPropertyChanged(ScreenId screenId,
        const ScreenProperty& property, ScreenPropertyChangeReason reason) override;
    void OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override;
    void OnSensorRotationChanged(ScreenId screenId, float sensorRotation) override;
    void OnHoverStatusChanged(ScreenId screenId, int32_t hoverStatus, bool needRotate = true) override;
    void OnScreenOrientationChanged(ScreenId screenId, float screenOrientation) override;
    void OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked) override;
    void OnScreenExtendChanged(ScreenId mainScreenId, ScreenId extendScreenId) override;
    void OnSuperFoldStatusChanged(ScreenId screenId, SuperFoldStatus superFoldStatus) override;
    void OnSecondaryReflexionChanged(ScreenId screenId, bool isSecondaryReflexion) override;

    void SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId) override;
    void ScreenCaptureNotify(ScreenId mainScreenId, int32_t uid, const std::string& clientName) override;

    void NotifyScreenConnect(const sptr<ScreenSession>& screenSession);
    void NotifyScreenDisconnect(const sptr<ScreenSession>& screenSession);

    mutable std::mutex screenSessionMapMutex_;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_;
    std::function<void()> switchingToAnotherUserFunc_ = nullptr;

    sptr<IScreenSessionManager> screenSessionManager_;

    IScreenConnectionListener* screenConnectionListener_;
    sptr<IScreenConnectionChangeListener> screenConnectionChangeListener_;
    sptr<IDisplayChangeListener> displayChangeListener_;
    FoldDisplayMode displayMode_ = FoldDisplayMode::UNKNOWN;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_STUB_H
