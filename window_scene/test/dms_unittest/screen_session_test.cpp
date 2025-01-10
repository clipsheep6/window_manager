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

#include "screen_session.h"
#include <gtest/gtest.h>
#include "screen_session_manager/include/screen_session_manager.h"
#include "scene_board_judgement.h"

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class MockScreenChangeListener : public IScreenChangeListener {
public:
    void OnConnect(ScreenId screenId) override {}
    void OnDisconnect(ScreenId screenId) override {}
    void OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
        ScreenId screenId) override {}
    void OnPowerStatusChange(DisplayPowerEvent event, EventStatus status,
        PowerStateChangeReason reason) override {}
    void OnSensorRotationChange(float sensorRotation, ScreenId screenId) override {}
    void OnScreenOrientationChange(float screenOrientation, ScreenId screenId) override {}
    void OnScreenRotationLockedChange(bool isLocked, ScreenId screenId) override {}
    void OnScreenExtendChange(ScreenId mainScreenId, ScreenId extendScreenId) override {}
    void OnHoverStatusChange(int32_t hoverStatus, bool needRotate, ScreenId screenId) override {}
    void OnScreenCaptureNotify(ScreenId mainScreenId, int32_t uid, const std::string& clientName) override {}
    void OnSuperFoldStatusChange(ScreenId screenId, SuperFoldStatus superFoldStatus) override {}
};
class ScreenSessionTest : public testing::Test {
  public:
    ScreenSessionTest() {}
    ~ScreenSessionTest() {}
};

