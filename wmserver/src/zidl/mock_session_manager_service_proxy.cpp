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

#include "zidl/mock_session_manager_service_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerServiceProxy"};
}
sptr<IRemoteObject> MockSessionManagerServiceProxy::GetSessionManagerService()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_GET_SESSION_MANAGER_SERVICE),
        data, reply, option) != ERR_NONE) {
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = reply.ReadRemoteObject();
    return remoteObject;
}

WMError MockSessionManagerServiceProxy::GetSessionDumpInfo(const std::vector<std::string>& params,
    SessionDumpInfo& dumpInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInterfaceToken(params)) {
        WLOGFE("WriteInterfaceToken params failed");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_GET_SESSION_DUMP_INFO), data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    sptr<SessionDumpInfo> pDumpInfo = reply.ReadParcelable<SessionDumpInfo>();
    dumpInfo = *pDumpInfo;
    return WMError::WM_ERROR_INVALID_PARAM;
}

WMError MockSessionManagerServiceProxy::GetSpecifiedSessionDumpInfo(const std::vector<std::string>& params,
    SpecifiedSessionDumpInfo& info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInterfaceToken(params)) {
        WLOGFE("WriteInterfaceToken params failed");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_GET_SPECIFIED_SSESSION_DUMP_INFO), data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    sptr<SpecifiedSessionDumpInfo> pDumpInfo = reply.ReadParcelable<SpecifiedSessionDumpInfo>();
    info = *pDumpInfo;
    return WMError::WM_ERROR_INVALID_PARAM;
}

} // namespace Rosen
} // namespace OHOS
