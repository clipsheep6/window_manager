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

const ScreenSessionManagerStub::ProcFuncMap ScreenSessionManagerStub::procFuncMap_ = {
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO,
        &ScreenSessionManagerStub::ProcGetDefaultDisplayInfo},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        &ScreenSessionManagerStub::ProcRegisterDisplayManagerAgent},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        &ScreenSessionManagerStub::ProcUnregisterDisplayManagerAgent},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN,
        &ScreenSessionManagerStub::ProcWakeUpBegin},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_WAKE_UP_END,
        &ScreenSessionManagerStub::ProcWakeUpEnd},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN,
        &ScreenSessionManagerStub::ProcSuspendBegin},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SUSPEND_END,
        &ScreenSessionManagerStub::ProcSuspendEnd},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE,
        &ScreenSessionManagerStub::ProcSetDisplayState},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER,
        &ScreenSessionManagerStub::ProcSetSpecifiedScreenPower},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL,
        &ScreenSessionManagerStub::ProcSetScreenPowerForAll},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE,
        &ScreenSessionManagerStub::ProcGetDisplayState},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT,
        &ScreenSessionManagerStub::ProcNotifyDisplayEvent},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER,
        &ScreenSessionManagerStub::ProcGetScreenPower},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID,
        &ScreenSessionManagerStub::ProcGetDisplayById},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN,
        &ScreenSessionManagerStub::ProcGetDisplayByScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS,
        &ScreenSessionManagerStub::ProcGetAllDisplayIds},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID,
        &ScreenSessionManagerStub::ProcGetScreenInfoById},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS,
        &ScreenSessionManagerStub::ProcGetAllScreenInfos},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS,
        &ScreenSessionManagerStub::ProcScreenGetSupportedColorGamuts},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN,
        &ScreenSessionManagerStub::ProcCreateVirtualScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE,
        &ScreenSessionManagerStub::ProcSetVirtualScreenSurface},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_CANVAS_ROTATION,
        &ScreenSessionManagerStub::ProcSetVirtualScreenCanvasRotation},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SCALE_MODE,
        &ScreenSessionManagerStub::ProcSetVirtualScreenScaleMode},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN,
        &ScreenSessionManagerStub::ProcDestoryVitrualScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR,
        &ScreenSessionManagerStub::ProcScreenMakeMirror},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR,
        &ScreenSessionManagerStub::ProcScreenStopMirror},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_DISABLE_MIRROR,
        &ScreenSessionManagerStub::ProcScreenDisableMirror},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND,
        &ScreenSessionManagerStub::ProcScreenMakeExpand},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND,
        &ScreenSessionManagerStub::ProcScreenStopExpand},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID,
        &ScreenSessionManagerStub::ProcGetScreenGroupInfoById},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP,
        &ScreenSessionManagerStub::ProcRemoveVirtualScreenFromScreenGroup},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT,
        &ScreenSessionManagerStub::ProcGetDisplaySnapshot},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE,
        &ScreenSessionManagerStub::ProcSetScreenActiveMode},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO,
        &ScreenSessionManagerStub::ProcSetVirtualPixelRatio},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM,
        &ScreenSessionManagerStub::ProcSetVirtualPixelRatioSystem},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_RESOLUTION,
        &ScreenSessionManagerStub::ProcSetResolution},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION,
        &ScreenSessionManagerStub::ProcGetDensityInCurrentResolution},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT,
        &ScreenSessionManagerStub::ProcScreenGetColorGamut},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT,
        &ScreenSessionManagerStub::ProcScreenSetColorGamut},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP,
        &ScreenSessionManagerStub::ProcScreenGetGamutMap},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP,
        &ScreenSessionManagerStub::ProcScreenSetGamutMap},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM,
        &ScreenSessionManagerStub::ProScreenSetColorTransform},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_PIXEL_FORMAT,
        &ScreenSessionManagerStub::ProcScreenGetPixelFormat},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_PIXEL_FORMAT,
        &ScreenSessionManagerStub::ProcScreenSetPixelFormat},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_HDR_FORMAT,
        &ScreenSessionManagerStub::ProcScreenGetSupportedHDRFormat},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_HDR_FORMAT,
        &ScreenSessionManagerStub::ProcScreenGetHDRFormat},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_HDR_FORMAT,
        &ScreenSessionManagerStub::ProcScreenSetHDRFormat},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_SPACE,
        &ScreenSessionManagerStub::ProcScreenGetSupprtedColorSpace},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_SPACE,
        &ScreenSessionManagerStub::ProcScreenGetColorSpace},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_SPACE,
        &ScreenSessionManagerStub::ProcScreenSetColorSpace},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_ORIENTATION,
        &ScreenSessionManagerStub::ProcSetOrientation},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED,
        &ScreenSessionManagerStub::ProcSetScreenRotationLocked},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED,
        &ScreenSessionManagerStub::ProcIsScreenRotationLocked},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO,
        &ScreenSessionManagerStub::ProcGetCutoutInfo},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW,
        &ScreenSessionManagerStub::ProcHasPrivateWindow},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_CONVERT_SCREENID_TO_RSSCREENID,
        &ScreenSessionManagerStub::ProcConvertScreenIdToRsScreenId},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_HAS_IMMERSIVE_WINDOW,
        &ScreenSessionManagerStub::ProcHasImmersiveWindow},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_ALL_SCREEN,
        &ScreenSessionManagerStub::ProcSceneBoardDumpAllScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_SPECIAL_SCREEN,
        &ScreenSessionManagerStub::ProcSceneBoardDumpSpecialScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_DEVICE_IS_CAPTURE,
        &ScreenSessionManagerStub::ProcDeviceIsCapture},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE,
        &ScreenSessionManagerStub::ProcSceneBoardSetFoldDisplayMode},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_LOCK_FOLD_DISPLAY_STATUS,
        &ScreenSessionManagerStub::ProcSceneBoardLockFoldDisplayStatus},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE,
        &ScreenSessionManagerStub::ProcSceneBoardGetFoldDisplayMode},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE,
        &ScreenSessionManagerStub::ProcSceneBoardIsFoldable},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS,
        &ScreenSessionManagerStub::ProcSceneBoardGetFoldStatus},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_CURRENT_FOLD_CREASE_REGION,
        &ScreenSessionManagerStub::ProcSceneBoardGetCurrentFoldCreaseRegion},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN,
        &ScreenSessionManagerStub::ProcSceneBoardMakeUniqueScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_CLIENT,
        &ScreenSessionManagerStub::ProcSetClient},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_SCREEN_PROPERTY,
        &ScreenSessionManagerStub::ProcGetScreenProperty},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_NODE,
        &ScreenSessionManagerStub::ProcGetDisplayNode},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_UPDATE_SCREEN_ROTATION_PROPERTY,
        &ScreenSessionManagerStub::ProcUpdateScreenRotationProperty},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_CURVED_SCREEN_COMPRESSION_AREA,
        &ScreenSessionManagerStub::ProcGetCurvedScreenCompressionArea},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_PHY_SCREEN_PROPERTY,
        &ScreenSessionManagerStub::ProcGetPhyScreenProperty},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO,
        &ScreenSessionManagerStub::ProcNotifyDisplayChangeInfo},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_STATE,
        &ScreenSessionManagerStub::ProcSetScreenPrivacyState},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREENID_PRIVACY_STATE,
        &ScreenSessionManagerStub::ProcSetScreenIdPrivacyState},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_WINDOW_LIST,
        &ScreenSessionManagerStub::ProcSetScreenPrivacyWindowList},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN,
        &ScreenSessionManagerStub::ProcResizeVirtualScreen},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_UPDATE_AVAILABLE_AREA,
        &ScreenSessionManagerStub::ProcUpdateAvailableArea},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_SCREEN_OFF_DELAY_TIME,
        &ScreenSessionManagerStub::ProcSetScreenOffDelayTime},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_NOTIFY_FOLD_TO_EXPAND_COMPLETION,
        &ScreenSessionManagerStub::ProcNotifyFoldToExpandCompletion},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_DEVICE_SCREEN_CONFIG,
        &ScreenSessionManagerStub::ProcGetDeviceScreenConfig},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_REFRESH_RATE,
        &ScreenSessionManagerStub::ProcSetVirtualScreenRefreshRate},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SWITCH_USER,
        &ScreenSessionManagerStub::ProcSwitchUser},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_SNAPSHOT_BY_PICKER,
        &ScreenSessionManagerStub::ProcGetSnapshotByPicker},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_AVAILABLE_AREA,
        &ScreenSessionManagerStub::ProcGetAvailableArea},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_GET_VIRTUAL_SCREEN_FLAG,
        &ScreenSessionManagerStub::ProcGetVirtualScreenFlag},
    {ScreenSessionManagerStub::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_FLAG,
        &ScreenSessionManagerStub::ProcSetVirtualScreenFlag},
};

