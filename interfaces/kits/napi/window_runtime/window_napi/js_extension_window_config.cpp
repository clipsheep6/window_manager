/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include <string>
#include "js_extension_window_config.h"
#include "js_window_utils.h"
#include "window_manager_hilog.h"

namespace OHOS{
namespace Rosen{
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsExtensionWindowConfig"};
    constexpr char EXTENSION_WINDOW_CONFIG_NAME[] = "__extension_window_config_ptr__";
    napi_value NapiGetUnderfined(napi_env env)
    {
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        return result;
    }

    void BindNativePropertys(napi_env env, napi_value object, const char* name, napi_callback getter,
        napi_callback setter)
    {
        napi_property_descriptor properties[1];
        properties[0].utf8name = name;
        properties[0].name = nullptr;
        properties[0].method = nullptr;
        properties[0].getter = getter;
        properties[0].setter = setter;
        properties[0].value = nullptr;
        properties[0].attributes = napi_default;
        properties[0].data = nullptr;
        napi_define_properties(env, object, 1, properties);

    }
} // namespace
JsExtensionWindowConfig::JsExtensionWindowConfig(const std::shared_ptr<ExtensionWindowConfig> &extensionWindowConfig)
    :extensionWindowConfig_(extensionWindowConfig)
{
}

JsExtensionWindowConfig::~JsExtensionWindowConfig(){}

void JsExtensionWindowConfig::Finalizer(napi_env env, void* data, void* hint)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]Finalizer");
    std::unique_ptr<JsExtensionWindowConfig>(static_cast<JsExtensionWindowConfig *>(data));
}

napi_value CreateJsExtensionWindowConfig(napi_env env, std::shared_ptr<ExtensionWindowConfig> extensionWindowConfig)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]CreateJsExtensionWindowConfig");
    napi_env objValue = nullptr;
    napi_createe_object(env, &objValue);

    std::unique_ptr<JsExtensionWindowConfig> jsExtensionWindowConfig =
        std::make_unique<JsExtensionWindowConfig>(extensionWindowConfig);
    SetNameNativePointer(env, objValue, EXTENSION_WINDOW_CONFIG_NAME, jsExtensionWindowConfig.release(),
        JsExtensionWindowConfig::Finalizer);
    BindNativePropertys(env, objValue, "windowName", JsExtensionWindowConfig::GetWindowName,
        JsExtensionWindowConfig::SetWindowName);
    BindNativePropertys(env, objValue, "windowAttribute", JsExtensionWindowConfig::GetWindowAttribute,
        JsExtensionWindowConfig::SetWindowAttribute);
    BindNativePropertys(env, objValue, "windowRect", JsExtensionWindowConfig::GetWindowRect,
        JsExtensionWindowConfig::SetWindowRect);
    BindNativePropertys(env, objValue, "subWindowOptions", JsExtensionWindowConfig::GetSubWindowOptions,
        JsExtensionWindowConfig::SetSubWindowOptions);
    BindNativePropertys(env, objValue, "systemWindowOptions", JsExtensionWindowConfig::GetSystemWindowOptions,
        JsExtensionWindowConfig::SetSystemWindowOptions);
    return objValue;
}

napi_value JsExtensionWindowConfig::CreateJsWindowRect(napi_env env)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]CreateJsWindowRect");
    napi_env objValue = nullptr;
    napi_createe_object(env, &objValue);

    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    std::unique_ptr<JsExtensionWindowConfig> jsExtensionWindowConfig =
        std::make_unique<JsExtensionWindowConfig>(extensionWindowConfig);
    SetNameNativePointer(env, objValue, EXTENSION_WINDOW_CONFIG_NAME, jsExtensionWindowConfig.release(),
        JsExtensionWindowConfig::Finalizer);
    BindNativePropertys(env, objValue, "left", JsExtensionWindowConfig::GetWindowRectLeft,
        JsExtensionWindowConfig::SetWindowRectLeft);
    BindNativePropertys(env, objValue, "top", JsExtensionWindowConfig::GetWindowRectTop,
        JsExtensionWindowConfig::SetWindowRectTop);
    BindNativePropertys(env, objValue, "width", JsExtensionWindowConfig::GetWindowRectWidth,
        JsExtensionWindowConfig::SetWindowRectWidth);
    BindNativePropertys(env, objValue, "height", JsExtensionWindowConfig::GetWindowRectHeight,
        JsExtensionWindowConfig::SetWindowRectHeight);
    return objValue;
}

