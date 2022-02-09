/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "display_manager_stub.h"

#include "dm_common.h"

#include <ipc_skeleton.h>

#include "window_manager_hilog.h"

#include "transaction/rs_interfaces.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerStub"};
}

int32_t DisplayManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFI("OnRemoteRequest code is %{public}d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    switch (code) {
        case TRANS_ID_GET_DEFAULT_DISPLAY_ID: {
            DisplayId displayId = GetDefaultDisplayId();
            reply.WriteUint64(displayId);
            break;
        }
        case TRANS_ID_GET_DISPLAY_BY_ID: {
            DisplayId displayId = data.ReadUint64();
            auto info = GetDisplayInfoById(displayId);
            reply.WriteParcelable(&info);
            break;
        }
        case TRANS_ID_CREATE_VIRTUAL_SCREEN: {
            std::string name = data.ReadString();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            float density = data.ReadFloat();
            sptr<IRemoteObject> surfaceObject = data.ReadRemoteObject();
            sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(surfaceObject);
            sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
            int32_t flags = data.ReadInt32();
            VirtualScreenOption option = {
                .name_ = name,
                .width_ = width,
                .height_ = height,
                .density_ = density,
                .surface_ = surface,
                .flags_ = flags
            };
            ScreenId screenId = CreateVirtualScreen(option);
            reply.WriteUint64(static_cast<uint64_t>(screenId));
            break;
        }
        case TRANS_ID_DESTROY_VIRTUAL_SCREEN: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            DMError result = DestroyVirtualScreen(screenId);
            reply.WriteInt32(static_cast<int32_t>(result));
            break;
        }
        case TRANS_ID_GET_DISPLAY_SNAPSHOT: {
            DisplayId displayId = data.ReadUint64();
            std::shared_ptr<Media::PixelMap> dispalySnapshot = GetDispalySnapshot(displayId);
            if (dispalySnapshot == nullptr) {
                reply.WriteParcelable(nullptr);
                break;
            }
            reply.WriteParcelable(dispalySnapshot.get());
        }
        case TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT: {
            auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
            auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
            reply.WriteBool(RegisterDisplayManagerAgent(agent, type));
            break;
        }
        case TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT: {
            auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
            auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
            reply.WriteBool(UnregisterDisplayManagerAgent(agent, type));
            break;
        }
        case TRANS_ID_WAKE_UP_BEGIN: {
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(WakeUpBegin(reason));
            break;
        }
        case TRANS_ID_WAKE_UP_END: {
            reply.WriteBool(WakeUpEnd());
            break;
        }
        case TRANS_ID_SUSPEND_BEGIN: {
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(SuspendBegin(reason));
            break;
        }
        case TRANS_ID_SUSPEND_END: {
            reply.WriteBool(SuspendEnd());
            break;
        }
        case TRANS_ID_SET_SCREEN_POWER_FOR_ALL: {
            DisplayPowerState state = static_cast<DisplayPowerState>(data.ReadUint32());
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(SetScreenPowerForAll(state, reason));
            break;
        }
        case TRANS_ID_SET_DISPLAY_STATE: {
            DisplayState state = static_cast<DisplayState>(data.ReadUint32());
            reply.WriteBool(SetDisplayState(state));
            break;
        }
        case TRANS_ID_GET_DISPLAY_STATE: {
            DisplayState state = GetDisplayState(data.ReadUint64());
            reply.WriteUint32(static_cast<uint32_t>(state));
            break;
        }
        case TRANS_ID_NOTIFY_DISPLAY_EVENT: {
            DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
            NotifyDisplayEvent(event);
            break;
        }
        case TRANS_ID_SCREEN_MAKE_MIRROR: {
            ScreenId mainScreenId = static_cast<ScreenId>(data.ReadUint64());
            std::vector<ScreenId> mirrorScreenId;
            if (!data.ReadUInt64Vector(&mirrorScreenId)) {
                WLOGE("fail to receive mirror screen in stub. screen:%{public}" PRIu64"", mainScreenId);
                break;
            }
            DMError result = MakeMirror(mainScreenId, mirrorScreenId);
            reply.WriteInt32(static_cast<int32_t>(result));
            break;
        }
        case TRANS_ID_GET_SCREEN_INFO_BY_ID: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            auto screenInfo = GetScreenInfoById(screenId);
            for (int i = 0; i < screenInfo->modes_.size(); i++) {
                WLOGFI("info modes is width: %{public}u, height: %{public}u, freshRate: %{public}u",
                    screenInfo->modes_[i]->width_, screenInfo->modes_[i]->height_, screenInfo->modes_[i]->freshRate_);
            }
            reply.WriteStrongParcelable(screenInfo);
            break;
        }
        case TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            auto screenGroupInfo = GetScreenGroupInfoById(screenId);
            reply.WriteStrongParcelable(screenGroupInfo);
            break;
        }
        case TRANS_ID_GET_ALL_SCREEN_INFOS: {
            std::vector<sptr<ScreenInfo>> screenInfos = GetAllScreenInfos();
            uint32_t nums = static_cast<uint32_t>(screenInfos.size());
            reply.WriteUint32(nums);
            for (uint32_t i = 0; i < nums; ++i) {
                reply.WriteStrongParcelable(screenInfos[i]);
            }
            break;
        }
        case TRANS_ID_SCREEN_MAKE_EXPAND: {
            std::vector<ScreenId> screenId;
            if (!data.ReadUInt64Vector(&screenId)) {
                WLOGE("fail to receive expand screen in stub.");
                break;
            }
            std::vector<Point> startPoint;
            uint32_t nums = data.ReadUint32();
            for (uint32_t i = 0; i < nums; ++i) {
                Point point { data.ReadInt32(), data.ReadInt32() };
                startPoint.push_back(point);
            }
            DMError result = MakeExpand(screenId, startPoint);
            reply.WriteInt32(static_cast<int32_t>(result));
            break;
        }
        case TRANS_ID_SET_SCREEN_ACTIVE_MODE: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            uint32_t modeId = data.ReadUint32();
            bool res = SetScreenActiveMode(screenId, modeId);
            reply.WriteBool(res);
            break;
        }
        case TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            std::vector<ScreenColorGamut> colorGamuts;
            DMError ret = GetScreenSupportedColorGamuts(screenId, colorGamuts);
            reply.WriteInt32(static_cast<int32_t>(ret));
            if (ret != DMError::DM_OK) {
                break;
            }
            uint32_t size = colorGamuts.size();
            reply.WriteUint32(size);
            for (uint32_t i = 0; i < size; i++) {
                reply.WriteUint32(static_cast<uint32_t>(colorGamuts[i]));
            }
            break;
        }
        case TRANS_ID_SCREEN_GET_COLOR_GAMUT: {
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
        case TRANS_ID_SCREEN_SET_COLOR_GAMUT: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            int32_t colorGamutIdx = data.ReadInt32();
            DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case TRANS_ID_SCREEN_GET_GAMUT_MAP: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            ScreenGamutMap gamutMap;
            DMError ret = GetScreenGamutsMap(screenId, gamutMap);
            reply.WriteInt32(static_cast<int32_t>(ret));
            if (ret != DMError::DM_OK) {
                break;
            }
            reply.WriteInt32(static_cast<uint32_t>(gamutMap));
            break;
        }
        case TRANS_ID_SCREEN_SET_GAMUT_MAP: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
            DMError ret = SetScreenGamutsMap(screenId, gamutMap);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case TRANS_ID_SCREEN_SET_COLOR_TRANSFORM: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            DMError ret = SetScreenColorTransform(screenId);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace OHOS::Rosen