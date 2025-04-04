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

#include "extension_manager.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t HOST_WINDOW_RECT_CHANGE = 1;
enum class ExtensionEventFlag : int32_t {
    EVENT_NONE = 0x00000000,
    EVENT_PAN_GESTURE_LEFT = 0x00000001,
    EVENT_PAN_GESTURE_RIGHT = 0x00000002,
    EVENT_PAN_GESTURE_UP = 0x00000004,
    EVENT_PAN_GESTURE_DOWN = 0x00000008,
    EVENT_CLICK = 0x00000100,
    EVENT_LONG_PRESS = 0x00000200,
};
}

void ExtensionManager::Finalizer(napi_env env, void* data, void* hint)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "in");
    std::unique_ptr<ExtensionManager>(static_cast<ExtensionManager*>(data));
}

static napi_status SetNamedProperty(napi_env env, napi_value obj, const std::string& name, uint32_t value)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "in");
    napi_value property = nullptr;
    napi_status status = napi_create_uint32(env, value, &property);
    if (status != napi_ok) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Failed to call napi_create_uint32");
        return status;
    }
    status = napi_set_named_property(env, obj, name.c_str(), property);
    if (status != napi_ok) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Failed to call napi_set_named_property");
        return status;
    }
    return status;
}

static napi_value ExportRectChangeReasonType(napi_env env)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "in");
    napi_value result = nullptr;
    napi_create_object(env, &result);
    if (result == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Failed to create object");
        return nullptr;
    }

    static_cast<void>(SetNamedProperty(env, result, "HOST_WINDOW_RECT_CHANGE", HOST_WINDOW_RECT_CHANGE));

    napi_object_freeze(env, result);
    return result;
}

static napi_value ExportEventFlag(napi_env env)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "in");
    napi_value result = nullptr;
    napi_create_object(env, &result);
    if (result == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Failed to create object");
        return nullptr;
    }

    static_cast<void>(SetNamedProperty(env, result, "EVENT_NONE",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_NONE)));
    static_cast<void>(SetNamedProperty(env, result, "EVENT_PAN_GESTURE_LEFT",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_PAN_GESTURE_LEFT)));
    static_cast<void>(SetNamedProperty(env, result, "EVENT_PAN_GESTURE_RIGHT",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_PAN_GESTURE_RIGHT)));
    static_cast<void>(SetNamedProperty(env, result, "EVENT_PAN_GESTURE_UP",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_PAN_GESTURE_UP)));
    static_cast<void>(SetNamedProperty(env, result, "EVENT_PAN_GESTURE_DOWN",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_PAN_GESTURE_DOWN)));
    static_cast<void>(SetNamedProperty(env, result, "EVENT_CLICK",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_CLICK)));
    static_cast<void>(SetNamedProperty(env, result, "EVENT_LONG_PRESS",
        static_cast<int32_t>(ExtensionEventFlag::EVENT_LONG_PRESS)));

    napi_object_freeze(env, result);
    return result;
}

napi_value ExtensionModuleInit(napi_env env, napi_value exportObj)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "in");
    if (env == nullptr || exportObj == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "env or exportObj is nullptr");
        return nullptr;
    }
    std::unique_ptr<ExtensionManager> extensionManager = std::make_unique<ExtensionManager>();
    napi_wrap(env, exportObj, extensionManager.release(), ExtensionManager::Finalizer, nullptr, nullptr);

    napi_set_named_property(env, exportObj, "RectChangeReason", ExportRectChangeReasonType(env));
    napi_set_named_property(env, exportObj, "EventFlag", ExportEventFlag(env));

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}
} // namespace Rosen
} // namespace OHOS