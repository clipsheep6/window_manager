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

#ifndef OHOS_ROSEN_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_SCENE_SESSION_MANAGER_H

#include <memory>
#include <vector>
#include <map>
#include <unistd.h>

#include <native_engine/native_engine.h>
#include "ws_single_instance.h"
#include "window_scene_common.h"
#include "scene_session.h"

namespace OHOS::Rosen {
class SceneSessionManager {
WS_DECLARE_SINGLE_INSTANCE(SceneSessionManager)
public:
    sptr<SceneSession> RequestSceneSession(const AbilityInfo& abilityInfo, SessionOption option, NativeEngine& engine);
    WSError RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession);

    sptr<SceneSession> FindSceneSession(NativeValue* jsSceneSessionObj);

private:
    uint32_t GenSessionId();

    // static inline SingletonDelegator<SceneSessionManager> delegator_;
    std::recursive_mutex mutex_;
    std::vector<sptr<SceneSession>> sessions_;
    int pid_ = getpid();
    std::atomic<uint32_t> sessionId_ = INVALID_SESSION_ID;

    std::map<std::shared_ptr<NativeReference>, sptr<SceneSession>> jsSceneSessionMap_;
};
}
#endif // OHOS_ROSEN_SCENE_SESSION_MANAGER_H