napi_value JsExtensionWindowConfig::CreateJsSubWindowOptions(napi_env env)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]CreateJsSubWindowOptions");
    napi_env objValue = nullptr;
    napi_createe_object(env, &objValue);

    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    std::unique_ptr<JsExtensionWindowConfig> jsExtensionWindowConfig =
        std::make_unique<JsExtensionWindowConfig>(extensionWindowConfig);
    SetNameNativePointer(env, objValue, EXTENSION_WINDOW_CONFIG_NAME, jsExtensionWindowConfig.release(),
        JsExtensionWindowConfig::Finalizer);
    BindNativePropertys(env, objValue, "title", JsExtensionWindowConfig::GetSubWindowOptionsTitle,
        JsExtensionWindowConfig::SetSubWindowOptionsTitle);
    BindNativePropertys(env, objValue, "decorEnabled", JsExtensionWindowConfig::GetSubWindowOptionsDecorEnabled,
        JsExtensionWindowConfig::SetSubWindowOptionsDecorEnabled);
    BindNativePropertys(env, objValue, "isModal", JsExtensionWindowConfig::GetSubWindowOptionsIsModal,
        JsExtensionWindowConfig::SetSubWindowOptionsIsModal);
    BindNativePropertys(env, objValue, "isTopmost", JsExtensionWindowConfig::GetSubWindowOptionsIsTopmost,
        JsExtensionWindowConfig::SetSubWindowOptionsIsTopmost);
    return objValue;
}

napi_value JsExtensionWindowConfig::CreateJsSystemWindowOptions(napi_env env)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]CreateJsSystemWindowOptions");
    napi_env objValue = nullptr;
    napi_createe_object(env, &objValue);

    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    std::unique_ptr<JsExtensionWindowConfig> jsExtensionWindowConfig =
        std::make_unique<JsExtensionWindowConfig>(extensionWindowConfig);
    SetNameNativePointer(env, objValue, EXTENSION_WINDOW_CONFIG_NAME, jsExtensionWindowConfig.release(),
        JsExtensionWindowConfig::Finalizer);
    BindNativePropertys(env, objValue, "windowType", JsExtensionWindowConfig::GetSystemWindowOptionsWindowType,
        JsExtensionWindowConfig::SetSystemWindowOptionsWindowType);
    return objValue;
}

napi_value JsExtensionWindowConfig::GetWindowName(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowName");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowName,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowName(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowName");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->windowName);
}

napi_value JsExtensionWindowConfig::GetWindowAttribute(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowAttribute");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowAttribute,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowAttribute(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowAttribute");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, static_cast<int32_t>(extensionWindowConfig->windowAttribute));
}

napi_value JsExtensionWindowConfig::GetWindowRect(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowRect");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowRect,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowRect(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowRect");
    return CreateJsWindowRect(env);
}

napi_value JsExtensionWindowConfig::GetWindowRectLeft(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowRectLeft");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowRectLeft,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowRectLeft(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowRectLeft");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->windowRect.posX_);
}

napi_value JsExtensionWindowConfig::GetWindowRectTop(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowRectTop");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowRectTop,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowRectTop(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowRectTop");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->windowRect.posY_);
}

napi_value JsExtensionWindowConfig::GetWindowRectWidth(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowRectWidth");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowRectWidth,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowRectWidth(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowRectWidth");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->windowRect.width_);
}

napi_value JsExtensionWindowConfig::GetWindowRectHeight(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetWindowRectHeight");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetWindowRectHeight,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetWindowRectHeight(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetWindowRectHeight");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->windowRect.height_);
}

napi_value JsExtensionWindowConfig::GetSubWindowOptions(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSubWindowOptions");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSubWindowOptions,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSubWindowOptions(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSubWindowOptions");
    return CreateJsSubWindowOptions(env);
}

