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

#ifndef OHOS_WINDOW_AGENT_H
#define OHOS_WINDOW_AGENT_H

#include "window_stub.h"
#include "window_impl.h"
#include "window_property.h"

namespace OHOS {
namespace Rosen {
class WindowAgent : public WindowStub {
public:
    WindowAgent(sptr<WindowImpl>& window);
    ~WindowAgent() = default;
    void UpdateWindowProperty(const WindowProperty& windowProperty) override;
    void UpdateWindowRect(const struct Rect& rect) override;
    void UpdateWindowMode(WindowMode mode) override;
    void UpdateFocusStatus(bool focused) override;
    void UpdateSystemBarProperty(const SystemBarProperty& prop) override;

private:
    sptr<WindowImpl> window_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_AGENT_H