int32_t ScreenSessionManagerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return ERR_INVALID_DATA;
    }
    DisplayManagerMessage msgId = static_cast<DisplayManagerMessage>(code);
    auto func = procFuncMap_.find(msgId);
    if (func == procFuncMap_.end()) {
        WLOGFW("unknown transaction code");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return (this->*(func->second))(data, reply);
}

int32_t ScreenSessionManagerStub::ProcGetAvailableArea(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    DMRect area;
    DMError ret = GetAvailableArea(displayId, area);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteInt32(area.posX_);
    reply.WriteInt32(area.posY_);
    reply.WriteUint32(area.width_);
    reply.WriteUint32(area.height_);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetSnapshotByPicker(MessageParcel& data, MessageParcel& reply)
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
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    VirtualScreenFlag screenFlag = static_cast<VirtualScreenFlag>(data.ReadUint32());
    DMError setRet = SetVirtualScreenFlag(screenId, screenFlag);
    reply.WriteInt32(static_cast<int32_t>(setRet));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    VirtualScreenFlag screenFlag = GetVirtualScreenFlag(screenId);
    reply.WriteUint32(static_cast<uint32_t>(screenFlag));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDefaultDisplayInfo(MessageParcel& data, MessageParcel& reply)
{
    auto info = GetDefaultDisplayInfo();
    reply.WriteParcelable(info);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcRegisterDisplayManagerAgent(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcUnregisterDisplayManagerAgent(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcWakeUpBegin(MessageParcel& data, MessageParcel& reply)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(WakeUpBegin(reason));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcWakeUpEnd(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteBool(WakeUpEnd());
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSuspendBegin(MessageParcel& data, MessageParcel& reply)
{
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SuspendBegin(reason));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSuspendEnd(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteBool(SuspendEnd());
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetDisplayState(MessageParcel& data, MessageParcel& reply)
{
    DisplayState state = static_cast<DisplayState>(data.ReadUint32());
    reply.WriteBool(SetDisplayState(state));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetSpecifiedScreenPower(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint32());
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetSpecifiedScreenPower(screenId, state, reason));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetScreenPowerForAll(MessageParcel& data, MessageParcel& reply)
{
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetScreenPowerForAll(state, reason));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDisplayState(MessageParcel& data, MessageParcel& reply)
{
    DisplayState state = GetDisplayState(data.ReadUint64());
    reply.WriteUint32(static_cast<uint32_t>(state));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcNotifyDisplayEvent(MessageParcel& data, MessageParcel& reply)
{
    DisplayEvent event = static_cast<DisplayEvent>(data.ReadUint32());
    NotifyDisplayEvent(event);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetScreenPower(MessageParcel& data, MessageParcel& reply)
{
    ScreenId dmsScreenId;
    if (!data.ReadUint64(dmsScreenId)) {
        WLOGFE("fail to read dmsScreenId.");
        return ERR_INVALID_DATA;
    }
    reply.WriteUint32(static_cast<uint32_t>(GetScreenPower(dmsScreenId)));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDisplayById(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = data.ReadUint64();
    auto info = GetDisplayInfoById(displayId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDisplayByScreen(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = data.ReadUint64();
    auto info = GetDisplayInfoByScreen(screenId);
    reply.WriteParcelable(info);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetAllDisplayIds(MessageParcel& data, MessageParcel& reply)
{
    std::vector<DisplayId> allDisplayIds = GetAllDisplayIds();
    reply.WriteUInt64Vector(allDisplayIds);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetScreenInfoById(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenInfo = GetScreenInfoById(screenId);
    reply.WriteStrongParcelable(screenInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetAllScreenInfos(MessageParcel& data, MessageParcel& reply)
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    DMError ret  = GetAllScreenInfos(screenInfos);
    reply.WriteInt32(static_cast<int32_t>(ret));
    if (!MarshallingHelper::MarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos)) {
        WLOGE("fail to marshalling screenInfos in stub.");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenGetSupportedColorGamuts(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcCreateVirtualScreen(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcSetVirtualScreenSurface(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcSetVirtualScreenCanvasRotation(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    bool autoRotate = data.ReadBool();
    DMError result = SetVirtualMirrorScreenCanvasRotation(screenId, autoRotate);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetVirtualScreenScaleMode(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(data.ReadUint32());
    DMError result = SetVirtualMirrorScreenScaleMode(screenId, scaleMode);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcDestoryVitrualScreen(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError result = DestroyVirtualScreen(screenId);
    reply.WriteInt32(static_cast<int32_t>(result));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenMakeMirror(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenStopMirror(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenDisableMirror(MessageParcel& data, MessageParcel& reply)
{
    DMError ret = DisableMirror(data.ReadBool());
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenMakeExpand(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenStopExpand(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcGetScreenGroupInfoById(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenGroupInfo = GetScreenGroupInfoById(screenId);
    reply.WriteStrongParcelable(screenGroupInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcRemoveVirtualScreenFromScreenGroup(MessageParcel& data, MessageParcel& reply)
{
    std::vector<ScreenId> screenId;
    if (!data.ReadUInt64Vector(&screenId)) {
        WLOGE("fail to receive screens in stub.");
        return 0;
    }
    RemoveVirtualScreenFromGroup(screenId);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDisplaySnapshot(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = data.ReadUint64();
    std::shared_ptr<Media::PixelMap> displaySnapshot = GetDisplaySnapshot(displayId);
    reply.WriteParcelable(displaySnapshot == nullptr ? nullptr : displaySnapshot.get());
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetScreenActiveMode(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t modeId = data.ReadUint32();
    DMError ret = SetScreenActiveMode(screenId, modeId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetVirtualPixelRatio(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetVirtualPixelRatioSystem(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetVirtualPixelRatioSystem(screenId, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetResolution(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    float virtualPixelRatio = data.ReadFloat();
    DMError ret = SetResolution(screenId, width, height, virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDensityInCurrentResolution(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio;
    DMError ret = GetDensityInCurResolution(screenId, virtualPixelRatio);
    reply.WriteFloat(virtualPixelRatio);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenGetColorGamut(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenSetColorGamut(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t colorGamutIdx = data.ReadInt32();
    DMError ret = SetScreenColorGamut(screenId, colorGamutIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenGetGamutMap(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenSetGamutMap(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenGamutMap gamutMap = static_cast<ScreenGamutMap>(data.ReadUint32());
    DMError ret = SetScreenGamutMap(screenId, gamutMap);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProScreenSetColorTransform(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    DMError ret = SetScreenColorTransform(screenId);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenGetPixelFormat(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenSetPixelFormat(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(data.ReadUint32());
    DMError ret = SetPixelFormat(screenId, pixelFormat);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenGetSupportedHDRFormat(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenGetHDRFormat(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenSetHDRFormat(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    int32_t modeIdx = data.ReadInt32();
    DMError ret = SetScreenHDRFormat(screenId, modeIdx);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcScreenGetSupprtedColorSpace(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenGetColorSpace(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcScreenSetColorSpace(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(data.ReadUint32());
    DMError ret = SetScreenColorSpace(screenId, colorSpace);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetOrientation(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    Orientation orientation = static_cast<Orientation>(data.ReadUint32());
    DMError ret = SetOrientation(screenId, orientation);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetScreenRotationLocked(MessageParcel& data, MessageParcel& reply)
{
    bool isLocked = static_cast<bool>(data.ReadBool());
    DMError ret = SetScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcIsScreenRotationLocked(MessageParcel& data, MessageParcel& reply)
{
    bool isLocked = false;
    DMError ret = IsScreenRotationLocked(isLocked);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(isLocked);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetCutoutInfo(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    sptr<CutoutInfo> cutoutInfo = GetCutoutInfo(displayId);
    reply.WriteParcelable(cutoutInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcHasPrivateWindow(MessageParcel& data, MessageParcel& reply)
{
    DisplayId id = static_cast<DisplayId>(data.ReadUint64());
    bool hasPrivateWindow = false;
    DMError ret = HasPrivateWindow(id, hasPrivateWindow);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(hasPrivateWindow);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcConvertScreenIdToRsScreenId(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenId rsId = SCREEN_ID_INVALID;
    bool ret = ConvertScreenIdToRsScreenId(screenId, rsId);
    reply.WriteBool(ret);
    reply.WriteUint64(rsId);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcHasImmersiveWindow(MessageParcel& data, MessageParcel& reply)
{
    bool immersive = false;
    DMError ret = HasImmersiveWindow(immersive);
    reply.WriteInt32(static_cast<int32_t>(ret));
    reply.WriteBool(immersive);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardDumpAllScreen(MessageParcel& data, MessageParcel& reply)
{
    std::string dumpInfo;
    DumpAllScreensInfo(dumpInfo);
    reply.WriteString(dumpInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardDumpSpecialScreen(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    std::string dumpInfo;
    DumpSpecialScreenInfo(screenId, dumpInfo);
    reply.WriteString(dumpInfo);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcDeviceIsCapture(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteBool(IsCaptured());
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardSetFoldDisplayMode(MessageParcel& data, MessageParcel& reply)
{
    FoldDisplayMode displayMode = static_cast<FoldDisplayMode>(data.ReadUint32());
    SetFoldDisplayMode(displayMode);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardLockFoldDisplayStatus(MessageParcel& data, MessageParcel& reply)
{
    bool lockDisplayStatus = static_cast<bool>(data.ReadUint32());
    SetFoldStatusLocked(lockDisplayStatus);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardGetFoldDisplayMode(MessageParcel& data, MessageParcel& reply)
{
    FoldDisplayMode displayMode = GetFoldDisplayMode();
    reply.WriteUint32(static_cast<uint32_t>(displayMode));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardIsFoldable(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteBool(IsFoldable());
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardGetFoldStatus(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteUint32(static_cast<uint32_t>(GetFoldStatus()));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardGetCurrentFoldCreaseRegion(MessageParcel& data, MessageParcel& reply)
{
    reply.WriteStrongParcelable(GetCurrentFoldCreaseRegion());
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSceneBoardMakeUniqueScreen(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcSetClient(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcGetScreenProperty(MessageParcel& data, MessageParcel& reply)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    if (!RSMarshallingHelper::Marshalling(reply, GetScreenProperty(screenId))) {
        WLOGFE("Write screenProperty failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDisplayNode(MessageParcel& data, MessageParcel& reply)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto displayNode = GetDisplayNode(screenId);
    if (!displayNode || !displayNode->Marshalling(reply)) {
        WLOGFE("Write displayNode failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::ProcUpdateScreenRotationProperty(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcGetCurvedScreenCompressionArea(MessageParcel& data, MessageParcel& reply)
{
    auto area = GetCurvedCompressionArea();
    reply.WriteUint32(area);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetPhyScreenProperty(MessageParcel& data, MessageParcel& reply)
{
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    if (!RSMarshallingHelper::Marshalling(reply, GetPhyScreenProperty(screenId))) {
        WLOGFE("Write screenProperty failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::ProcNotifyDisplayChangeInfo(MessageParcel& data, MessageParcel& reply)
{
    sptr<DisplayChangeInfo> info = DisplayChangeInfo::Unmarshalling(data);
    if (!info) {
        WLOGFE("Read DisplayChangeInfo failed");
        return ERR_INVALID_DATA;
    }
    NotifyDisplayChangeInfoChanged(info);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetScreenPrivacyState(MessageParcel& data, MessageParcel& reply)
{
    auto hasPrivate = data.ReadBool();
    SetScreenPrivacyState(hasPrivate);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetScreenIdPrivacyState(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    auto hasPrivate = data.ReadBool();
    SetPrivacyStateByDisplayId(displayId, hasPrivate);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetScreenPrivacyWindowList(MessageParcel& data, MessageParcel& reply)
{
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    std::vector<std::string> privacyWindowList;
    data.ReadStringVector(&privacyWindowList);
    SetScreenPrivacyWindowList(displayId, privacyWindowList);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcResizeVirtualScreen(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    DMError ret = ResizeVirtualScreen(screenId, width, height);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcUpdateAvailableArea(MessageParcel& data, MessageParcel& reply)
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

int32_t ScreenSessionManagerStub::ProcSetScreenOffDelayTime(MessageParcel& data, MessageParcel& reply)
{
    int32_t delay = data.ReadInt32();
    int32_t ret = SetScreenOffDelayTime(delay);
    reply.WriteInt32(ret);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcNotifyFoldToExpandCompletion(MessageParcel& data, MessageParcel& reply)
{
    bool foldToExpand = data.ReadBool();
    NotifyFoldToExpandCompletion(foldToExpand);
    return 0;
}

int32_t ScreenSessionManagerStub::ProcGetDeviceScreenConfig(MessageParcel& data, MessageParcel& reply)
{
    if (!RSMarshallingHelper::Marshalling(reply, GetDeviceScreenConfig())) {
        TLOGE(WmsLogTag::DMS, "Write deviceScreenConfig failed");
    }
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSetVirtualScreenRefreshRate(MessageParcel& data, MessageParcel& reply)
{
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint64());
    uint32_t refreshInterval = data.ReadUint32();
    DMError ret = SetVirtualScreenRefreshRate(screenId, refreshInterval);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return 0;
}

int32_t ScreenSessionManagerStub::ProcSwitchUser(MessageParcel& data, MessageParcel& reply)
{
    SwitchUser();
    return 0;
}
} // namespace OHOS::Rosen
