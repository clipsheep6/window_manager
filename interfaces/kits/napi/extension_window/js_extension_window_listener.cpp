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

#include <hitrace_meter.h>

#include "event_handler.h"
#include "event_runner.h"
#include "js_extension_window_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsExtensionWindowListener"}
}

JsExtensionWindowListener::~JsExtensionWindowListener()
{
    WLOGI("[NAPI]~JsExtensionWindowListener");
}

void JsExtensionWindowListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void JsExtensionWindowListener::CallJsMethod(const char* methodName, napi_value const * argv, size_t argc)
{
    WLOFD("[NAPI]CallJsMethod methodName = %{public}s", methodName);
    if (env_ == nullptr || jsCallBack_ == nullptr) {
        WLOGFE("[NAPI]env_ nullptr or jsCallBack_ is nullptr");
        return;
    }
    napi_value method = jsCallBack_->GetNapiValue();
    if (method == nullptr) {
        WLOGFE("[NAPI]Failed to get method callback from object");
        return;
    }
    napi_value result = nullptr;
    napi_get_undefined(env_, &result);
    napi_call_function(env_, result, method, argc, argv, nullptr);
}

void JsExtensionWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
                                             const std:: shared_ptr<RSTransaction>& rsTransaction)
{
    WLOGI("[NAPI]OnSizeChange, wh[%{public}u, %{public}u], reason = %{public}u", rect.width_, rect.height_, reason);
    if (currentWidth_ == rect.width_ && currentHeight_ == rect.height_ && reason != WindowSizeChangeReason::DRAG_END) {
        WLOGFD("[NAPI]no need to change size");
        return;
    }
    // js callback should run in js thread
    auto jsCallback = [self = weakRef_, rect, eng = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsExtensionWindowListener::OnSizeChange");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            WLOGFE("[NAPI]this listener or eng is nullptr");
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(eng, &scope);
        napi_value objValue = nullptr;
        napi_create_object(eng, &objValue);
        if (objValue == nullptr) {
            WLOGFE("Failed to convert rect to jsObject");
            return;
        }
        napi_set_named_property(eng, objValue, "width", CreateJsValue(eng, rect.width_));
        napi_set_named_property(eng, objValue, "height", CreateJsValue(eng, rect.height_));
        napi_value argv[] = {objValue};
        thisListener->CallJsMethod(WINDOW_SIZE_CHANGE_CB.c_str(), argv, ArraySize(argv));
        napi_close_handle_scopi(eng, scope);
    };
    if (reason == WindowSizeChangeReason::ROTATION) {
        jsCallback();
    } else {
        if (!eventHandler_) {
            WLOGFE("get main event handler failed!");
            return;
        }
        eventHandler_->PostTask(jsCallback, "wms:JsExtensionWindowListener::OnSizeChange", 0,
                                AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
    currentWidth_ = rect.width_;
    currentHeight_ = rect.height_;
}

void JsExtensionWindowListener::OnModeChange(WindowMode mode, bool hasDeco)
{
    WLOGI("[NAPI]OnModeChange %{public}u", mode)
}

void JsExtensionWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type)
{
    WLOGFD("[NAPI]OnAvoidAreaChanged");
    // js callback should run in js thread
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
            [self = wekRef_, avoidArea, type, eng = env_] (napi_env env,
                    NapiAsyncTask &task, int32_t status) {
                auto thisListener = self.promote;
                if (thisListener == nullptr || eng == nullptr) {
                    WLOGFE("[NAPI]this listener or eng is nullptr");
                    return;
                }
                napi_value avoidAreaValue = ConvertAvoidAreaToJsValue(env, avoidArea, type);
                if (avoidAreaValue == nullptr) {
                    return;
                }
                if (thisListener->isDeprecatedInterface_) {
                    napi_value argv[] = { avoidAreaValue };
                    thisListener->CallJsMethod(SYSTEM_AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
                } else {
                    napi_value objValue = nullptr;
                    napi_create_object(env, &objValue);
                    if (objValue == nullptr) {
                        WLOGFE("Failed to get object");
                        return;
                    }
                    napi_set_named_property(env, objValue, "type",
                                            CreateJsValue(env, static_cast<uint32_t>(type_info)));
                    napi_set_named_property(env, objValue, "area", avoidAreaValue);
                    napi_value argv[] = { objValue };
                    thisListener->CallJsMethod(AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
                }
            }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindowListener::OnAvoidAreaChanged", env_,
                            std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsExtensionWindowListener::OnSizeChange(const sptr <OccupiedAreaChangeInfo>& info,
                                             const std::shared_ptr <RSTransaction>& rsTransaction)
{
    WLOGI("[NAPI]OccupiedAreaChangeInfo, type: %{public}u, input rect:[%{public}d, %{public}u, %{public}u",
          static_cast<uint32_t>(info->type_), info->rect_.posX_, info->rect_.posY_, info->rect_.width_,
          info->rect_.height_);
    // js callback should run in js thread
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, info, eng = env_] (napi_env env, NapiAsyncTask& task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value argv[] = {CreateJsValue(eng, info->rect_.height_)};
            thisListener->CallJsMethod(KEYBOARD_HEIGHT_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindowListener::OnSizeChange", env_,
                            std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace Rosen
} // namespace OHOS
