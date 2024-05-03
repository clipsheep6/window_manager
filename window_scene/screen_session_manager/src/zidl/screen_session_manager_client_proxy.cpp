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

#include "zidl/screen_session_manager_client_proxy.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DMS_SCREEN_CLIENT,
                                          "ScreenSessionManagerClientProxy" };
} // namespace

void ScreenSessionManagerClientProxy::OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
    ScreenId rsId, const std::string& name)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (CheckScreenIdWriteUint64(data, screenId)) { return; }
    if (CheckScreenEventWriteUnit8(data, screenEvent)) { return; }
    if (CheckRsIdWriteUint64(data, rsId)) { return; }
    if (CheckNameWriteString(data, name)) { return; }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_CONNECTION_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnPropertyChanged(ScreenId screenId,
    const ScreenProperty& property, ScreenPropertyChangeReason reason)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (CheckScreenIdWriteUint64(data, screenId)) { return; }
    if (!RSMarshallingHelper::Marshalling(data, property)) {
        WLOGFE("Write property failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write reason failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_PROPERTY_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write event failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(status))) {
        WLOGFE("Write status failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write reason failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
    auto remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SendRequest failed, Remote is nullptr");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_POWER_STATUS_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnSensorRotationChanged(ScreenId screenId, float sensorRotation)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (CheckScreenIdWriteUint64(data, screenId)) { return; }
    if (!data.WriteFloat(sensorRotation)) {
        WLOGFE("Write sensorRotation failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SENSOR_ROTATION_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenOrientationChanged(ScreenId screenId, float screenOrientation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteFloat(screenOrientation)) {
        WLOGFE("Write screenOrientation failed");
        return;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ORIENTATION_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (CheckScreenIdWriteUint64(data, screenId)) { return; }
    if (!data.WriteBool(isLocked)) {
        WLOGFE("Write isLocked failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ROTATION_LOCKED_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (!data.WriteUint64(defaultDisplayId)) {
        WLOGFE("Write defaultDisplayId failed");
        return;
    }
    if (!data.WriteStrongParcelable(displayInfo)) {
        WLOGFE("Write displayInfo failed");
        return;
    }
    auto mapSize = static_cast<uint32_t>(displayInfoMap.size());
    if (!data.WriteUint32(mapSize)) {
        WLOGFE("Write mapSize failed");
        return;
    }
    for (auto [id, info] : displayInfoMap) {
        if (!data.WriteUint64(id)) {
            WLOGFE("Write id failed");
            return;
        }
        if (!data.WriteStrongParcelable(info)) {
            WLOGFE("Write info failed");
            return;
        }
    }
    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write type failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_DISPLAY_STATE_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (!data.WriteUInt64Vector(missionIds)) {
        WLOGFE("Write missionIds failed");
        return;
    }
    if (!data.WriteUInt64Vector(surfaceNodeIds)) {
        WLOGFE("Write surfaceNodeIds failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(static_cast<uint32_t>(
        ScreenSessionManagerClientMessage::TRANS_ID_GET_SURFACENODEID_FROM_MISSIONID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
    reply.ReadUInt64Vector(&surfaceNodeIds);
}

void ScreenSessionManagerClientProxy::OnUpdateFoldDisplayMode(FoldDisplayMode displayMode)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (!data.WriteUint32(static_cast<uint32_t>(displayMode))) {
        WLOGFE("Write displayMode failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_SET_FOLD_DISPLAY_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenshot(DisplayId displayId)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_SHOT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnImmersiveStateChanged(bool& immersive)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_IMMERSIVE_STATE_CHANGED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
    immersive = reply.ReadBool();
}

void ScreenSessionManagerClientProxy::SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    MessageParcel data;
    if (CheckInterfaceToken(data)) { return; }
    if (CheckScreenIdWriteUint64(data, screenId)) { return; }
    if (!data.WriteUint64(displayNodeScreenId)) {
        WLOGFE("Write displayNodeScreenId failed");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_SET_DISPLAY_NODE_SCREEN_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

bool ScreenSessionManagerClientProxy::CheckInterfaceToken(MessageParcel& data)
{
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return true;
    } else {
        return false;
    }
}

bool ScreenSessionManagerClientProxy::CheckScreenIdWriteUint64(MessageParcel& data, ScreenId& screenId)
{
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return true;
    } else {
        return false;
    }
}

bool ScreenSessionManagerClientProxy::CheckScreenEventWriteUnit8(MessageParcel& data, ScreenEvent& screenEvent)
{
    if (!data.WriteUint8(static_cast<uint8_t>(screenEvent))) {
        WLOGFE("Write screenEvent failed");
        return true;
    } else {
        return false;
    }
}

bool ScreenSessionManagerClientProxy::CheckRsIdWriteUint64(MessageParcel& data, ScreenId& rsId)
{
    if (!data.WriteUint64(rsId)) {
        WLOGFE("Write rsId failed");
        return true;
    } else {
        return false;
    }
}

bool ScreenSessionManagerClientProxy::CheckNameWriteString(MessageParcel& data, const std::string& name)
{
    if (!data.WriteString(name)) {
        WLOGFE("Write name failed");
        return true;
    } else {
        return false;
void ScreenSessionManagerClientProxy::SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId) || !data.WriteFloat(virtualPixelRatio)) {
        WLOGFE("Write screenId/virtualPixelRatio failed");
        return;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}
} // namespace OHOS::Rosen
