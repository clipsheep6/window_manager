/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "anr_handler.h"

#include <cinttypes>
#include <functional>
#include <string>

#include "window_manager_hilog.h"
#include "util.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRHandler" };
constexpr int64_t MAX_MARK_PROCESS_DELAY_TIME = 3500000;
constexpr int64_t MIN_MARK_PROCESS_DELAY_TIME = 50000;
constexpr int32_t INVALID_OR_PROCESSED_ID = -1;
constexpr int32_t TIME_TRANSITION = 1000;
const std::string ANR_HANDLER_RUNNER { "ANR_HANDLER" };
} // namespace

ANRHandler::ANRHandler() {
    auto runner = AppExecFwk::EventRunner::Create(ANR_HANDLER_RUNNER);
    if (runner == nullptr) {
        WLOGFE("Create eventRunner failed");
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

ANRHandler::~ANRHandler() {}

void ANRHandler::SetSessionStage(const wptr<ISessionStage> &sessionStage)
{
    sessionStage_ = sessionStage; 
}

void ANRHandler::SetLastProcessedEventStatus(bool status)
{
    std::lock_guard<std::mutex> guard(anrMtx_);
    event_.sendStatus = status;
}

void ANRHandler::UpdateLastProcessedEventId(int32_t eventId)
{
    std::lock_guard<std::mutex> guard(anrMtx_);
    event_.lastEventId = eventId;
}

void ANRHandler::SetLastProcessedEventId(int32_t eventId, int64_t actionTime)
{
    if (event_.lastEventId > eventId) {
        WLOGFE("Event id %{public}d less then last processed lastEventId %{public}d", eventId, event_.lastEventId);
        return;
    }
    UpdateLastProcessedEventId(eventId);

    int64_t currentTime = GetSysClockTime();
    int64_t timeoutTime = ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME - (currentTime - actionTime);
    WLOGFD("Processed id:%{public}d, actionTime:%{public}" PRId64 ", "
        "currentTime:%{public}" PRId64 ", timeoutTime:%{public}" PRId64, eventId, actionTime, currentTime, timeoutTime);

    if (!event_.sendStatus) {
        if (timeoutTime < MIN_MARK_PROCESS_DELAY_TIME) {
            SendEvent(0);
        } else {
            int64_t delayTime;
            if (timeoutTime >= MAX_MARK_PROCESS_DELAY_TIME) {
                delayTime = MAX_MARK_PROCESS_DELAY_TIME / TIME_TRANSITION;
            } else {
                delayTime = timeoutTime / TIME_TRANSITION;
            }
            SendEvent(delayTime);
        }
    }
}

int32_t ANRHandler::GetLastProcessedEventId()
{
    std::lock_guard<std::mutex> guard(anrMtx_);
    if (event_.lastEventId == INVALID_OR_PROCESSED_ID
        || event_.lastEventId <= event_.lastReportId) {
        WLOGFD("Invalid or processed, lastEventId:%{public}d, lastReportId:%{public}d", event_.lastEventId, event_.lastReportId);
        return INVALID_OR_PROCESSED_ID;
    }

    event_.lastReportId = event_.lastEventId;
    WLOGFD("Processed lastEventId:%{public}d, lastReportId:%{public}d", event_.lastEventId, event_.lastReportId);
    return event_.lastEventId;
}

void ANRHandler::MarkProcessed()
{
    int32_t eventId = GetLastProcessedEventId();
    if (eventId == INVALID_OR_PROCESSED_ID) {
        return;
    }
    WLOGFD("Processed event id:%{public}d", eventId);
    /**
        windowSessionImpl 就是 sessionStage
        SessionStage 中有 hostSession_
        hostSession 就是 SessionProxy
    */
    if (int32_t ret = 0;  ret = sessionStage_->MarkProcessed(eventId)) {
        WLOGFE("Send to scene board failed, ret:%{public}d", ret);
    }
    SetLastProcessedEventStatus(false);
}

void ANRHandler::SendEvent(int64_t delayTime)
{
    WLOGFD("Event delayTime:%{public}" PRId64, delayTime);
    SetLastProcessedEventStatus(true);
    if (eventHandler_ == nullptr) {
        WLOGFE("eventHandler is nullptr");
        return;
    }
    std::function<void()> eventFunc = std::bind(&ANRHandler::MarkProcessed, this);
    if (!eventHandler_->PostHighPriorityTask(eventFunc, delayTime)) {
        WLOGFE("Send dispatch event failed");
    }
}

void ANRHandler::ResetAnrArray()
{
    event_.sendStatus = false;
    event_.lastEventId = -1;
    event_.lastReportId = -1;
}
} // namespace Rosen
} // namespace OHOS