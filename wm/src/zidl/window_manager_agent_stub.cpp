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

#include "zidl/window_manager_agent_stub.h"
#include "ipc_skeleton.h"
#include "marshalling_helper.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerAgentStub"};
}

int WindowManagerAgentStub::ProcUpdateFocus(MessageParcel& data)
{
    sptr<FocusChangeInfo> info = data.ReadParcelable<FocusChangeInfo>();
    UpdateFocusChangeInfo(info, data.ReadBool());
    return 0;
}

int WindowManagerAgentStub::ProcUpdateWindowModeType(MessageParcel& data)
{
    UpdateWindowModeTypeInfo(static_cast<WindowModeType>(data.ReadUint8()));
    return 0;
}

int WindowManagerAgentStub::ProcUpdateSystemBarProps(MessageParcel& data)
{
    DisplayId displayId = data.ReadUint64();
    SystemBarRegionTints tints;
    bool res = MarshallingHelper::UnmarshallingVectorObj<SystemBarRegionTint>(data, tints,
        [](Parcel& parcel, SystemBarRegionTint& tint) {
            uint32_t type;
            SystemBarProperty prop;
            Rect region;
            bool res = parcel.ReadUint32(type) && parcel.ReadBool(prop.enable_) &&
                parcel.ReadUint32(prop.backgroundColor_) && parcel.ReadUint32(prop.contentColor_) &&
                parcel.ReadInt32(region.posX_) && parcel.ReadInt32(region.posY_) &&
                parcel.ReadUint32(region.width_) && parcel.ReadUint32(region.height_);
            tint.type_ = static_cast<WindowType>(type);
            tint.prop_ = prop;
            tint.region_ = region;
            return res;
        }
    );
    if (!res) {
        WLOGFE("fail to read SystemBarRegionTints.");
    } else {
        UpdateSystemBarRegionTints(displayId, tints);
    }
    return 0;
}

int WindowManagerAgentStub::ProcUpdateWindowStatus(MessageParcel& data)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<AccessibilityWindowInfo>(data, infos)) {
        WLOGFE("read accessibility window infos failed");
        return -1;
    }
    NotifyAccessibilityWindowInfo(infos, static_cast<WindowUpdateType>(data.ReadUint32()));
    return 0;
}

int WindowManagerAgentStub::ProcUpdateWindowVisibility(MessageParcel& data)
{
    std::vector<sptr<WindowVisibilityInfo>> infos;
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<WindowVisibilityInfo>(data, infos)) {
        WLOGFE("fail to read WindowVisibilityInfo.");
    } else {
        UpdateWindowVisibilityInfo(infos);
    }
    return 0;
}

int WindowManagerAgentStub::ProcUpdateWindowDrawingState(MessageParcel& data)
{
    std::vector<sptr<WindowDrawingContentInfo>> infos;
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<WindowDrawingContentInfo>(data, infos)) {
        WLOGFE("fail to read WindowDrawingContentInfo.");
    } else {
        UpdateWindowDrawingContentInfo(infos);
    }
    return 0;
}

int WindowManagerAgentStub::ProcUpdateCameraFloat(MessageParcel& data)
{
    UpdateCameraFloatWindowStatus(data.ReadUint32(), data.ReadBool());
    return 0;
}

int WindowManagerAgentStub::ProcUpdateWaterMarkFlag(MessageParcel& data)
{
    NotifyWaterMarkFlagChangedResult(data.ReadBool());
    return 0;
}

int WindowManagerAgentStub::ProcUpdateGestureNavigationEnabled(MessageParcel& data)
{
    NotifyGestureNavigationEnabledResult(data.ReadBool());
    return 0;
}

int WindowManagerAgentStub::ProcUpdateCameraWindowStatus(MessageParcel& data)
{
    UpdateCameraWindowStatus(data.ReadUint32(), data.ReadBool());
    return 0;
}

int WindowManagerAgentStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    WLOGFD("code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    WindowManagerAgentMsg msgId = static_cast<WindowManagerAgentMsg>(code);
    switch (msgId) {
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_FOCUS:
            return ProcUpdateFocus(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_MODE_TYPE:
            return ProcUpdateWindowModeType(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_SYSTEM_BAR_PROPS:
            return ProcUpdateSystemBarProps(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_STATUS:
            return ProcUpdateWindowStatus(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_VISIBILITY:
            return ProcUpdateWindowVisibility(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_DRAWING_STATE:
            return ProcUpdateWindowDrawingState(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_CAMERA_FLOAT:
            return ProcUpdateCameraFloat(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_WATER_MARK_FLAG:
            return ProcUpdateWaterMarkFlag(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_GESTURE_NAVIGATION_ENABLED:
            return ProcUpdateGestureNavigationEnabled(data);
        case WindowManagerAgentMsg::TRANS_ID_UPDATE_CAMERA_WINDOW_STATUS:
            return ProcUpdateCameraWindowStatus(data);
        default:
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
