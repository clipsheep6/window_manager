/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ORSEN_WINDOW_SCENE_ROOT_SCENE_SESSION_H
#define OHOS_ORSEN_WINDOW_SCENE_ROOT_SCENE_SESSION_H

#include "scene_session.h"

namespace OHOS::AbilityRuntime {
class Context;
}

class NativeEngine;
class NativeValue;

namespace OHOS::Rosen {
class RootSceneSession : public SceneSession {
public:
    using LoadContentFunc = std::function<void(const std::string&, NativeEngine*, NativeValue*,
        AbilityRuntime::Context*)>;
    RootSceneSession();
    ~RootSceneSession() = default;

    void SetLoadContentFunc(const LoadContentFunc& loadContentFunc);
    void LoadContent(const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
        AbilityRuntime::Context* context);

private:
    LoadContentFunc loadContentFunc_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ORSEN_WINDOW_SCENE_ROOT_SCENE_SESSION_H
