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
#include "anr_manager.h"

#include <algorithm>
#include <vector>

#include "dfx_hisysevent.h"
#include "entrance_log.h"
#include "proto.h"
#include "timer_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRManager" };
constexpr int32_t MAX_ANR_TIMER_COUNT = 64;
} // namespace

ANRManager::ANRManager() {}
ANRManager::~ANRManager() {}

void ANRManager::Init()
{
    CALL_DEBUG_ENTER;
    TimerMgr->Init();
}

void ANRManager::AddTimer(int32_t id, int64_t currentTime, uint32_t persistentId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (anrTimerCount_ >= MAX_ANR_TIMER_COUNT) {
        WLOGFD("AddAnrTimer failed, anrTimerCount exceeded %{public}d", MAX_ANR_TIMER_COUNT);
        return;
    }
    int32_t timerId = TimerMgr->AddTimer(ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME, [this, id, persistentId]() {
        WLOGFD("Anr callback enter. persistentId:%{public}" PRIu32 ", eventId:%{public}d", persistentId, id);
        eventStage_.SetAnrStatus(persistentId, true);
        int32_t pid = GetPidByPersistentId(persistentId);
        DfxHisysevent::ApplicationBlockInput(id, pid, persistentId);
        WLOGFE("Application not responding. persistentId:%{public}" PRIu32", eventId:%{public}d, pid:%{public}d",
            persistentId, id, pid);
        if (anrObserver_ != nullptr) {
            anrObserver_(pid);
        } else {
            WLOGFE("anrObserver is nullptr, do nothing");
        }
        std::vector<int32_t> timerIds = eventStage_.GetTimerIds(persistentId);
        for (int32_t item : timerIds) {
            if (item != -1) {
                TimerMgr->RemoveTimer(item);
                anrTimerCount_--;
            }
        }
        WLOGFD("Anr callback leave. persistentId:%{public}" PRIu32 ", eventId:%{public}d", persistentId, id);
    });
    anrTimerCount_++;
    eventStage_.SaveANREvent(persistentId, id, currentTime, timerId);
}

void ANRManager::MarkProcessed(int32_t eventId, uint32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    WLOGFD("eventId:%{public}d, persistentId:%{public}" PRIu32"", eventId, persistentId);
    std::list<int32_t> timerIds = eventStage_.DelEvents(persistentId, eventId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
        }
    }
}

bool ANRManager::IsANRTriggered(int64_t time, uint32_t persistentId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (eventStage_.CheckAnrStatus(persistentId)) {
        WLOGFD("Application not responding. persistentId:%{public}" PRIu32 "", persistentId);
        return true;
    }
    return false;
}

void ANRManager::OnSessionLost(uint32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    RemoveTimers(persistentId);
    RemovePersistentId(persistentId);
}

void ANRManager::SetApplicationPid(uint32_t persistentId, int32_t applicationPid)
{
    std::lock_guard<std::mutex> guard(mtx_);
    WLOGFD("persistentId:%{public}" PRIu32 " -> applicationPid:%{public}d", persistentId, applicationPid);
    applicationMap_[persistentId] = applicationPid;
}

void ANRManager::SetAnrObserver(std::function<void(int32_t)> anrObserver)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    anrObserver_ = anrObserver;
}

int32_t ANRManager::GetPidByPersistentId(uint32_t persistentId)
{
    if (applicationMap_.find(persistentId) != applicationMap_.end()) {
        WLOGFD("persistentId:%{public}" PRIu32 " -> pid:%{public}d", persistentId, applicationMap_[persistentId]);
        return applicationMap_[persistentId];
    }
    WLOGFD("No application matches persistentId:%{public}" PRIu32 "", persistentId);
    return -1;
}

void ANRManager::RemoveTimers(uint32_t persistentId)
{
    std::vector<int32_t> timerIds = eventStage_.GetTimerIds(persistentId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
        }
    }
}

void ANRManager::RemovePersistentId(uint32_t persistentId)
{
    if (applicationMap_.find(persistentId) == applicationMap_.end()) {
        WLOGFD("No persistentId:%{public}" PRIu32 " in applicationMap", persistentId);
        return;
    }
    WLOGFD("Remove persistentId:%{public}" PRIu32 " -> applicationPid:%{public}d",
        persistentId, applicationMap_[persistentId]);
    applicationMap_.erase(persistentId);
}
} // namespace Rosen
} // namespace OHOS