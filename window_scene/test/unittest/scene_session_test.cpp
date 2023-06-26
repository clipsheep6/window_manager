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

#include <ui/rs_surface_node.h>
#include "session/host/include/session.h"
#include "session/host/include/extension_session.h"
#include "mock/mock_sceneSession_stage.h"
#include "mock/mock_window_event_channel.h"
#include "sceneSession_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string UNDEFINED = "undefined";
}

class TestWindowEventChannel : public IWindowEventChannel {
public:
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    };
};

WSError TestWindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    return WSError::WS_OK;
}

class WindowSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    sptr<SceneSession> scenesceneSession_ = nullptr;
};

void WindowSessionTest::SetUpTestCase()
{
}

void WindowSessionTest::TearDownTestCase()
{
}

void WindowSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sceneSession_ = new (std::nothrow) Session(info);
    sceneSession_->surfaceNode_ = CreateRSSurfaceNode();
    EXPECT_NE(nullptr, sceneSession_);
}

void WindowSessionTest::TearDown()
{
    sceneSession_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "SceneSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

/**
 * @tc.name: IsSessionValid01
 * @tc.desc: check func IsSessionValid
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, IsSessionValid01, Function | SmallTest | Level2)
{
    sceneSession_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_FALSE(sceneSession_->Foreground());
    sceneSession_->state_ = SessionState::STATE_CONNECT;
    ASSERT_TRUE(sceneSession_->Background());
}

/**
 * @tc.name: Connect01
 * @tc.desc: check func Connect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Connect01, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    sceneSession_->state_ = SessionState::STATE_CONNECT;
    SystemSessionConfig systemConfig;
    auto result = sceneSession_->Connect(nullptr, nullptr, nullptr, systemConfig);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    sceneSession_->state_ = SessionState::STATE_DISCONNECT;
    result = sceneSession_->Connect(nullptr, nullptr, nullptr, systemConfig);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    result = sceneSession_->Connect(mockSessionStage, nullptr, surfaceNode, systemConfig);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    result = sceneSession_->Connect(mockSessionStage, testWindowEventChannel, surfaceNode, systemConfig);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Foreground01
 * @tc.desc: check func Foreground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Foreground01, Function | SmallTest | Level2)
{
    sceneSession_->state_ = SessionState::STATE_DISCONNECT;
    auto result = sceneSession_->Foreground();
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    sceneSession_->state_ = SessionState::STATE_CONNECT;
    sceneSession_->isActive_ = true;
    result = sceneSession_->Foreground();
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession_->isActive_ = false;
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Background01
 * @tc.desc: check func Background
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Background01, Function | SmallTest | Level2)
{
    sceneSession_->state_ = SessionState::STATE_CONNECT;
    auto result = sceneSession_->Background();
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    sceneSession_->state_ = SessionState::STATE_INACTIVE;
    result = sceneSession_->Background();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession_->state_, SessionState::STATE_BACKGROUND);
}

/**
 * @tc.name: Disconnect01
 * @tc.desc: check func Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Disconnect01, Function | SmallTest | Level2)
{
    sceneSession_->state_ = SessionState::STATE_CONNECT;
    auto result = sceneSession_->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession_->state_, SessionState::STATE_CONNECT);

    sceneSession_->state_ = SessionState::STATE_BACKGROUND;
    result = sceneSession_->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession_->state_, SessionState::STATE_DISCONNECT);
}

/**
 * @tc.name: PendingSessionActivation01
 * @tc.desc: check func PendingSessionActivation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, PendingSessionActivation01, Function | SmallTest | Level2)
{
    int resultValue = 0;
    NotifyPendingSessionActivationFunc callback = [&resultValue](const SessionInfo& info) {
        resultValue = 1;
    };

    sptr<AAFwk::SessionInfo> info = new (std::nothrow)AAFwk::SessionInfo();
    sceneSession_->pendingSessionActivationFunc_ = nullptr;
    sceneSession_->PendingSessionActivation(info);
    ASSERT_EQ(resultValue, 0);

    sceneSession_->SetPendingSessionActivationEventListener(callback);
    sceneSession_->PendingSessionActivation(info);
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: TerminateSession01
 * @tc.desc: check func TerminateSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TerminateSession01, Function | SmallTest | Level2)
{
    int resultValue = 0;
    NotifyTerminateSessionFunc callback = [&resultValue](const SessionInfo& info) {
        resultValue = 1;
    };

    sptr<AAFwk::SessionInfo> info = new (std::nothrow)AAFwk::SessionInfo();
    sceneSession_->terminateSessionFunc_ = nullptr;
    sceneSession_->TerminateSession(info);
    ASSERT_EQ(resultValue, 0);

    sceneSession_->SetTerminateSessionListener(callback);
    sceneSession_->TerminateSession(info);
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifySessionException01
 * @tc.desc: check func NotifySessionException
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifySessionException01, Function | SmallTest | Level2)
{
    int resultValue = 0;
    NotifySessionExceptionFunc callback = [&resultValue](const SessionInfo& info) {
        resultValue = 1;
    };

    sptr<AAFwk::SessionInfo> info = new (std::nothrow)AAFwk::SessionInfo();
    sceneSession_->sessionExceptionFunc_ = nullptr;
    sceneSession_->NotifySessionException(info);
    ASSERT_EQ(resultValue, 0);

    sceneSession_->SetSessionExceptionListener(callback);
    sceneSession_->NotifySessionException(info);
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: TransferPointerEvent01
 * @tc.desc: check func TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferPointerEvent01, Function | SmallTest | Level2)
{
    sceneSession_->windowEventChannel_ = nullptr;
    auto result = sceneSession_->TransferPointerEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    sceneSession_->windowEventChannel_ = testWindowEventChannel;

    result = sceneSession_->TransferPointerEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferKeyEvent01, Function | SmallTest | Level2)
{
    sceneSession_->windowEventChannel_ = nullptr;
    auto result = sceneSession_->TransferKeyEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    sceneSession_->windowEventChannel_ = testWindowEventChannel;

    result = sceneSession_->TransferKeyEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateActiveStatus01
 * @tc.desc: check func UpdateActiveStatus01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateActiveStatus01, Function | SmallTest | Level2)
{
    ASSERT_NE(sceneSession_, nullptr);
    sceneSession_->isActive_ = false;
    sceneSession_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    auto result = sceneSession_->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);

    result = sceneSession_->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(SessionState::STATE_ACTIVE, sceneSession_->state_);
}

/**
 * @tc.name: UpdateActiveStatus02
 * @tc.desc: check func UpdateActiveStatus02
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateActiveStatus02, Function | SmallTest | Level2)
{
    ASSERT_NE(sceneSession_, nullptr);
    sceneSession_->isActive_ = false;
    sceneSession_->UpdateSessionState(SessionState::STATE_INACTIVE);
    auto result = sceneSession_->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
    ASSERT_EQ(SessionState::STATE_INACTIVE, sceneSession_->state_);
    ASSERT_EQ(false, sceneSession_->isActive_);

    sceneSession_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    result = sceneSession_->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(SessionState::STATE_ACTIVE, sceneSession_->state_);

    result = sceneSession_->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(SessionState::STATE_INACTIVE, sceneSession_->state_);
}

/**
 * @tc.name: SetSessionRect
 * @tc.desc: check func SetSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionRect, Function | SmallTest | Level2)
{
    ASSERT_NE(sceneSession_, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    sceneSession_->SetSessionRect(rect);
    ASSERT_EQ(rect, sceneSession_->winRect_);
}

/**
 * @tc.name: GetSessionRect
 * @tc.desc: check func GetSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetSessionRect, Function | SmallTest | Level2)
{
    ASSERT_NE(sceneSession_, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    sceneSession_->SetSessionRect(rect);
    ASSERT_EQ(rect, sceneSession_->GetSessionRect());
}

/**
 * @tc.name: CheckDialogOnForeground
 * @tc.desc: check func CheckDialogOnForeground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, CheckDialogOnForeground, Function | SmallTest | Level2)
{
    ASSERT_NE(sceneSession_, nullptr);
    sceneSession_->dialogVec_.clear();
    ASSERT_EQ(false, sceneSession_->CheckDialogOnForeground());
    SessionInfo info;
    info.abilityName_ = "dialogAbilityName";
    info.moduleName_ = "dialogModuleName";
    info.bundleName_ = "dialogBundleName";
    sptr<Session> dialogSession = new (std::nothrow) Session(info);
    ASSERT_NE(dialogSession, nullptr);
    dialogSession->state_ = SessionState::STATE_INACTIVE;
    sceneSession_->dialogVec_.push_back(dialogSession);
    ASSERT_EQ(false, sceneSession_->CheckDialogOnForeground());
    sceneSession_->dialogVec_.clear();
}

/**
 * @tc.name: NotifyDestroy
 * @tc.desc: check func NotifyDestroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, NotifyDestroy, Function | SmallTest | Level2)
{
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    sceneSession_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), NotifyDestroy()).Times(1).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, sceneSession_->NotifyDestroy());
    sceneSession_->sessionStage_ = nullptr;
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, sceneSession_->NotifyDestroy());
}

/**
 * @tc.name: RequestSessionBack
 * @tc.desc: request session back
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, RequestSessionBack, Function | SmallTest | Level2)
{
    ASSERT_NE(sceneSession_, nullptr);

    ASSERT_EQ(WSError::WS_DO_NOTHING, sceneSession_->RequestSessionBack());

    NotifyBackPressedFunc callback = []() {};

    sceneSession_->SetBackPressedListenser(callback);
    ASSERT_EQ(WSError::WS_OK, sceneSession_->RequestSessionBack());
}
}
} // namespace Rosen
} // namespace OHOS
