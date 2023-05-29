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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_PROXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_PROXY_H

#include <iremote_proxy.h>
#include <refbase.h>
#include "session_manager/include/zidl/scene_session_manager_interface.h"

namespace OHOS::Rosen {
class SceneSessionManagerProxy : public IRemoteProxy<ISceneSessionManager> {
public:
    explicit SceneSessionManagerProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ISceneSessionManager>(impl) {};

    ~SceneSessionManagerProxy() {};

    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session) override;
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId) override;
    WSError BindDialogTarget(const uint64_t& persistentId, sptr<IRemoteObject> targetToken) override;
    WSError BindDialogToParent(const uint64_t& persistentId) override;

private:
    static inline BrokerDelegator<SceneSessionManagerProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_PROXY_H
