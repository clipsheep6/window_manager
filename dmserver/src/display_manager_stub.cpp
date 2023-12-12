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
            return DoGetDefaultDisplayInfo(reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID: {
            return DoGetDisplayInfoById(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN: {
            return DoGetDisplayInfoByScreen(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN: {
            return DoCreateVirtualScreen(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN: {
            return DoDestroyVirtualScreen(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE: {
            return DoSetVirtualScreenSurface(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_ORIENTATION: {
            return DoSetOrientation(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT: {
            return DoGetDisplaySnapshot(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT: {
            return DoRegisterDisplayManagerAgent(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT: {
            return DoUnregisterDisplayManagerAgent(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN: {
            return DoWakeUpBegin(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_WAKE_UP_END: {
            return DoWakeUpEnd(reply);
        }
        case DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN: {
            return DoSuspendBegin(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SUSPEND_END: {
            return DoSuspendEnd(reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER: {
            return DoSetSpecifiedScreenPower(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL: {
            return DoSetScreenPowerForAll(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER: {
            return DoGetScreenPower(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE: {
            return DoSetDisplayState(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE: {
            return DoGetDisplayState(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT: {
            return DoNotifyDisplayEvent(data);
        }
        case DisplayManagerMessage::TRANS_ID_SET_FREEZE_EVENT: {
            return DoSetFreeze(data);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR: {
            return DoMakeMirror(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID: {
            return DoGetScreenInfoById(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID: {
            return DoGetScreenGroupInfoById(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS: {
            return DoGetAllScreenInfos(reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS: {
            return DoGetAllDisplayIds(reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND: {
            return DoScreenMakeExpand(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP: {
            return DoRemoveVirtualScreenFromGroup(data);
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE: {
            return DoSetScreenActiveMode(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO: {
            return DoSetVirtualPixelRatio(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS: {
            return DoGetScreenSupportedColorGamuts(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT: {
            return DoGetScreenColorGamut(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT: {
            return DoSetScreenColorGamut(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP: {
            return DoGetScreenGamutMap(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP: {
            return DoSetScreenGamutMap(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM: {
            return DoSetScreenColorTransform(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED: {
            return DoIsScreenRotationLocked(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED: {
            return DoSetScreenRotationLocked(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW: {
            return DoHasPrivateWindow(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO: {
            return DoGetCutoutInfo(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_ADD_SURFACE_NODE: {
            return DoAddSurfaceNodeToDisplay(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_REMOVE_SURFACE_NODE: {
            return DoRemoveSurfaceNodeFromDisplay(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR: {
            return DoStopMirror(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND: {
            return DoStopExpand(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN: {
            return DoResizeVirtualScreen(data, reply);
        }
        case DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN: {
            return DoMakeUniqueScreen(data, reply);
        }
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}

int32_t DisplayManagerStub::DoGetDefaultDisplayInfo(MessageParcel &reply)
{
    auto info = GetDefaultDisplayInfo();
    reply.WriteParcelable(info);
    return 0;
}

int32_t DisplayManagerStub::DoGetDisplayInfoById(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = data.ReadUint64();
    auto info = GetDisplayInfoById(displayId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t DisplayManagerStub::DoGetDisplayInfoByScreen(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = data.ReadUint64();
    auto info = GetDisplayInfoByScreen(screenId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t DisplayManagerStub::DoCreateVirtualScreen(MessageParcel &data, MessageParcel &reply)
{
    std::string name = data.ReadString();
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    float density = data.ReadFloat();
    int32_t flags = data.ReadInt32();
    bool isForShot = data.ReadBool();
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
        .isForShot_ = isForShot
    };
    ScreenId screenId = CreateVirtualScreen(virScrOption, virtualScreenAgent);
    reply.WriteUint64(static_cast<uint64_t>(screenId));
    return 0;
}

int32_t DisplayManagerStub::DoDestroyVirtualScreen(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError result = DestroyVirtualScreen(screenId);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t DisplayManagerStub::DoSetVirtualScreenSurface(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    bool isSurfaceValid = data.ReadBool();
    sptr<IBufferProducer> bp = nullptr;
    if (isSurfaceValid) {
        sptr<IRemoteObject> surfaceObject = data.ReadRemoteObject();
        bp = iface_cast<IBufferProducer>(surfaceObject);
    }
    DMError result = SetVirtualScreenSurface(screenId, bp);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t DisplayManagerStub::DoSetOrientation(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    Orientation orientation = static_cast<Orientation>(data.ReadUint32());
    DMError ret = SetOrientation(screenId, orientation);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoGetDisplaySnapshot(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = data.ReadUint64();
    DmErrorCode errorCode = DmErrorCode::DM_OK;
    std::shared_ptr<Media::PixelMap> displaySnapshot = GetDisplaySnapshot(displayId, &errorCode);
    reply.WriteParcelable(displaySnapshot == nullptr ? nullptr : displaySnapshot.get());
    reply.WriteInt32(static_cast<int32_t>(errorCode));
    return 0;
}

int32_t DisplayManagerStub::DoRegisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = RegisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoUnregisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = UnregisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoWakeUpBegin(MessageParcel &data, MessageParcel &reply)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(WakeUpBegin(reason));
    return 0;
}

int32_t DisplayManagerStub::DoWakeUpEnd(MessageParcel &reply)
{
    reply.WriteBool(WakeUpEnd());
    return 0;
}

int32_t DisplayManagerStub::DoSuspendBegin(MessageParcel &data, MessageParcel &reply)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SuspendBegin(reason));
    return 0;
}

int32_t DisplayManagerStub::DoSuspendEnd(MessageParcel &reply)
{
    reply.WriteBool(SuspendEnd());
    return 0;
}

int32_t DisplayManagerStub::DoSetSpecifiedScreenPower(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint32());
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetSpecifiedScreenPower(screenId, state, reason));
    return 0;
}

int32_t DisplayManagerStub::DoSetScreenPowerForAll(MessageParcel &data, MessageParcel &reply)
{
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetScreenPowerForAll(state, reason));
    return 0;
}

int32_t DisplayManagerStub::DoGetScreenPower(MessageParcel &data, MessageParcel &reply)
{
    ScreenId dmsScreenId;
    if (!data.ReadUint64(dmsScreenId)) {
        WLOGFE("fail to read dmsScreenId.");
        return -1;
    }
    reply.WriteUint32(static_cast<uint32_t>(GetScreenPower(dmsScreenId)));
    return 0;
}

int32_t DisplayManagerStub::DoSetDisplayState(MessageParcel &data, MessageParcel &reply)
{
    DisplayState state = static_cast<DisplayState>(data.ReadUint32());
    reply.WriteBool(SetDisplayState(state));
    return 0;
}

int32_t DisplayManagerStub::DoGetDisplayState(MessageParcel &data, MessageParcel &reply)
{
    DisplayState state = GetDisplayState(data.ReadUint64());
    reply.WriteUint32(static_cast<uint32_t>(state));
    return 0;
}

int32_t DisplayManagerStub::DoNotifyDisplayEvent(MessageParcel &data)
{
    DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
    NotifyDisplayEvent(event);
    return 0;
}

int32_t DisplayManagerStub::DoSetFreeze(MessageParcel &data)
{
    std::vector<DisplayId> ids;
    data.ReadUInt64Vector(&ids);
    SetFreeze(ids, data.ReadBool());
    return 0;
}

int32_t DisplayManagerStub::DoMakeMirror(MessageParcel &data, MessageParcel &reply)
{
    ScreenId mainScreenId = static_cast<ScreenId>(data.ReadUint64());
    std::vector<ScreenId> mirrorScreenId;
    if (!data.ReadUInt64Vector(&mirrorScreenId)) {
        WLOGE("fail to receive mirror screen in stub. screen:%{public}" PRIu64"", mainScreenId);
        return -1;
    }
    ScreenId screenGroupId = INVALID_SCREEN_ID;
    DMError ret = MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteUint64(static_cast<uint64_t>(screenGroupId));
    return 0;
}

int32_t DisplayManagerStub::DoGetScreenInfoById(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenInfo = GetScreenInfoById(screenId);
    reply.WriteStrongParcelable(screenInfo);
    return 0;
}

int32_t DisplayManagerStub::DoGetScreenGroupInfoById(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenGroupInfo = GetScreenGroupInfoById(screenId);
    reply.WriteStrongParcelable(screenGroupInfo);
    return 0;
}

int32_t DisplayManagerStub::DoGetAllScreenInfos(MessageParcel &reply)
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    DMError ret = GetAllScreenInfos(screenInfos);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (!MarshallingHelper::MarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos)) {
        WLOGE("fail to marshalling screenInfos in stub.");
        return -1;
    }
    return 0;
}

int32_t DisplayManagerStub::DoGetAllDisplayIds(MessageParcel &reply)
{
    std::vector<DisplayId> allDisplayIds = GetAllDisplayIds();
    reply.WriteUInt64Vector(allDisplayIds);
    return 0;
}

int32_t DisplayManagerStub::DoScreenMakeExpand(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> screenId;
    if (!data.ReadUInt64Vector(&screenId)) {
        WLOGE("fail to receive expand screen in stub.");
        return -1;
    }
    std::vector<Point> startPoint;
    if (!MarshallingHelper::UnmarshallingVectorObj<Point>(data, startPoint, [](Parcel& parcel, Point& point) {
            return parcel.ReadInt32(point.posX_) && parcel.ReadInt32(point.posY_);
        })) {
        WLOGE("fail to receive startPoint in stub.");
        return 0;
    }
    ScreenId screenGroupId = INVALID_SCREEN_ID;
    DMError ret = MakeExpand(screenId, startPoint, screenGroupId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteUint64(static_cast<uint64_t>(screenGroupId));
    return 0;
}

int32_t DisplayManagerStub::DoRemoveVirtualScreenFromGroup(MessageParcel &data)
{
    std::vector<ScreenId> screenId;
    if (!data.ReadUInt64Vector(&screenId)) {
        WLOGE("fail to receive screens in stub.");
        return 0;
    }
    RemoveVirtualScreenFromGroup(screenId);
    return 0;
}

int32_t DisplayManagerStub::DoSetScreenActiveMode(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t modeId = data.ReadUint32();
    DMError ret = SetScreenActiveMode(screenId, modeId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoSetVirtualPixelRatio(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoGetScreenSupportedColorGamuts(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    std::vector<ScreenColorGamut> colorGamuts;
    DMError ret = GetScreenSupportedColorGamuts(screenId, colorGamuts);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return -1;
    }
    MarshallingHelper::MarshallingVectorObj<ScreenColorGamut>(reply, colorGamuts,
        [](Parcel& parcel, const ScreenColorGamut& color) {
            return parcel.WriteUint32(static_cast<uint32_t>(color));
        }
    );
    return 0;
}

int32_t DisplayManagerStub::DoGetScreenColorGamut(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenColorGamut colorGamut;
    DMError ret = GetScreenColorGamut(screenId, colorGamut);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return -1;
    }
    reply.WriteUint32(static_cast<uint32_t>(colorGamut));
    return 0;
}

int32_t DisplayManagerStub::DoSetScreenColorGamut(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t colorGamutIdx = data.ReadInt32();
    DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoGetScreenGamutMap(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap;
    DMError ret = GetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return -1;
    }
    reply.WriteInt32(static_cast<uint32_t>(gamutMap));
    return 0;
}

int32_t DisplayManagerStub::DoSetScreenGamutMap(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
    DMError ret = SetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoSetScreenColorTransform(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError ret = SetScreenColorTransform(screenId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoIsScreenRotationLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isLocked = false;
    DMError ret = IsScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(isLocked);
    return 0;
}

int32_t DisplayManagerStub::DoSetScreenRotationLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isLocked = static_cast<bool>(data.ReadBool());
    DMError ret = SetScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoHasPrivateWindow(MessageParcel &data, MessageParcel &reply)
{
    DisplayId id = static_cast<DisplayId>(data.ReadUint64());
    bool hasPrivateWindow = false;
    DMError ret = HasPrivateWindow(id, hasPrivateWindow);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(hasPrivateWindow);
    return 0;
}

int32_t DisplayManagerStub::DoGetCutoutInfo(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    sptr<CutoutInfo> cutoutInfo = GetCutoutInfo(displayId);
    reply.WriteParcelable(cutoutInfo);
    return 0;
}

int32_t DisplayManagerStub::DoAddSurfaceNodeToDisplay(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
    auto ret = AddSurfaceNodeToDisplay(displayId, surfaceNode, true);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoRemoveSurfaceNodeFromDisplay(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
    auto ret = RemoveSurfaceNodeFromDisplay(displayId, surfaceNode);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoStopMirror(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> mirrorScreenIds;
    if (!data.ReadUInt64Vector(&mirrorScreenIds)) {
        WLOGE("fail to receive mirror screens in stub.");
        return -1;
    }
    DMError ret = StopMirror(mirrorScreenIds);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoStopExpand(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> expandScreenIds;
    if (!data.ReadUInt64Vector(&expandScreenIds)) {
        WLOGE("fail to receive expand screens in stub.");
        return -1;
    }
    DMError ret = StopExpand(expandScreenIds);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoResizeVirtualScreen(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    DMError ret = ResizeVirtualScreen(screenId, width, height);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::DoMakeUniqueScreen(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> uniqueScreenIds;
    uint32_t size = data.ReadUint32();
    if (size > MAX_SCREEN_SIZE) {
        WLOGFE("screenIds size is bigger than %{public}u", MAX_SCREEN_SIZE);
        return -1;
    }
    if (!data.ReadUInt64Vector(&uniqueScreenIds)) {
        WLOGFE("failed to receive unique screens in stub");
        return -1;
    }
    DMError ret = MakeUniqueScreen(uniqueScreenIds);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}
} // namespace OHOS::Rosen

