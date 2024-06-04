/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "window_session_impl.h"

#include <gtest/gtest.h>

#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTwoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
};

void WindowSessionImplTwoTest::SetUpTestCase()
{
}

void WindowSessionImplTwoTest::TearDownTestCase()
{
}

void WindowSessionImplTwoTest::SetUp()
{
}

void WindowSessionImplTwoTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
sptr<WindowSessionImpl> GetTestWindowImpl(const std::string& name)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    option->SetWindowName(name);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    if (window == nullptr) {
        return nullptr;
    }

    SessionInfo sessionInfo = {name, name, name};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    if (session == nullptr) {
        return nullptr;
    }

    window->hostSession_ = session;
    return window;
}

template <typename TListener, typename MockListener>
std::vector<sptr<TListener>> GetListenerList()
{
    std::vector<sptr<TListener>> listeners;
    sptr<TListener> listener = new (std::nothrow) MockListener();
    if (listener == nullptr) {
        return listeners;
    }

    listeners.insert(listeners.begin(), listener);
    return listeners;
}

/**
 * @tc.name: GetTitleButtonVisible
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetTitleButtonVisible, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetTitleButtonVisible");
    ASSERT_NE(window, nullptr);
    bool isPC = false;
    bool isMaximizeVisible = false;
    bool isMinimizeVisible = false;
    bool isSplitVisible = false;
    window->GetTitleButtonVisible(isPC, isMaximizeVisible, isMinimizeVisible, isSplitVisible);
    ASSERT_FALSE(isSplitVisible);

    isPC = true;
    window->windowTitleVisibleFlags_.isSplitVisible = false;
    window->windowTitleVisibleFlags_.isMaximizeVisible = false;
    window->windowTitleVisibleFlags_.isMinimizeVisible = false;
    window->GetTitleButtonVisible(isPC, isMaximizeVisible, isMinimizeVisible, isSplitVisible);
    ASSERT_TRUE(isSplitVisible);
    ASSERT_TRUE(isMaximizeVisible);
    ASSERT_TRUE(isMinimizeVisible);

    window->windowTitleVisibleFlags_.isSplitVisible = true;
    window->windowTitleVisibleFlags_.isMaximizeVisible = true;
    window->windowTitleVisibleFlags_.isMinimizeVisible = true;
    window->GetTitleButtonVisible(isPC, isMaximizeVisible, isMinimizeVisible, isSplitVisible);
    ASSERT_TRUE(isSplitVisible);
    ASSERT_TRUE(isMaximizeVisible);
    ASSERT_TRUE(isMinimizeVisible);
}

/**
 * @tc.name: GetSystemSessionConfig
 * @tc.desc: GetSystemSessionConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetSystemSessionConfig, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetSystemSessionConfig");
    ASSERT_NE(window, nullptr);
    window->GetSystemSessionConfig();
    window->Destroy();
}

/**
 * @tc.name: GetColorSpaceFromSurfaceGamut
 * @tc.desc: GetColorSpaceFromSurfaceGamut
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetColorSpaceFromSurfaceGamut, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetColorSpaceFromSurfaceGamut");
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(window->GetColorSpaceFromSurfaceGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB),
        ColorSpace::COLOR_SPACE_DEFAULT);
    ASSERT_EQ(window->GetColorSpaceFromSurfaceGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3),
        ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    ASSERT_EQ(window->GetColorSpaceFromSurfaceGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID),
        ColorSpace::COLOR_SPACE_DEFAULT);
    window->Destroy();
}

/**
 * @tc.name: GetSurfaceGamutFromColorSpace
 * @tc.desc: GetSurfaceGamutFromColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetSurfaceGamutFromColorSpace, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetSurfaceGamutFromColorSpace");
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(window->GetSurfaceGamutFromColorSpace(ColorSpace::COLOR_SPACE_DEFAULT),
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(window->GetSurfaceGamutFromColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT),
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3);
    ASSERT_EQ(window->GetSurfaceGamutFromColorSpace(ColorSpace(uint32_t(3))),
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    window->Destroy();
}

/**
 * @tc.name: Create
 * @tc.desc: Create
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, Create, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("Create");
    ASSERT_NE(window, nullptr);
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<Rosen::ISession> ISession;
    ASSERT_EQ(window->Create(context, ISession), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: Destroy
 * @tc.desc: Destroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, Destroy, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("Destroy");
    ASSERT_NE(window, nullptr);

    window->property_->SetPersistentId(INVALID_SESSION_ID);
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_DESTROYED;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window->state_ = WindowState::STATE_INITIAL;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_OK);

    window = GetTestWindowImpl("Destroy");
    ASSERT_NE(window, nullptr);
    window->state_ = WindowState::STATE_INITIAL;
    window->property_->SetPersistentId(1);
    ASSERT_EQ(window->Destroy(true, false), WMError::WM_OK);

    window->hostSession_ = nullptr;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window->Destroy();
}

/**
 * @tc.name: GetWindowState
 * @tc.desc: GetWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetWindowState, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetWindowState");
    ASSERT_NE(window, nullptr);
    window->state_ = WindowState::STATE_DESTROYED;
    ASSERT_EQ(window->GetWindowState(), WindowState::STATE_DESTROYED);
    window->Destroy();
}

/**
 * @tc.name: RecoverSessionListener
 * @tc.desc: RecoverSessionListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, RecoverSessionListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RecoverSessionListener");
    ASSERT_NE(window, nullptr);
    int32_t id = 1;
    window->property_->SetPersistentId(id);
    window->avoidAreaChangeListeners_.clear();
    window->touchOutsideListeners_.clear();
    window->RecoverSessionListener();

    std::vector<sptr<IAvoidAreaChangedListener>> iAvoidAreaChangedListeners;
    std::vector<sptr<ITouchOutsideListener>> iTouchOutsideListeners;
    window->avoidAreaChangeListeners_.insert({id, iAvoidAreaChangedListeners});
    window->touchOutsideListeners_.insert({id, iTouchOutsideListeners});
    window->RecoverSessionListener();

    sptr<MockAvoidAreaChangedListener> changedListener = new (std::nothrow) MockAvoidAreaChangedListener();
    ASSERT_NE(nullptr, changedListener);
    sptr<MockTouchOutsideListener> touchOutsideListener = new (std::nothrow) MockTouchOutsideListener();
    ASSERT_NE(nullptr, touchOutsideListener);
    iAvoidAreaChangedListeners.insert(iAvoidAreaChangedListeners.begin(), changedListener);
    iTouchOutsideListeners.insert(iTouchOutsideListeners.begin(), touchOutsideListener);
    window->RecoverSessionListener();
    window->Destroy();
}

/**
 * @tc.name: NotifyUIContentFocusStatus
 * @tc.desc: NotifyUIContentFocusStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyUIContentFocusStatus, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyUIContentFocusStatus");
    ASSERT_NE(window, nullptr);
    window->isFocused_ = false;
    window->NotifyUIContentFocusStatus();

    window->isFocused_ = true;
    window->NotifyUIContentFocusStatus();

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->NotifyUIContentFocusStatus();
    window->Destroy();
}

/**
 * @tc.name: NotifyAfterFocused
 * @tc.desc: NotifyAfterFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyAfterFocused, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyAfterFocused");
    ASSERT_NE(window, nullptr);
    window->NotifyAfterFocused();
    ASSERT_TRUE(window->shouldReNotifyFocus_);

    window->shouldReNotifyFocus_ = false;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->NotifyAfterFocused();
    ASSERT_FALSE(window->shouldReNotifyFocus_);
    window->Destroy();
}

/**
 * @tc.name: NotifyForegroundFailed
 * @tc.desc: NotifyForegroundFailed
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyForegroundFailed, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyForegroundFailed");
    ASSERT_NE(window, nullptr);
    window->NotifyForegroundFailed(WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: NotifyTransferComponentDataSync
 * @tc.desc: NotifyTransferComponentDataSync
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyTransferComponentDataSync, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyTransferComponentDataSync");
    ASSERT_NE(window, nullptr);
    AAFwk::WantParams wantParams;
    AAFwk::WantParams reWantParams;
    ASSERT_EQ(WSErrorCode::WS_OK, window->NotifyTransferComponentDataSync(wantParams, reWantParams));
    window->Destroy();
}

/**
 * @tc.name: UpdateAvoidArea
 * @tc.desc: UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, UpdateAvoidArea, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdateAvoidArea");
    ASSERT_NE(window, nullptr);
    sptr<AvoidArea> avoidArea = new AvoidArea();
    avoidArea->topRect_ = {1, 0, 0, 0};
    avoidArea->leftRect_ = {0, 1, 0, 0};
    avoidArea->rightRect_ = {0, 0, 1, 0};
    avoidArea->bottomRect_ = {0, 0, 0, 1};
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM;
    ASSERT_EQ(WSError::WS_OK, window->UpdateAvoidArea(avoidArea, type));
    window->Destroy();
}

/**
 * @tc.name: DispatchKeyEventCallback
 * @tc.desc: DispatchKeyEventCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, DispatchKeyEventCallback, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("DispatchKeyEventCallback");
    ASSERT_NE(window, nullptr);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_VIRTUAL_MULTITASK);
    bool isConsumed = false;
    window->DispatchKeyEventCallback(keyEvent, isConsumed);
    ASSERT_FALSE(isConsumed);

    std::shared_ptr<MockInputEventConsumer> inputEventConsumer = std::make_shared<MockInputEventConsumer>();
    window->inputEventConsumer_ = inputEventConsumer;
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    window->DispatchKeyEventCallback(keyEvent, isConsumed);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    window->DispatchKeyEventCallback(keyEvent, isConsumed);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    window->inputEventConsumer_ = nullptr;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->DispatchKeyEventCallback(keyEvent, isConsumed);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEYCODE_ESCAPE);
    window->DispatchKeyEventCallback(keyEvent, isConsumed);
    window->Destroy();
}

/**
 * @tc.name: IsKeyboardEvent
 * @tc.desc: IsKeyboardEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, IsKeyboardEvent, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("IsKeyboardEvent");
    ASSERT_NE(window, nullptr);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_VIRTUAL_MULTITASK);
    ASSERT_FALSE(window->IsKeyboardEvent(keyEvent));

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    ASSERT_TRUE(window->IsKeyboardEvent(keyEvent));

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    ASSERT_TRUE(window->IsKeyboardEvent(keyEvent));

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_FN);
    ASSERT_TRUE(window->IsKeyboardEvent(keyEvent));

    window->Destroy();
}

/**
 * @tc.name: GetVSyncPeriod
 * @tc.desc: GetVSyncPeriod
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetVSyncPeriod, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetVSyncPeriod");
    ASSERT_NE(window, nullptr);

    auto vsyncStation = window->vsyncStation_;
    if (vsyncStation == nullptr) {
        vsyncStation = std::make_shared<VsyncStation>(DisplayId(0));
    }
    window->vsyncStation_ = nullptr;
    ASSERT_EQ(window->GetVSyncPeriod(), 0);

    window->vsyncStation_ = vsyncStation;
    window->GetVSyncPeriod();
    window->Destroy();
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, FlushFrameRate, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("FlushFrameRate");
    ASSERT_NE(window, nullptr);

    auto vsyncStation = window->vsyncStation_;
    if (vsyncStation == nullptr) {
        vsyncStation = std::make_shared<VsyncStation>(DisplayId(0));
    }
    window->vsyncStation_ = nullptr;
    window->FlushFrameRate(1, true);

    window->vsyncStation_ = vsyncStation;
    window->FlushFrameRate(1, true);
    window->Destroy();
}

/**
 * @tc.name: FindWindowById
 * @tc.desc: FindWindowById
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, FindWindowById, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("FindWindowById");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    ASSERT_EQ(window->FindWindowById(0), nullptr);

    window->windowSessionMap_.insert(
        {"test1", std::pair<int32_t, sptr<WindowSessionImpl>>(1, window)}
    );
    window->windowSessionMap_.insert(
        {"test2", std::pair<int32_t, sptr<WindowSessionImpl>>(2, window)}
    );
    ASSERT_EQ(window->FindWindowById(0), nullptr);

    window->windowSessionMap_.insert(
        {"test0", std::pair<int32_t, sptr<WindowSessionImpl>>(0, window)}
    );
    ASSERT_NE(window->FindWindowById(0), nullptr);
    window->Destroy();
}

/**
 * @tc.name: SetLayoutFullScreenByApiVersion
 * @tc.desc: SetLayoutFullScreenByApiVersion
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, SetLayoutFullScreenByApiVersion, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetLayoutFullScreenByApiVersion");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    ASSERT_EQ(window->SetLayoutFullScreenByApiVersion(true), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: SetSystemBarProperty
 * @tc.desc: SetSystemBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, SetSystemBarProperty, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetSystemBarProperty");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    SystemBarProperty property;
    ASSERT_EQ(window->SetSystemBarProperty(WindowType::APP_MAIN_WINDOW_END, property), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: SetSpecificBarProperty
 * @tc.desc: SetSpecificBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, SetSpecificBarProperty, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetSpecificBarProperty");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    SystemBarProperty property;
    ASSERT_EQ(window->SetSpecificBarProperty(WindowType::APP_MAIN_WINDOW_END, property), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo
 * @tc.desc: NotifyOccupiedAreaChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyOccupiedAreaChangeInfo, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyOccupiedAreaChangeInfo");
    ASSERT_NE(window, nullptr);

    auto listeners = GetListenerList<IOccupiedAreaChangeListener, MockIOccupiedAreaChangeListener>();
    ASSERT_NE(listeners.size(), 0);
    listeners.insert(listeners.begin(), nullptr);
    window->occupiedAreaChangeListeners_.insert({window->GetPersistentId(), listeners});

    sptr<OccupiedAreaChangeInfo> info = new (std::nothrow) OccupiedAreaChangeInfo();
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->NotifyOccupiedAreaChangeInfo(info);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->NotifyOccupiedAreaChangeInfo(info);

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    window->windowSessionMap_.insert(
        {"test1", std::pair<int32_t, sptr<WindowSessionImpl>>(window->GetPersistentId(), nullptr)}
    );
    window->NotifyOccupiedAreaChangeInfo(info);
    window->windowSessionMap_.clear();

    window->windowSessionMap_.insert(
        {"test1", std::pair<int32_t, sptr<WindowSessionImpl>>(window->GetPersistentId(), window)}
    );
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->NotifyOccupiedAreaChangeInfo(info);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->NotifyOccupiedAreaChangeInfo(info);
    window->Destroy();
}

/**
 * @tc.name: NotifyWindowStatusChange
 * @tc.desc: NotifyWindowStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyWindowStatusChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyWindowStatusChange");
    ASSERT_NE(window, nullptr);

    auto listeners = GetListenerList<IWindowStatusChangeListener, MockWindowStatusChangeListener>();
    ASSERT_NE(listeners.size(), 0);
    listeners.insert(listeners.begin(), nullptr);
    window->windowStatusChangeListeners_.insert({window->GetPersistentId(), listeners});

    WindowMode mode = WindowMode::WINDOW_MODE_FLOATING;
    window->state_ = WindowState::STATE_HIDDEN;
    window->property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    window->NotifyWindowStatusChange(mode);

    window->state_ = WindowState::STATE_FROZEN;
    window->property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    window->NotifyWindowStatusChange(mode);

    mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    window->NotifyWindowStatusChange(mode);

    mode = WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    window->NotifyWindowStatusChange(mode);

    mode = WindowMode::WINDOW_MODE_PIP;
    window->NotifyWindowStatusChange(mode);
    window->Destroy();
}

/**
 * @tc.name: UpdatePiPRect
 * @tc.desc: UpdatePiPRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, UpdatePiPRect, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdatePiPRect");
    ASSERT_NE(window, nullptr);

    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_FROZEN;
    Rect rect;
    window->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);

    window->hostSession_ = nullptr;
    window->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);
    window->Destroy();
}

/**
 * @tc.name: NotifyTransformChange
 * @tc.desc: NotifyTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyTransformChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyTransformChange");
    ASSERT_NE(window, nullptr);

    Transform transform;
    window->NotifyTransformChange(transform);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->NotifyTransformChange(transform);
    window->Destroy();
}

/**
 * @tc.name: SubmitNoInteractionMonitorTask
 * @tc.desc: SubmitNoInteractionMonitorTask
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, SubmitNoInteractionMonitorTask, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SubmitNoInteractionMonitorTask");
    ASSERT_NE(window, nullptr);

    IWindowNoInteractionListenerSptr listener = new (std::nothrow) MockWindowNoInteractionListener();
    window->SubmitNoInteractionMonitorTask(window->lastInteractionEventId_.load() + 1, listener);

    window->state_ = WindowState::STATE_SHOWN;
    window->SubmitNoInteractionMonitorTask(window->lastInteractionEventId_.load(), listener);

    window->state_ = WindowState::STATE_FROZEN;
    window->SubmitNoInteractionMonitorTask(window->lastInteractionEventId_.load(), listener);
    window->Destroy();
}

/**
 * @tc.name: RefreshNoInteractionTimeoutMonitor
 * @tc.desc: RefreshNoInteractionTimeoutMonitor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, RefreshNoInteractionTimeoutMonitor, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RefreshNoInteractionTimeoutMonitor");
    ASSERT_NE(window, nullptr);

    window->RefreshNoInteractionTimeoutMonitor();

    auto listeners = GetListenerList<IWindowStatusChangeListener, MockWindowStatusChangeListener>();
    ASSERT_NE(listeners.size(), 0);
    window->windowStatusChangeListeners_.insert({window->GetPersistentId(), listeners});
    window->RefreshNoInteractionTimeoutMonitor();
    window->Destroy();
}

/**
 * @tc.name: IsUserOrientation
 * @tc.desc: IsUserOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, IsUserOrientation, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("IsUserOrientation");
    ASSERT_NE(window, nullptr);

    ASSERT_FALSE(window->IsUserOrientation(Orientation::FOLLOW_DESKTOP));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_PORTRAIT));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_LANDSCAPE));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED));
    window->Destroy();
}

/**
 * @tc.name: WindowSessionCreateCheck
 * @tc.desc: WindowSessionCreateCheck
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, WindowSessionCreateCheck, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("WindowSessionCreateCheck");
    ASSERT_NE(window, nullptr);

    int32_t nullWindowTestId = 1001;
    int32_t nullPropertyId = 1002;
    int32_t displayId = 1003;
    int32_t cameraId = 1004;
    
    window->windowSessionMap_.clear();
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "nullWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(nullWindowTestId, nullptr)
        )
    );
    auto nullPropertyWindow = GetTestWindowImpl("nullPropertyWindow");
    ASSERT_NE(nullPropertyWindow, nullptr);
    nullPropertyWindow->property_ = nullptr;
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "nullPropertyWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(nullPropertyId, nullPropertyWindow)
        )
    );

    auto displayWindow = GetTestWindowImpl("displayWindow");
    ASSERT_NE(displayWindow, nullptr);
    displayWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_FREEZE_DISPLAY);
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "displayWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(displayId, displayWindow)
        )
    );
    ASSERT_EQ(window->WindowSessionCreateCheck(), WMError::WM_OK);

    window->windowSessionMap_.clear();
    auto cameraWindow = GetTestWindowImpl("cameraWindow");
    ASSERT_NE(cameraWindow, nullptr);
    cameraWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "cameraWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(cameraId, cameraWindow)
        )
    );
    ASSERT_EQ(window->WindowSessionCreateCheck(), WMError::WM_ERROR_REPEAT_OPERATION);
    window->Destroy();
    nullPropertyWindow->Destroy();
    displayWindow->Destroy();
    cameraWindow->Destroy();
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: NotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyForegroundInteractiveStatus, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyForegroundInteractiveStatus");
    ASSERT_NE(window, nullptr);

    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_SHOWN;
    window->NotifyForegroundInteractiveStatus(true);
    window->NotifyForegroundInteractiveStatus(false);

    window->state_ = WindowState::STATE_DESTROYED;
    window->NotifyForegroundInteractiveStatus(true);
    window->state_ = WindowState::STATE_FROZEN;
    window->NotifyForegroundInteractiveStatus(true);
    window->Destroy();
}

/**
 * @tc.name: InitUIContent
 * @tc.desc: InitUIContent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, InitUIContent, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("InitUIContent");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();

    OHOS::Ace::UIContentErrorCode aceRet = OHOS::Ace::UIContentErrorCode::NULL_WINDOW;
    window->isUIExtensionAbilityProcess_ = false;
    ASSERT_EQ(WMError::WM_OK,
        window->InitUIContent(
            "InitUIContent", nullptr, nullptr, WindowSetUIContentType::DEFAULT,
            BackupAndRestoreType::CONTINUATION, nullptr, aceRet
        )
    );

    window->uiContent_ = nullptr;
    window->isUIExtensionAbilityProcess_ = true;
    window->property_->SetExtensionFlag(false);
    ASSERT_EQ(WMError::WM_OK,
        window->InitUIContent(
            "InitUIContent", nullptr, nullptr, WindowSetUIContentType::DEFAULT,
            BackupAndRestoreType::CONTINUATION, nullptr, aceRet
        )
    );
    window->property_->SetExtensionFlag(true);
    ASSERT_EQ(WMError::WM_OK,
        window->InitUIContent(
            "InitUIContent", nullptr, nullptr, WindowSetUIContentType::RESTORE,
            BackupAndRestoreType::CONTINUATION, nullptr, aceRet
        )
    );
    ASSERT_EQ(WMError::WM_OK,
        window->InitUIContent(
            "InitUIContent", nullptr, nullptr, WindowSetUIContentType::BY_NAME,
            BackupAndRestoreType::CONTINUATION, nullptr, aceRet
        )
    );
    ASSERT_EQ(WMError::WM_OK,
        window->InitUIContent(
            "InitUIContent", nullptr, nullptr, WindowSetUIContentType::BY_ABC,
            BackupAndRestoreType::CONTINUATION, nullptr, aceRet
        )
    );
    window->Destroy();
}

/**
 * @tc.name: UpdateDecorEnableToAce
 * @tc.desc: UpdateDecorEnableToAce
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, UpdateDecorEnableToAce, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdateDecorEnableToAce");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    auto listeners = GetListenerList<IWindowChangeListener, MockWindowChangeListener>();
    sptr<MockWindowChangeListener> nullListener;
    listeners.insert(listeners.begin(), nullListener);
    window->windowChangeListeners_.insert({window->GetPersistentId(), listeners});
    window->windowSystemConfig_.freeMultiWindowSupport_ = false;
    window->UpdateDecorEnableToAce(false);

    window->uiContent_ = nullptr;
    window->UpdateDecorEnableToAce(false);
    window->Destroy();
}

/**
 * @tc.name: UpdateDecorEnable
 * @tc.desc: UpdateDecorEnable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, UpdateDecorEnable, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdateDecorEnable");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->UpdateDecorEnable(true, WindowMode::WINDOW_MODE_UNDEFINED);

    window->windowSystemConfig_.freeMultiWindowSupport_ = false;
    window->UpdateDecorEnable(true, WindowMode::WINDOW_MODE_FULLSCREEN);

    window->uiContent_ = nullptr;
    window->UpdateDecorEnable(true, WindowMode::WINDOW_MODE_FULLSCREEN);
    window->UpdateDecorEnable(false, WindowMode::WINDOW_MODE_FULLSCREEN);
    window->Destroy();
}

/**
 * @tc.name: NotifyModeChange
 * @tc.desc: NotifyModeChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyModeChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyModeChange");
    ASSERT_NE(window, nullptr);
    auto listeners = GetListenerList<IWindowChangeListener, MockWindowChangeListener>();
    sptr<MockWindowChangeListener> nullListener;
    listeners.insert(listeners.begin(), nullListener);
    window->windowChangeListeners_.insert({window->GetPersistentId(), listeners});

    window->NotifyModeChange(WindowMode::WINDOW_MODE_FULLSCREEN, true);
    window->Destroy();
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, SetRequestedOrientation, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetRequestedOrientation");
    ASSERT_NE(window, nullptr);
    window->property_->SetRequestedOrientation(Orientation::BEGIN);
    window->SetRequestedOrientation(Orientation::END);
    
    window->property_->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    window->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    
    window->property_->SetRequestedOrientation(Orientation::BEGIN);
    window->SetRequestedOrientation(Orientation::BEGIN);
    window->Destroy();
}

/**
 * @tc.name: GetRequestedOrientation
 * @tc.desc: GetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetRequestedOrientation, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetRequestedOrientation");
    ASSERT_NE(window, nullptr);
    window->property_ = nullptr;
    ASSERT_EQ(window->GetRequestedOrientation(), Orientation::UNSPECIFIED);
    window->Destroy();
}

/**
 * @tc.name: GetContentInfo
 * @tc.desc: GetContentInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetContentInfo, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetContentInfo");
    ASSERT_NE(window, nullptr);
    
    ASSERT_EQ(window->GetContentInfo(BackupAndRestoreType::CONTINUATION), "");
    ASSERT_EQ(window->GetContentInfo(BackupAndRestoreType::APP_RECOVERY), "");
    ASSERT_EQ(window->GetContentInfo(BackupAndRestoreType::NONE), "");
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->GetContentInfo(BackupAndRestoreType::NONE);
    window->Destroy();
}

/**
 * @tc.name: GetDecorHeight
 * @tc.desc: GetDecorHeight
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetDecorHeight, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetDecorHeight");
    ASSERT_NE(window, nullptr);
    int32_t height = -1;
    ASSERT_EQ(window->GetDecorHeight(height), WMError::WM_ERROR_NULLPTR);
    
    auto uiContent = std::make_unique<Ace::UIContentMocker>();
    EXPECT_CALL(*uiContent, GetContainerModalTitleHeight()).WillRepeatedly(Return(-1));
    window->uiContent_ = std::move(uiContent);
    ASSERT_EQ(window->GetDecorHeight(height), WMError::WM_DO_NOTHING);
    height = 1;
    window->GetDecorHeight(height);
    window->Destroy();
}

/**
 * @tc.name: GetTitleButtonArea
 * @tc.desc: GetTitleButtonArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetTitleButtonArea, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetTitleButtonArea");
    ASSERT_NE(window, nullptr);
    auto uiContent = std::make_unique<Ace::UIContentMocker>();
    EXPECT_CALL(*uiContent, GetContainerModalButtonsRect(testing::_, testing::_)).WillRepeatedly(Return(false));
    window->uiContent_ = std::move(uiContent);
    TitleButtonRect titleButtonRect;
    ASSERT_EQ(window->GetTitleButtonArea(titleButtonRect), WMError::WM_DO_NOTHING);
    window->Destroy();
}

/**
 * @tc.name: RegisterWindowTitleButtonRectChangeListener
 * @tc.desc: RegisterWindowTitleButtonRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, RegisterWindowTitleButtonRectChangeListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RegisterWindowTitleButtonRectChangeListener");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    sptr<IWindowTitleButtonRectChangedListener> listener =
        new (std::nothrow) MockWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);
    window->RegisterWindowTitleButtonRectChangeListener(listener);
    window->Destroy();
}

/**
 * @tc.name: UnregisterWindowTitleButtonRectChangeListener
 * @tc.desc: UnregisterWindowTitleButtonRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, UnregisterWindowTitleButtonRectChangeListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UnregisterWindowTitleButtonRectChangeListener");
    ASSERT_NE(window, nullptr);
    sptr<IWindowTitleButtonRectChangedListener> listener =
        new (std::nothrow) MockWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);
    window->UnregisterWindowTitleButtonRectChangeListener(listener);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->UnregisterWindowTitleButtonRectChangeListener(listener);
    window->Destroy();
}

/**
 * @tc.name: NotifyWindowTitleButtonRectChange
 * @tc.desc: NotifyWindowTitleButtonRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, NotifyWindowTitleButtonRectChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyWindowTitleButtonRectChange");
    ASSERT_NE(window, nullptr);
    auto listeners = GetListenerList<IWindowTitleButtonRectChangedListener,
        MockWindowTitleButtonRectChangedListener>();
    listeners.insert(listeners.begin(), nullptr);
    window->windowTitleButtonRectChangeListeners_.insert({window->GetPersistentId(), listeners});
    TitleButtonRect titleButtonRect;
    window->NotifyWindowTitleButtonRectChange(titleButtonRect);
    window->Destroy();
}

/**
 * @tc.name: RegisterWindowRectChangeListener
 * @tc.desc: RegisterWindowRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, RegisterWindowRectChangeListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RegisterWindowRectChangeListener");
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->RegisterWindowRectChangeListener(nullptr));

    sptr<IWindowRectChangeListener> listener = new (std::nothrow) MockWindowRectChangeListener();
    ASSERT_NE(listener, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));

    window->hostSession_ = nullptr;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    window->Destroy();
}

}
} // namespace Rosen
} // namespace OHOS
