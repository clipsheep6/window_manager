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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H

#include <iremote_proxy.h>
#include <refbase.h>
#include "session/host/include/zidl/session_interface.h"
#include "wm_common.h"

namespace OHOS::Rosen {
class SessionProxy : public IRemoteProxy<ISession> {
public:
    explicit SessionProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISession>(impl) {};
    ~SessionProxy() {};

    WMError Foreground() override;
    WMError Background() override;
    WMError Disconnect() override;
    WMError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint64_t& persistentId,
        sptr<WindowSessionProperty> property = nullptr) override;

    WMError UpdateActiveStatus(bool isActive) override;
    WMError PendingSessionActivation(const SessionInfo& info) override;
    WMError Recover() override;
    WMError Maximize() override;
private:
    static inline BrokerDelegator<SessionProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H
