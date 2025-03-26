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
#include "inner_window.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowInnerWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<PlaceHolderWindow> holderWindow_;
    sptr<PlaceholderWindowListener> windowListener_;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;
};

void WindowInnerWindowTest::SetUpTestCase()
{
}

void WindowInnerWindowTest::TearDownTestCase()
{
}

void WindowInnerWindowTest::SetUp()
{
    holderWindow_ = new PlaceHolderWindow();
    windowListener_ = new PlaceholderWindowListener();
    inputEventConsumer_ = std::make_shared<PlaceholderInputEventConsumer> ();
}

void WindowInnerWindowTest::TearDown()
{
}

namespace {
/**
 * @tc.name: PlaceHolderWindow01
 * @tc.desc: test PlaceHolderWindow create/destroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerWindowTest, CreatePlaceHolderWindow01, TestSize.Level1)
{
    Rect rect = { 100, 100, 200, 200 };
    holderWindow_->Create("test01", 0, rect, WindowMode::WINDOW_MODE_FULLSCREEN);
    holderWindow_->Destroy();

    rect = { 100, 100, 200, 200 };
    holderWindow_->Create("test02", 0, rect, WindowMode::WINDOW_MODE_FULLSCREEN);
    holderWindow_->Create("test02", 0, rect, WindowMode::WINDOW_MODE_FULLSCREEN);
    holderWindow_->Destroy();

    rect = { 100, 100, 200, 200 };
    holderWindow_->Create("", 0, rect, WindowMode::WINDOW_MODE_FULLSCREEN);
    holderWindow_->Destroy();

    rect = { 0, 0, 0, 0 };
    holderWindow_->Create("test03", 0, rect, WindowMode::WINDOW_MODE_FULLSCREEN);
    holderWindow_->Destroy();

    holderWindow_->Destroy();

    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    auto ret = inputEventConsumer_->OnInputEvent(keyEvent);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: PlaceholderWindowListener01
 * @tc.desc: test PlaceholderWindowListener OnTouchOutside/AfterUnfocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerWindowTest, PlaceholderWindowListener01, TestSize.Level1)
{
    windowListener_->OnTouchOutside();
    windowListener_->AfterUnfocused();

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    auto ret = inputEventConsumer_->OnInputEvent(pointerEvent);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: PlaceholderInputEventConsumer01
 * @tc.desc: test PlaceholderInputEventConsumer OnInputEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowInnerWindowTest, PlaceholderInputEventConsumer01, TestSize.Level1)
{
    std::shared_ptr<MMI::AxisEvent> axisEvent = nullptr;
    auto ret = inputEventConsumer_->OnInputEvent(axisEvent);
    ASSERT_EQ(ret, false);
}
}
}
}
