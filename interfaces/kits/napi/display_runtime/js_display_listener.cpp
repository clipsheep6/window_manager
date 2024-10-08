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

#include "js_display_listener.h"

#include <hitrace_meter.h>

#include "dm_common.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
#include "js_display.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplayListener"};
}

JsDisplayListener::JsDisplayListener(napi_env env) : env_(env), weakRef_(wptr<JsDisplayListener> (this))
{
    WLOGFI("Constructor execution");
    napi_add_env_cleanup_hook(env_, CleanEnv, this);
}

JsDisplayListener::~JsDisplayListener()
{
    WLOGFI("Destructor execution");
    napi_remove_env_cleanup_hook(env_, CleanEnv, this);
    env_ = nullptr;
}

void JsDisplayListener::CleanEnv(void* obj)
{
    JsDisplayListener* thisObj = reinterpret_cast<JsDisplayListener*>(obj);
    if (!thisObj) {
        WLOGE("obj is nullptr");
        return;
    }
    WLOGFI("env_ is invalid, set to nullptr");
    thisObj->env_ = nullptr;
}

void JsDisplayListener::AddCallback(const std::string& type, napi_value jsListenerObject)
{
    WLOGD("JsDisplayListener::AddCallback is called");
    std::unique_ptr<NativeReference> callbackRef;
    if (env_ == nullptr) {
        WLOGFE("env_ nullptr");
        return;
    }
    napi_ref result = nullptr;
    napi_create_reference(env_, jsListenerObject, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_[type].emplace_back(std::move(callbackRef));
    WLOGD("JsDisplayListener::AddCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(jsCallBack_[type].size()));
}

void JsDisplayListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_.clear();
}

void JsDisplayListener::RemoveCallback(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = jsCallBack_.find(type);
    if (it == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::RemoveCallback no callback to remove");
        return;
    }
    auto& listeners = it->second;
    for (auto iter = listeners.begin(); iter != listeners.end();) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, (*iter)->GetNapiValue(), &isEquals);
        if (isEquals) {
            listeners.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGI("JsDisplayListener::RemoveCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(listeners.size()));
}

void JsDisplayListener::CallJsMethod(const std::string& methodName, napi_value const * argv, size_t argc)
{
    if (methodName.empty()) {
        WLOGFE("empty method name str, call method failed");
        return;
    }
    WLOGD("CallJsMethod methodName = %{public}s", methodName.c_str());
    if (env_ == nullptr) {
        WLOGFE("env_ nullptr");
        return;
    }
    for (auto& callback : jsCallBack_[methodName]) {
        napi_value method = callback->GetNapiValue();
        if (method == nullptr) {
            WLOGFE("Failed to get method callback from object");
            continue;
        }
        napi_call_function(env_, NapiGetUndefined(env_), method, argc, argv, nullptr);
    }
}

void JsDisplayListener::OnCreate(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("JsDisplayListener::OnCreate is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (jsCallBack_.empty()) {
        WLOGFE("JsDisplayListener::OnCreate not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_ADD) == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::OnCreate not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, id, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnCreate");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, static_cast<uint32_t>(id))};
        thisListener->CallJsMethod(EVENT_ADD, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnCreate: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnCreate: env is nullptr");
    }
}

void JsDisplayListener::OnDestroy(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("JsDisplayListener::OnDestroy is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (jsCallBack_.empty()) {
        WLOGFE("JsDisplayListener::OnDestroy not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_REMOVE) == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::OnDestroy not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, id, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnDestroy");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, static_cast<uint32_t>(id))};
        thisListener->CallJsMethod(EVENT_REMOVE, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnDestroy: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnDestroy: env is nullptr");
    }
}

void JsDisplayListener::OnChange(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGD("JsDisplayListener::OnChange is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (jsCallBack_.empty()) {
        WLOGFE("JsDisplayListener::OnChange not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_CHANGE) == jsCallBack_.end()) {
        WLOGE("JsDisplayListener::OnChange not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, id, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnChange");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, static_cast<uint32_t>(id))};
        thisListener->CallJsMethod(EVENT_CHANGE, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnChange: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnChange: env is nullptr");
    }
}

