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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_STUB_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_STUB_H

#include <iremote_stub.h>

#include "session_manager/include/zidl/scene_session_manager_interface.h"

namespace OHOS::Rosen {
class SceneSessionManagerStub : public IRemoteStub<ISceneSessionManager> {
public:
    SceneSessionManagerStub() = default;
    virtual ~SceneSessionManagerStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void HandleUpdateProperty(MessageParcel &data, MessageParcel &reply);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_STUB_H
