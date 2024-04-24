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

#ifndef OHOS_WINDOW_SCENE_COMMON_UTILS_H
#define OHOS_WINDOW_SCENE_COMMON_UTILS_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <js_runtime_utils.h>

#include "wm_common.h"

namespace OHOS::Rosen {
napi_value NapiGetUndefined(napi_env env);
napi_valuetype GetType(napi_env env, napi_value value);

bool ParseArrayStringValue(napi_env env, napi_value array, std::vector<std::string>& vector);

class MainThreadScheduler {
public:
    using Task = std::function<void()>;
    explicit MainThreadScheduler(napi_env env);
    void PostMainThreadTask(Task && localTask, std::string traceInfo = "Unnamed", int64_t delayTime = 0);
private:
    void GetMainEventHandler();
    napi_env env_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_COMMON_UTILS_H