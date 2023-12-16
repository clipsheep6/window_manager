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

#ifndef OHOS_WINDOW_MANAGER_LITE_INTERFACE_H
#define OHOS_WINDOW_MANAGER_LITE_INTERFACE_H

#include <iremote_broker.h>

#include "window_property.h"
#include "window_transition_info.h"
#include "zidl/window_manager_agent_interface.h"
#include "interfaces/include/ws_common.h"

namespace OHOS {
namespace MMI {
class KeyEvent;
}
namespace Rosen {

class IWindowManagerLite : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowManagerLite");

    // do not need enum
    virtual WMError MinimizeAllAppWindows(DisplayId displayId) { return WMError::WM_OK; };  //unused
    virtual WMError ToggleShownStateForAllAppWindows() {return WMError::WM_OK; }; //unused
    virtual WMError SetWindowLayoutMode(WindowLayoutMode mode) {return WMError::WM_OK; }; //unused
    virtual WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0; //
    virtual WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0; //
    virtual WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) = 0; //
    virtual WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) { return WMError::WM_OK; }; //used unrealized??
    virtual WSError DumpSessionAll(std::vector<std::string> &infos) = 0; //
    virtual WSError DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos) = 0; //

    virtual WMError SetGestureNavigaionEnabled(bool enable) = 0;
    virtual WSError NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible) = 0;

    virtual void GetFocusWindowInfo(FocusChangeInfo& focusInfo) = 0; //
    virtual WMError CheckWindowId(int32_t windowId, int32_t &pid) = 0; //
    virtual WSError RaiseWindowToTop(int32_t persistentId) = 0; //
};
}
}
#endif // OHOS_WINDOW_MANAGER_INTERFACE_H
