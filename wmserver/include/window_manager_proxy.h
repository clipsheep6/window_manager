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

#ifndef OHOS_WINDOW_MANAGER_PROXY_H
#define OHOS_WINDOW_MANAGER_PROXY_H

#include <iremote_proxy.h>
#include "window_manager_interface.h"

namespace OHOS {
namespace Rosen {
class WindowManagerProxy : public IRemoteProxy<IWindowManager> {
public:
    explicit WindowManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IWindowManager>(impl) {};

    ~WindowManagerProxy() {};

    WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId) override;
    WMError AddWindow(sptr<WindowProperty>& property) override;
    WMError RemoveWindow(uint32_t windowId) override;
    WMError DestroyWindow(uint32_t windowId) override;
    WMError MoveTo(uint32_t windowId, int32_t x, int32_t y) override;
    WMError Resize(uint32_t windowId, uint32_t width, uint32_t height) override;
    WMError RequestFocus(uint32_t windowId) override;
    WMError SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId) override;
    WMError SetWindowMode(uint32_t windowId, WindowMode mode) override;
    WMError SetWindowType(uint32_t windowId, WindowType type) override;
    WMError SetWindowFlags(uint32_t windowId, uint32_t flags) override;
    WMError SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& prop) override;
    std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type) override;
    WMError MinimizeAllAppNodeAbility(uint32_t windowId) override;
    void ProcessWindowTouchedEvent(uint32_t windowId) override;
    void MinimizeAllAppWindows(DisplayId displayId) override;

    void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;

private:
    static inline BrokerDelegator<WindowManagerProxy> delegator_;
};
}
}
#endif // OHOS_WINDOW_MANAGER_PROXY_H
