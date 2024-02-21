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

#include "extension_window_impl.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionWindowImpl"};
}

Rosen::ExtensionWindowImpl::ExtensionWindowImpl(sptr<Window> sessionImpl)
    : windowExtensionSessionImpl_(sessionImpl) {
}

ExtensionWindowImpl::~ExtensionWindowImpl() {}

WMError ExtensionWindowImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    WLOGI("GetAvoidAreaByType is called");
    return windowExtensionSessionImpl_->GetAvoidAreaByType(type, avoidArea);
}

WMError ExtensionWindowImpl::NapiSetUIContent(const std::string& contentInfo, napi_env env,
    napi_value storage, bool isdistributed, sptr<IRemoteObject> token, AppExecFwk::Ability* ability)
{
    WLOGI("NapiSetUIContent is called");
    return windowExtensionSessionImpl_->NapiSetUIContent(contentInfo, env, storage, isdistributed, token, ability);
}

sptr<Window> ExtensionWindowImpl::GetWindow()
{
    return windowExtensionSessionImpl_;
}

WMError ExtensionWindowImpl::HideNonSecureWindows(bool shouldHide)
{
    WLOGI("HideNonSecureWindows is called");
    return windowExtensionSessionImpl_->HideNonSecureWindows(shouldHide);
}
} // namespace Rosen
} // namespace OHOS
