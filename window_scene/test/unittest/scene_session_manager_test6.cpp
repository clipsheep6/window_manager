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

#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include <gtest/gtest.h>
#include <regex>
#include "context.h"
#include "interfaces/include/ws_common.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "session_info.h"
#include "session_manager.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string EMPTY_DEVICE_ID = "";
    using ConfigItem = WindowSceneConfig::ConfigItem;
}
class SceneSessionManagerTest6 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest6::ssm_ = nullptr;

bool SceneSessionManagerTest6::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest6::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest6::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest6::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest6::SetUp()
{
}

void SceneSessionManagerTest6::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: MissionChanged
 * @tc.desc: MissionChanged
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, MissionChanged, Function | SmallTest | Level3)
{
    sptr<SceneSession> prevSession = nullptr;
    sptr<SceneSession> currSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(false, ret);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    prevSession = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(nullptr, prevSession);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(true, ret);
    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    currSession= sptr<SceneSession>::MakeSptr(sessionInfoSecond, nullptr);
    ASSERT_NE(nullptr, currSession);
    prevSession->persistentId_ = 0;
    currSession->persistentId_ = 0;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(false, ret);
    prevSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetWindowLayerChangeInfo
 * @tc.desc: GetWindowLayerChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetWindowLayerChangeInfo, Function | SmallTest | Level3)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    currVisibleData.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    currVisibleData.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION));
    currVisibleData.push_back(std::make_pair(3, WindowVisibilityState::WINDOW_LAYER_STATE_MAX));
    std::vector<std::pair<uint64_t, bool>> currDrawingContentData;
    currDrawingContentData.push_back(std::make_pair(0, true));
    currDrawingContentData.push_back(std::make_pair(1, false));
    VisibleData visibleData;
    visibleData.push_back(std::make_pair(0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
    visibleData.push_back(std::make_pair(1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE));
    visibleData.push_back(std::make_pair(2, WINDOW_LAYER_INFO_TYPE::INVISIBLE));
    visibleData.push_back(std::make_pair(3, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
    visibleData.push_back(std::make_pair(4, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
    visibleData.push_back(std::make_pair(5, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_UNKNOWN_TYPE));
    std::shared_ptr<RSOcclusionData> occlusionDataPtr = std::make_shared<RSOcclusionData>(visibleData);
    ASSERT_NE(nullptr, occlusionDataPtr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetWindowLayerChangeInfo(occlusionDataPtr, currVisibleData, currDrawingContentData);
}

/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdateWindowMode, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->UpdateWindowMode(0, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->UpdateWindowMode(2, 0);
    EXPECT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: SetScreenLocked && IsScreenLocked
 * @tc.desc: SceneSesionManager update screen locked state
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, IsScreenLocked, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetScreenLocked(true);
    ASSERT_NE(nullptr, ssm_);
    EXPECT_TRUE(ssm_->IsScreenLocked());
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessWindowModeType();
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetScreenLocked(false);
    ASSERT_NE(nullptr, ssm_);
    EXPECT_FALSE(ssm_->IsScreenLocked());
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessWindowModeType();
}

/**
 * @tc.name: CheckWindowModeType
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_OTHER, ret);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_OTHER, ret);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->isVisible_ = false;
    sceneSession->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_OTHER, ret);
}

/**
 * @tc.name: CheckWindowModeType01
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "privacy.test.first";
    sessionInfo1.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession1->property_);
    sceneSession1->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, sceneSession1->property_);
    sceneSession1->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession1->isVisible_ = true;
    sceneSession1->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_SPLIT_FLOATING, ret);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_FULLSCREEN_FLOATING, ret);
}

/**
 * @tc.name: CheckWindowModeType02
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    ssm_->lastWindowModeType_ = WindowModeType::WINDOW_MODE_FULLSCREEN;
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_FLOATING, ret);
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyRSSWindowModeTypeUpdate();
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_FULLSCREEN, ret);
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyRSSWindowModeTypeUpdate();
}

/**
 * @tc.name: CheckWindowModeType03
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType03, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_SPLIT, ret);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_SPLIT, ret);
}

/**
 * @tc.name: GetSceneSessionPrivacyModeBundles
 * @tc.desc: GetSceneSessionPrivacyModeBundles
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, GetSceneSessionPrivacyModeBundles, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    DisplayId displayId = 0;
    std::unordered_set<std::string> privacyBundles;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sceneSessionFirst->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSessionFirst->property_);
    sceneSessionFirst->property_->SetDisplayId(0);
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sceneSessionFirst->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sceneSessionFirst->state_ = SessionState::STATE_CONNECT;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
}

/**
 * @tc.name: GetSceneSessionPrivacyModeBundles01
 * @tc.desc: GetSceneSessionPrivacyModeBundles
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, GetSceneSessionPrivacyModeBundles01, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    std::unordered_set<std::string> privacyBundles;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSessionFirst->property_);
    sceneSessionFirst->property_->SetDisplayId(0);
    sceneSessionFirst->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionSecond = sptr<SceneSession>::MakeSptr(sessionInfoSecond, nullptr);
    ASSERT_NE(nullptr, sceneSessionSecond);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});
    ASSERT_NE(nullptr, sceneSessionSecond->property_);
    sceneSessionSecond->property_->displayId_ = 1;
    sceneSessionSecond->state_ = SessionState::STATE_ACTIVE;
    sceneSessionSecond->parentSession_ = sceneSessionFirst;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sceneSessionSecond->state_ = SessionState::STATE_FOREGROUND;
    sceneSessionSecond->state_ = SessionState::STATE_CONNECT;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
}

/**
 * @tc.name: GetSceneSessionPrivacyModeBundles02
 * @tc.desc: GetSceneSessionPrivacyModeBundles
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, GetSceneSessionPrivacyModeBundles02, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    std::unordered_set<std::string> privacyBundles;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSessionFirst->property_);
    sceneSessionFirst->property_->SetDisplayId(0);
    sceneSessionFirst->state_ = SessionState::STATE_ACTIVE;
    sceneSessionFirst->property_->isPrivacyMode_ = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: RegisterWindowManagerAgent
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR;
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: OnSessionStateChange
 * @tc.desc: OnSessionStateChange
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, OnSessionStateChange, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->OnSessionStateChange(1, state);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
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
HWTEST_F(SceneSessionManagerTest6, OnSessionStateChange01, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_BACKGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
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
HWTEST_F(SceneSessionManagerTest6, OnSessionStateChange02, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
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

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SetWindowFlags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, SetWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    sceneSession->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->SetWindowFlags(sceneSession, property);
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, ret);
}
}
} // namespace Rosen
} // namespace OHOS