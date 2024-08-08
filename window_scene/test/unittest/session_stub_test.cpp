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
#include <ipc_types.h>
#include <pointer_event.h>
#include "iremote_object_mocker.h"
#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_stub.h"
#include "ability_start_setting.h"
#include "parcel/accessibility_event_info_parcel.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "want.h"
#include "ws_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string UNDEFINED = "undefined";
}

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
class SessionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStub> session_ = nullptr;
};

void SessionStubTest::SetUpTestCase()
{
}

void SessionStubTest::TearDownTestCase()
{
}

void SessionStubTest::SetUp()
{
    session_ = new (std::nothrow) SessionStubMocker();
    EXPECT_NE(nullptr, session_);
}

void SessionStubTest::TearDown()
{
    session_ = nullptr;
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: sessionStub OnRemoteRequest01
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    uint32_t code = 1;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    auto res = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(0, res);
    data.WriteInterfaceToken(u"OHOS.ISession");
    res = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest01
 * @tc.desc: sessionStub ProcessRemoteRequestTest01
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    data.WriteBool(true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT), data, reply, option);
    ASSERT_EQ(0, res);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT), data, reply, option);
    ASSERT_EQ(5, res);
    ASSERT_EQ(data.WriteUint32(1), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT), data, reply, option);
    ASSERT_EQ(0, res);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP), data, reply, option);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest02
 * @tc.desc: sessionStub ProcessRemoteRequestTest02
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    data.WriteBool(true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED), data, reply, option);
    ASSERT_EQ(5, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED), data, reply, option);
    ASSERT_EQ(5, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA), data, reply, option);
    ASSERT_EQ(22, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED), data, reply, option);
    ASSERT_EQ(0, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT), data, reply, option);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: sessionStubTest02
 * @tc.desc: sessionStub sessionStubTest02
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, sessionStubTest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->HandleRaiseAboveTarget(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleRaiseAppMainWindowToTop(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleBackPressed(data, reply);
    if (!data.ReadBool()) {
        ASSERT_EQ(5, res);
    }
    res = session_->HandleMarkProcessed(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleSetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleGetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleNeedAvoid(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleGetAvoidAreaByType(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleSetAspectRatio(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleUpdateWindowSceneAfterCustomAnimation(data, reply);
    ASSERT_EQ(0, res);
    session_->HandleTransferAbilityResult(data, reply);
    res = session_->HandleTransferExtensionData(data, reply);
    ASSERT_EQ(22, res);
    res = session_->HandleNotifyExtensionDied(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleNotifyExtensionTimeout(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleGetStatusBarHeight(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleTriggerBindModalUIExtension001
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleTriggerBindModalUIExtension001, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleTriggerBindModalUIExtension(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleTransferAccessibilityEvent002
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleTransferAccessibilityEvent003, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleTransferAccessibilityEvent(data, reply);
    ASSERT_EQ(5, res);
}

/**
 * @tc.name: HandleNotifyPiPWindowPrepareClose003
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifyPiPWindowPrepareClose003, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleNotifyPiPWindowPrepareClose(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleUpdatePiPRect004
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdatePiPRect004, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleUpdatePiPRect(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleUpdatePiPControlStatus
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdatePiPControlStatus, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    data.WriteUint32(static_cast<uint32_t>(controlType));
    data.WriteInt32(static_cast<int32_t>(status));
    auto res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleProcessPointDownSession006
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleProcessPointDownSession006, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleProcessPointDownSession(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleSendPointerEvenForMoveDrag007
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleSendPointerEvenForMoveDrag007, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleSendPointerEvenForMoveDrag(data, reply);
    ASSERT_EQ(-1, res);
}

/**
 * @tc.name: HandleUpdateRectChangeListenerRegistered008
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdateRectChangeListenerRegistered008, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleUpdateRectChangeListenerRegistered(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleNotifySyncOn012
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifySyncOn012, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleNotifySyncOn(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleNotifyAsyncOn013
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifyAsyncOn013, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleNotifyAsyncOn(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleNotifyExtensionTimeout014
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifyExtensionTimeout014, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleNotifyExtensionTimeout(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleGetAppForceLandscapeConfig
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleGetAppForceLandscapeConfig, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_NE(session_, nullptr);
    auto res = session_->HandleGetAppForceLandscapeConfig(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleSetDialogSessionBackGestureEnabled01
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleSetDialogSessionBackGestureEnabled01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleSetDialogSessionBackGestureEnabled(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: ProcessRemoteRequest001
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest001, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND);
    data.WriteBool(false);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SHOW);
    data.WriteBool(false);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_HIDE);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DRAWING_COMPLETED);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_RECTCHANGE_LISTENER_REGISTERED);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequest002
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest002, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED);
    data.WriteBool(true);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED);
    int32_t eventId = 0;
    data.WriteInt32(eventId);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE);
    data.WriteUint32(1);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequest003
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest003, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO);
    data.WriteFloat(0.0f);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG);
    data.WriteBool(true);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION);
    data.WriteBool(true);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_LANDSCAPE_MULTI_WINDOW);
    data.WriteBool(true);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_ABOVE_TARGET);
    data.WriteInt32(1);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequest004
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest004, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_PROCESS_POINT_DOWN_SESSION);
    data.WriteInt32(0);
    data.WriteInt32(0);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SEND_POINTEREVENT_FOR_MOVE_DRAG);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(-1, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_KEYBOARD_SESSION_GRAVITY);
    data.WriteUint32(1);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequest005
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest005, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CALLING_SESSION_ID);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteUint32(1);
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CUSTOM_DECOR_HEIGHT);
    data.WriteInt32(0);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);
    data.WriteBool(false);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ADJUST_KEYBOARD_LAYOUT);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT);
    data.WriteUint32(1);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
}

/**
 * @tc.name: ProcessRemoteRequest006
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest006, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT);
    data.WriteInt32(0);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRIGGER_BIND_MODAL_UI_EXTENSION);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequest007
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequest007, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_CONTROL_STATUS);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(nullptr, session_);
    auto res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_LAYOUT_FULL_SCREEN_CHANGE);
    data.WriteBool(false);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_FORCE_LANDSCAPE_MODE);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    std::string name = "test";
    data.WriteString(name);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_STATUSBAR_HEIGHT);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_DIALOG_SESSION_BACKGESTURE_ENABLE);
    data.WriteBool(false);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = static_cast<uint32_t>(803);
    res = session_->ProcessRemoteRequest(code, data, reply, option);
    ASSERT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, res);
    // code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_ACTIVE_STATUS);
    // data.WriteBool(false);
    // res = session_->ProcessRemoteRequest(code, data, reply, option);
    // ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleForeground014
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleForeground014, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    data.WriteStrongParcelable(property);
    auto res = session_->HandleForeground(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    data.WriteBool(true);
    property = nullptr;
    data.WriteStrongParcelable(property);
    res = session_->HandleForeground(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    sptr<KeyboardLayoutParams> keyboardLayoutParams = sptr<KeyboardLayoutParams>::MakeSptr();
    data.WriteStrongParcelable(keyboardLayoutParams);
    res = session_->HandleAdjustKeyboardLayout(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}
}
} // namespace Rosen
} // namespace OHOS
