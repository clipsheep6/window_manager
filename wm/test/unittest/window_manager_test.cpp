/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "window_manager.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "scene_session_manager.h"

#include "window_manager.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class TestCameraFloatWindowChangedListener : public ICameraFloatWindowChangedListener {
public:
    void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) override
    {
        WLOGI("TestCameraFloatWindowChangedListener [%{public}u, %{public}u]", accessTokenId, isShowing);
    };
};

class TestVisibilityChangedListener : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
        WLOGI("TestVisibilityChangedListener");
    };
};

class TestSystemBarChangedListener : public ISystemBarChangedListener {
public:
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
        WLOGI("TestSystemBarChangedListener");
    };
};

class TestWindowUpdateListener : public IWindowUpdateListener {
public:
    void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
        WLOGI("TestWindowUpdateListener");
    };
};

class TestWindowModeChangedListener : public IWindowModeChangedListener {
public:
    void OnWindowModeUpdate(WindowModeType mode) override
    {
        WLOGI("TestWindowModeChangedListener");
    };
};

class TestWaterMarkFlagChangeListener : public IWaterMarkFlagChangedListener {
public:
    void OnWaterMarkFlagUpdate(bool showWaterMark) override
    {
        WLOGI("TestWaterMarkFlagChangeListener");
    };
};

class TestGestureNavigationEnabledChangedListener : public IGestureNavigationEnabledChangedListener {
public:
    void OnGestureNavigationEnabledUpdate(bool enable) override
    {
        WLOGI("TestGestureNavigationEnabledChangedListener");
    };
};

class TestVisibleWindowNumChangedListener : public IVisibleWindowNumChangedListener {
public:
    void OnVisibleWindowNumChange(const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo) override
    {
        for (const auto& num : visibleWindowNumInfo) {
            GTEST_LOG_(INFO) << "displayId " << num.displayId << ", visibleWindowNum " << num.visibleWindowNum;
        }
    };
};

class WindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowManagerTest::SetUpTestCase()
{
}

void WindowManagerTest::TearDownTestCase()
{
}

void WindowManagerTest::SetUp()
{
}

void WindowManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetVisibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<sptr<WindowVisibilityInfo>> infos;
    infos.clear();
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
}

/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: ToggleShownStateForAllAppWindows ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ToggleShownStateForAllAppWindows, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), ToggleShownStateForAllAppWindows()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().ToggleShownStateForAllAppWindows());
}

/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetAccessibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.clear();
    EXPECT_CALL(m->Mock(), GetAccessibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetAccessibilityWindowInfo(infos));
    EXPECT_CALL(m->Mock(), GetAccessibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, WindowManager::GetInstance().GetAccessibilityWindowInfo(infos));
}

