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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DMS_DM_SERVER, "DisplayManagerStub"};
    const static uint32_t MAX_SCREEN_SIZE = 32;
}

const std::map<DisplayManagerStub::DisplayManagerMessage,
    DisplayManagerStub::StubFunc> DisplayManagerStub::stubFuncMap_ {
    { DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO, &DisplayManagerStub::ProcGetfaultDisplayInfo },
    { DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID, &DisplayManagerStub::ProcGetDisplayById },
    { DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN, &DisplayManagerStub::ProcGetDisplayByScreen },
    { DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN, &DisplayManagerStub::ProcCreateVirtualScreen },
    { DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN, &DisplayManagerStub::ProcDestroyVirtualScreen },
    { DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE, &DisplayManagerStub::ProcSetVirtualScreenSurface },
    { DisplayManagerMessage::TRANS_ID_SET_ORIENTATION, &DisplayManagerStub::ProcSetOrirntation },
    { DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT,  &DisplayManagerStub::ProcGetDisplaySnapshot },
    { DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        &DisplayManagerStub::ProcRegisterDisplayManagerAgent },
    { DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        &DisplayManagerStub::ProcUnregisterDisplayManagerAgent },
    { DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN, &DisplayManagerStub::ProcWakeUpBegin },
    { DisplayManagerMessage::TRANS_ID_WAKE_UP_END, &DisplayManagerStub::ProcWakeUpEnd },
    { DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN, &DisplayManagerStub::ProcSuspendBegin },
    { DisplayManagerMessage::TRANS_ID_SUSPEND_END, &DisplayManagerStub::ProcSuspendEnd },
    { DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER, &DisplayManagerStub::ProcSetSpecifiedScreenPower },
    { DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL, &DisplayManagerStub::ProcSetScreenPowerForAll },
    { DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER, &DisplayManagerStub::ProcGetScreenPower },
    { DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE, &DisplayManagerStub::ProcSetDisplayState },
    { DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE, &DisplayManagerStub::ProcGetDisplayState },
    { DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT, &DisplayManagerStub::ProcNotifyDisplayEvent },
    { DisplayManagerMessage::TRANS_ID_SET_FREEZE_EVENT, &DisplayManagerStub::ProcSetFreezeEvent },
    { DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR, &DisplayManagerStub::ProcMakeScreenMirror },
    { DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID, &DisplayManagerStub::ProcGetScreenInfoById },
    { DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID, &DisplayManagerStub::ProcGetScreenGroupInfoById },
    { DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS, &DisplayManagerStub::ProcGetAllScreenInfos },
    { DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS, &DisplayManagerStub::ProcGetAllDisplayIds },
    { DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND, &DisplayManagerStub::ProcMakeScreenExpand },
    { DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP,
        &DisplayManagerStub::ProcRemoveVirtualScreenFromScreenGroup },
    { DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE, &DisplayManagerStub::ProSetScreenActiveMode },
    { DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO, &DisplayManagerStub::ProcSetVirtualPixelRatio },
    { DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS,
        &DisplayManagerStub::ProcGetScreenSupportedColorGamuts },
    { DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT, &DisplayManagerStub::ProcGetScreenColorGamut },
    { DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT, &DisplayManagerStub::ProcSetScreenColorGamut },
    { DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP, &DisplayManagerStub::ProcGetScreenGamutMap },
    { DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP, &DisplayManagerStub::ProcSetScreenGamutMap },
    { DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM, &DisplayManagerStub::ProcSetScreenColorTransfrom },
    { DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED, &DisplayManagerStub::ProcIsScreenRotationLocked },
    { DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED, &DisplayManagerStub::ProcSetScreenRotationLocked },
    { DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW, &DisplayManagerStub::ProcHasPrivateWindow },
    { DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO, &DisplayManagerStub::ProcGetCutoutInfo },
    { DisplayManagerMessage::TRANS_ID_ADD_SURFACE_NODE, &DisplayManagerStub::ProcAddSurfaceNode },
    { DisplayManagerMessage::TRANS_ID_REMOVE_SURFACE_NODE, &DisplayManagerStub::ProcRemoveSurfaceNode },
    { DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR, &DisplayManagerStub::ProcStopScreenMirror },
    { DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND, &DisplayManagerStub::ProcStopScreenExpand },
    { DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN, &DisplayManagerStub::ProcResizeVirtualScreen },
    { DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN, &DisplayManagerStub::ProcMakeUniqueScreen },
};

int32_t DisplayManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    DisplayManagerMessage msgId = static_cast<DisplayManagerMessage>(code);
    auto iter = stubFuncMap_.find(msgId);
    if (iter == stubFuncMap_.end()) {
        WLOGFW("unknown transaction code");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return (this->*(iter->second))(data, reply);
}

int32_t DisplayManagerStub::ProcGetfaultDisplayInfo(MessageParcel &data, MessageParcel &reply)
{
    auto info = GetDefaultDisplayInfo();
    reply.WriteParcelable(info);
    return 0;
}

