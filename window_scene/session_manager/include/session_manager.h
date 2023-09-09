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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H

#include <shared_mutex>
#include "screenlock_manager_interface.h"

#include "session_manager_service_interface.h"
#include "mock_session_manager_service_interface.h"
#include "zidl/scene_session_manager_interface.h"
#include "zidl/screen_session_manager_interface.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class SSMDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class SessionManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManager);
public:
    void CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr);
    void DestroyAndDisconnectSpecificSession(const int32_t& persistentId);
    WMError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    WMError SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent);
    WMError BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken);
    void ClearSessionManagerProxy();
    void Clear();

    sptr<ISceneSessionManager> GetSceneSessionManagerProxy();
    sptr<IScreenSessionManager> GetScreenSessionManagerProxy();
    sptr<ScreenLock::ScreenLockManagerInterface> GetScreenLockManagerProxy();

protected:
    SessionManager() = default;
    virtual ~SessionManager();

private:
    void InitSessionManagerServiceProxy();
    void InitSceneSessionManagerProxy();
    void InitScreenSessionManagerProxy();
    void InitScreenLockManagerProxy();

    sptr<IMockSessionManagerInterface> mockSessionManagerServiceProxy_ = nullptr;
    sptr<ISessionManagerService> sessionManagerServiceProxy_ = nullptr;
    sptr<ISceneSessionManager> sceneSessionManagerProxy_ = nullptr;
    sptr<IScreenSessionManager> screenSessionManagerProxy_ = nullptr;
    sptr<ScreenLock::ScreenLockManagerInterface> screenLockManagerProxy_ = nullptr;
    sptr<SSMDeathRecipient> ssmDeath_ = nullptr;
    std::recursive_mutex mutex_;
    bool destroyed_ = false;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H
