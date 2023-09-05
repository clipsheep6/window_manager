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

#include "session_manager/include/screen_rotation_property.h"
#include <gtest/gtest.h>

#include "window_manager_hilog.h"
#include "screen_session_manager.h"
#include <chrono>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenRotationPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenRotationPropertyTest::SetUpTestCase()
{
}

void ScreenRotationPropertyTest::TearDownTestCase()
{
}

void ScreenRotationPropertyTest::SetUp()
{
}

void ScreenRotationPropertyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Init
 * @tc.desc: test function : Init
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, Init, Function | SmallTest | Level1)
{
    int res = 0;
    ScreenRotationProperty::Init();
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: GetDefaultDisplayId
 * @tc.desc: test function : GetDefaultDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, GetDefaultDisplayId, Function | SmallTest | Level1)
{
    ScreenRotationProperty::Init();
    int res = 0;
    ScreenRotationProperty::GetDefaultDisplayId();
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: ConvertDeviceToDisplayRotation
 * @tc.desc: test function : ConvertDeviceToDisplayRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ConvertDeviceToDisplayRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertDeviceToDisplayRotation start";
    ScreenRotationProperty::Init();
    DeviceRotation deviceRotation = DeviceRotation::INVALID;
    Rotation rotation  = ScreenRotationProperty::ConvertDeviceToDisplayRotation(deviceRotation);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), rotation);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    rotation  = ScreenRotationProperty::ConvertDeviceToDisplayRotation(deviceRotation);
    ASSERT_EQ(ScreenRotationProperty::deviceToDisplayRotationMap_.at(deviceRotation), rotation);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertDeviceToDisplayRotation end";
}

/**
 * @tc.name: ProcessRotationMapping
 * @tc.desc: test function : ProcessRotationMapping
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessRotationMapping, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessRotationMapping_PORTRAIT start";
    ScreenRotationProperty::defaultDeviceRotation_ = 0;
    ScreenRotationProperty::ProcessRotationMapping();
    ASSERT_EQ(Rotation::ROTATION_0,
        ScreenRotationProperty::deviceToDisplayRotationMap_.at(DeviceRotation::ROTATION_PORTRAIT));
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessRotationMapping_PORTRAIT end";
}

/**
 * @tc.name: GetCurrentDisplayRotation
 * @tc.desc: test function : GetCurrentDisplayRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, GetCurrentDisplayRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: GetCurrentDisplayRotation start";
    ScreenRotationProperty::Init();
    ScreenSessionManager *screenSessionManager = new ScreenSessionManager;
    sptr<DisplayInfo> defaultDisplayInfo = screenSessionManager->GetDefaultDisplayInfo();
    Rotation rotation = ScreenRotationProperty::GetCurrentDisplayRotation();
    ASSERT_EQ(defaultDisplayInfo->GetRotation(), rotation);
    defaultDisplayInfo = nullptr;
    rotation = ScreenRotationProperty::GetCurrentDisplayRotation();
    ASSERT_EQ(ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT), rotation);
    ScreenRotationProperty::defaultDeviceRotation_ = 1;
    rotation = ScreenRotationProperty::GetCurrentDisplayRotation();
    ASSERT_EQ(ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE), rotation);
    delete screenSessionManager;
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: GetCurrentDisplayRotation end";
}

/**
 * @tc.name: HandleSensorEventInput
 * @tc.desc: test function : HandleSensorEventInput
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, HandleSensorEventInput, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: HandleSensorEventInput start";
    ScreenRotationProperty::Init();
    DeviceRotation deviceRotation = DeviceRotation::INVALID;
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    ScreenSessionManager *screenSessionManager = new ScreenSessionManager;
    auto defaultDisplayInfo = screenSessionManager->GetDefaultDisplayInfo();
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
    ScreenRotationProperty::GetCurrentDisplayRotation();
    ScreenRotationProperty::ConvertDeviceToDisplayRotation(deviceRotation);
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
    int32_t res = 0;
    ScreenRotationProperty::HandleSensorEventInput(DeviceRotation::INVALID);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: HandleSensorEventInput end";
}

/**
 * @tc.name: CalcDeviceRotation
 * @tc.desc: CalcDeviceRotation func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, CalcDeviceRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcDeviceRotation start";
    ScreenRotationProperty::Init();
    SensorRotation sensorRotation = SensorRotation::INVALID;
    auto result = ScreenRotationProperty::CalcDeviceRotation(sensorRotation);
    ASSERT_EQ(DeviceRotation::INVALID, result);
    sensorRotation = SensorRotation::ROTATION_0;
    ScreenRotationProperty::defaultDeviceRotationOffset_ = 90;
    ScreenRotationProperty::defaultDeviceRotation_ = 1;
    result = ScreenRotationProperty::CalcDeviceRotation(sensorRotation);
    ASSERT_EQ(DeviceRotation::ROTATION_PORTRAIT_INVERTED, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcDeviceRotation end";
}

/**
 * @tc.name: IsDefaultDisplayRotationPortrait
 * @tc.desc: IsDefaultDisplayRotationPortrait func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsDefaultDisplayRotationPortrait, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDefaultDisplayRotationPortrait start";
    ScreenRotationProperty::Init();
    auto result = ScreenRotationProperty::IsDefaultDisplayRotationPortrait();
    ASSERT_EQ(true, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDefaultDisplayRotationPortrait end";
}
}
}
}