/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "js_pip_window_listener.h"

#include "js_pip_controller.h"
#include "window_manager_hilog.h"
#include "picture_in_picture_manager.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

static napi_value CallJsFunction(napi_env env, napi_value method, napi_value const * argv, size_t argc)
{
    if (env == nullptr || method == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "env nullptr or method is nullptr");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_value callResult = nullptr;
    napi_get_undefined(env, &result);
    napi_get_undefined(env, &callResult);
    napi_call_function(env, result, method, argc, argv, &callResult);
    TLOGD(WmsLogTag::WMS_PIP, "called.");
    return callResult;
}

JsPiPWindowListener::~JsPiPWindowListener()
{
    TLOGI(WmsLogTag::WMS_PIP, "~JsWindowListener");
}

std::shared_ptr<NativeReference> JsPiPWindowListener::GetCallbackRef() const
{
    return jsCallBack_;
}

void JsPiPWindowListener::OnPreparePictureInPictureStart()
{
    OnPipListenerCallback(PiPState::ABOUT_TO_START, 0);
}

void JsPiPWindowListener::OnPictureInPictureStart()
{
    OnPipListenerCallback(PiPState::STARTED, 0);
}

void JsPiPWindowListener::OnPreparePictureInPictureStop()
{
    OnPipListenerCallback(PiPState::ABOUT_TO_STOP, 0);
}

void JsPiPWindowListener::OnPictureInPictureStop()
{
    OnPipListenerCallback(PiPState::STOPPED, 0);
}

void JsPiPWindowListener::OnRestoreUserInterface()
{
    OnPipListenerCallback(PiPState::ABOUT_TO_RESTORE, 0);
}

void JsPiPWindowListener::OnPictureInPictureOperationError(int32_t errorCode)
{
    OnPipListenerCallback(PiPState::ERROR, errorCode);
}

void JsPiPWindowListener::OnPipListenerCallback(PiPState state, int32_t errorCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "state: %{public}d", static_cast<int32_t>(state));
    auto napiTask = [jsCallback = jsCallBack_, state, errorCode, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, static_cast<uint32_t>(state)), CreateJsValue(env, errorCode)};
        CallJsFunction(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate, "OnPipListenerCallback");
        if (ret != napi_status::napi_ok) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to SendEvent");
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "env is nullptr");
    }
}

void JsPiPWindowListener::OnActionEvent(const std::string& actionEvent, int32_t statusCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "called, actionEvent: %{public}s", actionEvent.c_str());
    auto napiTask = [jsCallback = jsCallBack_, actionEvent, statusCode, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, actionEvent), CreateJsValue(env, statusCode)};
        CallJsFunction(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate, "OnActionEvent");
        if (ret != napi_status::napi_ok) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to SendEvent");
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "env is nullptr");
    }
}

void JsPiPWindowListener::OnControlEvent(PiPControlType controlType, PiPControlStatus statusCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, statusCode:%{public}d", controlType, statusCode);
    auto napiTask = [jsCallback = jsCallBack_, controlType, statusCode, env = env_]() {
        napi_value propertyValue = nullptr;
        napi_create_object(env, &propertyValue);
        if (propertyValue == nullptr) {
            TLOGE(WmsLogTag::WMS_PIP, "propertyValue is nullptr");
            return;
        }
        napi_set_named_property(env, propertyValue, "controlType", CreateJsValue(env, controlType));
        napi_set_named_property(env, propertyValue, "status", CreateJsValue(env, statusCode));
        napi_value argv[] = {propertyValue};
        CallJsFunction(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate, "OnControlEvent");
        if (ret != napi_status::napi_ok) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to SendEvent");
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "env is nullptr");
    }
}

void JsPiPWindowListener::OnPipSizeChange(const PiPWindowSize& size)
{
    TLOGI(WmsLogTag::WMS_PIP, "called, size info: weight %{public}u height %{public}u scale %{public}f",
          size.height, size.width, size.scale);
    auto napiTask = [jsCallback = jsCallBack_, size, env = env_]() {
        napi_value value = nullptr;
        napi_create_object(env, &value);
        if (value == nullptr) {
            TLOGNE(WmsLogTag::WMS_PIP, "Failed to convert rect to jsObject");
            return;
        }
        napi_set_named_property(env, value, "width", CreateJsValue(env, static_cast<uint32_t>(size.width)));
        napi_set_named_property(env, value, "height", CreateJsValue(env, static_cast<uint32_t>(size.height)));
        napi_set_named_property(env, value, "scale", CreateJsValue(env, static_cast<double>(size.scale)));
        napi_value argv[] = {value};
        CallJsFunction(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate, "OnPipSizeChange");
        if (ret != napi_status::napi_ok) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to SendEvent");
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "env is nullptr");
    }
}

void JsPiPWindowListener::OnPipTypeNodeChange(const napi_ref nodeRef)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    auto napiTask = [jsCallback = jsCallBack_, nodeRef, env = env_]() {
        napi_value typeNode = nullptr;
        napi_get_reference_value(env, nodeRef, &typeNode);
        napi_value argv[] = {typeNode};
        CallJsFunction(env, jsCallback->GetNapiValue(), argv, 1);
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate, "OnPipTypeNodeChange");
        if (ret != napi_status::napi_ok) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to SendEvent");
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "env is nullptr");
    }
}

} // namespace Rosen
} // namespace OHOS