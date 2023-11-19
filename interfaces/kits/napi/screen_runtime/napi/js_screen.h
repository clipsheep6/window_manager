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

#ifndef OHOS_JS_SCREEN_H
#define OHOS_JS_SCREEN_H
#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <refbase.h>

#include "screen.h"

namespace OHOS {
namespace Rosen {
enum class CM_ColorSpaceType : uint32_t {
    NONE = 0,
    ADOBE_RGB = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = LINEAR_SRGB,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = SRGB,
    DISPLAY_P3_SRGB = DISPLAY_P3,
    DISPLAY_P3_HLG = P3_HLG,
    DISPLAY_P3_PQ = P3_PQ,
    DISPLAY_BT2020_SRGB = 26,
};

napi_value CreateJsColorSpaceArray(napi_env env, const std::vector<CM_ColorSpaceType>& colorSpaces);
napi_value CreateJsScreenObject(napi_env env, sptr<Screen>& screen);
napi_value CreateJsScreenModeArrayObject(napi_env env, std::vector<sptr<SupportedScreenModes>> screenModes);
napi_value CreateJsScreenModeObject(napi_env env, const sptr<SupportedScreenModes>& mode);
napi_value NapiGetUndefined(napi_env env);
napi_valuetype GetType(napi_env env, napi_value root);
class JsScreen final {
public:
    explicit JsScreen(const sptr<Screen>& screen);
    ~JsScreen();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value SetScreenActiveMode(napi_env env, napi_callback_info info);
    static napi_value SetOrientation(napi_env env, napi_callback_info info);
    static napi_value SetDensityDpi(napi_env env, napi_callback_info info);

    static napi_value GetScreenSupportedColorSpaces(napi_env env, napi_callback_info info);

private:
    sptr<Screen> screen_ = nullptr;
    napi_value OnSetOrientation(napi_env env, napi_callback_info info);
    napi_value OnSetScreenActiveMode(napi_env env, napi_callback_info info);
    napi_value OnSetDensityDpi(napi_env env, napi_callback_info info);

    napi_value OnGetScreenSupportedColorSpaces(napi_env env, napi_callback_info info);
};
}  // namespace Rosen
}  // namespace OHOS
#endif
