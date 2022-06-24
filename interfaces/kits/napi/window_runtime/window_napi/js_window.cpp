/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <new>
#include "js_window_utils.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "window_option.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindow"};
    constexpr Rect g_emptyRect = {0, 0, 0, 0};
}

static thread_local std::map<std::string, std::shared_ptr<NativeReference>> g_jsWindowMap;
std::recursive_mutex g_mutex;
JsWindow::JsWindow(const sptr<Window>& window)
    : windowToken_(window), registerManager_(std::make_unique<JsWindowRegisterManager>())
{
    NotifyNativeWinDestroyFunc func = [](std::string windowName) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (windowName.empty() || g_jsWindowMap.count(windowName) == 0) {
            WLOGE("[NAPI]Can not find window %{public}s ", windowName.c_str());
            return;
        }
        g_jsWindowMap.erase(windowName);
        WLOGE("[NAPI]Destroy window %{public}s in js window", windowName.c_str());
    };
    windowToken_->RegisterWindowDestroyedListener(func);
}

JsWindow::~JsWindow()
{
    WLOGFI("[NAPI]~JsWindow");
    windowToken_ = nullptr;
}

std::string JsWindow::GetWindowName()
{
    if (windowToken_ == nullptr) {
        return "";
    }
    return windowToken_->GetWindowName();
}

void JsWindow::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("[NAPI]Finalizer");
    auto jsWin = std::unique_ptr<JsWindow>(static_cast<JsWindow*>(data));
    if (jsWin == nullptr) {
        WLOGFE("[NAPI]jsWin is nullptr");
        return;
    }
    std::string windowName = jsWin->GetWindowName();
    WLOGFI("[NAPI]Window %{public}s", windowName.c_str());
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_jsWindowMap.find(windowName) != g_jsWindowMap.end()) {
        g_jsWindowMap.erase(windowName);
        WLOGFI("[NAPI]Remove window %{public}s from g_jsWindowMap", windowName.c_str());
    }
}

NativeValue* JsWindow::Show(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]Show");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnShow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Destroy(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]Destroy");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDestroy(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Hide(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]Hide");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnHide(*engine, *info) : nullptr;
}

//test
NativeValue* JsWindow::CreateTransitionController(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]CreateTransitionController");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnCreateTransitionController(*engine, *info) : nullptr;
}

NativeValue* JsWindow::MoveTo(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]MoveTo");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnMoveTo(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Resize(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]Resize");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnResize(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowType(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetWindowType");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowType(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetWindowMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowMode(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetProperties(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]GetProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetProperties(*engine, *info) : nullptr;
}

NativeValue* JsWindow::RegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RegisterWindowCallback");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnRegisterWindowCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindow::UnregisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]UnregisterWindowCallback");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnUnregisterWindowCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindow::LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]LoadContent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetFullScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetFullScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetLayoutFullScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetLayoutFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetLayoutFullScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetSystemBarEnable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetSystemBarEnable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetSystemBarEnable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetSystemBarProperties(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetSystemBarProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetSystemBarProperties(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetAvoidArea(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]GetAvoidArea");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetAvoidArea(*engine, *info) : nullptr;
}

NativeValue* JsWindow::IsShowing(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]IsShowing");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnIsShowing(*engine, *info) : nullptr;
}

