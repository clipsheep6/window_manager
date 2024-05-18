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

#include "common/rs_rect.h"
#include "dm_common.h"
#include <ipc_skeleton.h>
#include "transaction/rs_marshalling_helper.h"

#include "marshalling_helper.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DMS_SCREEN_SESSION_MANAGER,
                                          "ScreenSessionManagerStub" };
const static uint32_t MAX_SCREEN_SIZE = 32;
const static int32_t ERR_INVALID_DATA = -1;
}

template<class F>
void ScreenSessionManagerStub::BindOp(DisplayManagerMessage code, F&& func)
{
    using namespace std::placeholders;
    ioOps_[code] = std::bind(func, this, _1, _2, _3);
}

void ScreenSessionManagerStub::InitGroup1()
{
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO,
        &ScreenSessionManagerStub::GetDefaultDisplayInfoFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        &ScreenSessionManagerStub::RegisterDisplayManagerAgentFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        &ScreenSessionManagerStub::UnregisterDisplayManagerAgentFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN, &ScreenSessionManagerStub::WakeUpBeginFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_WAKE_UP_END, &ScreenSessionManagerStub::WakeUpEndFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN, &ScreenSessionManagerStub::SuspendBeginFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SUSPEND_END, &ScreenSessionManagerStub::SuspendEndFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE, &ScreenSessionManagerStub::SetDisplayStateFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER,
        &ScreenSessionManagerStub::SetSpecificScreenPowerFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL,
        &ScreenSessionManagerStub::SetScreenPowerForAllFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE, &ScreenSessionManagerStub::GetDisplayStateFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT, &ScreenSessionManagerStub::NotifyDisplayEventFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER, &ScreenSessionManagerStub::GetScreenPowerFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID, &ScreenSessionManagerStub::GetDisplayByIdFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN, &ScreenSessionManagerStub::GetDisplayByScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS, &ScreenSessionManagerStub::GetAllDisplayidsFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID, &ScreenSessionManagerStub::GetScreenInfoByIdFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS, &ScreenSessionManagerStub::GetAllScreenInfosFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS,
        &ScreenSessionManagerStub::ScreenGetSupportedColorGamutsFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN,
        &ScreenSessionManagerStub::CreateVirtualScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE,
        &ScreenSessionManagerStub::SetVirtualScreenSurfaceFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_CANVAS_ROTATION,
        &ScreenSessionManagerStub::SetVirtualScreenCanvasRotationFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SCALE_MODE,
        &ScreenSessionManagerStub::SetVirtualScreenScaleModeFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN, &ScreenSessionManagerStub::DestroyVirtualScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR, &ScreenSessionManagerStub::ScreenMakeMirrorFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR, &ScreenSessionManagerStub::ScreenStopMirrorFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_DISABLE_MIRROR, &ScreenSessionManagerStub::ScreenDisableMirrorFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND, &ScreenSessionManagerStub::ScreenMakeExpandFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND, &ScreenSessionManagerStub::ScreenStopExpandFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID,
        &ScreenSessionManagerStub::GetScreenGroupInfoByIdFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP,
        &ScreenSessionManagerStub::RemoveVirtualScreenFromScreenGroupFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT, &ScreenSessionManagerStub::GetDisplaySnapshotFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_SNAPSHOT_BY_PICKER, &ScreenSessionManagerStub::GetSnapshotByPickerFunc);
}

