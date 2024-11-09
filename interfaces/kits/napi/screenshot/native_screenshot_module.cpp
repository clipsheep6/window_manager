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
#include "native_screenshot_module.h"

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <image_type.h>
#include <iosfwd>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <memory>
#include <napi/native_api.h>
#include <napi/native_common.h>
#include <string>
#include <type_traits>

#include "display_manager.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "dm_napi_common.h"

namespace OHOS::Rosen {
namespace save {
struct Option {
    Media::Rect rect;
    Media::Size size;
    int rotation = 0;
    DisplayId displayId = 0;
    bool isNeedNotify = true;
    bool isNeedPointer = true;
};

struct Param {
    DmErrorCode wret;
    Option option;
    std::string errMessage;
    bool useInputOption;
    bool validInputParam;
    std::shared_ptr<Media::PixelMap> image;
    Media::Rect imageRect;
    bool isPick;
};

static napi_valuetype GetType(napi_env env, napi_value root)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, root, &res);
    return res;
}

static void GetDisplayId(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: GetDisplayId");
    napi_value displayId;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "displayId", &displayId));
    if (displayId != nullptr && GetType(env, displayId) == napi_number) {
        int64_t dispId;
        NAPI_CALL_RETURN_VOID(env, napi_get_value_int64(env, displayId, &dispId));
        param->option.displayId = static_cast<DisplayId>(dispId);
        GNAPI_LOG("GetDisplayId success, displayId = %{public}" PRIu64"", param->option.displayId);
    } else {
        GNAPI_LOG("GetDisplayId failed, invalid param, use default displayId = 0");
    }
}

static void GetRotation(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: GetRotation");
    napi_value rotation;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "rotation", &rotation));
    if (rotation != nullptr && GetType(env, rotation) == napi_number) {
        NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, rotation, &param->option.rotation));
        GNAPI_LOG("GetRotation success, rotation = %{public}d", param->option.rotation);
    } else {
        GNAPI_LOG("GetRotation failed, invalid param, use default rotation = 0");
    }
}

static void GetScreenRect(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: GetScreenRect");
    napi_value screenRect;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "screenRect", &screenRect));
    if (screenRect != nullptr && GetType(env, screenRect) == napi_object) {
        GNAPI_LOG("get ScreenRect success");

        napi_value left;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "left", &left));
        if (left != nullptr && GetType(env, left) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, left, &param->option.rect.left));
            GNAPI_LOG("get ScreenRect.left success, left = %{public}d", param->option.rect.left);
        } else {
            GNAPI_LOG("get ScreenRect.left failed, invalid param, use default left = 0");
        }

        napi_value top;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "top", &top));
        if (top != nullptr && GetType(env, top) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, top, &param->option.rect.top));
            GNAPI_LOG("get ScreenRect.top success, top = %{public}d", param->option.rect.top);
        } else {
            GNAPI_LOG("get ScreenRect.top failed, invalid param, use default top = 0");
        }

        napi_value width;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "width", &width));
        if (width != nullptr && GetType(env, width) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, width, &param->option.rect.width));
            GNAPI_LOG("get ScreenRect.width success, width = %{public}d", param->option.rect.width);
        } else {
            GNAPI_LOG("get ScreenRect.width failed, invalid param, use default width = 0");
        }

        napi_value height;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "height", &height));
        if (height != nullptr && GetType(env, height) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, height, &param->option.rect.height));
            GNAPI_LOG("get ScreenRect.height success, height = %{public}d", param->option.rect.height);
        } else {
            GNAPI_LOG("get ScreenRect.height failed, invalid param, use default height = 0");
        }
    } else {
        GNAPI_LOG("get ScreenRect failed, use default ScreenRect param");
    }
}

static void GetImageSize(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: ImageSize");
    napi_value imageSize;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "imageSize", &imageSize));
    if (imageSize != nullptr && GetType(env, imageSize) == napi_object) {
        napi_value width;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, imageSize, "width", &width));
        if (width != nullptr && GetType(env, width) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, width, &param->option.size.width));
            GNAPI_LOG("get ImageSize.width success, width = %{public}d", param->option.size.width);
        } else {
            GNAPI_LOG("get ImageSize.width failed, invalid param, use default width = 0");
        }

        napi_value height;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, imageSize, "height", &height));
        if (height != nullptr && GetType(env, height) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, height, &param->option.size.height));
            GNAPI_LOG("get ImageSize.height success, height = %{public}d", param->option.size.height);
        } else {
            GNAPI_LOG("get ImageSize.height failed, invalid param, use default height = 0");
        }
    }
}