napi_value JsExtensionWindowConfig::GetSubWindowOptionsTitle(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSubWindowOptionsTitle");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSubWindowOptionsTitle,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSubWindowOptionsTitle(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSubWindowOptionsTitle");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->subWindowOptions.title);
}

napi_value JsExtensionWindowConfig::GetSubWindowOptionsDecorEnabled(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSubWindowOptionsDecorEnabled");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSubWindowOptionsDecorEnabled,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSubWindowOptionsDecorEnabled(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSubWindowOptionsDecorEnabled");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->subWindowOptions.decorEnabled);
}

napi_value JsExtensionWindowConfig::GetSubWindowOptionsIsModal(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSubWindowOptionsIsModal");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSubWindowOptionsIsModal,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSubWindowOptionsIsModal(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSubWindowOptionsIsModal");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->subWindowOptions.isModal);
}

napi_value JsExtensionWindowConfig::GetSubWindowOptionsIsTopmost(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSubWindowOptionsIsTopmost");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSubWindowOptionsIsTopmost,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSubWindowOptionsIsTopmost(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSubWindowOptionsIsTopmost");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->subWindowOptions.isTopmost);
}

napi_value JsExtensionWindowConfig::GetSystemWindowOptions(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSystemWindowOptions");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSystemWindowOptions,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSystemWindowOptions(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSystemWindowOptions");
    return CreateJsSystemWindowOptions(env);
}

napi_value JsExtensionWindowConfig::GetSystemWindowOptionsWindowType(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]GetSystemWindowOptionsWindowType");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnGetSystemWindowOptionsWindowType,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnGetSystemWindowOptionsWindowType(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnGetSystemWindowOptionsWindowType");
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    return CreateJsValue(env, extensionWindowConfig->systemWindowOptions.windowType);
}

napi_value JsExtensionWindowConfig::SetWindowName(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowName");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowName,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowName(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowName");
    napi_value result = nullptr;
    result = info.argv[0];
    std::string windowName;
    if (!ConvertFromJsValue(env, result, windowName))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowName");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowName = windowName;
    return result;
}

napi_value JsExtensionWindowConfig::SetWindowAttribute(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowAttribute");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowAttribute,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowAttribute(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowAttribute");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t value = 0;
    if (!ConvertFromJsValue(env, result, value))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowAttribute");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowAttribute = ExtensionWindowAttribute(value);
    return result;
}

napi_value JsExtensionWindowConfig::SetWindowRect(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowRect");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowRect,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowRect(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowRect");
    napi_value result = nullptr;
    result = info.argv[0];
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    int32_t res = 0;
    if (!ParseJsValue(result, env, "left", res))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect left");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.posX_ = res;
    if (!ParseJsValue(result, env, "top", res))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect top");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.posY_ = res;
    uint32_t ures = 0;
    if (!ParseJsValue(result, env, "width", res))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect width");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.width_ = ures;
    if (!ParseJsValue(result, env, "height", res))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect height");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.height_ = ures;
    return result;
}

napi_value JsExtensionWindowConfig::SetWindowRectLeft(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowRectLeft");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowRectLeft,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowRectLeft(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowRectLeft");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t left = 0;
    if (!ConvertFromJsValue(env, result, left))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect left");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.posX_ = left;
    return result;
}

napi_value JsExtensionWindowConfig::SetWindowRectTop(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowRectTop");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowRectTop,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowRectTop(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowRectTop");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t top = 0;
    if (!ConvertFromJsValue(env, result, top))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect top");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.posY_ = top;
    return result;
}

napi_value JsExtensionWindowConfig::SetWindowRectWidth(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowRectWidth");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowRectWidth,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowRectWidth(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowRectWidth");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t width = 0;
    if (!ConvertFromJsValue(env, result, width))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect width");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.width_ = width;
    return result;
}

napi_value JsExtensionWindowConfig::SetWindowRectHeight(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetWindowRectHeight");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetWindowRectHeight,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetWindowRectHeight(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetWindowRectHeight");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t height = 0;
    if (!ConvertFromJsValue(env, result, height))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to windowRect height");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->windowRect.height_ = height;
    return result;
}