void ScreenSessionManagerStub::InitGroup2()
{
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE, &ScreenSessionManagerStub::SetScreenActiveModeFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO,
        &ScreenSessionManagerStub::SetVirtualPixelRatioFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM,
        &ScreenSessionManagerStub::SetVirtualPixelRatioSystemFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_RESOLUTION, &ScreenSessionManagerStub::SetResolutionFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION,
        &ScreenSessionManagerStub::GetDensityInCurrentResolutionFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT, &ScreenSessionManagerStub::ScreenGetColorGamutFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT, &ScreenSessionManagerStub::ScreenSetColorGamutFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP, &ScreenSessionManagerStub::ScreenGetGamutMapFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP,
        &ScreenSessionManagerStub::ScreenSetGamutMapFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM,
        &ScreenSessionManagerStub::ScreenSetColorTransformFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_PIXEL_FORMAT,
        &ScreenSessionManagerStub::ScreenGetPixelFormatFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_SET_PIXEL_FORMAT,
        &ScreenSessionManagerStub::ScreenSetPixelFormatFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_HDR_FORMAT,
        &ScreenSessionManagerStub::ScreenGetSupportedHdrFormatFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_HDR_FORMAT, &ScreenSessionManagerStub::ScreenGetHdrFormatFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_SET_HDR_FORMAT,
        &ScreenSessionManagerStub::ScreenSetHdrFormatFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_SPACE,
        &ScreenSessionManagerStub::ScreenGetSupportedColorSpaceFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_SPACE, &ScreenSessionManagerStub::ScreenGetColorSpaceFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_SPACE, &ScreenSessionManagerStub::ScreenSetColorSpaceFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_ORIENTATION, &ScreenSessionManagerStub::SetOrientationFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED,
        &ScreenSessionManagerStub::SetScreenRotationtionLockedFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED,
        &ScreenSessionManagerStub::IsScreenRotationLockedFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO, &ScreenSessionManagerStub::GetCutoutInfoFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW, &ScreenSessionManagerStub::HasPrivateWindowFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_CONVERT_SCREENID_TO_RSSCREENID,
        &ScreenSessionManagerStub::ConvertScreenidToRsscreenidFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_HAS_IMMERSIVE_WINDOW, &ScreenSessionManagerStub::HasImmersiveWindowFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_ALL_SCREEN,
        &ScreenSessionManagerStub::SceneBoardDumpAllScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_SPECIAL_SCREEN,
        &ScreenSessionManagerStub::SceneBoardDumpSpecialScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_DEVICE_IS_CAPTURE, &ScreenSessionManagerStub::DeviceIsCaptureFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE,
        &ScreenSessionManagerStub::SceneBoardSetFoldDisplayModeFunc);
}

void ScreenSessionManagerStub::InitGroup3()
{
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_LOCK_FOLD_DISPLAY_STATUS,
        &ScreenSessionManagerStub::SceneBoardLockFoldDisplayStatusFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE,
        &ScreenSessionManagerStub::SceneBoardGetFoldDisplayModeFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE,
        &ScreenSessionManagerStub::SceneBoardIsFoldableFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS,
        &ScreenSessionManagerStub::SceneBoardGetFoldStatusFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_CURRENT_FOLD_CREASE_REGION,
        &ScreenSessionManagerStub::SceneBoardGetCurrentFoldCreaseRegionFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN,
        &ScreenSessionManagerStub::SceneBoardMakeUniqueScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_CLIENT, &ScreenSessionManagerStub::SetClientFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_SCREEN_PROPERTY, &ScreenSessionManagerStub::GetScreenPropertyFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_NODE, &ScreenSessionManagerStub::GetDisplayNodeFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_UPDATE_SCREEN_ROTATION_PROPERTY,
        &ScreenSessionManagerStub::UpdateScreenRotationPropertyFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_CURVED_SCREEN_COMPRESSION_AREA,
        &ScreenSessionManagerStub::GetCurvedScreenCompressionAreaFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_PHY_SCREEN_PROPERTY,
        &ScreenSessionManagerStub::GetPhyScreenPropertyFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO,
        &ScreenSessionManagerStub::NotifyDisplayChangeInfoFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_STATE,
        &ScreenSessionManagerStub::SetScreenPrivacyStateFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREENID_PRIVACY_STATE,
        &ScreenSessionManagerStub::SetScreenidPrivacyStateFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_WINDOW_LIST,
        &ScreenSessionManagerStub::SetScreenPrivacyWindowListFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN, &ScreenSessionManagerStub::ResizeVirtualScreenFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_UPDATE_AVAILABLE_AREA, &ScreenSessionManagerStub::UpdateAvailableAreaFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_SCREEN_OFF_DELAY_TIME,
        &ScreenSessionManagerStub::SetScreenOffDelayTimeFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_AVAILABLE_AREA, &ScreenSessionManagerStub::GetAvailableAreaFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_NOTIFY_FOLD_TO_EXPAND_COMPLETION,
        &ScreenSessionManagerStub::NotifyFoldToExpandCompletionFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_VIRTUAL_SCREEN_FLAG,
        &ScreenSessionManagerStub::GetVirtualScreenFlagFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_FLAG,
        &ScreenSessionManagerStub::SetVirtualScreenFlagFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_GET_DEVICE_SCREEN_CONFIG,
        &ScreenSessionManagerStub::GetDeviceScreenConfigFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_REFRESH_RATE,
        &ScreenSessionManagerStub::SetVirtualScreenRefreshRateFunc);
    BindOp(DisplayManagerMessage::TRANS_ID_SWITCH_USER, &ScreenSessionManagerStub::SwitchUserFunc);
}

