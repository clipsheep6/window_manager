/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "js_screen_session_manager.h"

#include <js_runtime_utils.h>
#include "session/screen/include/screen_session.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

#include "interfaces/include/ws_common.h"
#include "js_screen_session.h"
#include "js_screen_utils.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr size_t ARGC_ONE = 1;
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsScreenSessionManager" };
const std::string ON_SCREEN_CONNECTION_CHANGE_CALLBACK = "screenConnectChange";
} // namespace

JsScreenSessionManager::JsScreenSessionManager(NativeEngine& engine) : engine_(engine) {}

JsScreenSessionManager::~JsScreenSessionManager() {}

NativeValue* JsScreenSessionManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGD("Init.");
    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("Failed to init, engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("Failed to convert native value to native object, Object is null!");
        return nullptr;
    }

    auto jsScreenSessionManager = std::make_unique<JsScreenSessionManager>(*engine);
    object->SetNativePointer(jsScreenSessionManager.release(), JsScreenSessionManager::Finalizer, nullptr);
    object->SetProperty("ScreenConnectChangeType", JsScreenUtils::CreateJsScreenConnectChangeType(*engine));

    const char* moduleName = "JsScreenSessionManager";
    BindNativeFunction(*engine, *object, "on", moduleName, JsScreenSessionManager::RegisterCallback);
    BindNativeFunction(*engine, *object, "getCutoutInfo", moduleName, JsScreenSessionManager::GetCutoutInfo);
    return engine->CreateUndefined();
}

void JsScreenSessionManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGD("Finalizer.");
    std::unique_ptr<JsScreenSessionManager>(static_cast<JsScreenSessionManager*>(data));
}

void JsScreenSessionManager::OnScreenConnect(sptr<ScreenSession>& screenSession)
{
    if (screenConnectionCallback_ == nullptr) {
        return;
    }

    std::shared_ptr<NativeReference> callback_ = screenConnectionCallback_;
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback>(
        [callback_, screenSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* objValue = engine.CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
            if (object == nullptr) {
                WLOGFE("Object is null!");
                return;
            }

            object->SetProperty("screenSession", JsScreenSession::Create(engine, screenSession));
            object->SetProperty("screenConnectChangeType", CreateJsValue(engine, 0));

            NativeValue* argv[] = { objValue };
            NativeValue* method = callback_->Get();
            if (method == nullptr) {
                WLOGFE("Failed to get method callback from object!");
                return;
            }

            engine.CallFunction(engine.CreateUndefined(), method, argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsScreenSessionManager::OnScreenConnect", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSessionManager::OnScreenDisconnect(sptr<ScreenSession>& screenSession)
{
    if (screenConnectionCallback_ == nullptr) {
        return;
    }

    std::shared_ptr<NativeReference> callback_ = screenConnectionCallback_;
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback>(
        [callback_, screenSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* objValue = engine.CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
            if (object == nullptr) {
                WLOGFE("Object is null!");
                return;
            }

            object->SetProperty("screenSession", JsScreenSession::Create(engine, screenSession));
            object->SetProperty("screenConnectChangeType", CreateJsValue(engine, 1));

            NativeValue* argv[] = { objValue };
            NativeValue* method = callback_->Get();
            if (method == nullptr) {
                WLOGFE("Failed to get method callback from object!");
                return;
            }

            engine.CallFunction(engine.CreateUndefined(), method, argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsScreenSessionManager::OnScreenDisconnect", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

NativeValue* JsScreenSessionManager::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGD("Register callback.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

NativeValue* JsScreenSessionManager::OnRegisterCallback(NativeEngine& engine, const NativeCallbackInfo& info)
{
    WLOGD("On register callback.");
    if (screenConnectionCallback_ != nullptr) {
        return engine.CreateUndefined();
    }

    if (info.argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    std::string callbackType;
    if (!ConvertFromJsValue(engine, info.argv[0], callbackType)) {
        WLOGFE("Failed to convert parameter to callback type.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    if (callbackType != ON_SCREEN_CONNECTION_CHANGE_CALLBACK) {
        WLOGFE("Unsupported callback type: %{public}s.", callbackType.c_str());
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFE("Failed to register callback, callback is not callable!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    std::shared_ptr<NativeReference> callbackRef(engine.CreateReference(value, 1));
    screenConnectionCallback_ = callbackRef;
    sptr<IScreenConnectionListener> screenConnectionListener(this);
    ScreenSessionManager::GetInstance().RegisterScreenConnectionListener(screenConnectionListener);
    return engine.CreateUndefined();
}

NativeValue* JsScreenSessionManager::GetCutoutInfo(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("get cutout Info is called");
    JsScreenSessionManager* jsm = CheckParamsAndGetThis<JsScreenSessionManager>(engine, info);
    return (jsm != nullptr) ? jsm->OnGetCutoutInfo(*engine, *info) : nullptr;
}

NativeValue* JsScreenSessionManager::OnGetCutoutInfo(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("on get cutout info is called");
    if (!ScreenSessionManager::GetInstance().GetScreenCutoutController()) {
        return nullptr;
    }

    AsyncTask::CompleteCallback complete = [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
        sptr<CutoutInfo> cutoutInfo =
                ScreenSessionManager::GetInstance().GetScreenCutoutController()->GetScreenCutoutInfo();
            if (cutoutInfo != nullptr) {
                task.Resolve(engine, JsScreenUtils::CreateJsCutoutInfoObject(engine, cutoutInfo));
                WLOGI("OnGetCutoutInfo success");
            } else {
                WLOGI("OnGetCutoutInfo failed");
                task.Reject(engine, CreateJsError(engine,static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsScreenSessionManager::OnGetCutoutInfo failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_ONE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenSessionManager::OnGetCutoutInfo", engine,
        CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}
} // namespace OHOS::Rosen
