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

#include "interfaces/include/ws_common.h"
#include "session/host/include/session.h"

namespace OHOS::Rosen {
class SceneSession;
using SpecificSessionCreateCallback = std::function<sptr<SceneSession>(const SessionInfo& info)>;
using SpecificSessionDestroyCallback = std::function<WSError(const uint64_t& persistentId)>;
using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionRectChangeFunc = std::function<void(const WSRect& rect)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId)>;
using NotifyRaiseToTopFunc = std::function<void()>;
class SceneSession : public Session {
public:
    // callback for notify SceneSessionManager
    struct SpecificSessionCallback : public RefBase {
        SpecificSessionCreateCallback onCreate_;
        SpecificSessionDestroyCallback onDestroy_;
    };

    // callback for notify SceneBoard
    struct SessionChangeCallback : public RefBase {
        NotifyCreateSpecificSessionFunc onCreateSpecificSession_;
        NotifySessionRectChangeFunc onRectChange_;
        NotifyRaiseToTopFunc onRaiseToTop_;
        NotifySessionEventFunc OnSessionEvent_;
    };

    SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback, std::map<uint64_t, sptr<Session>>& sessionMap);
    ~SceneSession() = default;

    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session) override;
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId) override;
    void RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>& sessionChangeCallback);

private:
    sptr<SpecificSessionCallback> specificCallback_ = nullptr;
    sptr<SessionChangeCallback> sessionChangeCallback_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
