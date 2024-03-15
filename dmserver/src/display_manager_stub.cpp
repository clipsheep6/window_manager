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

const DisplayManagerStub::ProcFuncMap DisplayManagerStub::procFuncMap_ {
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO,
        &DisplayManagerStub::ProcGetDefaultDisplayInfo},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID,
        &DisplayManagerStub::ProcGetDisplayById},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN,
        &DisplayManagerStub::ProcGetDisplayByScreen},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN,
        &DisplayManagerStub::ProcCreateVirtualScreen},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN,
        &DisplayManagerStub::ProcDestroyVirtualScreen},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE,
        &DisplayManagerStub::ProcSetVirtualScreenSurface},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_ORIENTATION,
        &DisplayManagerStub::ProcSetOrientation},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT,
        &DisplayManagerStub::ProcGetDisplaySnapshot},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        &DisplayManagerStub::ProcRegisterDisplayManagerAgent},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        &DisplayManagerStub::ProcUnregisterDisplayManagerAgent},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN,
        &DisplayManagerStub::ProcWakeUpBegin},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_WAKE_UP_END,
        &DisplayManagerStub::ProcWakeUpEnd},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN,
        &DisplayManagerStub::ProcSuspendBegin},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SUSPEND_END,
        &DisplayManagerStub::ProcSuspendEnd},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER,
        &DisplayManagerStub::ProcSetSpecifiedScreenPower},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL,
        &DisplayManagerStub::ProcSetScreenPowerForAll},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER,
        &DisplayManagerStub::ProcGetScreenPower},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE,
        &DisplayManagerStub::ProcSetDisplayState},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE,
        &DisplayManagerStub::ProcGetDisplayState},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT,
        &DisplayManagerStub::ProcNotifyDisplayEvent},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_FREEZE_EVENT,
        &DisplayManagerStub::ProcSetFreezeEvent},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR,
        &DisplayManagerStub::ProcScreenMakeMirror},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID,
        &DisplayManagerStub::ProcGetScreenInfoById},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID,
        &DisplayManagerStub::ProcGetScreenGroupInfoById},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS,
        &DisplayManagerStub::ProcGetAllScreenInfos},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS,
        &DisplayManagerStub::ProcGetAllDisplayIds},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND,
        &DisplayManagerStub::ProcScreenMakeExpand},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP,
        &DisplayManagerStub::ProcRemoveVirtualScreenFromScreenGroup},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE,
        &DisplayManagerStub::ProcSetScreenActiveMode},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO,
        &DisplayManagerStub::ProcSetVirtualPixelRatio},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS,
        &DisplayManagerStub::ProcScreenGetSupportedColorGamuts},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT,
        &DisplayManagerStub::ProcScreenGetColorGamut},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT,
        &DisplayManagerStub::ProcScreenSetColorGamut},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP,
        &DisplayManagerStub::ProcScreenGetGamutMap},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP,
        &DisplayManagerStub::ProcScreenGetGamutMap},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM,
        &DisplayManagerStub::ProcScreenSetColorTransform},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED,
        &DisplayManagerStub::ProcIsScreenRotationLocked},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED,
        &DisplayManagerStub::ProcSetScreenRotationLocked},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW,
        &DisplayManagerStub::ProcHasPrivateWindow},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_REMOVE_SURFACE_NODE,
        &DisplayManagerStub::ProcRemoveSurfaceNode},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR,
        &DisplayManagerStub::ProcScreenStopMirror},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND,
        &DisplayManagerStub::ProcScreenStopExpand},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN,
        &DisplayManagerStub::ProcResizeVirtualScreen},
    {DisplayManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN,
        &DisplayManagerStub::ProcSceneBoardMakeUniqueScreen},
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
    if (auto search = procFuncMap_.find(msgId); search != procFuncMap_.end()) {
        return (this->*(search->second))(data, reply);
    }
    WLOGFW("unknown transaction code");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t DisplayManagerStub::ProcGetDefaultDisplayInfo(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcCreateVirtualScreen(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcSetOrientation(MessageParcel &data, MessageParcel &reply)
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
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = RegisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcUnregisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
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

int32_t DisplayManagerStub::ProcScreenMakeMirror(MessageParcel &data, MessageParcel &reply)
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
int32_t DisplayManagerStub::ProcScreenMakeExpand(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcSetScreenActiveMode(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcScreenGetSupportedColorGamuts(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcScreenGetColorGamut(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcScreenSetColorGamut(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t colorGamutIdx = data.ReadInt32();
    DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcScreenGetGamutMap(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcScreenSetGamutMap(MessageParcel &data, MessageParcel &reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
    DMError ret = SetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t DisplayManagerStub::ProcScreenSetColorTransform(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcGetGutoutInfo(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcScreenStopMirror(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcScreenStopExpand(MessageParcel &data, MessageParcel &reply)
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

int32_t DisplayManagerStub::ProcSceneBoardMakeUniqueScreen(MessageParcel &data, MessageParcel &reply)
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