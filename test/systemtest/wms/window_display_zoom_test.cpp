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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"
#include "window_accessibility_controller.h"
#include "window_impl.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowDisplayZoomTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo windowInfo_;
};

void WindowDisplayZoomTest::SetUpTestCase()
{
}

void WindowDisplayZoomTest::TearDownTestCase()
{
}

void WindowDisplayZoomTest::SetUp()
{
    windowInfo_ = {
        .name = "zoomWindow",
        .rect = {0, 0, 300, 100},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentName = "",
    };
}

void WindowDisplayZoomTest::TearDown()
{
}

namespace {
/**
 * @tc.name: DisplayZoom01
 * @tc.desc: test interface SetAnchorAndScale
 * @tc.type: FUNC
 * @tc.require: issueI5NGWL
 */
HWTEST_F(WindowDisplayZoomTest, DisplayZoom01, Function | MediumTest | Level3)
{
    WindowAccessibilityController::GetInstance().OffWindowZoom();
    sleep(1);

    windowInfo_.name = "DisplayZoom01";
    sptr<Window> window = Utils::CreateTestWindow(windowInfo_);
    Window* ptr = window.GetRefPtr();
    WindowImpl* implPtr = (WindowImpl*)ptr;
    ASSERT_EQ(WMError::WM_OK, window->Show());
    Transform expect;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(1);
    Rect rect = window->GetRect();
    expect.pivotX_ = (0 - rect.posX_) * 1.0 / rect.width_;
    expect.pivotY_ = (0 - rect.posY_) * 1.0 / rect.height_;
    expect.scaleX_ = expect.scaleY_ = 2;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(1);
    expect.scaleX_ = expect.scaleY_ = 4;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 0.5);
    sleep(1);
    expect.scaleX_ = expect.scaleY_ = 2;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 0.1);
    sleep(1);
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, -0.1);
    sleep(1);
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().OffWindowZoom();
    window->Destroy();
}

/**
 * @tc.name: DisplayZoom02
 * @tc.desc: test interface SetAnchorOffset
 * @tc.type: FUNC
 * @tc.require: issueI5NGWL
 */
HWTEST_F(WindowDisplayZoomTest, DisplayZoom02, Function | MediumTest | Level3)
{
    windowInfo_.name = "DisplayZoom02";
    sptr<Window> window = Utils::CreateTestWindow(windowInfo_);
    Window* ptr = window.GetRefPtr();
    WindowImpl* implPtr = (WindowImpl*)ptr;
    ASSERT_EQ(WMError::WM_OK, window->Show());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(1);
    WindowAccessibilityController::GetInstance().SetAnchorOffset(-100, -100);
    sleep(1);

    Transform expect;
    Rect rect = window->GetRect();
    expect.pivotX_ = (0 - rect.posX_) * 1.0 / rect.width_;
    expect.pivotY_ = (0 - rect.posY_) * 1.0 / rect.height_;
    expect.scaleX_ = expect.scaleY_ = 2;
    expect.translateX_ = expect.translateY_ = -100;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().SetAnchorOffset(200, 200);
    sleep(1);

    expect.translateX_ = expect.translateY_ = 0;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().OffWindowZoom();
    window->Destroy();
}

/**
 * @tc.name: DisplayZoom03
 * @tc.desc: test interface OffWindowZoom
 * @tc.type: FUNC
 * @tc.require: issueI5NGWL
 */
HWTEST_F(WindowDisplayZoomTest, DisplayZoom03, Function | MediumTest | Level3)
{
    windowInfo_.name = "DisplayZoom03";
    sptr<Window> window = Utils::CreateTestWindow(windowInfo_);
    Window* ptr = window.GetRefPtr();
    WindowImpl* implPtr = (WindowImpl*)ptr;
    ASSERT_EQ(WMError::WM_OK, window->Show());

    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(1);
    WindowAccessibilityController::GetInstance().OffWindowZoom();
    sleep(1);

    Transform expect;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    window->Destroy();
}

/**
 * @tc.name: DisplayZoom04
 * @tc.desc: test add and remove a window after zoom display
 * @tc.type: FUNC
 * @tc.require: issueI5NGWL
 */
HWTEST_F(WindowDisplayZoomTest, DisplayZoom04, Function | MediumTest | Level3)
{
    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(1);

    windowInfo_.name = "DisplayZoom04";
    sptr<Window> window = Utils::CreateTestWindow(windowInfo_);
    Window* ptr = window.GetRefPtr();
    WindowImpl* implPtr = (WindowImpl*)ptr;
    ASSERT_EQ(WMError::WM_OK, window->Show());

    Transform expect;
    Rect rect = window->GetRect();
    expect.pivotX_ = (0 - rect.posX_) * 1.0 / rect.width_;
    expect.pivotY_ = (0 - rect.posY_) * 1.0 / rect.height_;
    expect.scaleX_ = expect.scaleY_ = 2;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    ASSERT_EQ(WMError::WM_OK, window->Hide());;
    WindowAccessibilityController::GetInstance().OffWindowZoom();
    sleep(1);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    sleep(1);
    Transform identity;
    ASSERT_EQ(identity, implPtr->GetWindowProperty()->GetZoomTransform());

    window->Destroy();
}

/**
 * @tc.name: DisplayZoom05
 * @tc.desc: test animate and zoom transform
 * @tc.type: FUNC
 * @tc.require: issueI5NGWL
 */
HWTEST_F(WindowDisplayZoomTest, DisplayZoom05, Function | MediumTest | Level3)
{
    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(1);

    windowInfo_.name = "DisplayZoom05";
    sptr<Window> window = Utils::CreateTestWindow(windowInfo_);
    Window* ptr = window.GetRefPtr();
    WindowImpl* implPtr = (WindowImpl*)ptr;
    ASSERT_EQ(WMError::WM_OK, window->Show());

    Transform animate;
    animate.translateX_ = -100;
    window->SetTransform(animate);
    sleep(1);

    Transform expect;
    Rect rect = window->GetRect();
    expect.pivotX_ = (0 - rect.posX_) * 1.0 / rect.width_;
    expect.pivotY_ = (0 - rect.posY_) * 1.0 / rect.height_;
    expect.scaleX_ = expect.scaleY_ = 2;
    expect.translateX_ = -200;
    ASSERT_EQ(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().OffWindowZoom();
    window->Destroy();
}
}
} // namespace Rosen
} // namespace OHOS