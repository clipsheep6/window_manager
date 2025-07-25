/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include <hisysevent.h>
#include <chrono>

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_state_internel.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {

SensorFoldStateManager::SensorFoldStateManager() = default;
SensorFoldStateManager::~SensorFoldStateManager() = default;

void SensorFoldStateManager::HandleAngleChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy) {}

void SensorFoldStateManager::HandleHallChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy) {}

void SensorFoldStateManager::HandleAngleOrHallChange(const std::vector<float> &angles,
    const std::vector<uint16_t> &halls, sptr<FoldScreenPolicy> foldScreenPolicy, bool isPostureRegistered) {}

void SensorFoldStateManager::HandleTentChange(int tentType, sptr<FoldScreenPolicy> foldScreenPolicy, int32_t hall) {}

void SensorFoldStateManager::HandleSensorChange(FoldStatus nextState, float angle,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (nextState == FoldStatus::UNKNOWN) {
        TLOGW(WmsLogTag::DMS, "fold state is UNKNOWN");
        return;
    }
    if (mState_ == nextState) {
        TLOGD(WmsLogTag::DMS, "fold state doesn't change, foldState = %{public}d.", mState_);
        return;
    }
    TLOGI(WmsLogTag::DMS, "current state: %{public}d, next state: %{public}d.", mState_, nextState);
    ReportNotifyFoldStatusChange(static_cast<int32_t>(mState_), static_cast<int32_t>(nextState), angle);
    PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();

    NotifyReportFoldStatusToScb(mState_, nextState, angle);

    mState_ = nextState;
    if (foldScreenPolicy != nullptr) {
        foldScreenPolicy->SetFoldStatus(mState_);
    }
    ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(mState_);
    if (foldScreenPolicy != nullptr && foldScreenPolicy->lockDisplayStatus_ != true) {
        foldScreenPolicy->SendSensorResult(mState_);
    }
}

void SensorFoldStateManager::HandleSensorChange(FoldStatus nextState, const std::vector<float> &angles,
    const std::vector<uint16_t>& halls, sptr<FoldScreenPolicy> foldScreenPolicy)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (nextState == FoldStatus::UNKNOWN) {
        TLOGW(WmsLogTag::DMS, "fold state is UNKNOWN");
        return;
    }
    if (mState_ == nextState) {
        TLOGD(WmsLogTag::DMS, "fold state doesn't change, foldState = %{public}d.", mState_);
        return;
    }
    {
        std::unique_lock<std::mutex> lock(oneStepMutex_);
        if (isInOneStep_) {
            TLOGI(WmsLogTag::DMS, "is oneStep now, ignore nextState:%{public}d", nextState);
            switch (nextState) {
                case FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND:
                    [[fallthrough]];
                case FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED:
                    [[fallthrough]];
                case FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND:
                    [[fallthrough]];
                case FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED: {
                    oneStep_.notify_all();
                    ProcessNotifyFoldStatusChange(mState_, nextState, angles, foldScreenPolicy);
                    return;
                }
                default:
                    return;
            }
        }
    }
    auto task = [weakThis = wptr(this), nextState, angles, halls, weakPolicy = wptr(foldScreenPolicy)] {
        auto manager = weakThis.promote();
        auto policy = weakPolicy.promote();
        if (manager == nullptr || policy == nullptr) {
            TLOGNE(WmsLogTag::DMS, "sensorFoldStateManager or foldScreenPolicy is nullptr.");
            return;
        }
        TLOGNI(WmsLogTag::DMS, "current state: %{public}d, next state: %{public}d.", manager->mState_, nextState);
        FoldStatus currentState = manager->mState_;
        manager->mState_ = manager->HandleSecondaryOneStep(currentState, nextState, angles, halls);
        manager->ProcessNotifyFoldStatusChange(currentState, manager->mState_, angles, policy);
    };
    taskScheduler_ = ScreenSessionManager::GetInstance().GetPowerTaskScheduler();
    if (taskScheduler_ != nullptr) {
        taskScheduler_->PostAsyncTask(task, "secondaryFoldStatusChange");
    }
}

