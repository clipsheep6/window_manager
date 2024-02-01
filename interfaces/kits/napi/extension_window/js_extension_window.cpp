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

#include "js_extension_window.h"

#include "js_extension_window_utils.h"
#include "js_runtime_utils.h"
#include "js_window_utils.h"
#include "js_window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "extension_window.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsExtensionWindow"};
constexpr Rect g_emptyRect = {0, 0, 0, 0};
} // namespace

JsExtensionWindow::JsExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow)
    : extensionWindow_(extensionWindow),
    extensionRegisterManager_(std::make_unique<JsExtensionWindowRegisterManager>()) {
}

JsExtensionWindow::JsExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
    sptr<AAFwk::SessionInfo> sessionInfo)
    : extensionWindow_(extensionWindow), sessionInfo_(sessionInfo),
    extensionRegisterManager_(std::make_unique<JsExtensionWindowRegisterManager>()) {
}

JsExtensionWindow::~JsExtensionWindow() {}

napi_value JsExtensionWindow::CreateJsExtensionWindow(napi_env env, sptr<Rosen::Window> window)
{
    WLOGI("JsExtensionWindow CreateJsExtensionWindow");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    if (env == nullptr || window == nullptr) {
        WLOGFE("JsExtensionWindow env or window is nullptr");
        return nullptr;
    }

    std::shared_ptr<ExtensionWindow> extensionWindow = std::make_shared<ExtensionWindowImpl>(window);
    std::unique_ptr<JsExtensionWindow> jsExtensionWindow = std::make_unique<JsExtensionWindow>(extensionWindow);
    napi_wrap(env, objValue, jsExtensionWindow.release(), JsExtensionWindow::Finalizer, nullptr, nullptr);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_GETTER("properties", JsExtensionWindow::GetProperties)
    };
    NAPI_CALL(env, napi_define_properties(env, objValue, sizeof(desc) / sizeof(desc[0]), desc));

    const char *moduleName = "JsExtensionWindow";
    BindNativeFunction(env, objValue, "getWindowAvoidArea", moduleName, JsExtensionWindow::GetWindowAvoidArea);
    BindNativeFunction(env, objValue, "on", moduleName, JsExtensionWindow::RegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "off", moduleName, JsExtensionWindow::UnRegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "hideNonSecureWindows", moduleName, JsExtensionWindow::HideNonSecureWindows);

    return objValue;
}

napi_value JsExtensionWindow::CreateJsWindowStage(napi_env env, sptr<Rosen::Window> window,
    sptr<AAFwk::SessionInfo> sessionInfo)
{
    WLOGI("JsExtensionWindow CreateJsWindowStage");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    if (env == nullptr || window == nullptr) {
        WLOGFE("JsExtensionWindow env or window is nullptr");
        return nullptr;
    }

    std::shared_ptr<ExtensionWindow> extensionWindow = std::make_shared<ExtensionWindowImpl>(window);
    std::unique_ptr<JsExtensionWindow> jsExtensionWindow = std::make_unique<JsExtensionWindow>(extensionWindow,
    sessionInfo);
    napi_wrap(env, objValue, jsExtensionWindow.release(), JsExtensionWindow::Finalizer, nullptr, nullptr);

    const char *moduleName = "JsExtensionWindow";
    BindNativeFunction(env, objValue, "on", moduleName, JsExtensionWindow::RegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "off", moduleName, JsExtensionWindow::UnRegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "loadContent", moduleName, JsExtensionWindow::LoadContent);
    BindNativeFunction(env, objValue, "loadContentByName", moduleName, JsExtensionWindow::LoadContentByName);

    return objValue;
}

void JsExtensionWindow::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("Finalizer is called");
    std::unique_ptr<JsExtensionWindow>(static_cast<JsExtensionWindow*>(data));
}

