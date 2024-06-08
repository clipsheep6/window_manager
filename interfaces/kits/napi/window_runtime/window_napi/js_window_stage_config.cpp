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

#include "js_window_stage_config.h"
#include <string>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowStageConfig"};
constexpr char WINDOW_STAGE_CONFIG_NAME[] = "__window_stage_config_ptr__";
// void SetFormInfoPropertyInt32(napi_env env, int32_t inValue, napi_value &result, const char *outName)
// {
//     napi_value temp = CreateJsValue(env, inValue);
//     WLOGI("%{public}s=%{public}d.", outName, inValue);
//     napi_set_named_property(env, result, outName, temp);
// }

void GetInt32ByName(napi_env env, napi_value root, const char* name, int32_t *res)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, root, name, &tempValue);
    napi_get_value_int32(env, tempValue, res);
}

void GetUint32ByName(napi_env env, napi_value root, const char* name, uint32_t *res)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, root, name, &tempValue);
    napi_get_value_uint32(env, tempValue, res);
}
} // namespace

JsWindowStageConfig::JsWindowStageConfig(const std::shared_ptr<WindowStageConfig>& windowStageConfig)
    : windowStageConfig_(windowStageConfig)
{
}

JsWindowStageConfig::~JsWindowStageConfig()
{
}

void JsWindowStageConfig::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsWindowStageConfig>(static_cast<JsWindowStageConfig*>(data));
}

