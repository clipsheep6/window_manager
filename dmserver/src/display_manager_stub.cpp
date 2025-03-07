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

#include "display_manager_stub.h"

#include "dm_common.h"

#include <ipc_skeleton.h>

#include "marshalling_helper.h"
#include "window_manager_hilog.h"

#include "transaction/rs_interfaces.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerStub"};
const static uint32_t MAX_SCREEN_SIZE = 32;
}

int32_t DisplayManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
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
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID: {
            DisplayId displayId = data.ReadUint64();
            auto info = GetDisplayInfoById(displayId);
            reply.WriteParcelable(info);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_VISIBLE_AREA_DISPLAY_INFO_BY_ID: {
            DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
            auto info = GetVisibleAreaDisplayInfoById(displayId);
            reply.WriteParcelable(info);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN: {
            ScreenId screenId = data.ReadUint64();
            auto info = GetDisplayInfoByScreen(screenId);
            reply.WriteParcelable(info);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN: {
            std::string name = data.ReadString();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            float density = data.ReadFloat();
            int32_t flags = data.ReadInt32();
            bool isForShot = data.ReadBool();
            std::vector<uint64_t> missionIds;
            data.ReadUInt64Vector(&missionIds);
            bool isSurfaceValid = data.ReadBool();
            sptr<Surface> surface = nullptr;
            if (isSurfaceValid) {
                sptr<IRemoteObject> surfaceObject = data.ReadRemoteObject();
                sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(surfaceObject);
                surface = Surface::CreateSurfaceAsProducer(bp);
            }
            sptr<IRemoteObject> virtualScreenAgent = data.ReadRemoteObject();
            VirtualScreenOption virScrOption = {
                .name_ = name,
                .width_ = width,
                .height_ = height,
                .density_ = density,
                .surface_ = surface,
                .flags_ = flags,
                .isForShot_ = isForShot,
                .missionIds_ = missionIds
            };
            ScreenId screenId = CreateVirtualScreen(virScrOption, virtualScreenAgent);
            reply.WriteUint64(static_cast<uint64_t>(screenId));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            DMError result = DestroyVirtualScreen(screenId);
            reply.WriteInt32(static_cast<int32_t>(result));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            bool isSurfaceValid = data.ReadBool();
            sptr<IBufferProducer> bp = nullptr;
            if (isSurfaceValid) {
                sptr<IRemoteObject> surfaceObject = data.ReadRemoteObject();
                bp = iface_cast<IBufferProducer>(surfaceObject);
            }
            DMError result = SetVirtualScreenSurface(screenId, bp);
            reply.WriteInt32(static_cast<int32_t>(result));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_ORIENTATION: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            Orientation orientation = static_cast<Orientation>(data.ReadUint32());
            DMError ret = SetOrientation(screenId, orientation);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT: {
            DisplayId displayId = data.ReadUint64();
            DmErrorCode errorCode = DmErrorCode::DM_OK;
            std::shared_ptr<Media::PixelMap> displaySnapshot = GetDisplaySnapshot(displayId, &errorCode);
            reply.WriteParcelable(displaySnapshot == nullptr ? nullptr : displaySnapshot.get());
            reply.WriteInt32(static_cast<int32_t>(errorCode));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT: {
            auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
            if (agent == nullptr) {
                WLOGFE("agent is nullptr");
                break;
            }
            auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
            DMError ret = RegisterDisplayManagerAgent(agent, type);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT: {
            auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
            if (agent == nullptr) {
                WLOGFE("agent is nullptr");
                break;
            }
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
        case DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint32());
            ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(SetSpecifiedScreenPower(screenId, state, reason));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL: {
            ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
            PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
            reply.WriteBool(SetScreenPowerForAll(state, reason));
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
        case DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE: {
            DisplayState state = static_cast<DisplayState>(data.ReadUint32());
            reply.WriteBool(SetDisplayState(state));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE: {
            DisplayState state = GetDisplayState(data.ReadUint64());
            reply.WriteUint32(static_cast<uint32_t>(state));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_BRIGHTNESS: {
            uint64_t screenId = data.ReadUint64();
            uint32_t level = data.ReadUint64();
            reply.WriteBool(SetScreenBrightness(screenId, level));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_BRIGHTNESS: {
            uint64_t screenId = data.ReadUint64();
            reply.WriteUint32(GetScreenBrightness(screenId));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT: {
            DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
            NotifyDisplayEvent(event);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_FREEZE_EVENT: {
            std::vector<DisplayId> ids;
            data.ReadUInt64Vector(&ids);
            SetFreeze(ids, data.ReadBool());
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR: {
            ScreenId mainScreenId = static_cast<ScreenId>(data.ReadUint64());
            std::vector<ScreenId> mirrorScreenId;
            if (!data.ReadUInt64Vector(&mirrorScreenId)) {
                WLOGE("fail to receive mirror screen in stub. screen:%{public}" PRIu64"", mainScreenId);
                break;
            }
            ScreenId screenGroupId = INVALID_SCREEN_ID;
            DMError ret = MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
            reply.WriteInt32(static_cast<int32_t>(ret));
            reply.WriteUint64(static_cast<uint64_t>(screenGroupId));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            auto screenInfo = GetScreenInfoById(screenId);
            reply.WriteStrongParcelable(screenInfo);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            auto screenGroupInfo = GetScreenGroupInfoById(screenId);
            reply.WriteStrongParcelable(screenGroupInfo);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS: {
            std::vector<sptr<ScreenInfo>> screenInfos;
            DMError ret  = GetAllScreenInfos(screenInfos);
            reply.WriteInt32(static_cast<int32_t>(ret));
            if (!MarshallingHelper::MarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos)) {
                WLOGE("fail to marshalling screenInfos in stub.");
            }
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS: {
            std::vector<DisplayId> allDisplayIds = GetAllDisplayIds();
            reply.WriteUInt64Vector(allDisplayIds);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND: {
            std::vector<ScreenId> screenId;
            if (!data.ReadUInt64Vector(&screenId)) {
                WLOGE("fail to receive expand screen in stub.");
                break;
            }
            std::vector<Point> startPoint;
            if (!MarshallingHelper::UnmarshallingVectorObj<Point>(data, startPoint, [](Parcel& parcel, Point& point) {
                    return parcel.ReadInt32(point.posX_) && parcel.ReadInt32(point.posY_);
                })) {
                WLOGE("fail to receive startPoint in stub.");
                break;
            }
            ScreenId screenGroupId = INVALID_SCREEN_ID;
            DMError ret = MakeExpand(screenId, startPoint, screenGroupId);
            reply.WriteInt32(static_cast<int32_t>(ret));
            reply.WriteUint64(static_cast<uint64_t>(screenGroupId));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP: {
            std::vector<ScreenId> screenId;
            if (!data.ReadUInt64Vector(&screenId)) {
                WLOGE("fail to receive screens in stub.");
                break;
            }
            RemoveVirtualScreenFromGroup(screenId);
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
        case DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED: {
            bool isLocked = false;
            DMError ret = IsScreenRotationLocked(isLocked);
            reply.WriteInt32(static_cast<int32_t>(ret));
            reply.WriteBool(isLocked);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED: {
            bool isLocked = static_cast<bool>(data.ReadBool());
            DMError ret = SetScreenRotationLocked(isLocked);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED_FROM_JS: {
            bool isLocked = static_cast<bool>(data.ReadBool());
            DMError ret = SetScreenRotationLockedFromJs(isLocked);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW: {
            DisplayId id = static_cast<DisplayId>(data.ReadUint64());
            bool hasPrivateWindow = false;
            DMError ret = HasPrivateWindow(id, hasPrivateWindow);
            reply.WriteInt32(static_cast<int32_t>(ret));
            reply.WriteBool(hasPrivateWindow);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO: {
            DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
            sptr<CutoutInfo> cutoutInfo = GetCutoutInfo(displayId);
            reply.WriteParcelable(cutoutInfo);
            break;
        }
        case DisplayManagerMessage::TRANS_ID_ADD_SURFACE_NODE: {
            DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
            std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
            auto ret = AddSurfaceNodeToDisplay(displayId, surfaceNode, true);
            reply.WriteUint32(static_cast<uint32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_REMOVE_SURFACE_NODE: {
            DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
            std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
            auto ret = RemoveSurfaceNodeFromDisplay(displayId, surfaceNode);
            reply.WriteUint32(static_cast<uint32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR: {
            std::vector<ScreenId> mirrorScreenIds;
            if (!data.ReadUInt64Vector(&mirrorScreenIds)) {
                WLOGE("fail to receive mirror screens in stub.");
                break;
            }
            DMError ret = StopMirror(mirrorScreenIds);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND: {
            std::vector<ScreenId> expandScreenIds;
            if (!data.ReadUInt64Vector(&expandScreenIds)) {
                WLOGE("fail to receive expand screens in stub.");
                break;
            }
            DMError ret = StopExpand(expandScreenIds);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN: {
            ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            DMError ret = ResizeVirtualScreen(screenId, width, height);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN: {
            std::vector<ScreenId> uniqueScreenIds;
            uint32_t size = data.ReadUint32();
            if (size > MAX_SCREEN_SIZE) {
                WLOGFE("screenIds size is bigger than %{public}u", MAX_SCREEN_SIZE);
                break;
            }
            if (!data.ReadUInt64Vector(&uniqueScreenIds)) {
                WLOGFE("failed to receive unique screens in stub");
                break;
            }
            std::vector<DisplayId> displayIds;
            DMError ret = MakeUniqueScreen(uniqueScreenIds, displayIds);
            reply.WriteUInt64Vector(displayIds);
            reply.WriteInt32(static_cast<int32_t>(ret));
            break;
        }
        case DisplayManagerMessage::TRANS_ID_GET_ALL_PHYSICAL_DISPLAY_RESOLUTION: {
            auto physicalInfos = GetAllDisplayPhysicalResolution();
            size_t infoSize = physicalInfos.size();
            bool writeRet = reply.WriteInt32(static_cast<int32_t>(infoSize));
            if (!writeRet) {
                WLOGFE("write physical size error");
                break;
            }
            for (const auto &physicalItem : physicalInfos) {
                writeRet = reply.WriteUint32(static_cast<uint32_t>(physicalItem.foldDisplayMode_));
                if (!writeRet) {
                    WLOGFE("write display mode error");
                    break;
                }
                writeRet = reply.WriteUint32(physicalItem.physicalWidth_);
                if (!writeRet) {
                    WLOGFE("write physical width error");
                    break;
                }
                writeRet = reply.WriteUint32(physicalItem.physicalHeight_);
                if (!writeRet) {
                    WLOGFE("write physical height error");
                    break;
                }
            }
            break;
        }
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace OHOS::Rosen