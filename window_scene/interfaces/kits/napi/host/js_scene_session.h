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

#ifndef OHOS_JS_SCENE_SESSION_H
#define OHOS_JS_SCENE_SESSION_H

#include <map>
#include <string>
#include <refbase.h>
#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

namespace OHOS::Rosen {
class SceneSession;
class JsSceneSession final : public std::enable_shared_from_this<JsSceneSession> {
public:
    explicit JsSceneSession(NativeEngine* engine) : engine_(engine) {}
    ~JsSceneSession() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);

    void StartScene(const sptr<SceneSession>& session);

private:
    NativeValue* OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info);
    void CallJsMethod(const char* methodName, NativeValue* const* argv, size_t argc);

    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    NativeEngine* engine_ = nullptr;
    std::shared_ptr<NativeReference> jsCallBack_ = nullptr;
};

NativeValue* CreateJsSceneSessionObject(NativeEngine& engine, const sptr<SceneSession>& session,
    const std::shared_ptr<JsSceneSession>& jsSceneSession);
void BindFunctions(NativeEngine& engine, NativeObject* object, const char *moduleName);
}
#endif // OHOS_JS_SCENE_SESSION_H
