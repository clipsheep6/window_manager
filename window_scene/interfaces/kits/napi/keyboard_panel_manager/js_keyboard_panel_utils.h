/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_SCENE_JS_KEYBOARD_PANEL_UTILS_H
#define OHOS_WINDOW_SCENE_JS_KEYBOARD_PANEL_UTILS_H

#include "interfaces/kits/napi/common/js_common_utils.h"
#include "napi_common_util.h"
#include "wm_common.h"

namespace OHOS::Rosen {
enum KeyboardPrivateDataValueType : int32_t {VALUE_TYPE_STRING = 0, VALUE_TYPE_BOOL, VALUE_TYPE_NUMBER};

bool ConvertKeyboardPrivateCommandFromJs(napi_env env, napi_value in,
    std::unordered_map<std::string, KeyboardPrivateDataValue>& out);
bool GetPrivateDataValue(napi_env env, napi_value in, KeyboardPrivateDataValue& out);
napi_value GetJsKeyboardPrivateCommand(napi_env env,
    const std::unordered_map<std::string, KeyboardPrivateDataValue>& in);
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_JS_KEYBOARD_PANEL_UTILS_H