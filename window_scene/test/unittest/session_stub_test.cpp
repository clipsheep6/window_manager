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
#include "session/host/include/zidl/session_stub.h"
#include "session/host/include/zidl/session_host_ipc_interface_code.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager_agent.h"
#include "display_manager.h"
#include "zidl/scene_session_manager_stub.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SessionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SessionStub> stub_;
};

void SessionStubTest::SetUpTestCase()
{
}

void SessionStubTest::TearDownTestCase()
{
}

void SessionStubTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    stub_ = new (std::nothrow) SceneSession(info, nullptr);
}

void SessionStubTest::TearDown()
{
    stub_ = nullptr;
}

namespace {
/**
 * @tc.name: HandleSetWindowAnimationFlag
 * @tc.desc: test HandleSetWindowAnimationFlag
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSetWindowAnimationFlag, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    int res = stub_->HandleSetWindowAnimationFlag(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleForeground
 * @tc.desc: test HandleForeground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleForeground01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);
    sptr<WindowSessionProperty> windowSessionProperty = new WindowSessionProperty();
    data.WriteStrongParcelable(windowSessionProperty);
    int res = stub_->HandleForeground(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleForeground
 * @tc.desc: test HandleForeground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleForeground02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    int res = stub_->HandleForeground(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleBackground
 * @tc.desc: test HandleBackground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleBackground, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int res = stub_->HandleForeground(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleDisconnect
 * @tc.desc: test HandleDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleDisconnect, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int res = stub_->HandleDisconnect(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSessionEvent
 * @tc.desc: test HandleSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSessionEvent, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    uint32_t x = 2;
    data.WriteUint32(x);
    int res = stub_->HandleSessionEvent(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateActivateStatus
 * @tc.desc: test HandleUpdateActivateStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleUpdateActivateStatus, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    int res = stub_->HandleUpdateActivateStatus(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateSessionRect
 * @tc.desc: test HandleUpdateSessionRect
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleUpdateSessionRect, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t posX = 10;
    int32_t posY = 10;
    uint32_t width = 1000;
    uint32_t height = 1000;
    uint32_t reason = 0;
    data.WriteInt32(posX);
    data.WriteInt32(posY);
    data.WriteUint32(width);
    data.WriteUint32(height);
    data.WriteUint32(reason);
    int res = stub_->HandleUpdateSessionRect(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleDestroyAndDisconnectSpecificSession
 * @tc.desc: test HandleDestroyAndDisconnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleDestroyAndDisconnectSpecificSession, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    uint32_t id = 1;
    data.WriteUint32(id);
    int res = stub_->HandleDestroyAndDisconnectSpecificSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleRaiseToAppTop
 * @tc.desc: test HandleRaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleRaiseToAppTop, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleRaiseToAppTop(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleRaiseAboveTarget
 * @tc.desc: test HandleRaiseAboveTarget
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleRaiseAboveTarget, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t id = 1;
    data.WriteInt32(id);
    int res = stub_->HandleRaiseAboveTarget(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleBackPressed
 * @tc.desc: test HandleBackPressed
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleBackPressed, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    int res = stub_->HandleBackPressed(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleMarkProcessed
 * @tc.desc: test HandleMarkProcessed
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleMarkProcessed, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleMarkProcessed(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleSetGlobalMaximizeMode
 * @tc.desc: test HandleSetGlobalMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSetGlobalMaximizeMode, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    uint32_t x = 1;
    data.WriteUint32(x);
    int res = stub_->HandleSetGlobalMaximizeMode(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetGlobalMaximizeMode
 * @tc.desc: test HandleGetGlobalMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleGetGlobalMaximizeMode, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleGetGlobalMaximizeMode(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleNeedAvoid
 * @tc.desc: test HandleNeedAvoid
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleNeedAvoid, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    uint32_t x = 1;
    data.WriteUint32(x);
    int res = stub_->HandleNeedAvoid(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetAvoidAreaByType
 * @tc.desc: test HandleGetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleGetAvoidAreaByType, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    uint32_t x = 1;
    data.WriteUint32(x);
    int res = stub_->HandleGetAvoidAreaByType(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateWindowSessionProperty
 * @tc.desc: test HandleUpdateWindowSessionProperty
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleUpdateWindowSessionProperty, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    data.WriteStrongParcelable(property);
    int res = stub_->HandleUpdateWindowSessionProperty(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSetAspectRatio
 * @tc.desc: test HandleSetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSetAspectRatio, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    float ratio = 0.5;
    data.WriteFloat(ratio);
    int res = stub_->HandleSetAspectRatio(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateWindowSceneAfterCustomAnimation
 * @tc.desc: test HandleUpdateWindowSceneAfterCustomAnimation
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleUpdateWindowSceneAfterCustomAnimation, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    int res = stub_->HandleUpdateWindowSceneAfterCustomAnimation(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleTransferAbilityResult
 * @tc.desc: test HandleTransferAbilityResult
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleTransferAbilityResult, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    uint32_t x = 1;
    data.WriteUint32(x);
    int res = stub_->HandleTransferAbilityResult(data, reply);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleTransferExtensionData
 * @tc.desc: test HandleTransferExtensionData
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleTransferExtensionData, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleTransferExtensionData(data, reply);
    EXPECT_EQ(res, ERR_INVALID_VALUE);
}
}
}
}