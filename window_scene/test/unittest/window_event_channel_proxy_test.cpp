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

#include "session/container/include/zidl/window_event_channel_proxy.h"
#include "session/container/include/zidl/window_event_ipc_interface_code.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <message_option.h>
#include <message_parcel.h>
#include <pointer_event.h>
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
namespace OHOS::Accessibility {
class AccessibilityElementInfo;
}
namespace OHOS {
namespace Rosen {
class WindowEventChannelProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<WindowEventChannelProxy> windowEventChannelProxy_ =
        sptr<WindowEventChannelProxy>::MakeSptr(iRemoteObjectMocker);
};

void WindowEventChannelProxyTest::SetUpTestCase()
{
}

void WindowEventChannelProxyTest::TearDownTestCase()
{
}

void WindowEventChannelProxyTest::SetUp()
{
}

void WindowEventChannelProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: TransferKeyEvent
 * @tc.desc: test function : TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferKeyEvent, TestSize.Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};
    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(1);
    ASSERT_TRUE((windowEventChannelProxy_ != nullptr));
    WSError res = windowEventChannelProxy_->TransferKeyEvent(keyEvent);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: test function : TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferPointerEvent, TestSize.Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    ASSERT_TRUE((windowEventChannelProxy_ != nullptr));
    WSError res = windowEventChannelProxy_->TransferPointerEvent(pointerEvent);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferKeyEventForConsumed
 * @tc.desc: test function : TransferKeyEventForConsumed
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferKeyEventForConsumed, TestSize.Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};
    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(1);
    bool isConsumed = false;
    WSError res = windowEventChannelProxy_->TransferKeyEventForConsumed(keyEvent, isConsumed);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferKeyEventForConsumedAsync
 * @tc.desc: test function : TransferKeyEventForConsumedAsync
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferKeyEventForConsumedAsync, TestSize.Level1)
{
    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    bool isPreImeEvent = false;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    WSError res = windowEventChannelProxy_->TransferKeyEventForConsumedAsync(keyEvent, isPreImeEvent,
        iRemoteObjectMocker);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferFocusActiveEvent
 * @tc.desc: test function : TransferFocusActiveEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferFocusActiveEvent, TestSize.Level1)
{
    bool isFocusActive = false;
    WSError res = windowEventChannelProxy_->TransferFocusActiveEvent(isFocusActive);
    ASSERT_EQ(WSError::WS_OK, res);
    isFocusActive = true;
    res = windowEventChannelProxy_->TransferFocusActiveEvent(isFocusActive);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferFocusState
 * @tc.desc: test function : TransferFocusState
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferFocusState, TestSize.Level1)
{
    bool focusState = false;
    WSError res = windowEventChannelProxy_->TransferFocusActiveEvent(focusState);
    ASSERT_EQ(WSError::WS_OK, res);
    focusState = true;
    res = windowEventChannelProxy_->TransferFocusState(focusState);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferAccessibilityHoverEvent
 * @tc.desc: test function : TransferAccessibilityHoverEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferAccessibilityHoverEvent, TestSize.Level1)
{
    float pointX = 0.0f;
    float pointY = 0.0f;
    int32_t sourceType = 0;
    int32_t eventType = 0;
    int64_t timeMs = 0;
    WSError res = windowEventChannelProxy_->TransferAccessibilityHoverEvent(
        pointX, pointY, sourceType, eventType, timeMs);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferAccessibilityChildTreeRegister
 * @tc.desc: test function : TransferAccessibilityChildTreeRegister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferAccessibilityChildTreeRegister, TestSize.Level1)
{
    uint32_t windowId = 0;
    int32_t treeId = 0;
    int64_t accessibilityId = 0;

    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeRegister(windowId, treeId, accessibilityId);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferAccessibilityChildTreeUnregister
 * @tc.desc: test function : TransferAccessibilityChildTreeUnregister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferAccessibilityChildTreeUnregister, TestSize.Level1)
{
    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeUnregister();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferAccessibilityDumpChildInfo
 * @tc.desc: test function : TransferAccessibilityDumpChildInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferAccessibilityDumpChildInfo, TestSize.Level1)
{
    std::vector<std::string> params;
    std::vector<std::string> info;

    WSError res = windowEventChannelProxy_->TransferAccessibilityDumpChildInfo(params, info);
    ASSERT_EQ(WSError::WS_OK, res);
}
}
}
}