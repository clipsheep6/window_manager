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
#include "window_extension_context.h"
#include "window_extension_proxy.h"
#include "window_extension_stub.h"
#include "window_extension_stub_impl.h"
#include "window_extension_client_interface.h"
#include "window_extension_client_stub_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ExtensionCallback : public Rosen::IWindowExtensionCallback {
public:
    ExtensionCallback() = default;
    ~ExtensionCallback()  = default;
    void OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode) override;
    void OnExtensionDisconnected() override;
    void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event) override;
    void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event) override;
    void OnBackPress() override;
    bool isWindowReady_ = false;
    bool isBackPress_ = false;
    bool iskeyEvent_ = false;
    bool ispointerEvent_ = false;
};

void ExtensionCallback::OnWindowReady(const std::shared_ptr<Rosen::RSSurfaceNode>& rsSurfaceNode)
{
    isWindowReady_ = true;
}

void ExtensionCallback::OnExtensionDisconnected()
{
}

void ExtensionCallback::OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event)
{
    iskeyEvent_ = true;
}

void ExtensionCallback::OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event)
{
    ispointerEvent_ = true;
}

void ExtensionCallback::OnBackPress()
{
    isBackPress_ = true;
}

class WindowExtensionClientStubImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowExtensionClientStubImplTest::SetUpTestCase()
{
}

void WindowExtensionClientStubImplTest::TearDownTestCase()
{
}

void WindowExtensionClientStubImplTest::SetUp()
{
}

void WindowExtensionClientStubImplTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnWindowReady
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionClientStubImplTest, OnWindowReady, Function | SmallTest | Level2)
{
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
    ExtensionCallback extensionCallback;
    sptr<IWindowExtensionCallback> componentCallback;
    WindowExtensionClientStubImpl windowExtensionClientStubImpl(componentCallback);
    windowExtensionClientStubImpl.OnWindowReady(surfaceNode);
    componentCallback = new ExtensionCallback;
    windowExtensionClientStubImpl.OnWindowReady(surfaceNode);
    ASSERT_EQ(extensionCallback.isWindowReady_, false);
    componentCallback.clear();
}

/**
 * @tc.name: OnBackPress
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionClientStubImplTest, OnBackPress, Function | SmallTest | Level2)
{
    ExtensionCallback extensionCallback;
    sptr<IWindowExtensionCallback> componentCallback;
    WindowExtensionClientStubImpl windowExtensionClientStubImpl(componentCallback);
    windowExtensionClientStubImpl.OnBackPress();
    componentCallback = new ExtensionCallback;
    windowExtensionClientStubImpl.OnBackPress();
    ASSERT_EQ(extensionCallback.isBackPress_, false);
    componentCallback.clear();
}

/**
 * @tc.name: OnKeyEvent
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionClientStubImplTest, OnKeyEvent, Function | SmallTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    ExtensionCallback extensionCallback;
    sptr<IWindowExtensionCallback> componentCallback;
    WindowExtensionClientStubImpl windowExtensionClientStubImpl(componentCallback);
    windowExtensionClientStubImpl.OnKeyEvent(keyEvent);
    componentCallback = new ExtensionCallback;
    windowExtensionClientStubImpl.OnKeyEvent(keyEvent);
    ASSERT_EQ(extensionCallback.iskeyEvent_, false);
    componentCallback.clear();
}

/**
 * @tc.name: OnPointerEvent
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowExtensionClientStubImplTest, OnPointerEvent, Function | SmallTest | Level2)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    ExtensionCallback extensionCallback;
    sptr<IWindowExtensionCallback> componentCallback;
    WindowExtensionClientStubImpl windowExtensionClientStubImpl(componentCallback);
    windowExtensionClientStubImpl.OnPointerEvent(pointerEvent);
    componentCallback = new ExtensionCallback;
    windowExtensionClientStubImpl.OnPointerEvent(pointerEvent);
    ASSERT_EQ(extensionCallback.ispointerEvent_, false);
    componentCallback.clear();
}

}
}
}