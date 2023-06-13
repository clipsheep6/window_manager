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

// #include "ability_manager_client.h"

#include "event_stage.h"
#include "proto.h"
#include "timer_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRManager" };
constexpr int32_t MAX_ANR_TIMER_COUNT = 50;
} // namespace

ANRManager::ANRManager() {}
ANRManager::~ANRManager() {}

void ANRManager::Init()
{
   // TODO
   // 该函数需要在SceneBoard启动的时候执行
   // 这里需要注册ANRManager::OnSessionLost 到每一个 session， 使得有session死亡之后执行此处的OnSessionLost
}

void ANRManager::AddTimer(int32_t id, int64_t currentTime, int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (anrTimerCount_ >= MAX_ANR_TIMER_COUNT) {
        WLOGFD("AddAnrTimer failed, anrTimerCount exceeded %{public}d", MAX_ANR_TIMER_COUNT);
        return;
    }
    int32_t timerId = TimerMgr->AddTimer(ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME, 1, [this, id, persistentId]() {
        EVStage->SetAnrStatus(persistentId, true);
        int32_t pid = GetPidByPersistentId(persistentId);
        WLOGFE("Application not responding. persistentId:%{public}d, eventId:%{public}d, applicationId:%{public}d",
            persistentId, id, pid);
        /**
         * anrObserver_->OnAnr(pid);
        */
        std::vector<int32_t> timerIds = EVStage->GetTimerIds(persistentId);
        for (int32_t item : timerIds) {
            if (item != -1) {
                TimerMgr->RemoveTimer(item);
                anrTimerCount_--;
                WLOGFD("Clear anr timer, timer id:%{public}d, count:%{public}d", item, anrTimerCount_);
            }
        }
    });
    anrTimerCount_++;
    EVStage->SaveANREvent(persistentId, id, currentTime, timerId);
}

void ANRManager::MarkProcessed(int32_t eventId, int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    WLOGFD("eventId:%{public}d, persistentId:%{public}d", eventId, persistentId);
    std::list<int32_t> timerIds = EVStage->DelEvents(persistentId, eventId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
            WLOGFD("Remove anr timer, eventId:%{public}d, timer id:%{public}d,"
                "count:%{public}d", eventId, item, anrTimerCount_);
        }
    }
}

bool ANRManager::IsANRTriggered(int64_t time, int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (EVStage->CheckAnrStatus(persistentId)) {
        WLOGFD("Application not responding. persistentId:%{public}d", persistentId);
        return true;
    }
    WLOGFD("Event dispatch normal");
    return false;
}

void ANRManager::RemoveTimers(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    std::vector<int32_t> timerIds = EVStage->GetTimerIds(persistentId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
        }
    }
}

void ANRManager::OnSessionLost(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    RemoveTimers(persistentId);
}

void ANRManager::SetApplicationPid(int32_t persistentId, int32_t applicationPid)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    applicationMap_[persistentId] = applicationPid;
}

int32_t ANRManager::GetPidByPersistentId(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (applicationMap_.find(persistentId) != applicationMap_.end()) {
        return applicationMap_[persistentId];
    }
    WLOGFE("No application matches persistentId:%{public}d", persistentId);
    return -1;
}

// void ANRManager::SetAnrObserver(sptr<IAnrObserver> observer)
// {
//     CALL_DEBUG_ENTER;
//     std::lock_guard<std::mutex> guard(mtx_);
//     anrObserver_ = observer;
// }
} // namespace Rosen
} // namespace OHOS