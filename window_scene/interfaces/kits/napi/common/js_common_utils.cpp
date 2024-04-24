/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <hitrace_meter.h>

#include "js_common_utils.h"
#include "napi_common_util.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsCommonUtils" };
} // namespace
napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_valuetype GetType(napi_env env, napi_value value)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, value, &res);
    return res;
}

struct AsyncInfo {
    napi_env env;
    napi_async_work work;
    std::function<void()> func;
};

bool ParseArrayStringValue(napi_env env, napi_value array, std::vector<std::string> &vector)
{
    if (array == nullptr) {
        WLOGFE("array is nullptr!");
        return false;
    }
    bool isArray = false;
    if (napi_is_array(env, array, &isArray) != napi_ok || isArray == false) {
        WLOGFE("not array!");
        return false;
    }

    uint32_t arrayLen = 0;
    napi_get_array_length(env, array, &arrayLen);
    if (arrayLen == 0) {
        return true;
    }
    vector.reserve(arrayLen);
    for (uint32_t i = 0; i < arrayLen; i++) {
        std::string strItem;
        napi_value jsValue = nullptr;
        napi_get_element(env, array, i, &jsValue);
        if (!ConvertFromJsValue(env, jsValue, strItem)) {
            WLOGFW("Failed to ConvertFromJsValue, index: %{public}u", i);
            continue;
        }
        vector.emplace_back(std::move(strItem));
    }
    return true;
}

void NapiAsyncWork(napi_env env, std::function<void()> task)
{
    napi_value resource = nullptr;
    AsyncInfo* info = new AsyncInfo();
    info->env = env;
    info->func = task;
    napi_create_string_utf8(env, "AsyncWork", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {
        WLOGFI("AsyncWork in");
    },
    [](napi_env env, napi_status status, void* data) {
        AsyncInfo* info = (AsyncInfo*)data;
        info->func();
        napi_delete_async_work(env, info->work);
        delete info;
    }, (void*)info, &info->work);
    if (!NAPICall(env, napi_queue_async_work(env,info->work))) {
        delete info;
        return nullptr;
    }
}

MainThreadScheduler::MainThreadScheduler(napi_env env) : env_(env)
{
    GetMainEventHandler();
}

inline void MainThreadScheduler::GetMainEventHandler()
{
    if (handler_ != nullptr) {
        return;
    }
    auto runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
    if (runner == nullptr) {
        return;
    }
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
}

void MainThreadScheduler::PostMainThreadTask(Task && localTask, std::string traceInfo, int64_t delayTime)
{
    GetMainEventHandler();
    auto task = [env = env_, localTask, traceInfo] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SCBCb:%s", traceInfo.c_str());
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        localTask();
        napi_close_handle_scope(env, scope);
    };
    if (handler_ && handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        return task();
    } else if (handler_ && !handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        handler_->PostTask(std::move(task), "wms:" + traceInfo, delayTime,
            OHOS::AppExecFwk::EventQueue::Priority::IMMEDIATE);
    } else {
        NapiAsyncWork(env_, task);
    }
}
} // namespace OHOS::Rosen