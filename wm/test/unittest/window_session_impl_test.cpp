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

#include <gtest/gtest.h>
#include "ability_context_impl.h"
#include "mock_session.h"
#include "window_session_impl.h"
#include "mock_uicontent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MockWindowChangeListener : public IWindowChangeListener {
public:
    MOCK_METHOD3(OnSizeChange, void(Rect rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction));
};

class MockWindowLifeCycleListener : public IWindowLifeCycle {
public:
    MOCK_METHOD0(AfterForeground, void(void));
    MOCK_METHOD0(AfterBackground, void(void));
    MOCK_METHOD0(AfterFocused, void(void));
    MOCK_METHOD0(AfterUnfocused, void(void));
    MOCK_METHOD1(ForegroundFailed, void(int32_t));
    MOCK_METHOD0(AfterActive, void(void));
    MOCK_METHOD0(AfterInactive, void(void));
};

class WindowSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};

void WindowSessionImplTest::SetUpTestCase()
{
}

void WindowSessionImplTest::TearDownTestCase()
{
}

void WindowSessionImplTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSessionImplTest::TearDown()
{
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: CreateWindowAndDestroy01
 * @tc.desc: Create window and destroy window
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, CreateWindowAndDestroy01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());
    // session is null
    window = new WindowSessionImpl(option);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, nullptr));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());

    window = new WindowSessionImpl(option);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
}

