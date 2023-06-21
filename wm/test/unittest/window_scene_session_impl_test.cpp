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
#include "window_session_impl.h"
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
 * @tc.name: CreateWindowAndDestroy01
 * @tc.desc: Create window and destroy window
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Create01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
     ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Create(nullptr, nullptr));
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);`
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(nullptr, session));

    EXPECT_CALL(*(session), Connect(_, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    // ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session));
    // window->property_->SetPersistentId(1);
    // ASSERT_EQ(WMError::WM_OK, window->Destroy());

    // EXPECT_CALL(*(session), Connect(_, _, _, _, _)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    // ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Create(abilityContext_, session));
    // // session is null
    // window = new WindowSceneSessionImpl(option);
    // ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(abilityContext_, nullptr));
    // ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());
}


}
} // namespace Rosen
} // namespace OHOS
