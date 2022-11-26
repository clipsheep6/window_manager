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

#include "zidl/scene_session_stage_stub.h"
#include <ipc_types.h>
#include "window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionStageStub"};
}

const std::map<uint32_t, SceneSessionStageStubFunc> SceneSessionStageStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SessionStageMessage::TRANS_ID_SET_ACTIVE),
        &SceneSessionStageStub::HandleSetActive)
};

int SceneSessionStageStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session stage on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return ERR_UNKNOWN_TRANSACTION;
    }

    return (this->*(func->second))(data, reply);
}

int SceneSessionStageStub::HandleSetActive(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("SetActive!");
    bool active = data.ReadBool();
    WSError errCode = SetActive(active);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
}
