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
#include <regex>
#include <pointer_event.h>
#include <ui/rs_surface_node.h>

#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "mock/mock_pattern_detach_callback.h"
#include "session/host/include/extension_session.h"
#include "session/host/include/move_drag_controller.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "session/host/include/session.h"
#include "session_info.h"
#include "key_event.h"
#include "wm_common.h"
#include "window_event_channel_base.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string UNDEFINED = "undefined";
}

class WindowSession2Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    int32_t GetTaskCount();
    sptr<SceneSessionManager> ssm_;

private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    sptr<Session> session_ = nullptr;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
};

void WindowSession2Test::SetUpTestCase()
{
}

void WindowSession2Test::TearDownTestCase()
{
}

void WindowSession2Test::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    session_ = new (std::nothrow) Session(info);
    session_->surfaceNode_ = CreateRSSurfaceNode();
    EXPECT_NE(nullptr, session_);
    ssm_ = new SceneSessionManager();
    session_->SetEventHandler(ssm_->taskScheduler_->GetEventHandler(), ssm_->eventHandler_);
    auto isScreenLockedCallback = [this]() {
        return ssm_->IsScreenLocked();
    };
    session_->RegisterIsScreenLockedCallback(isScreenLockedCallback);
}

void WindowSession2Test::TearDown()
{
    session_ = nullptr;
    usleep(WAIT_SYNC_IN_NS);
}

RSSurfaceNode::SharedPtr WindowSession2Test::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSession2TestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (surfaceNode == nullptr) {
        GTEST_LOG_(INFO) << "WindowSession2Test::CreateRSSurfaceNode surfaceNode is nullptr";
    }
    return surfaceNode;
}

int32_t WindowSession2Test::GetTaskCount()
{
    std::string dumpInfo = session_->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
    std::regex pattern("\\d+");
    std::smatch matches;
    int32_t taskNum = 0;
    while (std::regex_search(dumpInfo, matches, pattern)) {
        taskNum += std::stoi(matches.str());
        dumpInfo = matches.suffix();
    }
    return taskNum;
}

