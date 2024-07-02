/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_common_event.h"

#include <thread>
#include <want.h>
#include "common_event_subscribe_info.h"
#include "event_runner.h"
#include "skills.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "CommonEvent"};
    constexpr int RETRY_MAX_COUNT = 3;
    const std::string THREAD_ID = "WindowCommonEventHandler";
}

WindowCommonEvent::WindowCommonEvent()
{
    std::map<CommonEventAction, std::string> eventCodeMap_;
    eventCodeMap_ = {
        {EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED,
            WindowCommonEvent::CommonEventAction::COMMON_EVENT_USER_SWITCHED},
    };
    auto runner = AppExecFwk::EventRunner::Create(THREAD_ID);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

WindowCommonEvent::~WindowCommonEvent()
{
}

void WindowCommonEvent::SubscriberEvent()
{
    EventFwk::MatchingSkills skills;
    skills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    EventFwk::CommonEventSubscribeInfo info(skills);
    subscriber_ = std::make_shared<EventSubscriber>(info, shared_from_this());
    int retry = RETRY_MAX_COUNT;
    SubscriberEventInner(retry);
}

void WindowCommonEvent::SubscriberEventInner(int retry)
{
    if (retry <= 0) {
        return;
    }
    retry--;
    WLOGI("called action = %{public}d", retry);
    if (EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_)) {
        return;
    }
    std::function<void()> func = [this, retry]() { this->SubscriberEventInner(retry); };
    // post task delay 500ms
    eventHandler_->PostTask(func, "wms:SubscriberEventInner", 500, AppExecFwk::EventQueue::Priority::HIGH);
}

void WindowCommonEvent::UnSubscriberEvent()
{
    auto task = [this] {
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
    };
    eventHandler_->PostTask(task, "wms:UnSubscriberEvent", 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void WindowCommonEvent::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    WLOGI("receive common event, action = %{public}s", data.GetWant().GetAction().c_str());
    auto task = [this, data] {
        std::string action = data.GetWant().GetAction();
        WLOGI("called action = %{public}s", action.c_str());
        CommonEventAction eventAction = eventCodeMap_[action];
        switch (static_cast<int>(eventAction)) {
            case static_cast<int>(CommonEventAction::COMMON_EVENT_USER_SWITCHED):
                HandleAccountSwitched(data);
                break;
            default:
                break;
        }
    };
    eventHandler_->PostTask(task, "wms:OnReceiveEvent", 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void WindowCommonEvent::HandleAccountSwitched(const EventFwk::CommonEventData& data) const
{
    int accountId = data.GetCode();
    WLOGI("handle account switch, account id = %{public}d", accountId);
    WindowManagerService::GetInstance().OnAccountSwitched(accountId);
}
} // Rosen
} // OHOS