static void IsNeedNotify(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: IsNeedNotify");
    napi_value isNeedNotify;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "isNotificationNeeded", &isNeedNotify));
    if (isNeedNotify != nullptr && GetType(env, isNeedNotify) == napi_boolean) {
        NAPI_CALL_RETURN_VOID(env, napi_get_value_bool(env, isNeedNotify, &param->option.isNeedNotify));
        GNAPI_LOG("IsNeedNotify: %{public}d", param->option.isNeedNotify);
    } else {
        GNAPI_LOG("IsNeedNotify failed, invalid param, use default true.");
    }
}

static void IsNeedPointer(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: IsNeedPointer");
    napi_value isNeedPointer;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "isPointerNeeded", &isNeedPointer));
    if (isNeedPointer != nullptr && GetType(env, isNeedPointer) == napi_boolean) {
        NAPI_CALL_RETURN_VOID(env, napi_get_value_bool(env, isNeedPointer, &param->option.isNeedPointer));
        GNAPI_LOG("IsNeedPointer: %{public}d", param->option.isNeedPointer);
    } else {
        GNAPI_LOG("IsNeedPointer failed, invalid param, use default true.");
    }
}

static void GetScreenshotParam(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    if (param == nullptr) {
        GNAPI_LOG("param == nullptr, use default param");
        return;
    }
    GetDisplayId(env, param, argv);
    GetRotation(env, param, argv);
    GetScreenRect(env, param, argv);
    GetImageSize(env, param, argv);
    IsNeedNotify(env, param, argv);
    IsNeedPointer(env, param, argv);
}

static void AsyncGetScreenshot(napi_env env, std::unique_ptr<Param> &param)
{
    if (!param->validInputParam) {
        WLOGFE("Invalid Input Param!");
        param->image = nullptr;
        param->wret = DmErrorCode::DM_ERROR_INVALID_PARAM;
        param->errMessage = "Get Screenshot Failed: Invalid input param";
        return;
    }
    CaptureOption option = { param->option.displayId, param->option.isNeedNotify, param->option.isNeedPointer};
    if (!param->isPick && (!option.isNeedNotify_ || !option.isNeedPointer_)) {
        if (param->useInputOption) {
            param->image = DisplayManager::GetInstance().GetScreenshotWithOption(option,
                param->option.rect, param->option.size, param->option.rotation, &param->wret);
        } else {
            param->image = DisplayManager::GetInstance().GetScreenshotWithOption(option, &param->wret);
        }
    } else {
        if (param->useInputOption) {
            GNAPI_LOG("Get Screenshot by input option");
            param->image = DisplayManager::GetInstance().GetScreenshot(param->option.displayId,
                param->option.rect, param->option.size, param->option.rotation, &param->wret);
        } else if (param->isPick) {
            GNAPI_LOG("Get Screenshot by picker");
            param->image = DisplayManager::GetInstance().GetSnapshotByPicker(param->imageRect, &param->wret);
        } else {
            GNAPI_LOG("Get Screenshot by default option");
            param->image = DisplayManager::GetInstance().GetScreenshot(param->option.displayId, &param->wret);
        }
    }
    if (param->image == nullptr && param->wret == DmErrorCode::DM_OK) {
        GNAPI_LOG("Get Screenshot failed!");
        param->wret = DmErrorCode::DM_ERROR_INVALID_SCREEN;
        param->errMessage = "Get Screenshot failed: Screenshot image is nullptr";
        return;
    }
}

napi_value CreateJsNumber(napi_env env, int32_t value)
{
    napi_value valRet = nullptr;
    napi_create_int32(env, value, &valRet);
    return valRet;
}

napi_value CreateJsRectObject(napi_env env, Media::Rect imageRect)
{
    napi_value objValue = nullptr;
    NAPI_CALL(env, napi_create_object(env, &objValue));
    napi_set_named_property(env, objValue, "left", CreateJsNumber(env, imageRect.left));
    napi_set_named_property(env, objValue, "top", CreateJsNumber(env, imageRect.top));
    napi_set_named_property(env, objValue, "width", CreateJsNumber(env, imageRect.width));
    napi_set_named_property(env, objValue, "height", CreateJsNumber(env, imageRect.height));
    return objValue;
}

