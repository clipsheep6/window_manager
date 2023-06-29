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
#include "iconsumer_surface.h"
#include "session_manager/include/scene_session_manager.h"
#include <surface.h>
#include "zidl/window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

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
};

void sceneSessionManagerProxyTest::SetUpTestCase()
{
}

void sceneSessionManagerProxyTest::TearDownTestCase()
{
}

void sceneSessionManagerProxyTest::SetUp()
{
}

void sceneSessionManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RegisterWindowManagerAgent01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RegisterWindowManagerAgent01, Function | SmallTest | Level2)
{
    sptr<ISceneSessionManager> sceneSessionManagerProxy_ = new SceneSessionManagerProxy();
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, sceneSessionManagerProxy_->RegisterWindowManagerAgent(type, nullptr));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(type, nullptr));

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent));

    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent));
    sceneSessionManagerProxy_ = nullptr;
}

}
}
}
