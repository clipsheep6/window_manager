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
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string UNDEFINED = "undefined";
}

class WindowSessionTest3 : public testing::Test {
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

void WindowSessionTest3::SetUpTestCase()
{
}

void WindowSessionTest3::TearDownTestCase()
{
}

void WindowSessionTest3::SetUp()
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

void WindowSessionTest3::TearDown()
{
    session_ = nullptr;
    usleep(WAIT_SYNC_IN_NS);
}

RSSurfaceNode::SharedPtr WindowSessionTest3::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTest2SurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (surfaceNode == nullptr) {
        GTEST_LOG_(INFO) << "WindowSessionTest2::CreateRSSurfaceNode surfaceNode is nullptr";
    }
    return surfaceNode;
}

int32_t WindowSessionTest3::GetTaskCount()
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
 * @tc.name: NotifyContextTransparent
 * @tc.desc: NotifyContextTransparent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, NotifyContextTransparent, Function | SmallTest | Level2)
{
    NotifyContextTransparentFunc contextTransparentFunc = session_->contextTransparentFunc_;
    if (contextTransparentFunc == nullptr) {
        contextTransparentFunc = {};
    }
    session_->contextTransparentFunc_ = nullptr;
    session_->NotifyContextTransparent();

    session_->SetContextTransparentFunc(contextTransparentFunc);
    session_->NotifyContextTransparent();
    session_->SetPendingSessionToBackgroundForDelegatorListener(nullptr);
    EXPECT_EQ(WSError::WS_OK, session_->PendingSessionToBackgroundForDelegator());
}
//  2

