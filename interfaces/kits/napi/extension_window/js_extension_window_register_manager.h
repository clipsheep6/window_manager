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

#ifndef OHOS_JS_EXTENSION_WINDOW_REGISTER_MANAGER_H
#define OHOS_JS_EXTENSION_WINDOW_REGISTER_MANAGER_H

#include <map>
#include <mutex>
#include "js_window_utils.h"
#include "js_extension_window_listener.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
struct JsWindowListenerInfo;

class JsExtensionWindowRegisterManager {
public:
    JsExtensionWindowRegisterManager();
    ~JsExtensionWindowRegisterManager();
    WmErrorCode RegisterListener(sptr<Window> window, CaseType caseType, const JsWindowListenerInfo& listenerInfo);
    WmErrorCode UnregisterListener(sptr<Window> window, CaseType caseType, const JsWindowListenerInfo& listenerInfo);

private:
    bool IsCallbackRegistered(napi_env env, std::string type, napi_value jsListenerObject);
    WmErrorCode ProcessWindowChangeRegister(sptr<JsExtensionWindowListener> listener,
        sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessAvoidAreaChangeRegister(sptr<JsExtensionWindowListener> listener,
        sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    using Func = WmErrorCode(JsExtensionWindowRegisterManager::*)(sptr<JsExtensionWindowListener>,
        sptr<Window> window, bool, napi_env env, const std::vector<napi_value>& parameters);
    WmErrorCode ProcessLifeCycleEventRegister(sptr<JsExtensionWindowListener> listener,
        sptr<Window> window, bool isRegister, napi_env env, const std::vector<napi_value>& parameters);
    std::map<std::string, std::map<std::shared_ptr<NativeReference>, sptr<JsExtensionWindowListener>>> jsCbMap_;
    std::mutex mtx_;
    std::map<CaseType, std::map<std::string, Func>> listenerProcess_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_EXTENSION_WINDOW_REGISTER_MANAGER_H
