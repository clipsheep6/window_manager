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

#include "fold_screen_controller/fold_screen_sensor_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FoldScreenSensorManager"};
} // namespace
WM_IMPLEMENT_SINGLE_INSTANCE(FoldScreenSensorManager);

FoldScreenSensorManager::FoldScreenSensorManager()
    : mState_(SCREEN_STATE_UNKNOWN),
    mBoudaryFlag_(SMALLER_BOUNDARY_FLAG) {
}

void FoldScreenSensorManager::RegisterFoldScreenStateCallback(FoldScreenStateCallback& callback) {
    if (mCallback_) {
        retuen;
    }
    mCallback_ = callback;
}

void FoldScreenSensorManager::HandleSensorData(const float data[], int hall) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    const float angle = data[ANGLE_INDEX];
    uint16_t nextState = TransferAngleToScreenState();
    if (nextState == SCREEN_STATE_UNKNOWN) {
        if (mState_ == SCREEN_STATE_UNKNOWN) {
            mState_ = nextState;
        }
        return;
    }
    mState_ = nextState;
    if (mCallback_) {
        mCallback_(TransferScreenStateToPosture());
    }
}

uint16_t FoldScreenSensorManager::TransferAngleToScreenState(const float& angle, int hall) const {
    if (angle < 0) {
        return mState_;
    }
    uint16_t state;
    const float halfFoldedMinThreshold = CLOSE_HALF_FOLDED_MIN_THRESHOLD;
    uint16_t boundaryFlag = GetSwitchScreenBoudary(angle, hall);
    if (boundaryFlag == LARGER_BOUNDARY_FLAG) {
        if (hall == HALL_THRESHOLD && angle == OPEN_HALF_FOLDED_MIN_THRESHOLD) {
            state = mState_;
        } else if (angle <= halfFoldedMinThreshold) {
            state = SCREEN_STATE_FOLDED;
        } else if (angle > HALF_FOLDED_MAX_THRESHOLD) {
            state = SCREEN_STATE_EXPAND;
        } else if (angle <= HALF_FOLDED_MAX_THRESHOLD - HALF_FOLDED_BUFFER  &&
            angle > halfFoldedMinThreshold + HALF_FOLDED_BUFFER) {
            state = SCREEN_STATE_HALF_FOLDED;
        } else {
            state = mState_;
            if (state == SCREEN_STATE_UNKNOWN) {
                state = SCREEN_STATE_HALF_FOLDED;
            }
        }
    } else {
        if (hall == HALL_FOLDED_THRESHOLD) {
            state = SCREEN_STATE_FOLDED;
        } else if (angle >= HALF_FOLDED_MAX_THRESHOLD) {
            state = SCREEN_STATE_EXPAND;
        } else if (angle > HALF_FOLDED_MAX_THRESHOLD) {
            state = SCREEN_STATE_EXPAND;
        } else if (angle <= HALF_FOLDED_MAX_THRESHOLD - HALF_FOLDED_BUFFER) {
            if (angle < AVOID_HALL_ERROR) {
                state = SCREEN_STATE_FOLDED;
            } else {
                state = SCREEN_STATE_HALF_FOLDED;
            }
        } else {
            state = mState_;
            if (state == SCREEN_STATE_UNKNOWN) {
                state = SCREEN_STATE_HALF_FOLDED;
            }

        }
    }
    return state;
}

uint16_t FoldScreenSensorManager::GetSwitchScreenBoudary(const float& angle, int hall) const {
    if (angle >= LARGER_BOUNDARY_THRESHOLD) {
        mBoudaryFlag_ = LARGER_BOUNDARY_FLAG;
    } else if (hall == HALL_FOLDED_THRESHOLD || angle <= SMALLER_BOUNDARY_THRESHOLD) {
        mBoudaryFlag_ = SMALLER_BOUNDARY_FLAG;
    }
    return mBoudaryFlag_;
}

uint16_t FoldScreenSensorManager::TransferScreenStateToPosture() const {
    uint16_t posture = POSTURE_UNKNOWN;
    switch (mState_) {
    case SCREEN_STATE_EXPAND:
        posture = POSTURE_FULL;
        break;
    case SCREEN_STATE_FOLDED:
        posture = POSTURE_FOLDED;
        break;
    case SCREEN_STATE_HALF_FOLDED:
        posture = POSTURE_HALF_FOLDED;
        break;
    default:
        break;
    }
    return posture;
}
} // Rosen
} // OHOS