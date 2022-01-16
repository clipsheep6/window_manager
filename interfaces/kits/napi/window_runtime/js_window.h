/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_WINDOW_H
#define OHOS_JS_WINDOW_H
#include "js_runtime_utils.h"
#include "js_window_listener.h"
#include "js_window_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"
namespace OHOS {
namespace Rosen {
NativeValue* CreateJsWindowObject(NativeEngine& engine, sptr<Window>& window);

class JsWindow final {
public:
    JsWindow(const sptr<Window>& window, NativeEngine& engine);
    ~JsWindow() = default;
    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* Show(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Destroy(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Hide(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* MoveTo(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Resize(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowType(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowMode(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetProperties(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* UnRegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* LoadContent(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnShow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnDestroy(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnHide(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnMoveTo(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnResize(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowType(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowMode(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetProperties(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnUnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info);
    sptr<Window> windowToken_ = nullptr;
    sptr<JsWindowListener> windowListener_ = nullptr;
    void* contentStorage_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif