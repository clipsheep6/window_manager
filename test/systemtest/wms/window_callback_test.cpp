/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

//gtest
#include <gtest/gtest.h>

#include "ability_context_impl.h"

#include "window_test_utils.h"
#include "mock_session.h"
#include "window_scene_session_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;

class WindowCallbackTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

private:
    static constexpr uint32_t TEST_SLEEP_S = 1; // test sleep time
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};

void WindowCallbackTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowCallbackTest::TearDown()
{
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: WindowRectChange01
 * @tc.desc: windowRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowRectChange01, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window1_1");
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    sptr<IWindowRectChangeListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->UnregisterWindowRectChangeListener(listener));

    sptr<IWindowRectChangeListener> listener1 =
     new (std::nothrow) IWindowRectChangeListener();
    ASSERT_NE(listener1, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener1));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener1));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowRectChange02
 * @tc.desc: windowRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowRectChange02, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window1_2");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    sptr<IWindowRectChangeListener> listener =
     new (std::nothrow) IWindowRectChangeListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowRectChange03
 * @tc.desc: windowRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowRectChange03, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window1_3");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    sptr<IWindowRectChangeListener> listener =
     new (std::nothrow) IWindowRectChangeListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowRectChange04
 * @tc.desc: windowRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowRectChange04, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window1_4");
    option->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    sptr<IWindowRectChangeListener> listener =
     new (std::nothrow) IWindowRectChangeListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowTitleButtonRectChange01
 * @tc.desc: windowTitleButtonRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowTitleButtonRectChange01, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window2_1");
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    sptr<IWindowTitleButtonRectChangedListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    sptr<IWindowTitleButtonRectChangedListener> listener1 =
     new (std::nothrow) IWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener1, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RegisterWindowTitleButtonRectChangeListener(listener1));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UnregisterWindowTitleButtonRectChangeListener(listener1));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowTitleButtonRectChange02
 * @tc.desc: windowTitleButtonRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowTitleButtonRectChange02, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window2_2");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    window->property_->SetPersistentId(10022);
    window->hostSession_ = session;

    sptr<IWindowTitleButtonRectChangedListener> listener =
     new (std::nothrow) IWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowTitleButtonRectChange03
 * @tc.desc: windowTitleButtonRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowTitleButtonRectChange03, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window2_3");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    window->property_->SetPersistentId(10023);
    window->hostSession_ = session;

    sptr<IWindowTitleButtonRectChangedListener> listener =
     new (std::nothrow) IWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->Destroy(true, true);
}

/**
 * @tc.name: WindowTitleButtonRectChange04
 * @tc.desc: windowTitleButtonRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, WindowTitleButtonRectChange04, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window2_4");
    option->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    window->property_->SetPersistentId(10024);
    window->hostSession_ = session;

    sptr<IWindowTitleButtonRectChangedListener> listener =
     new (std::nothrow) IWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowTitleButtonRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowTitleButtonRectChangeListener(listener));

    window->Destroy(true, true);
}

/**
 * @tc.name: MainWindowClose01
 * @tc.desc: MainWindowClose
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, MainWindowClose01, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window3_1");
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    sptr<IMainWindowCloseListener> listener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RegisterMainWindowCloseListeners(listener));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UnregisterMainWindowCloseListeners(listener));

    sptr<IMainWindowCloseListener> listener1 =
     new (std::nothrow) IMainWindowCloseListener();
    ASSERT_NE(listener1, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RegisterMainWindowCloseListeners(listener1));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UnregisterMainWindowCloseListeners(listener1));

    window->Destroy(true, true);
}

/**
 * @tc.name: MainWindowClose02
 * @tc.desc: MainWindowClose
 * @tc.type: FUNC
 */
HWTEST_F(WindowCallbackTest, MainWindowClose02, Function | MediumTest | Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Window3_2");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));

    window->property_->SetPersistentId(10032);
    window->hostSession_ = session;

    sptr<IMainWindowCloseListener> listener =
     new (std::nothrow) IMainWindowCloseListener();
    ASSERT_NE(listener, nullptr);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->RegisterMainWindowCloseListeners(listener));
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->UnregisterMainWindowCloseListeners(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(WMError::WM_OK, window->RegisterMainWindowCloseListeners(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterMainWindowCloseListeners(listener));

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->RegisterMainWindowCloseListeners(listener));
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->UnregisterMainWindowCloseListeners(listener));

    window->Destroy(true, true);
}

}

} // namespace Rosen
} // namespace OHOSgit