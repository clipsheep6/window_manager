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
#include "iremote_object_mocker.h"
#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
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

class SceneSessionManagerTest5 : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static bool gestureNavigationEnabled_;

    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneSessionManagerTest5::ssm_ = nullptr;
bool SceneSessionManagerTest5::gestureNavigationEnabled_ = true;

ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest5::callbackFunc_ = [](bool enable) {
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

void SceneSessionManagerTest5::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest5::TearDownTestCase()
{
    ssm_->sceneSessionMap_.clear();
    ssm_ = nullptr;
}

void SceneSessionManagerTest5::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest5::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: NotifySessionTouchOutside
 * @tc.desc: SceneSesionManager notify session touch outside
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, NotifySessionTouchOutside01, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(ssm_, nullptr);
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::APP_WINDOW_BASE);
    ssm_->NotifySessionTouchOutside(0);
    property->SetPersistentId(1);
    ssm_->NotifySessionTouchOutside(1);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: check func DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, DestroyAndDisconnectSpecificSessionInner, Function | SmallTest | Level2)
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
HWTEST_F(SceneSessionManagerTest5, DestroyAndDetachCallback, Function | SmallTest | Level3)
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
 * @tc.name: GetStartupPageFromResource
 * @tc.desc: GetStartupPageFromResource
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, GetStartupPageFromResource, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::AbilityInfo info;
    info.startWindowBackgroundId = 1;
    std::string path = "path";
    uint32_t bgColor = 1;
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, path, bgColor));
    info.startWindowIconId = 0;
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, path, bgColor));
    info.hapPath = "hapPath";
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, path, bgColor));
}

/**
 * @tc.name: GetStartupPage
 * @tc.desc: GetStartupPage
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, GetStartupPage, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<AppExecFwk::IBundleMgr> bundleMgr_ = nullptr;
    std::string path = "path";
    uint32_t bgColor = 1;
    ssm_->GetStartupPage(info, path, bgColor);
}

/**
 * @tc.name: OnSCBSystemSessionBufferAvailable
 * @tc.desc: OnSCBSystemSessionBufferAvailable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, OnSCBSystemSessionBufferAvailable02, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    sceneSessionManager->OnSCBSystemSessionBufferAvailable(WindowType::WINDOW_TYPE_FLOAT);
    delete sceneSessionManager;
}

/**
 * @tc.name: CreateKeyboardPanelSession
 * @tc.desc: CreateKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, CreateKeyboardPanelSession, Function | SmallTest | Level3)
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
 * @tc.name: PrepareTerminate
 * @tc.desc: SceneSesionManager prepare terminate
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, PrepareTerminate, Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    bool isPrepareTerminate = true;
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSessionManager->PrepareTerminate(persistentId, isPrepareTerminate));
    delete sceneSessionManager;
}

/**
 * @tc.name: IsKeyboardForeground
 * @tc.desc: IsKeyboardForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, IsKeyboardForeground, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSessionManager->IsKeyboardForeground();
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    sceneSessionManager->IsKeyboardForeground();
    delete sceneSessionManager;
}

/**
 * @tc.name: RequestInputMethodCloseKeyboard
 * @tc.desc: RequestInputMethodCloseKeyboard
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestInputMethodCloseKeyboard02, Function | SmallTest | Level3)
{
    int32_t persistentId = -1;
    bool isPrepareTerminate = true;
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    sceneSessionManager->PrepareTerminate(persistentId, isPrepareTerminate);
    delete sceneSessionManager;
}

/**
 * @tc.name: UpdatePropertyRaiseEnabled
 * @tc.desc: UpdatePropertyRaiseEnabled
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdatePropertyRaiseEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(property, nullptr);
    auto result = ssm_->UpdatePropertyRaiseEnabled(property, sceneSession);
    ssm_->UpdatePropertyDragEnabled(property, sceneSession);
    ASSERT_EQ(result, WMError::WM_ERROR_NOT_SYSTEM_APP);
}

/**
 * @tc.name: HandleSpecificSystemBarProperty
 * @tc.desc: HandleSpecificSystemBarProperty
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, HandleSpecificSystemBarProperty, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    ssm_->HandleSpecificSystemBarProperty(type, property, sceneSession);
}

/**
 * @tc.name: UpdateBrightness
 * @tc.desc: UpdateBrightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdateBrightness, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.isSystem_ = false;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(property, nullptr);
    ASSERT_EQ(WSError::WS_DO_NOTHING, ssm_->UpdateBrightness(1));
    FocusChangeInfo focusInfo;
    ssm_->GetCurrentUserId();
    ssm_->GetFocusWindowInfo(focusInfo);
}

/**
 * @tc.name: RegisterSessionSnapshotFunc
 * @tc.desc: RegisterSessionSnapshotFunc
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RegisterSessionSnapshotFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RegisterSessionSnapshotFunc(scensession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->RegisterSessionSnapshotFunc(scensession);
    info.isSystem_ = false;
    ssm_->RegisterSessionSnapshotFunc(scensession);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo;
    ssm_->RegisterSessionSnapshotFunc(scensession);
}

/**
 * @tc.name: RequestAllAppSessionUnfocus
 * @tc.desc: RequestAllAppSessionUnfocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestAllAppSessionUnfocus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->HandleHideNonSystemFloatingWindows(property, scensession);
    ssm_->RequestAllAppSessionUnfocus();
}

/**
 * @tc.name: RequestSessionFocus
 * @tc.desc: RequestSessionFocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestSessionFocus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetFocusable(false);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    FocusChangeReason reason = FocusChangeReason::DEFAULT;
    ssm_->RequestSessionFocus(0, true, reason);
    ssm_->RequestSessionFocus(100, true, reason);
}

/**
 * @tc.name: SetShiftFocusListener
 * @tc.desc: SetShiftFocusListener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, SetShiftFocusListener, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    FocusChangeReason reason = FocusChangeReason::SPLIT_SCREEN;
    sptr<SceneSession> scensession = nullptr;
    ssm_->ShiftFocus(scensession, reason);
    info.isSystem_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ProcessShiftFocusFunc fun;
    NotifySCBAfterUpdateFocusFunc func;
    ssm_->SetShiftFocusListener(fun);
    ssm_->SetSCBFocusedListener(func);
    ssm_->SetSCBUnfocusedListener(func);
    ProcessCallingSessionIdChangeFunc func1;
    ssm_->SetCallingSessionIdSessionListenser(func1);
    ProcessStartUIAbilityErrorFunc func2;
    ssm_->SetStartUIAbilityErrorListener(func2);
    ssm_->ShiftFocus(sceneSession, reason);
}

/**
 * @tc.name: UpdateFocusStatus
 * @tc.desc: UpdateFocusStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdateFocusStatus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    ssm_->UpdateFocusStatus(scensession, false);
    ssm_->UpdateFocusStatus(scensession, true);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->UpdateFocusStatus(sceneSession, true);
    ssm_->UpdateFocusStatus(sceneSession, false);
}
}
} // namespace Rosen
} // namespace OHOS
