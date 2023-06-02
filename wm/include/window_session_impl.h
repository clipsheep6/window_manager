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

#ifndef OHOS_ROSEN_WINDOW_SESSION_IMPL_H
#define OHOS_ROSEN_WINDOW_SESSION_IMPL_H

#include <ability_context.h>
#include <refbase.h>
#include <ui_content.h>
#include <ui/rs_surface_node.h>

#include "common/include/window_session_property.h"
#include "session/container/include/zidl/session_stage_stub.h"
#include "session/host/include/zidl/session_interface.h"
#include "window.h"
#include "window_option.h"

namespace OHOS {
namespace Rosen {
namespace {
template<typename T1, typename T2, typename Ret>
using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
}
class WindowSessionImpl : public Window, virtual public SessionStageStub {
public:
    explicit WindowSessionImpl(const sptr<WindowOption>& option);
    ~WindowSessionImpl();
    // inherits from window
    virtual WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession);
    WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    WMError Hide(uint32_t reason = 0, bool withAnimation = false, bool isFromInnerkits = true) override;
    WMError Destroy() override;
    virtual WMError Destroy(bool needClearListener);
    WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isdistributed, AppExecFwk::Ability* ability) override;
    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const override;
    const std::shared_ptr<AbilityRuntime::Context> GetContext() const override;
    Rect GetRequestRect() const override;
    WindowType GetType() const override;
    const std::string& GetWindowName() const override;
    WindowState GetWindowState() const override;
    WMError SetFocusable(bool isFocusable) override;
    WMError SetTouchable(bool isTouchable) override;
    WMError SetWindowType(WindowType type) override;
    bool GetTouchable() const override;
    uint32_t GetWindowId() const override;
    Rect GetRect() const override;
    bool GetFocusable() const override;
    std::string GetContentInfo() override;
    Ace::UIContent* GetUIContent() const override;
    void OnNewWant(const AAFwk::Want& want) override;
    void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) override;
    // inherits from session stage
    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;
    void NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    void NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    // callback
    WMError RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    WMError UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener) override;
    WMError RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    WMError UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener) override;
    void RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func) override;
    uint32_t GetParentId() const;
    uint64_t GetPersistentId() const;
    sptr<WindowSessionProperty> GetProperty() const;
    sptr<ISession> GetHostSession() const;
    void NotifyAfterForeground(bool needNotifyListeners = true, bool needNotifyUiContent = true);
    void NotifyAfterBackground(bool needNotifyListeners = true, bool needNotifyUiContent = true);
    void NotifyForegroundFailed(WMError ret);

    WindowState state_ { WindowState::STATE_INITIAL };

protected:
    WMError Connect();
    bool IsWindowSessionInvalid() const;
    void NotifyAfterActive();
    void NotifyAfterInactive();
    void NotifyBeforeDestroy(std::string windowName);
    void ClearListenersById(uint64_t persistentId);
    WMError WindowSessionCreateCheck();
    void UpdateDecorEnable(bool needNotify = false);
    void NotifyModeChange(WindowMode mode, bool hasDeco = true);

    std::unique_ptr<Ace::UIContent> uiContent_ = nullptr;
    sptr<ISession> hostSession_ = nullptr;
    std::shared_ptr<AbilityRuntime::Context> context_ = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    sptr<WindowSessionProperty> property_ = nullptr;
    // map of windowSession: <sessionName, <persistentId, windowSession>>
    static std::map<std::string, std::pair<uint64_t, sptr<WindowSessionImpl>>> windowSessionMap_;
    // map of subSession: <persistentId, std::vector<windowSession>>
    static std::map<uint64_t, std::vector<sptr<WindowSessionImpl>>> subWindowSessionMap_;
    std::recursive_mutex mutex_;
    WindowMode windowMode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    SystemSessionConfig windowSystemConfig_ ;

private:
    template<typename T> WMError RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> WMError UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> EnableIfSame<T, IWindowLifeCycle, std::vector<sptr<IWindowLifeCycle>>> GetListeners();
    template<typename T>
    EnableIfSame<T, IWindowChangeListener, std::vector<sptr<IWindowChangeListener>>> GetListeners();
    template<typename T> void ClearUselessListeners(std::map<uint64_t, T>& listeners, uint64_t persistentId);
    RSSurfaceNode::SharedPtr CreateSurfaceNode(std::string name, WindowType type);
    void NotifyAfterFocused();
    void NotifyAfterUnfocused(bool needNotifyUiContent = true);
    void UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason);
    void NotifySizeChange(Rect rect, WindowSizeChangeReason reason);

    static std::map<uint64_t, std::vector<sptr<IWindowLifeCycle>>> lifecycleListeners_;
    static std::map<uint64_t, std::vector<sptr<IWindowChangeListener>>> windowChangeListeners_;
    static std::recursive_mutex globalMutex_;
    NotifyNativeWinDestroyFunc notifyNativefunc_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SESSION_IMPL_H