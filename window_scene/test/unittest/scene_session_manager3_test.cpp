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
namespace {
    const std::string EMPTY_DEVICE_ID = "";
}
class SceneSessionManager3Test : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    int32_t GetTaskCount(sptr<SceneSession>& session);
    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneSessionManager3Test::ssm_ = nullptr;

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManager3Test::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManager3Test::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManager3Test::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManager3Test::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManager3Test::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

int32_t SceneSessionManager3Test::GetTaskCount(sptr<SceneSession>& session)
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
 * @tc.name: DumpSessionInfo
 * @tc.desc: SceneSesionManager dump session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, DumpSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    std::ostringstream oss;
    std::string dumpInfo;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    info.isSystem_ = false;
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());

    sptr<SceneSession::SpecificSessionCallback> specific = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, specific);
    scensession = new (std::nothrow) SceneSession(info, specific);
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, specific);
    ssm_->DumpAllAppSessionInfo(oss, ssm_->sceneSessionMap_);
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: SceneSesionManager dump session element info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, DumpSessionElementInfo, Function | SmallTest | Level3)
{
    DumpRootSceneElementInfoFunc func_ = DumpRootSceneElementInfoFuncTest;
    ssm_->SetDumpRootSceneElementInfoListener(func_);
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    info.isSystem_ = false;
    std::string strId = "10086";
    sptr<SceneSession> scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    std::vector<std::string> params_(5, "");
    std::string dumpInfo_;
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    WSError result01 = ssm_->GetSpecifiedSessionDumpInfo(dumpInfo_, params_, strId);
    EXPECT_NE(result01, WSError::WS_OK);
}

