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

#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerTest9 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest9::ssm_ = nullptr;

void NotifyRecoverSceneSessionFuncTest(const sptr<SceneSession>& session, const SessionInfo& sessionInfo)
{
}

bool getStateFalse(const ManagerState key)
{
    return false;
}

bool getStateTrue(const ManagerState key)
{
    return true;
}

bool TraverseFuncTest(const sptr<SceneSession>& session)
{
    return true;
}

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest9::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest9::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest9::SetUp()
{
}

void SceneSessionManagerTest9::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: TraverseSessionTreeFromTopToBottom
 * @tc.desc: TraverseSessionTreeFromTopToBottom
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TraverseSessionTreeFromTopToBottom, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->TraverseSessionTreeFromTopToBottom(TraverseFuncTest);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TraverseSessionTreeFromTopToBottom";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession));
    ssm_->TraverseSessionTreeFromTopToBottom(TraverseFuncTest);
}

/**
 * @tc.name: TestRequestFocusStatus_01
 * @tc.desc: Test RequestFocusStatus with sceneSession is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestFocusStatus_01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    auto res = ssm_->RequestFocusStatus(1, false, false, FocusChangeReason::FLOATING_SCENE);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestRequestFocusStatus_02
 * @tc.desc: Test RequestFocusStatus with not call by the same process
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestFocusStatus_02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestFocusStatus02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession->SetCallingPid(0);
    sceneSession->isVisible_ = false;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto res = ssm_->RequestFocusStatus(1, false, false, FocusChangeReason::FLOATING_SCENE);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestRequestSessionFocusImmediately_01
 * @tc.desc: Test RequestSessionFocusImmediately with invalid persistentId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocusImmediately_01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    WSError ret = ssm_->RequestSessionFocusImmediately(0);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: TestRequestSessionFocusImmediately_02
 * @tc.desc: Test RequestSessionFocusImmediately with session is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocusImmediately_02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    WSError ret = ssm_->RequestSessionFocusImmediately(2);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: TestRequestSessionFocusImmediately_03
 * @tc.desc: Test RequestSessionFocusImmediately with session is not focusable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocusImmediately_03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocusImmediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession->SetFocusable(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    auto ret = ssm_->RequestSessionFocusImmediately(1);
    EXPECT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocusImmediately_04
 * @tc.desc: Test RequestSessionFocusImmediately with session is not focused on show
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocusImmediately_04, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocusImmediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
  
    sceneSession->SetFocusable(true);
    sceneSession->SetFocusedOnShow(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto ret = ssm_->RequestSessionFocusImmediately(1);
    EXPECT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocusImmediately_05
 * @tc.desc: Test RequestSessionFocusImmediately with ForceHideState HIDDEN_WHEN_FOCUSED
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocusImmediately_05, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocusImmediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
  
    sceneSession->SetFocusable(true);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto ret = ssm_->RequestSessionFocusImmediately(1);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: TestRequestSessionFocusImmediately_06
 * @tc.desc: Test RequestSessionFocusImmediately with ForceHideState NOT_HIDDEN
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocusImmediately_06, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocusImmediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
  
    sceneSession->SetFocusable(true);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetForceHideState(ForceHideState::NOT_HIDDEN);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    WSError ret = ssm_->RequestSessionFocusImmediately(1);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: Test RequestSessionFocus_01
 * @tc.desc: Test RequestSessionFocus invalid persistentId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestSessionFocus_01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    WSError ret = ssm_->RequestSessionFocus(0);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: RequestSessionFocus_02
 * @tc.desc: Test RequestSessionFocus with session is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestSessionFocus_02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();

    WSError ret = ssm_->RequestSessionFocus(2);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: TestRequestSessionFocus_03
 * @tc.desc: Test RequestSessionFocus with session is not focusable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_03";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(false);
    sceneSession->UpdateVisibilityInner(true);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocus_04
 * @tc.desc: Test RequestSessionFocus with session is not visible
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_04, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_04";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocus_05
 * @tc.desc: Test RequestSessionFocus with session is not focused on show
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_05, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_05";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetFocusedOnShow(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocus_06
 * @tc.desc: Test RequestSessionFocus with session is not focusable on show
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_06, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_06";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetFocusableOnShow(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::FOREGROUND);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocus_07
 * @tc.desc: Test RequestSessionFocus with windowType is APP_SUB_WINDOW_BASE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_07, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_07";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetFocusableOnShow(true);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TestRequestSessionFocus_08
 * @tc.desc: Test RequestSessionFocus with windowType is WINDOW_TYPE_DIALOG
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_08, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_08";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetFocusableOnShow(true);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: TestRequestSessionFocus_09
 * @tc.desc: Test RequestSessionFocus with ForceHideState HIDDEN_WHEN_FOCUSED
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_09, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TestRequestSessionFocus_08";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetFocusableOnShow(true);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: TestRequestSessionFocus_010
 * @tc.desc: Test RequestSessionFocus with ForceHideState HIDDEN_WHEN_FOCUSED
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TestRequestSessionFocus_010, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocus02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->SetFocusedOnShow(true);
    sceneSession->SetFocusableOnShow(false);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetForceHideState(ForceHideState::NOT_HIDDEN);
    sceneSession->SetTopmost(true);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    WSError ret = ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(sceneSession->isFocused_, true);
}

/**
 * @tc.name: RequestSessionUnfocus02
 * @tc.desc: RequestSessionUnfocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestSessionUnfocus02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    WSError ret = ssm_->RequestSessionUnfocus(0, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_SESSION);
    ret = ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionUnfocus02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->focusedSessionId_ = 2;
    sceneSession->persistentId_ = 1;
    sceneSession->SetZOrder(50);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->isVisible_ = true;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    SessionInfo sessionInfo1;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->GetSessionProperty()->SetParentPersistentId(1);
    sceneSession1->persistentId_ = 2;
    sceneSession1->SetZOrder(100);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    ret = ssm_->RequestSessionUnfocus(2, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(ssm_->focusedSessionId_, 1);

    ssm_->lastFocusedSessionId_ = 4;
    sceneSession1->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    SessionInfo sessionInfo2;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo2, nullptr);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession2->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    sceneSession2->SetZOrder(20);
    sceneSession2->state_ = SessionState::STATE_FOREGROUND;
    sceneSession2->isVisible_ = true;
    sceneSession2->persistentId_ = 4;
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession2));
    ret = ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(ssm_->focusedSessionId_, 4);
}

/**
 * @tc.name: RequestAllAppSessionUnfocusInner
 * @tc.desc: RequestAllAppSessionUnfocusInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestAllAppSessionUnfocusInner, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->RequestAllAppSessionUnfocusInner();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestAllAppSessionUnfocusInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->focusedSessionId_ = 1;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestAllAppSessionUnfocusInner();

    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    ssm_->RequestAllAppSessionUnfocusInner();
}

/**
 * @tc.name: UpdateFocus04
 * @tc.desc: UpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, UpdateFocus04, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "UpdateFocus04";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->focusedSessionId_ = 0;
    sceneSession->UpdateFocus(false);
    ssm_->UpdateFocus(1, false);

    ssm_->listenerController_ = nullptr;
    ssm_->UpdateFocus(1, true);

    std::shared_ptr<SessionListenerController> listenerController = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    ssm_->UpdateFocus(1, true);

    sessionInfo.isSystem_ = false;
    ssm_->focusedSessionId_ = 1;
    sceneSession->UpdateFocus(true);
    ssm_->UpdateFocus(1, false);

    sceneSession->UpdateFocus(false);
    ssm_->UpdateFocus(1, true);
}

/**
 * @tc.name: ProcessFocusWhenForeground
 * @tc.desc: ProcessFocusWhenForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessFocusWhenForeground, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessFocusWhenForeground";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->persistentId_ = 1;
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessFocusWhenForeground(sceneSession);

    sceneSession->isVisible_ = true;
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessFocusWhenForeground(sceneSession);
}

/**
 * @tc.name: ProcessFocusWhenForeground01
 * @tc.desc: ProcessFocusWhenForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessFocusWhenForeground01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessFocusWhenForeground";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession->persistentId_ = 1;
    ssm_->focusedSessionId_ = 2;
    sceneSession->SetFocusedOnShow(false);
    ssm_->ProcessFocusWhenForeground(sceneSession);
    EXPECT_EQ(sceneSession->IsFocusedOnShow(), false);

    sceneSession->isVisible_ = true;
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessFocusWhenForeground(sceneSession);
    EXPECT_EQ(sceneSession->IsFocusedOnShow(), true);
}

/**
 * @tc.name: ProcessSubSessionForeground03
 * @tc.desc: ProcessSubSessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessSubSessionForeground03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessSubSessionForeground03";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    SessionInfo subSessionInfo;
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(subSessionInfo, nullptr);
    ASSERT_NE(nullptr, subSceneSession);
    subSceneSession->SetTopmost(true);
    subSceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    subSceneSession->persistentId_ = 1;
    sceneSession->GetSubSession().push_back(subSceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, subSceneSession));

    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;

    SessionInfo subSessionInfo1;
    sptr<SceneSession> subSceneSession1 = sptr<SceneSession>::MakeSptr(subSessionInfo1, nullptr);
    ASSERT_NE(nullptr, subSceneSession1);
    subSceneSession1->SetTopmost(true);
    subSceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    subSceneSession1->persistentId_ = 0;
    sceneSession->GetSubSession().push_back(subSceneSession1);

    SessionInfo subSessionInfo2;
    sptr<SceneSession> subSceneSession2 = sptr<SceneSession>::MakeSptr(subSessionInfo2, nullptr);
    ASSERT_NE(nullptr, subSceneSession2);
    subSceneSession2->SetTopmost(true);
    subSceneSession2->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->GetSubSession().push_back(subSceneSession2);

    SessionInfo subSessionInfo3;
    sptr<SceneSession> subSceneSession3 = sptr<SceneSession>::MakeSptr(subSessionInfo3, nullptr);
    ASSERT_NE(nullptr, subSceneSession3);
    subSceneSession3->SetTopmost(false);
    subSceneSession3->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->GetSubSession().push_back(subSceneSession3);

    sceneSession->GetSubSession().push_back(nullptr);

    ssm_->ProcessSubSessionForeground(sceneSession);

    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessSubSessionForeground(sceneSession);

    ssm_->focusedSessionId_ = 2;
    ssm_->ProcessSubSessionForeground(sceneSession);
}

/**
 * @tc.name: ProcessFocusWhenForegroundScbCore
 * @tc.desc: ProcessFocusWhenForegroundScbCore
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessFocusWhenForegroundScbCore, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->focusedSessionId_ = 0;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessFocusWhenForegroundScbCore";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession->persistentId_ = 1;
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->SetFocusableOnShow(false);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->ProcessFocusWhenForegroundScbCore(sceneSession);
    ASSERT_EQ(sceneSession->GetPostProcessFocusState().isFocused_, false);
    ASSERT_EQ(ssm_->focusedSessionId_, 0);

    sceneSession->SetFocusableOnShow(true);
    ssm_->ProcessFocusWhenForegroundScbCore(sceneSession); // SetPostProcessFocusState
    ASSERT_EQ(sceneSession->GetPostProcessFocusState().isFocused_, true);

    sceneSession->isVisible_ = true;
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessFocusWhenForegroundScbCore(sceneSession); // RequestSessionFocus
    ASSERT_EQ(ssm_->focusedSessionId_, 1);
}

/**
 * @tc.name: ProcessModalTopmostRequestFocusImmdediately02
 * @tc.desc: ProcessModalTopmostRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessModalTopmostRequestFocusImmdediately02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessModalTopmostRequestFocusImmdediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    SessionInfo subSessionInfo;
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(subSessionInfo, nullptr);
    ASSERT_NE(nullptr, subSceneSession);
    subSceneSession->SetTopmost(true);
    subSceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    subSceneSession->persistentId_ = 1;
    subSceneSession->SetFocusable(true);
    subSceneSession->SetFocusedOnShow(true);
    sceneSession->GetSubSession().push_back(subSceneSession);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, subSceneSession));
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;

    SessionInfo subSessionInfo1;
    sptr<SceneSession> subSceneSession1 = sptr<SceneSession>::MakeSptr(subSessionInfo1, nullptr);
    ASSERT_NE(nullptr, subSceneSession1);
    subSceneSession1->SetTopmost(true);
    subSceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    subSceneSession1->persistentId_ = 0;
    sceneSession->GetSubSession().push_back(subSceneSession1);

    SessionInfo subSessionInfo2;
    sptr<SceneSession> subSceneSession2 = sptr<SceneSession>::MakeSptr(subSessionInfo2, nullptr);
    ASSERT_NE(nullptr, subSceneSession2);
    subSceneSession2->SetTopmost(true);
    subSceneSession2->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->GetSubSession().push_back(subSceneSession2);

    SessionInfo subSessionInfo3;
    sptr<SceneSession> subSceneSession3 = sptr<SceneSession>::MakeSptr(subSessionInfo3, nullptr);
    ASSERT_NE(nullptr, subSceneSession3);
    subSceneSession3->SetTopmost(false);
    subSceneSession3->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->GetSubSession().push_back(subSceneSession3);

    sceneSession->GetSubSession().push_back(nullptr);

    WSError ret = ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);
    ASSERT_NE(ret, WSError::WS_OK);

    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessSubSessionForeground(sceneSession);

    subSceneSession->persistentId_ = 0;
    ret = ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);
    ASSERT_NE(ret, WSError::WS_OK);
}

/**
 * @tc.name: ProcessDialogRequestFocusImmdediately02
 * @tc.desc: ProcessDialogRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessDialogRequestFocusImmdediately02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessDialogRequestFocusImmdediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    SessionInfo dialogSessionInfo;
    sptr<SceneSession> dialogSceneSession = sptr<SceneSession>::MakeSptr(dialogSessionInfo, nullptr);
    ASSERT_NE(nullptr, dialogSceneSession);
    dialogSceneSession->SetTopmost(true);
    dialogSceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    dialogSceneSession->persistentId_ = 1;
    dialogSceneSession->SetFocusable(true);
    dialogSceneSession->SetFocusedOnShow(true);
    sceneSession->GetDialogVector().push_back(dialogSceneSession);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, dialogSceneSession));
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;

    SessionInfo dialogSessionInfo1;
    sptr<SceneSession> dialogSceneSession1 = sptr<SceneSession>::MakeSptr(dialogSessionInfo1, nullptr);
    ASSERT_NE(nullptr, dialogSceneSession1);
    dialogSceneSession1->SetTopmost(true);
    dialogSceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    dialogSceneSession1->persistentId_ = 0;
    sceneSession->GetDialogVector().push_back(dialogSceneSession1);

    SessionInfo dialogSessionInfo2;
    sptr<SceneSession> dialogSceneSession2 = sptr<SceneSession>::MakeSptr(dialogSessionInfo2, nullptr);
    ASSERT_NE(nullptr, dialogSceneSession2);
    dialogSceneSession2->SetTopmost(true);
    dialogSceneSession2->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->GetDialogVector().push_back(dialogSceneSession2);

    sceneSession->GetDialogVector().push_back(nullptr);

    WSError ret = ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);
    ASSERT_NE(ret, WSError::WS_OK);

    dialogSceneSession->persistentId_ = 0;
    ret = ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);
    ASSERT_NE(ret, WSError::WS_OK);
}

/**
 * @tc.name: NotifyCompleteFirstFrameDrawing03
 * @tc.desc: NotifyCompleteFirstFrameDrawing
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, NotifyCompleteFirstFrameDrawing03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "NotifyCompleteFirstFrameDrawing03";
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    abilityInfo->excludeFromMissions = true;
    sessionInfo.abilityInfo = abilityInfo;
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->listenerController_ = nullptr;
    ssm_->NotifyCompleteFirstFrameDrawing(1);

    std::shared_ptr<SessionListenerController> listenerController = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    sessionInfo.isSystem_ = false;
    ssm_->eventHandler_ = nullptr;
    ssm_->NotifyCompleteFirstFrameDrawing(1);

    std::shared_ptr<AppExecFwk::EventHandler> eventHandler = std::make_shared<AppExecFwk::EventHandler>();
    ssm_->eventHandler_ = eventHandler;
    abilityInfo->excludeFromMissions = false;
    ssm_->NotifyCompleteFirstFrameDrawing(1);
}

/**
 * @tc.name: SetSessionLabel02
 * @tc.desc: SetSessionLabel
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, SetSessionLabel02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "SetSessionLabel02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<IRemoteObject> token = sptr<MockIRemoteObject>::MakeSptr();
    ASSERT_NE(nullptr, token);
    sceneSession->SetAbilityToken(token);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->listenerController_ = nullptr;

    std::string label = "testLabel";
    ssm_->SetSessionLabel(token, label);

    std::shared_ptr<SessionListenerController> listenerController = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    sessionInfo.isSystem_ = false;
    ssm_->SetSessionLabel(token, label);

    sessionInfo.isSystem_ = true;
    ssm_->SetSessionLabel(token, label);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession02
 * @tc.desc: RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RecoverAndReconnectSceneSession02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RecoverAndReconnectSceneSession02";
    sessionInfo.moduleName_ = "moduleTest";
    sessionInfo.appIndex_ = 10;
    sessionInfo.persistentId_ = 1;
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    abilityInfo->excludeFromMissions = true;
    sessionInfo.abilityInfo = abilityInfo;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    sptr<WindowSessionProperty> property = sceneSession->GetSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    property->SetPersistentId(1);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowState(WindowState::STATE_SHOWN);
    property->SetRequestedOrientation(Orientation::UNSPECIFIED);
    ssm_->alivePersistentIds_.push_back(1);
    ssm_->recoveringFinished_ = false;
    ssm_->recoverSceneSessionFunc_ = NotifyRecoverSceneSessionFuncTest;

    sptr<ISession> session;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetPersistentId(2);
    result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: RefreshPcZorder
 * @tc.desc: RefreshPcZorder
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RefreshPcZorder, Function | SmallTest | Level3) {
    std::vector<int32_t> persistentIds;
    SessionInfo info1;
    info1.abilityName_ = "RefreshPcZorder1";
    info1.bundleName_ = "RefreshPcZorder1";
    sptr<SceneSession> session1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(session1, nullptr);
    persistentIds.push_back(session1->GetPersistentId());
    ssm_->sceneSessionMap_.insert({session1->GetPersistentId(), session1});
    SessionInfo info2;
    info2.abilityName_ = "RefreshPcZorder2";
    info2.bundleName_ = "RefreshPcZorder2";
    sptr<SceneSession> session2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    ASSERT_NE(session2, nullptr);
    persistentIds.push_back(session2->GetPersistentId());
    ssm_->sceneSessionMap_.insert({session2->GetPersistentId(), session2});
    SessionInfo info3;
    info3.abilityName_ = "RefreshPcZorder3";
    info3.bundleName_ = "RefreshPcZorder3";
    sptr<SceneSession> session3 = sptr<SceneSession>::MakeSptr(info3, nullptr);
    ASSERT_NE(session3, nullptr);
    session3->SetZOrder(404);
    ssm_->sceneSessionMap_.insert({session3->GetPersistentId(), session3});
    persistentIds.push_back(999);
    uint32_t startZOrder = 100;
    std::vector<int32_t> newPersistentIds = persistentIds;
    ssm_->RefreshPcZOrderList(startZOrder, std::move(persistentIds));
    ssm_->RefreshPcZOrderList(UINT32_MAX, std::move(newPersistentIds));
    auto start = std::chrono::system_clock::now();
    // Due to RefreshPcZOrderList being asynchronous, spin lock is added.
    // The spin lock itself is set with a timeout escape time of 3 seconds
    while (true) {
        if ((session1->GetZOrder() != 0 && session2->GetZOrder() != 0 && session1->GetZOrder() != 100) ||
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() >= 3) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_EQ(session2->GetZOrder(), 101);
    ASSERT_EQ(session3->GetZOrder(), 404);
    ASSERT_EQ(session1->GetZOrder(), UINT32_MAX);
}

/**
 * @tc.name: GetSessionRSVisible
 * @tc.desc: GetSessionRSVisible
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, GetSessionRSVisible, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "GetSessionRSVisible";
    sessionInfo.moduleName_ = "moduleTest";
    uint64_t windowId = 10;
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession01->persistentId_ = windowId;
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    currVisibleData.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    struct RSSurfaceNodeConfig config;
    sceneSession02->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(nullptr, sceneSession02->surfaceNode_);
    sceneSession02->surfaceNode_->id_ = 0;
    sceneSession02->persistentId_ = windowId;
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession02));

    bool actual = ssm_->GetSessionRSVisible(sceneSession01, currVisibleData);
    EXPECT_EQ(actual, true);
}

/**
 * @tc.name: CheckClickFocusIsDownThroughFullScreen
 * @tc.desc: CheckClickFocusIsDownThroughFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, CheckClickFocusIsDownThroughFullScreen, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";

    sptr<SceneSession> focusedSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(focusedSession, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    bool ret = ssm_->CheckClickFocusIsDownThroughFullScreen(focusedSession, sceneSession, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, false);
    
    focusedSession->property_->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    ret = ssm_->CheckClickFocusIsDownThroughFullScreen(focusedSession, sceneSession, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, false);

    sceneSession->SetZOrder(50);
    focusedSession->SetZOrder(100);
    focusedSession->blockingFocus_ = true;
    ret = ssm_->CheckClickFocusIsDownThroughFullScreen(focusedSession, sceneSession, FocusChangeReason::CLICK);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: ShiftFocus
 * @tc.desc: ShiftFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ShiftFocus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> focusedSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(focusedSession, nullptr);
    sptr<SceneSession> nextSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nextSession, nullptr);
    focusedSession->persistentId_ = 1;
    nextSession->persistentId_ = 4;
    ssm_->sceneSessionMap_.insert({1, focusedSession});
    ssm_->sceneSessionMap_.insert({4, nextSession});
    ssm_->focusedSessionId_ = 1;
    WSError ret = ssm_->ShiftFocus(nextSession, FocusChangeReason::DEFAULT);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(focusedSession->isFocused_, false);
    ASSERT_EQ(nextSession->isFocused_, true);
}

/**
 * @tc.name: CheckUIExtensionCreation
 * @tc.desc: CheckUIExtensionCreation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, CheckUIExtensionCreation, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::ElementName element;
    int32_t windowId = 5;
    uint32_t callingTokenId = 0;
    int32_t pid = 0;
    AppExecFwk::ExtensionAbilityType extensionAbilityType = AppExecFwk::ExtensionAbilityType::ACTION;
    auto ret = ssm_->CheckUIExtensionCreation(windowId, callingTokenId, element, extensionAbilityType, pid);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> callback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, callback);
    ssm_->sceneSessionMap_.insert(std::pair<int32_t, sptr<SceneSession>>(0, sceneSession));
    windowId = 0;

    Session session(info);
    session.getStateFromManagerFunc_ = getStateFalse;
    ret = ssm_->CheckUIExtensionCreation(windowId, callingTokenId, element, extensionAbilityType, pid);
    ASSERT_EQ(ret, WMError::WM_OK);

    session.getStateFromManagerFunc_ = getStateTrue;
    ssm_->CheckUIExtensionCreation(windowId, callingTokenId, element, extensionAbilityType, pid);
}

/**
 * @tc.name: CheckUIExtensionCreation01
 * @tc.desc: CheckUIExtensionCreation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, CheckUIExtensionCreation01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::ElementName element;
    int32_t windowId = 0;
    uint32_t callingTokenId = 0;
    int32_t pid = 0;
    AppExecFwk::ExtensionAbilityType extensionAbilityType = AppExecFwk::ExtensionAbilityType::ACTION;

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> callback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, callback);
    ssm_->sceneSessionMap_.insert(std::pair<int32_t, sptr<SceneSession>>(0, sceneSession));
    Session session(info);
    session.getStateFromManagerFunc_ = getStateTrue;
    auto ret = ssm_->CheckUIExtensionCreation(windowId, callingTokenId, element, extensionAbilityType, pid);

    session.property_ = nullptr;
    ret = ssm_->CheckUIExtensionCreation(windowId, callingTokenId, element, extensionAbilityType, pid);
    ASSERT_EQ(ret, WMError::WM_OK);

    sceneSession->IsShowOnLockScreen(0);
    session.zOrder_ = 1;
    ssm_->CheckUIExtensionCreation(windowId, callingTokenId, element, extensionAbilityType, pid);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: GetLockScreenZorder
 * @tc.desc: GetLockScreenZorder
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, GetLockScreenZorder, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> callback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, callback);
    constexpr uint32_t DEFAULT_LOCK_SCREEN_ZORDER = 2000;
    constexpr uint32_t LOCK_SCREEN_ZORDER = 2003;

    ssm_->sceneSessionMap_.insert(std::pair<int32_t, sptr<SceneSession>>(0, sceneSession));
    ASSERT_EQ(ssm_->GetLockScreenZorder(), DEFAULT_LOCK_SCREEN_ZORDER);
    Session session(info);
    session.isScreenLockWindow_ = true;
    session.SetZOrder(LOCK_SCREEN_ZORDER);
    ASSERT_EQ(ssm_->GetLockScreenZorder(), LOCK_SCREEN_ZORDER);
    session.SetZOrder(0);
    ASSERT_EQ(ssm_->GetLockScreenZorder(), DEFAULT_LOCK_SCREEN_ZORDER);
    session.SetZOrder(DEFAULT_LOCK_SCREEN_ZORDER);
    ASSERT_EQ(ssm_->GetLockScreenZorder(), DEFAULT_LOCK_SCREEN_ZORDER);
    session.SetZOrder(0DEFAULT_LOCK_SCREEN_ZORDER - 1);
    ASSERT_EQ(ssm_->GetLockScreenZorder(), DEFAULT_LOCK_SCREEN_ZORDER);

    ssm_->pipWindowSurfaceId_ = 0;
    RSSurfaceNodeConfig config;
    session.surfaceNode_ = std::make_shared<RSSurfaceNode>(config, true);
    ssm_->SelectSesssionFromMap(0);
    ssm_->NotifyPiPWindowVisibleChange(true);
    ssm_->NotifyPiPWindowVisibleChange(false);
}

/**
 * @tc.name: IsLastPiPWindowVisible
 * @tc.desc: IsLastPiPWindowVisible
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, IsLastPiPWindowVisible, Function | SmallTest | Level3)
{
    uint64_t surfaceId = 0;
    WindowVisibilityState lastVisibilityState = WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION;
    ssm_->sceneSessionMap_.insert(std::pair<int32_t, sptr<SceneSession>>(0, nullptr));
    auto ret = ssm_->IsLastPiPWindowVisible(surfaceId, lastVisibilityState);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsLastPiPWindowVisible01
 * @tc.desc: IsLastPiPWindowVisible
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, IsLastPiPWindowVisible01, Function | SmallTest | Level3)
{
    uint64_t surfaceId = 0;
    WindowVisibilityState lastVisibilityState = WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION;

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> callback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, callback);
    ssm_->sceneSessionMap_.insert(std::pair<int32_t, sptr<SceneSession>>(0, nullptr));

    struct RSSurfaceNodeConfig config;
    sceneSession->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(nullptr, sceneSession->surfaceNode_);
    sceneSession->surfaceNode_->id_ = 0;
    ssm_->SelectSesssionFromMap(0);
    sptr<WindowSessionProperty> property = sceneSession->GetSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);

    auto ret = ssm_->IsLastPiPWindowVisible(surfaceId, lastVisibilityState);
    ASSERT_EQ(ret, false);
    ssm_->isScreenLocked_ = true;
    ret = ssm_->IsLastPiPWindowVisible(surfaceId, lastVisibilityState);
    ASSERT_EQ(ret, false);
    lastVisibilityState = WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
    ret = ssm_->IsLastPiPWindowVisible(surfaceId, lastVisibilityState);
    ASSERT_EQ(ret, false);
}
}
} // namespace Rosen
} // namespace OHOS