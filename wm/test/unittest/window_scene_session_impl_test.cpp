/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
#include "window_scene_session_impl.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class MockWindowChangeListener : public IWindowChangeListener {
public:
    MOCK_METHOD3(OnSizeChange,
        void(Rect rect, WindowSizeChangeReason reason, const std::shared_ptr<RSTransaction> &rsTransaction));
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

class WindowSceneSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
};

void WindowSceneSessionImplTest::SetUpTestCase() {}

void WindowSceneSessionImplTest::TearDownTestCase() {}

void WindowSceneSessionImplTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest::TearDown()
{
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: CreateWindowAndDestroy01
 * @tc.desc: Create window and destroy window
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateWindowAndDestroy01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Create(abilityContext_, session));
}

/**
 * @tc.name: CreateAndConnectSpecificSession01
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowscenesession->CreateAndConnectSpecificSession());

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
}

/**
 * @tc.name: IsValidSystemWindowType01
 * @tc.desc: IsValidSystemWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, IsValidSystemWindowType01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_DIALOG));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_FLOAT));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_SCREENSHOT));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_VOICE_INTERACTION));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_POINTER));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_TOAST));
    ASSERT_TRUE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE));
    ASSERT_TRUE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_APP_LAUNCHING));
}

/*
 * @tc.name: InvalidWindow
 * @tc.desc: InvalidWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, InvalidWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MoveTo(0, 0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Resize(0, 0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetBackgroundColor(std::string("???")));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransparent(false));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Show(2, false));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Resize(2, 2));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Minimize());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Maximize());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MaximizeFloating());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Recover());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MaximizeFloating());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetGlobalMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR));
}

/**
 * @tc.name: FindParentSessionByParentId01
 * @tc.desc: FindParentSessionByParentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, FindParentSessionByParentId01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> parentscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentscenesession);
    parentscenesession->property_->SetPersistentId(100);
    WindowSessionImpl::windowSessionMap_["parentWindow"] = std::make_pair(1, parentscenesession);

    sptr<WindowOption> option_ = new WindowOption();
    option_->SetWindowTag(WindowTag::MAIN_WINDOW);
    option_->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> childscenesession = new (std::nothrow) WindowSceneSessionImpl(option_);
    ASSERT_TRUE(nullptr == childscenesession->FindParentSessionByParentId(101));
    childscenesession->property_->SetPersistentId(101);
    childscenesession->property_->SetParentId(100);
    childscenesession->property_->SetParentPersistentId(100);

    sptr<WindowSessionImpl> parent = childscenesession->FindParentSessionByParentId(101);
    ASSERT_FALSE(nullptr != childscenesession->FindParentSessionByParentId(101));
}

/**
 * @tc.name: FindMainWindowWithContext01
 * @tc.desc: FindMainWindowWithContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, FindMainWindowWithContext01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> parentscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentscenesession);
    parentscenesession->property_->SetPersistentId(100);
    parentscenesession->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_FALSE(parentscenesession->FindMainWindowWithContext() == nullptr);
    parentscenesession->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_TRUE(parentscenesession->FindMainWindowWithContext() == nullptr);
}

/**
 * @tc.name: DisableAppWindowDecor01
 * @tc.desc: DisableAppWindowDecor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, DisableAppWindowDecor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSessionImpl> windowession = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, windowession);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);

    ASSERT_NE(nullptr, session);
    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(WMError::WM_OK, windowession->Create(context, session));

    windowession->UpdateDecorEnable(false);
    windowession->windowSystemConfig_.isSystemDecorEnable_ = false;

    windowession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    windowession->DisableAppWindowDecor();
    ASSERT_FALSE(windowession->IsDecorEnable());
    windowession->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    windowession->DisableAppWindowDecor();
}

/**
 * @tc.name: HandleBackEvent01
 * @tc.desc: HandleBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, HandleBackEvent01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    std::shared_ptr<AbilityRuntime::AbilityContext> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    windowscenesession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker *content = reinterpret_cast<Ace::UIContentMocker *>(windowscenesession->uiContent_.get());
    EXPECT_CALL(*content, ProcessBackPressed()).WillOnce(Return(false));
    ASSERT_EQ(WSError::WS_OK, windowscenesession->HandleBackEvent());
}

/**
 * @tc.name: RaiseToAppTop01
 * @tc.desc: RaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, RaiseToAppTop01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(6);
    windowscenesession->property_->SetParentPersistentId(6);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_CALLING, windowscenesession->RaiseToAppTop());

    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowscenesession->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, windowscenesession->RaiseToAppTop());

    windowscenesession->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), RaiseToAppTop()).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WmErrorCode::WM_OK, windowscenesession->RaiseToAppTop());
}

/**
 * @tc.name: Resize01
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Resize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->state_ = WindowState::STATE_UNFROZEN;
}

/**
 * @tc.name: MoveTo01
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, MoveTo01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->state_ = WindowState::STATE_HIDDEN;
}

/**
 * @tc.name: Minimize01
 * @tc.desc: Minimize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Minimize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_MINIMIZE)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Minimize());
}

/**
 * @tc.name: StartMove01
 * @tc.desc: StartMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, StartMove01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    // show with null session

    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_START_MOVE)).WillOnce(Return(WSError::WS_OK));
    windowscenesession->StartMove();
}

/**
 * @tc.name: Close01
 * @tc.desc: Close
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Close01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_CLOSE)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Close());
}

/**
 * @tc.name: SetActive01
 * @tc.desc: SetActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetActive01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), UpdateActiveStatus(false)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, windowscenesession->SetActive(false));
}

/**
 * @tc.name: Recover01
 * @tc.desc: Recover
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Recover01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_RECOVER)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Recover());
}

/**
 * @tc.name: Maximize01
 * @tc.desc: Maximize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Maximize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
}

/**
 * @tc.name: Hide01
 * @tc.desc: Hide session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Hide01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Hide01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Hide(2, false, false));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));

    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    EXPECT_CALL(*(session), Background()).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));

    window->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Destroy(false));
}

/**
 * @tc.name: Show01
 * @tc.desc: Show session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Show01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Show01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    window->hostSession_ = session;
    EXPECT_CALL(*(session), Foreground()).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show(2, false));

    window->state_ = WindowState::STATE_CREATED;
    EXPECT_CALL(*(session), Foreground()).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Show(2, false));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
}

/**
 * @tc.name: SetBackgroundColor01
 * @tc.desc: test SetBackgroundColor withow uiContent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetBackgroundColor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBackgroundColor01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(WMError::WM_OK, window->Create(context, session));

    window->property_->SetPersistentId(1);
    window->Show();
}

/*
 * @tc.name: SetTransparent
 * @tc.desc: SetTransparent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetTransparent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransparent(true));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
}

/*
 * @tc.name: SetAspectRatio
 * @tc.desc: SetAspectRatio test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetAspectRatio, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetAspectRatio(0.1));

    window->property_->SetPersistentId(1);
    window->property_->SetDisplayId(3);
    WindowLimits windowLimits = { 3, 3, 3, 3, 2.0, 2.0 };
    window->property_->SetWindowLimits(windowLimits);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->SetAspectRatio(0.1));
}

/*
 * @tc.name: ResetAspectRatio
 * @tc.desc: ResetAspectRatio test GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, ResetAspectRatio, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), SetAspectRatio(_)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->ResetAspectRatio());
}

/*
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, GetAvoidAreaByType, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    AvoidArea avoidarea;
    ASSERT_EQ(WMError::WM_OK, window->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidarea));
}

/*
 * @tc.name: LimitCameraFloatWindowMininumSize
 * @tc.desc: LimitCameraFloatWindowMininumSize01 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, LimitCameraFloatWindowMininumSize, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    uint32_t width =33;
    uint32_t height =31;
    window->LimitCameraFloatWindowMininumSize(width, height);
}
}
} // namespace Rosen
} // namespace OHOS
