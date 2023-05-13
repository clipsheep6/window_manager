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

#include "session_manager/include/zidl/scene_session_manager_stub.h"

#include <ipc_types.h>
#include <ui/rs_surface_node.h>
#include "session/host/include/scene_session.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerStub"};
}

int SceneSessionManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    SceneSessionManagerMessage msgId = static_cast<SceneSessionManagerMessage>(code);
    switch (msgId) {
        case SceneSessionManagerMessage::TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION : {
            sptr<IRemoteObject> sessionStageObject = data.ReadRemoteObject();
            sptr<ISessionStage> sessionStage = iface_cast<ISessionStage>(sessionStageObject);
            sptr<IRemoteObject> eventChannelObject = data.ReadRemoteObject();
            sptr<IWindowEventChannel> eventChannel = iface_cast<IWindowEventChannel>(eventChannelObject);
            std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
            if (sessionStage == nullptr || eventChannel == nullptr || surfaceNode == nullptr) {
                WLOGFE("Failed to read scene session stage object or event channel object!");
                return ERR_INVALID_DATA;
            }

            sptr<WindowSessionProperty> property = nullptr;
            if (data.ReadBool()) {
                property = data.ReadStrongParcelable<WindowSessionProperty>();
            } else {
                WLOGFW("Property not exist!");
            }
            uint64_t persistentId = INVALID_SESSION_ID;
            sptr<ISession> sceneSession;
            CreateAndConnectSpecificSession(sessionStage, eventChannel, surfaceNode,
                property, persistentId, sceneSession);
            if (sceneSession== nullptr) {
                return ERR_INVALID_STATE;
            }
            reply.WriteUint64(persistentId);
            reply.WriteRemoteObject(sceneSession->AsObject());
            reply.WriteUint32(static_cast<uint32_t>(WSError::WS_OK));
            break;
        }
        case SceneSessionManagerMessage::TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION: {
            uint64_t persistentId = data.ReadUint64();
            const WSError& ret = DestroyAndDisconnectSpecificSession(persistentId);
            reply.WriteUint32(static_cast<uint32_t>(ret));
            break;
        }
        default:
            WLOGFE("Unknown session message!");
    }
    return ERR_NONE;
}
} // namespace OHOS::Rosen
