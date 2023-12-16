
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

#include "window_manager_agent_lite.h"
#include "foundation/window/window_manager/interfaces/innerkits/wm/window_manager_lite.h"
#include "singleton_container.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
void WindowManagerAgentLite::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused)
{
    SingletonContainer::Get<WindowManagerLite>().UpdateFocusChangeInfo(focusChangeInfo, focused);
}

void WindowManagerAgentLite::UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints)
{
    SingletonContainer::Get<WindowManagerLite>().UpdateSystemBarRegionTints(displayId, tints);
}

void WindowManagerAgentLite::NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
    WindowUpdateType type)
{
    SingletonContainer::Get<WindowManagerLite>().NotifyAccessibilityWindowInfo(infos, type);
}

void WindowManagerAgentLite::UpdateWindowVisibilityInfo(const std::vector<sptr<WindowVisibilityInfo>>& visibilityInfos)
{
    SingletonContainer::Get<WindowManagerLite>().UpdateWindowVisibilityInfo(visibilityInfos);
}

void WindowManagerAgentLite::UpdateWindowDrawingContentInfo(
    const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos)
{
    SingletonContainer::Get<WindowManagerLite>().UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
}

void WindowManagerAgentLite::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SingletonContainer::Get<WindowManagerLite>().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}

void WindowManagerAgentLite::NotifyWaterMarkFlagChangedResult(bool showWaterMark)
{
    SingletonContainer::Get<WindowManagerLite>().NotifyWaterMarkFlagChangedResult(showWaterMark);
}

void WindowManagerAgentLite::NotifyGestureNavigationEnabledResult(bool enable)
{
    SingletonContainer::Get<WindowManagerLite>().NotifyGestureNavigationEnabledResult(enable);
}
} // namespace Rosen
} // namespace OHOS
