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

#include "session_manager_service_stub.h"

#include "ipc_types.h"
#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerServiceStub" };
}

int SessionManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }

    auto msgId = static_cast<SessionManagerServiceMessage>(code);
    switch (msgId) {
        case SessionManagerServiceMessage::TRANS_ID_GET_SCENE_SESSION_MANAGER: {
            reply.WriteRemoteObject(GetSceneSessionManager());
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_GET_SCREEN_SESSION_MANAGER_SERVICE: {
            reply.WriteRemoteObject(GetScreenSessionManagerService());
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_GET_SCREEN_LOCK_MANAGER_SERVICE: {
            reply.WriteRemoteObject(GetScreenLockManagerService());
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_NOTIFY_WINDOW_INFO_CHANGE: {
            std::vector<sptr<AccessibilityWindowInfo>> infos;
            if (!MarshallingHelper::UnmarshallingVectorParcelableObj<AccessibilityWindowInfo>(data, infos)) {
                WLOGFE("read window infos failed.");
                return IPC_STUB_INVALID_DATA_ERR;
            }
            WindowUpdateType type = static_cast<WindowUpdateType>(data.ReadUint32());
            NotifyWindowInfoChange(infos, type);
            break;
        }
        default: {
            WLOGFW("unknown transaction code.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}
} // namespace OHOS::Rosen
