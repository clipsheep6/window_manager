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

#include "window_impl.h"

namespace OHOS {
namespace Rosen {
inline NotifyNativeWinDestroyFunc WindowImpl::GetNativeDestroyCallback()
{
    return notifyNativefunc_;
}

inline void WindowImpl::SetWindowState(WindowState state)
{
    state_ = state;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IWindowLifeCycle,
                                std::vector<sptr<IWindowLifeCycle>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IWindowLifeCycle>> lifecycleListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : lifecycleListeners_[GetWindowId()]) {
            lifecycleListeners.push_back(listener);
        }
    }
    return lifecycleListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IWindowChangeListener,
                                std::vector<sptr<IWindowChangeListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IWindowChangeListener>> windowChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : windowChangeListeners_[GetWindowId()]) {
            windowChangeListeners.push_back(listener);
        }
    }
    return windowChangeListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IAvoidAreaChangedListener,
                                std::vector<sptr<IAvoidAreaChangedListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IAvoidAreaChangedListener>> avoidAreaChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : avoidAreaChangeListeners_[GetWindowId()]) {
            avoidAreaChangeListeners.push_back(listener);
        }
    }
    return avoidAreaChangeListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IDisplayMoveListener, 
                                std::vector<sptr<IDisplayMoveListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IDisplayMoveListener>> displayMoveListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (auto& listener : displayMoveListeners_) {
            displayMoveListeners.push_back(listener);
        }
    }
    return displayMoveListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IScreenshotListener,
                                std::vector<sptr<IScreenshotListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IScreenshotListener>> screenshotListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : screenshotListeners_[GetWindowId()]) {
            screenshotListeners.push_back(listener);
        }
    }
    return screenshotListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, ITouchOutsideListener,
                                std::vector<sptr<ITouchOutsideListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<ITouchOutsideListener>> touchOutsideListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : touchOutsideListeners_[GetWindowId()]) {
            touchOutsideListeners.push_back(listener);
        }
    }
    return touchOutsideListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IDialogTargetTouchListener,
                                std::vector<sptr<IDialogTargetTouchListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IDialogTargetTouchListener>> dialogTargetTouchListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : dialogTargetTouchListeners_[GetWindowId()]) {
            dialogTargetTouchListeners.push_back(listener);
        }
    }
    return dialogTargetTouchListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IWindowDragListener,
                                std::vector<sptr<IWindowDragListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IWindowDragListener>> windowDragListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        for (auto& listener : windowDragListeners_) {
            windowDragListeners.push_back(listener);
        }
    }
    return windowDragListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IOccupiedAreaChangeListener, 
                                std::vector<sptr<IOccupiedAreaChangeListener>>> WindowImpl::GetListeners()
{
    std::vector<sptr<IOccupiedAreaChangeListener>> occupiedAreaChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : occupiedAreaChangeListeners_[GetWindowId()]) {
            occupiedAreaChangeListeners.push_back(listener);
        }
    }
    return occupiedAreaChangeListeners;
}

template<typename T>
inline WindowImpl::EnableIfSame<T, IDialogDeathRecipientListener,
                                wptr<IDialogDeathRecipientListener>> WindowImpl::GetListener()
{
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return dialogDeathRecipientListener_[GetWindowId()];
}

inline void WindowImpl::NotifyAfterForeground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterForeground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Foreground);
    }
}

inline void WindowImpl::NotifyAfterBackground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Background);
    }
}

inline void WindowImpl::NotifyAfterFocused()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
    CALL_UI_CONTENT(Focus);
}

inline void WindowImpl::NotifyAfterUnfocused(bool needNotifyUiContent)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    // use needNotifyUinContent to separate ui content callbacks
    CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(UnFocus);
    }
}

inline void WindowImpl::NotifyAfterResumed()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterResumed, lifecycleListeners);
}

inline void WindowImpl::NotifyAfterPaused()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterPaused, lifecycleListeners);
}

inline void WindowImpl::NotifyBeforeDestroy(std::string windowName)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ != nullptr) {
        auto uiContent = std::move(uiContent_);
        uiContent_ = nullptr;
        uiContent->Destroy();
    }
    if (notifyNativefunc_) {
        notifyNativefunc_(windowName);
    }
}

inline void WindowImpl::NotifyBeforeSubWindowDestroy(sptr<WindowImpl> window)
{
    auto uiContent = window->GetUIContent();
    if (uiContent != nullptr) {
        uiContent->Destroy();
    }
    if (window->GetNativeDestroyCallback()) {
        window->GetNativeDestroyCallback()(window->GetWindowName());
    }
}

inline void WindowImpl::NotifyAfterActive()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterActive, lifecycleListeners);
}

inline void WindowImpl::NotifyAfterInactive()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterInactive, lifecycleListeners);
}

inline void WindowImpl::NotifyForegroundFailed(WMError ret)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(ForegroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

inline void WindowImpl::NotifyBackgroundFailed(WMError ret)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(BackgroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

inline bool WindowImpl::IsStretchableReason(WindowSizeChangeReason reason)
{
    return reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::DRAG_END ||
        reason == WindowSizeChangeReason::DRAG_START || reason == WindowSizeChangeReason::RECOVER ||
        reason == WindowSizeChangeReason::MOVE || reason == WindowSizeChangeReason::UNDEFINED;
}
}
}