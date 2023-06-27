/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
class sceneSessionManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<sceneSessionManagerProxy> sceneSessionManagerProxy_;
};

void sceneSessionManagerProxyTest::SetUpTestCase()
{
}

void sceneSessionManagerProxyTest::TearDownTestCase()
{
}

void sceneSessionManagerProxyTest::SetUp()
{
    sceneSessionManagerProxy_ = SessionManager::GetInstance().GetSceneSessionManagerProxy();
}

void sceneSessionManagerProxyTest::TearDown()
{
    sceneSessionManagerProxy_ = nullptr;
}

namespace {
/**
 * @tc.name: RegisterWindowManagerAgent01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RegisterWindowManagerAgent01, Function | SmallTest | Level2)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new IWindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, sceneSessionManagerProxy_->RegisterWindowManagerAgent(nullptr, type));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(nullptr, type));

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(windowManagerAgent, type));

    ASSERT_EQ(DMError::DM_OK, sceneSessionManagerProxy_->RegisterWindowManagerAgent(windowManagerAgent, type));
    ASSERT_EQ(DMError::DM_OK, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(windowManagerAgent, type));
}

}
}
}