/**
 * @tc.name: NotifyDumpInfoResult
 * @tc.desc: SceneSesionManager notify dump info result
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, NotifyDumpInfoResult, Function | SmallTest | Level3)
{
    std::vector<std::string> info = {"std::", "vector", "<std::string>"};
    ssm_->NotifyDumpInfoResult(info);
    std::vector<std::string> params = {"-a"};
    std::string dumpInfo = "";
    WSError result01 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result01, WSError::WS_OK);
    params.clear();
    params.push_back("-w");
    params.push_back("23456");
    WSError result02 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_NE(result02, WSError::WS_OK);
    params.clear();
    WSError result03 = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_NE(result03, WSError::WS_OK);
}

/**
 * @tc.name: UpdateFocus
 * @tc.desc: SceneSesionManager update focus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateFocus, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    SessionInfo info;
    info.bundleName_ = "bundleName_";
    bool isFocused = true;
    WSError result = ssm_->UpdateFocus(persistentId, isFocused);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: SceneSesionManager update window mode
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateWindowMode, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    int32_t windowMode = 3;
    WSError result = ssm_->UpdateWindowMode(persistentId, windowMode);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_WINDOW);
    WindowChangedFunc func = [](int32_t persistentId, WindowUpdateType type) {
        OHOS::Rosen::WindowChangedFuncTest(persistentId, type);
    };
    ssm_->RegisterWindowChanged(func);
}

/**
 * @tc.name: SetScreenLocked && IsScreenLocked
 * @tc.desc: SceneSesionManager update screen locked state
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, IsScreenLocked, Function | SmallTest | Level3)
{
    ssm_->SetScreenLocked(true);
    EXPECT_TRUE(ssm_->IsScreenLocked());
    ssm_->SetScreenLocked(false);
    EXPECT_FALSE(ssm_->IsScreenLocked());
}

/**
 * @tc.name: UpdatePrivateStateAndNotify
 * @tc.desc: SceneSesionManager update private state and notify
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdatePrivateStateAndNotify, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(scensession);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(scensession, nullptr);
    ssm_->RegisterSessionStateChangeNotifyManagerFunc(scensession);
    ssm_->UpdatePrivateStateAndNotify(persistentId);
    auto displayId = scensession->GetSessionProperty()->GetDisplayId();
    std::vector<string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: GerPrivacyBundleListOneWindow
 * @tc.desc: get privacy bundle list when one window exist only.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, GerPrivacyBundleListOneWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->GetSessionProperty()->displayId_ = 0;
    sceneSession->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<std::string> privacyBundleList;
    sceneSession->GetSessionProperty()->isPrivacyMode_ = false;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);

    sceneSession->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSession->state_ = SessionState::STATE_BACKGROUND;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);

    sceneSession->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);
    EXPECT_EQ(privacyBundleList.at(0), sessionInfo.bundleName_);

    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(1, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 0);
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SceneSesionManager set window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, SetWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    property->SetWindowFlags(flags);
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->SetWindowFlags(scensession, property);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->SetWindowFlags(scensession, property);
    EXPECT_EQ(result02, WSError::WS_ERROR_NOT_SYSTEM_APP);
    property->SetSystemCalling(true);
    WSError result03 = ssm_->SetWindowFlags(scensession, property);
    ASSERT_EQ(result03, WSError::WS_OK);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: SceneSesionManager notify water mark flag changed result
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, NotifyWaterMarkFlagChangedResult, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    ssm_->NotifyCompleteFirstFrameDrawing(persistentId);
    bool hasWaterMark = true;
    AppExecFwk::AbilityInfo abilityInfo;
    WSError result01 = ssm_->NotifyWaterMarkFlagChangedResult(hasWaterMark);
    EXPECT_EQ(result01, WSError::WS_OK);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    ssm_->ProcessPreload(abilityInfo);
}

/**
 * @tc.name: IsValidSessionIds
 * @tc.desc: SceneSesionManager is valid session id
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, IsValidSessionIds, Function | SmallTest | Level3)
{
    std::vector<int32_t> sessionIds = {0, 1, 2, 3, 4, 5, 24, 10086};
    std::vector<bool> results = {};
    WSError result = ssm_->IsValidSessionIds(sessionIds, results);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UnRegisterSessionListener
 * @tc.desc: SceneSesionManager un register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UnRegisterSessionListener, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError result = ssm_->UnRegisterSessionListener(listener);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: GetSessionInfos
 * @tc.desc: SceneSesionManager get session infos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, GetSessionInfos, Function | SmallTest | Level3)
{
    std::string deviceId = "1245";
    int32_t numMax = 1024;
    AAFwk::MissionInfo infoFrist;
    infoFrist.label = "fristBundleName";
    AAFwk::MissionInfo infoSecond;
    infoSecond.label = "secondBundleName";
    std::vector<SessionInfoBean> sessionInfos = {infoFrist, infoSecond};
    WSError result = ssm_->GetSessionInfos(deviceId, numMax, sessionInfos);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    int32_t persistentId = 24;
    SessionInfoBean sessionInfo;
    int result01 = ssm_->GetRemoteSessionInfo(deviceId, persistentId, sessionInfo);
    ASSERT_NE(result01, ERR_OK);
}

/**
 * @tc.name: CheckIsRemote
 * @tc.desc: SceneSesionManager check is remote
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, CheckIsRemote, Function | SmallTest | Level3)
{
    std::string deviceId;
    bool result = ssm_->CheckIsRemote(deviceId);
    EXPECT_FALSE(result);
    deviceId.assign("deviceId");
    result = ssm_->CheckIsRemote(deviceId);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AnonymizeDeviceId
 * @tc.desc: SceneSesionManager anonymize deviceId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, AnonymizeDeviceId, Function | SmallTest | Level3)
{
    std::string deviceId;
    std::string result(ssm_->AnonymizeDeviceId(deviceId));
    EXPECT_EQ(result, EMPTY_DEVICE_ID);
    deviceId.assign("100964857");
    std::string result01 = "100964******";
    ASSERT_EQ(ssm_->AnonymizeDeviceId(deviceId), result01);
}

/**
 * @tc.name: RegisterSessionListener01
 * @tc.desc: SceneSesionManager register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, RegisterSessionListener01, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError result = ssm_->RegisterSessionListener(listener);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: RegisterSessionListener02
 * @tc.desc: SceneSesionManager register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, RegisterSessionListener02, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionChangeListener> sessionListener = nullptr;
    WSError result01 = ssm_->RegisterSessionListener(sessionListener);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_SESSION_LISTENER);
    ssm_->UnregisterSessionListener();
}

/**
 * @tc.name: RequestSceneSessionByCall
 * @tc.desc: SceneSesionManager request scene session by call
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, RequestSceneSessionByCall, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->RequestSceneSessionByCall(nullptr);
    EXPECT_EQ(result01, WSError::WS_OK);
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->RequestSceneSessionByCall(scensession);
    ASSERT_EQ(result02, WSError::WS_OK);
}

/**
 * @tc.name: StartAbilityBySpecified
 * @tc.desc: SceneSesionManager start ability by specified
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, StartAbilityBySpecified, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    ssm_->StartAbilityBySpecified(info);

    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    AAFwk::WantParams wantParams;
    want->SetParams(wantParams);
    info.want = want;
    ssm_->StartAbilityBySpecified(info);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: FindMainWindowWithToken
 * @tc.desc: SceneSesionManager find main window with token
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, FindMainWindowWithToken, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> targetToken = nullptr;
    sptr<SceneSession> result = ssm_->FindMainWindowWithToken(targetToken);
    EXPECT_EQ(result, nullptr);
    uint64_t persistentId = 1423;
    WSError result01 = ssm_->BindDialogSessionTarget(persistentId, targetToken);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: UpdateParentSessionForDialog001
 * @tc.desc: SceneSesionManager update parent session for dialog
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateParentSessionForDialog001, Function | SmallTest | Level3)
{
    SessionInfo dialogInfo;
    dialogInfo.abilityName_ = "DialogWindows";
    dialogInfo.bundleName_ = "DialogWindows";
    SessionInfo parentInfo;
    parentInfo.abilityName_ = "ParentWindows";
    parentInfo.bundleName_ = "ParentWindows";

    int32_t persistentId = 1005;
    sptr<SceneSession> parentSession = new (std::nothrow) MainSession(parentInfo, nullptr);
    EXPECT_NE(parentSession, nullptr);
    ssm_->sceneSessionMap_.insert({ persistentId, parentSession });

    sptr<SceneSession> dialogSession = new (std::nothrow) SystemSession(dialogInfo, nullptr);
    EXPECT_NE(dialogSession, nullptr);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetParentPersistentId(persistentId);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);

    WSError result = ssm_->UpdateParentSessionForDialog(dialogSession, property);
    EXPECT_EQ(dialogSession->GetParentPersistentId(), persistentId);
    EXPECT_NE(dialogSession->GetParentSession(), nullptr);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: MoveSessionsToBackground
 * @tc.desc: SceneSesionManager move sessions to background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, MoveSessionsToBackground, Function | SmallTest | Level3)
{
    int32_t type = CollaboratorType::RESERVE_TYPE;
    WSError result01 = ssm_->UnregisterIAbilityManagerCollaborator(type);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PERMISSION);
    std::vector<std::int32_t> sessionIds = {1, 2, 3, 15, 1423};
    std::vector<int32_t> res = {1, 2, 3, 15, 1423};
    WSError result03 = ssm_->MoveSessionsToBackground(sessionIds, res);
    ASSERT_EQ(result03, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: MoveSessionsToForeground
 * @tc.desc: SceneSesionManager move sessions to foreground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, MoveSessionsToForeground, Function | SmallTest | Level3)
{
    std::vector<std::int32_t> sessionIds = {1, 2, 3, 15, 1423};
    int32_t topSessionId = 1;
    WSError result = ssm_->MoveSessionsToForeground(sessionIds, topSessionId);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: UnlockSession
 * @tc.desc: SceneSesionManager unlock session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UnlockSession, Function | SmallTest | Level3)
{
    int32_t sessionId = 1;
    WSError result = ssm_->UnlockSession(sessionId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    result = ssm_->LockSession(sessionId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: UpdateImmersiveState
 * @tc.desc: test UpdateImmersiveState
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateImmersiveState, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->UpdateImmersiveState();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyAINavigationBarShowStatus
 * @tc.desc: test NotifyAINavigationBarShowStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, NotifyAINavigationBarShowStatus, Function | SmallTest | Level3)
{
    bool isVisible = false;
    WSRect barArea = { 0, 0, 320, 240}; // width: 320, height: 240
    uint64_t displayId = 0;
    WSError result = ssm_->NotifyAINavigationBarShowStatus(isVisible, barArea, displayId);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyWindowExtensionVisibilityChange
 * @tc.desc: test NotifyWindowExtensionVisibilityChange
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, NotifyWindowExtensionVisibilityChange, Function | SmallTest | Level3)
{
    int32_t pid = 1;
    int32_t uid = 32;
    bool isVisible = false;
    WSError result = ssm_->NotifyWindowExtensionVisibilityChange(pid, uid, isVisible);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateTopmostProperty
 * @tc.desc: test UpdateTopmostProperty
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateTopmostProperty, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateTopmostProperty";
    info.bundleName_ = "UpdateTopmostProperty";
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetTopmost(true);
    property->SetSystemCalling(true);
    sptr<SceneSession> scenesession = new (std::nothrow) MainSession(info, nullptr);
    scenesession->SetSessionProperty(property);
    WMError result = ssm_->UpdateTopmostProperty(property, scenesession);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: SceneSesionManager NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, NotifySessionForeground, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, scensession);
    uint32_t reason = 1;
    bool withAnimation = true;
    scensession->NotifySessionForeground(reason, withAnimation);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: SceneSesionManager NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, NotifySessionBackground, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, scensession);
    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    scensession->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

/**
 * @tc.name: UpdateSessionWindowVisibilityListener
 * @tc.desc: SceneSesionManager update window visibility listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateSessionWindowVisibilityListener, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    bool haveListener = true;
    WSError result = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: GetSessionSnapshotPixelMap
 * @tc.desc: SceneSesionManager get session snapshot pixelmap
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, GetSessionSnapshotPixelMap, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);

    int32_t persistentId = 65535;
    float scaleValue = 0.5f;
    auto pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);

    persistentId = 1;
    pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);
}

/**
 * @tc.name: CalculateCombinedExtWindowFlags
 * @tc.desc: SceneSesionManager calculate combined extension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, CalculateCombinedExtWindowFlags, Function | SmallTest | Level3)
{
    EXPECT_EQ(ssm_->combinedExtWindowFlags_.bitData, 0);
    ssm_->UpdateSpecialExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    ssm_->UpdateSpecialExtWindowFlags(5678, ExtensionWindowFlags(4), ExtensionWindowFlags(4));
    ssm_->CalculateCombinedExtWindowFlags();
    EXPECT_EQ(ssm_->combinedExtWindowFlags_.bitData, 7);
    ssm_->extWindowFlagsMap_.clear();
}

/**
 * @tc.name: UpdateSpecialExtWindowFlags
 * @tc.desc: SceneSesionManager update special extension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateSpecialExtWindowFlags, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->UpdateSpecialExtWindowFlags(persistentId, 3, 3);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.size(), 1);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->first, persistentId);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->second.bitData, 3);
    ssm_->UpdateSpecialExtWindowFlags(persistentId, 0, 3);
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->extWindowFlagsMap_.clear();
}

/**
 * @tc.name: HideNonSecureFloatingWindows
 * @tc.desc: SceneSesionManager hide non-secure floating windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, HideNonSecureFloatingWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HideNonSecureFloatingWindows";
    info.bundleName_ = "HideNonSecureFloatingWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));

    sptr<SceneSession> floatSession;
    floatSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(floatSession, nullptr);
    floatSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ssm_->nonSystemFloatSceneSessionMap_.insert(std::make_pair(floatSession->GetPersistentId(), floatSession));

    EXPECT_FALSE(ssm_->shouldHideNonSecureFloatingWindows_.load());
    EXPECT_FALSE(floatSession->GetSessionProperty()->GetForceHide());
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;
    ssm_->HideNonSecureFloatingWindows();
    EXPECT_TRUE(floatSession->GetSessionProperty()->GetForceHide());
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = false;
    ssm_->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;
    ssm_->HideNonSecureFloatingWindows();
    EXPECT_TRUE(floatSession->GetSessionProperty()->GetForceHide());
    ssm_->shouldHideNonSecureFloatingWindows_.store(false);
    ssm_->sceneSessionMap_.clear();
    ssm_->nonSystemFloatSceneSessionMap_.clear();
}

/**
 * @tc.name: HideNonSecureSubWindows
 * @tc.desc: SceneSesionManager hide non-secure sub windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, HideNonSecureSubWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HideNonSecureSubWindows";
    info.bundleName_ = "HideNonSecureSubWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;

    sptr<SceneSession> subSession;
    subSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(subSession, nullptr);
    sceneSession->AddSubSession(subSession);

    EXPECT_FALSE(subSession->GetSessionProperty()->GetForceHide());
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;
    ssm_->HideNonSecureSubWindows(sceneSession);
    EXPECT_TRUE(subSession->GetSessionProperty()->GetForceHide());
}

/**
 * @tc.name: HandleSecureSessionShouldHide
 * @tc.desc: SceneSesionManager handle secure session should hide
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, HandleSecureSessionShouldHide, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HandleSecureSessionShouldHide";
    info.bundleName_ = "HandleSecureSessionShouldHide";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));

    sptr<SceneSession> subSession;
    subSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(subSession, nullptr);
    sceneSession->AddSubSession(subSession);

    sptr<SceneSession> floatSession;
    floatSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(floatSession, nullptr);
    floatSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ssm_->nonSystemFloatSceneSessionMap_.insert(std::make_pair(floatSession->GetPersistentId(), floatSession));

    sceneSession->SetShouldHideNonSecureWindows(true);
    auto ret = ssm_->HandleSecureSessionShouldHide(sceneSession);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(subSession->GetSessionProperty()->GetForceHide());
    EXPECT_TRUE(floatSession->GetSessionProperty()->GetForceHide());
    EXPECT_TRUE(ssm_->shouldHideNonSecureFloatingWindows_.load());
    ssm_->sceneSessionMap_.clear();
    ssm_->nonSystemFloatSceneSessionMap_.clear();
}

/**
 * @tc.name: HandleSpecialExtWindowFlagsChange
 * @tc.desc: SceneSesionManager handle special uiextension window flags change
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, HandleSpecialExtWindowFlagsChange, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->HandleSpecialExtWindowFlagsChange(persistentId, 3, 3);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.size(), 1);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->first, persistentId);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->second.bitData, 3);
    ssm_->HandleSpecialExtWindowFlagsChange(persistentId, 0, 3);
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->extWindowFlagsMap_.clear();
}

/**
 * @tc.name: ClearUnrecoveredSessions
 * @tc.desc: test func ClearUnrecoveredSessions
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManager3Test, ClearUnrecoveredSessions, Function | SmallTest | Level1)
{
    ssm_->alivePersistentIds_.push_back(23);
    ssm_->alivePersistentIds_.push_back(24);
    ssm_->alivePersistentIds_.push_back(25);
    std::vector<int32_t> recoveredPersistentIds;
    recoveredPersistentIds.push_back(23);
    recoveredPersistentIds.push_back(24);
    ssm_->ClearUnrecoveredSessions(recoveredPersistentIds);
}

/**
 * @tc.name: RecoverSessionInfo
 * @tc.desc: test func RecoverSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManager3Test, RecoverSessionInfo, Function | SmallTest | Level1)
{
    SessionInfo info = ssm_->RecoverSessionInfo(nullptr);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    info = ssm_->RecoverSessionInfo(property);
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: SceneSesionManager hide non-secure windows by scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, AddOrRemoveSecureSession, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "AddOrRemoveSecureSession";
    info.bundleName_ = "AddOrRemoveSecureSession1";

    int32_t persistentId = 12345;
    auto ret = ssm_->AddOrRemoveSecureSession(persistentId, true);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: SceneSesionManager update uiextension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, UpdateExtWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateExtWindowFlags";
    info.bundleName_ = "UpdateExtWindowFlags";

    int32_t parentId = 1234;
    int32_t persistentId = 12345;
    auto ret = ssm_->UpdateExtWindowFlags(parentId, persistentId, 7, 7);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: SetScreenLocked001
 * @tc.desc: SetScreenLocked001
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, SetScreenLocked001, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetEventHandler(ssm_->taskScheduler_->GetEventHandler(), ssm_->eventHandler_);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::ATTACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_UNDEFINED;
    sceneSession->SetDetectTaskInfo(detectTaskInfo);
    std::string taskName = "wms:WindowStateDetect" + std::to_string(sceneSession->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    sceneSession->handler_->PostTask(task, taskName, delayTime);
    int32_t beforeTaskNum = GetTaskCount(sceneSession);
    ssm_->SetScreenLocked(true);
    ASSERT_EQ(beforeTaskNum - 1, GetTaskCount(sceneSession));
    ASSERT_EQ(DetectTaskState::NO_TASK, sceneSession->detectTaskInfo_.taskState);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, sceneSession->detectTaskInfo_.taskWindowMode);
}

/**
 * @tc.name: AccessibilityFillEmptySceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill empty scene session list to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, AccessibilityFillEmptySceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    EXPECT_EQ(accessibilityInfo.size(), 0);
}

/**
 * @tc.name: AccessibilityFillOneSceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill one sceneSession to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, AccessibilityFillOneSceneSessionListToNotifyList, Function | SmallTest | Level3)
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
}

/**
 * @tc.name: AccessibilityFilterEmptySceneSessionList
 * @tc.desc: SceneSesionManager filter empty scene session list;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, AccessibilityFilterEmptySceneSessionList, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;

    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 0);
}

/**
 * @tc.name: AccessibilityFilterOneWindow
 * @tc.desc: SceneSesionManager filter one window;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, AccessibilityFilterOneWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetSessionRect({100, 100, 200, 200});
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: GetMainWindowInfos
 * @tc.desc: SceneSesionManager get topN main window infos;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, GetMainWindowInfos, Function | SmallTest | Level3)
{
    int32_t topNum = 1024;
    std::vector<MainWindowInfo> topNInfos;
    auto result = ssm_->GetMainWindowInfos(topNum, topNInfos);
    EXPECT_EQ(result, WMError::WM_OK);

    topNum = 0;
    result = ssm_->GetMainWindowInfos(topNum, topNInfos);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_PARAM);

    topNum = 1000;
    MainWindowInfo info;
    topNInfos.push_back(info);
    result = ssm_->GetMainWindowInfos(topNum, topNInfos);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: TestNotifyEnterRecentTask
 * @tc.desc: Test whether the enterRecent_ is set correctly;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, TestNotifyEnterRecentTask, Function | SmallTest | Level3)
{
    GTEST_LOG_(INFO) << "SceneSessionManager3Test: TestNotifyEnterRecentTask start";
    sptr<SceneSessionManager> sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(nullptr, sceneSessionManager);
    
    ASSERT_EQ(sceneSessionManager->NotifyEnterRecentTask(true), WSError::WS_OK);
    ASSERT_EQ(sceneSessionManager->enterRecent_.load(), true);
}

/**
 * @tc.name: TestIsEnablePiPCreate
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManager3Test, TestIsEnablePiPCreate, Function | SmallTest | Level3)
{
    GTEST_LOG_(INFO) << "SceneSessionManager3Test: TestIsEnablePiPCreate start";
    ssm_->isScreenLocked_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    ssm_->isScreenLocked_ = false;
    Rect reqRect = { 0, 0, 0, 0 };
    property->SetRequestRect(reqRect);
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    reqRect = { 0, 0, 10, 0 };
    property->SetRequestRect(reqRect);
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    reqRect = { 0, 0, 10, 10 };
    property->SetRequestRect(reqRect);
    PiPTemplateInfo info = {0, 0, {}};
    property->SetPiPTemplateInfo(info);
    SessionInfo info1;
    info1.abilityName_ = "test1";
    info1.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sceneSession->pipTemplateInfo_ = {0, 100, {}};
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));
    ssm_->sceneSessionMap_.clear();
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    property->SetParentPersistentId(100);
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    ssm_->sceneSessionMap_.clear();
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ASSERT_TRUE(ssm_->isEnablePiPCreate(property));
}
}
} // namespace Rosen
} // namespace OHOS
