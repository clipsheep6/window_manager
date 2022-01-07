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

#include "foundation/windowmanager/interfaces/innerkits/wm/window_manager.h"
#include <algorithm>

#include "window_adapter.h"
#include "window_manager_agent.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowManager"};
}

class WindowManager::Impl {
public:
    void NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, int32_t displayId) const;
    void NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, int32_t displayId) const;

    static inline SingletonDelegator<WindowManager> delegator_;

    std::mutex mutex_;
    std::vector<sptr<IFocusChangedListener>> focusChangedListeners_;
    sptr<WindowManagerAgent> focusChangedListenerAgent_;
};

void WindowManager::Impl::NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, int32_t displayId) const
{
    WLOGFI("NotifyFocused [%{public}d; %{public}p; %{public}d; %{public}d]", windowId, abilityToken.GetRefPtr(),
        static_cast<uint32_t>(windowType), displayId);
    for (auto& listener : focusChangedListeners_) {
        listener->OnFocused(windowId, abilityToken, windowType, displayId);
    }
}

void WindowManager::Impl::NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, int32_t displayId) const
{
    WLOGFI("NotifyUnfocused [%{public}d; %{public}p; %{public}d; %{public}d]", windowId, abilityToken.GetRefPtr(),
        static_cast<uint32_t>(windowType), displayId);
    for (auto& listener : focusChangedListeners_) {
        listener->OnUnfocused(windowId, abilityToken, windowType, displayId);
    }
}

WindowManager::WindowManager() : pImpl_(std::make_unique<Impl>())
{
}

WindowManager::~WindowManager()
{
}

void WindowManager::RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }

    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    pImpl_->focusChangedListeners_.push_back(listener);
    if (pImpl_->focusChangedListenerAgent_ == nullptr) {
        pImpl_->focusChangedListenerAgent_ = new WindowManagerAgent();
        SingletonContainer::Get<WindowAdapter>().RegisterFocusChangedListener(pImpl_->focusChangedListenerAgent_);
    }
}

void WindowManager::UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return;
    }

    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->focusChangedListeners_.begin(), pImpl_->focusChangedListeners_.end(), listener);
    if (iter == pImpl_->focusChangedListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    pImpl_->focusChangedListeners_.erase(iter);
    if (pImpl_->focusChangedListeners_.empty() && pImpl_->focusChangedListenerAgent_ != nullptr) {
        SingletonContainer::Get<WindowAdapter>().UnregisterFocusChangedListener(pImpl_->focusChangedListenerAgent_);
    }
}

void WindowManager::UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
    int32_t displayId, bool focused) const
{
    WLOGFI("window focus status: %{public}d, id: %{public}d", focused, windowId);
    if (focused) {
        pImpl_->NotifyFocused(windowId, abilityToken, windowType, displayId);
    } else {
        pImpl_->NotifyUnfocused(windowId, abilityToken, windowType, displayId);
    }
}
} // namespace Rosen
} // namespace OHOS