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
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
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
    usleep(WAIT_SYNC_IN_NS);
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
}
} // namespace Rosen
} // namespace OHOS