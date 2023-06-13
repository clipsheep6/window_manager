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

#include "session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<Session> sessionProxy_;
    // static sptr<SessionProxy> sessionProxy_;
};

sptr<Session> SessionProxyTest::ssn_ = nullptr;

void SessionProxyTest::SetUpTestCase()
{

    // sptr<WindowOption> option = new WindowOption();
    // sptr<WindowSceneSessionImpl> windowsession = new WindowSceneSessionImpl(option);
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    session_ = new (std::nothrow) Session(info);
    sessionProxy_ = new SessionProxy(session_);
    // ssn_ = new Session();
}

void SessionProxyTest::TearDownTestCase()
{
    sessionProxy_ = nullptr;
}

void SessionProxyTest::SetUp()
{
}

void SessionProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateActiveStatus
 * @tc.desc: ScreenSesionManager rigister display manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateActiveStatus01, Function | SmallTest | Level3)
{
    // sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    // DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_STATE_LISTENER;

    // ASSERT_EQ(DMError::DM_ERROR_NULLPTR, sessionProxy_->RegisterDisplayManagerAgent(nullptr, type));
    // ASSERT_EQ(DMError::DM_ERROR_NULLPTR, sessionProxy_->UnregisterDisplayManagerAgent(nullptr, type));

    // ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssn_->UnregisterDisplayManagerAgent(displayManagerAgent, type));

    // ASSERT_EQ(DMError::DM_OK, ssn_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    // ASSERT_EQ(DMError::DM_OK, ssn_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}


/**
 * @tc.name: UpdateActiveStatus
 * @tc.desc: UpdateActiveStatus UpdateActiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateActiveStatus01, Function | SmallTest | Level2)
{
    bool isActive_ = false;
    WMError err = windowProxy_->UpdateActiveStatus(isActive_);
    ASSERT_EQ(err, WSError::WS_DO_NOTHING);

    isActive_=true;
    WMError err = windowProxy_->UpdateActiveStatus(isActive_);
    ASSERT_EQ(err, WSError::WS_OK);
}

/**
 * @tc.name: ScreenPower
 * @tc.desc: ScreenSesionManager screen power
 * @tc.type: FUNC
 */
// HWTEST_F(SessionProxyTest, ScreenPower, Function | SmallTest | Level3)
// {
//     PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
//     ScreenPowerState state = ScreenPowerState::POWER_ON;
//     DisplayState displayState = DisplayState::ON;

//     ASSERT_EQ(false, ssn_->WakeUpBegin(reason));

// }

}
} // namespace Rosen
} // namespace OHOS
