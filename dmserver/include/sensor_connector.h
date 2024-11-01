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

#ifndef OHOS_ROSEN_SCREEN_CONNECTOR_H
#define OHOS_ROSEN_SCREEN_CONNECTOR_H

#include <map>
#include <refbase.h>

#include "dm_common.h"
#include "screen_rotation_controller.h"
#include "window_manager_hilog.h"
#include "sensor_plugin.h"

#ifdef SENSOR_ENABLE
#include "sensor_agent.h"
#endif

namespace OHOS {
namespace Rosen {
class SensorConnector : public RefBase {
public:
    SensorConnector() = delete;
    ~SensorConnector() = default;

    static void SubscribeRotationSensor();
    static void UnsubscribeRotationSensor();
};

#ifdef SENSOR_ENABLE
class GravitySensorSubscriber {
friend SensorConnector;
public:
    GravitySensorSubscriber() = delete;
    ~GravitySensorSubscriber() = default;
private:
    static void SubscribeGravitySensor();
    static void UnsubscribeGravitySensor();

    static void HandleGravitySensorEventCallback(SensorEvent *event);
    static bool CheckCallbackTimeInterval();
    static int CalcRotationDegree(GravityData* gravityData);
    static SensorRotation CalcSensorRotation(int sensorDegree);

    static SensorUser user_;
    static bool isGravitySensorSubscribed_;
    static long lastCallbackTime_;
};
#endif

#ifdef WM_SUBSCRIBE_MOTION_ENABLE
class MotionSubscriber {
friend SensorConnector;
public:
    MotionSubscriber() = delete;
    ~MotionSubscriber() = default;
private:
    static void SubscribeMotionSensor();
    static void UnsubscribeMotionSensor();

    static bool isMotionSensorSubscribed_;
};
#endif
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SCREEN_CONNECTOR_H
