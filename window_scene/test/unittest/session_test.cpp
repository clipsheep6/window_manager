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
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"

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
    sptr<Session> session_ = nullptr;
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
    info.bundleName_ = "testSession2";
    session_ = new (std::nothrow) Session(info);
    session_->surfaceNode_ = CreateRSSurfaceNode();
    EXPECT_NE(nullptr, session_);
}

void WindowSessionTest::TearDown()
{
    session_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: SetActive01
 * @tc.desc: set session active
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(WindowSessionTest, SetActive01, Function | SmallTest | Level2)
{
    sptr<ISession> sessionToken = nullptr;
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    EXPECT_CALL(*(mockSessionStage), SetActive(_)).WillOnce(Return(WSError::WS_OK));
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _)).Times(1).WillOnce(Return(WSError::WS_OK));
    session_->sessionStage_ = mockSessionStage;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->SetActive(true));

    sptr<WindowEventChannelMocker> mockEventChannel = new(std::nothrow) WindowEventChannelMocker(mockSessionStage);
    EXPECT_NE(nullptr, mockEventChannel);
    auto surfaceNode = CreateRSSurfaceNode();
    SystemSessionConfig sessionConfig;
    ASSERT_EQ(WSError::WS_OK, session_->Connect(mockSessionStage, mockEventChannel, surfaceNode, sessionConfig));
    ASSERT_EQ(WSError::WS_OK, session_->SetActive(true));
    ASSERT_EQ(false, session_->isActive_);

    session_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    ASSERT_EQ(WSError::WS_OK, session_->SetActive(true));
    ASSERT_EQ(true, session_->isActive_);
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: update rect
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(WindowSessionTest, UpdateRect01, Function | SmallTest | Level2)
{
    sptr<ISession> sessionToken = nullptr;
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _)).Times(1).WillOnce(Return(WSError::WS_OK));

    WSRect rect = {0, 0, 0, 0};
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->UpdateRect(rect, SizeChangeReason::UNDEFINED));
    sptr<WindowEventChannelMocker> mockEventChannel = new(std::nothrow) WindowEventChannelMocker(mockSessionStage);
    EXPECT_NE(nullptr, mockEventChannel);
    SystemSessionConfig sessionConfig;
    ASSERT_EQ(WSError::WS_OK, session_->Connect(mockSessionStage, mockEventChannel, nullptr, sessionConfig));

    rect = {0, 0, 100, 100};
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _)).Times(1).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, session_->UpdateRect(rect, SizeChangeReason::UNDEFINED));
    ASSERT_EQ(rect, session_->winRect_);
}

/**
 * @tc.name: IsSessionValid01
 * @tc.desc: check func IsSessionValid
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, IsSessionValid01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_FALSE(session_->IsSessionValid());
    session_->state_ = SessionState::STATE_CONNECT;
    ASSERT_TRUE(session_->IsSessionValid());
}

/**
 * @tc.name: Connect01
 * @tc.desc: check func Connect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Connect01, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    session_->state_ = SessionState::STATE_CONNECT;
    SystemSessionConfig systemConfig;
    auto result = session_->Connect(nullptr, nullptr, nullptr, systemConfig);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_DISCONNECT;
    result = session_->Connect(nullptr, nullptr, nullptr, systemConfig);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    result = session_->Connect(mockSessionStage, nullptr, surfaceNode, systemConfig);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    result = session_->Connect(mockSessionStage, testWindowEventChannel, surfaceNode, systemConfig);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Foreground01
 * @tc.desc: check func Foreground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Foreground01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_DISCONNECT;
    auto result = session_->Foreground();
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_CONNECT;
    session_->isActive_ = true;
    result = session_->Foreground();
    ASSERT_EQ(result, WSError::WS_OK);

    session_->isActive_ = false;
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Background01
 * @tc.desc: check func Background
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Background01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_CONNECT;
    auto result = session_->Background();
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    session_->state_ = SessionState::STATE_INACTIVE;
    result = session_->Background();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(session_->state_, SessionState::STATE_BACKGROUND);
}

/**
 * @tc.name: Disconnect01
 * @tc.desc: check func Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, Disconnect01, Function | SmallTest | Level2)
{
    session_->state_ = SessionState::STATE_CONNECT;
    auto result = session_->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(session_->state_, SessionState::STATE_CONNECT);

    session_->state_ = SessionState::STATE_BACKGROUND;
    result = session_->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(session_->state_, SessionState::STATE_DISCONNECT);
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

    SessionInfo info;
    session_->pendingSessionActivationFunc_ = nullptr;
    session_->PendingSessionActivation(info);
    ASSERT_EQ(resultValue, 0);

    session_->SetPendingSessionActivationEventListener(callback);
    session_->PendingSessionActivation(info);
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: TransferPointerEvent01
 * @tc.desc: check func TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferPointerEvent01, Function | SmallTest | Level2)
{
    session_->windowEventChannel_ = nullptr;
    auto result = session_->TransferPointerEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    session_->windowEventChannel_ = testWindowEventChannel;

    result = session_->TransferPointerEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, TransferKeyEvent01, Function | SmallTest | Level2)
{
    session_->windowEventChannel_ = nullptr;
    auto result = session_->TransferKeyEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<TestWindowEventChannel> testWindowEventChannel = new(std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);
    session_->windowEventChannel_ = testWindowEventChannel;

    result = session_->TransferKeyEvent(nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateActiveStatus01
 * @tc.desc: check func UpdateActiveStatus01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateActiveStatus01, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->isActive_ = false;
    session_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    auto result = session_->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);

    result = session_->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(SessionState::STATE_ACTIVE, session_->state_);
}

/**
 * @tc.name: UpdateActiveStatus02
 * @tc.desc: check func UpdateActiveStatus02
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, UpdateActiveStatus02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->isActive_ = false;
    session_->UpdateSessionState(SessionState::STATE_INACTIVE);
    auto result = session_->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
    ASSERT_EQ(SessionState::STATE_INACTIVE, session_->state_);
    ASSERT_EQ(false, session_->isActive_);

    session_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    result = session_->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(SessionState::STATE_ACTIVE, session_->state_);

    result = session_->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(SessionState::STATE_INACTIVE, session_->state_);
}

/**
 * @tc.name: SetSessionRect
 * @tc.desc: check func SetSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, SetSessionRect, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    session_->SetSessionRect(rect);
    ASSERT_EQ(rect, session_->winRect_);
}

/**
 * @tc.name: GetSessionRect
 * @tc.desc: check func GetSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest, GetSessionRect, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    WSRect rect = { 0, 0, 320, 240}; // width: 320, height: 240
    session_->SetSessionRect(rect);
    ASSERT_EQ(rect, session_->GetSessionRect());
}
}
} // namespace Rosen
} // namespace OHOS