void SensorFoldStateManager::ProcessNotifyFoldStatusChange(FoldStatus currentStatus, FoldStatus nextStatus,
    const std::vector<float>& angles, sptr<FoldScreenPolicy> foldScreenPolicy)
{
    ReportNotifyFoldStatusChange(static_cast<int32_t>(currentStatus), static_cast<int32_t>(nextStatus), angles);
    PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
    NotifyReportFoldStatusToScb(currentStatus, nextStatus, angles);
    if (foldScreenPolicy != nullptr) {
        foldScreenPolicy->SetFoldStatus(nextStatus);
    }
    ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(nextStatus);
    if (foldScreenPolicy != nullptr && foldScreenPolicy->lockDisplayStatus_ != true) {
        foldScreenPolicy->SendSensorResult(nextStatus);
    }
}

FoldStatus SensorFoldStateManager::GetCurrentState()
{
    return mState_;
}

void SensorFoldStateManager::ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus,
    float postureAngle)
{
    TLOGI(WmsLogTag::DMS, "ReportNotifyFoldStatusChange currentStatus: %{public}d, nextStatus: %{public}d, "
        "postureAngle: %{public}f", currentStatus, nextStatus, postureAngle);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "NOTIFY_FOLD_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_FOLD_STATUS", currentStatus,
        "NEXT_FOLD_STATUS", nextStatus,
        "SENSOR_POSTURE", postureAngle);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "ReportNotifyFoldStatusChange Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void SensorFoldStateManager::ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus,
    const std::vector<float> &postureAngles)
{
    TLOGI(WmsLogTag::DMS, "currentStatus: %{public}d, nextStatus: %{public}d, %{public}s",
        currentStatus, nextStatus, FoldScreenStateInternel::TransVec2Str(postureAngles, "postureAngles").c_str());
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "NOTIFY_FOLD_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_FOLD_STATUS", currentStatus,
        "NEXT_FOLD_STATUS", nextStatus,
        "SENSOR_POSTURE", FoldScreenStateInternel::TransVec2Str(postureAngles, "postureAngles"));
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void SensorFoldStateManager::ClearState(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    mState_ = FoldStatus::UNKNOWN;
    foldScreenPolicy->ClearState();
}

void SensorFoldStateManager::RegisterApplicationStateObserver()
{
    TLOGI(WmsLogTag::DMS, "current device is not supported");
}

void SensorFoldStateManager::NotifyReportFoldStatusToScb(FoldStatus currentStatus, FoldStatus nextStatus,
    float postureAngle)
{
    std::chrono::time_point<std::chrono::system_clock> timeNow = std::chrono::system_clock::now();
    int32_t duration = static_cast<int32_t>(
        std::chrono::duration_cast<std::chrono::seconds>(timeNow - mLastStateClock_).count());
    mLastStateClock_ = timeNow;

    std::vector<std::string> screenFoldInfo {std::to_string(static_cast<int32_t>(currentStatus)),
        std::to_string(static_cast<int32_t>(nextStatus)), std::to_string(duration), std::to_string(postureAngle)};
    ScreenSessionManager::GetInstance().ReportFoldStatusToScb(screenFoldInfo);
}

void SensorFoldStateManager::NotifyReportFoldStatusToScb(FoldStatus currentStatus, FoldStatus nextStatus,
    const std::vector<float> &postureAngles)
{
    std::chrono::time_point<std::chrono::system_clock> timeNow = std::chrono::system_clock::now();
    int32_t duration = static_cast<int32_t>(
        std::chrono::duration_cast<std::chrono::seconds>(timeNow - mLastStateClock_).count());
    mLastStateClock_ = timeNow;
    std::vector<std::string> screenFoldInfo {std::to_string(static_cast<int32_t>(currentStatus)),
        std::to_string(static_cast<int32_t>(nextStatus)), std::to_string(duration)};
    for (const float &angle : postureAngles) {
        screenFoldInfo.emplace_back(std::to_string(angle));
    }
    ScreenSessionManager::GetInstance().ReportFoldStatusToScb(screenFoldInfo);
}

bool SensorFoldStateManager::IsTentMode()
{
    return tentModeType_ != 0;
}

void SensorFoldStateManager::SetTentMode(int tentType)
{
    TLOGI(WmsLogTag::DMS, "tent mode changing: %{public}d -> %{public}d", tentModeType_, tentType);
    tentModeType_ = tentType;
}
} // namespace OHOS::Rosen