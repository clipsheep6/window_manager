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
using WI = WindowImpl;
using namespace std;

inline NotifyNativeWinDestroyFunc WI::GetNativeDestroyCallback()
{
    return notifyNativefunc_;
}

inline void WI::SetWindowState(WindowState state)
{
    state_ = state;
}

template<typename T>
inline WI::EnableIfSame<T, IWindowLifeCycle, vector<sptr<IWindowLifeCycle>>> WI::GetListeners()
{
    vector<sptr<IWindowLifeCycle>> lifecycleListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : lifecycleListeners_[GetWindowId()]) {
            lifecycleListeners.push_back(listener);
        }
    }
    return lifecycleListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IWindowChangeListener, vector<sptr<IWindowChangeListener>>> WI::GetListeners()
{
    vector<sptr<IWindowChangeListener>> windowChangeListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : windowChangeListeners_[GetWindowId()]) {
            windowChangeListeners.push_back(listener);
        }
    }
    return windowChangeListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IAvoidAreaChangedListener, vector<sptr<IAvoidAreaChangedListener>>> WI::GetListeners()
{
    vector<sptr<IAvoidAreaChangedListener>> avoidAreaChangeListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : avoidAreaChangeListeners_[GetWindowId()]) {
            avoidAreaChangeListeners.push_back(listener);
        }
    }
    return avoidAreaChangeListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IDisplayMoveListener, vector<sptr<IDisplayMoveListener>>> WI::GetListeners()
{
    vector<sptr<IDisplayMoveListener>> displayMoveListeners;
    {
        lock_guard<recursive_mutex> lock(mutex_);
        for (auto& listener : displayMoveListeners_) {
            displayMoveListeners.push_back(listener);
        }
    }
    return displayMoveListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IScreenshotListener, vector<sptr<IScreenshotListener>>> WI::GetListeners()
{
    vector<sptr<IScreenshotListener>> screenshotListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : screenshotListeners_[GetWindowId()]) {
            screenshotListeners.push_back(listener);
        }
    }
    return screenshotListeners;
}

template<typename T>
inline WI::EnableIfSame<T, ITouchOutsideListener, vector<sptr<ITouchOutsideListener>>> WI::GetListeners()
{
    vector<sptr<ITouchOutsideListener>> touchOutsideListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : touchOutsideListeners_[GetWindowId()]) {
            touchOutsideListeners.push_back(listener);
        }
    }
    return touchOutsideListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IDialogTargetTouchListener, vector<sptr<IDialogTargetTouchListener>>> WI::GetListeners()
{
    vector<sptr<IDialogTargetTouchListener>> dialogTargetTouchListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : dialogTargetTouchListeners_[GetWindowId()]) {
            dialogTargetTouchListeners.push_back(listener);
        }
    }
    return dialogTargetTouchListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IWindowDragListener, vector<sptr<IWindowDragListener>>> WI::GetListeners()
{
    vector<sptr<IWindowDragListener>> windowDragListeners;
    {
        lock_guard<recursive_mutex> lock(mutex_);
        for (auto& listener : windowDragListeners_) {
            windowDragListeners.push_back(listener);
        }
    }
    return windowDragListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IOccupiedAreaChangeListener, vector<sptr<IOccupiedAreaChangeListener>>> WI::GetListeners()
{
    vector<sptr<IOccupiedAreaChangeListener>> occupiedAreaChangeListeners;
    {
        lock_guard<recursive_mutex> lock(globalMutex_);
        for (auto& listener : occupiedAreaChangeListeners_[GetWindowId()]) {
            occupiedAreaChangeListeners.push_back(listener);
        }
    }
    return occupiedAreaChangeListeners;
}

template<typename T>
inline WI::EnableIfSame<T, IDialogDeathRecipientListener, wptr<IDialogDeathRecipientListener>> WI::GetListener()
{
    lock_guard<recursive_mutex> lock(globalMutex_);
    return dialogDeathRecipientListener_[GetWindowId()];
}

inline void WI::NotifyAfterForeground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterForeground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Foreground);
    }
}

inline void WI::NotifyAfterBackground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Background);
    }
}

inline void WI::NotifyAfterFocused()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
    CALL_UI_CONTENT(Focus);
}

inline void WI::NotifyAfterUnfocused(bool needNotifyUiContent)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    // use needNotifyUinContent to separate ui content callbacks
    CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(UnFocus);
    }
}

inline void WI::NotifyAfterResumed()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterResumed, lifecycleListeners);
}

inline void WI::NotifyAfterPaused()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterPaused, lifecycleListeners);
}

inline void WI::NotifyBeforeDestroy(string windowName)
{
    lock_guard<recursive_mutex> lock(mutex_);
    if (uiContent_ != nullptr) {
        auto uiContent = move(uiContent_);
        uiContent_ = nullptr;
        uiContent->Destroy();
    }
    if (notifyNativefunc_) {
        notifyNativefunc_(windowName);
    }
}

inline void WI::NotifyBeforeSubWindowDestroy(sptr<WI> window)
{
    auto uiContent = window->GetUIContent();
    if (uiContent != nullptr) {
        uiContent->Destroy();
    }
    if (window->GetNativeDestroyCallback()) {
        window->GetNativeDestroyCallback()(window->GetWindowName());
    }
}

inline void WI::NotifyAfterActive()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterActive, lifecycleListeners);
}

inline void WI::NotifyAfterInactive()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterInactive, lifecycleListeners);
}

inline void WI::NotifyForegroundFailed(WMError ret)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(ForegroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

inline void WI::NotifyBackgroundFailed(WMError ret)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(BackgroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

inline bool WI::IsStretchableReason(WindowSizeChangeReason reason)
{
    return reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::DRAG_END ||
        reason == WindowSizeChangeReason::DRAG_START || reason == WindowSizeChangeReason::RECOVER ||
        reason == WindowSizeChangeReason::MOVE || reason == WindowSizeChangeReason::UNDEFINED;
}
}
}