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
#include "window_inner_manager.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using utils = WindowTestUtils;
class WindowSplitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    int displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
};

void WindowLayoutTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId_);
    if (display == nullptr) {
        printf("GetDefaultDisplay: failed!\n");
    } else {
        printf("GetDefaultDisplay: id %llu, w %d, h %d, fps %u\n", display->GetId(), display->GetWidth(),
            display->GetHeight(), display->GetFreshRate());
    }
    Rect screenRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByScreenRect(screenRect);
}

void WindowLayoutTest::TearDownTestCase()
{
}

void WindowLayoutTest::SetUp()
{
    activeWindows_.clear();
}

void WindowLayoutTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
 * @tc.name: SplitCreen01
 * @tc.desc: Split Primary and Secondary Window
 * @tc.type: FUNC
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowLayoutTest, SplitCreen01, Function | MediumTest | Level3)
{
    utils::TestWindowInfo splitInfo = {
        .name = "main",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& priWindow = utils::CreateTestWindow(splitInfo);
    activeWindows_.push_back(priWindow);

    splitInfo.mode = WINDOW_MODE_SPLIT_SECONDARY;
    const sptr<Window>& secWindow = utils::CreateTestWindow(splitInfo);
    activeWindows_.push_back(secWindow);

    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    ASSERT_EQ(WMError::WM_OK, secWindow->Show());
    ASSERT_TRUE(utils::SplitRectEqualTo(priWindow, utils::splitRects_.primaryRect));
    ASSERT_TRUE(utils::SplitRectEqualTo(secWindow, utils::splitRects_.secondaryRect));
    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, secWindow->Hide());
}

/**
 * @tc.name: SplitCreen01
 * @tc.desc: One Split Primary and Secondary Window
 * @tc.type: FUNC
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowLayoutTest, SplitCreen01, Function | MediumTest | Level3)
{
    utils::TestWindowInfo splitInfo = {
        .name = "main",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& priWindow = utils::CreateTestWindow(splitInfo);
    activeWindows_.push_back(priWindow);

    splitInfo.mode = WINDOW_MODE_SPLIT_SECONDARY;
    const sptr<Window>& secWindow = utils::CreateTestWindow(splitInfo);
    activeWindows_.push_back(secWindow);

    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    ASSERT_EQ(WMError::WM_OK, secWindow->Show());

    sptr<Window> divWindow= SingletonContainer::Get<WindowInnerManager>().GetDividerWindow(displayId_);
    ASSERT_TRUE(divWindow != nullptr);
    ASSERT_TRUE(utils::SplitRectEqualTo(divWindow, utils::splitRects_.dividerRect));

    int32_t target = static_cast<int32_t>(splitRects_.displayRect.width_ * 0.3); // move to (width * 0.3)
    divWindow.MoveTo(target, target);
    UpdateSplitRect(target);
    ASSERT_TRUE(utils::SplitRectEqualTo(divWindow, utils::splitRects_.dividerRect));
    ASSERT_TRUE(utils::SplitRectEqualTo(priWindow, utils::splitRects_.primaryRect));
    ASSERT_TRUE(utils::SplitRectEqualTo(secWindow, utils::splitRects_.secondaryRect));

    target = static_cast<int32_t>(splitRects_.displayRect.width_ * 0.8); // move to (width * 0.8)
    divWindow.MoveTo(target, target);
    UpdateSplitRect(target);
    ASSERT_TRUE(utils::SplitRectEqualTo(divWindow, utils::splitRects_.dividerRect));
    ASSERT_TRUE(utils::SplitRectEqualTo(priWindow, utils::splitRects_.primaryRect));
    ASSERT_TRUE(utils::SplitRectEqualTo(secWindow, utils::splitRects_.secondaryRect));

    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, secWindow->Hide());
}
} // namespace Rosen
} // namespace OHOSdividerWindow