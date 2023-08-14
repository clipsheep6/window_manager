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
#ifndef OHOS_ROSEN_FOLD_SCREEN_SENSOR_MANAGER_H
#define OHOS_ROSEN_FOLD_SCREEN_SENSOR_MANAGER_H

#include <functional>
#include <mutex>

#include "refbase.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {

class FoldScreenSensorManager : public RefBase {
    WM_DECLARE_SINGLE_INSTANCE_BASE(FoldScreenSensorManager);
    using FoldScreenStateCallback = std::function<(void)(uint16_t)>;

public:
    void RegisterFoldScreenStateCallback(FoldScreenStateCallback&);

private:
    FoldScreenStateCallback mCallback_;

    std::recursive_mutex mutex_;

    uint16_t mState_;

    uint16_t mBoudaryFlag_;

    void HandleSensorData(const float[], int);

    uint16_t TransferAngleToScreenState(const float&, int) const;

    uint16_t GetSwitchScreenBoudary(const float&, int) const;

    uint16_t TransferScreenStateToPosture() const;

    FoldScreenSensorManager();

    ~FoldScreenSensorManager() = default;

namespace {
    /**
     * Angle index in sensor data array
     */
    constexpr uint16_t ANGLE_INDEX = 6;
    /**
     * Screen state type of device - UNKNOWN
     */
    constexpr uint16_t SCREEN_STATE_UNKNOWN = 0;
    /**
     * Screen state type of device - EXPAND
     */
    constexpr uint16_t SCREEN_STATE_EXPAND = 1;
    /**
     * Screen state type of device - FOLDED
     */
    constexpr uint16_t SCREEN_STATE_FOLDED = 2;
    /**
     * Screen state type of device - HALF_FOLDED
     */
    constexpr uint16_t SCREEN_STATE_HALF_FOLDED = 3;

    /**
     * Posture mode type of device - UNKNOWN
     */
    constexpr uint16_t POSTURE_UNKNOWN = 100;
    /**
     * Posture mode type of device - FOLDED
     */
    constexpr uint16_t POSTURE_FOLDED = 103;
    /**
     * Posture mode type of device - HALF_FOLDED
     */
    constexpr uint16_t POSTURE_HALF_FOLDED = 106;
    /**
     * Posture mode type of device - FULL
     */
    constexpr uint16_t POSTURE_FULL = 109;

    /**
     * Invalid hall threshold
     */
    constexpr uint16_t HALL_THRESHOLD_INVALID = -1;
    constexpr uint16_t HALL_FOLDED_THRESHOLD = 0;
    constexpr uint16_t HALL_THRESHOLD = 1;

    /**
     * The lower boundary angle of the folded state in the small screen switching interval
     */
    constexpr float SMALLER_BOUNDARY_THRESHOLD = 5.0F;
    /**
     * The lower boundary angle of the folded state in the large screen switching interval
     */
    constexpr float LARGER_BOUNDARY_THRESHOLD = 90.0F;
    constexpr uint16_t SMALLER_BOUNDARY_FLAG = 0;
    constexpr uint16_t LARGER_BOUNDARY_FLAG = 1;

    constexpr float HALF_FOLDED_MAX_THRESHOLD = 140.0F;
    constexpr float CLOSE_HALF_FOLDED_MIN_THRESHOLD = 90.0F;
    constexpr float OPEN_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
    constexpr float HALF_FOLDED_BUFFER = 10.0F;
    constexpr float AVOID_HALL_ERROR = 10.0F;
}
};
} // namespace Rosen
} // namespace OHOS
#endif