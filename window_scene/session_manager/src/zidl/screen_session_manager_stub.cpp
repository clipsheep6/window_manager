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

#include "zidl/screen_session_manager_stub.h"

#include <ipc_skeleton.h>

#include "marshalling_helper.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerStub"};
}

int32_t ScreenSessionManagerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    DisplayManagerMessage msgId = static_cast<DisplayManagerMessage>(code);
    switch (msgId) {
        case DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO: {
            auto info = GetDefaultDisplayInfo();
            reply.WriteParcelable(info);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            auto screenInfo = GetScreenInfoById(screenId);
            reply.WriteStrongParcelable(screenInfo);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            uint32_t modeId = data.ReadUint32();
            DMError ret = SetScreenActiveMode(screenId, modeId);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            float virtualPixelRatio = data.ReadFloat();
            DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            std::vector<ScreenColorGamut> colorGamuts;
            DMError ret = GetScreenSupportedColorGamuts(screenId, colorGamuts);
            reply.WriteInt32(static_cast<int32_t>(ret));
            if (ret != DMError::DM_OK) {
                break;
            }
            MarshallingHelper::MarshallingVectorObj<ScreenColorGamut>(reply, colorGamuts,
                [](Parcel& parcel, const ScreenColorGamut& color) {
                    return parcel.WriteUint32(static_cast<uint32_t>(color));
                }
            );
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            ScreenColorGamut colorGamut;
            DMError ret = GetScreenColorGamut(screenId, colorGamut);
            reply.WriteInt32(static_cast<int32_t>(ret));
            if (ret != DMError::DM_OK) {
                break;
            }
            reply.WriteUint32(static_cast<uint32_t>(colorGamut));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            int32_t colorGamutIdx = data.ReadInt32();
            DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            ScreenGamutMap gamutMap;
            DMError ret = GetScreenGamutMap(screenId, gamutMap);
            reply.WriteInt32(static_cast<int32_t>(ret));
            if (ret != DMError::DM_OK) {
                break;
            }
            reply.WriteInt32(static_cast<uint32_t>(gamutMap));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
            DMError ret = SetScreenGamutMap(screenId, gamutMap);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            DMError ret = SetScreenColorTransform(screenId);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT: {
            auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
            auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
            DMError ret = RegisterDisplayManagerAgent(agent, type);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT: {
            auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
            auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
            DMError ret = UnregisterDisplayManagerAgent(agent, type);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN: {
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(WakeUpBegin(reason));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_WAKE_UP_END: {
            reply.WriteBool(WakeUpEnd());
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN: {
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(SuspendBegin(reason));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SUSPEND_END: {
            reply.WriteBool(SuspendEnd());
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE: {
            DisplayState state = static_cast<DisplayState>(data.ReadUint32());
            reply.WriteBool(SetDisplayState(state));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL: {
            ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(SetScreenPowerForAll(state, reason));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE: {
            DisplayState state = GetDisplayState(data.ReadUint64());
            reply.WriteUint32(static_cast<uint32_t>(state));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT: {
            DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
            NotifyDisplayEvent(event);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER: {
            ScreenId dmsScreenId;
            if (!data.ReadUint64(dmsScreenId)) {
                WLOGFE("fail to read dmsScreenId.");
                break;
            }
            reply.WriteUint32(static_cast<uint32_t>(GetScreenPower(dmsScreenId)));
            break;
        }
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace OHOS::Rosen
