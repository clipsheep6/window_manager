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

#include "session/container/include/zidl/session_stage_proxy.h"
#include "session/container/include/zidl/session_stage_ipc_interface_code.h"

#include <cstdint>
#include <ipc_types.h>
#include <message_option.h>
#include <message_parcel.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStageProxy"};
}

WSError SessionStageProxy::SetActive(bool active)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteBool(active)) {
        WLOGFE("Write active failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_ACTIVE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionStageProxy::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!(data.WriteInt32(rect.posX_) && data.WriteInt32(rect.posY_) &&
        data.WriteUint32(rect.width_) && data.WriteUint32(rect.height_))) {
        WLOGFE("Write WindowRect failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write SessionSizeChangeReason failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    bool hasRSTransaction = rsTransaction != nullptr;
    if (!data.WriteBool(hasRSTransaction)) {
        WLOGFE("Write has transaction failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (hasRSTransaction) {
        if (!data.WriteParcelable(rsTransaction.get())) {
            WLOGFE("Write transaction sync Id failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SIZE_CHANGE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

void SessionStageProxy::UpdateDensity()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_CHANGE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SessionStageProxy::HandleBackEvent()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_HANDLE_BACK_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionStageProxy::MarkProcessed(int32_t eventId)
{
    return WSError::WS_DO_NOTHING;
}

WSError SessionStageProxy::NotifyDestroy()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DESTROY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

void SessionStageProxy::NotifyTouchDialogTarget()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TOUCH_DIALOG_TARGET),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SessionStageProxy::UpdateFocus(bool focus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteBool(focus)) {
        WLOGFE("Write focus failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_FOCUS_CHANGE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionStageProxy::NotifyTransferComponentData(const AAFwk::WantParams& wantParams)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteParcelable(&wantParams)) {
        WLOGFE("wantParams write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

void SessionStageProxy::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(info.GetRefPtr())) {
        WLOGFE("occupied info write failed.");
        return;
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_OCCUPIED_AREA_CHANGE_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
    return;
}

WSError SessionStageProxy::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteStrongParcelable(avoidArea)) {
        WLOGFE("Write AvoidArea failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write AvoidAreaType failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_UPDATE_AVOID_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return WSError::WS_OK;
}

void SessionStageProxy::DumpSessionElementInfo(const std::vector<std::string>& params)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteStringVector(params)) {
        WLOGFE("Write params failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_DUMP_SESSSION_ELEMENT_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void SessionStageProxy::NotifyScreenshot()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SCREEN_SHOT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SessionStageProxy::NotifyTouchOutside()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TOUCH_OUTSIDE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return WSError::WS_OK;
}

WSError SessionStageProxy::UpdateWindowMode(WindowMode mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(mode))) {
        WLOGFE("Write mode failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_WINDOW_MODE_CHANGE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

void SessionStageProxy::NotifyForegroundInteractiveStatus(bool interactive)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteBool(interactive)) {
        WLOGFE("Write interactive failed");
        return;
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_FOREGROUND_INTERACTIVE_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void SessionStageProxy::NotifyConfigurationUpdated()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_CONFIGURATION_UPDATED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

} // namespace OHOS::Rosen
