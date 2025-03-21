/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef MOCK_WINDOW_MANAGER_AGENT_LITE_H
#define MOCK_WINDOW_MANAGER_AGENT_LITE_H
#include "window_manager_agent_lite.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class WindowManagerAgentLiteMocker : public WindowManagerAgentLite {
public:
    WindowManagerAgentLiteMocker() {}
    ~WindowManagerAgentLiteMocker() {}

    MOCK_METHOD1(NotifyCallingWindowDisplayChanged, void(const CallingWindowInfo& callingWindowInfo));
};
} // namespace Rosen
} // namespace OHOS
#endif // MOCK_WINDOW_MANAGER_AGENT_LITE_H