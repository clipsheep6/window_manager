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

#include "session/host/include/extension_session.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSession" };
}

ExtensionSession::ExtensionSession(const SessionInfo& info) : Session(info)
{
    WLOGFD("Create extension session, bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s.",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str());
    GeneratePersistentId(true, info);
}

WSError ExtensionSession::TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want)
{
    if (transferAbilityResultFunc_) {
        transferAbilityResultFunc_(resultCode, want);
    }
    return WSError::WS_OK;
}

void ExtensionSession::RegisterTransferAbilityResultListener(const NotifyTransferAbilityResultFunc& func)
{
    transferAbilityResultFunc_ = func;
}

WSError ExtensionSession::TransferExtensionData(const AAFwk::WantParams& wantParams)
{
    if (transferExtensionDataFunc_) {
        transferExtensionDataFunc_(wantParams);
    }
    return WSError::WS_OK;
}

void ExtensionSession::RegisterTransferExtensionDataListener(const NotifyTransferExtensionDataFunc& func)
{
    transferExtensionDataFunc_ = func;
}

WSError ExtensionSession::TransferComponentData(const AAFwk::WantParams& wantParams)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionStage_->NotifyTransferComponentData(wantParams);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