napi_value CreateJsPickerObject(napi_env env, std::unique_ptr<Param> &param)
{
    napi_value objValue = nullptr;
    NAPI_CALL(env, napi_create_object(env, &objValue));
    if (param == nullptr) {
        napi_value result;
        WLOGFE("param nullptr.");
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }
    napi_set_named_property(env, objValue, "pixelMap", OHOS::Media::PixelMapNapi::CreatePixelMap(env, param->image));
    napi_set_named_property(env, objValue, "pickRect", CreateJsRectObject(env, param->imageRect));
    WLOGFI("pick end");
    return objValue;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    napi_value result;
    napi_value error;
    napi_value code;
    bool isThrowError = true;
    if (param->wret != DmErrorCode::DM_OK) {
        napi_create_error(env, nullptr, nullptr, &error);
        napi_create_int32(env, (int32_t)param->wret, &code);
    }
    switch (param->wret) {
        case DmErrorCode::DM_ERROR_NO_PERMISSION:
            napi_set_named_property(env, error, "DM_ERROR_NO_PERMISSION", code);
            break;
        case DmErrorCode::DM_ERROR_INVALID_PARAM:
            napi_set_named_property(env, error, "DM_ERROR_INVALID_PARAM", code);
            break;
        case DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT:
            napi_set_named_property(env, error, "DM_ERROR_DEVICE_NOT_SUPPORT", code);
            break;
        case DmErrorCode::DM_ERROR_SYSTEM_INNORMAL:
            napi_set_named_property(env, error, "DM_ERROR_SYSTEM_INNORMAL", code);
            break;
        default:
            isThrowError = false;
            WLOGFI("screen shot default.");
            break;
    }
    WLOGFI("screen shot ret=%{public}d.", param->wret);
    if (isThrowError) {
        napi_throw(env, error);
        return error;
    }
    if (param->wret != DmErrorCode::DM_OK) {
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }
    if (param->isPick) {
        GNAPI_LOG("Resolve Screenshot by picker");
        return CreateJsPickerObject(env, param);
    }
    GNAPI_LOG("Screenshot image Width %{public}d, Height %{public}d",
        param->image->GetWidth(), param->image->GetHeight());
    napi_value jsImage = OHOS::Media::PixelMapNapi::CreatePixelMap(env, param->image);
    return jsImage;
}

napi_value PickFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_value argv[1] = { nullptr };  // the max number of input parameters is 1
    size_t argc = 1;  // the max number of input parameters is 1
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    auto param = std::make_unique<Param>();
    if (param == nullptr) {
        WLOGFE("Create param failed.");
        return nullptr;
    }
    napi_ref ref = nullptr;
    if (argc == 0) {  // 0 valid parameters
        GNAPI_LOG("argc == 0");
        param->validInputParam = true;
    } else if (GetType(env, argv[0]) == napi_function) {  // 1 valid parameters napi_function
        GNAPI_LOG("argc >= 1, argv[0]'s type is napi_function");
        param->validInputParam = true;
        NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &ref));
    } else {  // 0 valid parameters
        GNAPI_LOG("argc == 0");
        param->validInputParam = true;
    }
    param->isPick = true;
    return AsyncProcess<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenshot, Resolve, ref, param);
}

static void AsyncGetScreenCapture(napi_env env, std::unique_ptr<Param> &param)
{
    CaptureOption captureOption;
    captureOption.displayId_ = param->option.displayId;
    captureOption.isNeedNotify_ = param->option.isNeedNotify;
    captureOption.isNeedPointer_ = param->option.isNeedPointer;
    GNAPI_LOG("capture option isNeedNotify=%{public}d isNeedPointer=%{public}d", captureOption.isNeedNotify_,
        captureOption.isNeedPointer_);
    param->image = DisplayManager::GetInstance().GetScreenCapture(captureOption, &param->wret);
    if (param->image == nullptr && param->wret == DmErrorCode::DM_OK) {
        GNAPI_LOG("screen capture failed!");
        param->wret = DmErrorCode::DM_ERROR_SYSTEM_INNORMAL;
        param->errMessage = "ScreenCapture failed: image is null.";
        return;
    }
}

