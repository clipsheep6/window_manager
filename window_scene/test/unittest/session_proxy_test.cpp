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

#include "session_proxy.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include "accessibility_event_info.h"
#include "ws_common.h"
#include "mock_message_parcel.h"
#include "pointer_event.h"
#include "ui/rs_canvas_node.h"
#include "transaction/rs_transaction.h"

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyTest : public testing::Test {
public:
    SessionProxyTest() {}
    ~SessionProxyTest() {}
};
namespace {

/**
 * @tc.name: OnSessionEvent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnSessionEvent, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnSessionEvent start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    SessionEvent event = SessionEvent::EVENT_MAXIMIZE;
    WSError res = sProxy->OnSessionEvent(event);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnSessionEvent end";
}

/**
 * @tc.name: UpdateSessionRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateSessionRect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateSessionRect start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    WSRect rect{ .posX_ = 1, .posY_ = 1, .width_ = 100, .height_ = 100 };
    SizeChangeReason reason = SizeChangeReason::RECOVER;
    WSError res = sProxy->UpdateSessionRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateSessionRect end";
}

/**
 * @tc.name: Restore
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnRestoreMainWindow, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnRestoreMainWindow start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->OnRestoreMainWindow();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnRestoreMainWindow end";
}

/**
 * @tc.name: RaiseToAppTop
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RaiseToAppTop, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseToAppTop start";
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(remoteMocker);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WSError res = sProxy->RaiseToAppTop();
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    sptr<SessionProxy> tempProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = tempProxy->RaiseToAppTop();
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);

    remoteMocker->SetRequestResult(ERR_INVALID_DATA);
    res = sProxy->RaiseToAppTop();
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    remoteMocker->SetRequestResult(ERR_NONE);

    res = sProxy->RaiseToAppTop();
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseToAppTop end";
}

/**
 * @tc.name: RaiseAboveTarget
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RaiseAboveTarget, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseAboveTarget start";
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SessionProxy> proxy = sptr<SessionProxy>::MakeSptr(remoteMocker);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    int32_t subWindowId = 0;
    WSError res = proxy->RaiseAboveTarget(subWindowId);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    sptr<SessionProxy> tempProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = tempProxy->RaiseAboveTarget(subWindowId);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);

    remoteMocker->SetRequestResult(ERR_INVALID_DATA);
    res = proxy->RaiseAboveTarget(subWindowId);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    remoteMocker->SetRequestResult(ERR_NONE);

    res = proxy->RaiseAboveTarget(subWindowId);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseAboveTarget end";
}

/**
 * @tc.name: OnNeedAvoid
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnNeedAvoid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnNeedAvoid start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    bool status = false;
    WSError res = sProxy->OnNeedAvoid(status);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: OnNeedAvoid end";
}

/**
 * @tc.name: RequestSessionBack
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RequestSessionBack, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RequestSessionBack start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    bool needMoveToBackground = true;
    WSError res = sProxy->RequestSessionBack(needMoveToBackground);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: RequestSessionBack end";
}

/**
 * @tc.name: MarkProcessed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, MarkProcessed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: MarkProcessed start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    int32_t eventId = 0;
    WSError res = sProxy->MarkProcessed(eventId);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: MarkProcessed end";
}

/**
 * @tc.name: SetGlobalMaximizeMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetGlobalMaximizeMode, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetGlobalMaximizeMode start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    MaximizeMode mode = MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    WSError res = sProxy->SetGlobalMaximizeMode(mode);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: SetGlobalMaximizeMode end";
}

/**
 * @tc.name: GetGlobalMaximizeMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetGlobalMaximizeMode, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetGlobalMaximizeMode start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    MaximizeMode mode = MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    WSError res = sProxy->GetGlobalMaximizeMode(mode);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: GetGlobalMaximizeMode end";
}

/**
 * @tc.name: SetAspectRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetAspectRatio, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetAspectRatio start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    float ratio = 10;
    WSError res = sProxy->SetAspectRatio(ratio);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: SetAspectRatio end";
}

/**
 * @tc.name: UpdateWindowSceneAfterCustomAnimation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateWindowSceneAfterCustomAnimation, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowSceneAfterCustomAnimation start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    bool isAdd = false;
    WSError res = sProxy->UpdateWindowSceneAfterCustomAnimation(isAdd);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowSceneAfterCustomAnimation end";
}

/**
 * @tc.name: SetSystemWindowEnableDrag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetSystemWindowEnableDrag, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetSystemWindowEnableDrag start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    bool enableDrag = false;
    WMError res = sProxy->SetSystemWindowEnableDrag(enableDrag);
    ASSERT_EQ(res, WMError::WM_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: SetSystemWindowEnableDrag end";
}

/**
 * @tc.name: TransferAbilityResult
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TransferAbilityResult, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAbilityResult start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    uint32_t resultCode = 0;
    AAFwk::Want want;
    WSError res = sProxy->TransferAbilityResult(resultCode, want);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAbilityResult end";
}

/**
 * @tc.name: NotifyExtensionDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyExtensionDied, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyExtensionDied start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sProxy->NotifyExtensionDied();

    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyExtensionDied end";
}

/**
 * @tc.name: NotifyExtensionTimeout
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyExtensionTimeout, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyExtensionTimeout start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sProxy->NotifyExtensionTimeout(2);

    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyExtensionTimeout end";
}

/**
 * @tc.name: UpdateWindowAnimationFlag
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateWindowAnimationFlag, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowAnimationFlag start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    bool needDefaultAnimationFlag = false;
    WSError res = sProxy->UpdateWindowAnimationFlag(needDefaultAnimationFlag);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateWindowAnimationFlag end";
}

/**
 * @tc.name: TransferAccessibilityEvent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TransferAccessibilityEvent, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAccessibilityEvent start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    Accessibility::AccessibilityEventInfo info;
    int64_t uiExtensionIdLevel = 0;
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_OK);

    GTEST_LOG_(INFO) << "SessionProxyTest: TransferAccessibilityEvent end";
}

/**
 * @tc.name: OnTitleAndDockHoverShowChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnTitleAndDockHoverShowChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnTitleAndDockHoverShowChange start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    bool isTitleHoverShown = true;
    bool isDockHoverShown = true;
    WSError res = sProxy->OnTitleAndDockHoverShowChange(isTitleHoverShown, isDockHoverShown);
    EXPECT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnTitleAndDockHoverShowChange end";
}

/**
 * @tc.name: UpdatePiPControlStatus
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdatePiPControlStatus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdatePiPControlStatus start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    WSError res = sProxy->UpdatePiPControlStatus(controlType, status);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdatePiPControlStatus end";
}

/**
 * @tc.name: SetAutoStartPiP
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionProxyTest, SetAutoStartPiP, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAutoStartPiP: SetAutoStartPiP start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    bool isAutoStartValid = true;
    uint32_t priority = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    ASSERT_EQ(WSError::WS_OK, sProxy->SetAutoStartPiP(isAutoStartValid, priority, width, height));
    GTEST_LOG_(INFO) << "SetAutoStartPiP: SetAutoStartPiP end";
}

/**
 * @tc.name: UpdatePiPTemplateInfo
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionProxyTest, UpdatePiPTemplateInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "UpdatePiPTemplateInfo: UpdatePiPTemplateInfo start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    PiPTemplateInfo templateInfo;
    ASSERT_EQ(WSError::WS_OK, sProxy->UpdatePiPTemplateInfo(templateInfo));
    GTEST_LOG_(INFO) << "UpdatePiPTemplateInfo: UpdatePiPTemplateInfo end";
}

/**
 * @tc.name: SetWindowTransitionAnimation
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionProxyTest, SetWindowTransitionAnimation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SetWindowTransitionAnimation: SetWindowTransitionAnimation start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    TransitionAnimation animation;
    ASSERT_EQ(WSError::WS_OK, sProxy->SetWindowTransitionAnimation(WindowTransitionType::DESTROY, animation));
    GTEST_LOG_(INFO) << "SetWindowTransitionAnimation: SetWindowTransitionAnimation end";
}

/**
 * @tc.name: GetGlobalScaledRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetGlobalScaledRect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetGlobalScaledRect start";
    Rect rect;
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(remoteMocker);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WMError res = sProxy->GetGlobalScaledRect(rect);
    ASSERT_EQ(res, WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    sptr<SessionProxy> tempProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = tempProxy->GetGlobalScaledRect(rect);
    ASSERT_EQ(res, WMError::WM_ERROR_IPC_FAILED);

    remoteMocker->SetRequestResult(ERR_INVALID_DATA);
    res = sProxy->GetGlobalScaledRect(rect);
    ASSERT_EQ(res, WMError::WM_ERROR_IPC_FAILED);
    remoteMocker->SetRequestResult(ERR_NONE);

    MockMessageParcel::SetReadInt32ErrorFlag(true);
    MockMessageParcel::SetReadUint32ErrorFlag(true);
    res = sProxy->GetGlobalScaledRect(rect);
    ASSERT_EQ(res, WMError::WM_ERROR_IPC_FAILED);

    MockMessageParcel::ClearAllErrorFlag();
    res = sProxy->GetGlobalScaledRect(rect);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: GetGlobalScaledRect end";
}

/**
 * @tc.name: GetStatusBarHeight
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetStatusBarHeight, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetStatusBarHeight start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    int32_t res = sProxy->GetStatusBarHeight();
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "SessionProxyTest: GetStatusBarHeight end";
}

/**
 * @tc.name: SetDialogSessionBackGestureEnabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetDialogSessionBackGestureEnabled, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetDialogSessionBackGestureEnabled start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    WSError res = sProxy->SetDialogSessionBackGestureEnabled(true);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: SetDialogSessionBackGestureEnabled end";
}

/**
 * @tc.name: GetAppForceLandscapeConfig
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetAppForceLandscapeConfig, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetAppForceLandscapeConfig start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    AppForceLandscapeConfig config = {};
    auto res = sProxy->GetAppForceLandscapeConfig(config);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: GetAppForceLandscapeConfig end";
}

/**
 * @tc.name: NotifyExtensionEventAsync
 * @tc.desc: NotifyExtensionEventAsync test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyExtensionEventAsync, TestSize.Level1)
{
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifyExtensionEventAsync(0);

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifyExtensionEventAsync(0);

    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    sProxy->NotifyExtensionEventAsync(0);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    sProxy->NotifyExtensionEventAsync(0);
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: RequestFocus Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RequestFocus, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RequestFocus start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    WSError res = sProxy->RequestFocus(true);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: RequestFocus end";
}

/**
 * @tc.name: UpdateClientRect01
 * @tc.desc: UpdateClientRect test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateClientRect01, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateClientRect01 start";
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    WSRect rect = { 200, 200, 200, 200 };
    ASSERT_EQ(sProxy->UpdateClientRect(rect), WSError::WS_ERROR_IPC_FAILED);

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_EQ(sProxy->UpdateClientRect(rect), WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateClientRect01 start";
}

/**
 * @tc.name: TransferExtensionData
 * @tc.desc: TransferExtensionData test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TransferExtensionData, TestSize.Level1)
{
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);
    AAFwk::WantParams wantParams;
    auto res = sProxy->TransferExtensionData(wantParams);
    ASSERT_EQ(res, IPC_PROXY_ERR);

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);

    res = sProxy->TransferExtensionData(wantParams);
    ASSERT_EQ(res, ERR_NONE);
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    res = sProxy->TransferExtensionData(wantParams);
    ASSERT_EQ(res, IPC_PROXY_ERR);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    res = sProxy->TransferExtensionData(wantParams);
    ASSERT_EQ(res, IPC_PROXY_ERR);
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: NotifyAsyncOn
 * @tc.desc: NotifyAsyncOn test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyAsyncOn, TestSize.Level1)
{
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifyAsyncOn();

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifyAsyncOn();

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    sProxy->NotifyAsyncOn();
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: OnSetWindowRectAutoSave
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnSetWindowRectAutoSave, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnSetWindowRectAutoSave start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->OnSetWindowRectAutoSave(true, false);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnSetWindowRectAutoSave end";
}

/**
 * @tc.name: NotifyMainModalTypeChange
 * @tc.desc: NotifyMainModalTypeChange test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyMainModalTypeChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyMainModalTypeChange start";
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_EQ(sProxy->NotifyMainModalTypeChange(true), WSError::WS_ERROR_IPC_FAILED);
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_EQ(sProxy->NotifyMainModalTypeChange(true), WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyMainModalTypeChange end";
}

/**
 * @tc.name: Foreground
 * @tc.desc: Foreground test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Foreground, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    bool isFromClient = true;
    std::string identityToken = "foregroundTest";
    WSError ret = sProxy->Foreground(property, isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground end";
}

/**
 * @tc.name: Foreground02
 * @tc.desc: Foreground test property is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Foreground02, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground02 start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    bool isFromClient = true;
    std::string identityToken = "foregroundTest";
    WSError ret = sProxy->Foreground(nullptr, isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground02 end";
}

/**
 * @tc.name: Foreground03
 * @tc.desc: Foreground test isFromClient is false
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Foreground03, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground03 start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    bool isFromClient = false;
    std::string identityToken = "foregroundTest";
    WSError ret = sProxy->Foreground(property, isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Foreground03 end";
}

/**
 * @tc.name: Background
 * @tc.desc: Background test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Background, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Background start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    bool isFromClient = true;
    std::string identityToken = "backgroundTest";
    WSError ret = sProxy->Background(isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Background end";
}

/**
 * @tc.name: Background02
 * @tc.desc: Background test isFromClient is false
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Background02, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Background02 start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    bool isFromClient = false;
    std::string identityToken = "backgroundTest";
    WSError ret = sProxy->Background(isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Background02 end";
}

/**
 * @tc.name: Show
 * @tc.desc: Show Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Show, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Show start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    WSError ret = sProxy->Show(property);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);

    // property is nullptr
    ret = sProxy->Show(nullptr);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Show end";
}

/**
 * @tc.name: Hide
 * @tc.desc: Hide Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Hide, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Hide start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    WSError ret = sProxy->Hide();
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Hide end";
}

/**
 * @tc.name: Disconnect
 * @tc.desc: Disconnect Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: Disconnect start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    bool isFromClient = true;
    std::string identityToken = "disconnectTest";
    WSError ret = sProxy->Disconnect(isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);

    // isFromClient is false
    isFromClient = false;
    ret = sProxy->Disconnect(isFromClient, identityToken);
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: Disconnect end";
}

/**
 * @tc.name: DrawingCompleted
 * @tc.desc: DrawingCompleted Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, DrawingCompleted, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: DrawingCompleted start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);

    WSError ret = sProxy->DrawingCompleted();
    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: DrawingCompleted end";
}

/**
 * @tc.name: NotifySupportWindowModesChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifySupportWindowModesChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifySupportWindowModesChange start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    std::vector<AppExecFwk::SupportWindowMode> supportedWindowModes = { AppExecFwk::SupportWindowMode::FULLSCREEN,
                                                                        AppExecFwk::SupportWindowMode::SPLIT,
                                                                        AppExecFwk::SupportWindowMode::FLOATING };
    WSError res = sProxy->NotifySupportWindowModesChange(supportedWindowModes);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifySupportWindowModesChange end";
}

/**
 * @tc.name: GetIsMidScene
 * @tc.desc: GetIsMidScene
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetIsMidScene, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetIsMidScene start";
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);

    bool isMidScene = false;
    WSError res = sProxy->GetIsMidScene(isMidScene);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    ASSERT_EQ(isMidScene, false);
    GTEST_LOG_(INFO) << "SessionProxyTest: GetIsMidScene end";
}

/**
 * @tc.name: ChangeSessionVisibilityWithStatusBar
 * @tc.desc: ChangeSessionVisibilityWithStatusBar test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, ChangeSessionVisibilityWithStatusBar, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: ChangeSessionVisibilityWithStatusBar start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool visible = true;
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    WSError res = sProxy->ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, visible);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, visible);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    abilitySessionInfo = nullptr;
    res = sProxy->ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, visible);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);
    GTEST_LOG_(INFO) << "SessionProxyTest: ChangeSessionVisibilityWithStatusBar end";
}

/**
 * @tc.name: SyncSessionEvent
 * @tc.desc: SyncSessionEvent test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SyncSessionEvent, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SyncSessionEvent start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    SessionEvent event = SessionEvent::EVENT_MAXIMIZE;
    WSError res = sProxy->SyncSessionEvent(event);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: SyncSessionEvent end";
}

/**
 * @tc.name: OnLayoutFullScreenChange
 * @tc.desc: OnLayoutFullScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnLayoutFullScreenChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnLayoutFullScreenChange start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool isLayoutFullScreen = true;
    WSError res = sProxy->OnLayoutFullScreenChange(isLayoutFullScreen);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->OnLayoutFullScreenChange(isLayoutFullScreen);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnLayoutFullScreenChange end";
}

/**
 * @tc.name: OnDefaultDensityEnabled
 * @tc.desc: OnDefaultDensityEnabled test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnDefaultDensityEnabled, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnDefaultDensityEnabled start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool isDefaultDensityEnabled = true;
    WSError res = sProxy->OnDefaultDensityEnabled(isDefaultDensityEnabled);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->OnDefaultDensityEnabled(isDefaultDensityEnabled);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnDefaultDensityEnabled end";
}

/**
 * @tc.name: NotifyFrameLayoutFinishFromApp
 * @tc.desc: NotifyFrameLayoutFinishFromApp test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyFrameLayoutFinishFromApp, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyFrameLayoutFinishFromApp start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool notifyListener = true;
    WSRect rect = { 200, 200, 200, 200 };
    WSError res = sProxy->NotifyFrameLayoutFinishFromApp(notifyListener, rect);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->NotifyFrameLayoutFinishFromApp(notifyListener, rect);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyFrameLayoutFinishFromApp end";
}

/**
 * @tc.name: RaiseAppMainWindowToTop
 * @tc.desc: RaiseAppMainWindowToTop test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, RaiseAppMainWindowToTop, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseAppMainWindowToTop start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    WSError res = sProxy->RaiseAppMainWindowToTop();
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->RaiseAppMainWindowToTop();
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: RaiseAppMainWindowToTop end";
}

/**
 * @tc.name: GetAllAvoidAreas
 * @tc.desc: GetAllAvoidAreas test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetAllAvoidAreas, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: GetAllAvoidAreas start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    WSError res = sProxy->GetAllAvoidAreas(avoidAreas);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->GetAllAvoidAreas(avoidAreas);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: GetAllAvoidAreas end";
}

/**
 * @tc.name: SetLandscapeMultiWindow
 * @tc.desc: SetLandscapeMultiWindow test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetLandscapeMultiWindow, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetLandscapeMultiWindow start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool isLandscapeMultiWindow = true;
    WSError res = sProxy->SetLandscapeMultiWindow(isLandscapeMultiWindow);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->SetLandscapeMultiWindow(isLandscapeMultiWindow);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: SetLandscapeMultiWindow end";
}

/**
 * @tc.name: NotifySyncOn
 * @tc.desc: NotifySyncOn test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifySyncOn, TestSize.Level1)
{
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifySyncOn();

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifySyncOn();

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    sProxy->NotifySyncOn();
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: TriggerBindModalUIExtension
 * @tc.desc: TriggerBindModalUIExtension test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, TriggerBindModalUIExtension, TestSize.Level1)
{
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);
    sProxy->TriggerBindModalUIExtension();

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    sProxy->TriggerBindModalUIExtension();

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    sProxy->TriggerBindModalUIExtension();
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: NotifyPiPWindowPrepareClose
 * @tc.desc: NotifyPiPWindowPrepareClose test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyPiPWindowPrepareClose, TestSize.Level1)
{
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifyPiPWindowPrepareClose();

    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    sProxy->NotifyPiPWindowPrepareClose();

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    sProxy->NotifyPiPWindowPrepareClose();
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: UpdatePiPRect
 * @tc.desc: UpdatePiPRect test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdatePiPRect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdatePiPRect start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    Rect rect = { 200, 200, 200, 200 };
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError res = sProxy->UpdatePiPRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->UpdatePiPRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdatePiPRect end";
}

/**
 * @tc.name: SetSessionLabelAndIcon
 * @tc.desc: SetSessionLabelAndIcon
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetSessionLabelAndIcon, TestSize.Level1)
{
    std::string label = "SetSessionLabelAndIcon";
    std::shared_ptr<Media::PixelMap> icon = std::make_shared<Media::PixelMap>();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(nullptr);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sProxy->SetSessionLabelAndIcon(label, icon));
}

/**
 * @tc.name: UpdateFlag
 * @tc.desc: UpdateFlag
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateFlag, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateFlag start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    std::string flag = "test";
    WSError res = sProxy->UpdateFlag(flag);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateFlag end";
}

/**
 * @tc.name: ProcessPointDownSession
 * @tc.desc: ProcessPointDownSession test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, ProcessPointDownSession, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: ProcessPointDownSession start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    int32_t posX = 50;
    int32_t posY = 50;
    WSError res = sProxy->ProcessPointDownSession(posX, posY);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->ProcessPointDownSession(posX, posY);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: ProcessPointDownSession end";
}

/**
 * @tc.name: SendPointEventForMoveDrag
 * @tc.desc: SendPointEventForMoveDrag test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SendPointEventForMoveDrag, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SendPointEventForMoveDrag start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    bool isExecuteDelayRaise = true;
    WSError res = sProxy->SendPointEventForMoveDrag(pointerEvent, isExecuteDelayRaise);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->SendPointEventForMoveDrag(pointerEvent, isExecuteDelayRaise);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: SendPointEventForMoveDrag end";
}

/**
 * @tc.name: IsStartMoving
 * @tc.desc: IsStartMoving test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, IsStartMoving, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: IsStartMoving start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool res = sProxy->IsStartMoving();
    ASSERT_EQ(res, false);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->IsStartMoving();
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "SessionProxyTest: IsStartMoving end";
}

/**
 * @tc.name: UpdateRectChangeListenerRegistered
 * @tc.desc: UpdateRectChangeListenerRegistered test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateRectChangeListenerRegistered, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateRectChangeListenerRegistered start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool isRegister = true;
    WSError res = sProxy->UpdateRectChangeListenerRegistered(isRegister);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->UpdateRectChangeListenerRegistered(isRegister);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateRectChangeListenerRegistered end";
}

/**
 * @tc.name: SendExtensionData
 * @tc.desc: SendExtensionData test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SendExtensionData, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SendExtensionData start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    WSError res = sProxy->SendExtensionData(data, reply, option);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->SendExtensionData(data, reply, option);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: SendExtensionData end";
}

/**
 * @tc.name: SetGestureBackEnabled
 * @tc.desc: SetGestureBackEnabled test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetGestureBackEnabled, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: SetGestureBackEnabled start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    bool isEnabled = true;
    WMError res = sProxy->SetGestureBackEnabled(isEnabled);
    ASSERT_EQ(res, WMError::WM_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->SetGestureBackEnabled(isEnabled);
    ASSERT_EQ(res, WMError::WM_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: SetGestureBackEnabled end";
}

/**
 * @tc.name: NotifySubModalTypeChange
 * @tc.desc: NotifySubModalTypeChange test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifySubModalTypeChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifySubModalTypeChange start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);

    SubWindowModalType subWindowModalType = SubWindowModalType::TYPE_APPLICATION_MODALITY;
    WSError res = sProxy->NotifySubModalTypeChange(subWindowModalType);
    ASSERT_EQ(res, WSError::WS_OK);
    sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    res = sProxy->NotifySubModalTypeChange(subWindowModalType);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifySubModalTypeChange end";
}

/**
 * @tc.name: GetCrossAxisState
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetCrossAxisState, TestSize.Level1)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    CrossAxisState state = CrossAxisState::STATE_CROSS;
    WSError res = sProxy->GetCrossAxisState(state);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: OnContainerModalEvent
 * @tc.desc: OnContainerModalEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, OnContainerModalEvent, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: OnContainerModalEvent start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->OnContainerModalEvent("name", "value");
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: OnContainerModalEvent end";
}

/**
 * @tc.name: GetWaterfallMode
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetWaterfallMode, TestSize.Level1)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    bool isWaterfallMode = false;
    WSError res = sProxy->GetWaterfallMode(isWaterfallMode);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: NotifyFollowParentMultiScreenPolicy
 * @tc.desc: NotifyFollowParentMultiScreenPolicy test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyFollowParentMultiScreenPolicy, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyFollowParentMultiScreenPolicy start";
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    ASSERT_EQ(sProxy->NotifyFollowParentMultiScreenPolicy(true), WSError::WS_ERROR_IPC_FAILED);
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_EQ(sProxy->NotifyFollowParentMultiScreenPolicy(true), WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: NotifyFollowParentMultiScreenPolicy end";
}


/**
 * @tc.name: UpdateRotationChangeRegistered
 * @tc.desc: UpdateRotationChangeRegistered test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateRotationChangeRegistered, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateRotationChangeRegistered start";
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->UpdateRotationChangeRegistered(0, false);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyTest: UpdateRotationChangeRegistered start";
}

/**
 * @tc.name: GetTargetOrientationConfigInfo
 * @tc.desc: GetTargetOrientationConfigInfo test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetTargetOrientationConfigInfo, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    std::map<Rosen::WindowType, Rosen::SystemBarProperty> properties;
    Rosen::SystemBarProperty statusBarProperty;
    properties[Rosen::WindowType::WINDOW_TYPE_STATUS_BAR] = statusBarProperty;
    WSError res = sProxy->GetTargetOrientationConfigInfo(Orientation::USER_ROTATION_PORTRAIT, properties);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: KeyFrameAnimateEnd
 * @tc.desc: KeyFrameAnimateEnd test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, KeyFrameAnimateEnd, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    ASSERT_EQ(sProxy->KeyFrameAnimateEnd(), WSError::WS_OK);
}

/**
 * @tc.name: UpdateKeyFrameCloneNode
 * @tc.desc: UpdateKeyFrameCloneNode test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, UpdateKeyFrameCloneNode, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    auto rsCanvasNode = RSCanvasNode::Create();
    ASSERT_NE(rsCanvasNode, nullptr);
    auto rsTransaction = std::make_shared<RSTransaction>();
    ASSERT_NE(rsTransaction, nullptr);
    ASSERT_EQ(sProxy->UpdateKeyFrameCloneNode(rsCanvasNode, rsTransaction), WSError::WS_OK);
}

/**
 * @tc.name: GetIsHighlighted
 * @tc.desc: GetIsHighlighted test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, GetIsHighlighted, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    bool isHighlighted = false;
    ASSERT_EQ(sProxy->GetIsHighlighted(isHighlighted), WSError::WS_ERROR_IPC_FAILED);
}
/**
 * @tc.name: NotifyKeyboardWillShowRegistered
 * @tc.desc: NotifyKeyboardWillShowRegistered
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyKeyboardWillShowRegistered, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    bool registered = true;
    sProxy->NotifyKeyboardWillShowRegistered(registered);
}

/**
 * @tc.name: NotifyKeyboardWillHideRegistered
 * @tc.desc: NotifyKeyboardWillHideRegistered
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, NotifyKeyboardWillHideRegistered, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    bool registered = true;
    sProxy->NotifyKeyboardWillHideRegistered(registered);
}

/**
 * @tc.name: SetSubWindowSource
 * @tc.desc: SetSubWindowSource test
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyTest, SetSubWindowSource, Function | SmallTest | Level2)
{
    auto iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    SubWindowSource source = SubWindowSource::SUB_WINDOW_SOURCE_UNKNOWN;
    ASSERT_EQ(sProxy->SetSubWindowSource(source), WSError::WS_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS