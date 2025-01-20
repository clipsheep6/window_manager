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

#ifndef OHOS_ROSEN_SMALL_DEVICE_SCREEN_SENSOR_MANAGER_H
#define OHOS_ROSEN_SMALL_DEVICE_SCREEN_SENSOR_MANAGER_H

#include <mutex>
#include <refbase.h>
#include <chrono>

#include "dm_common.h"
#include "fold_screen_controller/fold_screen_policy.h"

namespace OHOS::Rosen {
class SensorFoldStateManager : public RefBase {
public:
    SensorFoldStateManager();
    virtual ~SensorFoldStateManager();

    virtual void HandleAngleChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy);
    virtual void HandleHallChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy);
    virtual void HandleTentChange(bool isTentMode, sptr<FoldScreenPolicy> foldScreenPolicy);
    virtual void HandleAngleOrHallChange(const std::vector<float> &angles, const std::vector<uint16_t> &halls,
        sptr<FoldScreenPolicy> foldScreenPolicy);
    virtual void RegisterApplicationStateObserver();
    void ClearState(sptr<FoldScreenPolicy> foldScreenPolicy);
    bool IsTentMode();

protected:
    void HandleSensorChange(FoldStatus nextState, float angle, sptr<FoldScreenPolicy> foldScreenPolicy);
    void HandleSensorChange(FoldStatus nextState, const std::vector<float> &angles,
        sptr<FoldScreenPolicy> foldScreenPolicy);
    FoldStatus GetCurrentState();
    void SetTentMode(bool status);
    std::recursive_mutex mutex_;

private:
    void ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus, float postureAngle);
    void ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus,
        const std::vector<float> &postureAngles);
    FoldStatus mState_ = FoldStatus::UNKNOWN;
    bool isTentMode_ = false;

    std::chrono::time_point<std::chrono::system_clock> mLastStateClock_ = std::chrono::system_clock::now();
    void NotifyReportFoldStatusToScb(FoldStatus currentStatus, FoldStatus nextStatus, float postureAngle);
    void NotifyReportFoldStatusToScb(FoldStatus currentStatus, FoldStatus nextStatus,
        const std::vector<float> &postureAngles);
};
} // namespace OHOS::Rosen
#endif //OHOS_ROSEN_SMALL_DEVICE_SCREEN_SENSOR_MANAGER_H