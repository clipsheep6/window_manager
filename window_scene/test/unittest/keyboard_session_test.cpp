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

#include "session/host/include/keyboard_session.h"
#include <gtest/gtest.h>
#include <parameters.h>

#include "common/include/session_permission.h"
#include "interfaces/include/ws_common.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_keyboard_session.h"
#include "session/host/include/session.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "KeyboardSessionTest"};
}

constexpr int WAIT_ASYNC_US = 1000000;
class KeyboardSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<KeyboardSession> GetKeyboardSession(const std::string& abilityName, const std::string& bundleName);
    sptr<SceneSession> GetSceneSession(const std::string& abilityName, const std::string& bundleName);
    sptr<KSSceneSessionMocker> GetSceneSessionMocker(const std::string& abilityName, const std::string& bundleName);
};

void KeyboardSessionTest::SetUpTestCase()
{
}

void KeyboardSessionTest::TearDownTestCase()
{
}

void KeyboardSessionTest::SetUp()
{
}

void KeyboardSessionTest::TearDown()
{
}

sptr<KeyboardSession> KeyboardSessionTest::GetKeyboardSession(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    sptr<WindowSessionProperty> keyboardProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(keyboardProperty, nullptr);
    keyboardProperty->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(keyboardProperty);

    return keyboardSession;
}

sptr<SceneSession> KeyboardSessionTest::GetSceneSession(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);

    return sceneSession;
}

sptr<KSSceneSessionMocker> KeyboardSessionTest::GetSceneSessionMocker(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KSSceneSessionMocker> mockSession = sptr<KSSceneSessionMocker>::MakeSptr(info, nullptr);

    return mockSession;
}

namespace {
/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetKeyboardGravity, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetKeyboardGravity";
    info.bundleName_ = "GetKeyboardGravity";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, nullptr);
    ASSERT_TRUE((keyboardSession != nullptr));
    keyboardSession->property_ = nullptr;
    ASSERT_EQ(SessionGravity::SESSION_GRAVITY_DEFAULT, keyboardSession->GetKeyboardGravity());
 
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_EQ(SessionGravity::SESSION_GRAVITY_BOTTOM, keyboardSession->GetKeyboardGravity());
}