NativeValue* JsWindow::IsSupportWideGamut(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]IsSupportWideGamut");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnIsSupportWideGamut(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBackgroundColor(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetBackgroundColor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBackgroundColor(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBrightness(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetBrightness");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBrightness(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetDimBehind(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetDimBehind");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetDimBehind(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetFocusable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetFocusable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetFocusable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetKeepScreenOn(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetKeepScreenOn");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetKeepScreenOn(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetOutsideTouchable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetOutsideTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetOutsideTouchable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetPrivacyMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetPrivacyMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetPrivacyMode(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetTouchable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetTouchable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetTransparent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetTransparent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetTransparent(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetCallingWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetCallingWindow");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetCallingWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::DisableWindowDecor(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]DisableWindowDecor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDisableWindowDecor(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetColorSpace(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetColorSpace(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetColorSpace(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]GetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetColorSpace(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Dump(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]Dump");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDump(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetForbidSplitMove(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]SetForbidSplitMove");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetForbidSplitMove(*engine, *info) : nullptr;
}

NativeValue* JsWindow::OnShow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Show();
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] show end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnShow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}
//test
NativeValue* JsWindow::OnCreateTransitionController(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }

            // auto objValue = CreateJsWindowTransitionControllerObject(engine);
            // if (objValue != nullptr) {
            //     task.Resolve(engine, objValue);
            // } else {
            //     task.Reject(engine, CreateJsError(engine,
            //         static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "Window create transition controller failed"));
            // }
            // WLOGFI("[NAPI]Window [%{public}u, %{public}s] create transition controller end, objValue = %{public}p",
            //     weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), objValue);
        };

    // NativeValue* lastParam = (info.argc == 0) ? nullptr :
    //     (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    // AsyncTask::Schedule(
    //     engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDestroy(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [this, weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Destroy();
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] destroy end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
            if (ret != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window destroy failed"));
                return;
            }
            windowToken_ = nullptr; // ensure window dtor when finalizer invalid
            task.Resolve(engine, engine.CreateUndefined());
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnDestroy",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnHide(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Hide();
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window hide failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] hide end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnHide",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnMoveTo(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 2 || info.argc > 3) { // 2:minimum param num, 3: maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t x = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], x)) {
        WLOGFE("[NAPI]Failed to convert parameter to x");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    int32_t y = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[1], y)) {
        WLOGFE("[NAPI]Failed to convert parameter to y");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode, x, y](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->MoveTo(x, y);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window move failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] move end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    // 2: params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnMoveTo",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnResize(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 2 || info.argc > 3) { // 2: minimum param num, 3: maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    uint32_t width = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], width)) {
        WLOGFE("[NAPI]Failed to convert parameter to width");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    uint32_t height = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[1], height)) {
        WLOGFE("[NAPI]Failed to convert parameter to height");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode, width, height](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Resize(width, height);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window resize failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] resize end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    // 2: params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnResize",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowType(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2 is max num of argc
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    WindowType winType = WindowType::SYSTEM_WINDOW_BASE;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeType == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to windowType");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else if (static_cast<uint32_t>(*nativeType) >= static_cast<uint32_t>(WindowType::SYSTEM_WINDOW_BASE)) {
            winType = static_cast<WindowType>(static_cast<uint32_t>(*nativeType)); // adapt to the old version
        } else {
            if (JS_TO_NATIVE_WINDOW_TYPE_MAP.count(
                static_cast<ApiWindowType>(static_cast<uint32_t>(*nativeType))) != 0) {
                winType = JS_TO_NATIVE_WINDOW_TYPE_MAP.at(
                    static_cast<ApiWindowType>(static_cast<uint32_t>(*nativeType)));
            } else {
                WLOGFE("[NAPI]Do not surppot this type: %{public}u", static_cast<uint32_t>(*nativeType));
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, winType, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->SetWindowType(winType);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set type failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set type end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowType",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2 is max num of argc
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    WindowMode winMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeMode == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to windowMode");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            if (static_cast<uint32_t>(*nativeMode) >= static_cast<uint32_t>(WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
                winMode = static_cast<WindowMode>(static_cast<uint32_t>(*nativeMode));
            } else if (static_cast<uint32_t>(*nativeMode) >= static_cast<uint32_t>(ApiWindowMode::UNDEFINED) &&
                static_cast<uint32_t>(*nativeMode) <= static_cast<uint32_t>(ApiWindowMode::MODE_END)) {
                winMode = JS_TO_NATIVE_WINDOW_MODE_MAP.at(
                    static_cast<ApiWindowMode>(static_cast<uint32_t>(*nativeMode)));
            } else {
                WLOGFE("[NAPI]Do not surppot this mode: %{public}u", static_cast<uint32_t>(*nativeMode));
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, winMode, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->SetWindowMode(winMode);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(ret), "Window set mode failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set type end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc == 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowMode",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetProperties(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            auto objValue = CreateJsWindowPropertiesObject(engine, weakWindow);
            if (objValue != nullptr) {
                task.Resolve(engine, objValue);
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "Window get properties failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] get properties end, objValue = %{public}p",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), objValue);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnGetProperties",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return engine.CreateUndefined();
    }
    if (info.argc != 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFI("[NAPI]Callback(info->argv[1]) is not callable");
        return engine.CreateUndefined();
    }
    registerManager_->RegisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, engine, value);
    WLOGFI("[NAPI]Register end, window [%{public}u, %{public}s], type = %{public}s, callback = %{public}p",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), cbType.c_str(), value);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnUnregisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr || info.argc < 1 || info.argc > 2) { // 2: maximum params nums
        WLOGFE("[NAPI]Window is nullptr or argc is invalid: %{public}zu", info.argc);
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }

    NativeValue* value = nullptr;
    if (info.argc == 2) { // 2: maximum params nums
        value = info.argv[1];
        if (!value->IsCallable()) {
            WLOGFI("[NAPI]Callback(info->argv[1]) is not callable");
            return engine.CreateUndefined();
        }
    }
    registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, value);

    WLOGFI("[NAPI]Unregister end, window [%{public}u, %{public}s], type = %{public}s, callback = %{public}p",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), cbType.c_str(), value);
    return engine.CreateUndefined();
}

