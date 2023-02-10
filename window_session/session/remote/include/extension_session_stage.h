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

#ifndef OHOS_ORSEN_WINDOW_EXTENSION_SESSION_STAGE_H
#define OHOS_ORSEN_WINDOW_EXTENSION_SESSION_STAGE_H

#include "zidl/session_interface.h"
#include "session_stage.h"
#include "ws_common.h"
#include "zidl/session_stage_stub.h"

namespace OHOS::Rosen {
class ExtensionSessionStage : public SessionStage {
public:
    ExtensionSessionStage() {};
    ~ExtensionSessionStage() = default;

    virtual WSError Connect() override;
};
}
#endif // OHOS_ORSEN_WINDOW_EXTENSION_SESSION_STAGE_H