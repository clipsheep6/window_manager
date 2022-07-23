/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "foundation/window/window_manager/interfaces/innerkits/wm/window_manager.h"
#include "singleton_container.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
void WindowManagerAgent::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused)
{
    auto wm = SingletonContainer::GetPointer<WindowManager>();
    if (wm == nullptr) {
        return;
    }
    wm->UpdateFocusChangeInfo(focusChangeInfo, focused);
}

void WindowManagerAgent::UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints)
{
    auto wm = SingletonContainer::GetPointer<WindowManager>();
    if (wm == nullptr) {
        return;
    }
    wm->UpdateSystemBarRegionTints(displayId, tints);
}

void WindowManagerAgent::NotifyAccessibilityWindowInfo(const sptr<AccessibilityWindowInfo>& windowInfo,
    WindowUpdateType type)
{
    auto wm = SingletonContainer::GetPointer<WindowManager>();
    if (wm == nullptr) {
        return;
    }
    wm->NotifyAccessibilityWindowInfo(windowInfo, type);
}

void WindowManagerAgent::UpdateWindowVisibilityInfo(const std::vector<sptr<WindowVisibilityInfo>>& visibilityInfos)
{
    auto wm = SingletonContainer::GetPointer<WindowManager>();
    if (wm == nullptr) {
        return;
    }
    wm->UpdateWindowVisibilityInfo(visibilityInfos);
}

void WindowManagerAgent::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    auto wm = SingletonContainer::GetPointer<WindowManager>();
    if (wm == nullptr) {
        return;
    }
    wm->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}
} // namespace Rosen
} // namespace OHOS
