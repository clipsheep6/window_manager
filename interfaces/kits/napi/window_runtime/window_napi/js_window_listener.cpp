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
#include "js_window_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowListener"};
}

constexpr uint32_t AVOID_AREA_NUM = 4;

void JsWindowListener::CallJsMethod(const char* methodName, NativeValue* const* argv, size_t argc)
{
    WLOGFI("CallJsMethod methodName = %{public}s", methodName);
    if (engine_ == nullptr || jsCallBack_ == nullptr) {
        WLOGFE("engine_ nullptr or jsCallBack_ is nullptr");
        return;
    }
    NativeValue* method = jsCallBack_->Get();
    if (method == nullptr) {
        WLOGFE("Failed to get method callback from object");
        return;
    }
    engine_->CallFunction(engine_->CreateUndefined(), method, argv, argc);
}

void JsWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason)
{
    WLOGFI("JsWindowListener::OnSizeChange is called");
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [self = weakRef_, rect, eng = engine_] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or engine is nullptr");
                return;
            }
            NativeValue* sizeValue = eng->CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(sizeValue);
            if (object == nullptr) {
                WLOGFE("Failed to convert rect to jsObject");
                return;
            }
            object->SetProperty("width", CreateJsValue(*eng, rect.width_));
            object->SetProperty("height", CreateJsValue(*eng, rect.height_));
            NativeValue* argv[] = {sizeValue};
            thisListener->CallJsMethod(WINDOW_SIZE_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnModeChange(WindowMode mode)
{
    WLOGFI("JsWindowListener::OnModeChange is called");
}

void JsWindowListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
    WLOGFI("JsWindowListener::OnSystemBarPropertyChange is called");
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [self = weakRef_, displayId, tints, eng = engine_] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng ==nullptr) {
                WLOGFE("[NAPI]this listener or engine is nullptr");
                return;
            }
            NativeValue* propertyValue = eng->CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(propertyValue);
            if (object == nullptr) {
                WLOGFE("Failed to convert prop to jsObject");
                return;
            }
            object->SetProperty("displayId", CreateJsValue(*eng, static_cast<uint32_t>(displayId)));
            object->SetProperty("regionTint", CreateJsSystemBarRegionTintArrayObject(*eng, tints));
            NativeValue* argv[] = {propertyValue};
            thisListener->CallJsMethod(SYSTEM_BAR_TINT_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnAvoidAreaChanged(const std::vector<Rect> avoidAreas)
{
    WLOGFI("JsWindowListener::OnAvoidAreaChanged is called");
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [self = weakRef_,  avoidAreas, eng = engine_] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or engine is nullptr");
                return;
            }
            NativeValue* avoidAreaValue = eng->CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(avoidAreaValue);
            if (object == nullptr) {
                WLOGFE("JsWindowListener::OnAvoidAreaChanged Failed to convert rect to jsObject");
                return;
            }

            if (static_cast<uint32_t>(avoidAreas.size()) != AVOID_AREA_NUM) {
                WLOGFE("AvoidAreas size is not 4 (left, top, right, bottom). Current avoidAreas size is %{public}u",
                    static_cast<uint32_t>(avoidAreas.size()));
                return;
            }

            object->SetProperty("leftRect", GetRectAndConvertToJsValue(*eng, avoidAreas[0]));   // idx 0 : left
            object->SetProperty("topRect", GetRectAndConvertToJsValue(*eng, avoidAreas[1]));    // idx 1 : top
            object->SetProperty("rightRect", GetRectAndConvertToJsValue(*eng, avoidAreas[2]));  // idx 2 : right
            object->SetProperty("bottomRect", GetRectAndConvertToJsValue(*eng, avoidAreas[3])); // idx 3 : bottom
            NativeValue* argv[] = {avoidAreaValue};
            thisListener->CallJsMethod(SYSTEM_AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::LifeCycleCallBack(LifeCycleEventType eventType)
{
    WLOGFI("JsWindowListener::LifeCycleCallBack is called, type: %{public}d", eventType);
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback>(
        [self = weakRef_,eventType, eng = engine_] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or engine is nullptr");
                return;
            }
            NativeValue* argv[] = {CreateJsValue(*eng, static_cast<uint32_t>(eventType))};
            thisListener->CallJsMethod(LIFECYCLE_EVENT_CB.c_str(), argv, ArraySize(argv));
        }
    );
    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::AfterForeground()
{
    LifeCycleCallBack(LifeCycleEventType::FOREGROUND);
}

void JsWindowListener::AfterBackground()
{
    LifeCycleCallBack(LifeCycleEventType::BACKGROUND);
}

void JsWindowListener::AfterFocused()
{
    LifeCycleCallBack(LifeCycleEventType::ACTIVE);
}

void JsWindowListener::AfterUnfocused()
{
    LifeCycleCallBack(LifeCycleEventType::INACTIVE);
}

void JsWindowListener::OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info)
{
    WLOGFI("[NAPI]OccupiedAreaChangeInfo, type: %{public}u, " \
        "input rect: [%{public}d, %{public}d, %{public}u, %{public}u]", static_cast<uint32_t>(info->type_),
        info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, info->rect_.height_)};
            CallJsMethod(KEYBOARD_HEIGHT_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace Rosen
} // namespace OHOS