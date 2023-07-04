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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H

#include "session/host/include/session.h"
#include "session/host/include/move_drag_controller.h"

namespace OHOS::PowerMgr {
    class RunningLock;
}

namespace OHOS::Rosen {
class SceneSession;

using SpecificSessionCreateCallback = std::function<sptr<SceneSession>(const SessionInfo& info, sptr<WindowSessionProperty> property)>;
using SpecificSessionDestroyCallback = std::function<WSError(const uint64_t& persistentId)>;
using CameraFloatSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionRectChangeFunc = std::function<void(const WSRect& rect)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId)>;
using NotifyRaiseToTopFunc = std::function<void()>;
using NotifySystemBarPropertyChangeFunc = std::function<void(
    const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)>;
using NotifyNeedAvoidFunc = std::function<void(bool status)>;
class SceneSession : public Session {
public:
    // callback for notify SceneSessionManager
    struct SpecificSessionCallback : public RefBase {
        SpecificSessionCreateCallback onCreate_;
        SpecificSessionDestroyCallback onDestroy_;
        CameraFloatSessionChangeCallback onCameraFloatSessionChange_;
    };

    // callback for notify SceneBoard
    struct SessionChangeCallback : public RefBase {
        NotifyCreateSpecificSessionFunc onCreateSpecificSession_;
        NotifySessionRectChangeFunc onRectChange_;
        NotifyRaiseToTopFunc onRaiseToTop_;
        NotifySessionEventFunc OnSessionEvent_;
        NotifySystemBarPropertyChangeFunc OnSystemBarPropertyChange_;
        NotifyNeedAvoidFunc OnNeedAvoid_;
    };

    SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    ~SceneSession()
    {
        sessionChangeCallbackList_.clear();
        sessionChangeCallbackList_.shrink_to_fit();
    }

    WSError Foreground() override;
    WSError Background() override;

    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session) override;
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId) override;
    WSError SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty);
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    void RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>& sessionChangeCallback);
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError SetAspectRatio(float ratio) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    std::string GetSessionSnapshot();
    static MaximizeMode maximizeMode_;

    WSError SetTurnScreenOn(bool turnScreenOn);
    bool IsTurnScreenOn() const;
    WSError SetKeepScreenOn(bool keepScreenOn);
    bool IsKeepScreenOn() const;
    const std::string& GetWindowName() const;
    bool IsDecorEnable();

    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;
private:
    void UpdateCameraFloatWindowStatus(bool isShowing);
    void NotifySessionRectChange(const WSRect& rect);
    void ProcessVsyncHandleRegister();
    void OnVsyncHandle();
    bool FixRectByAspectRatio(WSRect& rect);
    bool SaveAspectRatio(float ratio);
    sptr<SpecificSessionCallback> specificCallback_ = nullptr;
    std::vector<sptr<SessionChangeCallback>> sessionChangeCallbackList_;
    sptr<MoveDragController> moveDragController_ = nullptr;
    bool isFirstStart_ = true;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
