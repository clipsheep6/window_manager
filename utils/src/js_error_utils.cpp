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

#include "js_error_utils.h"

#include <map>

namespace OHOS::Rosen {

constexpr const char* WM_ERROR_MSG_OK = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_NO_PERMISSION = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_NOT_SYSTEM_APP = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_INVALID_PARAM = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_DEVICE_NOT_SUPPORT = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_REPEAT_OPERATION = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_STATE_ABNORMALLY = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_SYSTEM_ABNORMALLY = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_INVALID_CALLING = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_STAGE_ABNORMALLY = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_CONTEXT_ABNORMALLY = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_START_ABILITY_FAILED = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_INVALID_DISPLAY = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_INVALID_PARENT = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_OPER_FULLSCREEN_FAILED = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_PIP_DESTROY_FAILED = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_PIP_STATE_ABNORMALLY = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_PIP_CREATE_FAILED = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_PIP_INTERNAL_ERROR = "WM_ERROR_MSG_OK";
constexpr const char* WM_ERROR_MSG_PIP_REPEAT_OPERATION = "WM_ERROR_MSG_OK";

static std::map<WmErrorCode, const char*> WM_ERROR_CODE_TO_ERROR_MSG_MAP {
    {WmErrorCode::WM_OK,                              WM_ERROR_MSG_OK                     },
    {WmErrorCode::WM_ERROR_NO_PERMISSION,             WM_ERROR_MSG_NO_PERMISSION          },
    {WmErrorCode::WM_ERROR_NOT_SYSTEM_APP,            WM_ERROR_MSG_NOT_SYSTEM_APP         },
    {WmErrorCode::WM_ERROR_INVALID_PARAM,             WM_ERROR_MSG_INVALID_PARAM          },
    {WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT,        WM_ERROR_MSG_DEVICE_NOT_SUPPORT     },
    {WmErrorCode::WM_ERROR_REPEAT_OPERATION,          WM_ERROR_MSG_REPEAT_OPERATION       },
    {WmErrorCode::WM_ERROR_STATE_ABNORMALLY,          WM_ERROR_MSG_STATE_ABNORMALLY       },
    {WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY,         WM_ERROR_MSG_SYSTEM_ABNORMALLY      },
    {WmErrorCode::WM_ERROR_INVALID_CALLING,           WM_ERROR_MSG_INVALID_CALLING        },
    {WmErrorCode::WM_ERROR_STAGE_ABNORMALLY,          WM_ERROR_MSG_STAGE_ABNORMALLY       },
    {WmErrorCode::WM_ERROR_CONTEXT_ABNORMALLY,        WM_ERROR_MSG_CONTEXT_ABNORMALLY     },
    {WmErrorCode::WM_ERROR_START_ABILITY_FAILED,      WM_ERROR_MSG_START_ABILITY_FAILED   },
    {WmErrorCode::WM_ERROR_INVALID_DISPLAY,           WM_ERROR_MSG_INVALID_DISPLAY        },
    {WmErrorCode::WM_ERROR_INVALID_PARENT,            WM_ERROR_MSG_INVALID_PARENT         },
    {WmErrorCode::WM_ERROR_OPER_FULLSCREEN_FAILED,    WM_ERROR_MSG_OPER_FULLSCREEN_FAILED },
    {WmErrorCode::WM_ERROR_PIP_DESTROY_FAILED,        WM_ERROR_MSG_PIP_DESTROY_FAILED     },
    {WmErrorCode::WM_ERROR_PIP_STATE_ABNORMALLY,      WM_ERROR_MSG_PIP_STATE_ABNORMALLY   },
    {WmErrorCode::WM_ERROR_PIP_CREATE_FAILED,         WM_ERROR_MSG_PIP_CREATE_FAILED      },
    {WmErrorCode::WM_ERROR_PIP_INTERNAL_ERROR,        WM_ERROR_MSG_PIP_INTERNAL_ERROR     },
    {WmErrorCode::WM_ERROR_PIP_REPEAT_OPERATION,      WM_ERROR_MSG_PIP_REPEAT_OPERATION   },
};

constexpr const char* DM_ERROR_MSG_OK = "DM_ERROR_MSG_OK";
constexpr const char* DM_ERROR_MSG_NO_PERMISSION = "DM_ERROR_MSG_NO_PERMISSION";
constexpr const char* DM_ERROR_MSG_NOT_SYSTEM_APP = "DM_ERROR_MSG_NOT_SYSTEM_APP";
constexpr const char* DM_ERROR_MSG_INVALID_PARAM = "DM_ERROR_MSG_INVALID_PARAM";
constexpr const char* DM_ERROR_MSG_DEVICE_NOT_SUPPORT = "DM_ERROR_MSG_DEVICE_NOT_SUPPORT";
constexpr const char* DM_ERROR_MSG_INVALID_SCREEN = "DM_ERROR_MSG_INVALID_SCREEN";
constexpr const char* DM_ERROR_MSG_INVALID_CALLING = "DM_ERROR_MSG_INVALID_CALLING";
constexpr const char* DM_ERROR_MSG_SYSTEM_INNORMAL = "DM_ERROR_MSG_SYSTEM_INNORMAL";

static std::map<DmErrorCode, const char*> DM_ERROR_CODE_TO_ERROR_MSG_MAP {
    {DmErrorCode::DM_OK,                              DM_ERROR_MSG_OK                 },
    {DmErrorCode::DM_ERROR_NO_PERMISSION,             DM_ERROR_MSG_NO_PERMISSION      },
    {DmErrorCode::DM_ERROR_NOT_SYSTEM_APP,            DM_ERROR_MSG_NOT_SYSTEM_APP     },
    {DmErrorCode::DM_ERROR_INVALID_PARAM,             DM_ERROR_MSG_INVALID_PARAM      },
    {DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT,        DM_ERROR_MSG_DEVICE_NOT_SUPPORT },
    {DmErrorCode::DM_ERROR_INVALID_SCREEN,            DM_ERROR_MSG_INVALID_SCREEN     },
    {DmErrorCode::DM_ERROR_INVALID_CALLING,           DM_ERROR_MSG_INVALID_CALLING    },
    {DmErrorCode::DM_ERROR_SYSTEM_INNORMAL,           DM_ERROR_MSG_SYSTEM_INNORMAL    },
};

std::string JsErrorUtils::GetErrorMsg(const WmErrorCode& errorCode)
{
    return WM_ERROR_CODE_TO_ERROR_MSG_MAP.find(errorCode) == WM_ERROR_CODE_TO_ERROR_MSG_MAP.end() ?
        WM_ERROR_CODE_TO_ERROR_MSG_MAP.at(errorCode) : "";
}

std::string JsErrorUtils::GetErrorMsg(const DmErrorCode& errorCode)
{
    return DM_ERROR_CODE_TO_ERROR_MSG_MAP.find(errorCode) == DM_ERROR_CODE_TO_ERROR_MSG_MAP.end() ?
        DM_ERROR_CODE_TO_ERROR_MSG_MAP.at(errorCode) : "";
}

template<class T>
napi_value JsErrorUtils::CreateJsValue(napi_env env, const T& value)
{
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    napi_value result = nullptr;
    if constexpr (std::is_same_v<ValueType, bool>) {
        napi_get_boolean(env, value, &result);
        return result;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return CreateJsNumber(env, value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        napi_create_string_utf8(env, value.c_str(), value.length(), &result);
        return result;
    } else if constexpr (std::is_enum_v<ValueType>) {
        return CreateJsNumber(env, static_cast<std::make_signed_t<ValueType>>(value));
    } else if constexpr (std::is_same_v<ValueType, const char*>) {
        (value != nullptr) ? napi_create_string_utf8(env, value, strlen(value), &result) :
            napi_get_undefined(env, &result);
        return result;
    }
}

napi_value JsErrorUtils::CreateJsError(napi_env env, const WMError& errorCode, std::string msg)
{
    return CreateJsError(env, WM_JS_TO_ERROR_CODE_MAP.at(errorCode), msg);
}

napi_value JsErrorUtils::CreateJsError(napi_env env, const WmErrorCode& errorCode, std::string msg)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, std::to_string(static_cast<int32_t>(errorCode))),
        CreateJsValue(env, msg == "" ? GetErrorMsg(errorCode) : msg), &result);
    return result;
}

napi_value JsErrorUtils::CreateJsError(napi_env env, const DMError& errorCode, std::string msg)
{
    return CreateJsError(env, DM_JS_TO_ERROR_CODE_MAP.at(errorCode), msg);
}

napi_value JsErrorUtils::CreateJsError(napi_env env, const DmErrorCode& errorCode, std::string msg)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, std::to_string(static_cast<int32_t>(errorCode))),
        CreateJsValue(env, msg == "" ? GetErrorMsg(errorCode) : msg), &result);
    return result;
}
} // namespace OHOS::Rosen
