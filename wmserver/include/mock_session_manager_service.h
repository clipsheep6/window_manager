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

#ifndef OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H
#define OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H

#include <system_ability.h>

#include "wm_single_instance.h"
#include "zidl/mock_session_manager_service_stub.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class MockSessionManagerService : public SystemAbility, public MockSessionManagerServiceStub {
DECLARE_SYSTEM_ABILITY(MockSessionManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(MockSessionManagerService);
public:
    bool SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService);
    sptr<IRemoteObject> GetSessionManagerService() override;
    sptr<IRemoteObject> GetSceneSessionManager();
    void OnStart() override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;    

protected:
    MockSessionManagerService();
    virtual ~MockSessionManagerService() = default;

private:
    bool RegisterMockSessionManagerService();
    int DumpWindowInfo(const std::vector<std::string>& args, std::string& dumpInfo);
    int DumpAllWindowInfo(std::string& dumpInfo);
    void ShowHelpInfo(std::string& dumpInfo);
    void ShowAceDumpHelp(std::string& dumpInfo);
    void ShowIllegalArgsInfo(std::string& dumpInfo);

    class SMSDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject>& object) override;
    };

    sptr<IRemoteObject> sessionManagerService_;
    sptr<IRemoteObject> sceneSessionManager_;
    sptr<SMSDeathRecipient> smsDeathRecipient_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_MOCK_SESSION_MANAGER_SERVICE_H
