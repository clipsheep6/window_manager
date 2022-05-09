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
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>

#include <input_window_monitor.h>
#include <nocopyable.h>
#include <system_ability.h>
#include <window_manager_service_handler_stub.h>
#include "display_change_listener.h"
#include "drag_controller.h"
#include "freeze_controller.h"
#include "singleton_delegator.h"
#include "wm_single_instance.h"
#include "window_controller.h"
#include "zidl/window_manager_stub.h"
#include "window_dumper.h"
#include "window_root.h"
#include "snapshot_controller.h"

namespace OHOS {
namespace Rosen {
class DisplayChangeListener : public IDisplayChangeListener {
public:
    virtual void OnDisplayStateChange(DisplayId id, DisplayStateChangeType type) override;
};

class WindowManagerServiceHandler : public AAFwk::WindowManagerServiceHandlerStub {
public:
    virtual void NotifyWindowTransition(
        sptr<AAFwk::AbilityTransitionInfo> from, sptr<AAFwk::AbilityTransitionInfo> to) override;
    int32_t GetFocusWindow(sptr<IRemoteObject>& abilityToken) override;
};

namespace Detail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(t, t->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() {}

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
}

class WindowManagerService : public SystemAbility, public WindowManagerStub {
friend class DisplayChangeListener;
friend class WindowManagerServiceHandler;
DECLARE_SYSTEM_ABILITY(WindowManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManagerService);

public:
    void OnStart() override;
    void OnStop() override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;

    WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        uint32_t& windowId, sptr<IRemoteObject> token) override;
    WMError AddWindow(sptr<WindowProperty>& property) override;
    WMError RemoveWindow(uint32_t windowId) override;
    WMError NotifyWindowTransition(sptr<WindowTransitionInfo>& from, sptr<WindowTransitionInfo>& to) override;
    WMError DestroyWindow(uint32_t windowId, bool onlySelf = false) override;
    WMError RequestFocus(uint32_t windowId) override;
    WMError SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level) override;
    WMError SetAlpha(uint32_t windowId, float alpha) override;
    std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType) override;
    void ProcessPointDown(uint32_t windowId, bool isStartDrag) override;
    void ProcessPointUp(uint32_t windowId) override;
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) override;
    void MinimizeAllAppWindows(DisplayId displayId) override;
    void ToggleShownStateForAllAppWindows() override;
    WMError MaxmizeWindow(uint32_t windowId) override;
    WMError SetWindowLayoutMode(WindowLayoutMode mode) override;
    WMError UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action) override;
    WMError GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo) override;

    void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;

    WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller) override;
    WMError GetSystemDecorEnable(bool& isSystemDecorEnable) override;
    WMError GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones) override;

protected:
    WindowManagerService();
    virtual ~WindowManagerService() = default;

private:
    bool Init();
    void RegisterSnapshotHandler();
    void RegisterWindowManagerServiceHandler();
    void OnWindowEvent(Event event, uint32_t windowId);
    void NotifyDisplayStateChange(DisplayId id, DisplayStateChangeType type);
    WMError GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken);
    void ConfigureWindowManagerService();
    using Task = std::function<void()>;
    void HandleTasks();
    void PostTask(Task task);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

    static inline SingletonDelegator<WindowManagerService> delegator;
    std::recursive_mutex mutex_;
    sptr<WindowRoot> windowRoot_;
    sptr<WindowController> windowController_;
    sptr<InputWindowMonitor> inputWindowMonitor_;
    sptr<SnapshotController> snapshotController_;
    sptr<WindowManagerServiceHandler> wmsHandler_;
    sptr<DragController> dragController_;
    sptr<FreezeController> freezeDisplayController_;
    sptr<WindowDumper> windowDumper_;
    bool isSystemDecorEnable_ = true;
    ModeChangeHotZonesConfig hotZonesConfig_ { false, 0, 0, 0 };
    std::mutex mutex_thread_;
    std::condition_variable cv_;
    std::vector<WindowManagerService::Task> tasks_;
};
}
}
#endif // OHOS_WINDOW_MANAGER_SERVICE_H
