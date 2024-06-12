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

#include "session_manager_lite.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include <system_ability_definition.h>

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
 * @tc.name: SaveSessionListener
 * @tc.desc:  SaveSessionListener
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, SaveSessionListener, Function | SmallTest | Level2)
{
    std::shared_ptr<SessionManagerLite> sessionManagerLite =
        std::make_shared<SessionManagerLite>();
    ASSERT_NE(nullptr, sessionManagerLite);
    sessionManagerLite->ClearSessionManagerProxy();
    sessionManagerLite->destroyed_ = true;
    sessionManagerLite->ClearSessionManagerProxy();

    sessionManagerLite->GetScreenSessionManagerLiteProxy();
    sptr<ISessionManagerService> sessionManagerService = nullptr;
    sessionManagerLite->RecoverSessionManagerService(sessionManagerService);

    sessionManagerLite->ReregisterSessionListener();
    sessionManagerLite->OnWMSConnectionChanged(0, 0, true, sessionManagerService);
    sessionManagerLite->currentWMSUserId_ = BASE_USER_RANGE;

    sessionManagerLite->OnUserSwitch(sessionManagerService);
}
/**
 * @tc.name: InitScreenSessionManagerLiteProxy
 * @tc.desc:  InitScreenSessionManagerLiteProxy
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitScreenSessionManagerLiteProxy, Function | SmallTest | Level2)
{
    std::shared_ptr<SessionManagerLite> sessionManagerLite =
        std::make_shared<SessionManagerLite>();
    ASSERT_NE(nullptr, sessionManagerLite);

    sptr<IRemoteObject> remoteObject = sessionManagerLite->mockSessionManagerServiceProxy_->GetScreenSessionManagerLite();
    sessionManagerLite->screenSessionManagerLiteProxy_ = iface_cast<IScreenSessionManagerLite>(remoteObject);
    sessionManagerLite->InitScreenSessionManagerLiteProxy();
    sessionManagerLite->Clear();
}
}
}
}