static void LoadContentTask(std::weak_ptr<NativeReference> contentStorage, std::string contextUrl,
    sptr<Window> weakWindow, NativeEngine& engine, AsyncTask& task)
{
    NativeValue* nativeStorage = (contentStorage.lock() == nullptr) ? nullptr : contentStorage.lock()->Get();
    AppExecFwk::Ability* ability = nullptr;
    GetAPI7Ability(engine, ability);
    WMError ret = weakWindow->SetUIContent(contextUrl, &engine, nativeStorage, false, ability);
    if (ret == WMError::WM_OK) {
        task.Resolve(engine, engine.CreateUndefined());
    } else {
        task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
    return;
}

NativeValue* JsWindow::OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 3) { // 3 maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to context url");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* storage = nullptr;
    NativeValue* callBack = nullptr;
    if (info.argc == 2) { // 2: num of params
        NativeValue* value = info.argv[1];
        if (value->TypeOf() == NATIVE_FUNCTION) {
            callBack = info.argv[1];
        } else {
            storage = info.argv[1];
        }
    } else if (info.argc == 3) { // 3: num of params
        storage = info.argv[1];
        // 2: index of callback
        callBack = (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    }
    std::weak_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, contentStorage, contextUrl, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]Window is nullptr or get invalid param");
                return;
            }
            LoadContentTask(contentStorage, contextUrl, weakWindow, engine, task);
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnLoadContent",
        engine, CreateAsyncTaskWithLastParam(engine, callBack, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetFullScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isFullScreen = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isFullScreen");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isFullScreen = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isFullScreen, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetFullScreen(isFullScreen);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window SetFullScreen failed."));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetFullScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetLayoutFullScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isLayoutFullScreen = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isLayoutFullScreen");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isLayoutFullScreen = static_cast<bool>(*nativeVal);
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isLayoutFullScreen, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetLayoutFullScreen(isLayoutFullScreen);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "Window OnSetLayoutFullScreen failed."));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set layout full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetLayoutFullScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetSystemBarEnable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 2 || windowToken_ == nullptr) { // 2: maximum params num
        WLOGFE("[NAPI]Window is nullptr or argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (errCode == WMError::WM_OK && !GetSystemBarStatus(systemBarProperties, engine, info, windowToken_)) {
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, systemBarProperties, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR));
            ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "JsWindow::OnSetSystemBarEnable failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set system bar enable end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = nullptr;
    if (info.argc > 0 && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    } else if (info.argc > 1 && info.argv[1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetSystemBarEnable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetSystemBarProperties(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (errCode == WMError::WM_OK) {
        NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to SystemBarProperties");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            if (!SetSystemBarPropertiesFromJs(engine, nativeObj, systemBarProperties, windowToken_)) {
                WLOGFE("[NAPI]Failed to GetSystemBarProperties From Js Object");
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, systemBarProperties, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
            }
            WMError ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR));
            ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnSetSystemBarProperties failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set system bar properties end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetSystemBarProperties",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetAvoidArea(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    if (errCode == WMError::WM_OK) {
        // Parse info->argv[0] as AvoidAreaType
        NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeMode == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to AvoidAreaType");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            avoidAreaType = static_cast<AvoidAreaType>(static_cast<uint32_t>(*nativeMode));
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode, avoidAreaType](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            // getAvoidRect by avoidAreaType
            AvoidArea avoidArea;
            WMError ret = weakWindow->GetAvoidAreaByType(avoidAreaType, avoidArea);
            if (ret != WMError::WM_OK) {
                avoidArea = { g_emptyRect, g_emptyRect, g_emptyRect, g_emptyRect }; // left, top, right, bottom
            }
            // native avoidArea -> js avoidArea
            NativeValue* avoidAreaObj = ChangeAvoidAreaToJsValue(engine, avoidArea);
            if (avoidAreaObj != nullptr) {
                task.Resolve(engine, avoidAreaObj);
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnGetAvoidArea failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] get avoid area end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnGetAvoidArea",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnIsShowing(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            bool state = weakWindow->GetShowState();
            task.Resolve(engine, CreateJsValue(engine, state));
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] get show state end, state = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), state);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnIsShowing",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnIsSupportWideGamut(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            bool flag = weakWindow->IsSupportWideGamut();
            task.Resolve(engine, CreateJsValue(engine, flag));
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] OnIsSupportWideGamut end, ret = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), flag);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnIsSupportWideGamut",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetBackgroundColor(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string color;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], color)) {
        WLOGFE("[NAPI]Failed to convert parameter to background color");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, color, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetBackgroundColor(color);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set background color failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set background color end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetBackgroundColor",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetBrightness(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    float brightness = UNDEFINED_BRIGHTNESS;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to brightness");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            brightness = static_cast<double>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, brightness, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetBrightness(brightness);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set brightness failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set brightness end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetBrightness",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetDimBehind(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [](NativeEngine& engine, AsyncTask& task, int32_t status) {
            task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetDimBehind",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetFocusable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool focusable = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to focusable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            focusable = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, focusable, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetFocusable(focusable);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set focusable failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set focusable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetFocusable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetKeepScreenOn(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool keepScreenOn = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to keepScreenOn");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            keepScreenOn = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, keepScreenOn, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetKeepScreenOn(keepScreenOn);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set keep screen on failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set keep screen on end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetKeepScreenOn",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetOutsideTouchable(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [](NativeEngine& engine, AsyncTask& task, int32_t status) {
            task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetOutsideTouchable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetPrivacyMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isPrivacyMode = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isPrivacyMode");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isPrivacyMode = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isPrivacyMode, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params"));
                return;
            }
            weakWindow->SetPrivacyMode(isPrivacyMode);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set privacy mode end, mode = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), isPrivacyMode);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetPrivacyMode",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetTouchable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool touchable = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to touchable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            touchable = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, touchable, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetTouchable(touchable);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set touchable failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set touchable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetTouchable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetTransparent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isTransparent = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isTransparent");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isTransparent = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isTransparent, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetTransparent(isTransparent);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set transparent failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set transparent end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetTransparent",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetCallingWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    uint32_t callingWindow = INVALID_WINDOW_ID;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to touchable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            callingWindow = static_cast<uint32_t>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, callingWindow, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetCallingWindow(callingWindow);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set calling window failed"));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set calling window end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetCallingWindow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDisableWindowDecor(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr) {
        return engine.CreateUndefined();
    }
    windowToken_->DisableAppWindowDecor();
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] disable app window decor end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetColorSpace(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    ColorSpace colorSpace = ColorSpace::COLOR_SPACE_DEFAULT;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeType == nullptr) {
            errCode = WMError::WM_ERROR_INVALID_PARAM;
            WLOGFE("[NAPI]Failed to convert parameter to ColorSpace");
        } else {
            colorSpace = static_cast<ColorSpace>(static_cast<uint32_t>(*nativeType));
            if (colorSpace > ColorSpace::COLOR_SPACE_WIDE_GAMUT) {
                WLOGFE("[NAPI]ColorSpace %{public}u invalid!", static_cast<uint32_t>(colorSpace));
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, colorSpace, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "OnSetColorSpace failed"));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            weakWindow->SetColorSpace(colorSpace);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] OnSetColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetColorSpace",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetColorSpace(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            ColorSpace colorSpace = weakWindow->GetColorSpace();
            task.Resolve(engine, CreateJsValue(engine, static_cast<uint32_t>(colorSpace)));
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] OnGetColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnGetColorSpace",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDump(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]dump window start");
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        return nullptr;
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]window is nullptr or get invalid param");
        return nullptr;
    }
    std::vector<std::string> params;
    if (!ConvertNativeValueToVector(engine, info.argv[0], params)) {
        WLOGFE("[NAPI]ConvertNativeValueToVector fail");
        return nullptr;
    }
    std::vector<std::string> dumpInfo;
    windowToken_->DumpInfo(params, dumpInfo);
    NativeValue* dumpInfoValue = CreateNativeArray(engine, dumpInfo);
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] dump end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return dumpInfoValue;
}