/**
 * @tc.name: Foreground02
 * @tc.desc: Foreground Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, Foreground02, Function | SmallTest | Level2)
{
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    session_->SetSessionState(SessionState::STATE_BACKGROUND);//1
    session_->isActive_ = true;
    auto result = session_->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    session_->SetSessionState(SessionState::STATE_INACTIVE);//2
    session_->isActive_ = false;//3
    auto result02 = session_->Foreground(property);
    ASSERT_EQ(result02, WSError::WS_OK);
}
//  3

/**
 * @tc.name: Foreground03
 * @tc.desc: Foreground Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, Foreground03, Function | SmallTest | Level2)
{
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    session_->SetSessionState(SessionState::STATE_BACKGROUND);
    session_->isActive_ = true;

    property->type_ = WindowType::WINDOW_TYPE_DIALOG;//4 5
    auto result = session_->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    SessionInfo parentInfo;
    parentInfo.abilityName_ = "testSession1";
    parentInfo.moduleName_ = "testSession2";
    parentInfo.bundleName_ = "testSession3";
    sptr<Session> parentSession = new (std::nothrow) Session(parentInfo);
    session_->SetParentSession(parentSession);//6 7
    auto result02 = session_->Foreground(property);
    ASSERT_EQ(result02, WSError::WS_OK);
}
//  4

/**
 * @tc.name: SetFocusable04
 * @tc.desc: SetFocusable Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, SetFocusable04, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->property_ = nullptr;
    auto result = session_->SetFocusable(false);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    session_->isFocused_ = true;
    session_->property_ = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(session_->property_, nullptr);
    session_->property_->SetFocusable(false);

    auto result02 = session_->SetFocusable(false);
    EXPECT_EQ(result02, WSError::WS_OK);
}
//  2

/**
 * @tc.name: SetTouchable03
 * @tc.desc: IsSessionValid() and touchable return true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, SetTouchable03, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->SetSessionState(SessionState::STATE_FOREGROUND);
    session_->sessionInfo_.isSystem_ = false;
    EXPECT_EQ(WSError::WS_OK, session_->SetTouchable(true));
}
//  1

/**
 * @tc.name: GetTouchable02
 * @tc.desc: GetTouchable Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, GetTouchable02, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    session_->property_ = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(session_->property_, nullptr);
    EXPECT_EQ(true, session_->GetTouchable());

    session_->property_ = nullptr;
    ASSERT_EQ(session_->property_, nullptr);
    EXPECT_EQ(true, session_->GetTouchable());
}
//  1

/**
 * @tc.name: UpdateDensity02
 * @tc.desc: UpdateDensity Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, UpdateDensity02, Function | SmallTest | Level2)
{
    session_->SetSessionState(SessionState::STATE_FOREGROUND);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;
    auto result = session_->UpdateDensity();
    EXPECT_EQ(result, WSError::WS_OK);
}
//  1

/**
 * @tc.name: UpdateOrientation
 * @tc.desc: UpdateOrientation Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, UpdateOrientation, Function | SmallTest | Level2)
{
    session_->sessionInfo_.isSystem_ = true;
    auto result = session_->UpdateOrientation();
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);// 1

    session_->sessionInfo_.isSystem_ = false;
    session_->SetSessionState(SessionState::STATE_FOREGROUND);// 1
    auto result02 = session_->UpdateOrientation();// 1
    EXPECT_EQ(result02, WSError::WS_ERROR_NULLPTR);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;// 1
    auto result03 = session_->UpdateOrientation();
    EXPECT_EQ(result03, WSError::WS_OK);
}
//  4

/**
 * @tc.name: HandleDialogBackground
 * @tc.desc: HandleDialogBackground Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, HandleDialogBackground, Function | SmallTest | Level2)
{
    session_->property_ = new (std::nothrow) WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    session_->HandleDialogBackground();// 1

    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<Session> session01 = nullptr;// 1

    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sptr<Session> session02 = new (std::nothrow) Session(info);// 1
    sptr<Session> session03 = new (std::nothrow) Session(info);//
    EXPECT_NE(session02, nullptr);
    EXPECT_NE(session03, nullptr);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session02->sessionStage_ = mockSessionStage;

    session03->sessionStage_ = nullptr;

    session_->dialogVec_.push_back(session01);
    session_->dialogVec_.push_back(session02);// 2
    session_->dialogVec_.push_back(session03);
    session_->HandleDialogBackground();
    session_->SetPendingSessionToBackgroundForDelegatorListener(nullptr);
    EXPECT_EQ(WSError::WS_OK, session_->PendingSessionToBackgroundForDelegator());
}
// 5

/**
 * @tc.name: HandleDialogForeground
 * @tc.desc: HandleDialogForeground Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, HandleDialogForeground, Function | SmallTest | Level2)
{
    session_->property_ = new (std::nothrow) WindowSessionProperty();
    session_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    session_->HandleDialogForeground();// 1

    session_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);// 1
    sptr<Session> session01 = nullptr;// 1

    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sptr<Session> session02 = new (std::nothrow) Session(info);// 1
    sptr<Session> session03 = new (std::nothrow) Session(info);//
    EXPECT_NE(session02, nullptr);
    EXPECT_NE(session03, nullptr);

    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session02->sessionStage_ = mockSessionStage;
    session03->sessionStage_ = nullptr;

    session_->dialogVec_.push_back(session01);
    session_->dialogVec_.push_back(session02);// 2
    session_->dialogVec_.push_back(session03);
    session_->HandleDialogForeground();
    session_->SetPendingSessionToBackgroundForDelegatorListener(nullptr);
    EXPECT_EQ(WSError::WS_OK, session_->PendingSessionToBackgroundForDelegator());
}
//  5

/**
 * @tc.name: Background
 * @tc.desc: Background Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, Background, Function | SmallTest | Level2)
{
    session_->SetSessionState(SessionState::STATE_ACTIVE);
    session_->Background();// 2
    auto result = session_->Background();// 1
    session_->SetSessionSnapshotListener(nullptr);// 1
    EXPECT_EQ(result, WSError::WS_OK);
}
//  4

/**
 * @tc.name: Background02
 * @tc.desc: Background Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, Background02, Function | SmallTest | Level2)
{
    session_->SetSessionState(SessionState::STATE_ACTIVE);
    session_->property_ = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(session_->property_, nullptr);
    session_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    auto result = session_->Background();
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}
//  2

/**
 * @tc.name: SetActive
 * @tc.desc: SetActive Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, SetActive, Function | SmallTest | Level2)
{
    session_->SetSessionState(SessionState::STATE_CONNECT);// 1
    auto result = session_->SetActive(false);// 1
    EXPECT_EQ(result, WSError::WS_DO_NOTHING);
}
// 2

/**
 * @tc.name: SetActive02
 * @tc.desc: SetActive Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, SetActive02, Function | SmallTest | Level2)
{
    session_->SetSessionState(SessionState::STATE_FOREGROUND);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;
    auto result = session_->SetActive(true);// 3
    EXPECT_EQ(result, WSError::WS_OK);
}
//  4

/**
 * @tc.name: SetActive03
 * @tc.desc: SetActive Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, SetActive03, Function | SmallTest | Level2)
{
    session_->SetSessionState(SessionState::STATE_CONNECT);// 1
    auto result = session_->SetActive(true);
    EXPECT_EQ(result, WSError::WS_OK);

    session_->isActive_ = true;
    auto result02 = session_->SetActive(false);
    EXPECT_EQ(result02, WSError::WS_OK);

    session_->SetSessionState(SessionState::STATE_ACTIVE);
    session_->isActive_ = true;
    auto result03 = session_->SetActive(false);
    EXPECT_EQ(result03, WSError::WS_OK);
}
//  4

/**
 * @tc.name: NotifyCloseExistPipWindow02
 * @tc.desc: NotifyCloseExistPipWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionTest3, NotifyCloseExistPipWindow02, Function | SmallTest | Level2)
{
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), NotifyCloseExistPipWindow()).Times(::testing::AtLeast(1)).WillOnce(Return(WSError::WS_OK));

    session_->sessionStage_ = nullptr;
    auto result = session_->NotifyCloseExistPipWindow();
    EXPECT_EQ(result, WSError::WS_ERROR_NULLPTR);
}
// 2
}
} // namespace Rosen
} // namespace OHOS