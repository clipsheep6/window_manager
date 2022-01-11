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

#include "window_manager_agent.h"
#include "foundation/windowmanager/interfaces/innerkits/wm/window_manager.h"
#include "singleton_container.h"

namespace OHOS {
namespace Rosen {
void WindowManagerAgent::UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, int32_t displayId, bool focused)
{
    SingletonContainer::Get<WindowManager>().UpdateFocusStatus(windowId, abilityToken, windowType, displayId, focused);
}

void WindowManagerAgent::UpdateSystemBarProperties(uint32_t displayId, const SystemBarProps& props)
{
    SingletonContainer::Get<WindowManager>().UpdateSystemBarProperties(displayId, props);
}
} // namespace Rosen
} // namespace OHOS
