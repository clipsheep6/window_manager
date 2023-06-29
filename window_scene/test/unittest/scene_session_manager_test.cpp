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
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"

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
    
    static sptr<IWindowManager> ssm_;
};

void SceneSessionManagerTest::SetUpTestCase()
{
    ssm_ = SessionManager::GetInstance().GetSceneSessionManagerProxy();
}

void SceneSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest::SetUp()
{
}

void SceneSessionManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetBrightness
 * @tc.desc: ScreenSesionManager set session brightness
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetBrightness, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result = SceneSessionManager::GetInstance().SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SceneSesionManager rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ssm_->RegisterWindowManagerAgent(type, nullptr));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ssm_->UnregisterWindowManagerAgent(type, nullptr));

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent));

    ASSERT_EQ(WMError::WM_OK, ssm_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent));
}

}
} // namespace Rosen
} // namespace OHOS