/**
 * @tc.name: RegisterCameraFloatWindowChangedListener01
 * @tc.desc: check RegisterCameraFloatWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterCameraFloatWindowChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->cameraFloatWindowChangedListeners_;
    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->cameraFloatWindowChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterCameraFloatWindowChangedListener(nullptr));

    sptr<TestCameraFloatWindowChangedListener> listener = new TestCameraFloatWindowChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterCameraFloatWindowChangedListener(listener));

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterCameraFloatWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterCameraFloatWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->cameraFloatWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterCameraFloatWindowChangedListener01
 * @tc.desc: check UnregisterCameraFloatWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterCameraFloatWindowChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->cameraFloatWindowChangedListeners_;
    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->cameraFloatWindowChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterCameraFloatWindowChangedListener(nullptr));

    sptr<TestCameraFloatWindowChangedListener> listener1 = new TestCameraFloatWindowChangedListener();
    sptr<TestCameraFloatWindowChangedListener> listener2 = new TestCameraFloatWindowChangedListener();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraFloatWindowChangedListener(listener1));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterCameraFloatWindowChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterCameraFloatWindowChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraFloatWindowChangedListener(listener1));


    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraFloatWindowChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_);

    windowManager.pImpl_->cameraFloatWindowChangedListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterCameraFloatWindowChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->cameraFloatWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterVisibilityChangedListener01
 * @tc.desc: check RegisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibilityChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowVisibilityListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowVisibilityListeners_;
    windowManager.pImpl_->windowVisibilityListenerAgent_ = nullptr;
    windowManager.pImpl_->windowVisibilityListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterVisibilityChangedListener(nullptr));

    sptr<TestVisibilityChangedListener> listener = new TestVisibilityChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterVisibilityChangedListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowVisibilityListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibilityChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowVisibilityListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibilityChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowVisibilityListeners_.size());

    windowManager.pImpl_->windowVisibilityListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowVisibilityListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterVisibilityChangedListener01
 * @tc.desc: check UnregisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterVisibilityChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowVisibilityListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowVisibilityListeners_;
    windowManager.pImpl_->windowVisibilityListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->windowVisibilityListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterVisibilityChangedListener(nullptr));

    sptr<TestVisibilityChangedListener> listener1 = new TestVisibilityChangedListener();
    sptr<TestVisibilityChangedListener> listener2 = new TestVisibilityChangedListener();

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterVisibilityChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterVisibilityChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->windowVisibilityListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterVisibilityChangedListener(listener1));
    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterVisibilityChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->windowVisibilityListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowVisibilityListenerAgent_);

    windowManager.pImpl_->windowVisibilityListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterVisibilityChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->windowVisibilityListeners_.size());

    windowManager.pImpl_->windowVisibilityListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowVisibilityListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWindowUpdateListener01
 * @tc.desc: check RegisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowUpdateListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowUpdateListeners_;
    windowManager.pImpl_->windowUpdateListenerAgent_ = nullptr;
    windowManager.pImpl_->windowUpdateListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener = new TestWindowUpdateListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWindowUpdateListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowUpdateListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWindowUpdateListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowUpdateListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWindowUpdateListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowUpdateListeners_.size());

    windowManager.pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowUpdateListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowUpdateListener01
 * @tc.desc: check UnregisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowUpdateListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowUpdateListeners_;
    windowManager.pImpl_->windowUpdateListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->windowUpdateListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener1 = new TestWindowUpdateListener();
    sptr<TestWindowUpdateListener> listener2 = new TestWindowUpdateListener();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowUpdateListener(listener1));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterWindowUpdateListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterWindowUpdateListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->windowUpdateListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowUpdateListener(listener1));
    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowUpdateListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->windowUpdateListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowUpdateListenerAgent_);

    windowManager.pImpl_->windowUpdateListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowUpdateListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->windowUpdateListeners_.size());

    windowManager.pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowUpdateListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWindowModeChangedListener01
 * @tc.desc: check RegisterWindowModeChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowModeChangedListener01, Function | SmallTest | Level2)
{
    auto &windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowModeListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowModeListeners_;
    windowManager.pImpl_->windowModeListenerAgent_ = nullptr;
    windowManager.pImpl_->windowModeListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWindowModeChangedListener(nullptr));

    sptr<TestWindowModeChangedListener> listener = new TestWindowModeChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWindowModeChangedListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowModeListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWindowModeChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowModeListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWindowModeChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowModeListeners_.size());

    windowManager.pImpl_->windowModeListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowModeListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterWindowModeChangedListener01
 * @tc.desc: check UnregisterWindowModeChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowModeChangedListener01, Function | SmallTest | Level2)
{
    auto &windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowModeListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowModeListeners_;
    windowManager.pImpl_->windowModeListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->windowModeListeners_.clear();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterWindowModeChangedListener(nullptr));

    sptr<TestWindowModeChangedListener> listener1 = new TestWindowModeChangedListener();
    sptr<TestWindowModeChangedListener> listener2 = new TestWindowModeChangedListener();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListener(listener1));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterWindowModeChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterWindowModeChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->windowModeListeners_.size());

    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListener(listener1));
    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->windowModeListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowModeListenerAgent_);

    windowManager.pImpl_->windowModeListeners_.emplace_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWindowModeChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->windowModeListeners_.size());

    windowManager.pImpl_->windowModeListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowModeListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterSystemBarChangedListener01
 * @tc.desc: check RegisterSystemBarChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterSystemBarChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->systemBarChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->systemBarChangedListeners_;
    windowManager.pImpl_->systemBarChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->systemBarChangedListeners_.clear();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterSystemBarChangedListener(nullptr));

    sptr<ISystemBarChangedListener> listener = new TestSystemBarChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->systemBarChangedListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->systemBarChangedListeners_.size());

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->systemBarChangedListeners_.size());

    windowManager.pImpl_->systemBarChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->systemBarChangedListeners_ = oldListeners;
}

/**
 * @tc.name: UnregisterSystemBarChangedListener01
 * @tc.desc: check UnregisterSystemBarChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterSystemBarChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->systemBarChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->systemBarChangedListeners_;
    windowManager.pImpl_->systemBarChangedListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->systemBarChangedListeners_.clear();
    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterSystemBarChangedListener(nullptr));

    sptr<TestSystemBarChangedListener> listener1 = new TestSystemBarChangedListener();
    sptr<TestSystemBarChangedListener> listener2 = new TestSystemBarChangedListener();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterSystemBarChangedListener(listener1));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterSystemBarChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterSystemBarChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->systemBarChangedListeners_.size());


    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterSystemBarChangedListener(listener1));
    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterSystemBarChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->systemBarChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->systemBarChangedListenerAgent_);

    windowManager.pImpl_->systemBarChangedListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterSystemBarChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->systemBarChangedListeners_.size());

    windowManager.pImpl_->systemBarChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->systemBarChangedListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWaterMarkListener01
 * @tc.desc: check RegisterWaterMarkListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWaterMarkFlagChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();

    windowManager.pImpl_->waterMarkFlagChangeAgent_ = nullptr;
    windowManager.pImpl_->waterMarkFlagChangeListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWaterMarkFlagChangedListener(nullptr));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<TestWaterMarkFlagChangeListener> listener = new TestWaterMarkFlagChangeListener();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterWaterMarkFlagChangedListener(listener));
    ASSERT_EQ(0, windowManager.pImpl_->waterMarkFlagChangeListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->waterMarkFlagChangeAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWaterMarkFlagChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->waterMarkFlagChangeListeners_.size());
    ASSERT_NE(nullptr, windowManager.pImpl_->waterMarkFlagChangeAgent_);

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterWaterMarkFlagChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->waterMarkFlagChangeListeners_.size());
}

/**
 * @tc.name: UnregisterWaterMarkFlagChangedListener01
 * @tc.desc: check UnregisterWaterMarkFlagChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWaterMarkFlagChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->waterMarkFlagChangeAgent_ = nullptr;
    windowManager.pImpl_->waterMarkFlagChangeListeners_.clear();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterWaterMarkFlagChangedListener(nullptr));

    sptr<TestWaterMarkFlagChangeListener> listener1 = new TestWaterMarkFlagChangeListener();
    sptr<TestWaterMarkFlagChangeListener> listener2 = new TestWaterMarkFlagChangeListener();
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWaterMarkFlagChangedListener(listener1));

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterWaterMarkFlagChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterWaterMarkFlagChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->waterMarkFlagChangeListeners_.size());

    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWaterMarkFlagChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWaterMarkFlagChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->waterMarkFlagChangeListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->waterMarkFlagChangeAgent_);

    // if agent == nullptr, it can not be crashed.
    windowManager.pImpl_->waterMarkFlagChangeListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterWaterMarkFlagChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->waterMarkFlagChangeListeners_.size());
}

/**
 * @tc.name: RegisterGestureNavigationEnabledChangedListener
 * @tc.desc: check RegisterGestureNavigationEnabledChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterGestureNavigationEnabledChangedListener, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();

    windowManager.pImpl_->gestureNavigationEnabledAgent_ = nullptr;
    windowManager.pImpl_->gestureNavigationEnabledListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterGestureNavigationEnabledChangedListener(nullptr));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<TestGestureNavigationEnabledChangedListener> listener = new TestGestureNavigationEnabledChangedListener();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterGestureNavigationEnabledChangedListener(listener));
    ASSERT_EQ(0, windowManager.pImpl_->gestureNavigationEnabledListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->gestureNavigationEnabledAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterGestureNavigationEnabledChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->gestureNavigationEnabledListeners_.size());
    ASSERT_NE(nullptr, windowManager.pImpl_->gestureNavigationEnabledAgent_);

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterGestureNavigationEnabledChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->gestureNavigationEnabledListeners_.size());
}

/**
 * @tc.name: UnregisterGestureNavigationEnabledChangedListener
 * @tc.desc: check UnregisterGestureNavigationEnabledChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterGestureNavigationEnabledChangedListener, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->gestureNavigationEnabledAgent_ = nullptr;
    windowManager.pImpl_->gestureNavigationEnabledListeners_.clear();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterGestureNavigationEnabledChangedListener(nullptr));

    sptr<TestGestureNavigationEnabledChangedListener> listener1 = new TestGestureNavigationEnabledChangedListener();
    sptr<TestGestureNavigationEnabledChangedListener> listener2 = new TestGestureNavigationEnabledChangedListener();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM,
        windowManager.UnregisterGestureNavigationEnabledChangedListener(listener1));

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterGestureNavigationEnabledChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterGestureNavigationEnabledChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->gestureNavigationEnabledListeners_.size());

    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterGestureNavigationEnabledChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterGestureNavigationEnabledChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->gestureNavigationEnabledListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->gestureNavigationEnabledAgent_);

    // if agent == nullptr, it can not be crashed.
    windowManager.pImpl_->gestureNavigationEnabledListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterGestureNavigationEnabledChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->gestureNavigationEnabledListeners_.size());
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: window GetFocusWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetFocusWindowInfo, Function | SmallTest | Level2)
{
    FocusChangeInfo focusInfo;
    auto ret = 0;
    WindowManager::GetInstance().GetFocusWindowInfo(focusInfo);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: MinimizeAllAppWindows
 * @tc.desc: window MinimizeAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, MinimizeAllAppWindows, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    WMError ret = WindowManager::GetInstance().MinimizeAllAppWindows(displayId);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: SetWindowLayoutMode
 * @tc.desc: window SetWindowLayoutMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, SetWindowLayoutMode, Function | SmallTest | Level2)
{
    WindowLayoutMode mode = WindowLayoutMode::BASE;
    WMError ret = WindowManager::GetInstance().SetWindowLayoutMode(mode);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus
 * @tc.desc: UpdateCameraFloatWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UpdateCameraFloatWindowStatus, Function | SmallTest | Level2)
{
    uint32_t accessTokenId = 0;
    bool isShowing = true;
    auto ret = 0;
    WindowManager::GetInstance().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: NotifyWaterMarkFlagChangedResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyWaterMarkFlagChangedResult, Function | SmallTest | Level2)
{
    bool showwatermark = true;
    auto ret = 0;
    WindowManager::GetInstance().NotifyWaterMarkFlagChangedResult(showwatermark);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: NotifyGestureNavigationEnabledResult
 * @tc.desc: NotifyGestureNavigationEnabledResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, NotifyGestureNavigationEnabledResult, Function | SmallTest | Level2)
{
    bool enable = true;
    auto ret = 0;
    WindowManager::GetInstance().NotifyGestureNavigationEnabledResult(enable);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: DumpSessionAll
 * @tc.desc: DumpSessionAll
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, DumpSessionAll, Function | SmallTest | Level2)
{
    std::vector<std::string> infos;
    infos.push_back("DumpSessionWithId");
    WMError res = WindowManager::GetInstance().DumpSessionAll(infos);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: DumpSessionWithId
 * @tc.desc: DumpSessionWithId
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, DumpSessionWithId, Function | SmallTest | Level2)
{
    std::vector<std::string> infos;
    infos.push_back("DumpSessionWithId");
    int32_t persistentId = 0;
    WMError res = WindowManager::GetInstance().DumpSessionWithId(persistentId, infos);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: RegisterVisibleWindowNumChangedListener
 * @tc.desc: check RegisterVisibleWindowNumChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibleWindowNumChangedListener, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();

    windowManager.pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->visibleWindowNumChangedListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterVisibleWindowNumChangedListener(nullptr));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<TestVisibleWindowNumChangedListener> listener = new TestVisibleWindowNumChangedListener();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterVisibleWindowNumChangedListener(listener));
    ASSERT_EQ(0, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->visibleWindowNumChangedListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibleWindowNumChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
    ASSERT_NE(nullptr, windowManager.pImpl_->visibleWindowNumChangedListenerAgent_);

    // to check that the same listner can not be registered twice
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibleWindowNumChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
}

/**
 * @tc.name: UnregisterVisibleWindowNumChangedListener
 * @tc.desc: check UnregisterVisibleWindowNumChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterVisibleWindowNumChangedListener, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    windowManager.pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->visibleWindowNumChangedListeners_.clear();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    // check nullpter
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.UnregisterVisibleWindowNumChangedListener(nullptr));

    sptr<TestVisibleWindowNumChangedListener> listener1 = new TestVisibleWindowNumChangedListener();
    sptr<TestVisibleWindowNumChangedListener> listener2 = new TestVisibleWindowNumChangedListener();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM,
        windowManager.UnregisterVisibleWindowNumChangedListener(listener1));

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterVisibleWindowNumChangedListener(listener1);
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowManager.RegisterVisibleWindowNumChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());

    EXPECT_CALL(m->Mock(), UnregisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterVisibleWindowNumChangedListener(listener1));
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterVisibleWindowNumChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->visibleWindowNumChangedListenerAgent_);

    // if agent == nullptr, it can not be crashed.
    windowManager.pImpl_->visibleWindowNumChangedListeners_.push_back(listener1);
    ASSERT_EQ(WMError::WM_OK, windowManager.UnregisterVisibleWindowNumChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
}

/**
 * @tc.name: RegisterAndOnVisibleWindowNumChanged
 * @tc.desc: check RegisterAndOnVisibleWindowNumChanged
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterAndOnVisibleWindowNumChanged, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();

    windowManager.pImpl_->visibleWindowNumChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->visibleWindowNumChangedListeners_.clear();

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterVisibleWindowNumChangedListener(nullptr));

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<TestVisibleWindowNumChangedListener> listener = new TestVisibleWindowNumChangedListener();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowManager.RegisterVisibleWindowNumChangedListener(listener));
    ASSERT_EQ(0, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->visibleWindowNumChangedListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, windowManager.RegisterVisibleWindowNumChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->visibleWindowNumChangedListeners_.size());
 
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo = {};
    visibleWindowNumInfo newInfo;
    newInfo.displayId = 0;
    newInfo.visibleWindowNum = 2;
    visibleWindowNumInfo.push_back(newInfo);
    auto ret = 0;
    WindowManager::GetInstance().UpdateVisibleWindowNum(visibleWindowNumInfo);
    ASSERT_EQ(0, ret);
}
}
} // namespace Rosen
} // namespace OHOS