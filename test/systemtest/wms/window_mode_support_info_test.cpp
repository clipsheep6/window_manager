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
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "scene_board_judgement.h"
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowModeSupportTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo fullAppInfo_1_;
    Utils::TestWindowInfo fullAppInfo_2_;

private:
    static constexpr uint32_t WAIT_SYANC_US = 100000;
};

void WindowModeSupportTypeTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = { 0, 0, display->GetWidth(), display->GetHeight() };
    Utils::InitByDisplayRect(displayRect);
}

void WindowModeSupportTypeTest::TearDownTestCase()
{
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Destroy();
#endif
}

void WindowModeSupportTypeTest::SetUp()
{
    fullAppInfo_1_ = {
        .name = "FullWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    fullAppInfo_2_ = {
        .name = "FullWindow2",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowModeSupportTypeTest::TearDown() {}

namespace {
/**
 * @tc.name: WindowModeSupportType01
 * @tc.desc: SetRequestWindowModeSupportType | GetRequestWindowModeSupportType
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportTypeTest, WindowModeSupportType01, TestSize.Level1)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    if (window == nullptr) {
        return;
    }
    window->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN, window->GetRequestWindowModeSupportType());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportType02
 * @tc.desc: windowModeSupportType test for single window, only support fullScreen mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportTypeTest, WindowModeSupportType02, TestSize.Level1)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    if (window == nullptr) {
        return;
    }
    window->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportType03
 * @tc.desc: windowModeSupportType test for single window, support both fullScreen and floating mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportTypeTest, WindowModeSupportType03, TestSize.Level1)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    if (window == nullptr) {
        return;
    }
    window->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN |
                                            WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportType04
 * @tc.desc: windowModeSupportType test for single window, window mode is not supported when show, show failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportTypeTest, WindowModeSupportType04, TestSize.Level1)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    if (window == nullptr) {
        return;
    }
    window->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING |
                                            WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                                            WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
    ASSERT_NE(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportType05
 * @tc.desc: windowModeSupportType test for layout cascade
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportTypeTest, WindowModeSupportType05, TestSize.Level1)
{
    const sptr<Window>& window1 = Utils::CreateTestWindow(fullAppInfo_1_);
    if (window1 == nullptr) {
        return;
    }
    window1->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    const sptr<Window>& window2 = Utils::CreateTestWindow(fullAppInfo_2_);
    ASSERT_NE(nullptr, window2);
    window2->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    usleep(WAIT_SYANC_US);

    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window1->GetWindowMode());
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window2->GetWindowMode());
    } else {
        ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window2->GetWindowMode());
    }
    window1->Destroy();
    window2->Destroy();
}

/**
 * @tc.name: WindowModeSupportType06
 * @tc.desc: windowModeSupportType test for layout tile
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportTypeTest, WindowModeSupportType06, TestSize.Level1)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    if (window == nullptr) {
        return;
    }
    window->SetRequestWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    usleep(WAIT_SYANC_US);

    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetWindowMode());

    window->Destroy();
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    usleep(WAIT_SYANC_US);
}
} // namespace
} // namespace Rosen
} // namespace OHOS