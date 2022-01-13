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

#ifndef OHOS_WINDOW_MANAGER_SERVICE_H
#define OHOS_WINDOW_MANAGER_SERVICE_H

#include <vector>
#include <map>

#include <input_window_monitor.h>
#include <nocopyable.h>
#include <system_ability.h>
#include "singleton_delegator.h"
#include "single_instance.h"
#include "window_controller.h"
#include "window_manager_stub.h"
#include "window_root.h"
#include "snapshot_controller.h"

namespace OHOS {
namespace Rosen {
class WindowManagerService : public SystemAbility, public WindowManagerStub {
DECLARE_SYSTEM_ABILITY(WindowManagerService);

DECLARE_SINGLE_INSTANCE_BASE(WindowManagerService);

public:
    void OnStart() override;
    void OnStop() override;

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

    void RegisterFocusChangedListener(const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    void UnregisterFocusChangedListener(const sptr<IWindowManagerAgent>& windowManagerAgent) override;

protected:
    WindowManagerService();
    virtual ~WindowManagerService() = default;

private:
    bool Init();
    void RegisterSnapshotHandler();
    void OnWindowEvent(Event event, uint32_t windowId);
    static inline SingletonDelegator<WindowManagerService> delegator;
    std::recursive_mutex mutex_;
    sptr<WindowRoot> windowRoot_;
    sptr<WindowController> windowController_;
    sptr<InputWindowMonitor> inputWindowMonitor_;
    sptr<SnapshotController> snapshotController_;
    const int WAITING_RS_TIME = 10;
};
}
}
#endif // OHOS_WINDOW_MANAGER_SERVICE_H