NativeValue* JsWindow::OnSetForbidSplitMove(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isForbidSplitMove = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isForbidSplitMove");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isForbidSplitMove = static_cast<bool>(*nativeVal);
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isForbidSplitMove, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret;
            if (isForbidSplitMove) {
                ret = weakWindow->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
            } else {
                ret = weakWindow->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
            }
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "Window OnSetForbidSplitMove failed."));
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] set forbid split move end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnDump",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::shared_ptr<NativeReference> FindJsWindowObject(std::string windowName)
{
    WLOGFI("[NAPI]Try to find window %{public}s in g_jsWindowMap", windowName.c_str());
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_jsWindowMap.find(windowName) == g_jsWindowMap.end()) {
        WLOGFI("[NAPI]Can not find window %{public}s in g_jsWindowMap", windowName.c_str());
        return nullptr;
    }
    return g_jsWindowMap[windowName];
}

NativeValue* CreateJsWindowObject(NativeEngine& engine, sptr<Window>& window)
{
    WLOGFI("[NAPI]CreateJsWindowObject");
    std::string windowName = window->GetWindowName();
    // avoid repeatedly create js window when getWindow
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
    if (jsWindowObj != nullptr && jsWindowObj->Get() != nullptr) {
        WLOGFI("[NAPI]FindJsWindowObject %{public}s", windowName.c_str());
        return jsWindowObj->Get();
    }
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    WLOGFI("[NAPI]CreateJsWindow %{public}s", windowName.c_str());
    std::unique_ptr<JsWindow> jsWindow = std::make_unique<JsWindow>(window);
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
    BindNativeFunction(engine, *object, "off", JsWindow::UnregisterWindowCallback);
    BindNativeFunction(engine, *object, "loadContent", JsWindow::LoadContent);
    BindNativeFunction(engine, *object, "setFullScreen", JsWindow::SetFullScreen);
    BindNativeFunction(engine, *object, "setLayoutFullScreen", JsWindow::SetLayoutFullScreen);
    BindNativeFunction(engine, *object, "setSystemBarEnable", JsWindow::SetSystemBarEnable);
    BindNativeFunction(engine, *object, "setSystemBarProperties", JsWindow::SetSystemBarProperties);
    BindNativeFunction(engine, *object, "getAvoidArea", JsWindow::GetAvoidArea);
    BindNativeFunction(engine, *object, "isShowing", JsWindow::IsShowing);
    BindNativeFunction(engine, *object, "isSupportWideGamut", JsWindow::IsSupportWideGamut);
    BindNativeFunction(engine, *object, "setColorSpace", JsWindow::SetColorSpace);
    BindNativeFunction(engine, *object, "getColorSpace", JsWindow::GetColorSpace);
    BindNativeFunction(engine, *object, "setBackgroundColor", JsWindow::SetBackgroundColor);
    BindNativeFunction(engine, *object, "setBrightness", JsWindow::SetBrightness);
    BindNativeFunction(engine, *object, "setDimBehind", JsWindow::SetDimBehind);
    BindNativeFunction(engine, *object, "setFocusable", JsWindow::SetFocusable);
    BindNativeFunction(engine, *object, "setKeepScreenOn", JsWindow::SetKeepScreenOn);
    BindNativeFunction(engine, *object, "setOutsideTouchable", JsWindow::SetOutsideTouchable);
    BindNativeFunction(engine, *object, "setPrivacyMode", JsWindow::SetPrivacyMode);
    BindNativeFunction(engine, *object, "setTouchable", JsWindow::SetTouchable);
    BindNativeFunction(engine, *object, "setTransparent", JsWindow::SetTransparent);
    BindNativeFunction(engine, *object, "setCallingWindow", JsWindow::SetCallingWindow);
    BindNativeFunction(engine, *object, "disableWindowDecor", JsWindow::DisableWindowDecor);
    BindNativeFunction(engine, *object, "dump", JsWindow::Dump);
    BindNativeFunction(engine, *object, "setForbidSplitMove", JsWindow::SetForbidSplitMove);
    BindNativeFunction(engine, *object, "createTransitionController", JsWindow::CreateTransitionController);

    std::shared_ptr<NativeReference> jsWindowRef;
    jsWindowRef.reset(engine.CreateReference(objValue, 1));
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_jsWindowMap[windowName] = jsWindowRef;
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
