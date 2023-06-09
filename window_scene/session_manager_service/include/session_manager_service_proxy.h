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

#ifndef FOUNDATION_WINDOW_SCENE_SESSION_MANAGER_SERVICE_PROXY_H
#define FOUNDATION_WINDOW_SCENE_SESSION_MANAGER_SERVICE_PROXY_H

#include "session_manager_service_interface.h"

#include <iremote_proxy.h>

#include "singleton_delegator.h"

namespace OHOS::Rosen {
class SessionManagerServiceProxy : public IRemoteProxy<ISessionManagerService> {
public:
    explicit SessionManagerServiceProxy(const sptr<IRemoteObject>& remoteObject);

    ~SessionManagerServiceProxy() override;

    int GetValueById(int id) override;

    sptr<IRemoteObject> GetSceneSessionManager() override;
    sptr<IRemoteObject> GetScreenSessionManagerService() override;
    sptr<IRemoteObject> GetScreenLockManagerService() override;

private:
    static inline BrokerDelegator<SessionManagerServiceProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_WINDOW_SCENE_SESSION_MANAGER_SERVICE_PROXY_H