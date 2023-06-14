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

#include "event_stage.h"

#include <algorithm>

#include "proto.h"
#include "util.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "EventStage" };
} // namespace

EventStage::EventStage() {}
EventStage::~EventStage() {}

void EventStage::SetAnrStatus(int32_t persistentId, bool status)
{
    CALL_DEBUG_ENTER;
    isAnrProcess_[persistentId] = status;
}

bool EventStage::CheckAnrStatus(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    if (isAnrProcess_.find(persistentId) != isAnrProcess_.end()) {
        return isAnrProcess_[persistentId];
    }
    WLOGFD("Current persistentId:%{public}d is not in event stage", persistentId);
    return false;
}

void EventStage::SaveANREvent(int32_t persistentId, int32_t id, int64_t time, int32_t timerId)
{
    CALL_DEBUG_ENTER;
    WLOGFD("Current persistentId:%{public}d, eventId:%{public}d, timerId:%{public}d", persistentId, id, timerId);
    EventTime eventTime { id, time, timerId };
    if (events_.find(persistentId) != events_.end()) {
        events_[persistentId].push_back(eventTime);
    }
}

std::vector<int32_t> EventStage::GetTimerIds(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::vector<int32_t> timers;
    if (events_.find(persistentId) != events_.end()) {
        for (auto &item : events_[persistentId]) {
            timers.push_back(item.timerId);
            item.timerId = -1;
        }
        return timers;
    }
    WLOGFD("Current persistentId:%{public}d have no event", persistentId);
    return timers;
}

std::list<int32_t> EventStage::DelEvents(int32_t persistentId, int32_t id)
{
    CALL_DEBUG_ENTER;
    WLOGFD("Delete events, persistentId:%{public}d, id:%{public}d", persistentId, id);
    if (events_.find(persistentId) == events_.end()) {
        WLOGFE("Current events have no event persistentId:%{public}d", persistentId);
        return {};
    }
    auto &events = events_[persistentId];
    auto fistMatchIter = find_if(events.begin(), events.end(), [id](const auto &item){
        return item.id > id;
    });
    if (fistMatchIter == events.end()) {
        WLOGFW("Can not find event:%{public}d", id);
        return { };
    }
    std::list<int32_t> timerIds;
    for (auto iter = events.begin(); iter != fistMatchIter; iter++) {
        timerIds.push_back(iter->timerId);
    }
    events.erase(events.begin(), fistMatchIter);
    if (events.empty()) {
        SetAnrStatus(persistentId, false);
        return timerIds;
    }
    int64_t endTime = 0;
    if (!AddInt64(events.begin()->eventTime, ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME, endTime)) {
        WLOGFE("The addition of endTime overflows");
        return timerIds;
    }
    auto currentTime = GetSysClockTime();
    if (currentTime < endTime) {
        SetAnrStatus(persistentId, false);
    }
    return timerIds;
}

} // namespace MMI
} // namespace OHOS
