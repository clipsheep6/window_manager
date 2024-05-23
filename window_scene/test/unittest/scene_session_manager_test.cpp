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
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneSessionManagerTest::ssm_ = nullptr;

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

namespace {
/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: check func RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    auto result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, nullptr, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    property->SetPersistentId(1);
    result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: GerPrivacyBundleListTwoWindow
 * @tc.desc: get privacy bundle list when two windows exist.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GerPrivacyBundleListTwoWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfoSecond, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    sceneSessionFirst->GetSessionProperty()->displayId_ = 0;
    sceneSessionFirst->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSessionFirst->state_ = SessionState::STATE_FOREGROUND;

    sceneSessionSecond->GetSessionProperty()->displayId_ = 0;
    sceneSessionSecond->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSessionSecond->state_ = SessionState::STATE_FOREGROUND;

    std::vector<std::string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 2);

    sceneSessionSecond->GetSessionProperty()->displayId_ = 1;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);

    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(1, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);
}

/**
 * @tc.name: TerminateSessionNew
 * @tc.desc: SceneSesionManager terminate session new
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TerminateSessionNew, Function | SmallTest | Level3)
{
    sptr<AAFwk::SessionInfo> info = nullptr;
    bool needStartCaller = true;
    WSError result01 = ssm_->TerminateSessionNew(info, needStartCaller);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, result01);
    info = new (std::nothrow) AAFwk::SessionInfo();
    WSError result02 = ssm_->TerminateSessionNew(info, needStartCaller);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, result02);
}

/**
 * @tc.name: AccessibilityFillTwoSceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill two sceneSessions to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillTwoSceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    SetVisibleForAccessibility(sceneSessionFirst);

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    SetVisibleForAccessibility(sceneSessionSecond);

    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 2);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 2);
}

/**
 * @tc.name: AccessibilityFillEmptyBundleName
 * @tc.desc: SceneSesionManager fill empty bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptyBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, "");
    ASSERT_EQ(sceneSessionList.at(0)->GetSessionInfo().bundleName_, "");
    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, sceneSessionList.at(0)->GetSessionInfo().bundleName_);
}

/**
 * @tc.name: AccessibilityFillBundleName
 * @tc.desc: SceneSesionManager fill bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, "accessibilityNotifyTesterBundleName");
    ASSERT_EQ(sceneSessionList.at(0)->GetSessionInfo().bundleName_, "accessibilityNotifyTesterBundleName");
    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, sceneSessionList.at(0)->GetSessionInfo().bundleName_);
}

/**
 * @tc.name: AccessibilityFillFilterBundleName
 * @tc.desc: SceneSesionManager fill filter bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillFilterBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SCBGestureTopBar";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 0);
}

/**
 * @tc.name: AccessibilityFillEmptyHotAreas
 * @tc.desc: SceneSesionManager fill empty hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptyHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 0);
}

/**
 * @tc.name: AccessibilityFillOneHotAreas
 * @tc.desc: SceneSesionManager fill one hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillOneHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    Rect rect = {100, 200, 100, 200};
    std::vector<Rect> hotAreas;
    hotAreas.push_back(rect);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetTouchHotAreas(hotAreas);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 1);

    ASSERT_EQ(rect.posX_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).posX_);
    ASSERT_EQ(rect.posY_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).posY_);
    ASSERT_EQ(rect.width_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).width_);
    ASSERT_EQ(rect.height_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).height_);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posX_, rect.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posY_, rect.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).width_, rect.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).height_, rect.height_);
}

/**
 * @tc.name: AccessibilityFillTwoHotAreas
 * @tc.desc: SceneSesionManager fill two hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillTwoHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    std::vector<Rect> hotAreas;
    Rect rectFitst = {100, 200, 100, 200};
    Rect rectSecond = {50, 50, 20, 30};
    hotAreas.push_back(rectFitst);
    hotAreas.push_back(rectSecond);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetTouchHotAreas(hotAreas);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 2);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posX_, rectFitst.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posY_, rectFitst.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).width_, rectFitst.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).height_, rectFitst.height_);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).posX_, rectSecond.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).posY_, rectSecond.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).width_, rectSecond.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).height_, rectSecond.height_);
}

/**
 * @tc.name: AccessibilityFilterTwoWindowNotCovered
 * @tc.desc: SceneSesionManager filter two windows that not covered each other;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterTwoWindowNotCovered, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->SetSessionRect({0, 0, 200, 200});
    SetVisibleForAccessibility(sceneSessionFirst);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    sceneSessionSecond->SetSessionRect({300, 300, 200, 200});
    SetVisibleForAccessibility(sceneSessionSecond);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 2);
}

/**
 * @tc.name: AccessibilityFilterTwoWindowCovered
 * @tc.desc: SceneSesionManager filter two windows that covered each other;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterTwoWindowCovered, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->SetSessionRect({0, 0, 200, 200});
    SetVisibleForAccessibility(sceneSessionFirst);
    sceneSessionFirst->SetZOrder(20);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    sceneSessionSecond->SetSessionRect({50, 50, 50, 50});
    SetVisibleForAccessibility(sceneSessionSecond);
    sceneSessionSecond->SetZOrder(10);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}
}
} // namespace Rosen
} // namespace OHOS
