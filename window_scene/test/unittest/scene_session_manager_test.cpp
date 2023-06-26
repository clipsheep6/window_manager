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

#include "session_manager/include/scene_session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "iconsumer_surface.h"
#include <surface.h>

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

    static sptr<SceneSessionManager> ssm_;

    SceneId DEFAULT_Scene_ID {0};
    SceneId VIRTUAL_Scene_ID {2};
};

sptr<SceneSessionManager> SceneSessionManagerTest::ssm_ = nullptr;

void SceneSessionManagerTest::SetUpTestCase()
{
    ssm_ = new SceneSessionManager();
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
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SceneSesionManager rigister display manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->RegisterWindowManagerAgent(nullptr, type));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->UnregisterWindowManagerAgent(nullptr, type));

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->UnregisterWindowManagerAgent(windowManagerAgent, type));

    ASSERT_EQ(DMError::DM_OK, ssm_->RegisterWindowManagerAgent(windowManagerAgent, type));
    ASSERT_EQ(DMError::DM_OK, ssm_->UnregisterWindowManagerAgent(windowManagerAgent, type));
}

}
} // namespace Rosen
} // namespace OHOS
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
}
} // namespace Rosen
} // namespace OHOS

