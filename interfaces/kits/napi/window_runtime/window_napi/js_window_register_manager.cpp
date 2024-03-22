/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "js_window_register_manager.h"
#include "singleton_container.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "js_runtime_utils.h"
#include "js_window_utils.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsRegisterManager"};
}

JsWindowRegisterManager::JsWindowRegisterManager()
{
    // white register list for window manager
    listenerProcess_[CaseType::CASE_WINDOW_MANAGER] = {
        {SYSTEM_BAR_TINT_CHANGE_CB,            &JsWindowRegisterManager::ProcessSystemBarChangeRegister               },
        {GESTURE_NAVIGATION_ENABLED_CHANGE_CB, &JsWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister},
        {WATER_MARK_FLAG_CHANGE_CB,            &JsWindowRegisterManager::ProcessWaterMarkFlagChangeRegister           },
    };
    // white register list for window
    listenerProcess_[CaseType::CASE_WINDOW] = {
        { WINDOW_SIZE_CHANGE_CB,              &JsWindowRegisterManager::ProcessWindowChangeRegister               },
        { SYSTEM_AVOID_AREA_CHANGE_CB,        &JsWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister      },
        { AVOID_AREA_CHANGE_CB,               &JsWindowRegisterManager::ProcessAvoidAreaChangeRegister            },
        { LIFECYCLE_EVENT_CB,                 &JsWindowRegisterManager::ProcessLifeCycleEventRegister             },
        { WINDOW_EVENT_CB,                    &JsWindowRegisterManager::ProcessLifeCycleEventRegister             },
        { KEYBOARD_HEIGHT_CHANGE_CB,          &JsWindowRegisterManager::ProcessOccupiedAreaChangeRegister         },
        { TOUCH_OUTSIDE_CB,                   &JsWindowRegisterManager::ProcessTouchOutsideRegister               },
        { SCREENSHOT_EVENT_CB,                &JsWindowRegisterManager::ProcessScreenshotRegister                 },
        { DIALOG_TARGET_TOUCH_CB,             &JsWindowRegisterManager::ProcessDialogTargetTouchRegister          },
        { DIALOG_DEATH_RECIPIENT_CB,          &JsWindowRegisterManager::ProcessDialogDeathRecipientRegister       },
        { WINDOW_STATUS_CHANGE_CB,            &JsWindowRegisterManager::ProcessWindowStatusChangeRegister         },
        { WINDOW_TITLE_BUTTON_RECT_CHANGE_CB, &JsWindowRegisterManager::ProcessWindowTitleButtonRectChangeRegister},
        { WINDOW_VISIBILITY_CHANGE_CB,        &JsWindowRegisterManager::ProcessWindowVisibilityChangeRegister     },
        { WINDOW_NO_INTERACTION_DETECT_CB,    &JsWindowRegisterManager::ProcessWindowNoInteractionRegister        },
    };
    // white register list for window stage
    listenerProcess_[CaseType::CASE_STAGE] = {
        {WINDOW_STAGE_EVENT_CB,         &JsWindowRegisterManager::ProcessLifeCycleEventRegister    }
    };
}

JsWindowRegisterManager::~JsWindowRegisterManager()
{
}

WmErrorCode JsWindowRegisterManager::ProcessWindowChangeRegister(sptr<JsWindowListener> listener,
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

WmErrorCode JsWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]listener is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    listener->SetIsDeprecatedInterface(true);
    sptr<IAvoidAreaChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterAvoidAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterAvoidAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessAvoidAreaChangeRegister(sptr<JsWindowListener> listener,
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

WmErrorCode JsWindowRegisterManager::ProcessLifeCycleEventRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
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

WmErrorCode JsWindowRegisterManager::ProcessOccupiedAreaChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IOccupiedAreaChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterOccupiedAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterOccupiedAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessSystemBarChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    sptr<ISystemBarChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().RegisterSystemBarChangedListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().UnregisterSystemBarChangedListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    sptr<IGestureNavigationEnabledChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().RegisterGestureNavigationEnabledChangedListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().UnregisterGestureNavigationEnabledChangedListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWaterMarkFlagChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    sptr<IWaterMarkFlagChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().RegisterWaterMarkFlagChangedListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().UnregisterWaterMarkFlagChangedListener(thisListener));
    }
    return ret;
}
WmErrorCode JsWindowRegisterManager::ProcessTouchOutsideRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    WLOGI("called");
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<ITouchOutsideListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterTouchOutsideListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterTouchOutsideListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowVisibilityChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    WLOGD("called");
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowVisibilityChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowVisibilityChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowVisibilityChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowNoInteractionRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    WLOGD("called");
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }

    sptr<IWindowNoInteractionListener> thisListener(listener);

    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        uint32_t timeout = 0;
        if (!ConvertFromJsNumber(env, parameters.at(0), timeout)) {
            WLOGFE("Failed to convert parameter to timeout");
            return WmErrorCode::WM_ERROR_INVALID_PARAM;
        }
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowNoInteractionListener(thisListener, timeout));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowNoInteractionListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessScreenshotRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    WLOGI("called");
    if (window == nullptr) {
        WLOGFE("%{public}sregister screenshot listener failed. window is null", isRegister? "" : "un");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IScreenshotListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterScreenshotListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterScreenshotListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessDialogTargetTouchRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IDialogTargetTouchListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterDialogTargetTouchListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterDialogTargetTouchListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessDialogDeathRecipientRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IDialogDeathRecipientListener> thisListener(listener);
    if (isRegister) {
        window->RegisterDialogDeathRecipientListener(thisListener);
    } else {
        window->UnregisterDialogDeathRecipientListener(thisListener);
    }
    return WmErrorCode::WM_OK;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowTitleButtonRectChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    WLOGD("called");
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowTitleButtonRectChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowTitleButtonRectChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowTitleButtonRectChangeListener(thisListener));
    }
    return ret;
}

bool JsWindowRegisterManager::IsCallbackRegistered(napi_env env, std::string type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("[NAPI]Method %{public}s has not been registerted", type.c_str());
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

WmErrorCode JsWindowRegisterManager::RegisterListener(sptr<Window> window, CaseType caseType,
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
    sptr<JsWindowListener> windowManagerListener = new(std::nothrow) JsWindowListener(listenerInfo.env, callbackRef);
    if (windowManagerListener == nullptr) {
        WLOGFE("[NAPI]New JsWindowListener failed");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    windowManagerListener->SetMainEventHandler();
    WmErrorCode ret = (this->*listenerProcess_[caseType][listenerInfo.type])(windowManagerListener,
        window, true, listenerInfo.env, listenerInfo.parameters);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Register type %{public}s failed", listenerInfo.type.c_str());
        return ret;
    }
    jsCbMap_[listenerInfo.type][callbackRef] = windowManagerListener;
    WLOGI("[NAPI]Register type %{public}s success! callback map size: %{public}zu",
        listenerInfo.type.c_str(), jsCbMap_[listenerInfo.type].size());
    return WmErrorCode::WM_OK;
}

WmErrorCode JsWindowRegisterManager::UnregisterListener(sptr<Window> window, CaseType caseType,
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

WmErrorCode JsWindowRegisterManager::ProcessWindowStatusChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowStatusChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowStatusChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowStatusChangeListener(thisListener));
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS