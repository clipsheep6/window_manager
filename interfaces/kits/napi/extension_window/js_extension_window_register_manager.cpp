/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include "js_extension_window_register_manager.h"
#include "singleton_container.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "js_window_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsExtensionWindowRegisterManager"};
const std::string WINDOW_SIZE_CHANGE_CB = "windowSizeChange";
const std::string AVOID_AREA_CHANGE_CB = "avoidAreaChange";
const std::string WINDOW_STAGE_EVENT_CB = "windowStageEvent";
const std::string WINDOW_EVENT_CB = "windowEvent";
}

JsExtensionWindowRegisterManager::JsExtensionWindowRegisterManager()
{
    // white register list for window
    listenerProcess_[CaseType::CASE_WINDOW] = {
            { WINDOW_SIZE_CHANGE_CB, &JsExtensionWindowRegisterManager::ProcessWindowChangeRegister },
            { AVOID_AREA_CHANGE_CB, &JsExtensionWindowRegisterManager::ProcessAvoidAreaChangeRegister },
            { WINDOW_EVENT_CB, &JsExtensionWindowRegisterManager::ProcessLifeCycleEventRegister },
    };
    // white register list for window stage
    listenerProcess_[CaseType::CASE_STAGE] = {
            {WINDOW_STAGE_EVENT_CB, &JsExtensionWindowRegisterManager::ProcessLifeCycleEventRegister }
    };
}

JsExtensionWindowRegisterManager::~JsExtensionWindowRegisterManager()
{
}

WmErrorCode JsExtensionWindowRegisterManager::ProcessWindowChangeRegister(sptr<JsExtensionWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsExtensionWindowRegisterManager::ProcessAvoidAreaChangeRegister(sptr<JsExtensionWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IAvoidAreaChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterAvoidAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterAvoidAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsExtensionWindowRegisterManager::ProcessLifeCycleEventRegister(sptr<JsExtensionWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        WLOGFE("Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowLifeCycle> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterLifeCycleListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterLifeCycleListener(thisListener));
    }
    return ret;
}

bool JsExtensionWindowRegisterManager::IsCallbackRegistered(napi_env env, std::string type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("[NAPI]Method %{public}s has not been registered", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_[type].begin(); iter != jsCbMap_[type].end(); ++iter) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter->first->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

WmErrorCode JsExtensionWindowRegisterManager::RegisterListener(sptr<Window> window, CaseType caseType,
    const JsWindowListenerInfo& listenerInfo)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCallbackRegistered(listenerInfo.env, listenerInfo.type, listenerInfo.callback)) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerProcess_[caseType].count(listenerInfo.type) == 0) {
        WLOGFE("[NAPI]Type %{public}s is not supported", listenerInfo.type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    napi_ref result = nullptr;
    napi_create_reference(listenerInfo.env, listenerInfo.callback, 1, &result);
    std::shared_ptr<NativeReference> callbackRef(reinterpret_cast<NativeReference*>(result));
    sptr<JsExtensionWindowListener> extensionWindowListener =
        new(std::nothrow) JsExtensionWindowListener(listenerInfo.env, callbackRef);
    if (extensionWindowListener == nullptr) {
        WLOGFE("[NAPI]New JsExtensionWindowListener failed");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    extensionWindowListener->SetMainEventHandler();
    WmErrorCode ret = (this->*listenerProcess_[caseType][listenerInfo.type])(extensionWindowListener,
        window, true, listenerInfo.env, listenerInfo.parameters);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Register type %{public}s failed", listenerInfo.type.c_str());
        return ret;
    }
    jsCbMap_[listenerInfo.type][callbackRef] = extensionWindowListener;
    WLOGI("[NAPI]Register type %{public}s success! callback map size: %{public}zu",
        listenerInfo.type.c_str(), jsCbMap_[listenerInfo.type].size());
    return WmErrorCode::WM_OK;
}

WmErrorCode JsExtensionWindowRegisterManager::UnregisterListener(sptr<Window> window, CaseType caseType,
    const JsWindowListenerInfo& listenerInfo)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCbMap_.empty() || jsCbMap_.find(listenerInfo.type) == jsCbMap_.end()) {
        WLOGFE("[NAPI]Type %{public}s was not registerted", listenerInfo.type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerProcess_[caseType].count(listenerInfo.type) == 0) {
        WLOGFE("[NAPI]Type %{public}s is not supported", listenerInfo.type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerInfo.callback == nullptr) {
        for (auto it = jsCbMap_[listenerInfo.type].begin(); it != jsCbMap_[listenerInfo.type].end();) {
            WmErrorCode ret = (this->*listenerProcess_[caseType][listenerInfo.type])(it->second,
                window, false, listenerInfo.env, {});
            if (ret != WmErrorCode::WM_OK) {
                WLOGFE("[NAPI]Unregister type %{public}s failed, no value", listenerInfo.type.c_str());
                return ret;
            }
            jsCbMap_[listenerInfo.type].erase(it++);
        }
    } else {
        bool findFlag = false;
        for (auto it = jsCbMap_[listenerInfo.type].begin(); it != jsCbMap_[listenerInfo.type].end(); ++it) {
            bool isEquals = false;
            napi_strict_equals(listenerInfo.env, listenerInfo.callback, it->first->GetNapiValue(), &isEquals);
            if (!isEquals) {
                continue;
            }
            findFlag = true;
            WmErrorCode ret = (this->*listenerProcess_[caseType][listenerInfo.type])(it->second,
                window, false, listenerInfo.env, {});
            if (ret != WmErrorCode::WM_OK) {
                WLOGFE("[NAPI]Unregister type %{public}s failed", listenerInfo.type.c_str());
                return ret;
            }
            jsCbMap_[listenerInfo.type].erase(it);
            break;
        }
        if (!findFlag) {
            WLOGFE("[NAPI]Unregister type %{public}s failed because not found callback!", listenerInfo.type.c_str());
            return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
        }
    }
    WLOGI("[NAPI]Unregister type %{public}s success! callback map size: %{public}zu",
        listenerInfo.type.c_str(), jsCbMap_[listenerInfo.type].size());
    // erase type when there is no callback in one type
    if (jsCbMap_[listenerInfo.type].empty()) {
        jsCbMap_.erase(listenerInfo.type);
    }
    return WmErrorCode::WM_OK;
}
} // namespace Rosen
} // namespace OHOS