/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include <refbase.h>
#include "js_pip_utils.h"
#include "js_runtime_utils.h"
#include "picture_in_picture_controller.h"
#include "picture_in_picture_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "picture_in_picture_interface.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

JsPiPWindowListener::~JsPiPWindowListener()
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]~JsWindowListener");
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

napi_value JsPiPWindowListener::CallJsMethod(napi_value methodName, napi_value const * argv, size_t argc)
{
    if (env_ == nullptr || jsCallBack_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]env_ nullptr or jsCallBack_ is nullptr");
        return nullptr;
    }
    napi_value method = jsCallBack_->GetNapiValue();
    if (method == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to get method callback from object");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_value callResult = nullptr;
    napi_get_undefined(env_, &result);
    napi_get_undefined(env_, &callResult);
    napi_call_function(env_, result, method, argc, argv, &callResult);
    TLOGI(WmsLogTag::WMS_PIP, "called.");
    return callResult;
}

void JsPiPWindowListener::OnPipListenerCallback(PiPState state, int32_t errorCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "state: %{public}d", static_cast<int32_t>(state));
    auto jsCallback = jsCallBack_;
    sptr<JsPiPWindowListener> pipWindowListener = this;
    auto napiTask = [this, jsCallback = jsCallBack_, pipWindowListener, state, errorCode, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, static_cast<uint32_t>(state)), CreateJsValue(env, errorCode)};
        CallJsMethod(jsCallback->GetNapiValue(), argv, ArraySize(argv));
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
        if (ret != napi_status::napi_ok) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to SendEvent");
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "env is nullptr");
    }
}

void JsPiPWindowListener::OnActionEvent(const std::string& actionEvent, int32_t statusCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "OnActionEvent is called, actionEvent: %{public}s", actionEvent.c_str());
    auto jsCallback = jsCallBack_;
    sptr<JsPiPWindowListener> pipWindowListener = this;
    auto napiTask = [this, jsCallback = jsCallBack_, pipWindowListener, actionEvent, statusCode, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, actionEvent), CreateJsValue(env, statusCode)};
        CallJsMethod(jsCallback->GetNapiValue(), argv, ArraySize(argv));
    };
    if (env_ != nullptr) {
        napi_status ret = napi_send_event(env_, napiTask, napi_eprio_immediate);
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
    napi_value result = nullptr;
    std::unique_ptr<NapiAsyncTask> napiAsyncTask = CreateEmptyAsyncTask(env_, nullptr, &result);
    auto asyncTask = [jsCallback = jsCallBack_, controlType, statusCode, env = env_,
        task = napiAsyncTask.get(), this]() {
        napi_value propertyValue = nullptr;
        napi_create_object(env, &propertyValue);
        if (propertyValue == nullptr) {
            TLOGI(WmsLogTag::WMS_PIP, "propertyValue is nullptr");
            return;
        }
        napi_set_named_property(env, propertyValue, "controlType", CreateJsValue(env, controlType));
        napi_set_named_property(env, propertyValue, "status", CreateJsValue(env, statusCode));
        napi_value argv[] = {propertyValue};
        CallJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        delete task;
    };
    if (napi_send_event(env_, asyncTask, napi_eprio_high) != napi_status::napi_ok) {
        napiAsyncTask->Reject(env_, CreateJsError(env_, 1, "send event failed"));
    } else {
        napiAsyncTask.release();
    }
}
} // namespace Rosen
} // namespace OHOS