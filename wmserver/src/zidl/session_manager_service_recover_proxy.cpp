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

#include "zidl/session_manager_service_recover_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerServiceRecoverProxy"};
}

void SessionManagerServiceRecoverProxy::OnSessionManagerServiceRecover(
    const sptr<IRemoteObject>& sessionManagerService)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[RECOVER] WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteRemoteObject(sessionManagerService)) {
        WLOGFE("[RECOVER] WriteRemoteObject failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[RECOVER] SendRequest failed");
        return;
    }
}

} // namespace Rosen
} // namespace OHOS