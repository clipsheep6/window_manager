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

#include "session/host/include/zidl/session_stub.h"

#include <ipc_types.h>
#include <ui/rs_surface_node.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStub"};
}

const std::map<uint32_t, SessionStubFunc> SessionStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_FOREGROUND), &SessionStub::HandleForeground),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_BACKGROUND), &SessionStub::HandleBackground),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_DISCONNECT), &SessionStub::HandleDisconnect),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_CONNECT), &SessionStub::HandleConnect),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_ACTIVE_PENDING_SESSION),
        &SessionStub::HandlePendingSessionActivation),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_UPDATE_ACTIVE_STATUS),
        &SessionStub::HandleUpdateActivateStatus),

    // for scene only
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_SESSION_EVENT), &SessionStub::HandleSessionEvent),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_TERMINATE), &SessionStub::HandleTerminateSession),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_UPDATE_SESSION_RECT),
        &SessionStub::HandleUpdateSessionRect),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION),
        &SessionStub::HandleCreateAndConnectSpecificSession),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION),
        &SessionStub::HandleDestroyAndDisconnectSpecificSession),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_RAISE_TO_APP_TOP),
        &SessionStub::HandleRaiseToAppTop),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_BACKPRESSED), &SessionStub::HandleBackPressed)
};

int SessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto& func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(func->second))(data, reply);
}

int SessionStub::HandleForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Foreground!");
    const WSError& errCode = Foreground();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Background!");
    const WSError& errCode = Background();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Disconnect!");
    const WSError& errCode = Disconnect();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Connect!");
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
    SystemSessionConfig systemConfig;
    WSError errCode = Connect(sessionStage, eventChannel, surfaceNode, systemConfig, property);
    reply.WriteParcelable(&systemConfig);
    reply.WriteUint64(property->GetPersistentId());
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSessionEvent(MessageParcel& data, MessageParcel& reply)
{
    uint32_t eventId = data.ReadUint32();
    WLOGFD("HandleSessionEvent eventId: %{public}d", eventId);
    WSError errCode = OnSessionEvent(static_cast<SessionEvent>(eventId));
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleTerminateSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("mdquan run HandleTerminateSession");
    sptr<AAFwk::SessionInfo> abilitySessionInfo(new AAFwk::SessionInfo());
    std::unique_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    abilitySessionInfo->want = *want;
    abilitySessionInfo->callerToken = data.ReadRemoteObject();
    abilitySessionInfo->resultCode = data.ReadInt32();

    const WSError& errCode = TerminateSession(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandlePendingSessionActivation(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("PendingSessionActivation!");
    sptr<AAFwk::SessionInfo> abilitySessionInfo(new AAFwk::SessionInfo());
    std::unique_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    abilitySessionInfo->want = *want;
    abilitySessionInfo->callerToken = data.ReadRemoteObject();
    abilitySessionInfo->requestCode = data.ReadInt32();

    const WSError& errCode = PendingSessionActivation(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateActivateStatus(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateActivateStatus!");
    bool isActive = data.ReadBool();
    const WSError& errCode = UpdateActiveStatus(isActive);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateSessionRect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateSessionRect!");
    auto posX = data.ReadInt32();
    auto posY = data.ReadInt32();
    auto width = data.ReadUint32();
    auto height = data.ReadUint32();
    WSRect rect = {posX, posY, width, height};
    WLOGFI("HandleUpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", posX, posY,
        width, height);
    const SizeChangeReason& reason = static_cast<SizeChangeReason>(data.ReadUint32());
    const WSError& errCode = UpdateSessionRect(rect, reason);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleCreateAndConnectSpecificSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleCreateAndConnectSpecificSession!");
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
    return ERR_NONE;
}

int SessionStub::HandleDestroyAndDisconnectSpecificSession(MessageParcel& data, MessageParcel& reply)
{
    uint64_t persistentId = data.ReadUint64();
    const WSError& ret = DestroyAndDisconnectSpecificSession(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SessionStub::HandleRaiseToAppTop(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseToAppTop!");
    const WSError& errCode = RaiseToAppTop();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackPressed(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleBackPressed!");
    WSError errCode = RequestSessionBack();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
