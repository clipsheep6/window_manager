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

#include "screen_rotation_property.h"

#include <chrono>
#include <securec.h>

#include "screen_session_manager.h"
#include "parameters.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenRotationProperty"};
}

DisplayId ScreenRotationProperty::defaultDisplayId_ = 0;
uint32_t ScreenRotationProperty::defaultDeviceRotationOffset_ = 0;
uint32_t ScreenRotationProperty::defaultDeviceRotation_ = 0;
std::map<SensorRotation, DeviceRotation> ScreenRotationProperty::sensorToDeviceRotationMap_;
std::map<DeviceRotation, Rotation> ScreenRotationProperty::deviceToDisplayRotationMap_;
std::map<Rotation, DisplayOrientation> ScreenRotationProperty::displayToDisplayOrientationMap_;
DeviceRotation ScreenRotationProperty::lastSensorRotationConverted_ = DeviceRotation::INVALID;

void ScreenRotationProperty::Init()
{
    ProcessRotationMapping();
    defaultDisplayId_ = GetDefaultDisplayId();
}

DisplayId ScreenRotationProperty::GetDefaultDisplayId()
{
    sptr<DisplayInfo> defaultDisplayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("GetDefaultDisplayId, defaultDisplayInfo is nullptr.");
        return DISPLAY_ID_INVALID;
    }
    return defaultDisplayInfo->GetDisplayId();
}

void ScreenRotationProperty::HandleSensorEventInput(DeviceRotation deviceRotation)
{
    auto isPhone = system::GetParameter("const.product.devicetype", "unknown") == "phone";
    if (!isPhone) {
        WLOGFW("device is not phone, return.");
        return;
    }
    if (deviceRotation == DeviceRotation::INVALID) {
        WLOGFW("deviceRotation is invalid, return.");
        return;
    }
    auto screenSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
    if (!screenSession) {
        WLOGFW("screenSession is null.");
        return;
    }
    if (lastSensorRotationConverted_ == deviceRotation) {
        return;
    }
    lastSensorRotationConverted_ = deviceRotation;
    Rotation targetSensorRotation = ConvertDeviceToDisplayRotation(deviceRotation);
    screenSession->SensorRotationChange(targetSensorRotation);
}

Rotation ScreenRotationProperty::GetCurrentDisplayRotation()
{
    sptr<DisplayInfo> defaultDisplayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("Cannot get default display info");
        return defaultDeviceRotation_ == 0 ? ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT) :
            ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE);
    }
    return defaultDisplayInfo->GetRotation();
}

DeviceRotation ScreenRotationProperty::CalcDeviceRotation(SensorRotation sensorRotation)
{
    if (sensorRotation == SensorRotation::INVALID) {
        return DeviceRotation::INVALID;
    }
    // offset(in degree) divided by 90 to get rotation bias
    int32_t bias = static_cast<int32_t>(defaultDeviceRotationOffset_ / 90);
    int32_t deviceRotationValue = static_cast<int32_t>(sensorRotation) - bias;
    while (deviceRotationValue < 0) {
        // +4 is used to normalize the values into the range 0~3, corresponding to the four rotations.
        deviceRotationValue += 4;
    }
    if (defaultDeviceRotation_ == 1) {
        deviceRotationValue += static_cast<int32_t>(defaultDeviceRotation_);
        // %2 to determine whether the rotation is horizontal or vertical.
        if (deviceRotationValue % 2 == 0) {
            // if device's default rotation is landscape, use -2 to swap 0 and 90, 180 and 270.
            deviceRotationValue -= 2;
        }
    }
    return static_cast<DeviceRotation>(deviceRotationValue);
}

Rotation ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation deviceRotation)
{
    if (deviceRotation == DeviceRotation::INVALID) {
        return GetCurrentDisplayRotation();
    }
    if (deviceToDisplayRotationMap_.empty()) {
        ProcessRotationMapping();
    }
    return deviceToDisplayRotationMap_.at(deviceRotation);
}

void ScreenRotationProperty::ProcessRotationMapping()
{
    sptr<SupportedScreenModes> modes =
        ScreenSessionManager::GetInstance().GetScreenModesByDisplayId(defaultDisplayId_);

    // 0 means PORTRAIT, 1 means LANDSCAPE.
    defaultDeviceRotation_ = (modes == nullptr || modes->width_ < modes->height_) ? 0 : 1;
    if (deviceToDisplayRotationMap_.empty()) {
        deviceToDisplayRotationMap_ = {
            {DeviceRotation::ROTATION_PORTRAIT,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_0 : Rotation::ROTATION_90},
            {DeviceRotation::ROTATION_LANDSCAPE,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_0 : Rotation::ROTATION_90},
            {DeviceRotation::ROTATION_PORTRAIT_INVERTED,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_180 : Rotation::ROTATION_270},
            {DeviceRotation::ROTATION_LANDSCAPE_INVERTED,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_180 : Rotation::ROTATION_270},
        };
    }
    if (displayToDisplayOrientationMap_.empty()) {
        displayToDisplayOrientationMap_ = {
            {defaultDeviceRotation_ == 0 ? Rotation::ROTATION_0 : Rotation::ROTATION_90,
                DisplayOrientation::PORTRAIT},
            {defaultDeviceRotation_ == 1 ? Rotation::ROTATION_0 : Rotation::ROTATION_90,
                DisplayOrientation::LANDSCAPE},
            {defaultDeviceRotation_ == 0 ? Rotation::ROTATION_180 : Rotation::ROTATION_270,
                DisplayOrientation::PORTRAIT_INVERTED},
            {defaultDeviceRotation_ == 1 ? Rotation::ROTATION_180 : Rotation::ROTATION_270,
                DisplayOrientation::LANDSCAPE_INVERTED},
        };
    }
    if (sensorToDeviceRotationMap_.empty()) {
        sensorToDeviceRotationMap_ = {
            {SensorRotation::ROTATION_0, CalcDeviceRotation(SensorRotation::ROTATION_0)},
            {SensorRotation::ROTATION_90, CalcDeviceRotation(SensorRotation::ROTATION_90)},
            {SensorRotation::ROTATION_180, CalcDeviceRotation(SensorRotation::ROTATION_180)},
            {SensorRotation::ROTATION_270, CalcDeviceRotation(SensorRotation::ROTATION_270)},
            {SensorRotation::INVALID, DeviceRotation::INVALID},
        };
    }
}

bool ScreenRotationProperty::IsDefaultDisplayRotationPortrait()
{
    return Rotation::ROTATION_0 == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT);
}
} // Rosen
} // OHOS