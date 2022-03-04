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

#include "wm_napi_common.h"
#include <hilog/log.h>
#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "ipc_skeleton.h"

namespace OHOS {
napi_status SetMemberInt32(napi_env env, napi_value result, const char *key, int32_t value)
{
    napi_value num;
    GNAPI_INNER(napi_create_int32(env, value, &num));
    GNAPI_INNER(napi_set_named_property(env, result, key, num));
    return napi_ok;
}

napi_status SetMemberUint32(napi_env env, napi_value result, const char *key, uint32_t value)
{
    napi_value num;
    GNAPI_INNER(napi_create_uint32(env, value, &num));
    GNAPI_INNER(napi_set_named_property(env, result, key, num));
    return napi_ok;
}

napi_status SetMemberUndefined(napi_env env, napi_value result, const char *key)
{
    napi_value undefined;
    GNAPI_INNER(napi_get_undefined(env, &undefined));
    GNAPI_INNER(napi_set_named_property(env, result, key, undefined));
    return napi_ok;
}

bool CheckCallingPermission(std::string permission)
{
    WLOGFI("Screenshot::CheckCallingPermission, permission:%{public}s", permission.c_str());
    WLOGFI("Screenshot::IPCSkeleton::GetCallingTokenID(): %{public}d", IPCSkeleton::GetCallingTokenID());
    WLOGFI("Screenshot::AccessTokenKit::VerifyAccessToken(): %{public}d",
        Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission));
    WLOGFI("Screenshot::Constants::PERMISSION_GRANTED(): %{public}d", AppExecFwk::Constants::PERMISSION_GRANTED);
    
    if (!permission.empty() &&
        Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission)
        != AppExecFwk::Constants::PERMISSION_GRANTED) {
        WLOGFE("PERMISSION_GRANTED: %{public}d", AppExecFwk::Constants::PERMISSION_GRANTED);
        WLOGFE("%{public}s permission not granted.", permission.c_str());
        return false;
    }
    WLOGFI("Screenshot::CheckCallingPermission end.");
    return true;
}

void ProcessPromise(napi_env env, Rosen::WMError wret, napi_deferred deferred, napi_value result[])
{
    GNAPI_LOG("Process Promise");
    if (wret != Rosen::WMError::WM_OK) {
        GNAPI_LOG("Process Promise reject");
        napi_reject_deferred(env, deferred, result[0]);
        return;
    }
    GNAPI_LOG("Process Promise resolve");
    napi_resolve_deferred(env, deferred, result[1]);
}

void ProcessCallback(napi_env env, napi_ref ref, napi_value result[])
{
    GNAPI_LOG("Process Callback");
    napi_value callback = nullptr;
    napi_value returnVal = nullptr;
    napi_get_reference_value(env, ref, &callback);
    napi_call_function(env, nullptr, callback, 2, result, &returnVal); // 2: callback func input number
    napi_delete_reference(env, ref);
}
} // namespace OHOS