napi_value JsExtensionWindow::GetWindowAvoidArea(napi_env env, napi_callback_info info)
{
    WLOGI("GetWindowAvoidArea is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowAvoidArea(env, info) : nullptr;
}

napi_value JsExtensionWindow::RegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    WLOGI("RegisterExtensionWindowCallback is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnRegisterExtensionWindowCallback(env, info) : nullptr;
}

napi_value JsExtensionWindow::UnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    WLOGI("UnRegisterExtensionWindowCallback is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnUnRegisterExtensionWindowCallback(env, info) : nullptr;
}

napi_value JsExtensionWindow::GetMainWindow(napi_env env, napi_callback_info info)
{
    WLOGFD("GetMainWindow is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnGetMainWindow(env, info) : nullptr;
}

napi_value JsExtensionWindow::LoadContent(napi_env env, napi_callback_info info)
{
    WLOGI("LoadContent is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, false) : nullptr;
}

napi_value JsExtensionWindow::LoadContentByName(napi_env env, napi_callback_info info)
{
    WLOGI("LoadContentByName is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, true) : nullptr;
}

napi_value JsExtensionWindow::OnGetMainWindow(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [extwin = extensionWindow_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (extwin == nullptr) {
                WLOGFE("ExtensionWindow_ is nullptr!");
                return;
            }
            sptr<Window> windowImpl = extwin->GetWindow();
            if (windowImpl != nullptr) {
                task.Resolve(env, OHOS::Rosen::CreateJsWindowObject(env, windowImpl));
                WLOGI("Get extensionWindow [%{public}u, %{public}s]",
                    windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str());
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Get extensionWindow failed."));
            }
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value callback = (argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ?
        argv[0] : nullptr;
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindow::OnGetMainWindow",
        env, CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

static void LoadContentTask(std::shared_ptr<NativeReference> contentStorage, std::string contextUrl,
    const std::shared_ptr<Rosen::ExtensionWindow> win, napi_env env, NapiAsyncTask& task,
    sptr<IRemoteObject> parentToken, bool isLoadedByName)
{
    napi_value nativeStorage = (contentStorage == nullptr) ? nullptr : contentStorage->GetNapiValue();
    sptr<Window> windowImpl = win->GetWindow();
    WMError ret;
    if (isLoadedByName) {
        ret = windowImpl->SetUIContentByName(contextUrl, env, nativeStorage);
    } else {
        ret = win->NapiSetUIContent(contextUrl, env, nativeStorage, false, parentToken);
    }
    if (ret == WMError::WM_OK) {
        task.Resolve(env, NapiGetUndefined(env));
    } else {
        task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGI("[NAPI]Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), ret);
    return;
}

napi_value JsExtensionWindow::OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName)
{
    WLOGI("OnLoadContent is called");
    WmErrorCode errCode = WmErrorCode::WM_OK;
    std::string contextUrl;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (!ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGI("[NAPI]Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    napi_value storage = nullptr;
    napi_value callBack = nullptr;
    napi_value value1 = argv[1];
    napi_value value2 = argv[2];
    if (GetType(env, value1) == napi_function) {
        callBack = value1;
    } else if (GetType(env, value1) == napi_object) {
        storage = value1;
    }
    if (GetType(env, value2) == napi_function) {
        callBack = value2;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGI("[NAPI]Window is null or get invalid param");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (storage != nullptr) {
        napi_ref result = nullptr;
        napi_create_reference(env, storage, 1, &result);
        contentStorage = std::shared_ptr<NativeReference>(reinterpret_cast<NativeReference*>(result));
    }
    
    sptr<IRemoteObject> parentToken = sessionInfo_->parentToken;
    NapiAsyncTask::CompleteCallback complete =
        [extwin = extensionWindow_, contentStorage, contextUrl, parentToken, isLoadedByName](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            LoadContentTask(contentStorage, contextUrl, extwin, env, task, parentToken, isLoadedByName);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindow::OnLoadContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsExtensionWindow::HideNonSecureWindows(napi_env env, napi_callback_info info)
{
    WLOGI("HideNonSecureWindows is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnHideNonSecureWindows(env, info) : nullptr;
}

napi_value JsExtensionWindow::OnGetWindowAvoidArea(napi_env env, napi_callback_info info)
{
    WLOGI("OnGetWindowAvoidArea is called");

    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    napi_value nativeMode = argv[0];
    if (nativeMode == nullptr) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        uint32_t resultValue = 0;
        napi_get_value_uint32(env, nativeMode, &resultValue);
        avoidAreaType = static_cast<AvoidAreaType>(resultValue);
        errCode = ((avoidAreaType > AvoidAreaType::TYPE_NAVIGATION_INDICATOR) ||
                   (avoidAreaType < AvoidAreaType::TYPE_SYSTEM)) ?
                  WmErrorCode::WM_ERROR_INVALID_PARAM : WmErrorCode::WM_OK;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    if (extensionWindow_ == nullptr) {
        WLOGFE("extensionWindow_ is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
        return CreateJsValue(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY));
    }
    // getAvoidRect by avoidAreaType
    AvoidArea avoidArea;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(extensionWindow_->GetAvoidAreaByType(avoidAreaType, avoidArea));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("OnGetAvoidAreaByType failed");
        avoidArea.topRect_ = g_emptyRect;
        avoidArea.leftRect_ = g_emptyRect;
        avoidArea.rightRect_ = g_emptyRect;
        avoidArea.bottomRect_ = g_emptyRect;
    }
    napi_value avoidAreaObj = ConvertAvoidAreaToJsValue(env, avoidArea, avoidAreaType);
    if (avoidAreaObj != nullptr) {
        WLOGI("avoidAreaObj is finish");
        return avoidAreaObj;
    } else {
        WLOGFE("avoidAreaObj is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

napi_value JsExtensionWindow::OnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("WindowImpl is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_value value = argv[1];
    if (!NapiIsCallable(env, value)) {
        WLOGFE("Callback(info->argv[1]) is not callable");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = extensionRegisterManager_->RegisterListener(windowImpl, cbType, CaseType::CASE_WINDOW,
        env, value);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Callback(info->argv[1]) is not callable");
        return NapiThrowError(env, ret);
    }
    WLOGI("Register end, window [%{public}u, %{public}s], type = %{public}s",
          windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsExtensionWindow::OnUnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("windowImpl is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    napi_value value = nullptr;
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (argc == 1) {
        ret = extensionRegisterManager_->UnregisterListener(windowImpl, cbType, CaseType::CASE_WINDOW, env, value);
    } else {
        value = argv[1];
        if (value == nullptr || !NapiIsCallable(env, value)) {
            ret = extensionRegisterManager_->UnregisterListener(windowImpl, cbType, CaseType::CASE_WINDOW,
                env, nullptr);
        } else {
            ret = extensionRegisterManager_->UnregisterListener(windowImpl, cbType, CaseType::CASE_WINDOW, env, value);
        }
    }

    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    WLOGI("UnRegister end, window [%{public}u, %{public}s], type = %{public}s",
          windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsExtensionWindow::OnHideNonSecureWindows(napi_env env, napi_callback_info info)
{
    if (extensionWindow_ == nullptr) {
        WLOGFE("extensionWindow_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("windowImpl is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    bool shouldHide = false;
    if (!ConvertFromJsValue(env, argv[0], shouldHide)) {
        WLOGFE("Failed to convert parameter to bool");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    WmErrorCode ret = WmErrorCode::WM_OK;
    ret = WM_JS_TO_ERROR_CODE_MAP.at(extensionWindow_->HideNonSecureWindows(shouldHide));
    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    WLOGI("OnHideNonSecureWindows end, window [%{public}u, %{public}s], shouldHide:%{public}u",
          windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), shouldHide);
    return NapiGetUndefined(env);
}

napi_value JsExtensionWindow::GetProperties(napi_env env, napi_callback_info info)
{
    WLOGI("GetProperties is called");
    napi_value jsThis;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsThis, nullptr));

    JsExtensionWindow* jsExtensionWindow = nullptr;
    NAPI_CALL(env, napi_unwrap(env, jsThis, reinterpret_cast<void**>(&jsExtensionWindow)));
    if (!jsExtensionWindow || !jsExtensionWindow->extensionWindow_) {
        WLOGFE("window is nullptr");
        return nullptr;
    }
    sptr<Rosen::Window> window = jsExtensionWindow->extensionWindow_->GetWindow();
    return CreateJsExtensionWindowPropertiesObject(env, window);
}
}  // namespace Rosen
}  // namespace OHOS