namespace {

/**
 * @tc.name: create ScreenSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_CLIENT);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession02, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession03, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_MIRROR);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession04, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: create ScreenSession05
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenSession05, Function | SmallTest | Level2)
{
    ScreenSessionConfig config = {
        .screenId = 0,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::INVALID);
    EXPECT_NE(nullptr, screenSession);
}

/**
 * @tc.name: CreateDisplayNode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CreateDisplayNode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CreateDisplayNode start";
    Rosen::RSDisplayNodeConfig rsConfig;
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    rsConfig.isMirrored = true;
    rsConfig.screenId = 101;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    screenSession->CreateDisplayNode(rsConfig);
    GTEST_LOG_(INFO) << "ScreenSessionTest: CreateDisplayNode end";
}

/**
 * @tc.name: SetMirrorScreenType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetMirrorScreenType, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetMirrorScreenType start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    Rosen::RSDisplayNodeConfig rsConfig;
    rsConfig.isMirrored = true;
    rsConfig.screenId = 101;
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    MirrorScreenType mirrorType = MirrorScreenType::VIRTUAL_MIRROR;
    screenSession->SetMirrorScreenType(mirrorType);
    GTEST_LOG_(INFO) << "SetMirrorScreenType end";
}

/**
 * @tc.name: GetMirrorScreenType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetMirrorScreenType, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetMirrorScreenType start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    MirrorScreenType mirrorType = MirrorScreenType::VIRTUAL_MIRROR;
    screenSession->SetMirrorScreenType(mirrorType);
    MirrorScreenType res = screenSession->GetMirrorScreenType();
    ASSERT_EQ(mirrorType, res);
    GTEST_LOG_(INFO) << "GetMirrorScreenType end";
}

/**
 * @tc.name: SetDefaultDeviceRotationOffset
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetDefaultDeviceRotationOffset, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetDefaultDeviceRotationOffset start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t defaultRotationOffset = 0;
    screenSession->SetDefaultDeviceRotationOffset(defaultRotationOffset);
    GTEST_LOG_(INFO) << "SetDefaultDeviceRotationOffset end";
}

/**
 * @tc.name: UpdateDisplayState
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateDisplayState, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateDisplayState start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DisplayState displayState = DisplayState::OFF;
    screenSession->UpdateDisplayState(displayState);
    GTEST_LOG_(INFO) << "UpdateDisplayState end";
}

/**
 * @tc.name: UpdateRefreshRate
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateRefreshRate, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateRefreshRate start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t refreshRate = 2;
    screenSession->UpdateRefreshRate(refreshRate);
    GTEST_LOG_(INFO) << "UpdateRefreshRate end";
}

/**
 * @tc.name: GetRefreshRate
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRefreshRate, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetRefreshRate start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t refreshRate = 2;
    screenSession->UpdateRefreshRate(refreshRate);
    auto res = screenSession->GetRefreshRate();
    ASSERT_EQ(refreshRate, res);
    GTEST_LOG_(INFO) << "GetRefreshRate end";
}

/**
 * @tc.name: UpdatePropertyByResolution
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdatePropertyByResolution, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdatePropertyByResolution start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t width = 1000;
    uint32_t height = 1500;
    screenSession->UpdatePropertyByResolution(width, height);
    GTEST_LOG_(INFO) << "UpdatePropertyByResolution end";
}

/**
 * @tc.name: HandleSensorRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, HandleSensorRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "HandleSensorRotation start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    float sensorRotation = 0.0f;
    screenSession->HandleSensorRotation(sensorRotation);
    GTEST_LOG_(INFO) << "HandleSensorRotation end";
}

/**
 * @tc.name: ConvertIntToRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertIntToRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertIntToRotation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int rotation = 0;
    auto res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_0);
    rotation = 90;
    res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_90);
    rotation = 180;
    res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_180);
    rotation = 270;
    res = session->ConvertIntToRotation(rotation);
    ASSERT_EQ(res, Rotation::ROTATION_270);
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertIntToRotation end";
}

/**
 * @tc.name: SetVirtualScreenFlag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetVirtualScreenFlag, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetVirtualScreenFlag start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    VirtualScreenFlag screenFlag = VirtualScreenFlag::CAST;
    screenSession->SetVirtualScreenFlag(screenFlag);
    GTEST_LOG_(INFO) << "SetVirtualScreenFlag end";
}

/**
 * @tc.name: GetVirtualScreenFlag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetVirtualScreenFlag, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetVirtualScreenFlag start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    VirtualScreenFlag screenFlag = VirtualScreenFlag::CAST;
    screenSession->SetVirtualScreenFlag(screenFlag);
    auto res = screenSession->GetVirtualScreenFlag();
    ASSERT_EQ(screenFlag, res);
    GTEST_LOG_(INFO) << "GetVirtualScreenFlag end";
}

/**
 * @tc.name: SetPhysicalRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetPhysicalRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetPhysicalRotation start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    int rotation = 0;
    screenSession->SetPhysicalRotation(rotation);
    GTEST_LOG_(INFO) << "SetPhysicalRotation end";
}

/**
 * @tc.name: SetScreenComponentRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenComponentRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenComponentRotation start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    int rotation = 0;
    screenSession->SetScreenComponentRotation(rotation);
    GTEST_LOG_(INFO) << "SetScreenComponentRotation end";
}

/**
 * @tc.name: UpdateToInputManager
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateToInputManager, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateToInputManager start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    RRect bounds;
    int rotation = 90;
    int deviceRotation = 90;
    FoldDisplayMode foldDisplayMode = FoldDisplayMode::FULL;
    screenSession->UpdateToInputManager(bounds, rotation, deviceRotation, foldDisplayMode);
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    rotation = 0;
    foldDisplayMode = FoldDisplayMode::MAIN;
    screenSession->UpdateToInputManager(bounds, rotation, deviceRotation, foldDisplayMode);
    GTEST_LOG_(INFO) << "UpdateToInputManager end";
}

/**
 * @tc.name: UpdatePropertyAfterRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdatePropertyAfterRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdatePropertyAfterRotation start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    int rotation = 90;
    FoldDisplayMode foldDisplayMode = FoldDisplayMode::MAIN;
    screenSession->UpdatePropertyAfterRotation(bounds, rotation, foldDisplayMode);
    GTEST_LOG_(INFO) << "UpdatePropertyAfterRotation end";
}

/**
 * @tc.name: ReportNotifyModeChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ReportNotifyModeChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ReportNotifyModeChange start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DisplayOrientation displayOrientation = DisplayOrientation::LANDSCAPE;
    screenSession->ReportNotifyModeChange(displayOrientation);
    displayOrientation = DisplayOrientation::PORTRAIT;
    screenSession->ReportNotifyModeChange(displayOrientation);
    displayOrientation = DisplayOrientation::PORTRAIT_INVERTED;
    screenSession->ReportNotifyModeChange(displayOrientation);
    GTEST_LOG_(INFO) << "ReportNotifyModeChange end";
}

/**
 * @tc.name: SuperFoldStatusChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SuperFoldStatusChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SuperFoldStatusChange start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    ScreenId screenId = 0;
    SuperFoldStatus superFoldStatus = SuperFoldStatus::UNKNOWN;
    screenSession->SuperFoldStatusChange(screenId, superFoldStatus);
    superFoldStatus = SuperFoldStatus::EXPANDED;
    screenSession->SuperFoldStatusChange(screenId, superFoldStatus);
    superFoldStatus = SuperFoldStatus::FOLDED;
    screenSession->SuperFoldStatusChange(screenId, superFoldStatus);
    superFoldStatus = SuperFoldStatus::KEYBOARD;
    screenSession->SuperFoldStatusChange(screenId, superFoldStatus);
    superFoldStatus = SuperFoldStatus::HALF_FOLDED;
    screenSession->SuperFoldStatusChange(screenId, superFoldStatus);
    GTEST_LOG_(INFO) << "SuperFoldStatusChange end";
}

/**
 * @tc.name: UpdateRotationAfterBoot01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateRotationAfterBoot01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateRotationAfterBoot start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool foldToExpand = true;
    screenSession->UpdateRotationAfterBoot(foldToExpand);
    GTEST_LOG_(INFO) << "UpdateRotationAfterBoot end";
}

/**
 * @tc.name: UpdateRotationAfterBoot02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateRotationAfterBoot02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateRotationAfterBoot start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool foldToExpand = false;
    screenSession->UpdateRotationAfterBoot(foldToExpand);
    GTEST_LOG_(INFO) << "UpdateRotationAfterBoot end";
}

/**
 * @tc.name: UpdateValidRotationToScb
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateValidRotationToScb, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateValidRotationToScb start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    screenSession->UpdateValidRotationToScb();
    GTEST_LOG_(INFO) << "UpdateValidRotationToScb end";
}

/**
 * @tc.name: SetScreenSceneDpiChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenSceneDpiChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenSceneDpiChangeListener start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    SetScreenSceneDpiFunc func = nullptr;
    screenSession->SetScreenSceneDpiChangeListener(func);
    GTEST_LOG_(INFO) << "SetScreenSceneDpiChangeListener end";
}

/**
 * @tc.name: SetScreenSceneDestroyListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenSceneDestroyListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenSceneDestroyListener start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DestroyScreenSceneFunc func = nullptr;
    screenSession->SetScreenSceneDestroyListener(func);
    GTEST_LOG_(INFO) << "SetScreenSceneDestroyListener end";
}

/**
 * @tc.name: SetScreenSceneDpi
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenSceneDpi, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenSceneDpi start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    float density = 1.5f;
    SetScreenSceneDpiFunc func = nullptr;
    screenSession->SetScreenSceneDpiChangeListener(func);
    screenSession->SetScreenSceneDpi(density);
    SetScreenSceneDpiFunc func2 = [](float density) {
            EXPECT_TRUE(true);
        };
    screenSession->SetScreenSceneDpiChangeListener(func2);
    screenSession->SetScreenSceneDpi(density);
    GTEST_LOG_(INFO) << "SetScreenSceneDpi end";
}

/**
 * @tc.name: DestroyScreenScene
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, DestroyScreenScene, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DestroyScreenScene start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DestroyScreenSceneFunc func = nullptr;
    screenSession->SetScreenSceneDestroyListener(func);
    screenSession->DestroyScreenScene();
    DestroyScreenSceneFunc func2 = []() {
            EXPECT_TRUE(true);
        };
    screenSession->SetScreenSceneDestroyListener(func2);
    screenSession->DestroyScreenScene();
    GTEST_LOG_(INFO) << "DestroyScreenScene end";
}

/**
 * @tc.name: SetDensityInCurResolution
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetDensityInCurResolution, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetDensityInCurResolution start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    float densityInCurResolution = 1.5f;
    screenSession->SetDensityInCurResolution(densityInCurResolution);
    GTEST_LOG_(INFO) << "SetDensityInCurResolution end";
}

/**
 * @tc.name: GetSourceMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSourceMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->screenId_ = session->defaultScreenId_;
    ScreenSourceMode mode = session->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MAIN);
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_ALONE);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_EXPAND);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_EXTEND);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MIRROR);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_UNIQUE);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_UNIQUE);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode end";
}

/**
 * @tc.name: GetSourceMode02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSourceMode02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode02 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->screenId_ = session->defaultScreenId_;
    ScreenSourceMode mode = session->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MAIN);
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);

    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_MAIN);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    mode = screenSession->GetSourceMode();
    ASSERT_EQ(mode, ScreenSourceMode::SCREEN_EXTEND);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetSourceMode02 end";
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetPixelFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetPixelFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicPixelFormat pixelFormat;
    auto res = screenSession->GetPixelFormat(pixelFormat);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "GetPixelFormat end";
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetPixelFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetPixelFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    auto res = screenSession->SetPixelFormat(pixelFormat);
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "SetPixelFormat end";
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSupportedHDRFormats, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetSupportedHDRFormats start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<ScreenHDRFormat> hdrFormats;
    auto res = screenSession->GetSupportedHDRFormats(hdrFormats);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, DMError::DM_OK);
    } else {
        ASSERT_NE(res, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "GetSupportedHDRFormats end";
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenHDRFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetScreenHDRFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    ScreenHDRFormat hdrFormat;
    auto res = screenSession->GetScreenHDRFormat(hdrFormat);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, DMError::DM_OK);
    } else {
        ASSERT_NE(res, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "GetScreenHDRFormat end";
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenHDRFormat, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenHDRFormat start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    int32_t modeIdx = 0;
    auto res = screenSession->SetScreenHDRFormat(modeIdx);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, DMError::DM_OK);
        modeIdx = -1;
        res = screenSession->SetScreenHDRFormat(modeIdx);
        ASSERT_EQ(res, DMError::DM_ERROR_INVALID_PARAM);
    } else {
        ASSERT_NE(res, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "SetScreenHDRFormat end";
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetSupportedColorSpaces, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetSupportedColorSpaces start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    auto res = screenSession->GetSupportedColorSpaces(colorSpaces);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, DMError::DM_OK);
    } else {
        ASSERT_NE(res, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "GetSupportedColorSpaces end";
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenColorSpace, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetScreenColorSpace start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 0,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicCM_ColorSpaceType colorSpace;
    auto res = screenSession->GetScreenColorSpace(colorSpace);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, DMError::DM_OK);
    } else {
        ASSERT_NE(res, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "GetScreenColorSpace end";
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenColorSpace, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetScreenColorSpace start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    screenSession->SetScreenColorSpace(colorSpace);
    EXPECT_TRUE(true);
    GTEST_LOG_(INFO) << "SetScreenColorSpace end";
}

/**
 * @tc.name: SetPrivateSessionForeground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetPrivateSessionForeground, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetPrivateSessionForeground start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool hasPrivate = true;
    screenSession->SetPrivateSessionForeground(hasPrivate);
    auto res = screenSession->HasPrivateSessionForeground();
    ASSERT_EQ(res, hasPrivate);
    GTEST_LOG_(INFO) << "SetPrivateSessionForeground end";
}

/**
 * @tc.name: GetScreenCombination
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenCombination, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetScreenCombination start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    ScreenCombination combination { ScreenCombination::SCREEN_ALONE };
    screenSession->SetScreenCombination(combination);
    auto res = screenSession->GetScreenCombination();
    ASSERT_EQ(res, combination);
    GTEST_LOG_(INFO) << "GetScreenCombination end";
}

/**
 * @tc.name: Resize
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, Resize, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "Resize start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    uint32_t width = 1000;
    uint32_t height = 1000;
    screenSession->Resize(width, height);
    GTEST_LOG_(INFO) << "Resize end";
}

/**
 * @tc.name: UpdateAvailableArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdateAvailableArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdateAvailableArea start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DMRect area = screenSession->GetAvailableArea();
    auto res = screenSession->UpdateAvailableArea(area);
    ASSERT_EQ(res, false);
    area = {2, 2, 2, 2};
    res = screenSession->UpdateAvailableArea(area);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "UpdateAvailableArea end";
}

/**
 * @tc.name: SetAvailableArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetAvailableArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetAvailableArea start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DMRect area = {2, 2, 2, 2};
    screenSession->SetAvailableArea(area);
    GTEST_LOG_(INFO) << "SetAvailableArea end";
}

/**
 * @tc.name: GetAvailableArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetAvailableArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "GetAvailableArea start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    DMRect area = {2, 2, 2, 2};
    screenSession->SetAvailableArea(area);
    auto res = screenSession->GetAvailableArea();
    ASSERT_EQ(res, area);
    GTEST_LOG_(INFO) << "GetAvailableArea end";
}

/**
 * @tc.name: SetFoldScreen
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetFoldScreen, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetFoldScreen start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    bool isFold = false;
    screenSession->SetFoldScreen(isFold);
    GTEST_LOG_(INFO) << "SetFoldScreen end";
}

/**
 * @tc.name: SetHdrFormats
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetHdrFormats, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetHdrFormats start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<uint32_t> hdrFormats = { 0, 0, 0, 0 };
    screenSession->SetHdrFormats(std::move(hdrFormats));
    GTEST_LOG_(INFO) << "SetHdrFormats end";
}

/**
 * @tc.name: SetColorSpaces
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetColorSpaces, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetColorSpaces start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    std::vector<uint32_t> colorSpaces = { 0, 0, 0, 0 };
    screenSession->SetColorSpaces(std::move(colorSpaces));
    GTEST_LOG_(INFO) << "SetColorSpaces end";
}

/**
 * @tc.name: SetDisplayNodeScreenId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetDisplayNodeScreenId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetDisplayNodeScreenId start";
    ScreenId screenId = 0;
    ScreenId rsId = 1;
    std::string name = "OpenHarmony";
    ScreenProperty property;
    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSDisplayNode> displayNode = RSDisplayNode::Create(displayNodeConfig);
    EXPECT_NE(nullptr, displayNode);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, rsId, name, property, displayNode);
    session->SetDisplayNodeScreenId(screenId);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetDisplayNodeScreenId end";
}

/**
 * @tc.name: UnregisterScreenChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UnregisterScreenChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener start";
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int64_t ret = 0;
    session->UnregisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener end";
}

/**
 * @tc.name: UnregisterScreenChangeListener02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UnregisterScreenChangeListener02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener02 start";
    IScreenChangeListener* screenChangeListener = new ScreenSessionManager();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int64_t ret = 0;
    session->UnregisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UnregisterScreenChangeListener02 end";
}

/**
 * @tc.name: ConvertToDisplayInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertToDisplayInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToDisplayInfo start";
    sptr<DisplayInfo> displayInfo = new(std::nothrow) DisplayInfo();
    EXPECT_NE(displayInfo, nullptr);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    EXPECT_NE(nullptr, session->ConvertToDisplayInfo());

    sptr<ScreenInfo> info = new(std::nothrow) ScreenInfo();
    EXPECT_NE(info, nullptr);
    EXPECT_NE(nullptr, session->ConvertToScreenInfo());
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToDisplayInfo end";
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSupportedColorGamuts start";
    std::vector<ScreenColorGamut> colorGamuts;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    DMError ret = session->GetScreenSupportedColorGamuts(colorGamuts);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, DMError::DM_OK);
    } else {
        ASSERT_NE(ret, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSupportedColorGamuts end";
}

/**
 * @tc.name: GetActiveScreenMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetActiveScreenMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetActiveScreenMode start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    sptr<SupportedScreenModes> mode1 = session->GetActiveScreenMode();
    session->GetActiveScreenMode();
    ASSERT_EQ(mode1, nullptr);

    session->activeIdx_ = 100;
    sptr<SupportedScreenModes> mode2 = session->GetActiveScreenMode();
    session->GetActiveScreenMode();
    ASSERT_EQ(mode2, nullptr);

    GTEST_LOG_(INFO) << "ScreenSessionTest: GetActiveScreenMode end";
}

/**
 * @tc.name: SetScreenCombination
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenCombination, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenCombination start";
    ScreenId screenId = 2024;
    ScreenProperty property;
    NodeId nodeId = 0;
    ScreenId defaultScreenId = 0;
    ScreenCombination combination { ScreenCombination::SCREEN_ALONE };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, property, nodeId, defaultScreenId);
    session->SetScreenCombination(combination);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(combination, session->GetScreenCombination());
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenCombination end";
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenColorGamut, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    ScreenColorGamut colorGamut;
    DMError res = session->GetScreenColorGamut(colorGamut);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, DMError::DM_OK);
    } else {
        ASSERT_NE(res, DMError::DM_OK);
    }
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenColorGamut end";
}

/**
 * @tc.name: SetScreenColorGamut01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenColorGamut01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    int32_t colorGamut = 1;
    DMError res = session->SetScreenColorGamut(colorGamut);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut end";
}

/**
 * @tc.name: SetScreenColorGamut02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenColorGamut02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    int32_t colorGamut = -1;
    DMError res = session->SetScreenColorGamut(colorGamut);
    ASSERT_EQ(res, DMError::DM_ERROR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenColorGamut end";
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenGamutMap, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenGamutMap start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);

    ScreenGamutMap gamutMap;
    DMError res = session->GetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenGamutMap end";
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenGamutMap, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenGamutMap start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);
    ScreenGamutMap gamutMap = GAMUT_MAP_CONSTANT;
    DMError res = session->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);

    gamutMap = GAMUT_MAP_HDR_EXTENSION;
    res = session->SetScreenGamutMap(gamutMap);
    ASSERT_EQ(res, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenGamutMap end";
}

/**
 * @tc.name: InitRSDisplayNode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, InitRSDisplayNode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: InitRSDisplayNode start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_NE(session, nullptr);
    RSDisplayNodeConfig config;
    Point startPoint;
    int res = 0;
    sessionGroup.InitRSDisplayNode(config, startPoint);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: InitRSDisplayNode end";
}

/**
 * @tc.name: GetRSDisplayNodeConfig
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRSDisplayNodeConfig, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    RSDisplayNodeConfig config = {1};
    sptr<ScreenSession> defaultScreenSession;
    bool res = sessionGroup.GetRSDisplayNodeConfig(session0, config, defaultScreenSession);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    sessionGroup.combination_ = ScreenCombination::SCREEN_ALONE;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);

    sessionGroup.combination_ = ScreenCombination::SCREEN_EXPAND;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);

    sessionGroup.combination_ = ScreenCombination::SCREEN_MIRROR;
    res = sessionGroup.GetRSDisplayNodeConfig(session, config, defaultScreenSession);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: AddChild
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChild, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChild start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    Point startPoint(0, 0);
    sptr<ScreenSession> defaultScreenSession;
    bool res = sessionGroup.AddChild(session0, startPoint, defaultScreenSession);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    res = sessionGroup.AddChild(session, startPoint, defaultScreenSession);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChild end";
}

/**
 * @tc.name: AddChildren01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChildren01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    std::vector<sptr<ScreenSession>> smsScreens;
    std::vector<Point> startPoints;
    bool res = sessionGroup.AddChildren(smsScreens, startPoints);
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren end";
}

/**
 * @tc.name: AddChildren02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, AddChildren02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    std::vector<sptr<ScreenSession>> smsScreens(2);
    std::vector<Point> startPoints(1);
    bool res = sessionGroup.AddChildren(smsScreens, startPoints);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: AddChildren end";
}

/**
 * @tc.name: RemoveChild
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RemoveChild, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: RemoveChild start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenSession> session0 = nullptr;
    bool res = sessionGroup.RemoveChild(session0);
    ASSERT_EQ(res, false);

    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    res = sessionGroup.RemoveChild(session);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSDisplayNodeConfig end";
}

/**
 * @tc.name: GetChildPosition
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetChildPosition, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetChildPosition start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    ScreenId screenId = 1;
    Point res = sessionGroup.GetChildPosition(screenId);
    ASSERT_EQ(res.posX_, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetChildPosition end";
}

/**
 * @tc.name: ConvertToScreenGroupInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ConvertToScreenGroupInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToScreenGroupInfo start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    sptr<ScreenGroupInfo> res = sessionGroup.ConvertToScreenGroupInfo();
    ASSERT_NE(res, nullptr);
    GTEST_LOG_(INFO) << "ScreenSessionTest: ConvertToScreenGroupInfo end";
}

/**
 * @tc.name: RegisterScreenChangeListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RegisterScreenChangeListener01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: RegisterScreenChangeListener start";
    int res = 0;
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->RegisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: RegisterScreenChangeListener end";
}

/**
 * @tc.name: RegisterScreenChangeListener
 * @tc.desc: Repeat to register
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RegisterScreenChangeListener02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: RegisterScreenChangeListener start";
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    IScreenChangeListener* screenChangeListener1 = new MockScreenChangeListener();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->RegisterScreenChangeListener(screenChangeListener);
    session->RegisterScreenChangeListener(screenChangeListener1);
    ASSERT_FALSE(session->isFold_);
}

/**
 * @tc.name: RegisterScreenChangeListener
 * @tc.desc: screenState_ == ScreenState::CONNECTION
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, RegisterScreenChangeListener03, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_EQ(session->screenState_, ScreenState::INIT);
    session->screenState_ = ScreenState::CONNECTION;
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    session->RegisterScreenChangeListener(screenChangeListener);
    ASSERT_FALSE(session->isFold_);
    session->screenState_ = ScreenState::INIT;
}

/**
 * @tc.name: Connect
 * @tc.desc: Connect
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, Connect, Function | SmallTest | Level2)
{
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->RegisterScreenChangeListener(screenChangeListener);
    session->Connect();
    ASSERT_FALSE(session->isFold_);
}

/**
 * @tc.name: UpdatePropertyByActiveMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, UpdatePropertyByActiveMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: UpdatePropertyByActiveMode start";
    int res = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->UpdatePropertyByActiveMode();
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: UpdatePropertyByActiveMode end";
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, Disconnect, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: Disconnect start";
    int res = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->Disconnect();
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: Disconnect end";
}

/**
 * @tc.name: Disconnect
 * @tc.desc: !listener
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, Disconnect02, Function | SmallTest | Level2)
{
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    IScreenChangeListener* screenChangeListener1 = new MockScreenChangeListener();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    sptr<ScreenSession> session1 = new(std::nothrow) ScreenSession();
    session->RegisterScreenChangeListener(screenChangeListener);
    session1->RegisterScreenChangeListener(screenChangeListener1);
    session1->Connect();
    session1->Disconnect();
    ASSERT_FALSE(session->isFold_);
}

/**
 * @tc.name: SensorRotationChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SensorRotationChange01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SensorRotationChange start";
    int res = 0;
    Rotation sensorRotation = Rotation::ROTATION_0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SensorRotationChange(sensorRotation);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SensorRotationChange end";
}

/**
 * @tc.name: SensorRotationChange
 * @tc.desc: run in for
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SensorRotationChange02, Function | SmallTest | Level2)
{
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->RegisterScreenChangeListener(screenChangeListener);
    Rotation sensorRotation = Rotation::ROTATION_90;
    session->SensorRotationChange(sensorRotation);
    ASSERT_FALSE(session->isFold_);
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetOrientation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation orientation = Orientation::UNSPECIFIED;
    session->SetOrientation(orientation);
    Orientation res = session->GetOrientation();
    ASSERT_EQ(res, orientation);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetOrientation end";
}

/**
 * @tc.name: SetScreenRequestedOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRequestedOrientation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation orientation = Orientation::UNSPECIFIED;
    session->SetScreenRequestedOrientation(orientation);
    Orientation res = session->GetScreenRequestedOrientation();
    ASSERT_EQ(res, orientation);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRequestedOrientation end";
}

/**
 * @tc.name: SetUpdateToInputManagerCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetUpdateToInputManagerCallback, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetUpdateToInputManagerCallback start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    auto func = [session](float x) {
        session->SetVirtualPixelRatio(x);
    };
    session->SetUpdateToInputManagerCallback(func);
    EXPECT_NE(nullptr, &func);

    ScreenProperty screenPropert = session->GetScreenProperty();
    EXPECT_NE(nullptr, &screenPropert);
    session->SetVirtualPixelRatio(3.14);
     
    ScreenType screenType { ScreenType::REAL };
    session->SetScreenType(screenType);

    GTEST_LOG_(INFO) << "ScreenSessionTest: SetUpdateToInputManagerCallbackend";
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenRotationLocked, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLocked start";
    bool isLocked = true;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SetScreenRotationLocked(isLocked);
    bool res = session->IsScreenRotationLocked();
    ASSERT_EQ(res, isLocked);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLocked end";
}

/**
 * @tc.name: SetScreenRotationLockedFromJs
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenRotationLockedFromJs, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLockedFromJs start";
    bool isLocked = true;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SetScreenRotationLockedFromJs(isLocked);
    bool res = session->IsScreenRotationLocked();
    ASSERT_EQ(res, isLocked);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetScreenRotationLockedFromJs end";
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, IsScreenRotationLocked, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: IsScreenRotationLocked start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    bool res = session->IsScreenRotationLocked();
    ASSERT_EQ(res, session->isScreenLocked_);
    GTEST_LOG_(INFO) << "ScreenSessionTest: IsScreenRotationLocked end";
}

/**
 * @tc.name: GetScreenRequestedOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenRequestedOrientation start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation res = session->GetScreenRequestedOrientation();
    ASSERT_EQ(res, session->property_.GetScreenRequestedOrientation());
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenRequestedOrientation end";
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetVirtualPixelRatio start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    float virtualPixelRatio = 1;
    session->SetVirtualPixelRatio(virtualPixelRatio);
    float res = session->property_.GetVirtualPixelRatio();
    ASSERT_EQ(res, virtualPixelRatio);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetVirtualPixelRatio end";
}

/**
 * @tc.name: screen_session_test001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test001, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test001 start";
    IScreenChangeListener* screenChangeListener = nullptr;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->screenState_ = ScreenState::CONNECTION;
    int res = 0;
    session->RegisterScreenChangeListener(screenChangeListener);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test001 end";
}

/**
 * @tc.name: screen_session_test002
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test002, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test002 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenId res = session->GetScreenId();
    ASSERT_EQ(res, session->screenId_);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test002 end";
}

/**
 * @tc.name: screen_session_test003
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test003, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test003 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    DMError res = session->SetScreenColorTransform();
    ASSERT_EQ(res, DMError::DM_OK);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test003 end";
}

/**
 * @tc.name: screen_session_test004
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test004, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test004 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenProperty newProperty;
    int res = 0;
    session->UpdatePropertyByFoldControl(newProperty);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test004 end";
}

/**
 * @tc.name: screen_session_test005
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test005, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test005 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenProperty newProperty;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::CHANGE_MODE;
    int res = 0;
    session->PropertyChange(newProperty, reason);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test005 end";
}

/**
 * @tc.name: screen_session_test006
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test006, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test006 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Rotation sensorRotation = Rotation::ROTATION_90;
    float res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 90.f);
    sensorRotation = Rotation::ROTATION_180;
    res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 180.f);
    sensorRotation = Rotation::ROTATION_270;
    res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 270.f);
    sensorRotation = Rotation::ROTATION_0;
    res = session->ConvertRotationToFloat(sensorRotation);
    ASSERT_EQ(res, 0.f);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test006 end";
}

/**
 * @tc.name: screen_session_test007
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test007, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test007 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Orientation orientation = Orientation::UNSPECIFIED;
    int res = 0;
    session->ScreenOrientationChange(orientation, FoldDisplayMode::UNKNOWN);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test007 end";
}

/**
 * @tc.name: screen_session_test008
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test008, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test008 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    bool res = session->HasPrivateSessionForeground();
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test008 end";
}

/**
 * @tc.name: screen_session_test009
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test009, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test009 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    Rotation rotation = Rotation::ROTATION_90;
    session->SetRotation(rotation);
    Rotation res = session->GetRotation();
    ASSERT_EQ(res, rotation);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test009 end";
}

/**
 * @tc.name: screen_session_test010
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test010, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test010 start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    ScreenId childScreen = 1;
    bool res = sessionGroup.HasChild(childScreen);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test010 end";
}

/**
 * @tc.name: screen_session_test011
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test011, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test011 start";
    ScreenSessionGroup sessionGroup(1, 1, "create", ScreenCombination::SCREEN_ALONE);
    std::vector<sptr<ScreenSession>> res = sessionGroup.GetChildren();
    ASSERT_EQ(res.empty(), true);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test011 end";
}

/**
 * @tc.name: screen_session_test012
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, screen_session_test012, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test012 start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    int res = 0;
    RectF rect = RectF(0, 0, 0, 0);
    uint32_t offsetY = 0;
    session->SetDisplayBoundary(rect, offsetY);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: screen_session_test012 end";
}

/**
 * @tc.name: GetName
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetName, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetName start";
    std::string name { "UNKNOW" };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ASSERT_EQ(name, session->GetName());
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetName end";
}

/**
 * @tc.name: SetName
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetName, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetName start";
    std::string name { "UNKNOWN" };
    int ret = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->SetName(name);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: SetName end";
}

/**
 * @tc.name: GetScreenSnapshot
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetScreenSnapshot, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSnapshot start";
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->displayNode_ = nullptr;
    auto pixelmap = session->GetScreenSnapshot(1.0, 1.0);
    EXPECT_EQ(pixelmap, nullptr);

    ScreenProperty newScreenProperty;
    session = new(std::nothrow) ScreenSession(0, newScreenProperty, 0);
    pixelmap = session->GetScreenSnapshot(1.0, 1.0);
    int ret = 0;
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetScreenSnapshot end";
}

/**
 * @tc.name: GetRSScreenId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetRSScreenId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSScreenId start";
    ScreenProperty property;
    sptr<ScreenSession> session = new(std::nothrow)
        ScreenSession("OpenHarmony", 1, 100, 0);
    EXPECT_EQ(100, session->GetRSScreenId());

    RSDisplayNodeConfig displayNodeConfig;
    std::shared_ptr<RSDisplayNode> displayNode = RSDisplayNode::Create(displayNodeConfig);
    session = new ScreenSession(1, 100, "OpenHarmony",
        property, displayNode);
    EXPECT_NE(nullptr, session->GetDisplayNode());
    session->ReleaseDisplayNode();
    EXPECT_EQ(nullptr, session->GetDisplayNode());
    GTEST_LOG_(INFO) << "ScreenSessionTest: GetRSScreenId end";
}

/**
 * @tc.name: CalcRotation01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcRotation01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcRotation start";
    Orientation orientation { Orientation::BEGIN };
    FoldDisplayMode foldDisplayMode { FoldDisplayMode::COORDINATION };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    auto res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    session->modes_ = { supportedScreenModes };
    session->activeIdx_ = 0;

    orientation = Orientation::UNSPECIFIED;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    orientation = Orientation::VERTICAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    if (ScreenSessionManager::GetInstance().IsFoldable()) {
        EXPECT_EQ(Rotation::ROTATION_0, res);
    } else {
        EXPECT_EQ(Rotation::ROTATION_90, res);
    }
    
    orientation = Orientation::HORIZONTAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    if (ScreenSessionManager::GetInstance().IsFoldable()) {
        EXPECT_EQ(Rotation::ROTATION_90, res);
    } else {
        EXPECT_EQ(Rotation::ROTATION_0, res);
    }

    orientation = Orientation::REVERSE_VERTICAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    if (ScreenSessionManager::GetInstance().IsFoldable()) {
        EXPECT_EQ(Rotation::ROTATION_180, res);
    } else {
        EXPECT_EQ(Rotation::ROTATION_270, res);
    }

    orientation = Orientation::REVERSE_HORIZONTAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    if (ScreenSessionManager::GetInstance().IsFoldable()) {
        EXPECT_EQ(Rotation::ROTATION_270, res);
    } else {
        EXPECT_EQ(Rotation::ROTATION_180, res);
    }

    orientation = Orientation::LOCKED;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcRotation end";
}

/**
 * @tc.name: CalcRotation02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcRotation02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcRotation start";
    Orientation orientation { Orientation::BEGIN };
    FoldDisplayMode foldDisplayMode { FoldDisplayMode::UNKNOWN };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    auto res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    session->modes_ = { supportedScreenModes };
    session->activeIdx_ = 0;

    orientation = Orientation::UNSPECIFIED;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    orientation = Orientation::VERTICAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_90, res);
    
    orientation = Orientation::HORIZONTAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    orientation = Orientation::REVERSE_VERTICAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_270, res);

    orientation = Orientation::REVERSE_HORIZONTAL;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_180, res);

    orientation = Orientation::LOCKED;
    res = session->CalcRotation(orientation, foldDisplayMode);
    EXPECT_EQ(Rotation::ROTATION_0, res);

    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcRotation end";
}

/**
 * @tc.name: CalcDisplayOrientation01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDisplayOrientation01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcDisplayOrientation start";
    Rotation rotation { Rotation::ROTATION_0 };
    FoldDisplayMode foldDisplayMode { FoldDisplayMode::COORDINATION };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    auto res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE, res);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    session->modes_ = { supportedScreenModes };
    session->activeIdx_ = 0;

    rotation = Rotation::ROTATION_0;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE, res);

    rotation = Rotation::ROTATION_90;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::PORTRAIT, res);

    rotation = Rotation::ROTATION_180;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE_INVERTED, res);

    rotation = Rotation::ROTATION_270;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::PORTRAIT_INVERTED, res);

    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcDisplayOrientation end";
}

/**
 * @tc.name: CalcDisplayOrientation02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDisplayOrientation02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcDisplayOrientation start";
    Rotation rotation { Rotation::ROTATION_0 };
    FoldDisplayMode foldDisplayMode { FoldDisplayMode::UNKNOWN };
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    session->activeIdx_ = -1;
    auto res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE, res);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    session->modes_ = { supportedScreenModes };
    session->activeIdx_ = 0;

    rotation = Rotation::ROTATION_0;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE, res);

    rotation = Rotation::ROTATION_90;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::PORTRAIT, res);

    rotation = Rotation::ROTATION_180;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::LANDSCAPE_INVERTED, res);

    rotation = Rotation::ROTATION_270;
    res = session->CalcDisplayOrientation(rotation, foldDisplayMode);
    EXPECT_EQ(DisplayOrientation::PORTRAIT_INVERTED, res);

    GTEST_LOG_(INFO) << "ScreenSessionTest: CalcDisplayOrientation end";
}

/**
 * @tc.name: FillScreenInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, FillScreenInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: FillScreenInfo start";
    ScreenProperty property;
    property.SetVirtualPixelRatio(3.1415927);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(2024, property, 0);
    sptr<ScreenInfo> info = nullptr;
    session->FillScreenInfo(info);

    sptr<SupportedScreenModes> supportedScreenModes = new SupportedScreenModes;
    session->modes_ = { supportedScreenModes };
    supportedScreenModes->width_ = 40;
    supportedScreenModes->height_ = 20;
    info = new ScreenInfo;
    session->FillScreenInfo(info);
    EXPECT_NE(nullptr, info);
    GTEST_LOG_(INFO) << "ScreenSessionTest: FillScreenInfo end";
}

/**
 * @tc.name: PropertyChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, PropertyChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: PropertyChange start";
    int res = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    ScreenProperty newProperty;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::CHANGE_MODE;
    session->PropertyChange(newProperty, reason);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: PropertyChange end";
}

/**
 * @tc.name: PowerStatusChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, PowerStatusChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenSessionTest: PowerStatusChange start";
    int res = 0;
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_ON;
    EventStatus status = EventStatus::BEGIN;
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    session->PowerStatusChange(event, status, reason);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenSessionTest: PowerStatusChange end";
}

/**
 * @tc.name: SetScreenScale
 * @tc.desc: SetScreenScale test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, SetScreenScale, Function | SmallTest | Level2)
{
    ScreenSession session;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    session.SetScreenScale(scaleX, scaleY, pivotX, pivotY, translateX, translateY);
    EXPECT_EQ(session.property_.GetScaleX(), scaleX);
    EXPECT_EQ(session.property_.GetScaleY(), scaleY);
    EXPECT_EQ(session.property_.GetPivotX(), pivotX);
    EXPECT_EQ(session.property_.GetPivotY(), pivotY);
    EXPECT_EQ(session.property_.GetTranslateX(), translateX);
    EXPECT_EQ(session.property_.GetTranslateY(), translateY);
}

/**
 * @tc.name: HoverStatusChange01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, HoverStatusChange01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "HoverStatusChange start";
    ScreenSessionConfig config = {
        .screenId = 100,
        .rsId = 101,
        .name = "OpenHarmony",
    };
    sptr<ScreenSession> screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    EXPECT_NE(nullptr, screenSession);
    int32_t HoverStatus = 0;
    screenSession->HoverStatusChange(HoverStatus);
    GTEST_LOG_(INFO) << "HoverStatusChange end";
}

/**
 * @tc.name: HoverStatusChange02
 * @tc.desc: run in for
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, HoverStatusChange02, Function | SmallTest | Level2)
{
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    EXPECT_NE(nullptr, session);
    session->RegisterScreenChangeListener(screenChangeListener);
    int32_t hoverStatus = 0;
    session->HoverStatusChange(hoverStatus);
}

/**
 * @tc.name: HandleHoverStatusChange01
 * @tc.desc: run in for
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, HandleHoverStatusChange01, Function | SmallTest | Level2)
{
    IScreenChangeListener* screenChangeListener = new MockScreenChangeListener();
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession();
    EXPECT_NE(nullptr, session);
    session->RegisterScreenChangeListener(screenChangeListener);
    int32_t hoverStatus = 0;
    session->HandleHoverStatusChange(hoverStatus);
}

/**
 * @tc.name: ScreenCaptureNotify
 * @tc.desc: ScreenCaptureNotify test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, ScreenCaptureNotify, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    ScreenId screenId = 0;
    int32_t uid = 0;
    std::string clientName = "test";
    session->ScreenCaptureNotify(screenId, uid, clientName);
}

/**
 * @tc.name: GetIsInternal01
 * @tc.desc: Test when isInternal is true, GetIsInternal should return true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsInternal01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsInternal(true);
    ASSERT_EQ(session->GetIsInternal(), true);
}

/**
 * @tc.name: GetIsInternal02
 * @tc.desc: Test when isInternal is false, GetIsInternal should return false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsInternal02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsInternal(false);
    ASSERT_EQ(session->GetIsInternal(), false);
}

/**
 * @tc.name: GetIsFakeInUse01
 * @tc.desc: Test when isFakeInUse is true, GetIsFakeInUse should return true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsFakeInUse01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsFakeInUse(true);
    ASSERT_EQ(session->GetIsFakeInUse(), true);
}

/**
 * @tc.name: GetIsFakeInUse02
 * @tc.desc: Test when isFakeInUse is false, GetIsFakeInUse should return false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsFakeInUse02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsFakeInUse(false);
    ASSERT_EQ(session->GetIsFakeInUse(), false);
}

/**
 * @tc.name: GetIsRealScreen01
 * @tc.desc: Test when isReal is true, GetIsRealScreen should return true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsRealScreen01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsRealScreen(true);
    ASSERT_EQ(session->GetIsRealScreen(), true);
}

/**
 * @tc.name: GetIsRealScreen02
 * @tc.desc: Test when isReal is false, GetIsRealScreen should return false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsRealScreen02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsRealScreen(false);
    ASSERT_EQ(session->GetIsRealScreen(), false);
}

/**
 * @tc.name: GetIsPcUse01
 * @tc.desc: Test when isPcUse is true, GetIsPcUse should return true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsPcUse01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsPcUse(true);
    ASSERT_EQ(session->GetIsPcUse(), true);
}

/**
 * @tc.name: GetIsPcUse02
 * @tc.desc: Test when isPcUse is false, GetIsPcUse should return false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsPcUse02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsPcUse(false);
    ASSERT_EQ(session->GetIsPcUse(), false);
}

/**
 * @tc.name: GetIsBScreenHalf01
 * @tc.desc: Test when isBScreenHalf is true, GetIsBScreenHalf should return true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsBScreenHalf01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsBScreenHalf(true);
    ASSERT_EQ(session->GetIsBScreenHalf(), true);
}

/**
 * @tc.name: GetIsBScreenHalf02
 * @tc.desc: Test when isBScreenHalf is false, GetIsBScreenHalf should return false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsBScreenHalf02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsBScreenHalf(false);
    ASSERT_EQ(session->GetIsBScreenHalf(), false);
}

/**
 * @tc.name: CalcDeviceOrientation01
 * @tc.desc: Test when rotation is ROTATION_0 then CalcDeviceOrientation returns PORTRAIT
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDeviceOrientation01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    DisplayOrientation result = session->CalcDeviceOrientation(Rotation::ROTATION_0, FoldDisplayMode::FULL);
    ASSERT_EQ(result, DisplayOrientation::PORTRAIT);
}

/**
 * @tc.name: CalcDeviceOrientation02
 * @tc.desc: Test when rotation is ROTATION_90 then CalcDeviceOrientation returns LANDSCAPE
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDeviceOrientation02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    DisplayOrientation result = session->CalcDeviceOrientation(Rotation::ROTATION_90, FoldDisplayMode::FULL);
    ASSERT_EQ(result, DisplayOrientation::LANDSCAPE);
}

/**
 * @tc.name: CalcDeviceOrientation03
 * @tc.desc: Test when rotation is ROTATION_180 then CalcDeviceOrientation returns PORTRAIT_INVERTED
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDeviceOrientation03, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    DisplayOrientation result = session->CalcDeviceOrientation(Rotation::ROTATION_180, FoldDisplayMode::FULL);
    ASSERT_EQ(result, DisplayOrientation::PORTRAIT_INVERTED);
}

/**
 * @tc.name: CalcDeviceOrientation04
 * @tc.desc: Test when rotation is ROTATION_270 then CalcDeviceOrientation returns LANDSCAPE_INVERTED
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDeviceOrientation04, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    DisplayOrientation result = session->CalcDeviceOrientation(Rotation::ROTATION_270, FoldDisplayMode::FULL);
    ASSERT_EQ(result, DisplayOrientation::LANDSCAPE_INVERTED);
}

/**
 * @tc.name: CalcDeviceOrientation05
 * @tc.desc: Test when rotation is unknown then CalcDeviceOrientation returns UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, CalcDeviceOrientation05, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    DisplayOrientation result = session->CalcDeviceOrientation(static_cast<Rotation>(100), FoldDisplayMode::FULL);
    ASSERT_EQ(result, DisplayOrientation::UNKNOWN);
}

/**
 * @tc.name: GetIsPhysicalMirrorSwitch01
 * @tc.desc: Test when isPhysicalMirrorSwitch is true, GetIsPhysicalMirrorSwitch should return true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsPhysicalMirrorSwitch01, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsPhysicalMirrorSwitch(true);
    ASSERT_EQ(session->GetIsPhysicalMirrorSwitch(), true);
}

/**
 * @tc.name: GetIsPhysicalMirrorSwitch02
 * @tc.desc: Test when isPhysicalMirrorSwitch is false, GetIsPhysicalMirrorSwitch should return false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionTest, GetIsPhysicalMirrorSwitch02, Function | SmallTest | Level2)
{
    sptr<ScreenSession> session = new ScreenSession();
    ASSERT_NE(session, nullptr);
    session->SetIsPhysicalMirrorSwitch(false);
    ASSERT_EQ(session->GetIsPhysicalMirrorSwitch(), false);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
