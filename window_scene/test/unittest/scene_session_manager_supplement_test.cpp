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
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "screen_session_manager/include/screem_session_manager_client.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock_mock_window_event_channel.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionManagerSupplementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManager> ssm_;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerSupplementTest::ssm_ = nullptr;

void SceneSessionManagerSupplementTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance()；
}

void SceneSessionManagerSupplementTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerSupplementTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerSupplementTest::TearDown()
{
    ssm_->sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: LoadFreeMultiWindowConfig
 * @tc.desc: SceneSessionManagerSupplementTest LoadFreeMultiWindowConfig
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, LoadFreeMultiWindowConfig, Function | SmallTest | Level3)
{
    ssm_->LoadFreeMultiWindowConfig(true);
    ssm_->LoadFreeMultiWindowConfig(false);
    auto config = ssm_->GetSystemSessionConfig();
    config.freeMultiWindowSupport_ = false;
    ssm_->SwitchFreeMultiWindow(true);
    config.freeMultiWindowSupport_ = true;
    ssm_->SwitchFreeMultiWindow(true);
    sptr<SceneSession> sceneSession;
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->SwitchFreeMultiWindow(true);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accseeibilityNotifyTesterAbilityName";
    sessionInfo.abilityName_ = "accseeibilityNotifyTesterAbilityName";
    sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ssm_->SwitchFreeMultiWindow(true);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    sceneSession->SetSessionProperty(property);
    ssm_->SwitchFreeMultiWindow(true);
    ssm_->SwitchFreeMultiWindow(false);
    ssm_->SetEnableInputEvent(true);
    ASSERT_EQ(ssm_->IsInputEventEnabled(), true);
}
}
} //namespace Rosen
} //namespace OHOS