namespace {
/**
 * @tc.name: SetParentSession
 * @tc.desc: SetParentSession Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetParentSession, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sptr<Session> session = new (std::nothrow) Session(info);
    session_->SetParentSession(session);

    session_->property_ = new WindowSessionProperty();
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: BindDialogToParentSession
 * @tc.desc: BindDialogToParentSession Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, BindDialogToParentSession, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sptr<Session> session = new (std::nothrow) Session(info);
    session_->BindDialogToParentSession(session);

    session_->property_ = new WindowSessionProperty();
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: RemoveDialogToParentSession
 * @tc.desc: RemoveDialogToParentSession Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, RemoveDialogToParentSession, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sptr<Session> session = new (std::nothrow) Session(info);
    session_->RemoveDialogToParentSession(session);

    session_->property_ = new WindowSessionProperty();
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: TransferPointerEvent01
 * @tc.desc: !IsSystemSession() && !IsSessionValid() is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferPointerEvent01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = false;
    session_->state_ = SessionState::STATE_DISCONNECT;

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->TransferPointerEvent(pointerEvent));
}

/**
 * @tc.name: TransferPointerEvent02
 * @tc.desc: pointerEvent is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferPointerEvent02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->sessionInfo_.isSystem_ = true;

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferPointerEvent(pointerEvent));
}

/**
 * @tc.name: TransferPointerEvent03
 * @tc.desc: WindowType is WINDOW_TYPE_APP_MAIN_WINDOW, CheckDialogOnForeground() is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferPointerEvent03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    session_->property_ = new WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    SessionInfo info;
    info.abilityName_ = "dialogAbilityName";
    info.moduleName_ = "dialogModuleName";
    info.bundleName_ = "dialogBundleName";
    sptr<Session> dialogSession = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession, nullptr);
    dialogSession->state_ = SessionState::STATE_ACTIVE;
    session_->dialogVec_.push_back(dialogSession);

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferPointerEvent(pointerEvent));
}

/**
 * @tc.name: TransferPointerEvent04
 * @tc.desc: parentSession_ && parentSession_->CheckDialogOnForeground() is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferPointerEvent04, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);

    session_->property_ = new WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

    SessionInfo info;
    info.abilityName_ = "dialogAbilityName";
    info.moduleName_ = "dialogModuleName";
    info.bundleName_ = "dialogBundleName";
    sptr<Session> dialogSession = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession, nullptr);
    dialogSession->state_ = SessionState::STATE_ACTIVE;
    session_->dialogVec_.push_back(dialogSession);
    session_->parentSession_ = session_;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_PERMISSION, session_->TransferPointerEvent(pointerEvent));
}

/**
 * @tc.name: TransferPointerEvent05
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferPointerEvent05, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    session_->property_ = new WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_SCENE_BOARD);

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferPointerEvent(pointerEvent));
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: !IsSystemSession() && !IsSessionVaild() is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEvent01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = false;
    session_->state_ = SessionState::STATE_DISCONNECT;
    
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent02
 * @tc.desc: keyEvent is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEvent02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent03
 * @tc.desc: WindowType is WINDOW_TYPE_APP_MAIN_WINDOW, CheckDialogOnForeground() is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEvent03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);

    session_->property_ = new WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    SessionInfo info;
    info.abilityName_ = "dialogAbilityName";
    info.moduleName_ = "dialogModuleName";
    info.bundleName_ = "dialogBundleName";
    sptr<Session> dialogSession = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession, nullptr);
    dialogSession->state_ = SessionState::STATE_ACTIVE;
    session_->dialogVec_.push_back(dialogSession);

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent04
 * @tc.desc: parentSession_ && parentSession_->CheckDialogOnForeground() is true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEvent04, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);

    session_->property_ = new WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

    SessionInfo info;
    info.abilityName_ = "dialogAbilityName";
    info.moduleName_ = "dialogModuleName";
    info.bundleName_ = "dialogBundleName";
    sptr<Session> dialogSession = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession, nullptr);
    dialogSession->state_ = SessionState::STATE_ACTIVE;
    session_->dialogVec_.push_back(dialogSession);
    session_->parentSession_ = session_;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent05
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEvent05, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);

    session_->property_ = new WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_SCENE_BOARD);

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferBackPressedEventForConsumed01
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferBackPressedEventForConsumed01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = nullptr;

    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferBackPressedEventForConsumed(isConsumed));
}

/**
 * @tc.name: TransferBackPressedEventForConsumed02
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferBackPressedEventForConsumed02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_OK, session_->TransferBackPressedEventForConsumed(isConsumed));
}

/**
 * @tc.name: TransferKeyEventForConsumed01
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEventForConsumed01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = nullptr;

    auto keyEvent = MMI::KeyEvent::Create();
    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEventForConsumed(keyEvent, isConsumed));
}

/**
 * @tc.name: TransferKeyEventForConsumed02
 * @tc.desc: windowEventChannel_ is not nullptr, keyEvent is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEventForConsumed02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferKeyEventForConsumed(keyEvent, isConsumed));
}

/**
 * @tc.name: TransferKeyEventForConsumed03
 * @tc.desc: windowEventChannel_ is not nullptr, keyEvent is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferKeyEventForConsumed03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    bool isConsumed = false;
    ASSERT_EQ(WSError::WS_OK, session_->TransferKeyEventForConsumed(keyEvent, isConsumed));
}

/**
 * @tc.name: TransferFocusActiveEvent01
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferFocusActiveEvent01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferFocusActiveEvent(false));
}

/**
 * @tc.name: TransferFocusActiveEvent02
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferFocusActiveEvent02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    ASSERT_EQ(WSError::WS_OK, session_->TransferFocusActiveEvent(false));
}

/**
 * @tc.name: TransferFocusStateEvent01
 * @tc.desc: windowEventChannel_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferFocusStateEvent01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->TransferFocusStateEvent(false));
}

/**
 * @tc.name: TransferFocusStateEvent02
 * @tc.desc: windowEventChannel_ is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, TransferFocusStateEvent02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->windowEventChannel_ = new TestWindowEventChannel();

    ASSERT_EQ(WSError::WS_OK, session_->TransferFocusStateEvent(false));
}

/**
 * @tc.name: Snapshot01
 * @tc.desc: ret is false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, Snapshot01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->surfaceNode_ = nullptr;

    ASSERT_EQ(nullptr, session_->Snapshot());
}

/**
 * @tc.name: SetSessionStateChangeListenser
 * @tc.desc: SetSessionStateChangeListenser Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionStateChangeListenser, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    NotifySessionStateChangeFunc func = nullptr;
    session_->SetSessionStateChangeListenser(func);

    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetSessionFocusableChangeListener
 * @tc.desc: SetSessionFocusableChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionFocusableChangeListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    NotifySessionFocusableChangeFunc func = [](const bool isFocusable)
    {
    };
    session_->SetSessionFocusableChangeListener(func);

    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetSessionTouchableChangeListener
 * @tc.desc: SetSessionTouchableChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionTouchableChangeListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    NotifySessionTouchableChangeFunc func = [](const bool touchable)
    {
    };
    session_->SetSessionTouchableChangeListener(func);

    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetSessionInfoLockedStateChangeListener
 * @tc.desc: SetSessionInfoLockedStateChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoLockedStateChangeListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    NotifySessionTouchableChangeFunc func = [](const bool lockedState)
    {
    };
    session_->SetSessionInfoLockedStateChangeListener(func);

    session_->SetSessionInfoLockedState(true);
    ASSERT_EQ(true, session_->sessionInfo_.lockedState);
}

/**
 * @tc.name: SetClickListener
 * @tc.desc: SetClickListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetClickListener, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    NotifyClickFunc func = nullptr;
    session_->SetClickListener(func);

    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: UpdateFocus01
 * @tc.desc: isFocused_ equal isFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, UpdateFocus01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    bool isFocused = session_->isFocused_;
    ASSERT_EQ(WSError::WS_DO_NOTHING, session_->UpdateFocus(isFocused));
}

/**
 * @tc.name: UpdateFocus02
 * @tc.desc: isFocused_ not equal isFocused, IsSessionValid() return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, UpdateFocus02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionInfo_.isSystem_ = true;

    bool isFocused = session_->isFocused_;
    ASSERT_EQ(WSError::WS_OK, session_->UpdateFocus(!isFocused));
}

/**
 * @tc.name: UpdateWindowMode01
 * @tc.desc: IsSessionValid() return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, UpdateWindowMode01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->property_ = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->UpdateWindowMode(WindowMode::WINDOW_MODE_UNDEFINED));
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: NotifyForegroundInteractiveStatus Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyForegroundInteractiveStatus, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);

    session_->sessionStage_ = nullptr;
    bool interactive = true;
    session_->NotifyForegroundInteractiveStatus(interactive);

    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    session_->sessionStage_ = mockSessionStage;
    session_->state_ = SessionState::STATE_FOREGROUND;
    interactive = false;
    session_->NotifyForegroundInteractiveStatus(interactive);

    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_EQ(WSError::WS_OK, session_->SetFocusable(false));
}

/**
 * @tc.name: SetEventHandler001
 * @tc.desc: SetEventHandler Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetEventHandler001, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    session_->SetEventHandler(handler);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: PostTask002
 * @tc.desc: PostTask Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, PostTask002, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int32_t persistentId = 0;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    property->SetPersistentId(persistentId);
    int32_t res = session_->GetPersistentId();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetSurfaceNode003
 * @tc.desc: GetSurfaceNode Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetSurfaceNode003, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->surfaceNode_ = nullptr;
    std::shared_ptr<RSSurfaceNode> res = session_->GetSurfaceNode();
    ASSERT_EQ(res, nullptr);
}

/**
 * @tc.name: GetLeashWinSurfaceNode004
 * @tc.desc: GetLeashWinSurfaceNode Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetLeashWinSurfaceNode004, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->leashWinSurfaceNode_ = nullptr;
    std::shared_ptr<RSSurfaceNode> res = session_->GetLeashWinSurfaceNode();
    ASSERT_EQ(res, nullptr);
}

/**
 * @tc.name: SetSessionInfoAncoSceneState005
 * @tc.desc: SetSessionInfoAncoSceneState Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoAncoSceneState005, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    int32_t ancoSceneState = 0;
    session_->SetSessionInfoAncoSceneState(ancoSceneState);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: SetSessionInfoTime006
 * @tc.desc: SetSessionInfoTime Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoTime006, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    std::string time = "";
    session_->SetSessionInfoTime(time);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: SetSessionInfoAbilityInfo007
 * @tc.desc: SetSessionInfoAbilityInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoAbilityInfo007, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = nullptr;
    session_->SetSessionInfoAbilityInfo(abilityInfo);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: SetSessionInfoWant008
 * @tc.desc: SetSessionInfoWant Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoWant008, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    std::shared_ptr<AAFwk::Want> want = nullptr;
    session_->SetSessionInfoWant(want);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: SetSessionInfoPersistentId009
 * @tc.desc: SetSessionInfoPersistentId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoPersistentId009, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    int32_t persistentId = 0;
    session_->SetSessionInfoPersistentId(persistentId);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: SetSessionInfoCallerPersistentId010
 * @tc.desc: SetSessionInfoCallerPersistentId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoCallerPersistentId010, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int res = 0;
    int32_t callerPersistentId = 0;
    session_->SetSessionInfoCallerPersistentId(callerPersistentId);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: PostExportTask011
 * @tc.desc: PostExportTask Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, PostExportTask011, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int32_t persistentId = 0;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    property->SetPersistentId(persistentId);
    int32_t ret = session_->GetPersistentId();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetPersistentId012
 * @tc.desc: GetPersistentId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetPersistentId012, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    int32_t persistentId = 0;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    property->SetPersistentId(persistentId);
    int32_t ret = session_->GetPersistentId();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetLeashWinSurfaceNode013
 * @tc.desc: SetLeashWinSurfaceNode Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetLeashWinSurfaceNode013, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    auto leashWinSurfaceNode = WindowSession2Test::CreateRSSurfaceNode();
    session_->SetLeashWinSurfaceNode(leashWinSurfaceNode);
    ASSERT_EQ(session_->leashWinSurfaceNode_, leashWinSurfaceNode);
}

/**
 * @tc.name: SetSessionInfoContinueState014
 * @tc.desc: SetSessionInfoContinueState Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoContinueState014, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    enum ContinueState state;
    state = CONTINUESTATE_UNKNOWN;
    session_->SetSessionInfoContinueState(state);
    ASSERT_EQ(session_->sessionInfo_.continueState, state);
}

/**
 * @tc.name: SetSessionInfoIsClearSession015
 * @tc.desc: SetSessionInfoIsClearSession return false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoIsClearSession015, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetSessionInfoIsClearSession(false);
    ASSERT_EQ(false, session_->sessionInfo_.isClearSession);
}

/**
 * @tc.name: SetSessionInfoIsClearSession016
 * @tc.desc: SetSessionInfoIsClearSession return true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoIsClearSession016, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetSessionInfoIsClearSession(true);
    ASSERT_EQ(true, session_->sessionInfo_.isClearSession);
}

/**
 * @tc.name: SetSessionInfoAffinity017
 * @tc.desc: SetSessionInfoAffinity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfoAffinity017, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    std::string affinity = "setSessionIofoAffinity";
    session_->SetSessionInfoAffinity(affinity);
    ASSERT_EQ(affinity, session_->sessionInfo_.sessionAffinity);
}

/**
 * @tc.name: SetSessionInfo018
 * @tc.desc: SetSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionInfo018, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionInfo info;
    info.want = nullptr;
    info.callerToken_ = nullptr;
    info.requestCode = 1;
    info.callerPersistentId_ = 1;
    info.callingTokenId_ = 1;
    info.uiAbilityId_ = 1;
    info.startSetting = nullptr;
    info.continueSessionId_ = "";
    session_->SetSessionInfo(info);
    ASSERT_EQ(nullptr, session_->sessionInfo_.want);
    ASSERT_EQ(nullptr, session_->sessionInfo_.callerToken_);
    ASSERT_EQ(1, session_->sessionInfo_.requestCode);
    ASSERT_EQ(1, session_->sessionInfo_.callerPersistentId_);
    ASSERT_EQ(1, session_->sessionInfo_.callingTokenId_);
    ASSERT_EQ(1, session_->sessionInfo_.uiAbilityId_);
    ASSERT_EQ("", session_->sessionInfo_.continueSessionId_);
    ASSERT_EQ(nullptr, session_->sessionInfo_.startSetting);
}

/**
 * @tc.name: SetScreenId019
 * @tc.desc: SetScreenId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetScreenId019, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: RegisterLifecycleListener020
 * @tc.desc: RegisterLifecycleListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, RegisterLifecycleListener020, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    const std::shared_ptr<ILifecycleListener>& listener = nullptr;
    bool ret = session_->RegisterLifecycleListener(listener);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: UnregisterLifecycleListener021
 * @tc.desc: UnregisterLifecycleListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, UnregisterLifecycleListener021, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    const std::shared_ptr<ILifecycleListener>& listener = nullptr;
    bool ret = session_->UnregisterLifecycleListener(listener);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: NotifyActivation022
 * @tc.desc: NotifyActivation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyActivation022, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->NotifyActivation();
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: NotifyConnect023
 * @tc.desc: NotifyConnect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyConnect023, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->NotifyConnect();
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: NotifyForeground024
 * @tc.desc: NotifyForeground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyForeground024, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->NotifyForeground();
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: NotifyBackground025
 * @tc.desc: NotifyBackground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyBackground025, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->NotifyBackground();
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: NotifyDisconnect026
 * @tc.desc: NotifyDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyDisconnect026, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->NotifyDisconnect();
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: NotifyExtensionDied027
 * @tc.desc: NotifyExtensionDied
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyExtensionDied027, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->NotifyExtensionDied();
    uint64_t screenId = 0;
    session_->SetScreenId(screenId);
    ASSERT_EQ(0, session_->sessionInfo_.screenId_);
}

/**
 * @tc.name: GetAspectRatio028
 * @tc.desc: GetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetAspectRatio028, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    float ret = session_->aspectRatio_;
    float res = 0.0f;
    ASSERT_EQ(ret, res);
}

/**
 * @tc.name: SetAspectRatio029
 * @tc.desc: SetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetAspectRatio029, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    float radio = 2.0f;
    WSError ERR = session_->SetAspectRatio(radio);
    float ret = session_->aspectRatio_;
    ASSERT_EQ(ret, radio);
    ASSERT_EQ(ERR, WSError::WS_OK);
}

/**
 * @tc.name: GetSessionState030
 * @tc.desc: GetSessionState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetSessionState030, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionState state = session_->GetSessionState();
    ASSERT_EQ(state, session_->state_);
}

/**
 * @tc.name: SetSessionState031
 * @tc.desc: SetSessionState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionState031, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionState state = SessionState::STATE_CONNECT;
    session_->SetSessionState(state);
    ASSERT_EQ(state, session_->state_);
}

/**
 * @tc.name: UpdateSessionState32
 * @tc.desc: UpdateSessionState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, UpdateSessionState32, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionState state = SessionState::STATE_CONNECT;
    session_->UpdateSessionState(state);
    ASSERT_EQ(session_->state_, SessionState::STATE_CONNECT);
}

/**
 * @tc.name: GetTouchable33
 * @tc.desc: GetTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetTouchable33, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool res = session_->GetTouchable();
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetSystemTouchable34
 * @tc.desc: SetSystemTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSystemTouchable34, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool touchable = false;
    session_->SetSystemTouchable(touchable);
    ASSERT_EQ(session_->systemTouchable_, touchable);
}

/**
 * @tc.name: GetSystemTouchable35
 * @tc.desc: GetSystemTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetSystemTouchable35, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool res = session_->GetSystemTouchable();
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetVisible36
 * @tc.desc: SetVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetVisible36, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool isVisible = false;
    ASSERT_EQ(WSError::WS_OK, session_->SetVisible(isVisible));
}

/**
 * @tc.name: GetVisible37
 * @tc.desc: GetVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetVisible37, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    if (!session_->GetVisible()) {
        ASSERT_EQ(false, session_->GetVisible());
    }
}

/**
 * @tc.name: SetVisibilityState38
 * @tc.desc: SetVisibilityState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetVisibilityState38, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WindowVisibilityState state { WINDOW_VISIBILITY_STATE_NO_OCCLUSION};
    ASSERT_EQ(WSError::WS_OK, session_->SetVisibilityState(state));
    ASSERT_EQ(state, session_->visibilityState_);
}

/**
 * @tc.name: GetVisibilityState39
 * @tc.desc: GetVisibilityState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetVisibilityState39, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WindowVisibilityState state { WINDOW_LAYER_STATE_MAX};
    ASSERT_EQ(state, session_->GetVisibilityState());
}

/**
 * @tc.name: SetDrawingContentState40
 * @tc.desc: SetDrawingContentState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetDrawingContentState40, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool isRSDrawing = false;
    ASSERT_EQ(WSError::WS_OK, session_->SetDrawingContentState(isRSDrawing));
    ASSERT_EQ(false, session_->isRSDrawing_);
}

/**
 * @tc.name: GetDrawingContentState41
 * @tc.desc: GetDrawingContentState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetDrawingContentState41, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool res = session_->GetDrawingContentState();
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: GetBrightness42
 * @tc.desc: GetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetBrightness42, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    session_->property_ = nullptr;
    ASSERT_EQ(UNDEFINED_BRIGHTNESS, session_->GetBrightness());
}

/**
 * @tc.name: IsActive43
 * @tc.desc: IsActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, IsActive43, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool res = session_->IsActive();
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: IsSystemSession44
 * @tc.desc: IsSystemSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, IsSystemSession44, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool res = session_->IsSystemSession();
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: Hide45
 * @tc.desc: Hide
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, Hide45, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    auto result = session_->Hide();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Show46
 * @tc.desc: Show
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, Show46, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    auto result = session_->Show(property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: IsSystemActive47
 * @tc.desc: IsSystemActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, IsSystemActive47, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool res = session_->IsSystemActive();
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: SetSystemActive48
 * @tc.desc: SetSystemActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSystemActive48, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    bool systemActive = false;
    session_->SetSystemActive(systemActive);
    ASSERT_EQ(systemActive, session_->isSystemActive_);
}

/**
 * @tc.name: IsTerminated49
 * @tc.desc: IsTerminated
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, IsTerminated49, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->state_ = SessionState::STATE_DISCONNECT;
    bool res = session_->IsTerminated();
    ASSERT_EQ(true, res);
    session_->state_ = SessionState::STATE_FOREGROUND;
    res = session_->IsTerminated();
    ASSERT_EQ(false, res);
    session_->state_ = SessionState::STATE_ACTIVE;
    res = session_->IsTerminated();
    ASSERT_EQ(false, res);
    session_->state_ = SessionState::STATE_INACTIVE;
    res = session_->IsTerminated();
    ASSERT_EQ(false, res);
    session_->state_ = SessionState::STATE_BACKGROUND;
    res = session_->IsTerminated();
    ASSERT_EQ(false, res);
    session_->state_ = SessionState::STATE_CONNECT;
    res = session_->IsTerminated();
    ASSERT_EQ(false, res);
}

/**
 * @tc.name: SetSystemActive48
 * @tc.desc: SetSystemActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetChangeSessionVisibilityWithStatusBarEventListener, Function | SmallTest | Level2)
{
    int resultValue = 0;
    NotifyChangeSessionVisibilityWithStatusBarFunc func1 = [&resultValue](SessionInfo& info, const bool visible) {
        resultValue = 1;
    };
    NotifyChangeSessionVisibilityWithStatusBarFunc func2 = [&resultValue](SessionInfo& info, const bool visible) {
        resultValue = 2;
    };

    session_->SetChangeSessionVisibilityWithStatusBarEventListener(func1);
    ASSERT_NE(session_->changeSessionVisibilityWithStatusBarFunc_, nullptr);

    SessionInfo info;
    session_->changeSessionVisibilityWithStatusBarFunc_(info, true);
    ASSERT_EQ(resultValue, 1);

    session_->SetChangeSessionVisibilityWithStatusBarEventListener(func2);
    ASSERT_NE(session_->changeSessionVisibilityWithStatusBarFunc_, nullptr);
    session_->changeSessionVisibilityWithStatusBarFunc_(info, true);
    ASSERT_EQ(resultValue, 2);
}

/**
 * @tc.name: SetAttachState01
 * @tc.desc: SetAttachState Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetAttachState01, Function | SmallTest | Level2)
{
    session_->SetAttachState(false);
    ASSERT_EQ(session_->isAttach_, false);
}

/**
 * @tc.name: SetAttachState02
 * @tc.desc: SetAttachState Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetAttachState02, Function | SmallTest | Level2)
{
    int32_t persistentId = 123;
    sptr<PatternDetachCallbackMocker> detachCallback = new PatternDetachCallbackMocker();
    session_->persistentId_ = persistentId;
    session_->SetAttachState(true);
    session_->RegisterDetachCallback(detachCallback);
    session_->SetAttachState(false);
    usleep(WAIT_SYNC_IN_NS);
    Mock::VerifyAndClearExpectations(&detachCallback);
    ASSERT_EQ(session_->isAttach_, false);
}

/**
 * @tc.name: RegisterDetachCallback01
 * @tc.desc: RegisterDetachCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, RegisterDetachCallback01, Function | SmallTest | Level2)
{
    sptr<IPatternDetachCallback> detachCallback;
    session_->RegisterDetachCallback(detachCallback);
    ASSERT_EQ(session_->detachCallback_, detachCallback);
}

/**
 * @tc.name: RegisterDetachCallback02
 * @tc.desc: RegisterDetachCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, RegisterDetachCallback02, Function | SmallTest | Level2)
{
    sptr<IPatternDetachCallback> detachCallback;
    session_->RegisterDetachCallback(detachCallback);
    ASSERT_EQ(session_->detachCallback_, detachCallback);
    sptr<IPatternDetachCallback> detachCallback2;
    session_->RegisterDetachCallback(detachCallback2);
    ASSERT_EQ(session_->detachCallback_, detachCallback2);
}

/**
 * @tc.name: RegisterDetachCallback03
 * @tc.desc: RegisterDetachCallback Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, RegisterDetachCallback03, Function | SmallTest | Level2)
{
    int32_t persistentId = 123;
    sptr<PatternDetachCallbackMocker> detachCallback = new PatternDetachCallbackMocker();
    EXPECT_CALL(*detachCallback, OnPatternDetach(persistentId)).Times(1);
    session_->persistentId_ = persistentId;
    session_->SetAttachState(true);
    session_->SetAttachState(false);
    session_->RegisterDetachCallback(detachCallback);
    Mock::VerifyAndClearExpectations(&detachCallback);
}

/**
 * @tc.name: SetContextTransparentFunc
 * @tc.desc: SetContextTransparentFunc Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetContextTransparentFunc, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetContextTransparentFunc(nullptr);
    ASSERT_EQ(session_->contextTransparentFunc_, nullptr);
    NotifyContextTransparentFunc func = [](){};
    session_->SetContextTransparentFunc(func);
    ASSERT_NE(session_->contextTransparentFunc_, nullptr);
}

/**
 * @tc.name: NeedCheckContextTransparent
 * @tc.desc: NeedCheckContextTransparent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NeedCheckContextTransparent, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetContextTransparentFunc(nullptr);
    ASSERT_EQ(session_->NeedCheckContextTransparent(), false);
    NotifyContextTransparentFunc func = [](){};
    session_->SetContextTransparentFunc(func);
    ASSERT_EQ(session_->NeedCheckContextTransparent(), true);
}

/**
 * @tc.name: SetShowRecent001
 * @tc.desc: Exist detect task when in recent.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetShowRecent001, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    session_->handler_->PostTask(task, taskName, delayTime);
    int32_t beforeTaskNum = GetTaskCount();

    session_->SetShowRecent(true);
    ASSERT_EQ(beforeTaskNum, GetTaskCount());
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: SetShowRecent002
 * @tc.desc: SetShowRecent:showRecent is false, showRecent_ is false.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetShowRecent002, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    session_->handler_->PostTask(task, taskName, delayTime);
    session_->showRecent_ = false;
    int32_t beforeTaskNum = GetTaskCount();

    session_->SetShowRecent(false);
    ASSERT_EQ(beforeTaskNum, GetTaskCount());
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: SetShowRecent003
 * @tc.desc: SetShowRecent:showRecent is false, showRecent_ is true, detach task.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetShowRecent003, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    session_->handler_->PostTask(task, taskName, delayTime);
    session_->showRecent_ = true;
    session_->isAttach_ = false;
    int32_t beforeTaskNum = GetTaskCount();

    session_->SetShowRecent(false);
    ASSERT_EQ(beforeTaskNum, GetTaskCount());
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask001
 * @tc.desc: Create detection task when there are no pre_existing tasks.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, CreateDetectStateTask001, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::NO_TASK;
    int32_t beforeTaskNum = GetTaskCount();
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(false, WindowMode::WINDOW_MODE_FULLSCREEN);

    ASSERT_NE(beforeTaskNum + 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::DETACH_TASK, session_->GetDetectTaskInfo().taskState);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask002
 * @tc.desc: Detect state when window mode changed.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, CreateDetectStateTask002, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    session_->handler_->PostTask(task, taskName, delayTime);
    int32_t beforeTaskNum = GetTaskCount();

    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::DETACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(true, WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    ASSERT_NE(beforeTaskNum - 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::NO_TASK, session_->GetDetectTaskInfo().taskState);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, session_->GetDetectTaskInfo().taskWindowMode);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask003
 * @tc.desc: Detect sup and down tree tasks fo the same type.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, CreateDetectStateTask003, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::DETACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    int32_t beforeTaskNum = GetTaskCount();
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(false, WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    ASSERT_NE(beforeTaskNum + 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::DETACH_TASK, session_->GetDetectTaskInfo().taskState);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: CreateDetectStateTask004
 * @tc.desc: Detection tasks under the same window mode.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, CreateDetectStateTask004, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    DetectTaskInfo detectTaskInfo;
    int32_t beforeTaskNum = GetTaskCount();
    detectTaskInfo.taskState = DetectTaskState::DETACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    session_->SetDetectTaskInfo(detectTaskInfo);
    session_->CreateDetectStateTask(true, WindowMode::WINDOW_MODE_FULLSCREEN);

    ASSERT_NE(beforeTaskNum + 1, GetTaskCount());
    ASSERT_EQ(DetectTaskState::ATTACH_TASK, session_->GetDetectTaskInfo().taskState);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: GetAttachState001
 * @tc.desc: GetAttachState001
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, GetAttachState001, Function | SmallTest | Level2)
{
    std::string taskName = "wms:WindowStateDetect" + std::to_string(session_->persistentId_);
    session_->SetAttachState(false);
    bool isAttach = session_->GetAttachState();
    ASSERT_EQ(false, isAttach);
    session_->handler_->RemoveTask(taskName);
}

/**
 * @tc.name: UpdateSizeChangeReason
 * @tc.desc: UpdateSizeChangeReason UpdateDensity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, UpdateSizeChangeReason, Function | SmallTest | Level2)
{
    SizeChangeReason reason = SizeChangeReason{1};
    ASSERT_EQ(session_->UpdateSizeChangeReason(reason), WSError::WS_OK);
}

/**
 * @tc.name: SetPendingSessionActivationEventListener
 * @tc.desc: SetPendingSessionActivationEventListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetPendingSessionActivationEventListener, Function | SmallTest | Level2)
{
    int resultValue = 0;
    NotifyPendingSessionActivationFunc callback = [&resultValue](const SessionInfo& info) {
        resultValue = 1;
    };

    sptr<AAFwk::SessionInfo> info = new (std::nothrow)AAFwk::SessionInfo();
    session_->SetPendingSessionActivationEventListener(callback);
    NotifyTerminateSessionFunc callback1 = [&resultValue](const SessionInfo& info) {
        resultValue = 2;
    };
    session_->SetTerminateSessionListener(callback1);
    LifeCycleTaskType taskType = LifeCycleTaskType{0};
    session_->RemoveLifeCycleTask(taskType);
    ASSERT_EQ(resultValue, 0);
}

/**
 * @tc.name: SetSessionIcon
 * @tc.desc: SetSessionIcon UpdateDensity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSessionIcon, Function | SmallTest | Level2)
{
    std::shared_ptr<Media::PixelMap> icon;
    session_->SetSessionIcon(icon);
    ASSERT_EQ(session_->Clear(), WSError::WS_OK);
    session_->SetSessionSnapshotListener(nullptr);
    ASSERT_EQ(session_->PendingSessionToForeground(), WSError::WS_OK);
}

/**
 * @tc.name: SetRaiseToAppTopForPointDownFunc
 * @tc.desc: SetRaiseToAppTopForPointDownFunc Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetRaiseToAppTopForPointDownFunc, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetRaiseToAppTopForPointDownFunc(nullptr);
    session_->RaiseToAppTopForPointDown();
    session_->HandlePointDownDialog();
    session_->ClearDialogVector();

    session_->SetBufferAvailableChangeListener(nullptr);
    session_->UnregisterSessionChangeListeners();
    session_->SetSessionStateChangeNotifyManagerListener(nullptr);
    session_->SetSessionInfoChangeNotifyManagerListener(nullptr);
    session_->NotifyFocusStatus(true);

    session_->SetRequestFocusStatusNotifyManagerListener(nullptr);
    session_->SetNotifyUIRequestFocusFunc(nullptr);
    session_->SetNotifyUILostFocusFunc(nullptr);
    session_->UnregisterSessionChangeListeners();
    ASSERT_EQ(WSError::WS_OK, session_->PendingSessionToBackgroundForDelegator());
}

/**
 * @tc.name: NotifyCloseExistPipWindow
 * @tc.desc: check func NotifyCloseExistPipWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, NotifyCloseExistPipWindow, Function | SmallTest | Level2)
{
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    ManagerState key = ManagerState{0};
    session_->GetStateFromManager(key);
    session_->NotifyUILostFocus();
    session_->SetSystemSceneBlockingFocus(true);
    session_->GetBlockingFocus();
    session_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), NotifyCloseExistPipWindow()).Times(1).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, session_->NotifyCloseExistPipWindow());
    session_->sessionStage_ = nullptr;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, session_->NotifyCloseExistPipWindow());
}

/**
 * @tc.name: SetSystemConfig
 * @tc.desc: SetSystemConfig Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetSystemConfig, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SystemSessionConfig systemConfig;
    session_->SetSystemConfig(systemConfig);
    float snapshotScale = 0.5;
    session_->SetSnapshotScale(snapshotScale);
    session_->ProcessBackEvent();
    session_->NotifyOccupiedAreaChangeInfo(nullptr);
    session_->UpdateMaximizeMode(true);
    ASSERT_EQ(session_->GetZOrder(), 0);

    session_->SetUINodeId(0);
    session_->GetUINodeId();
    session_->SetShowRecent(true);
    session_->GetShowRecent();
    session_->SetBufferAvailable(true);

    session_->SetNeedSnapshot(true);
    session_->SetFloatingScale(0.5);
    ASSERT_EQ(session_->GetFloatingScale(), 0.5f);
    session_->SetScale(50, 100, 50, 100);
    session_->GetScaleX();
    session_->GetScaleY();
    session_->GetPivotX();
    session_->GetPivotY();
    session_->SetSCBKeepKeyboard(true);
    session_->GetSCBKeepKeyboardFlag();
    ASSERT_EQ(WSError::WS_OK, session_->MarkProcessed(11));
}

/**
 * @tc.name: SetOffset
 * @tc.desc: SetOffset Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, SetOffset, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetOffset(50, 100);
    session_->GetOffsetX();
    session_->GetOffsetY();
    WSRectF bounds;
    session_->SetBounds(bounds);
    session_->GetBounds();
    session_->TransferAccessibilityHoverEvent(50, 100, 50, 50, 500);
    session_->UpdateTitleInTargetPos(true, 100);
    session_->SetNotifySystemSessionPointerEventFunc(nullptr);
    session_->SetNotifySystemSessionKeyEventFunc(nullptr);
    ASSERT_EQ(session_->GetBufferAvailable(), false);
}

/**
 * @tc.name: ResetSessionConnectState
 * @tc.desc: ResetSessionConnectState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSession2Test, ResetSessionConnectState, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->ResetSessionConnectState();
    ASSERT_EQ(session_->state_, SessionState::STATE_DISCONNECT);
    ASSERT_EQ(session_->GetCallingPid(), -1);
}
}
} // namespace Rosen
} // namespace OHOS
