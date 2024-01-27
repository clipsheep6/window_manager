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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H

#include <mutex>
#include <vector>

#include <refbase.h>
#include <screen_manager/screen_types.h>
#include <ui/rs_display_node.h>

#include "screen_property.h"
#include "dm_common.h"
#include "display_info.h"
#include "screen.h"
#include "screen_info.h"
#include "screen_group.h"
#include "screen_group_info.h"
#include "event_handler.h"
#include "session_manager/include/screen_rotation_property.h"

namespace OHOS::Rosen {
class IScreenChangeListener {
public:
    virtual void OnConnect(ScreenId screenId) = 0;
    virtual void OnDisconnect(ScreenId screenId) = 0;
    virtual void OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
        ScreenId screenId) = 0;
    virtual void OnPowerStatusChange(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) = 0;
    virtual void OnSensorRotationChange(float sensorRotation, ScreenId screenId) = 0;
    virtual void OnScreenOrientationChange(float screenOrientation, ScreenId screenId) = 0;
    virtual void OnScreenRotationLockedChange(bool isLocked, ScreenId screenId) = 0;
};

enum class ScreenState : int32_t {
    INIT,
    CONNECTION,
    DISCONNECTION,
};

class ScreenSession : public RefBase {
public:
    ScreenSession() = default;
    ScreenSession(ScreenId screenId, ScreenId rsId, const std::string& name,
        const ScreenProperty& property, const std::shared_ptr<RSDisplayNode>& displayNode);
    ScreenSession(ScreenId screenId, const ScreenProperty& property, ScreenId defaultScreenId);
    ScreenSession(ScreenId screenId, const ScreenProperty& property, NodeId nodeId, ScreenId defaultScreenId);
    ScreenSession(const std::string& name, ScreenId smsId, ScreenId rsId, ScreenId defaultScreenId);
    virtual ~ScreenSession() = default;

    void SetDisplayNodeScreenId(ScreenId screenId);
    void RegisterScreenChangeListener(IScreenChangeListener* screenChangeListener);
    void UnregisterScreenChangeListener(IScreenChangeListener* screenChangeListener);

    sptr<DisplayInfo> ConvertToDisplayInfo();
    sptr<ScreenInfo> ConvertToScreenInfo() const;
    sptr<SupportedScreenModes> GetActiveScreenMode() const;
    ScreenSourceMode GetSourceMode() const;
    void SetScreenCombination(ScreenCombination combination);
    ScreenCombination GetScreenCombination() const;

    Orientation GetOrientation() const;
    void SetOrientation(Orientation orientation);
    Rotation GetRotation() const;
    void SetRotation(Rotation rotation);
    void SetScreenRequestedOrientation(Orientation orientation);
    Orientation GetScreenRequestedOrientation() const;
    void SetUpdateToInputManagerCallback(std::function<void(float)> updateToInputManagerCallback);

    void SetVirtualPixelRatio(float virtualPixelRatio);
    void SetScreenType(ScreenType type);

    std::string GetName();
    ScreenId GetScreenId();
    ScreenId GetRSScreenId();
    ScreenProperty GetScreenProperty() const;
    void UpdatePropertyByActiveMode();
    std::shared_ptr<RSDisplayNode> GetDisplayNode() const;
    void ReleaseDisplayNode();

