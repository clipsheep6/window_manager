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

#ifndef OHOS_JS_WINDOW_STAGE_CONFIG_H
#define OHOS_JS_WINDOW_STAGE_CONFIG_H
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "js_runtime_utils.h"
#include "wm_common.h"
namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif
WINDOW_EXPORT napi_value CreateJsWindowStageConfig(napi_env env, std::shared_ptr<WindowStageConfig> windowStageConfig);
class JsWindowStageConfig {
public:
    explicit JsWindowStageConfig(const std::shared_ptr<WindowStageConfig>& windowStageConfig);
    ~JsWindowStageConfig();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value GetWindowStageAttribute(napi_env env, napi_callback_info info);
    static napi_value GetRect(napi_env env, napi_callback_info info);
    static napi_value GetRectLeft(napi_env env, napi_callback_info info);
    static napi_value GetRectTop(napi_env env, napi_callback_info info);
    static napi_value GetRectWidth(napi_env env, napi_callback_info info);
    static napi_value GetRectHeight(napi_env env, napi_callback_info info);
    static napi_value SetWindowStageAttribute(napi_env env, napi_callback_info info);
    static napi_value SetRect(napi_env env, napi_callback_info info);
    static napi_value SetRectLeft(napi_env env, napi_callback_info info);
    static napi_value SetRectTop(napi_env env, napi_callback_info info);
    static napi_value SetRectWidth(napi_env env, napi_callback_info info);
    static napi_value SetRectHeight(napi_env env, napi_callback_info info);

private:
    napi_value CreateJsWindowRect(napi_env env);
    napi_value OnGetWindowStageAttribute(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetRect(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetRectLeft(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetRectTop(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetRectWidth(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetRectHeight(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowStageAttribute(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetRect(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetRectLeft(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetRectTop(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetRectWidth(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetRectHeight(napi_env env, AbilityRuntime::NapiCallbackInfo& info);

    std::weak_ptr<WindowStageConfig> windowStageConfig_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_WINDOW_STAGE_CONFIG_H
