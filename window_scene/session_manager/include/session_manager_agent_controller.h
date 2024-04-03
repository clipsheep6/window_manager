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

#ifndef OHOS_ROSEN_SESSION_MANAGER_AGENT_CONTROLLER_H
#define OHOS_ROSEN_SESSION_MANAGER_AGENT_CONTROLLER_H

#include "client_agent_container.h"
#include "window_manager.h"
#include "wm_single_instance.h"
#include "zidl/window_manager_agent_interface.h"
#include "window_visibility_info.h"
#include "window_drawing_content_info.h"

namespace OHOS {
namespace Rosen {
class SessionManagerAgentController {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManagerAgentController)
public:
    WMError RegisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
        WindowManagerAgentType type);
    WMError UnregisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
        WindowManagerAgentType type);

    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused);
    void UpdateWindowModeTypeInfo(WindowModeType type);
    void NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
        WindowUpdateType type);
    void NotifyWaterMarkFlagChangedResult(bool hasWaterMark);
    void UpdateWindowVisibilityInfo(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos);
    void UpdateWindowDrawingContentInfo(const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos);
private:
    SessionManagerAgentController() {}
    virtual ~SessionManagerAgentController() = default;

    ClientAgentContainer<IWindowManagerAgent, WindowManagerAgentType> smAgentContainer_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_MANAGER_AGENT_CONTROLLER_H
