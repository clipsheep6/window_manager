/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_AGENT_H
#define OHOS_ROSEN_DISPLAY_MANAGER_AGENT_H

#include <refbase.h>
#include <zidl/display_manager_agent_stub.h>

namespace OHOS {
namespace Rosen {
class DisplayManagerAgent : public DisplayManagerAgentStub {
public:
    DisplayManagerAgent() = default;
    ~DisplayManagerAgent() = default;

    virtual void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) override;
};
}
}
#endif // OHOS_ROSEN_DISPLAY_MANAGER_AGENT_H