void JsDisplayListener::OnPrivateWindow(bool hasPrivate)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("OnPrivateWindow is called, private status: %{public}u", static_cast<uint32_t>(hasPrivate));
    if (jsCallBack_.empty()) {
        WLOGFE("OnPrivateWindow not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_PRIVATE_MODE_CHANGE) == jsCallBack_.end()) {
        WLOGE("OnPrivateWindow not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, hasPrivate, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnPrivateWindow");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, hasPrivate)};
        thisListener->CallJsMethod(EVENT_PRIVATE_MODE_CHANGE, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnPrivateWindow: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnPrivateWindow: env is nullptr");
    }
}

void JsDisplayListener::OnFoldStatusChanged(FoldStatus foldStatus)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("OnFoldStatusChanged is called, foldStatus: %{public}u", static_cast<uint32_t>(foldStatus));
    if (jsCallBack_.empty()) {
        WLOGFE("OnFoldStatusChanged not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_FOLD_STATUS_CHANGED) == jsCallBack_.end()) {
        WLOGE("OnFoldStatusChanged not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, foldStatus, env = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnFoldStatusChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, foldStatus)};
        thisListener->CallJsMethod(EVENT_FOLD_STATUS_CHANGED, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnFoldStatusChanged: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnFoldStatusChanged: env is nullptr");
    }
}

void JsDisplayListener::OnFoldAngleChanged(std::vector<float> foldAngles)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCallBack_.empty()) {
        WLOGFE("OnFoldAngleChanged not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_FOLD_ANGLE_CHANGED) == jsCallBack_.end()) {
        WLOGE("OnFoldAngleChanged not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, foldAngles, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnFoldAngleChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateNativeArray(env, foldAngles)};
        thisListener->CallJsMethod(EVENT_FOLD_ANGLE_CHANGED, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnFoldAngleChanged: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnFoldAngleChanged: env is nullptr");
    }
}

void JsDisplayListener::OnCaptureStatusChanged(bool isCapture)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCallBack_.empty()) {
        WLOGFE("OnCaptureStatusChanged not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_CAPTURE_STATUS_CHANGED) == jsCallBack_.end()) {
        WLOGE("OnCaptureStatusChanged not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, isCapture, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnCaptureStatusChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, isCapture)};
        thisListener->CallJsMethod(EVENT_CAPTURE_STATUS_CHANGED, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnCaptureStatusChanged: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnCaptureStatusChanged: env is nullptr");
    }
}

void JsDisplayListener::OnDisplayModeChanged(FoldDisplayMode displayMode)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("OnDisplayModeChanged is called, displayMode: %{public}u", static_cast<uint32_t>(displayMode));
    if (jsCallBack_.empty()) {
        WLOGFE("OnDisplayModeChanged not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_DISPLAY_MODE_CHANGED) == jsCallBack_.end()) {
        WLOGE("OnDisplayModeChanged not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, displayMode, env = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnDisplayModeChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsValue(env, displayMode)};
        thisListener->CallJsMethod(EVENT_DISPLAY_MODE_CHANGED, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnDisplayModeChanged: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnDisplayModeChanged: env is nullptr");
    }
}

void JsDisplayListener::OnAvailableAreaChanged(DMRect area)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("OnAvailableAreaChanged is called");
    if (jsCallBack_.empty()) {
        WLOGFE("OnAvailableAreaChanged not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_AVAILABLE_AREA_CHANGED) == jsCallBack_.end()) {
        WLOGE("OnAvailableAreaChanged not this event, return");
        return;
    }
    auto napiTask = [self = weakRef_, area, env = env_]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsDisplayListener::OnAvailableAreaChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            WLOGFE("[NAPI]this listener or env is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsRectObject(env, area)};
        thisListener->CallJsMethod(EVENT_AVAILABLE_AREA_CHANGED, argv, ArraySize(argv));
    };

    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            WLOGFE("OnAvailableAreaChanged: Failed to SendEvent.");
        }
    } else {
        WLOGFE("OnAvailableAreaChanged: env is nullptr");
    }
}
} // namespace Rosen
} // namespace OHOS
