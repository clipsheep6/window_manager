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

#include "timer_manager.h"

#include <algorithm>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MIN_DELAY = -1;
constexpr int32_t MIN_INTERVAL = 50;
constexpr int32_t MAX_INTERVAL_MS = 10000;
constexpr int32_t MAX_TIMER_COUNT = 64;
constexpr int32_t NONEXISTENT_ID = -1;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "TimerManager" };
} // namespace

TimerManager::TimerManager() {}
TimerManager::~TimerManager() {}

int32_t TimerManager::AddTimer(int32_t intervalMs, int32_t repeatCount, std::function<void()> callback)
{
    CALL_DEBUG_ENTER;
    return AddTimerInternal(intervalMs, repeatCount, callback);
}

int32_t TimerManager::RemoveTimer(int32_t timerId)
{
    CALL_DEBUG_ENTER;
    return RemoveTimerInternal(timerId);
}

int32_t TimerManager::ResetTimer(int32_t timerId)
{
    CALL_DEBUG_ENTER;
    return ResetTimerInternal(timerId);
}

bool TimerManager::IsExist(int32_t timerId)
{
    CALL_DEBUG_ENTER;
    return IsExistInternal(timerId);
}

int32_t TimerManager::CalcNextDelay()
{
    CALL_DEBUG_ENTER;
    return CalcNextDelayInternal();
}

void TimerManager::ProcessTimers()
{
    CALL_DEBUG_ENTER;
    ProcessTimersInternal();
}

int32_t TimerManager::TakeNextTimerId()
{
    CALL_DEBUG_ENTER;
    uint64_t timerSlot = 0;
    uint64_t one = 1;

    for (const auto &timer : timers_) {
        timerSlot |= (one << timer->id);
    }

    for (int32_t i = 0; i < MAX_TIMER_COUNT; i++) {
        if ((timerSlot & (one << i)) == 0) {
            return i;
        }
    }
    return NONEXISTENT_ID;
}

int32_t TimerManager::AddTimerInternal(int32_t intervalMs, int32_t repeatCount, std::function<void()> callback)
{
    CALL_DEBUG_ENTER;
    if (intervalMs < MIN_INTERVAL) {
        intervalMs = MIN_INTERVAL;
    } else if (intervalMs > MAX_INTERVAL_MS) {
        intervalMs = MAX_INTERVAL_MS;
    }
    if (!callback) {
        return NONEXISTENT_ID;
    }
    int32_t timerId = TakeNextTimerId();
    if (timerId < 0) {
        return NONEXISTENT_ID;
    }
    auto timer = std::make_unique<TimerItem>();
    timer->id = timerId;
    timer->intervalMs = intervalMs;
    timer->repeatCount = repeatCount;
    timer->callbackCount = 0;
    auto nowTime = GetMillisTime();
    if (!AddInt64(nowTime, timer->intervalMs, timer->nextCallTime)) {
        WLOGFE("The addition of nextCallTime in TimerItem overflows");
        return NONEXISTENT_ID;
    }
    timer->callback = callback;
    InsertTimerInternal(timer);
    return timerId;
}

int32_t TimerManager::RemoveTimerInternal(int32_t timerId)
{
    CALL_DEBUG_ENTER;
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if ((*it)->id == timerId) {
            timers_.erase(it);
            return 0;
        }
    }
    return -1;
}

int32_t TimerManager::ResetTimerInternal(int32_t timerId)
{
    CALL_DEBUG_ENTER;
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if ((*it)->id == timerId) {
            auto timer = std::move(*it);
            timers_.erase(it);
            auto nowTime = GetMillisTime();
            if (!AddInt64(nowTime, timer->intervalMs, timer->nextCallTime)) {
                WLOGFE("The addition of nextCallTime in TimerItem overflows");
                return -1;
            }
            timer->callbackCount = 0;
            InsertTimerInternal(timer);
            return 0;
        }
    }
    return -1;
}

bool TimerManager::IsExistInternal(int32_t timerId)
{
    CALL_DEBUG_ENTER;
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if ((*it)->id == timerId) {
            return true;
        }
    }
    return false;
}

void TimerManager::InsertTimerInternal(std::unique_ptr<TimerItem>& timer)
{
    CALL_DEBUG_ENTER;
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if ((*it)->nextCallTime > timer->nextCallTime) {
            timers_.insert(it, std::move(timer));
            return;
        }
    }
    timers_.push_back(std::move(timer));
}

int32_t TimerManager::CalcNextDelayInternal()
{
    CALL_DEBUG_ENTER;
    auto delay = MIN_DELAY;
    if (!timers_.empty()) {
        auto nowTime = GetMillisTime();
        const auto& item = *timers_.begin();
        if (nowTime >= item->nextCallTime) {
            delay = 0;
        } else {
            delay = item->nextCallTime - nowTime;
        }
    }
    return delay;
}

void TimerManager::ProcessTimersInternal()
{
    CALL_DEBUG_ENTER;
    if (timers_.empty()) {
        return;
    }
    auto nowTime = GetMillisTime();
    for (;;) {
        auto it = timers_.begin();
        if (it == timers_.end()) {
            break;
        }
        if ((*it)->nextCallTime > nowTime) {
            break;
        }
        auto curTimer = std::move(*it);
        timers_.erase(it);
        ++curTimer->callbackCount;
        if ((curTimer->repeatCount >= 1) && (curTimer->callbackCount >= curTimer->repeatCount)) {
            curTimer->callback();
            continue;
        }
        if (!AddInt64(curTimer->nextCallTime, curTimer->intervalMs, curTimer->nextCallTime)) {
            WLOGFE("The addition of nextCallTime in TimerItem overflows");
            return;
        }
        auto callback = curTimer->callback;
        InsertTimerInternal(curTimer);
        callback();
    }
}
} // namespace Rosen
} // namespace OHOS
