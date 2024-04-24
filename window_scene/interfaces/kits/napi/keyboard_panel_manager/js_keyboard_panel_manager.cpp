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

#include "js_keyboard_panel_manager.h"

#include "interfaces/include/ws_common.h"
#include "session_manager/include/keyboard_panel_manager.h"
#include "window_manager_hilog.h"


namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsKeyboardPanelManager" };
const std::string KEYBOARD_RECEIVE_PRIVATE_COMMAND_CB = "receiveKeyboardPrivateCommand";
const std::string KEYBOARD_RECEIVE_IS_PANEL_SHOW_CB = "receiveKeyboardIsPanelShow";
}

napi_value JsKeyboardPanelManager::Init(napi_env env, napi_value exportObj)
{
    WLOGI("[NAPI]JsKeyboardPanelManager Init");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("env or exportObj is null!");
        return nullptr;
    }

    std::unique_ptr<JsKeyboardPanelManager> jsKeyboardPanelManager = std::make_unique<JsKeyboardPanelManager>(env);
    napi_wrap(env, exportObj, jsKeyboardPanelManager.release(), JsKeyboardPanelManager::Finalizer, nullptr, nullptr);

    const char* moduleName = "JsKeyboardPanelManager";
    BindNativeFunction(env, exportObj, "sendKeyboardPrivateCommand", moduleName,
        JsKeyboardPanelManager::SendKeyboardPrivateCommand);
    BindNativeFunction(env, exportObj, "getSmartMenuCfg", moduleName, JsKeyboardPanelManager::GetSmartMenuCfg);
    BindNativeFunction(env, exportObj, "on", moduleName, JsKeyboardPanelManager::RegisterCallback);
    return NapiGetUndefined(env);
}

void JsKeyboardPanelManager::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsKeyboardPanelManager>(static_cast<JsKeyboardPanelManager*>(data));
}

JsKeyboardPanelManager::JsKeyboardPanelManager(napi_env env) : env_(env)
{
    listenerFunc_ = {
        { KEYBOARD_RECEIVE_PRIVATE_COMMAND_CB,   &JsKeyboardPanelManager::ProcessKeyboardPrivateCommandRegister },
        { KEYBOARD_RECEIVE_IS_PANEL_SHOW_CB,     &JsKeyboardPanelManager::ProcessKeyboardIsPanelShowRegister },
    };
    taskScheduler_ = std::make_shared<MainThreadScheduler>(env);
}

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

napi_value JsKeyboardPanelManager::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]RegisterCallback");
    JsKeyboardPanelManager* me = CheckParamsAndGetThis<JsKeyboardPanelManager>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsKeyboardPanelManager::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        WLOGFE("[NAPI]Callback is nullptr or not callable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (IsCallbackRegistered(env, cbType, value)) {
        return NapiGetUndefined(env);
    }

    (this->*listenerFunc_[cbType])();
    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    {
        std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
        jsCbMap_[cbType] = callbackRef;
    }
    WLOGFD("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

bool JsKeyboardPanelManager::IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        return false;
    }

    for (auto iter = jsCbMap_.begin(); iter != jsCbMap_.end(); ++iter) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter->second->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

napi_value JsKeyboardPanelManager::GetSmartMenuCfg(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetSmartMenuCfg");
    JsKeyboardPanelManager *me = CheckParamsAndGetThis<JsKeyboardPanelManager>(env, info);
    return (me != nullptr) ? me->OnGetSmartMenuCfg(env, info) : nullptr;
}

napi_value JsKeyboardPanelManager::OnGetSmartMenuCfg(napi_env env, napi_callback_info info)
{
    // 4 represent the maximum number of the parameters
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= 1) {
        WLOGFE("Argc is invalid: %{public}zu, expect zero params", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    auto smartMenuCfg = KeyboardPanelManager::GetInstance().GetSmartMenuCfg();
    napi_value result = nullptr;
    napi_create_string_utf8(env, smartMenuCfg.c_str(), smartMenuCfg.length(), &result);
    return result;
}

napi_value JsKeyboardPanelManager::SendKeyboardPrivateCommand(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]SendKeyboardPrivateCommand");
    JsKeyboardPanelManager* me = CheckParamsAndGetThis<JsKeyboardPanelManager>(env, info);
    return (me != nullptr) ? me->OnSendKeyboardPrivateCommand(env, info) : nullptr;
}

napi_value JsKeyboardPanelManager::OnSendKeyboardPrivateCommand(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]OnSendKeyboardPrivateCommand");
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::unordered_map<std::string, KeyboardPrivateDataValue> privateCommand;
    if (!ConvertKeyboardPrivateCommandFromJs(env, argv[0], privateCommand)) {
        WLOGFE("[NAPI]failed to convert parameter");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    KeyboardPanelManager::GetInstance().SendKeyboardPrivateCommand(privateCommand);
    return NapiGetUndefined(env);
}

void JsKeyboardPanelManager::ProcessKeyboardPrivateCommandRegister()
{
    WLOGFI("[NAPI]ProcessKeyboardPrivateCommandRegister");
    NotifyReceiveKeyboardPrivateCommandFunc func = [this](const std::unordered_map<std::string,
        KeyboardPrivateDataValue>& privateCommand) {
        this->ReceiveKeyboardPrivateCommand(privateCommand);
    };
    KeyboardPanelManager::GetInstance().SetKeyboardPrivateCommandListener(func);
}

void JsKeyboardPanelManager::ReceiveKeyboardPrivateCommand(const std::unordered_map<std::string,
    KeyboardPrivateDataValue>& privateCommand)
{
    WLOGFI("[NAPI]ReceivePrivateCommand");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(KEYBOARD_RECEIVE_PRIVATE_COMMAND_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [privateCommand, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsPrivateCommand = GetJsKeyboardPrivateCommand(env, privateCommand);
        if (jsPrivateCommand == nullptr) {
            WLOGFE("[NAPI]GetJsPrivateCommand failed, jsPrivateCommand is null");
            return;
        }
        napi_value argv[] = {jsPrivateCommand};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "ReceivePrivateCommand");
}

void JsKeyboardPanelManager::ProcessKeyboardIsPanelShowRegister()
{
    WLOGFI("[NAPI]ProcessKeyboardStatusRegister");
    NotifyReceiveIsPanelShowFunc func = [this](bool isShow) {
        this->ReceiveKeyboardIsPanelShow(isShow);
    };
    KeyboardPanelManager::GetInstance().SetKeyboardPanelIsPanelShowsListener(func);
}

void JsKeyboardPanelManager::ReceiveKeyboardIsPanelShow(bool isShow)
{
    WLOGFI("[NAPI]ReceiveKeyboardStatus");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(KEYBOARD_RECEIVE_IS_PANEL_SHOW_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [isShow, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }

        napi_value result = CreateJsValue(env, isShow);
        if (result == nullptr) {
            WLOGFE("[NAPI]GetJsValue failed, result is null");
            return;
        }
        napi_value argv[] = {result};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "ReceiveKeyboardStatus");
}
} // namespace OHOS::Rosen
