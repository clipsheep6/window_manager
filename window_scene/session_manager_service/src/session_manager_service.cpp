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

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "session_manager_service.h"
#include "ability_manager_client.h"
#include "session_manager/include/scene_session_manager.h"
#include "mock_session_manager_service_interface.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerService" };
}

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManagerService)

void SessionManagerService::Init()
{
    AAFwk::AbilityManagerClient::GetInstance()->SetSessionManagerService(this->AsObject());
}

void SessionManagerService::NotifySceneBoardAvailable()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("[RECOVER] Failed to get system ability manager.");
        return;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject) {
        WLOGFE("[RECOVER] Get window manager service failed, remote object is nullptr");
        return;
    }

    auto mockSessionManagerServiceProxy = iface_cast<IMockSessionManagerInterface>(remoteObject);
    if (!mockSessionManagerServiceProxy) {
        WLOGFE("[RECOVER] Get mock session manager service proxy failed, nullptr");
        return;
    }
    WLOGFI("[RECOVER] Get mock session manager ok");
    mockSessionManagerServiceProxy->NotifySceneBoardAvailable();
}

sptr<IRemoteObject> SessionManagerService::GetSceneSessionManager()
{
    if (sceneSessionManagerObj_) {
        return sceneSessionManagerObj_;
    }
    sceneSessionManagerObj_ = SceneSessionManager::GetInstance().AsObject();
    return sceneSessionManagerObj_;
}
} // namesapce OHOS::Rosen
