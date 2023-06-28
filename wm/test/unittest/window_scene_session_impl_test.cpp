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
#include "ability_context_impl.h"
#include "mock_session.h"
#include "window_scene_session_impl.h"
#include "mock_uicontent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MockWindowChangeListener : public IWindowChangeListener {
public:
    MOCK_METHOD3(OnSizeChange, void(Rect rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction));
};

class MockWindowLifeCycleListener : public IWindowLifeCycle {
public:
    MOCK_METHOD0(AfterForeground, void(void));
    MOCK_METHOD0(AfterBackground, void(void));
    MOCK_METHOD0(AfterFocused, void(void));
    MOCK_METHOD0(AfterUnfocused, void(void));
    MOCK_METHOD1(ForegroundFailed, void(int32_t));
    MOCK_METHOD0(AfterActive, void(void));
    MOCK_METHOD0(AfterInactive, void(void));
};

class WindowSceneSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};

void WindowSceneSessionImplTest::SetUpTestCase()
{
}

void WindowSceneSessionImplTest::TearDownTestCase()
{
}

void WindowSceneSessionImplTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest::TearDown()
{
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: LimitCameraFloatWindowMininumSize01
 * @tc.desc: Create window and destroy window
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, LimitCameraFloatWindowMininumSize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("LimitCameraFloatWindowMininumSize01");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    uint32_t width = 100;
    uint32_t height = 200;
    window->LimitCameraFloatWindowMininumSize(nullptr, session);
    ASSERT_EQ(width, 360);
    ASSERT_EQ(height, 742);
    width = 480;
    height = 960;
    window->LimitCameraFloatWindowMininumSize(nullptr, session);
    ASSERT_EQ(width, 360);
    ASSERT_EQ(height, 742);
}

/**
 * @tc.name: Resize01
 * @tc.desc: Hide session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Resize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Resize01");
    sptr<WindowSceneSessionImpl> window = new(std::nothrow) WindowSceneSessionImpl(option);
    uint32_t width = 100;
    uint32_t height = 200;
    ASSERT_EQ(true, window->Resize(nullptr, session));
    ASSERT_EQ(width, 360);
    ASSERT_EQ(height, 742);
    width = 480;
    height = 960;
    ASSERT_EQ(true, window->Resize(nullptr, session));
    ASSERT_EQ(width, 360);
    ASSERT_EQ(height, 742);
}
}
} // namespace Rosen
} // namespace OHOS