ScreenSessionManagerStub::ScreenSessionManagerStub()
{
    InitGroup1();
    InitGroup2();
    InitGroup3();
}

int32_t ScreenSessionManagerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return ERR_INVALID_DATA;
    }
    DisplayManagerMessage msgId = static_cast<DisplayManagerMessage>(code);
    if (auto ite = ioOps_.find(msgId); ite != ioOps_.cend()) {
        return ite->second(data, reply, option);
    } else {
        WLOGFW("unknown transaction code %{public}d", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}

void ScreenSessionManagerStub::ProcGetAvailableArea(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    DMRect area;
    DMError ret = GetAvailableArea(displayId, area);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteInt32(area.posX_);
    reply.WriteInt32(area.posY_);
    reply.WriteUint32(area.width_);
    reply.WriteUint32(area.height_);
}

void ScreenSessionManagerStub::ProcGetSnapshotByPicker(MessageParcel& reply)
{
    DmErrorCode errCode = DmErrorCode::DM_OK;
    Media::Rect imgRect { 0, 0, 0, 0 };
    std::shared_ptr<Media::PixelMap> snapshot = GetSnapshotByPicker(imgRect, &errCode);
    reply.WriteParcelable(snapshot == nullptr ? nullptr : snapshot.get());
    reply.WriteInt32(static_cast<int32_t>(errCode));
    reply.WriteInt32(imgRect.left);
    reply.WriteInt32(imgRect.top);
    reply.WriteInt32(imgRect.width);
    reply.WriteInt32(imgRect.height);
}

void ScreenSessionManagerStub::ProcSetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    VirtualScreenFlag screenFlag = static_cast<VirtualScreenFlag>(data.ReadUint32());
    DMError setRet = SetVirtualScreenFlag(screenId, screenFlag);
    reply.WriteInt32(static_cast<int32_t>(setRet));
}

void ScreenSessionManagerStub::ProcGetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    VirtualScreenFlag screenFlag = GetVirtualScreenFlag(screenId);
    reply.WriteUint32(static_cast<uint32_t>(screenFlag));
}
int32_t ScreenSessionManagerStub::GetDefaultDisplayInfoFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto info = GetDefaultDisplayInfo();
    reply.WriteParcelable(info);
    return 0;
}

int32_t ScreenSessionManagerStub::RegisterDisplayManagerAgentFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    if (agent == nullptr) {
        return ERR_INVALID_DATA;
    }
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = RegisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::UnregisterDisplayManagerAgentFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    if (agent == nullptr) {
        return ERR_INVALID_DATA;
    }
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = UnregisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::WakeUpBeginFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(WakeUpBegin(reason));
    return 0;
}

int32_t ScreenSessionManagerStub::WakeUpEndFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    reply.WriteBool(WakeUpEnd());
    return 0;
}

int32_t ScreenSessionManagerStub::SuspendBeginFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SuspendBegin(reason));
    return 0;
}

int32_t ScreenSessionManagerStub::SuspendEndFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    reply.WriteBool(SuspendEnd());
    return 0;
}

int32_t ScreenSessionManagerStub::SetDisplayStateFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayState state = static_cast<DisplayState>(data.ReadUint32());
    reply.WriteBool(SetDisplayState(state));
    return 0;
}

int32_t ScreenSessionManagerStub::SetSpecificScreenPowerFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint32());
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetSpecifiedScreenPower(screenId, state, reason));
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenPowerForAllFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetScreenPowerForAll(state, reason));
    return 0;
}

int32_t ScreenSessionManagerStub::GetDisplayStateFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayState state = GetDisplayState(data.ReadUint64());
    reply.WriteUint32(static_cast<uint32_t>(state));
    return 0;
}

int32_t ScreenSessionManagerStub::NotifyDisplayEventFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
    NotifyDisplayEvent(event);
    return 0;
}

