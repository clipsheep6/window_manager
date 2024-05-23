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
#include "session_manager.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <ipc_skeleton.h>

#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"
#include "session_manager_lite.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionManagerLiteTest::SetUpTestCase()
{
}

void SessionManagerLiteTest::TearDownTestCase()
{
}

void SessionManagerLiteTest::SetUp()
{
}

void SessionManagerLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ClearSessionManagerProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, ClearSessionManagerProxy, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.GetSessionManagerServiceProxy();
    
    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: RecoverSessionManagerService
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RecoverSessionManagerService, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.RecoverSessionManagerService(nullptr);
    sessionManagerLite.ClearSessionManagerProxy();
    sptr<ISessionManagerService> sessionManagerService = sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.RecoverSessionManagerService(sessionManagerService);

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: ReregisterSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, ReregisterSessionListener, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.ReregisterSessionListener();
    sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.ReregisterSessionListener();

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: OnWMSConnectionChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChanged01, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sptr<ISessionManagerService> sessionManagerService = sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.OnWMSConnectionChanged(100, 100, true, sessionManagerService);

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: OnWMSConnectionChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChanged02, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sptr<ISessionManagerService> sessionManagerService = sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.OnWMSConnectionChanged(100, 100, false, sessionManagerService);

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: OnWMSConnectionChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChanged03, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sptr<ISessionManagerService> sessionManagerService = sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.OnWMSConnectionChanged(-1, -1, true, sessionManagerService);

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: OnWMSConnectionChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChanged04, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sptr<ISessionManagerService> sessionManagerService = sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.OnWMSConnectionChanged(-1, -1, false, sessionManagerService);

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: InitSceneSessionManagerLiteProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitSceneSessionManagerLiteProxy01, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.InitSceneSessionManagerLiteProxy();
    sessionManagerLite.InitSceneSessionManagerLiteProxy();

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: InitSceneSessionManagerLiteProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitSceneSessionManagerLiteProxy02, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.GetSceneSessionManagerLiteProxy();
    sessionManagerLite.InitSceneSessionManagerLiteProxy();

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: InitSceneSessionManagerLiteProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitSceneSessionManagerLiteProxy03, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.GetSessionManagerServiceProxy();
    sessionManagerLite.InitSceneSessionManagerLiteProxy();
    sessionManagerLite.InitSceneSessionManagerLiteProxy();

    sessionManagerLite.ClearSessionManagerProxy();
    sessionManagerLite.Clear();
    int ret = 0;
    ASSERT_EQ(0, ret);
}
/**
 * @tc.name: Clear
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, Clear01, Function | SmallTest | Level2)
{
    SessionManagerLite& sessionManagerLite = SessionManagerLite::GetInstance();
    sessionManagerLite.Clear();

    sessionManagerLite.GetSceneSessionManagerLiteProxy();
    sessionManagerLite.InitSceneSessionManagerLiteProxy();
    sessionManagerLite.Clear();
    sessionManagerLite.ClearSessionManagerProxy();

    sessionManagerLite.GetSceneSessionManagerLiteProxy();
    sessionManagerLite.Clear();
    sessionManagerLite.ClearSessionManagerProxy();

    sessionManagerLite.InitSceneSessionManagerLiteProxy();
    sessionManagerLite.Clear();
    sessionManagerLite.ClearSessionManagerProxy();
    
    int ret = 0;
    ASSERT_EQ(0, ret);
}

}
}
}