    Rotation CalcRotation(Orientation orientation, FoldDisplayMode foldDisplayMode) const;
    DisplayOrientation CalcDisplayOrientation(Rotation rotation, FoldDisplayMode foldDisplayMode) const;
    void FillScreenInfo(sptr<ScreenInfo> info) const;
    void InitRSDisplayNode(RSDisplayNodeConfig& config, Point& startPoint);

    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts);
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut);
    DMError SetScreenColorGamut(int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap);
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform();

    DMError GetPixelFormat(GraphicPixelFormat& pixelFormat);
    DMError SetPixelFormat(GraphicPixelFormat pixelFormat);
    DMError GetSupportedHDRFormats(std::vector<ScreenHDRFormat>& hdrFormats);
    DMError GetScreenHDRFormat(ScreenHDRFormat& hdrFormat);
    DMError SetScreenHDRFormat(int32_t modeIdx);
    DMError GetSupportedColorSpaces(std::vector<GraphicCM_ColorSpaceType>& colorSpaces);
    DMError GetScreenColorSpace(GraphicCM_ColorSpaceType& colorSpace);
    DMError SetScreenColorSpace(GraphicCM_ColorSpaceType colorSpace);

    void SetSensorRotation(DeviceRotation sensorRotation);
    DeviceRotation GetSensorRotation();
    float ConvertRotationToFloat(Rotation sensorRotation);
    float GetCurrentSensorRotation();

    bool HasPrivateSessionForeground() const;
    void SetPrivateSessionForeground(bool hasPrivate);
    void SetDisplayBoundary(const RectF& rect, const uint32_t& offsetY);
    void SetScreenRotationLocked(bool isLocked);
    void SetScreenRotationLockedFromJs(bool isLocked);
    bool IsScreenRotationLocked();

    void UpdateToInputManager(RRect bounds, int rotation, FoldDisplayMode foldDisplayMode);
    void UpdatePropertyAfterRotation(RRect bounds, int rotation, FoldDisplayMode foldDisplayMode);
    void UpdateAfterFoldExpand(bool foldToExpand);
    void UpdatePropertyByFoldControl(RRect bounds, RRect phyBounds);
    void UpdateRefreshRate(int32_t refreshRate);
    void UpdatePropertyByResolution(uint32_t width, uint32_t height);
    void SetName(std::string name);
    void Resize(uint32_t width, uint32_t height);

    void SetHdrFormats(std::vector<uint32_t>&& hdrFormats);
    void SetColorSpaces(std::vector<uint32_t>&& colorSpaces);

    std::string name_ { "UNKNOW" };
    ScreenId screenId_ {};
    ScreenId rsId_ {};
    ScreenId defaultScreenId_ = SCREEN_ID_INVALID;

    NodeId nodeId_ {};

    int32_t activeIdx_ { 0 };
    std::vector<sptr<SupportedScreenModes>> modes_ = {};

    bool isScreenGroup_ { false };
    ScreenId groupSmsId_ { SCREEN_ID_INVALID };
    ScreenId lastGroupSmsId_ { SCREEN_ID_INVALID };
    bool isScreenLocked_ = true;

    void Connect();
    void Disconnect();
    void PropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason);
    void PowerStatusChange(DisplayPowerEvent event, EventStatus status, PowerStateChangeReason reason);
    // notify scb
    void SensorRotationChange(Rotation sensorRotation);
    void SensorRotationChange(float sensorRotation);
    void ScreenOrientationChange(Orientation orientation, FoldDisplayMode foldDisplayMode);
    void ScreenOrientationChange(float orientation);
    DMRect GetAvailableArea();
    void SetAvailableArea(DMRect area);
    bool UpdateAvailableArea(DMRect area);
    void SetFoldScreen(bool isFold);
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(float scaleX, float scaleY);

private:
    Rotation ConvertIntToRotation(int rotation);
    ScreenProperty property_;
    std::shared_ptr<RSDisplayNode> displayNode_;
    ScreenState screenState_ { ScreenState::INIT };
    std::vector<IScreenChangeListener*> screenChangeListenerList_;
    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
    bool hasPrivateWindowForeground_ = false;
    std::recursive_mutex mutex_;
    std::function<void(float)> updateToInputManagerCallback_ = nullptr;
    bool isFold_ = false;
    std::vector<uint32_t> hdrFormats_;
    std::vector<uint32_t> colorSpaces_;
    DeviceRotation sensorRotation_ = DeviceRotation::INVALID;
    float currentSensorRotation_ { 0.0f };
};

class ScreenSessionGroup : public ScreenSession {
public:
    ScreenSessionGroup(ScreenId smsId, ScreenId rsId, std::string name, ScreenCombination combination);
    ScreenSessionGroup() = delete;
    WM_DISALLOW_COPY_AND_MOVE(ScreenSessionGroup);
    ~ScreenSessionGroup();

    bool AddChild(sptr<ScreenSession>& smsScreen, Point& startPoint);
    bool AddChild(sptr<ScreenSession>& smsScreen, Point& startPoint, sptr<ScreenSession> defaultScreenSession);
    bool AddChildren(std::vector<sptr<ScreenSession>>& smsScreens, std::vector<Point>& startPoints);
    bool RemoveChild(sptr<ScreenSession>& smsScreen);
    bool HasChild(ScreenId childScreen) const;
    std::vector<sptr<ScreenSession>> GetChildren() const;
    std::vector<Point> GetChildrenPosition() const;
    Point GetChildPosition(ScreenId screenId) const;
    size_t GetChildCount() const;
    sptr<ScreenGroupInfo> ConvertToScreenGroupInfo() const;
    ScreenCombination GetScreenCombination() const;

    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
    ScreenId mirrorScreenId_ { SCREEN_ID_INVALID };

private:
    bool GetRSDisplayNodeConfig(sptr<ScreenSession>& screenSession, struct RSDisplayNodeConfig& config,
        sptr<ScreenSession> defaultScreenSession);

    std::map<ScreenId, std::pair<sptr<ScreenSession>, Point>> screenSessionMap_;
};

} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H