int32_t DisplayManagerStub::ProcGetDisplayById(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = data.ReadUint64();
    auto info = GetDisplayInfoById(displayId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t DisplayManagerStub::ProcGetDisplayByScreen(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = data.ReadUint64();
    auto info = GetDisplayInfoByScreen(screenId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t DisplayManagerStub::ProcCreateVirtualScreen (MessageParcel &data, MessageParcel &reply)
{
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
    return 0;
}

int32_t DisplayManagerStub::ProcDestroyVirtualScreen(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError result = DestroyVirtualScreen(screenId);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t DisplayManagerStub::ProcSetVirtualScreenSurface(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcSetOrirntation(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    Orientation orientation = static_cast<Orientation>(data.ReadUint32());
    DMError ret = SetOrientation(screenId, orientation);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcGetDisplaySnapshot(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = data.ReadUint64();
    DmErrorCode errorCode = DmErrorCode::DM_OK;
    std::shared_ptr<Media::PixelMap> displaySnapshot = GetDisplaySnapshot(displayId, &errorCode);
    reply.WriteParcelable(displaySnapshot == nullptr ? nullptr : displaySnapshot.get());
    reply.WriteInt32(static_cast<int32_t>(errorCode));
    return 0;
}

int32_t DisplayManagerStub::ProcRegisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    if (agent == nullptr) {
        WLOGFE("agent is nullptr");
        return 0;
    }
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = RegisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcUnregisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    if (agent == nullptr) {
        WLOGFE("agent is nullptr");
        return 0;
    }
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = UnregisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcWakeUpBegin(MessageParcel &data, MessageParcel &reply)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(WakeUpBegin(reason));
    return 0;
}

int32_t DisplayManagerStub::ProcWakeUpEnd(MessageParcel &data, MessageParcel &reply)
{
    reply.WriteBool(WakeUpEnd());
    return 0;
}

int32_t DisplayManagerStub::ProcSuspendBegin(MessageParcel &data, MessageParcel &reply)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SuspendBegin(reason));
    return 0;
}

int32_t DisplayManagerStub::ProcSuspendEnd(MessageParcel &data, MessageParcel &reply)
{
    reply.WriteBool(SuspendEnd());
    return 0;
}

int32_t DisplayManagerStub::ProcSetSpecifiedScreenPower(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint32());
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetSpecifiedScreenPower(screenId, state, reason));
    return 0;
}

int32_t DisplayManagerStub::ProcSetScreenPowerForAll(MessageParcel &data, MessageParcel &reply)
{
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetScreenPowerForAll(state, reason));
    return 0;
}

int32_t DisplayManagerStub::ProcGetScreenPower(MessageParcel &data, MessageParcel &reply)
{
    ScreenId dmsScreenId;
    if (!data.ReadUint64(dmsScreenId)) {
        WLOGFE("fail to read dmsScreenId.");
        return 0;
    }
    reply.WriteUint32(static_cast<uint32_t>(GetScreenPower(dmsScreenId)));
    return 0;
}

int32_t DisplayManagerStub::ProcSetDisplayState(MessageParcel &data, MessageParcel &reply)
{
    DisplayState state = static_cast<DisplayState>(data.ReadUint32());
    reply.WriteBool(SetDisplayState(state));
    return 0;
}

int32_t DisplayManagerStub::ProcGetDisplayState(MessageParcel &data, MessageParcel &reply)
{
    DisplayState state = GetDisplayState(data.ReadUint64());
    reply.WriteUint32(static_cast<uint32_t>(state));
    return 0;
}

int32_t DisplayManagerStub::ProcNotifyDisplayEvent(MessageParcel &data, MessageParcel &reply)
{
    DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
    NotifyDisplayEvent(event);
    return 0;
}

int32_t DisplayManagerStub::ProcSetFreezeEvent(MessageParcel &data, MessageParcel &reply)
{
    std::vector<DisplayId> ids;
    data.ReadUInt64Vector(&ids);
    SetFreeze(ids, data.ReadBool());
    return 0;
}

int32_t DisplayManagerStub::ProcMakeScreenMirror(MessageParcel &data, MessageParcel &reply)
{
    ScreenId mainScreenId = static_cast<ScreenId>(data.ReadUint64());
    std::vector<ScreenId> mirrorScreenId;
    if (!data.ReadUInt64Vector(&mirrorScreenId)) {
        WLOGE("fail to receive mirror screen in stub. screen:%{public}" PRIu64"", mainScreenId);
        return 0;
    }
    ScreenId screenGroupId = INVALID_SCREEN_ID;
    DMError ret = MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteUint64(static_cast<uint64_t>(screenGroupId));
    return 0;
}

int32_t DisplayManagerStub::ProcGetScreenInfoById(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenInfo = GetScreenInfoById(screenId);
    reply.WriteStrongParcelable(screenInfo);
    return 0;
}

int32_t DisplayManagerStub::ProcGetScreenGroupInfoById(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenGroupInfo = GetScreenGroupInfoById(screenId);
    reply.WriteStrongParcelable(screenGroupInfo);
    return 0;
}

int32_t DisplayManagerStub::ProcGetAllScreenInfos(MessageParcel &data, MessageParcel &reply)
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    DMError ret  = GetAllScreenInfos(screenInfos);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (!MarshallingHelper::MarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos)) {
        WLOGE("fail to marshalling screenInfos in stub.");
    }
    return 0;
}