napi_value JsWindowStageConfig::GetWindowStageAttribute(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetWindowStageAttribute");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnGetWindowStageAttribute,
        WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::GetRect(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetRect");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnGetRect, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::GetRectLeft(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetRectLeft");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnGetRectLeft, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::GetRectTop(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetRectTop");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnGetRectTop, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::GetRectWidth(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetRectWidth");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnGetRectWidth, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::GetRectHeight(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetRectHeight");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnGetRectHeight, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::SetWindowStageAttribute(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetWindowStageAttribute");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnSetWindowStageAttribute,
        WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::SetRect(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetRect");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnSetRect, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::SetRectLeft(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetRectLeft");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnSetRectLeft, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::SetRectTop(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetRectTop");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnSetRectTop, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::SetRectWidth(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetRectWidth");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnSetRectWidth, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::SetRectHeight(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetRectHeight");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsWindowStageConfig, OnSetRectHeight, WINDOW_STAGE_CONFIG_NAME);
}

napi_value JsWindowStageConfig::CreateJsWindowRect(napi_env env)
{
    WLOGFI("[NAPI]CreateJsWindowRect");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    auto windowStageConfig = windowStageConfig_.lock();
    std::unique_ptr<JsWindowStageConfig> jsWindowStageConfig =
        std::make_unique<JsWindowStageConfig>(windowStageConfig);
    SetNamedNativePointer(env, objValue, WINDOW_STAGE_CONFIG_NAME, jsWindowStageConfig.release(),
        JsWindowStageConfig::Finalizer);
    //napi_wrap(env, objValue, jsWindowStageConfig.release(), JsWindowStageConfig::Finalizer, nullptr, nullptr);

    BindNativeProperty(env, objValue, "left", JsWindowStageConfig::GetRectLeft, JsWindowStageConfig::SetRectLeft);
    BindNativeProperty(env, objValue, "top", JsWindowStageConfig::GetRectTop, JsWindowStageConfig::SetRectTop);
    BindNativeProperty(env, objValue, "width", JsWindowStageConfig::GetRectWidth, JsWindowStageConfig::SetRectWidth);
    BindNativeProperty(env, objValue, "height", JsWindowStageConfig::GetRectHeight, JsWindowStageConfig::SetRectHeight);

    return objValue;
}

napi_value JsWindowStageConfig::OnGetWindowStageAttribute(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetWindowStageAttribute");
    auto windowStageConfig = windowStageConfig_.lock();
    if (windowStageConfig == nullptr) {
        WLOGE("window stage config is null.");
    }
    return CreateJsValue(env, static_cast<int32_t>(windowStageConfig->windowStageAttribute));
}

napi_value JsWindowStageConfig::OnGetRect(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetRect");
    // auto windowStageConfig = windowStageConfig_.lock();
    // if (windowStageConfig == nullptr) {
    //     WLOGE("window stage config is null.");
    // }
    // napi_value rectValue = CreateJSObject(env);
    // if (rectValue == nullptr) {
    //     WLOGE("invalid rectValue.");
    //     return nullptr;
    // }
    // SetFormInfoPropertyInt32(env, windowStageConfig->rect.posX_, rectValue, "left");
    // SetFormInfoPropertyInt32(env, windowStageConfig->rect.posY_, rectValue, "top");
    // SetFormInfoPropertyInt32(env, windowStageConfig->rect.width_, rectValue, "width");
    // SetFormInfoPropertyInt32(env, windowStageConfig->rect.height_, rectValue, "height");

    // return rectValue;
    return CreateJsWindowRect(env);
}

napi_value JsWindowStageConfig::OnGetRectLeft(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetRectLeft");
    auto windowStageConfig = windowStageConfig_.lock();
    return CreateJsValue(env, windowStageConfig->rect.posX_);
}

napi_value JsWindowStageConfig::OnGetRectTop(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetRectTop");
    auto windowStageConfig = windowStageConfig_.lock();
    return CreateJsValue(env, windowStageConfig->rect.posY_);
}

napi_value JsWindowStageConfig::OnGetRectWidth(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetRectWidth");
    auto windowStageConfig = windowStageConfig_.lock();
    return CreateJsValue(env, windowStageConfig->rect.width_);
}

napi_value JsWindowStageConfig::OnGetRectHeight(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetRectHeight");
    auto windowStageConfig = windowStageConfig_.lock();
    return CreateJsValue(env, windowStageConfig->rect.height_);
}

napi_value JsWindowStageConfig::OnSetWindowStageAttribute(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnSetWindowStageAttribute");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t value = 0;
    napi_get_value_int32(env, info.argv[0], &value);
    auto windowStageConfig = windowStageConfig_.lock();
    windowStageConfig->windowStageAttribute = WindowStageAttribute(value);
    return result;
}

napi_value JsWindowStageConfig::OnSetRect(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnSetRect");
    napi_value result = nullptr;
    auto windowStageConfig = windowStageConfig_.lock();
    result = info.argv[0];
    int32_t res = 0;
    GetInt32ByName(env, result, "left", &res);
    windowStageConfig->rect.posX_ = res;
    GetInt32ByName(env, result, "top", &res);
    windowStageConfig->rect.posY_ = res;
    uint32_t ures = 0;
    GetUint32ByName(env, result, "width", &ures);
    windowStageConfig->rect.width_ = ures;
    GetUint32ByName(env, result, "height", &ures);
    windowStageConfig->rect.height_ = ures;
    return result;
}

napi_value JsWindowStageConfig::OnSetRectLeft(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnSetRectLeft");
    napi_value result = nullptr;
    auto windowStageConfig = windowStageConfig_.lock();
    result = info.argv[0];
    int32_t left = 0;
    ConvertFromJsNumber(env, result, left);
    windowStageConfig->rect.posX_ = left;
    return result;
}

napi_value JsWindowStageConfig::OnSetRectTop(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnSetRectTop");
    napi_value result = nullptr;
    auto windowStageConfig = windowStageConfig_.lock();
    result = info.argv[0];
    int32_t top = 0;
    ConvertFromJsNumber(env, result, top);
    windowStageConfig->rect.posY_ = top;
    return result;
}

napi_value JsWindowStageConfig::OnSetRectWidth(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnSetRectWidth");
    napi_value result = nullptr;
    auto windowStageConfig = windowStageConfig_.lock();
    result = info.argv[0];
    uint32_t width = 0;
    ConvertFromJsNumber(env, result, width);
    windowStageConfig->rect.width_ = width;
    return result;
}

napi_value JsWindowStageConfig::OnSetRectHeight(napi_env env, NapiCallbackInfo& info)
{
    WLOGI("[NAPI]OnSetRectHeight");
    napi_value result = nullptr;
    auto windowStageConfig = windowStageConfig_.lock();
    result = info.argv[0];
    uint32_t height = 0;
    ConvertFromJsNumber(env, result, height);
    windowStageConfig->rect.height_ = height;
    return result;
}

napi_value CreateJsWindowStageConfig(napi_env env, std::shared_ptr<WindowStageConfig> windowStageConfig)
{
    WLOGFI("[NAPI]CreateJsWindowStageConfig");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    std::unique_ptr<JsWindowStageConfig> jsWindowStageConfig = std::make_unique<JsWindowStageConfig>(windowStageConfig);
    SetNamedNativePointer(env, objValue, WINDOW_STAGE_CONFIG_NAME, jsWindowStageConfig.release(),
        JsWindowStageConfig::Finalizer);
    //napi_wrap(env, objValue, jsWindowStageConfig.release(), JsWindowStageConfig::Finalizer, nullptr, nullptr);

    BindNativeProperty(env, objValue, "windowStageAttribute", JsWindowStageConfig::GetWindowStageAttribute,
        JsWindowStageConfig::SetWindowStageAttribute);
    BindNativeProperty(env, objValue, "rect", JsWindowStageConfig::GetRect, JsWindowStageConfig::SetRect);

    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS