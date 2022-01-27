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

#include "display_manager_proxy.h"

#include <cinttypes>
#include <ipc_types.h>

#include "window_manager_hilog.h"

#include <parcel.h>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerProxy"};
}

DisplayId DisplayManagerProxy::GetDefaultDisplayId()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDefaultDisplayId: remote is nullptr");
        return DISPLAY_ID_INVALD;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDefaultDisplayId: WriteInterfaceToken failed");
        return DISPLAY_ID_INVALD;
    }
    if (remote->SendRequest(TRANS_ID_GET_DEFAULT_DISPLAY_ID, data, reply, option) != ERR_NONE) {
        WLOGFW("GetDefaultDisplayId: SendRequest failed");
        return DISPLAY_ID_INVALD;
    }

    DisplayId displayId = reply.ReadUint64();
    WLOGFI("DisplayManagerProxy::GetDefaultDisplayId %" PRIu64"", displayId);
    return displayId;
}

DisplayInfo DisplayManagerProxy::GetDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDisplayInfoById: remote is nullptr");
        return DisplayInfo();
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDisplayInfoById: WriteInterfaceToken failed");
        return DisplayInfo();
    }
    data.WriteUint64(displayId);
    if (remote->SendRequest(TRANS_ID_GET_DISPLAY_BY_ID, data, reply, option) != ERR_NONE) {
        WLOGFW("GetDisplayInfoById: SendRequest failed");
        return DisplayInfo();
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDisplayInfoById SendRequest nullptr.");
        return DisplayInfo();
    }
    return *info;
}

DMError DisplayManagerProxy::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetScreenActiveMode: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SetScreenActiveMode: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    data.WriteUint64(static_cast<uint64_t>(screenId));
    data.WriteUint32(static_cast<uint32_t>(modeId));
    if (remote->SendRequest(TRANS_ID_SET_SCREEN_ACTIVE_MODE, data, reply, option) != ERR_NONE) {
        WLOGFW("SetScreenActiveMode: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    DMError res = static_cast<DMError>(reply.ReadInt32());
    return res;
}

std::vector<RSScreenModeInfo> DisplayManagerProxy::GetScreenSupportedModes(ScreenId screenId)
{
    std::vector<RSScreenModeInfo> screenSupportedModes;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetScreenSupportedModes: remote is nullptr");
        return screenSupportedModes;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("GetScreenSupportedModes: WriteInterfaceToken failed");
        return screenSupportedModes;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    data.WriteUint64(static_cast<uint64_t>(screenId));
    if (remote->SendRequest(TRANS_ID_GET_SCREEN_SUPPORTED_MODES, data, reply, option) != NO_ERROR) {
        WLOGFW("GetScreenSupportedModes: SendRequest failed");
        return screenSupportedModes;
    }

    uint32_t modeCount = reply.ReadUint32();
    screenSupportedModes.resize(modeCount);
    for (uint32_t modeIndex = 0; modeIndex < modeCount; modeIndex++) {
        sptr<RSScreenModeInfo> itemScreenMode = reply.ReadParcelable<RSScreenModeInfo>();
        if (itemScreenMode == nullptr) {
            break;
        } else {
            screenSupportedModes[modeIndex] = *itemScreenMode;
        }
    }
    return screenSupportedModes;
}

ScreenId DisplayManagerProxy::CreateVirtualScreen(VirtualScreenOption virtualOption)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::CreateVirtualScreen: remote is nullptr");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DisplayManagerProxy::CreateVirtualScreen: WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    bool res = data.WriteString(virtualOption.name_) && data.WriteUint32(virtualOption.width_) &&
        data.WriteUint32(virtualOption.height_) && data.WriteFloat(virtualOption.density_) &&
        data.WriteRemoteObject(virtualOption.surface_->GetProducer()->AsObject()) &&
        data.WriteInt32(virtualOption.flags_);
    if (!res) {
        WLOGFE("DisplayManagerProxy::Write data failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(TRANS_ID_CREATE_VIRTUAL_SCREEN, data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::CreateVirtualScreen: SendRequest failed");
        return SCREEN_ID_INVALID;
    }

    ScreenId screenId = static_cast<ScreenId>(reply.ReadUint64());
    WLOGFI("DisplayManagerProxy::CreateVirtualScreen %" PRIu64"", screenId);
    return screenId;
}

DMError DisplayManagerProxy::DestroyVirtualScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::DestroyVirtualScreen: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DisplayManagerProxy::DestroyVirtualScreen: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    data.WriteUint64(static_cast<uint64_t>(screenId));
    if (remote->SendRequest(TRANS_ID_DESTROY_VIRTUAL_SCREEN, data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::DestroyVirtualScreen: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

std::shared_ptr<Media::PixelMap> DisplayManagerProxy::GetDispalySnapshot(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDispalySnapshot: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDispalySnapshot: WriteInterfaceToken failed");
        return nullptr;
    }

    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write dispalyId failed");
        return nullptr;
    }

    if (remote->SendRequest(TRANS_ID_GET_DISPLAY_SNAPSHOT, data, reply, option) != ERR_NONE) {
        WLOGFW("GetDispalySnapshot: SendRequest failed");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    if (pixelMap == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDispalySnapshot SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

bool DisplayManagerProxy::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IDisplayManagerAgent failed");
        return false;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return false;
    }

    if (Remote()->SendRequest(TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return false;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return false;
    }

    if (Remote()->SendRequest(TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::WakeUpBegin(PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_WAKE_UP_BEGIN, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::WakeUpEnd()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_WAKE_UP_END, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendBegin(PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SUSPEND_BEGIN, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendEnd()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SUSPEND_END, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SET_SCREEN_POWER_FOR_ALL, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetDisplayState(DisplayState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayState failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SET_DISPLAY_STATE, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

DisplayState DisplayManagerProxy::GetDisplayState(DisplayId displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DisplayState::UNKNOWN;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return DisplayState::UNKNOWN;
    }
    if (Remote()->SendRequest(TRANS_ID_GET_DISPLAY_STATE, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DisplayState::UNKNOWN;
    }
    return static_cast<DisplayState>(reply.ReadUint32());
}

void DisplayManagerProxy::NotifyDisplayEvent(DisplayEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write DisplayEvent failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_NOTIFY_DISPLAY_EVENT, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return;
    }
}

DMError DisplayManagerProxy::AddMirror(ScreenId mainScreenId, ScreenId mirrorScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::AddMirror: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DisplayManagerProxy::AddMirror: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(mainScreenId)) &&
        data.WriteUint64(static_cast<uint64_t>(mirrorScreenId));
    if (!res) {
        WLOGFE("DisplayManagerProxy::AddMirror: data write failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(TRANS_ID_ADD_MIRROR, data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::AddMirror: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    WLOGFI("DisplayManagerProxy::AddMirror");
    return static_cast<DMError>(reply.ReadInt32());
}
} // namespace OHOS::Rosen