napi_value JsExtensionWindowConfig::SetSubWindowOptions(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSubWindowOptions");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSubWindowOptions,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSubWindowOptions(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSubWindowOptions");
    napi_value result = nullptr;
    result = info.argv[0];
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    std::string title;
    if (!ParseJsValue(result, env, "title", res))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions title");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.title = title;

    bool decorEnabled = false;
    if (!ParseJsValue(result, env, "decorEnabled", decorEnabled))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions decorEnabled");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.decorEnabled = decorEnabled;

    bool isModal = false;
    if (!ParseJsValue(result, env, "isModal", isModal))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions isModal");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.isModal = isModal;

    bool isTopmost = false;
    if (!ParseJsValue(result, env, "isTopmost", isTopmost))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions isTopmost");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.isTopmost = isTopmost;
    return result;
}

napi_value JsExtensionWindowConfig::SetSubWindowOptionsTitle(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSubWindowOptionsTitle");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSubWindowOptionsTitle,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSubWindowOptionsTitle(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSubWindowOptionsTitle");
    napi_value result = nullptr;
    result = info.argv[0];
    std::string title;
    if (!ConvertFromJsValue(env, result, title))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions title");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.title = title;
    return result;
}


napi_value JsExtensionWindowConfig::SetSubWindowOptionsDecorEnabled(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSubWindowOptionsDecorEnabled");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSubWindowOptionsDecorEnabled,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSubWindowOptionsDecorEnabled(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSubWindowOptionsDecorEnabled");
    napi_value result = nullptr;
    result = info.argv[0];
    bool decorEnabled = false;
    if (!ConvertFromJsValue(env, result, decorEnabled))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions decorEnabled");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.decorEnabled = decorEnabled;
    return result;
}

napi_value JsExtensionWindowConfig::SetSubWindowOptionsIsModal(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSubWindowOptionsIsModal");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSubWindowOptionsIsModal,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSubWindowOptionsIsModal(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSubWindowOptionsIsModal");
    napi_value result = nullptr;
    result = info.argv[0];
    bool isModal = false;
    if (!ConvertFromJsValue(env, result, isModal))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions isModal");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.isModal = isModal;
    return result;
}

napi_value JsExtensionWindowConfig::SetSubWindowOptionsIsTopmost(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSubWindowOptionsIsTopmost");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSubWindowOptionsIsTopmost,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSubWindowOptionsIsTopmost(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSubWindowOptionsIsTopmost");
    napi_value result = nullptr;
    result = info.argv[0];
    bool isTopmost = false;
    if (!ConvertFromJsValue(env, result, isTopmost))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to subWindowOptions isTopmost");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->subWindowOptions.isTopmost = isTopmost;
    return result;
}

napi_value JsExtensionWindowConfig::SetSystemWindowOptions(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSystemWindowOptions");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSystemWindowOptions,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSystemWindowOptions(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSystemWindowOptions");
    napi_value result = nullptr;
    result = info.argv[0];
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    int32_t windowType = 0;
    if (!ParseJsValue(result, env, "windowType", res))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to systemWindowOptions windowType");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->systemWindowOptions.windowType = windowType;
    return result;
}

napi_value JsExtensionWindowConfig::SetSystemWindowOptionsWindowType(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]SetSystemWindowOptionsWindowType");
    GET_NAPI_INFO_WITH_NAME_AND_CALL(env, info, JsExtensionWindowConfig, OnSetSystemWindowOptionsWindowType,
        EXTENSION_WINDOW_CONFIG_NAME);
}

napi_value JsExtensionWindowConfig::OnSetSystemWindowOptionsWindowType(napi_env env, NapiCallbackInfo& info)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[NAPI]OnSetSystemWindowOptionsWindowType");
    napi_value result = nullptr;
    result = info.argv[0];
    int32_t windowType = 0;
    if (!ConvertFromJsValue(env, result, windowType))
    {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to convert parameter to systemWindowOptions windowType");
        return NapiGetUnderfined(env);
    }
    auto extensionWindowConfig = extensionWindowConfig_.lock();
    if (extensionWindowConfig == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extension window config is null.");
        return NapiGetUnderfined(env);
    }
    extensionWindowConfig->systemWindowOptions.windowType = windowType;
    return result;
}
}
}