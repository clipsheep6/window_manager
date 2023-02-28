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

#include "zidl/session_stub.h"
#include <ipc_types.h>
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
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_START_ABILITY),
        &SessionStub::HandleStartAbility),

    // for scene only
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_RECOVER), &SessionStub::HandleRecover),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_MAXIMUM), &SessionStub::HandleMaximum)
};

int SessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
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

int SessionStub::HandleForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Foreground!");
    WSError errCode = Foreground();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Background!");
    WSError errCode = Background();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Disconnect!");
    WSError errCode = Disconnect();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Connect!");
    sptr<IRemoteObject> sessionStageObject = data.ReadRemoteObject();
    sptr<ISessionStage> sessionStageProxy = iface_cast<ISessionStage>(sessionStageObject);
    WSError errCode = Connect(sessionStageProxy);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRecover(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Recover!");
    WSError errCode = Recover();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleMaximum(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Maximum!");
    WSError errCode = Maximum();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleStartAbility(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("StartAbility!");
    AbilityInfo info;
    info.bundleName_ = data.ReadString();
    info.abilityName_ = data.ReadString();
    if (data.ReadBool()) {
        info.callerToken_ = data.ReadRemoteObject();
    }
    info.isExtension_ = data.ReadBool();
    info.extensionType_ = data.ReadUint32();
    WSError errCode = StartAbility(info);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
}
