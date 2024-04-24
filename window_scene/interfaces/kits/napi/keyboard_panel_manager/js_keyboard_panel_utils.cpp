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

#include "js_keyboard_panel_utils.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsKeyboardPanelUtils" };
} // namespace

bool ConvertKeyboardPrivateCommandFromJs(napi_env env, napi_value in,
    std::unordered_map<std::string, KeyboardPrivateDataValue>& out)
{
    if (in == nullptr) {
        WLOGFE("[NAPI]value is nullptr");
        return false;
    }

    if (!CheckTypeForNapiValue(env, in, napi_object)) {
        WLOGFE("The type of value is not napi_object.");
        return false;
    }

    std::vector<std::string> propNames;
    napi_value array = nullptr;
    napi_get_property_names(env, in, &array);
    if (!ParseArrayStringValue(env, array, propNames)) {
        WLOGFE("Failed to property names");
        return false;
    }

    for (const auto &propName : propNames) {
        napi_value prop = nullptr;
        napi_get_named_property(env, in, propName.c_str(), &prop);
        if (prop == nullptr) {
            WLOGFE("prop is null: %{public}s", propName.c_str());
            continue;
        }
        KeyboardPrivateDataValue privateCommand;
        if (!GetPrivateDataValue(env, prop, privateCommand)) {
            WLOGFE("Get privateCommand failed: %{public}s", propName.c_str());
            continue;
        }
        out.emplace(propName, privateCommand);
    }
    return true;
}

bool GetPrivateDataValue(napi_env env, napi_value in, KeyboardPrivateDataValue& out)
{
    napi_valuetype valueType = napi_undefined;
    napi_status status = napi_typeof(env, in, &valueType);
    if (status != napi_ok) {
        WLOGFE("param is undefined");
        return false;
    }
    if (valueType == napi_string) {
        std::string privateDataStr;
        if (!ConvertFromJsValue(env, in, privateDataStr)) {
            WLOGFE("Failed to convert parameter to privateDataStr");
            return false;
        }
        out.emplace<std::string>(privateDataStr);
    } else if (valueType == napi_boolean) {
        bool privateDataBool = false;
        if (!ConvertFromJsValue(env, in, privateDataBool)) {
            WLOGFE("Failed to convert parameter to privateDataBool");
            return false;
        }
        out.emplace<bool>(privateDataBool);
    } else if (valueType == napi_number) {
        int32_t privateDataInt = 0;
        if (!ConvertFromJsValue(env, in, privateDataInt)) {
            WLOGFE("Failed to convert parameter to privateDataInt");
            return false;
        }
        out.emplace<int32_t>(privateDataInt);
    } else {
        WLOGFE("value type must be string | boolean | number");
        return false;
    }
    return status == napi_ok;
}

napi_value GetJsKeyboardPrivateCommand(napi_env env,
    const std::unordered_map<std::string, KeyboardPrivateDataValue>& in)
{
    napi_value jsPrivateCommand = nullptr;
    napi_create_object(env, &jsPrivateCommand);
    for (const auto &iter : in) {
        size_t idx = iter.second.index();
        napi_value value = nullptr;
        if (idx == static_cast<size_t>(KeyboardPrivateDataValueType::VALUE_TYPE_STRING)) {
            auto stringValue = std::get_if<std::string>(&iter.second);
            if (stringValue != nullptr) {
                napi_create_string_utf8(env, (*stringValue).c_str(), (*stringValue).size(), &value);
            }
        } else if (idx == static_cast<size_t>(KeyboardPrivateDataValueType::VALUE_TYPE_BOOL)) {
            auto boolValue = std::get_if<bool>(&iter.second);
            if (boolValue != nullptr) {
                NAPI_CALL(env, napi_get_boolean(env, *boolValue, &value));
            }
        } else if (idx == static_cast<size_t>(KeyboardPrivateDataValueType::VALUE_TYPE_NUMBER)) {
            auto numberValue = std::get_if<int32_t>(&iter.second);
            if (numberValue != nullptr) {
                napi_create_int32(env, *numberValue, &value);
            }
        }
        napi_set_named_property(env, jsPrivateCommand, iter.first.c_str(), value);
    }
    return jsPrivateCommand;
}
} // namespace OHOS::Rosen