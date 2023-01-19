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

#include "session_manager/screen/include/screen_session_manager.h"

#include <memory>

#include "utils/include/window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManager"};
    const std::string SCREEN_SESSION_MANAGER_THREAD = "ScreenSessionManager";
}

ScreenSessionManager& ScreenSessionManager::GetInstance()
{
    static ScreenSessionManager screenSessionManager;
    return screenSessionManager;
}

ScreenSessionManager::ScreenSessionManager()
    : rsInterface_(RSInterfaces::GetInstance())
{
    Init();
}

void ScreenSessionManager::RegisterScreenConnectionCallback(const ScreenConnectionCallback& screenConnectionCallback)
{
    if (screenConnectionCallback == nullptr) {
        WLOGFE("Failed to register screen connection callback, callback is null!");
        return;
    }

    if (screenConnectionCallback_ != nullptr) {
        WLOGFE("Repeat to register screen connection callback!");
        return;
    }

    screenConnectionCallback_ = screenConnectionCallback;
    for (auto sessionIt : screenSessionMap_) {
        screenConnectionCallback_(sessionIt.second);
    }
}

void ScreenSessionManager::Init()
{
    WLOGFI("Screen session manager init.");
    auto runner = AppExecFwk::EventRunner::Create(SCREEN_SESSION_MANAGER_THREAD);
    handler_ = std::make_unique<AppExecFwk::EventHandler>(runner);
    RegisterScreenChangeListener();
}

void ScreenSessionManager::PostTask(AppExecFwk::EventHandler::Callback callback, int64_t delayTime)
{
    if (handler_ == nullptr) {
        WLOGFE("Failed to post task, handler is null!");
        return;
    }

    handler_->PostTask(callback, delayTime, AppExecFwk::EventQueue::Priority::HIGH);
}

void ScreenSessionManager::RegisterScreenChangeListener()
{
    WLOGFD("Register screen change listener.");
    wptr<ScreenSessionManager> weakScreenSessionManager = this;
    auto res = rsInterface_.SetScreenChangeCallback(
        [weakScreenSessionManager](ScreenId screenId, ScreenEvent screenEvent) {
            auto screenSessionManager = weakScreenSessionManager.promote();
            if (screenSessionManager == nullptr) {
                WLOGFE("Failed to set screen change callback, screen session manager is null!");
                return;
            }

            auto task = [weakScreenSessionManager, screenId, screenEvent] {
                auto screenSessionManager = weakScreenSessionManager.promote();
                if (screenSessionManager != nullptr) {
                    screenSessionManager->OnScreenChange(screenId, screenEvent);
                }
            };

            screenSessionManager->PostTask(task);
        }
    );

    if (res != StatusCode::SUCCESS) {
        auto task = [weakScreenSessionManager] {
            auto screenSessionManager = weakScreenSessionManager.promote();
            if (screenSessionManager == nullptr) {
                WLOGFE("Failed to register screen change listener, screen session manager is null!");
                return;
            }

            screenSessionManager->RegisterScreenChangeListener();
        };

        // Retry after 50 ms.
        PostTask(task, 50);
    }
}

void ScreenSessionManager::OnScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    WLOGFI("On screen change. id:%{public}" PRIu64", event:%{public}u", screenId, static_cast<uint32_t>(screenEvent));
    auto screenSession = GetOrCreateScreenSession(screenId);

    if (screenEvent == ScreenEvent::CONNECTED) {
        if (screenConnectionCallback_ != nullptr) {
            screenConnectionCallback_(screenSession);
        }
        screenSession->Connect();
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        screenSessionMap_.erase(screenId);
        screenSession->Disconnect();
    } else {
        WLOGE("Unknown message:%{public}ud", static_cast<uint8_t>(screenEvent));
    }
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    auto sessionIt = screenSessionMap_.find(screenId);
    if (sessionIt != screenSessionMap_.end()) {
        return sessionIt->second;
    }

    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight()}, 0.0f, 0.0f);
    ScreenProperty property;
    property.SetRotation(0.0f);
    property.SetBounds(screenBounds);
    sptr<ScreenSession> session = new ScreenSession(screenId, property);
    screenSessionMap_[screenId] = session;
    return session;
}
} // namespace OHOS::Rosen