int32_t ScreenSessionManagerStub::GetScreenPowerFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId dmsScreenId;
    if (!data.ReadUint64(dmsScreenId)) {
        WLOGFE("fail to read dmsScreenId.");
        return ERR_INVALID_DATA;
    }
    reply.WriteUint32(static_cast<uint32_t>(GetScreenPower(dmsScreenId)));
    return 0;
}

int32_t ScreenSessionManagerStub::GetDisplayByIdFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayId displayId = data.ReadUint64();
    auto info = GetDisplayInfoById(displayId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t ScreenSessionManagerStub::GetDisplayByScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = data.ReadUint64();
    auto info = GetDisplayInfoByScreen(screenId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t ScreenSessionManagerStub::GetAllDisplayidsFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    std::vector<DisplayId> allDisplayIds = GetAllDisplayIds();
    reply.WriteUInt64Vector(allDisplayIds);
    return 0;
}

int32_t ScreenSessionManagerStub::GetScreenInfoByIdFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenInfo = GetScreenInfoById(screenId);
    reply.WriteStrongParcelable(screenInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::GetAllScreenInfosFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    DMError ret  = GetAllScreenInfos(screenInfos);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (!MarshallingHelper::MarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos)) {
        WLOGE("fail to marshalling screenInfos in stub.");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetSupportedColorGamutsFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::CreateVirtualScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::SetVirtualScreenSurfaceFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::SetVirtualScreenCanvasRotationFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::SetVirtualScreenScaleModeFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    bool autoRotate = data.ReadBool();
    DMError result = SetVirtualMirrorScreenCanvasRotation(screenId, autoRotate);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t ScreenSessionManagerStub::DestroyVirtualScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError result = DestroyVirtualScreen(screenId);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenMakeMirrorFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::ScreenStopMirrorFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::ScreenDisableMirrorFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DMError ret = DisableMirror(data.ReadBool());
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenMakeExpandFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::ScreenStopExpandFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::GetScreenGroupInfoByIdFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenGroupInfo = GetScreenGroupInfoById(screenId);
    reply.WriteStrongParcelable(screenGroupInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::RemoveVirtualScreenFromScreenGroupFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    std::vector<ScreenId> screenId;
    if (!data.ReadUInt64Vector(&screenId)) {
        WLOGE("fail to receive screens in stub.");
        return 0;
    }
    RemoveVirtualScreenFromGroup(screenId);
    return 0;
}

int32_t ScreenSessionManagerStub::GetDisplaySnapshotFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayId displayId = data.ReadUint64();
    std::shared_ptr<Media::PixelMap> displaySnapshot = GetDisplaySnapshot(displayId);
    reply.WriteParcelable(displaySnapshot == nullptr ? nullptr : displaySnapshot.get());
    return 0;
}

int32_t ScreenSessionManagerStub::GetSnapshotByPickerFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ProcGetSnapshotByPicker(reply);
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenActiveModeFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t modeId = data.ReadUint32();
    DMError ret = SetScreenActiveMode(screenId, modeId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::SetVirtualPixelRatioFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::SetVirtualPixelRatioSystemFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetVirtualPixelRatioSystem(screenId, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::SetResolutionFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetResolution(screenId, width, height, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::GetDensityInCurrentResolutionFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio;
    DMError ret = GetDensityInCurResolution(screenId, virtualPixelRatio);
    reply.WriteFloat(virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetColorGamutFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::ScreenSetColorGamutFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t colorGamutIdx = data.ReadInt32();
    DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetGamutMapFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::ScreenSetGamutMapFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
    DMError ret = SetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenSetColorTransformFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError ret = SetScreenColorTransform(screenId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetPixelFormatFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    GraphicPixelFormat pixelFormat;
    DMError ret = GetPixelFormat(screenId, pixelFormat);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    reply.WriteInt32(static_cast<uint32_t>(pixelFormat));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenSetPixelFormatFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(data.ReadUint32());
    DMError ret = SetPixelFormat(screenId, pixelFormat);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetSupportedHdrFormatFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    std::vector<ScreenHDRFormat> hdrFormats;
    DMError ret = GetSupportedHDRFormats(screenId, hdrFormats);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    MarshallingHelper::MarshallingVectorObj<ScreenHDRFormat>(reply, hdrFormats,
        [](Parcel& parcel, const ScreenHDRFormat& hdrFormat) {
            return parcel.WriteUint32(static_cast<uint32_t>(hdrFormat));
        }
    );
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetHdrFormatFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenHDRFormat hdrFormat;
    DMError ret = GetScreenHDRFormat(screenId, hdrFormat);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    reply.WriteInt32(static_cast<uint32_t>(hdrFormat));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenSetHdrFormatFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t modeIdx = data.ReadInt32();
    DMError ret = SetScreenHDRFormat(screenId, modeIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetSupportedColorSpaceFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    DMError ret = GetSupportedColorSpaces(screenId, colorSpaces);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    MarshallingHelper::MarshallingVectorObj<GraphicCM_ColorSpaceType>(reply, colorSpaces,
        [](Parcel& parcel, const GraphicCM_ColorSpaceType& color) {
            return parcel.WriteUint32(static_cast<uint32_t>(color));
        }
    );
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenGetColorSpaceFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    GraphicCM_ColorSpaceType colorSpace;
    DMError ret = GetScreenColorSpace(screenId, colorSpace);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (ret != DMError::DM_OK) {
        return 0;
    }
    reply.WriteInt32(static_cast<uint32_t>(colorSpace));
    return 0;
}

int32_t ScreenSessionManagerStub::ScreenSetColorSpaceFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(data.ReadUint32());
    DMError ret = SetScreenColorSpace(screenId, colorSpace);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::SetOrientationFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    Orientation orientation = static_cast<Orientation>(data.ReadUint32());
    DMError ret = SetOrientation(screenId, orientation);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenRotationtionLockedFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    bool isLocked = static_cast<bool>(data.ReadBool());
    DMError ret = SetScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::IsScreenRotationLockedFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    bool isLocked = false;
    DMError ret = IsScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(isLocked);
    return 0;
}

int32_t ScreenSessionManagerStub::GetCutoutInfoFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    sptr<CutoutInfo> cutoutInfo = GetCutoutInfo(displayId);
    reply.WriteParcelable(cutoutInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::HasPrivateWindowFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayId id = static_cast<DisplayId>(data.ReadUint64());
    bool hasPrivateWindow = false;
    DMError ret = HasPrivateWindow(id, hasPrivateWindow);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(hasPrivateWindow);
    return 0;
}

int32_t ScreenSessionManagerStub::ConvertScreenidToRsscreenidFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenId rsId = SCREEN_ID_INVALID;
    bool ret = ConvertScreenIdToRsScreenId(screenId, rsId);
    reply.WriteBool(ret);
    reply.WriteUint64(rsId);
    return 0;
}

int32_t ScreenSessionManagerStub::HasImmersiveWindowFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    bool immersive = false;
    DMError ret = HasImmersiveWindow(immersive);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(immersive);
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardDumpAllScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    std::string dumpInfo;
    DumpAllScreensInfo(dumpInfo);
    reply.WriteString(dumpInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardDumpSpecialScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    std::string dumpInfo;
    DumpSpecialScreenInfo(screenId, dumpInfo);
    reply.WriteString(dumpInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::DeviceIsCaptureFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    reply.WriteBool(IsCaptured());
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardSetFoldDisplayModeFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    FoldDisplayMode displayMode = static_cast<FoldDisplayMode>(data.ReadUint32());
    SetFoldDisplayMode(displayMode);
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardLockFoldDisplayStatusFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    bool lockDisplayStatus = static_cast<bool>(data.ReadUint32());
    SetFoldStatusLocked(lockDisplayStatus);
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardGetFoldDisplayModeFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    FoldDisplayMode displayMode = GetFoldDisplayMode();
    reply.WriteUint32(static_cast<uint32_t>(displayMode));
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardIsFoldableFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    reply.WriteBool(IsFoldable());
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardGetFoldStatusFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    reply.WriteUint32(static_cast<uint32_t>(GetFoldStatus()));
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardGetCurrentFoldCreaseRegionFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    reply.WriteStrongParcelable(GetCurrentFoldCreaseRegion());
    return 0;
}

int32_t ScreenSessionManagerStub::SceneBoardMakeUniqueScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
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

int32_t ScreenSessionManagerStub::SetClientFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto remoteObject = data.ReadRemoteObject();
    auto clientProxy = iface_cast<IScreenSessionManagerClient>(remoteObject);
    if (clientProxy == nullptr) {
        WLOGFE("clientProxy is null");
        return 0;
    }
    SetClient(clientProxy);
    return 0;
}

int32_t ScreenSessionManagerStub::GetScreenPropertyFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    if (!RSMarshallingHelper::Marshalling(reply, GetScreenProperty(screenId))) {
        WLOGFE("Write screenProperty failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::GetDisplayNodeFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto displayNode = GetDisplayNode(screenId);
    if (!displayNode || !displayNode->Marshalling(reply)) {
        WLOGFE("Write displayNode failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::UpdateScreenRotationPropertyFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    RRect bounds;
    if (!RSMarshallingHelper::Unmarshalling(data, bounds)) {
        WLOGFE("Read bounds failed");
        return 0;
    }
    auto rotation = data.ReadFloat();
    UpdateScreenRotationProperty(screenId, bounds, rotation);
    return 0;
}

int32_t ScreenSessionManagerStub::GetCurvedScreenCompressionAreaFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto area = GetCurvedCompressionArea();
    reply.WriteUint32(area);
    return 0;
}

int32_t ScreenSessionManagerStub::GetPhyScreenPropertyFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    if (!RSMarshallingHelper::Marshalling(reply, GetPhyScreenProperty(screenId))) {
        WLOGFE("Write screenProperty failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::NotifyDisplayChangeInfoFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    sptr<DisplayChangeInfo> info = DisplayChangeInfo::Unmarshalling(data);
    if (!info) {
        WLOGFE("Read DisplayChangeInfo failed");
        return ERR_INVALID_DATA;
    }
    NotifyDisplayChangeInfoChanged(info);
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenPrivacyStateFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto hasPrivate = data.ReadBool();
    SetScreenPrivacyState(hasPrivate);
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenidPrivacyStateFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    auto hasPrivate = data.ReadBool();
    SetPrivacyStateByDisplayId(displayId, hasPrivate);
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenPrivacyWindowListFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    std::vector<std::string> privacyWindowList;
    data.ReadStringVector(&privacyWindowList);
    SetScreenPrivacyWindowList(displayId, privacyWindowList);
    return 0;
}

int32_t ScreenSessionManagerStub::ResizeVirtualScreenFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    DMError ret = ResizeVirtualScreen(screenId, width, height);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::UpdateAvailableAreaFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t posX = data.ReadInt32();
    int32_t posY = data.ReadInt32();
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    DMRect area = {posX, posY, width, height};
    UpdateAvailableArea(screenId, area);
    return 0;
}

int32_t ScreenSessionManagerStub::SetScreenOffDelayTimeFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    int32_t delay = data.ReadInt32();
    int32_t ret = SetScreenOffDelayTime(delay);
    reply.WriteInt32(ret);
    return 0;
}

int32_t ScreenSessionManagerStub::GetAvailableAreaFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ProcGetAvailableArea(data, reply);
    return 0;
}

int32_t ScreenSessionManagerStub::NotifyFoldToExpandCompletionFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    bool foldToExpand = data.ReadBool();
    NotifyFoldToExpandCompletion(foldToExpand);
    return 0;
}

int32_t ScreenSessionManagerStub::GetVirtualScreenFlagFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ProcGetVirtualScreenFlag(data, reply);
    return 0;
}

int32_t ScreenSessionManagerStub::SetVirtualScreenFlagFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ProcSetVirtualScreenFlag(data, reply);
    return 0;
}

int32_t ScreenSessionManagerStub::GetDeviceScreenConfigFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    if (!RSMarshallingHelper::Marshalling(reply, GetDeviceScreenConfig())) {
        TLOGE(WmsLogTag::DMS, "Write deviceScreenConfig failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::SetVirtualScreenRefreshRateFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t refreshInterval = data.ReadUint32();
    DMError ret = SetVirtualScreenRefreshRate(screenId, refreshInterval);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::SwitchUserFunc(
    MessageParcel& data, MessageParcel& reply, MessageOption &option)
{
    SwitchUser();
    return 0;
}

} // namespace OHOS::Rosen
