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
#include "window_scene_session_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
    using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
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

class WindowSceneSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
};

void WindowSceneSessionImplTest::SetUpTestCase()
{
}

void WindowSceneSessionImplTest::TearDownTestCase()
{
}

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
HWTEST_F(WindowSceneSessionImplTest, Create01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Create(nullptr, nullptr));
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(nullptr, session));
    EXPECT_CALL(*(session), Connect(_, _, _,_, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));

    EXPECT_CALL(*(session), Connect(_, _, _, _,_, _)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Create(abilityContext_, session));
    // session is null
    window = new WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(abilityContext_, nullptr));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));

}

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
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(nullptr, session));
    EXPECT_CALL(*(session), Connect(_, _, _,_, _, _)).WillOnce(Return(WSError::WS_OK));

    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));

    EXPECT_CALL(*(session), Connect(_, _, _,_, _, _)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Create(abilityContext_, session));
    // session is null
    window = new WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(abilityContext_, nullptr));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CreateAndConnectSpecificSession());

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), CreateAndConnectSpecificSession(_, _, _, _, _,_)).WillOnce(Return(WSError::WS_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowscenesession->CreateAndConnectSpecificSession());
    
    EXPECT_CALL(*(session), CreateAndConnectSpecificSession(_, _, _, _, _,_)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->CreateAndConnectSpecificSession());

    windowscenesession->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    EXPECT_CALL(*(session), CreateAndConnectSpecificSession(_, _, _, _, _,_)).WillOnce(Return(WSError::WS_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowscenesession->CreateAndConnectSpecificSession());
    EXPECT_CALL(*(session), CreateAndConnectSpecificSession(_, _, _, _, _,_)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->CreateAndConnectSpecificSession());
    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
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
    sptr<WindowSceneSessionImpl> parentscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentscenesession);
    parentscenesession->property_->SetPersistentId(100);
    WindowSessionImpl::windowSessionMap_["parentWindow"]= std::make_pair(1,parentscenesession);

    sptr<WindowOption> option_ = new WindowOption();
    option_->SetWindowTag(WindowTag::MAIN_WINDOW);
    option_->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> childscenesession = new(std::nothrow) WindowSceneSessionImpl(option_);
    ASSERT_NE(nullptr, childscenesession);
    childscenesession->property_->SetPersistentId(101);
    childscenesession->property_->SetParentId(100);
    childscenesession->property_->SetParentPersistentId(100);
    WindowSessionImpl::windowSessionMap_["childWindow"]= std::make_pair(2,childscenesession);

    sptr<WindowSessionImpl> parent=childscenesession->FindParentSessionByParentId(101);
    ASSERT_NE(nullptr, parent);
    parentscenesession->DestroySubWindow();
    ASSERT_NE(nullptr, childscenesession);

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
    sptr<WindowSceneSessionImpl> parentscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentscenesession);
    parentscenesession->property_->SetPersistentId(100);
    parentscenesession->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_NE(nullptr, parentscenesession->FindMainWindowWithContext());

    parentscenesession->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_TRUE(parentscenesession->FindMainWindowWithContext()!=nullptr);

}

/**
 * @tc.name: DisableAppWindowDecor01
 * @tc.desc: DisableAppWindowDecor
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, DisableAppWindowDecor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    windowscenesession->property_->isDecorEnable_ = true;
    windowscenesession->windowSystemConfig_.isSystemDecorEnable_ = false;

    windowscenesession->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    windowscenesession->DisableAppWindowDecor();
    ASSERT_FALSE(windowscenesession->IsDecorEnable());
    windowscenesession->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    windowscenesession->DisableAppWindowDecor();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Show());
    ASSERT_FALSE(windowscenesession->IsDecorEnable());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Destroy(false));
}

/**
 * @tc.name: HandleBackEvent01
 * @tc.desc: HandleBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, HandleBackEvent01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    std::shared_ptr<AbilityRuntime::AbilityContext> context =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    windowscenesession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(windowscenesession->uiContent_.get());
    EXPECT_CALL(*content, ProcessBackPressed()).WillOnce(Return(true));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->HandleBackKeyPressedEvent());

    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), RequestSessionBack()).WillOnce(Return(WSError::WS_OK));
}

/**
 * @tc.name: Resize01
 * @tc.desc: Resize 
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Resize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    WSRect wsRect = {1, 2, 2, 2};
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Resize(10,10));
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_PARENT, windowscenesession->Resize(10,10));
    option->setWindowTag(WindowTag::SUB_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowscenesession->Resize(10,10));
    windowscenesession->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WS_OK, windowscenesession->Resize(10,10));
    windowscenesession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WS_OK, windowscenesession->Resize(10,10));
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), UpdateSessionRect(wsRect,SizeChangeReason::RESIZE)).WillOnce(Return(WSError::WS_OK));
   
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    ASSERT_EQ(WMError::WM_OK, windowscenesession->RaiseToAppTop());
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_PARENT, windowscenesession->RaiseToAppTop());
    option->setWindowTag(WindowTag::SUB_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowscenesession->RaiseToAppTop());
    windowscenesession->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_STATE_ABNORMALLY, windowscenesession->RaiseToAppTop());
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), RaiseToAppTop()).WillOnce(Return(WSError::WS_OK));

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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->MoveTo(3,3));

    windowscenesession->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WS_OK, windowscenesession->MoveTo(10,10));
    windowscenesession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WS_OK, windowscenesession->MoveTo(10,10));
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), UpdateSessionRect(wsRect,SizeChangeReason::MOVE)).WillOnce(Return(WSError::WS_OK));

    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->Minimize());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_MINIMIZE)).WillOnce(Return(WSError::WS_OK));
    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->StartMove());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_START_MOVE)).WillOnce(Return(WSError::WS_OK));
    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->Close());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_CLOSE)).WillOnce(Return(WSError::WS_OK));
    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, windowscenesession->SetActive(false));

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), UpdateActiveStatus(false)).WillOnce(Return(WSError::WS_OK));
    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->Recover());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_RECOVER)).WillOnce(Return(WSError::WS_OK));
    
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
    sptr<WindowSceneSessionImpl> windowscenesession = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->Maximize());
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_MAXIMIZE)).WillOnce(Return(WSError::WS_OK));
    
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
    sptr<WindowSceneSessionImpl> window = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Hide(2,false,false));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    ASSERT_EQ(WMError::WM_OK, window->Hide(2,false,false));
    ASSERT_EQ(WMError::WM_OK, window->Hide(2,false,false));
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_OK, window->Hide(2,false,false));
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    EXPECT_CALL(*(session), Background()).WillOnce(Return(WSError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Hide(2,false,false));

    window->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));


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
    sptr<WindowSceneSessionImpl> window = new(std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Show(2,false));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), Foreground()).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show(2,false));
    ASSERT_EQ(WMError::WM_OK, window->Show(2,false));
    window->state_ = WindowState::STATE_CREATED;
    EXPECT_CALL(*(session), Foreground()).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Show(2,false));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
}

}
} // namespace Rosen
} // namespace OHOS
