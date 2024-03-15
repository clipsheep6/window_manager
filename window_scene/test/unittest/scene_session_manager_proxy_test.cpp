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
#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_proxy.h"
#include "window_manager_agent.h"
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
    sptr<IRemoteObject> iRemoteObjectMocker;
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_;
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
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: UpdateSessionWindowVisibilityListener001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateSessionWindowVisibilityListener001, Function | SmallTest | Level2)
{
    int32_t persistendId = 0;
    bool haveListener = true;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->UpdateSessionWindowVisibilityListener(persistendId,
        haveListener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, AddOrRemoveSecureSession001, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    int32_t persistentId = 12345;
    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->AddOrRemoveSecureSession(persistentId, true));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: AddOrRemoveSecureExtSession001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, AddOrRemoveSecureExtSession001, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    int32_t persistentId = 12345;
    int32_t parentId = 1234;
    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->AddOrRemoveSecureExtSession(persistentId, parentId,
        true));
    sceneSessionManagerProxy_ = nullptr;
}

}
}
}