int32_t DisplayManagerStub::ProcGetAllDisplayIds(MessageParcel &data, MessageParcel &reply)
{
    std::vector<DisplayId> allDisplayIds = GetAllDisplayIds();
    reply.WriteUInt64Vector(allDisplayIds);
    return 0;
}

int32_t DisplayManagerStub::ProcMakeScreenExpand(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> screenId;
    if (!data.ReadUInt64Vector(&screenId)) {
        WLOGE("fail to receive expand screen in stub.");
        return 0;
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

int32_t DisplayManagerStub::ProcRemoveVirtualScreenFromScreenGroup(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> screenId;
    if (!data.ReadUInt64Vector(&screenId)) {
        WLOGE("fail to receive screens in stub.");
        return 0;
    }
    RemoveVirtualScreenFromGroup(screenId);
    return 0;
}

int32_t DisplayManagerStub::ProSetScreenActiveMode(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t modeId = data.ReadUint32();
    DMError ret = SetScreenActiveMode(screenId, modeId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcSetVirtualPixelRatio(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcGetScreenSupportedColorGamuts(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    std::vector<ScreenColorGamut> colorGamuts;
    DMError ret = GetScreenSupportedColorGamuts(screenId, colorGamuts);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    MarshallingHelper::MarshallingVectorObj<ScreenColorGamut>(reply, colorGamuts,
        [](Parcel& parcel, const ScreenColorGamut& color) {
            return parcel.WriteUint32(static_cast<uint32_t>(color));
        }
    );
    return 0;
}

int32_t DisplayManagerStub::ProcGetScreenColorGamut(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenColorGamut colorGamut;
    DMError ret = GetScreenColorGamut(screenId, colorGamut);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    reply.WriteUint32(static_cast<uint32_t>(colorGamut));
    return 0;
}

int32_t DisplayManagerStub::ProcSetScreenColorGamut(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t colorGamutIdx = data.ReadInt32();
    DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcGetScreenGamutMap(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap;
    DMError ret = GetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    reply.WriteInt32(static_cast<uint32_t>(gamutMap));
    return 0;
}

int32_t DisplayManagerStub::ProcSetScreenGamutMap(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
    DMError ret = SetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcSetScreenColorTransfrom (MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError ret = SetScreenColorTransform(screenId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcIsScreenRotationLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isLocked = false;
    DMError ret = IsScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(isLocked);
    return 0;
}

int32_t DisplayManagerStub::ProcSetScreenRotationLocked(MessageParcel &data, MessageParcel &reply)
{
    bool isLocked = static_cast<bool>(data.ReadBool());
    DMError ret = SetScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcHasPrivateWindow(MessageParcel &data, MessageParcel &reply)
{
    DisplayId id = static_cast<DisplayId>(data.ReadUint64());
    bool hasPrivateWindow = false;
    DMError ret = HasPrivateWindow(id, hasPrivateWindow);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(hasPrivateWindow);
    return 0;
}

int32_t DisplayManagerStub::ProcGetCutoutInfo(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    sptr<CutoutInfo> cutoutInfo = GetCutoutInfo(displayId);
    reply.WriteParcelable(cutoutInfo);
    return 0;
}

int32_t DisplayManagerStub::ProcAddSurfaceNode(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
    auto ret = AddSurfaceNodeToDisplay(displayId, surfaceNode, true);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcRemoveSurfaceNode(MessageParcel &data, MessageParcel &reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
    auto ret = RemoveSurfaceNodeFromDisplay(displayId, surfaceNode);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcStopScreenMirror(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> mirrorScreenIds;
    if (!data.ReadUInt64Vector(&mirrorScreenIds)) {
        WLOGE("fail to receive mirror screens in stub.");
        return 0;
    }
    DMError ret = StopMirror(mirrorScreenIds);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcStopScreenExpand(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> expandScreenIds;
    if (!data.ReadUInt64Vector(&expandScreenIds)) {
        WLOGE("fail to receive expand screens in stub.");
        return 0;
    }
    DMError ret = StopExpand(expandScreenIds);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcResizeVirtualScreen(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    DMError ret = ResizeVirtualScreen(screenId, width, height);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcMakeUniqueScreen(MessageParcel &data, MessageParcel &reply)
{
    std::vector<ScreenId> uniqueScreenIds;
    uint32_t size = data.ReadUint32();
    if (size > MAX_SCREEN_SIZE) {
        WLOGFE("screenIds size is bigger than %{public}u", MAX_SCREEN_SIZE);
        return 0;
    }
    if (!data.ReadUInt64Vector(&uniqueScreenIds)) {
        WLOGFE("failed to receive unique screens in stub");
        return 0;
    }
    DMError ret = MakeUniqueScreen(uniqueScreenIds);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

} // namespace OHOS::Rosen