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
#ifndef OHOS_JS_WINDOW_REGISTER_MANAGER_H
#define OHOS_JS_WINDOW_REGISTER_MANAGER_H
#include <map>
#include <mutex>
#include "js_window_listener.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"
namespace OHOS {
namespace Rosen {

/**
 * @struct JsWindowListenerInfo.
 */
struct JsWindowListenerInfo {
    napi_env env;
    std::string type;
    napi_value callback;
    std::vector<napi_value> parameters;
};

class JsWindowRegisterManager {
public:
    JsWindowRegisterManager();
    ~JsWindowRegisterManager();
    WmErrorCode RegisterListener(sptr<Window> window, CaseType caseType, const JsWindowListenerInfo& listenerInfo);
    WmErrorCode UnregisterListener(sptr<Window> window, CaseType caseType, const JsWindowListenerInfo& listenerInfo);
private:
    bool IsCallbackRegistered(napi_env env, std::string type, napi_value jsListenerObject);
    WmErrorCode ProcessWindowChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessSystemAvoidAreaChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessAvoidAreaChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessLifeCycleEventRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessOccupiedAreaChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessSystemBarChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessTouchOutsideRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessScreenshotRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessDialogTargetTouchRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister,
        napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessDialogDeathRecipientRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessGestureNavigationEnabledChangeRegister(sptr<JsWindowListener> listener,
        sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessWaterMarkFlagChangeRegister(sptr<JsWindowListener> listener,
        sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessWindowVisibilityChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessWindowNoInteractionRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessWindowStatusChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessWindowTitleButtonRectChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window,
        bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    using Func = WmErrorCode(JsWindowRegisterManager::*)(sptr<JsWindowListener>, sptr<Window> window, bool,
        napi_env env, const std::vector<napi_value>& parameters);
    std::map<std::string, std::map<std::shared_ptr<NativeReference>, sptr<JsWindowListener>>> jsCbMap_;
    std::mutex mtx_;
    std::map<CaseType, std::map<std::string, Func>> listenerProcess_;
};
} // namespace Rosen
} // namespace OHOS

#endif