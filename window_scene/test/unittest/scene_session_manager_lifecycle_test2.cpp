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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "iremote_object_mocker.h"
#include "interfaces/include/ws_common.h"
#include "screen_fold_data.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "application_info.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string EMPTY_DEVICE_ID = "";
}
class SceneSessionManagerLifecycleTest2 : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    int32_t GetTaskCount(sptr<SceneSession>& session);
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerLifecycleTest2::ssm_ = nullptr;

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerLifecycleTest2::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerLifecycleTest2::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerLifecycleTest2::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLifecycleTest2::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLifecycleTest2::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

int32_t SceneSessionManagerLifecycleTest2::GetTaskCount(sptr<SceneSession>& session)
{
    std::string dumpInfo = session->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
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
 * @tc.name: RequestSceneSession03
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSession03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = true;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});

    sptr<SceneSession> getSceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(getSceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSession04
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSession04, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = true;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSessionBackground01
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionBackground01, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = false;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground02
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionBackground02, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground03
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionBackground03, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground04
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionBackground04, Function | SmallTest | Level3)
{
    bool isDelegator = true;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionDestruction01
 * @tc.desc: SceneSesionManager test RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionDestruction01, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction";
    info.bundleName_ = "RequestSceneSessionDestruction";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(ssm_->RequestSceneSessionDestruction(
        sceneSession, needRemoveSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionDestruction02
 * @tc.desc: SceneSesionManager test RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionDestruction02, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction";
    info.bundleName_ = "RequestSceneSessionDestruction";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionDestruction(
        sceneSession, needRemoveSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall01
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionByCall01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall02
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionByCall02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    info.callState_ = static_cast<uint32_t>(AAFwk::CallToState::BACKGROUND);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall03
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, RequestSceneSessionByCall03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    info.callState_ = static_cast<uint32_t>(AAFwk::CallToState::FOREGROUND);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSession01
 * @tc.desc: SceneSesionManager test DestroyAndDisconnectSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, DestroyAndDisconnectSpecificSession01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "DestroyAndDisconnectSpecificSession";
    info.bundleName_ = "DestroyAndDisconnectSpecificSession";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_NE(ssm_->DestroyAndDisconnectSpecificSession(1), WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: removeFailRecoveredSession
 * @tc.desc: removeFailRecoveredSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, removeFailRecoveredSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession02));
    ssm_->failRecoveredPersistentIdSet_.insert(1);
    ssm_->failRecoveredPersistentIdSet_.insert(2);
    ssm_->failRecoveredPersistentIdSet_.insert(3);
    ssm_->removeFailRecoveredSession();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: ClearSession
 * @tc.desc: ClearSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, ClearSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->ClearSession(nullptr));
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->ClearSession(sceneSession));
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: check func DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, DestroyAndDisconnectSpecificSessionInner, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    ProcessShiftFocusFunc shiftFocusFunc_;
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    property->SetPersistentId(1);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);

    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: SceneSesionManager destroy and disconnect specific session with detach callback
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest2, DestroyAndDetachCallback, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    ASSERT_NE(ssm_, nullptr);
    sptr<IRemoteObject> callback = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(callback, nullptr);
    ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
    sptr<WindowSessionProperty> property;
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
}

/**
 * @tc.name: CreateKeyboardPanelSession
 * @tc.desc: CreateKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, CreateKeyboardPanelSession, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSessionManager->CreateKeyboardPanelSession(sceneSession);
    delete sceneSessionManager;
}

/**
 * @tc.name: OnSessionStateChange
 * @tc.desc: OnSessionStateChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, OnSessionStateChange, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->OnSessionStateChange(1, state);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerLifecycleTest22";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    ssm_->focusedSessionId_ = 1;
    ssm_->OnSessionStateChange(1, state);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(nullptr, ssm_);
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;
    ssm_->OnSessionStateChange(1, state);
    ASSERT_NE(nullptr, ssm_);
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->OnSessionStateChange(1, state);
    ssm_->focusedSessionId_ = 0;
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
}

/**
 * @tc.name: OnSessionStateChange01
 * @tc.desc: OnSessionStateChange01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, OnSessionStateChange01, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_BACKGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerLifecycleTest22";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    state = SessionState::STATE_END;
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
}

/**
 * @tc.name: OnSessionStateChange02
 * @tc.desc: OnSessionStateChange02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, OnSessionStateChange02, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerLifecycleTest22";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    sceneSession->SetFocusedOnShow(true);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    sceneSession->SetFocusedOnShow(false);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
}
}
} // namespace Rosen
} // namespace OHOS
