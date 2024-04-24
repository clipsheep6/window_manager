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

#ifndef OHOS_WINDOW_SCENE_JS_KEYBOARD_PANEL_MANAGER_H
#define OHOS_WINDOW_SCENE_JS_KEYBOARD_PANEL_MANAGER_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <js_runtime_utils.h>

#include "interfaces/kits/napi/common/js_common_utils.h"
#include "js_keyboard_panel_utils.h"
#include "task_scheduler.h"

namespace OHOS::Rosen {
class JsKeyboardPanelManager final {
public:
    explicit JsKeyboardPanelManager(napi_env env);
    ~JsKeyboardPanelManager() = default;

    static napi_value Init(napi_env env, napi_value exportObj);
    static void Finalizer(napi_env env, void* data, void* hint);

    static napi_value SendKeyboardPrivateCommand(napi_env env, napi_callback_info info);
    static napi_value GetSmartMenuCfg(napi_env env, napi_callback_info info);
    static napi_value RegisterCallback(napi_env env, napi_callback_info info);
private:
    napi_value OnRegisterCallback(napi_env env, napi_callback_info info);
    napi_value OnSendKeyboardPrivateCommand(napi_env env, napi_callback_info info);
    napi_value OnGetSmartMenuCfg(napi_env env, napi_callback_info info);
    bool IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject);
    void ProcessKeyboardPrivateCommandRegister();
    void ProcessKeyboardIsPanelShowRegister();

    void ReceiveKeyboardPrivateCommand(const std::unordered_map<std::string, KeyboardPrivateDataValue>& privateCommand);
    void ReceiveKeyboardIsPanelShow(bool isShow);

    napi_env env_;
    std::shared_mutex jsCbMapMutex_;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    using Func = void(JsKeyboardPanelManager::*)();

    std::map<std::string, Func> listenerFunc_;
    std::shared_ptr<MainThreadScheduler> taskScheduler_;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_JS_KEYBOARD_PANEL_MANAGER_H