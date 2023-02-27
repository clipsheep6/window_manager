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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H

#include <iremote_object.h>
#include <map>
#include <mutex>
#include <refbase.h>
#include <unistd.h>

#include "interfaces/include/ws_common.h"
#include "session/host/include/root_scene_session.h"
#include "session_manager_base.h"
#include "wm_single_instance.h"
namespace OHOS::Ace::NG {
class UIWindow;
}
namespace OHOS::Rosen {
class SceneSession;
class SceneSessionManager : public SessionManagerBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneSessionManager)
public:
    sptr<SceneSession> RequestSceneSession(const SessionInfo& sessionInfo);
    WSError RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession);

    sptr<RootSceneSession> GetRootSceneSession();
protected:
    SceneSessionManager();
    virtual ~SceneSessionManager() = default;
private:
    void Init();
    std::recursive_mutex mutex_;
    int pid_ = getpid();
    std::atomic<uint32_t> sessionId_ = INVALID_SESSION_ID;
    std::map<uint32_t, std::pair<sptr<SceneSession>, sptr<IRemoteObject>>> abilitySceneMap_;
    sptr<RootSceneSession> rootSceneSession_;
    std::shared_ptr<Ace::NG::UIWindow> rootScene_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