/**
 * @tc.name: Show01
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Show01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Show01";
    info.bundleName_ = "Show01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    keyboardSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));

    keyboardSession->isKeyboardPanelEnabled_ = false;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));
}

/**
 * @tc.name: Show02
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Show02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Show02";
    info.bundleName_ = "Show02";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, keyboardSession->Show(nullptr));
}

/**
 * @tc.name: Hide
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Hide, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Hide";
    info.bundleName_ = "Hide";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));

    ASSERT_EQ(WSError::WS_OK, keyboardSession->Hide());
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Disconnect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Disconnect";
    info.bundleName_ = "Disconnect";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);
    keyboardSession->isActive_ = true;
    auto result = keyboardSession->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyClientToUpdateRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect";
    info.bundleName_ = "NotifyClientToUpdateRect";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    // NotifyClientToUpdateRectTask return not ok
    WSError ret = keyboardSession->NotifyClientToUpdateRect("KeyboardSessionTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);

    // NotifyClientToUpdateRectTask return ok and session->reason_ is UNDEFINED
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    keyboardSession->sessionStage_ = mockSessionStage;
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    ret = keyboardSession->NotifyClientToUpdateRect("KeyboardSessionTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);

    // NotifyClientToUpdateRectTask return ok and session->reason_ is DRAG
    keyboardSession->reason_ = SizeChangeReason::DRAG;
    keyboardSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    ret = keyboardSession->NotifyClientToUpdateRect("KeyboardSessionTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyClientToUpdateRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect01";
    info.bundleName_ = "NotifyClientToUpdateRect01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    keyboardSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    keyboardSession->sessionStage_ = mockSessionStage;
    auto ret = keyboardSession->NotifyClientToUpdateRect("KeyboardSessionTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect02
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyClientToUpdateRect02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect02";
    info.bundleName_ = "NotifyClientToUpdateRect02";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    keyboardSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    keyboardSession->reason_ = SizeChangeReason::MOVE;
    keyboardSession->isKeyboardPanelEnabled_ = true;
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    windowSessionProperty->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    keyboardSession->SetSessionProperty(windowSessionProperty);
    auto ret = keyboardSession->NotifyClientToUpdateRect("KeyboardSessionTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: GetSceneSession01
 * @tc.desc: GetSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetSceneSession01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetSceneSession01";
    info.bundleName_ = "GetSceneSession01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);
    auto ret = keyboardSession->GetSceneSession(id);

    keyboardCb->onGetSceneSession_ = [](uint32_t) {
        return nullptr;
    };
    EXPECT_NE(keyboardCb->onGetSceneSession_, nullptr);
    ret = keyboardSession->GetSceneSession(id);
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo
 * @tc.desc: NotifyOccupiedAreaChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyOccupiedAreaChangeInfo, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyOccupiedAreaChangeInfo";
    info.bundleName_ = "NotifyOccupiedAreaChangeInfo";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<SceneSession> callingSession = new (std::nothrow) SceneSession(info, nullptr);
    WSRect rect = { 0, 0, 0, 0 };
    WSRect occupiedArea = { 0, 0, 0, 0 };
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);

    WSRect lastSR = {1, 1, 1, 1};
    callingSession->lastSafeRect = lastSR;
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);

    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);
}

/**
 * @tc.name: RestoreCallingSession
 * @tc.desc: RestoreCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RestoreCallingSession, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RestoreCallingSession";
    info.bundleName_ = "RestoreCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    // callingSession is nullptr
    keyboardSession->RestoreCallingSession();

    // callingsession is not nullptr
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(callingSession, nullptr);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };
    keyboardSession->RestoreCallingSession();
    ASSERT_EQ(callingSession->GetOriPosYBeforeRaisedByKeyboard(), 0); // 0: default value

    callingSession->SetOriPosYBeforeRaisedByKeyboard(100); // 100 is not default
    ASSERT_NE(callingSession->property_, nullptr);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    keyboardSession->RestoreCallingSession();
    ASSERT_EQ(callingSession->GetOriPosYBeforeRaisedByKeyboard(), 0); // 0: default value
}

/**
 * @tc.name: UseFocusIdIfCallingSessionIdInvalid
 * @tc.desc: UseFocusIdIfCallingSessionIdInvalid
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UseFocusIdIfCallingSessionIdInvalid, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UseFocusIdIfCallingSessionIdInvalid";
    info.bundleName_ = "UseFocusIdIfCallingSessionIdInvalid";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);

    keyboardSession->GetSessionProperty()->SetCallingSessionId(id);
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
}

/**
 * @tc.name: UpdateCallingSessionIdAndPosition
 * @tc.desc: UpdateCallingSessionIdAndPosition
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UpdateCallingSessionIdAndPosition, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateCallingSessionIdAndPosition";
    info.bundleName_ = "UpdateCallingSessionIdAndPosition";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);

    keyboardSession->UpdateCallingSessionIdAndPosition(id);
}

/**
 * @tc.name: RelayoutKeyBoard
 * @tc.desc: RelayoutKeyBoard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RelayoutKeyBoard, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);

    keyboardSession->RelayoutKeyBoard();
}

/**
 * @tc.name: GetFocusedSessionId
 * @tc.desc: GetFocusedSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetFocusedSessionId, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    keyboardCb->onGetFocusedSessionId_ = []()
    {
        return 0;
    };
    EXPECT_NE(keyboardCb->onGetFocusedSessionId_, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    ASSERT_EQ(INVALID_WINDOW_ID, keyboardSession->GetFocusedSessionId());

    keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);

    ASSERT_EQ(INVALID_WINDOW_ID, keyboardSession->GetFocusedSessionId());
}

/**
 * @tc.name: OnKeyboardPanelUpdated
 * @tc.desc: OnKeyboardPanelUpdated
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, OnKeyboardPanelUpdated, Function | SmallTest | Level1)
{
    WLOGFI("OnKeyboardPanelUpdated begin!");
    int ret = 0;

    SessionInfo info;
    info.abilityName_ = "OnKeyboardPanelUpdated";
    info.bundleName_ = "OnKeyboardPanelUpdated";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardPanelEnabled_ = false;
    keyboardSession->OnKeyboardPanelUpdated();

    keyboardSession->isKeyboardPanelEnabled_ = true;
    keyboardSession->specificCallback_ = nullptr;
    keyboardSession->OnKeyboardPanelUpdated();

    keyboardSession->specificCallback_ = specificCb;
    auto onUpdateAvoidArea = specificCb->onUpdateAvoidArea_;
    if (onUpdateAvoidArea == nullptr) {
        onUpdateAvoidArea = [](const int32_t& id){};
    }
    specificCb->onUpdateAvoidArea_ = nullptr;
    keyboardSession->OnKeyboardPanelUpdated();

    specificCb->onUpdateAvoidArea_ = onUpdateAvoidArea;
    keyboardSession->OnKeyboardPanelUpdated();

    ASSERT_EQ(ret, 0);
    WLOGFI("OnKeyboardPanelUpdated end!");
}

/**
 * @tc.name: SetCallingSessionId
 * @tc.desc: SetCallingSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, SetCallingSessionId, Function | SmallTest | Level1)
{
    WLOGFI("SetCallingSessionId begin!");
    SessionInfo info;
    info.abilityName_ = "SetCallingSessionId";
    info.bundleName_ = "SetCallingSessionId";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    // keyboardCallback_->onGetSceneSession_ is nullptr, getCallingSession is nullptr
    keyboardSession->SetCallingSessionId(0);
    ASSERT_EQ(keyboardSession->GetCallingSessionId(), INVALID_SESSION_ID);

    // getCallingSession is not nullptr
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(callingSession, nullptr);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [callingSession](int32_t persistenId)->sptr<SceneSession> {
        if (persistenId != 100) { // callingSession's persistentId is 100
            return nullptr;
        }
        return callingSession;
    };
    keyboardSession->keyboardCallback_->onGetFocusedSessionId_ = []()->int32_t {
        return 100; // focusSession's persistentId is 100
    };
    keyboardSession->SetCallingSessionId(0);
    ASSERT_EQ(keyboardSession->GetCallingSessionId(), 100); // 100 is callingSessionId

    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(INVALID_SESSION_ID);
    keyboardSession->keyboardCallback_->onCallingSessionIdChange_ = [](int32_t callingSessionid){};
    keyboardSession->SetCallingSessionId(100);
    ASSERT_EQ(keyboardSession->GetCallingSessionId(), 100); // 100 is callingSessionId
}

/**
 * @tc.name: GetCallingSessionId
 * @tc.desc: GetCallingSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetCallingSessionId, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetCallingSessionId";
    info.bundleName_ = "GetCallingSessionId";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    auto ret = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(ret, INVALID_WINDOW_ID);
}

/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, AdjustKeyboardLayout01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout01";
    info.bundleName_ = "AdjustKeyboardLayout01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);
    keyboardSession->RegisterSessionChangeCallback(nullptr);

    KeyboardLayoutParams params;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    sptr<SceneSession::SessionChangeCallback> sessionChangeCallback =
        new (std::nothrow) SceneSession::SessionChangeCallback();
    EXPECT_NE(sessionChangeCallback, nullptr);
    keyboardSession->adjustKeyboardLayoutFunc_ = nullptr;
    keyboardSession->RegisterSessionChangeCallback(sessionChangeCallback);
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = [](const KeyboardLayoutParams& params){};
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, AdjustKeyboardLayout02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout02";
    info.bundleName_ = "AdjustKeyboardLayout02";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    keyboardSession->SetSessionProperty(nullptr);

    KeyboardLayoutParams params;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: CheckIfNeedRaiseCallingSession
 * @tc.desc: CheckIfNeedRaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CheckIfNeedRaiseCallingSession, Function | SmallTest | Level1)
{
    WLOGFI("CheckIfNeedRaiseCallingSession begin!");
    SessionInfo info;
    info.abilityName_ = "CheckIfNeedRaiseCallingSession";
    info.bundleName_ = "CheckIfNeedRaiseCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(property);

    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(nullptr, true));

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);

    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true));

    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    ASSERT_TRUE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, false));

    property->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true));

    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true);

    WLOGFI("CheckIfNeedRaiseCallingSession end!");
}

/**
 * @tc.name: OpenKeyboardSyncTransaction
 * @tc.desc: OpenKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, OpenKeyboardSyncTransaction, Function | SmallTest | Level1)
{
    std::string abilityName = "OpenKeyboardSyncTransaction";
    std::string bundleName = "OpenKeyboardSyncTransaction";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    // isKeyBoardSyncTransactionOpen_ is false
    keyboardSession->OpenKeyboardSyncTransaction();

    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->OpenKeyboardSyncTransaction();
}

/**
 * @tc.name: CloseKeyboardSyncTransaction1
 * @tc.desc: CloseKeyboardSyncTransaction1
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CloseKeyboardSyncTransaction1, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction1";
    std::string bundleName = "CloseKeyboardSyncTransaction1";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    bool isRotating = false;

    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
}

/**
 * @tc.name: CloseKeyboardSyncTransaction2
 * @tc.desc: CloseKeyboardSyncTransaction2
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CloseKeyboardSyncTransaction2, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction2";
    std::string bundleName = "CloseKeyboardSyncTransaction2";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    bool isRotating = false;

    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->OpenKeyboardSyncTransaction();
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
}

/**
 * @tc.name: BindKeyboardPanelSession
 * @tc.desc: BindKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, BindKeyboardPanelSession, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "BindKeyboardPanelSession";
    info.bundleName_ = "BindKeyboardPanelSession";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    sptr<SceneSession> panelSession = nullptr;
    keyboardSession->BindKeyboardPanelSession(panelSession);
    sptr<SceneSession> getPanelSession = keyboardSession->GetKeyboardPanelSession();
    ASSERT_EQ(getPanelSession, nullptr);
    panelSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(panelSession, nullptr);
    keyboardSession->BindKeyboardPanelSession(panelSession);
    getPanelSession = keyboardSession->GetKeyboardPanelSession();
    EXPECT_EQ(getPanelSession, panelSession);
}

/**
 * @tc.name: GetKeyboardGravity01
 * @tc.desc: GetKeyboardGravity01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetKeyboardGravity01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetKeyboardGravity";
    info.bundleName_ = "GetKeyboardGravity";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto ret = keyboardSession->GetKeyboardGravity();
    EXPECT_EQ(SessionGravity::SESSION_GRAVITY_DEFAULT, ret);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    ASSERT_NE(keyboardSession, nullptr);
    ret = keyboardSession->GetKeyboardGravity();
    EXPECT_EQ(SessionGravity::SESSION_GRAVITY_BOTTOM, ret);
}

/**
 * @tc.name: GetCallingSessionId01
 * @tc.desc: GetCallingSessionId01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetCallingSessionId01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetCallingSessionId";
    info.bundleName_ = "GetCallingSessionId";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto ret = keyboardSession->GetCallingSessionId();
    EXPECT_EQ(ret, INVALID_SESSION_ID);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(1);
    ret = keyboardSession->GetCallingSessionId();
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name: NotifyKeyboardPanelInfoChange
 * @tc.desc: NotifyKeyboardPanelInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyKeyboardPanelInfoChange, Function | SmallTest | Level1)
{
    WSRect rect = {800, 800, 1200, 1200};
    SessionInfo info;
    info.abilityName_ = "NotifyKeyboardPanelInfoChange";
    info.bundleName_ = "NotifyKeyboardPanelInfoChange";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardPanelEnabled_ = false;
    keyboardSession->NotifyKeyboardPanelInfoChange(rect, true);
    keyboardSession->isKeyboardPanelEnabled_ = true;
    keyboardSession->sessionStage_ = nullptr;
    keyboardSession->NotifyKeyboardPanelInfoChange(rect, true);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(mockSessionStage, nullptr);
    keyboardSession->sessionStage_ = mockSessionStage;
    ASSERT_NE(keyboardSession->sessionStage_, nullptr);
    keyboardSession->NotifyKeyboardPanelInfoChange(rect, true);
}

/**
 * @tc.name: CheckIfNeedRaiseCallingSession01
 * @tc.desc: CheckIfNeedRaiseCallingSession01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CheckIfNeedRaiseCallingSession01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "CheckIfNeedRaiseCallingSession";
    info.bundleName_ = "CheckIfNeedRaiseCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCb, nullptr);
    sptr<SceneSession> callingSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    ASSERT_NE(callingSession, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    keyboardSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    callingSession->systemConfig_.freeMultiWindowSupport_ = true;
    callingSession->systemConfig_.freeMultiWindowEnable_ = true;
    auto ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, false);
    callingSession->systemConfig_.freeMultiWindowEnable_ = false;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, false);
    callingSession->systemConfig_.freeMultiWindowEnable_ = true;
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    callingSession->systemConfig_.freeMultiWindowEnable_ = false;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    keyboardSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, false);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: UpdateCallingSessionIdAndPosition01
 * @tc.desc: UpdateCallingSessionIdAndPosition01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UpdateCallingSessionIdAndPosition01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateCallingSessionIdAndPosition";
    info.bundleName_ = "UpdateCallingSessionIdAndPosition";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(-1);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
    keyboardSession->UpdateCallingSessionIdAndPosition(-1);
    keyboardSession->property_->SetCallingSessionId(0);
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
}

/**
 * @tc.name: OpenKeyboardSyncTransaction01
 * @tc.desc: OpenKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, OpenKeyboardSyncTransaction01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateCallingSessionIdAndPosition";
    info.bundleName_ = "UpdateCallingSessionIdAndPosition";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    keyboardSession->OpenKeyboardSyncTransaction();
    keyboardSession->isKeyboardSyncTransactionOpen_ = false;
    keyboardSession->OpenKeyboardSyncTransaction();
    WSRect keyboardPanelRect = {0, 0, 0, 0};
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, true, true);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, false);
}

/**
 * @tc.name: RelayoutKeyBoard01
 * @tc.desc: RelayoutKeyBoard01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RelayoutKeyBoard01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    keyboardSession->RelayoutKeyBoard();
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    keyboardSession->RelayoutKeyBoard();
}

/**
 * @tc.name: Hide01
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Hide01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Hide";
    info.bundleName_ = "Hide";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    ASSERT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    ASSERT_NE(keyboardSession, nullptr);

    // setActive false return not ok
    keyboardSession->state_ = SessionState::STATE_DISCONNECT;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // setActive false return ok and deviceType is phone
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->isActive_ = true;
    keyboardSession->sessionStage_ = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(keyboardSession->sessionStage_, nullptr);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // deviceType is pc and property is not nullptr
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // deviceType is pc and property is nullptr
    keyboardSession->property_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());
}

/**
 * @tc.name: RaiseCallingSession01
 * @tc.desc: test function : RaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RaiseCallingSession01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("RaiseCallingSession01",
        "RaiseCallingSession01");
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;

    Rosen::WSRect resultRect{ 0, 0, 0, 0 };
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);

    callingSession->updateRectCallback_ = [&resultRect](const WSRect& rect, const SizeChangeReason reason) {
        resultRect.posX_ = rect.posX_;
        resultRect.posY_ = rect.posY_;
        resultRect.width_ = rect.width_;
        resultRect.height_ = rect.height_;
    };
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    Rosen::WSRect keyboardPanelRect{ 0, 0, 0, 0 };
    Rosen::WSRect emptyRect{ 0, 0, 0, 0 };
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, rsTransaction);
    ASSERT_EQ(resultRect, emptyRect);

    // for cover GetSceneSession
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, rsTransaction);
    // for cover CheckIfNeedRaiseCallingSession
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    // for empty rect check;
    keyboardSession->winRect_.posX_ = 1;
    keyboardSession->winRect_.posY_ = 1;
    keyboardSession->winRect_.posX_ = 1;
    keyboardSession->winRect_.posX_ = 1;

    // for cover oriPosYBeforeRaisedBykeyboard == 0
    callingSession->SetOriPosYBeforeRaisedByKeyboard(0);
    ASSERT_EQ(resultRect, emptyRect);
}

/**
 * @tc.name: RaiseCallingSession02
 * @tc.desc: test function : RaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RaiseCallingSession02, Function | SmallTest | Level1)
{
    Rosen::WSRect keyboardPanelRect{ 1, 1, 1, 1 };
    auto keyboardSession = GetKeyboardSession("RaiseCallingSession02",
        "RaiseCallingSession02");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    callingSession->winRect_ = { 1, 1, 1, 1 };
    keyboardSession->keyboardCallback_->onGetSceneSession_ = [callingSession](int32_t persistentId) {
        return callingSession;
    };
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 0;
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->winRect_.posY_, 1);

    callingSession->oriPosYBeforeRaisedByKeyboard_ = 10;
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->winRect_.posY_, 1);

    keyboardPanelRect = { 0, 0, 0, 0 };
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 10;
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->winRect_.posY_, 1);
}

/**
 * @tc.name: RaiseCallingSession03
 * @tc.desc: test function : RaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RaiseCallingSession03, Function | SmallTest | Level1)
{
    Rosen::WSRect keyboardPanelRect{ 1, 1, 1, 1 };
    auto keyboardSession = GetKeyboardSession("RaiseCallingSession03",
        "RaiseCallingSession03");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);
    callingSession->winRect_ = { 1, 1, 1, 1 };
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 0;
    callingSession->updateRectCallback_ = [](const WSRect& rect, const SizeChangeReason reason) {};
    keyboardSession->keyboardCallback_->onGetSceneSession_ = [callingSession](int32_t persistentId) {
        return callingSession;
    };
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;
    auto callingOriPosY = 0;
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    callingOriPosY = callingSession->oriPosYBeforeRaisedByKeyboard_;
    ASSERT_EQ(callingOriPosY, 0);

    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    callingSession->winRect_.posY_ = 200;
    keyboardPanelRect.posY_ = 200;
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    callingOriPosY = callingSession->oriPosYBeforeRaisedByKeyboard_;
    ASSERT_EQ(callingOriPosY, 200);

    callingSession->oriPosYBeforeRaisedByKeyboard_ = 10;
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    callingOriPosY = callingSession->oriPosYBeforeRaisedByKeyboard_;
    ASSERT_EQ(callingOriPosY, 10);
}

/**
 * @tc.name: IsCallingSessionSplitMode01
 * @tc.desc: test function : IsCallingSessionSplitMode
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, IsCallingSessionSplitMode01, Function | SmallTest | Level1)
{
    Rosen::WSRect keyboardPanelRect{ 0, 0, 0, 0 };
    auto keyboardSession = GetKeyboardSession("IsCallingSessionSplitMode01",
        "IsCallingSessionSplitMode01");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 0;
    callingSession->winRect_ = { 0, 0, 0, 0 };
    callingSession->updateRectCallback_ = [](const WSRect& rect, const SizeChangeReason reason) {};
    keyboardSession->keyboardCallback_->onGetSceneSession_ = [callingSession](int32_t persistentId) {
        return callingSession;
    };
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;

    auto callingParentSession = GetSceneSession("callingParentSession", "callingParentSession");
    ASSERT_NE(callingSession, nullptr);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->parentSession_ = callingParentSession;
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    callingParentSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingParentSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);
}

/**
 * @tc.name: CloseKeyBoardSyncTransaction3
 * @tc.desc: test function : CloseKeyBoardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CloseKeyBoardSyncTransaction3, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyBoardSyncTransaction3";
    std::string bundleName = "CloseKeyBoardSyncTransaction3";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);
    ASSERT_NE(keyboardSession, nullptr);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    bool isRotating = false;

    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = [](uint32_t callingSessionId) {};
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(keyboardSession->isKeyboardSyncTransactionOpen_, false);
}

/**
 * @tc.name: CloseKeyboardSyncTransaction4
 * @tc.desc: test function : CloseKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CloseKeyboardSyncTransaction4, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction4";
    std::string bundleName = "CloseKeyboardSyncTransaction4";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);
    ASSERT_NE(keyboardSession, nullptr);
    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    keyboardSession->dirtyFlags_ = 0;

    keyboardSession->isKeyboardSyncTransactionOpen_ = false;
    keyboardSession->property_ = nullptr;
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, false);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(0, keyboardSession->dirtyFlags_);

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(keyboardProperty, nullptr);
    keyboardSession->property_ = keyboardProperty;
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    keyboardSession->property_->SetCallingSessionId(1);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, false);
    usleep(WAIT_ASYNC_US);
    auto callingSessionId = keyboardSession->property_->GetCallingSessionId();
    ASSERT_EQ(callingSessionId, INVALID_WINDOW_ID);
}

/**
 * @tc.name: GetRSTransaction01
 * @tc.desc: test function : GetRSTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetRSTransaction01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("GetRSTransaction01",
        "GetRSTransaction01");
    ASSERT_NE(keyboardSession, nullptr);

    auto rsTransaction = keyboardSession->GetRSTransaction();
    ASSERT_EQ(rsTransaction, nullptr);
}

/**
 * @tc.name: GetSessionScreenName01
 * @tc.desc: test function : GetSessionScreenName
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetSessionScreenName01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("GetSessionScreenName01",
        "GetSessionScreenName01");
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto resultStr = keyboardSession->GetSessionScreenName();
    ASSERT_EQ(resultStr, "");

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(keyboardProperty, nullptr);
    keyboardSession->property_ = keyboardProperty;
    keyboardSession->property_->displayId_ = 100;

    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ASSERT_NE(screenSession, nullptr);
    screenSession->name_ = "testScreenSession";
    screenSession->screenId_ = 100;
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.emplace(100, screenSession);

    resultStr = keyboardSession->GetSessionScreenName();
    ASSERT_EQ(resultStr, screenSession->name_);
}

/**
 * @tc.name: UseFocusIdIfCallingSessionIdInvalid01
 * @tc.desc: test function : UseFocusIdIfCallingSessionIdInvalid
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UseFocusIdIfCallingSessionIdInvalid01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("UseFocusIdIfCallingSessionIdInvalid01",
        "UseFocusIdIfCallingSessionIdInvalid01");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCallback =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    ASSERT_NE(keyboardCallback, nullptr);
    keyboardSession->keyboardCallback_ = keyboardCallback;
    sptr<SceneSession> sceneSession = GetSceneSession("TestSceneSession", "TestSceneSession");
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->persistentId_ = 100;
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [sceneSession](uint32_t callingSessionId)->sptr<SceneSession> {
            if (sceneSession->persistentId_ != callingSessionId) {
                return nullptr;
            }
            return sceneSession;
        };
    
    keyboardSession->GetSessionProperty()->SetCallingSessionId(100);
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
    auto resultId = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(resultId, 100);

    keyboardSession->GetSessionProperty()->SetCallingSessionId(101);
    keyboardSession->keyboardCallback_->onGetFocusedSessionId_ = []()->int32_t {
        return 100;
    };
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
    resultId = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(resultId, 100);
}

/**
 * @tc.name: UpdateKeyboardAvoidArea01
 * @tc.desc: test function : UpdateKeyboardAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UpdateKeyboardAvoidArea01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("UpdateKeyboardAvoidArea01",
        "UpdateKeyboardAvoidArea01");
    ASSERT_NE(keyboardSession, nullptr);

    // not foreground
    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->UpdateKeyboardAvoidArea();
    ASSERT_EQ(keyboardSession->dirtyFlags_, 0);

    // has callback
    auto expectDirtyFlag = 0;
    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = [&expectDirtyFlag](const uint32_t& persistentId) {
        expectDirtyFlag = 1;
    };
    keyboardSession->UpdateKeyboardAvoidArea();
    if (Session::IsScbCoreEnabled()) {
        expectDirtyFlag = 0 | static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
        ASSERT_EQ(keyboardSession->dirtyFlags_, expectDirtyFlag);
    } else {
        ASSERT_EQ(expectDirtyFlag, 1);
    }

    // miss callback
    expectDirtyFlag = 0;
    keyboardSession->dirtyFlags_ = 1;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = nullptr;
    keyboardSession->UpdateKeyboardAvoidArea();
    if (Session::IsScbCoreEnabled()) {
        ASSERT_EQ(keyboardSession->dirtyFlags_, 1);
    } else {
        ASSERT_EQ(expectDirtyFlag, 0);
    }

    expectDirtyFlag = 0;
    keyboardSession->dirtyFlags_ = 2;
    keyboardSession->specificCallback_ = nullptr;
    keyboardSession->UpdateKeyboardAvoidArea();
    if (Session::IsScbCoreEnabled()) {
        ASSERT_EQ(keyboardSession->dirtyFlags_, 2);
    } else {
        ASSERT_EQ(expectDirtyFlag, 0);
    }
}

/**
 * @tc.name: MoveAndResizeKeyboard01
 * @tc.desc: test function : MoveAndResizeKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, MoveAndResizeKeyboard01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("MoveAndResizeKeyboard01",
        "MoveAndResizeKeyboard01");
    ASSERT_NE(keyboardSession, nullptr);

    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };

    keyboardSession->isScreenAngleMismatch_ = true;
    keyboardSession->targetScreenWidth_ = 300;
    keyboardSession->targetScreenHeight_ = 400;

    // branch SESSION_GRAVITY_BOTTOM
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    Rect expectRect = { 0, 300, 300, 100 };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, false);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);

    //branch SESSION_GRAVITY_DEFAULT
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_DEFAULT;
    expectRect = { 200, 300, 200, 100 };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);
}

/**
 * @tc.name: MoveAndResizeKeyboard02
 * @tc.desc: test function : MoveAndResizeKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, MoveAndResizeKeyboard02, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("MoveAndResizeKeyboard02",
        "MoveAndResizeKeyboard02");
    ASSERT_NE(keyboardSession, nullptr);

    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };

    keyboardSession->isScreenAngleMismatch_ = true;
    keyboardSession->targetScreenWidth_ = 300;
    keyboardSession->targetScreenHeight_ = 400;
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;

    // branch else
    Rect expectRect = param.PortraitKeyboardRect_;
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);
    
    param.PortraitKeyboardRect_ = { 200, 200, 200, 0 };
    auto requestRect = keyboardSession->GetSessionRequestRect();
    expectRect = { requestRect.posX_, requestRect.posY_, requestRect.width_, requestRect.height_ };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);

    param.PortraitKeyboardRect_ = { 200, 200, 0, 0 };
    requestRect = keyboardSession->GetSessionRequestRect();
    expectRect = { requestRect.posX_, requestRect.posY_, requestRect.width_, requestRect.height_ };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);
}

/**
 * @tc.name: OnCallingSessionUpdated01
 * @tc.desc: test function : OnCallingSessionUpdated
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, OnCallingSessionUpdated01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("OnCallingSessionUpdated01",
        "OnCallingSessionUpdated01");
    ASSERT_NE(keyboardSession, nullptr);

    // keyboardSession is not foreground
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_DISCONNECT);

    // keyboardSession's isVisible_ is false
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);

    // keyboardSession's isVisible_ is true
    keyboardSession->isVisible_ = true;
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);

    // callingsession is not nullptr
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    SessionInfo info;
    info.abilityName_ = "OnCallingSessionUpdated01";
    info.bundleName_ = "OnCallingSessionUpdated01";
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(callingSession, nullptr);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };
    // callingSession is fullScreen and isCallingSessionFloating is false
    // keyboardSession's gravity is SessionGravity::SESSION_GRAVITY_DEFAULT
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);

    // keyboardSession's gravity is WindowGravity::Window_GRAVITY_FLOAT
    EXPECT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;

    ASSERT_EQ(keyboardSession->GetKeyboardGravity(), SessionGravity::SESSION_GRAVITY_FLOAT);
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);
}
}  // namespace
}  // namespace Rosen
}  // namespace OHOS
