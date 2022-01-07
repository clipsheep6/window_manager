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

#include "js_window.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "window_option.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindow"};
}
JsWindow::JsWindow(const sptr<Window>& window, NativeEngine& engine) : windowToken_(window)
{
    windowListener_ = new JsWindowListener(&engine);
}

void JsWindow::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsWindow::Finalizer is called");
    std::unique_ptr<JsWindow>(static_cast<JsWindow*>(data));
}

NativeValue* JsWindow::Show(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::Show is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnShow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Destroy(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::Destroy is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDestroy(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Hide(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::Hide is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnHide(*engine, *info) : nullptr;
}

NativeValue* JsWindow::MoveTo(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::MoveTo is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnMoveTo(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Resize(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::Resize is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnResize(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowType(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::SetWindowType is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowType(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::SetWindowMode is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowMode(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetProperties(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::GetProperties is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetProperties(*engine, *info) : nullptr;
}

NativeValue* JsWindow::RegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::RegisterWindowCallback is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnRegisterWindowCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindow::UnRegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindow::UnRegisterWindowCallback is called");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnUnRegisterWindowCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindow::OnShow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnShow is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->Show();
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindow::OnShow success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "ShowWindow failed."));
            }
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr : info.argv[0];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDestroy(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnDestroy is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->Destroy();
            windowToken_ = nullptr;
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindow::OnDestroy success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "JsWindow::OnDestroy failed."));
            }
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr : info.argv[0];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnHide(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnHide is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->Hide();
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindow::OnHide success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "JsWindow::OnHide failed."));
            }
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr : info.argv[0];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnMoveTo(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnMoveTo is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    int32_t x;
    if (!ConvertFromJsValue(engine, info.argv[0], x)) {
        WLOGFE("Failed to convert parameter to x");
        return engine.CreateUndefined();
    }

    int32_t y;
    if (!ConvertFromJsValue(engine, info.argv[1], y)) {
        WLOGFE("Failed to convert parameter to y");
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this, x, y](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->MoveTo(x, y);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindow::OnMoveTo success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "JsWindow::OnMoveTo failed."));
            }
        };

    NativeValue* lastParam = (info.argc == ARGC_TWO) ? nullptr : info.argv[INDEX_TWO];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnResize(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnResize is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    uint32_t width;
    if (!ConvertFromJsValue(engine, info.argv[0], width)) {
        WLOGFE("Failed to convert parameter to width");
        return engine.CreateUndefined();
    }

    uint32_t height;
    if (!ConvertFromJsValue(engine, info.argv[1], height)) {
        WLOGFE("Failed to convert parameter to height");
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this, width, height](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->Resize(width, height);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "JsWindow::OnResize failed."));
            }
        };
    NativeValue* lastParam = (info.argc == ARGC_TWO) ? nullptr : info.argv[INDEX_TWO];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowType(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnSetWindowType is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeType == nullptr) {
        WLOGFE("Failed to convert parameter to windowType");
        return engine.CreateUndefined();
    }
    WindowType winType = static_cast<WindowType>(static_cast<uint32_t>(*nativeType));
    AsyncTask::CompleteCallback complete =
        [this, winType](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->SetWindowType(winType);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindow::OnSetWindowType success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "JsWindow::OnSetWindowType failed."));
            }
        };

    NativeValue* lastParam = (info.argc == ARGC_ONE) ? nullptr : info.argv[INDEX_ONE];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnSetWindowMode is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeMode == nullptr) {
        WLOGFE("Failed to convert parameter to windowMode");
        return engine.CreateUndefined();
    }
    WindowMode winMode = static_cast<WindowMode>(static_cast<uint32_t>(*nativeMode));
    AsyncTask::CompleteCallback complete =
        [this, winMode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            WMError ret = windowToken_->SetWindowMode(winMode);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindow::OnSetWindowMode success");
            } else {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(ret), "JsWindow::OnSetWindowMode failed."));
            }
        };

    NativeValue* lastParam = (info.argc == ARGC_ONE) ? nullptr : info.argv[INDEX_ONE];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetProperties(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnGetProperties is called");
    if (windowToken_ == nullptr) {
        WLOGFE("JsWindow windowToken_ is nullptr");
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto objValue = CreateJsWindowPropertiesObject(engine, windowToken_);
            WLOGFI("JsWindow::OnGetProperties objValue %{public}p", objValue);
            if (objValue != nullptr) {
                task.Resolve(engine, objValue);
                WLOGFI("JsWindow::OnGetProperties success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnGetProperties failed."));
            }
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr : info.argv[0];
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnRegisterWindowCallback is called");
    if (windowToken_ == nullptr || windowListener_ == nullptr) {
        WLOGFE("JsWindow windowToken_ or windowListener_ is nullptr");
        return engine.CreateUndefined();
    }
    if (info.argc != ARGC_TWO) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFI("JsWindow::OnRegisterWindowCallback info->argv[1] is not callable");
        return engine.CreateUndefined();
    }
    if (!windowListener_->AddJsListenerObject(cbType, info.argv[1])) {
        WLOGFI("JsWindow::OnRegisterWindowCallback failed");
        return engine.CreateUndefined();
    }
    if (cbType.compare("windowSizeChange") == 0) {
        sptr<IWindowChangeListener> thisListener(windowListener_);
        windowToken_->RegisterWindowChangeListener(thisListener);
        WLOGFI("JsWindow::OnRegisterWindowCallback success");
    }
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnUnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindow::OnUnRegisterWindowCallback is called");
    if (windowToken_ == nullptr || windowListener_ == nullptr) {
        WLOGFE("JsWindow windowToken_ or windowListener_ is nullptr");
        return engine.CreateUndefined();
    }
    if (info.argc == 0) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* lastParam = (info.argc == 1) ? nullptr : info.argv[1];
    windowListener_->RemoveJsListenerObject(cbType, lastParam);
    return engine.CreateUndefined();
}

NativeValue* CreateJsWindowObject(NativeEngine& engine, sptr<Window>& window)
{
    WLOGFI("JsWindow::CreateJsWindow is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsWindow> jsWindow = std::make_unique<JsWindow>(window, engine);
    object->SetNativePointer(jsWindow.release(), JsWindow::Finalizer, nullptr);

    BindNativeFunction(engine, *object, "show", JsWindow::Show);
    BindNativeFunction(engine, *object, "destroy", JsWindow::Destroy);
    BindNativeFunction(engine, *object, "hide", JsWindow::Hide);
    BindNativeFunction(engine, *object, "moveTo", JsWindow::MoveTo);
    BindNativeFunction(engine, *object, "resetSize", JsWindow::Resize);
    BindNativeFunction(engine, *object, "setWindowType", JsWindow::SetWindowType);
    BindNativeFunction(engine, *object, "setWindowMode", JsWindow::SetWindowMode);
    BindNativeFunction(engine, *object, "getProperties", JsWindow::GetProperties);
    BindNativeFunction(engine, *object, "on", JsWindow::RegisterWindowCallback);
    BindNativeFunction(engine, *object, "off", JsWindow::UnRegisterWindowCallback);
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