napi_value CaptureFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_value argv[1] = { nullptr };
    size_t argc = 1;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    auto param = std::make_unique<Param>();
    if (param == nullptr) {
        WLOGFE("Create param failed.");
        return nullptr;
    }
    param->option.displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    napi_ref ref = nullptr;
    if (argc > 0 && GetType(env, argv[0]) == napi_object) {
        GNAPI_LOG("argv[0]'s type is napi_object");
        GetScreenshotParam(env, param, argv[0]);
    } else {
        GNAPI_LOG("use default.");
    }
    return AsyncProcess<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenCapture, Resolve, ref, param);
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_value argv[2] = {nullptr}; // the max number of input parameters is 2
    size_t argc = 2; // the max number of input parameters is 2
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    auto param = std::make_unique<Param>();
    if (param == nullptr) {
        WLOGFE("Create param failed.");
        return nullptr;
    }
    param->option.displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    napi_ref ref = nullptr;
    if (argc == 0) { // 0 valid parameters
        GNAPI_LOG("argc == 0");
        param->validInputParam = true;
    } else if (GetType(env, argv[0]) == napi_function) { // 1 valid parameters napi_function
        GNAPI_LOG("argc >= 1, argv[0]'s type is napi_function");
        param->validInputParam = true;
        NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &ref));
    } else if (GetType(env, argv[0]) == napi_object) {
        if ((argc >= 2) && (GetType(env, argv[1]) == napi_function)) { // 2 valid parameters napi_object napi_function
            GNAPI_LOG("argc >= 2, argv[0]'s type is napi_object, argv[1]'s type is napi_function");
            param->validInputParam = true;
            param->useInputOption = true;
            GetScreenshotParam(env, param, argv[0]);
            NAPI_CALL(env, napi_create_reference(env, argv[1], 1, &ref));
        } else { // 1 valid parameters napi_object
            GNAPI_LOG("argc >= 1, argv[0]'s type is napi_object");
            param->validInputParam = true;
            param->useInputOption = true;
            GetScreenshotParam(env, param, argv[0]);
        }
    } else { // 0 valid parameters
        GNAPI_LOG("argc == 0");
        param->validInputParam = true;
    }
    param->isPick = false;
    return AsyncProcess<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenshot, Resolve, ref, param);
}
} // namespace save

void SetNamedProperty(napi_env env, napi_value dstObj, const int32_t objValue, const char *propName)
{
    napi_value prop = nullptr;
    napi_create_int32(env, objValue, &prop);
    napi_set_named_property(env, dstObj, propName, prop);
}

napi_value ScreenshotModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_value errorCode = nullptr;
    napi_value dmErrorCode = nullptr;
    napi_create_object(env, &errorCode);
    napi_create_object(env, &dmErrorCode);

    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED, "DM_ERROR_INIT_DMS_PROXY_LOCKED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_IPC_FAILED, "DM_ERROR_IPC_FAILED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_REMOTE_CREATE_FAILED, "DM_ERROR_REMOTE_CREATE_FAILED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_NULLPTR, "DM_ERROR_NULLPTR");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_INVALID_PARAM, "DM_ERROR_INVALID_PARAM");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED, "DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_DEATH_RECIPIENT, "DM_ERROR_DEATH_RECIPIENT");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_INVALID_MODE_ID, "DM_ERROR_INVALID_MODE_ID");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_WRITE_DATA_FAILED, "DM_ERROR_WRITE_DATA_FAILED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_RENDER_SERVICE_FAILED, "DM_ERROR_RENDER_SERVICE_FAILED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_UNREGISTER_AGENT_FAILED, "DM_ERROR_UNREGISTER_AGENT_FAILED");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_INVALID_CALLING, "DM_ERROR_INVALID_CALLING");
    SetNamedProperty(env, errorCode,
        (int32_t)DMError::DM_ERROR_UNKNOWN, "DM_ERROR_UNKNOWN");

    SetNamedProperty(env, dmErrorCode,
        (int32_t)DmErrorCode::DM_ERROR_NO_PERMISSION, "DM_ERROR_NO_PERMISSION");
    SetNamedProperty(env, dmErrorCode,
        (int32_t)DmErrorCode::DM_ERROR_INVALID_PARAM, "DM_ERROR_INVALID_PARAM");
    SetNamedProperty(env, dmErrorCode,
        (int32_t)DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT, "DM_ERROR_DEVICE_NOT_SUPPORT");
    SetNamedProperty(env, dmErrorCode,
        (int32_t)DmErrorCode::DM_ERROR_INVALID_SCREEN, "DM_ERROR_INVALID_SCREEN");
    SetNamedProperty(env, dmErrorCode,
        (int32_t)DmErrorCode::DM_ERROR_INVALID_CALLING, "DM_ERROR_INVALID_CALLING");
    SetNamedProperty(env, dmErrorCode,
        (int32_t)DmErrorCode::DM_ERROR_SYSTEM_INNORMAL, "DM_ERROR_SYSTEM_INNORMAL");

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("save", save::MainFunc),
        DECLARE_NAPI_FUNCTION("pick", save::PickFunc),
        DECLARE_NAPI_FUNCTION("capture", save::CaptureFunc),
        DECLARE_NAPI_PROPERTY("DMError", errorCode),
        DECLARE_NAPI_PROPERTY("DmErrorCode", dmErrorCode),
    };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}
} // namespace OHOS::Rosen

static napi_module g_screenshotModule = {
    .nm_version = 1, // NAPI v1
    .nm_flags = 0, // normal
    .nm_filename = nullptr,
    .nm_register_func = OHOS::Rosen::ScreenshotModuleInit,
    .nm_modname = "screenshot",
    .nm_priv = nullptr,
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_screenshotModule);
}