/**
 * @tc.name: Connect01
 * @tc.desc: Connect session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, Connect01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // connect with null session
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Connect());

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Connect());
    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Connect());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Show01
 * @tc.desc: Show session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, Show01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Show01");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Show());

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), Foreground(_)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Show());
    window->state_ = WindowState::STATE_CREATED;
    EXPECT_CALL(*(session), Foreground(_)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Hide01
 * @tc.desc: Hide session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, Hide01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Hide01");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Hide());

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->type_ = WindowType::WINDOW_TYPE_FLOAT;
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowType01
 * @tc.desc: SetWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, SetWindowType01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetWindowType01 start";
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(window->property_, nullptr);

    window->property_->SetPersistentId(1);
    option->SetWindowName("SetWindowType01");
    WindowType type = WindowType::WINDOW_TYPE_BOOT_ANIMATION;
    option->SetWindowType(type);
    window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WindowType type1 = WindowType::WINDOW_TYPE_POINTER;
    option->SetWindowType(type1);
    window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WindowType type2 = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    option->SetWindowType(type2);
    window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WindowType type3 = WindowType::APP_MAIN_WINDOW_END;
    option->SetWindowType(type3);
    window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetWindowType01 end";
}

/**
 * @tc.name: ColorSpace
 * @tc.desc: SetColorSpace and GetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, ColorSpace, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: ColorSpace start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ColorSpace");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);

    window->SetColorSpace(ColorSpace::COLOR_SPACE_DEFAULT);
    ColorSpace colorSpace = window->GetColorSpace();
    ASSERT_EQ(colorSpace, ColorSpace::COLOR_SPACE_DEFAULT);

    window->SetColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    ColorSpace colorSpace1 = window->GetColorSpace();
    ASSERT_EQ(colorSpace1, ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: ColorSpace end";
}

/**
 * @tc.name: WindowSessionCreateCheck01
 * @tc.desc: WindowSessionCreateCheck01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, WindowSessionCreateCheck01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: WindowSessionCreateCheck01 start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);

    sptr<WindowOption> option1 = new WindowOption();
    option1->SetWindowName("WindowSessionCreateCheck"); // set the same name
    sptr<WindowSessionImpl> window1 =
        new (std::nothrow) WindowSessionImpl(option1);
    ASSERT_NE(nullptr, window1);

    WMError res = window1->WindowSessionCreateCheck();
    ASSERT_EQ(res, WMError::WM_OK);

    window1->property_ = nullptr;
    WMError res1 = window1->WindowSessionCreateCheck();
    ASSERT_EQ(res1, WMError::WM_ERROR_NULLPTR);

    GTEST_LOG_(INFO) << "WindowSessionImplTest: WindowSessionCreateCheck01 end";
}

/**
 * @tc.name: WindowSessionCreateCheck02
 * @tc.desc: WindowSessionCreateCheck02
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, WindowSessionCreateCheck02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: WindowSessionCreateCheck02 start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    WMError res1 = window->WindowSessionCreateCheck();
    ASSERT_EQ(res1, WMError::WM_OK);

    GTEST_LOG_(INFO) << "WindowSessionImplTest: WindowSessionCreateCheck02 end";
}

/**
 * @tc.name: SetActive
 * @tc.desc: SetActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, SetActive, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetActive start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WSError res1 = window->SetActive(true);
    ASSERT_EQ(res1, WSError::WS_OK);
    res1 = window->SetActive(false);
    ASSERT_EQ(res1, WSError::WS_OK);

    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetActive end";
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, UpdateRect01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateRect01 start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.height_ = 50;
    rect.width_ = 50;

    Rect rectW; // GetRect().IsUninitializedRect is false
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 200; // rectW - rect > 50
    rectW.width_ = 200;  // rectW - rect > 50

    window->property_->SetWindowRect(rectW);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rectW.height_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rectW.height_ = 200;
    rectW.width_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateRect01 end";
}

/**
 * @tc.name: UpdateRect02
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, UpdateRect02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateRect02 start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.height_ = 0;
    rect.width_ = 0;

    Rect rectW; // GetRect().IsUninitializedRect is true
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 0; // rectW - rect > 50
    rectW.width_ = 0;  // rectW - rect > 50

    window->property_->SetWindowRect(rectW);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rect.height_ = 50;
    rect.width_ = 50;
    rectW.height_ = 50;
    rectW.width_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateRect02 end";
}

/**
 * @tc.name: UpdateFocus
 * @tc.desc: UpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, UpdateFocus, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateFocus start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WSError res = window->UpdateFocus(true);
    ASSERT_EQ(res, WSError::WS_OK);
    res = window->UpdateFocus(false);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateFocus end";
}

/**
 * @tc.name: UpdateViewportConfig
 * @tc.desc: UpdateViewportConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, UpdateViewportConfig, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateViewportConfig start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    Rect rectW; // GetRect().IsUninitializedRect is true
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 0; // rectW - rect > 50
    rectW.width_ = 0;  // rectW - rect > 50

    WindowSizeChangeReason reason = WindowSizeChangeReason::UNDEFINED;
    int32_t res = 0;
    window->UpdateViewportConfig(rectW, reason);
    ASSERT_EQ(res, 0);

    DisplayId displayId = 1;
    window->property_->SetDisplayId(displayId);
    window->UpdateViewportConfig(rectW, reason);
    ASSERT_EQ(res, 0);

    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateViewportConfig end";
}

/**
 * @tc.name: CreateWindowAndDestroy01
 * @tc.desc: GetPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, GetPersistentId01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetPersistentId start";
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(window->property_, nullptr);

    window->property_->SetPersistentId(1);
    const int32_t res2 = window->GetPersistentId();
    ASSERT_EQ(res2, 1);

    window->property_ = nullptr;
    const int32_t res1 = window->GetPersistentId();
    ASSERT_EQ(res1, INVALID_SESSION_ID);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetPersistentId end";
}

/**
 * @tc.name: GetFloatingWindowParentId
 * @tc.desc: GetFloatingWindowParentId and UpdateTitleButtonVisibility
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, GetFloatingWindowParentId, Function | SmallTest | Level2) {
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetFloatingWindowParentId start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // connect with null session
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Connect());

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _))
        .WillOnce(Return(WSError::WS_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Connect());
    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _))
        .WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Connect());

    window->UpdateTitleButtonVisibility();
    int32_t res = window->GetFloatingWindowParentId();
    ASSERT_EQ(res, INVALID_SESSION_ID);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetFloatingWindowParentId start";
}

/**
 * @tc.name: UpdateDecorEnable
 * @tc.desc: UpdateDecorEnable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, UpdateDecorEnable, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateDecorEnable start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("UpdateDecorEnable");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    int res = 1;
    window->UpdateDecorEnable(true);
    window->UpdateDecorEnable(false);
    ASSERT_EQ(res, 1);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateDecorEnable end";
}

/**
 * @tc.name: NotifyModeChange
 * @tc.desc: NotifyModeChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, NotifyModeChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: NotifyModeChange start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("NotifyModeChange");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WindowMode mode = WindowMode::WINDOW_MODE_UNDEFINED;
    int res = 1;
    window->NotifyModeChange(mode, true);
    window->NotifyModeChange(mode, false);
    ASSERT_EQ(res, 1);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: NotifyModeChange end";
}

/**
 * @tc.name: SetFocusable
 * @tc.desc: SetFocusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, SetFocusable, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetFocusable start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetFocusable");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->property_->SetPersistentId(1);

    WMError res = window->SetFocusable(true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());

    // session is null
    window = new WindowSessionImpl(option);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, nullptr));
    res = window->SetFocusable(true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    res = window->SetFocusable(false);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetFocusable end";
}

/**
 * @tc.name: SetTouchable
 * @tc.desc: SetTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, SetTouchable, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetTouchable start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTouchable");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->property_->SetPersistentId(1);

    WMError res = window->SetTouchable(true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());

    // session is null
    window = new WindowSessionImpl(option);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, nullptr));
    res = window->SetTouchable(true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    res = window->SetTouchable(false);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetTouchable end";
}

/**
 * @tc.name: SetBrightness01
 * @tc.desc: SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, SetBrightness01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetBrightness01 start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBrightness01");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->property_->SetPersistentId(1);

    float brightness = -1.0; // brightness < 0
    WMError res = window->SetBrightness(brightness);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PARAM);
    brightness = 2.0; // brightness > 1
    res = window->SetBrightness(brightness);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PARAM);

    brightness = 0.5;
    res = window->SetBrightness(brightness);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetBrightness01 end";
}

/**
 * @tc.name: SetBrightness02
 * @tc.desc: SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest, SetBrightness02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetBrightness02 start";
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBrightness02");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    float brightness = 0.5;

    WMError res = window->SetBrightness(brightness);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_TYPE);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetBrightness02 end";
}
}
} // namespace Rosen
} // namespace OHOS
