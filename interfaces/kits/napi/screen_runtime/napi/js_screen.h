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
#include <set>

#include "screen.h"

namespace OHOS {
namespace Rosen {
enum class ApiColorSpaceType : uint32_t {
    UNKNOWN = 0,
    ADOBE_RGB = 1,
    BT2020_HLG = 2,
    BT2020_PQ= 3,
    BT601_EBU = 4,
    BT601_SMPTE_C = 5,
    BT709 = 6,
    P3_HLG = 7,
    P3_PQ = 8,
    DISPLAY_P3 = 9,
    SRGB = 10,
    LINEAR_SRGB = 11,
    LINEAR_P3 = 12,
    LINEAR_BT2020 = 13,
};

const std::map<GraphicCM_ColorSpaceType, ApiColorSpaceType> NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP {
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE,        ApiColorSpaceType::UNKNOWN },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_FULL,          ApiColorSpaceType::ADOBE_RGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_LIMIT,         ApiColorSpaceType::ADOBE_RGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_FULL,        ApiColorSpaceType::BT2020_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_LIMIT,       ApiColorSpaceType::BT2020_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_BT2020_HLG,     ApiColorSpaceType::BT2020_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_FULL,         ApiColorSpaceType::BT2020_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_LIMIT,        ApiColorSpaceType::BT2020_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_BT2020_PQ,      ApiColorSpaceType::BT2020_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL,         ApiColorSpaceType::BT601_EBU },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_LIMIT,        ApiColorSpaceType::BT601_EBU },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPLE_C_FULL,     ApiColorSpaceType::BT601_SMPTE_C },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_SMPLE_C_LIMIT,    ApiColorSpaceType::BT601_SMPTE_C },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_FULL,             ApiColorSpaceType::BT709 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_LIMIT,            ApiColorSpaceType::BT709 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_FULL,            ApiColorSpaceType::P3_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_HLG_LIMIT,           ApiColorSpaceType::P3_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_HLG,         ApiColorSpaceType::P3_HLG },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_FULL,             ApiColorSpaceType::P3_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_PQ_LIMIT,            ApiColorSpaceType::P3_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_PQ,          ApiColorSpaceType::P3_PQ },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL,                ApiColorSpaceType::DISPLAY_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_LIMIT,               ApiColorSpaceType::DISPLAY_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_P3_SRGB,        ApiColorSpaceType::DISPLAY_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL,              ApiColorSpaceType::SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_LIMIT,             ApiColorSpaceType::SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_DISPLAY_SRGB,           ApiColorSpaceType::SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_SRGB,            ApiColorSpaceType::LINEAR_SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_BT709,           ApiColorSpaceType::LINEAR_SRGB },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_P3,              ApiColorSpaceType::LINEAR_P3 },
    { GraphicCM_ColorSpaceType::GRAPHIC_CM_LINEAR_BT2020,          ApiColorSpaceType::LINEAR_BT2020 },
};

enum class ApiHDRFormat : uint32_t {
    NONE = 0,
    VIDEO_HLG = 1,
    VIDEO_HDR10 = 2,
    VIDEO_HDR_VIVID = 3,
    IMAGE_HDR_VIVID_DUAL = 4,
    IMAGE_HDR_VIVID_SINGLE = 5,
    IMAGE_HDR_ISO_DUAL = 6,
    IMAGE_HDR_ISO_SINGLE = 7,
};

const std::map<ScreenHDRFormat, ApiHDRFormat> NATIVE_TO_JS_HDR_FORMAT_TYPE_MAP {
    { ScreenHDRFormat::NOT_SUPPORT_HDR,             ApiHDRFormat::NONE },
    { ScreenHDRFormat::VIDEO_HLG,                   ApiHDRFormat::VIDEO_HLG },
    { ScreenHDRFormat::VIDEO_HDR10,                 ApiHDRFormat::VIDEO_HDR10 },
    { ScreenHDRFormat::VIDEO_HDR_VIVID,             ApiHDRFormat::VIDEO_HDR_VIVID },
    { ScreenHDRFormat::IMAGE_HDR_VIVID_DUAL,        ApiHDRFormat::IMAGE_HDR_VIVID_DUAL },
    { ScreenHDRFormat::IMAGE_HDR_VIVID_SINGLE,      ApiHDRFormat::IMAGE_HDR_VIVID_SINGLE },
    { ScreenHDRFormat::IMAGE_HDR_ISO_DUAL,          ApiHDRFormat::IMAGE_HDR_ISO_DUAL },
    { ScreenHDRFormat::IMAGE_HDR_ISO_SINGLE,        ApiHDRFormat::IMAGE_HDR_ISO_SINGLE },
};

napi_value CreateJsColorSpaceArray(napi_env env, const std::vector<GraphicCM_ColorSpaceType>& colorSpaces);
napi_value CreateJsHDRFormatArray(napi_env env, const std::vector<ScreenHDRFormat>& hdrFormats);
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
    static napi_value GetSupportedColorSpaces(napi_env env, napi_callback_info info);
    static napi_value GetSupportedHDRFormats(napi_env env, napi_callback_info info);

private:
    sptr<Screen> screen_ = nullptr;
    napi_value OnSetOrientation(napi_env env, napi_callback_info info);
    napi_value OnSetScreenActiveMode(napi_env env, napi_callback_info info);
    napi_value OnSetDensityDpi(napi_env env, napi_callback_info info);
    napi_value OnGetSupportedColorSpaces(napi_env env, napi_callback_info info);
    napi_value OnGetSupportedHDRFormat(napi_env env, napi_callback_info info);
};
}  // namespace Rosen
}  // namespace OHOS
#endif