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

#include <gtest/gtest.h>
#include "screen_session_manager/include/fold_screen_controller/sensor_fold_state_manager/secondary_display_sensor_fold_state_manager.h"
#include "screen_session_manager/include/fold_screen_controller/secondary_display_fold_policy.h"
#include "fold_screen_state_internel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class SecondaryDisplaySensorFoldStateManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SecondaryDisplaySensorFoldStateManagerTest::SetUpTestCase()
{
}

void SecondaryDisplaySensorFoldStateManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void SecondaryDisplaySensorFoldStateManagerTest::SetUp()
{
}

void SecondaryDisplaySensorFoldStateManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleAngleOrHallChange01
 * @tc.desc: test function : HandleAngleOrHallChange
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, HandleAngleOrHallChange01, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    std::vector<float> angels = {0, 0, 0};
    std::vector<uint16_t> halls = {0, 0};
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SecondaryDisplaySensorFoldStateManager manager;
    manager.HandleAngleOrHallChange(angels, halls, foldScreenPolicy);
    EXPECT_EQ(manager.GetNextFoldState(angels, halls), FoldStatus::FOLDED);
}

/**
 * @tc.name: HandleAngleOrHallChange02
 * @tc.desc: test angles.size() != 3
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, HandleAngleOrHallChange02, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    std::vector<float> angels = {0, 0};
    std::vector<uint16_t> halls = {0, 0};
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SecondaryDisplaySensorFoldStateManager manager;
    manager.HandleAngleOrHallChange(angels, halls, foldScreenPolicy);
    angels = {0, 0, 0};
    EXPECT_EQ(manager.GetNextFoldState(angels, halls), FoldStatus::FOLDED);
}

/**
 * @tc.name: HandleAngleOrHallChange03
 * @tc.desc: test isSecondaryReflexion
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, HandleAngleOrHallChange03, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    std::vector<float> angels = {180, 180, 1};
    std::vector<uint16_t> halls = {1, 1};
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    sptr<FoldScreenPolicy> foldScreenPolicy = new SecondaryDisplayFoldPolicy(displayInfoMutex,
        screenPowerTaskScheduler);
    EXPECT_NE(foldScreenPolicy, nullptr);
    SecondaryDisplaySensorFoldStateManager manager;
    manager.HandleAngleOrHallChange(angels, halls, foldScreenPolicy);
    EXPECT_EQ(manager.GetNextFoldState(angels, halls), FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND);
}

/**
 * @tc.name: HandleAngleOrHallChange04
 * @tc.desc: test isHasReflexioned && !isSecondaryReflexion
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, HandleAngleOrHallChange04, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    std::vector<float> angels = {180, 180, 0};
    std::vector<uint16_t> halls = {1, 1};
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    sptr<FoldScreenPolicy> foldScreenPolicy = new SecondaryDisplayFoldPolicy(displayInfoMutex,
        screenPowerTaskScheduler);
    EXPECT_NE(foldScreenPolicy, nullptr);
    SecondaryDisplaySensorFoldStateManager manager;
    manager.HandleAngleOrHallChange(angels, halls, foldScreenPolicy);
    EXPECT_EQ(manager.GetNextFoldState(angels, halls), FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND);
}

/**
 * @tc.name: UpdateSwitchScreenBoundaryForLargeFoldDeviceAB
 * @tc.desc: test function : UpdateSwitchScreenBoundaryForLargeFoldDeviceAB
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, UpdateSwitchScreenBoundaryForLargeFoldDeviceAB,
        TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    float angel = 0;
    uint16_t hall = 0;
    SecondaryDisplaySensorFoldStateManager manager;
    FoldStatus state = FoldStatus::UNKNOWN;
    manager.UpdateSwitchScreenBoundaryForLargeFoldDeviceAB(angel, hall, state);
    EXPECT_EQ(manager.allowUserSensorForLargeFoldDeviceAB, 0);

    angel = 91.0F;
    hall = 1;
    manager.UpdateSwitchScreenBoundaryForLargeFoldDeviceAB(angel, hall, state);
    EXPECT_EQ(manager.allowUserSensorForLargeFoldDeviceAB, 1);
}

/**
 * @tc.name: UpdateSwitchScreenBoundaryForLargeFoldDeviceBC
 * @tc.desc: test function : UpdateSwitchScreenBoundaryForLargeFoldDeviceBC
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, UpdateSwitchScreenBoundaryForLargeFoldDeviceBC,
        TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    float angel = 0;
    uint16_t hall = 0;
    SecondaryDisplaySensorFoldStateManager manager;
    FoldStatus state = FoldStatus::UNKNOWN;
    manager.UpdateSwitchScreenBoundaryForLargeFoldDeviceBC(angel, hall, state);
    EXPECT_EQ(manager.allowUserSensorForLargeFoldDeviceBC, 0);

    angel = 91.0F;
    hall = 1;
    manager.UpdateSwitchScreenBoundaryForLargeFoldDeviceBC(angel, hall, state);
    EXPECT_EQ(manager.allowUserSensorForLargeFoldDeviceBC, 1);
}

/**
 * @tc.name: GetNextFoldStateHalf01
 * @tc.desc: test function : GetNextFoldStateHalf
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, GetNextFoldStateHalf01, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    float angel = -0.1;
    uint16_t hall = 0;
    int32_t allowUserSensorForLargeFoldDevice = 0;
    SecondaryDisplaySensorFoldStateManager manager;
    FoldStatus state = FoldStatus::UNKNOWN;
    auto result1 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result1), 0);

    angel = 90.0F;
    hall = 1;
    auto result2 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result2), 3);

    angel = 130.0F - 0.1;
    hall = 1;
    auto result3 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result3), 3);

    angel = 130.0F - 0.1;
    hall = 0;
    auto result4 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result4), 3);

    angel = 130.0F + 0.1;
    hall = 0;
    auto result5 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result5), 3);

    angel = 140.0F + 0.1;
    hall = 0;
    auto result6 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result6), 3);

    angel = 140.0F + 0.1;
    hall = 1;
    auto result7 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result7), 1);
}

/**
 * @tc.name: GetNextFoldStateHalf02
 * @tc.desc: test function : GetNextFoldStateHalf
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, GetNextFoldStateHalf02, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    SecondaryDisplaySensorFoldStateManager manager;
    int32_t allowUserSensorForLargeFoldDevice = 1;
    FoldStatus state = FoldStatus::UNKNOWN;
    float angel = 45.0F;
    uint16_t hall = 1;
    auto result1 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result1), 0);

    angel = 70.0F - 0.1;
    auto result2 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result2), 2);

    angel = 70.0F + 0.1;
    auto result3 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result3), 3);

    angel = 130.0F - 0.1;
    auto result4 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result4), 3);

    angel = 130.0F + 0.1;
    auto result5 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result5), 3);

    angel = 80.0F - 0.1;
    auto result6 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result6), 3);

    angel = 70.0F + 0.1;
    hall = 0;
    auto result7 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result7), 3);

    angel = 130.0F + 0.1;
    auto result8 = manager.GetNextFoldStateHalf(angel, hall, state, allowUserSensorForLargeFoldDevice);
    EXPECT_EQ(static_cast<int>(result8), 3);
}

/**
 * @tc.name: GetGlobalFoldState
 * @tc.desc: test function : GetGlobalFoldState
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, GetGlobalFoldState, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    SecondaryDisplaySensorFoldStateManager manager;
    FoldStatus nextStatePrimary = FoldStatus::EXPAND;
    FoldStatus nextStateSecondary = FoldStatus::EXPAND;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND);

    nextStatePrimary = FoldStatus::FOLDED;
    nextStateSecondary = FoldStatus::EXPAND;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_EXPAND);

    nextStatePrimary = FoldStatus::HALF_FOLD;
    nextStateSecondary = FoldStatus::EXPAND;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND);

    nextStatePrimary = FoldStatus::EXPAND;
    nextStateSecondary = FoldStatus::FOLDED;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::EXPAND);

    nextStatePrimary = FoldStatus::FOLDED;
    nextStateSecondary = FoldStatus::FOLDED;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLDED);

    nextStatePrimary = FoldStatus::HALF_FOLD;
    nextStateSecondary = FoldStatus::FOLDED;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::HALF_FOLD);

    nextStatePrimary = FoldStatus::EXPAND;
    nextStateSecondary = FoldStatus::HALF_FOLD;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED);

    nextStatePrimary = FoldStatus::FOLDED;
    nextStateSecondary = FoldStatus::HALF_FOLD;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_HALF_FOLDED);

    nextStatePrimary = FoldStatus::HALF_FOLD;
    nextStateSecondary = FoldStatus::HALF_FOLD;
    EXPECT_EQ(manager.GetGlobalFoldState(nextStatePrimary, nextStateSecondary),
    FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED);
}
}
}
}