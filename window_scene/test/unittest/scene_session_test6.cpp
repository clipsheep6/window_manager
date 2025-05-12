/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <pointer_event.h>

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"

#include "screen_manager.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/system_session.h"
#include <ui/rs_surface_node.h>
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
namespace {
std::string g_errlog;
void ScreenSessionLogCallback(const LogType type,
                              const LogLevel level,
                              const unsigned int domain,
                              const char* tag,
                              const char* msg)
{
    g_errlog = msg;
}
} // namespace

class SceneSessionTest6 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest6::SetUpTestCase() {}

void SceneSessionTest6::TearDownTestCase() {}

void SceneSessionTest6::SetUp() {}

void SceneSessionTest6::TearDown() {}

namespace {

/**
 * @tc.name: RegisterNotifySurfaceBoundsChangeFunc
 * @tc.desc: RegisterNotifySurfaceBoundsChangeFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, RegisterNotifySurfaceBoundsChangeFunc01, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    constexpr int sessionId = 10001;
    sceneSession->RegisterNotifySurfaceBoundsChangeFunc(sessionId, nullptr);
    ASSERT_EQ(nullptr, sceneSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);

    auto task = [](const WSRect& rect, bool isGlobal, bool needFlush) {};
    sceneSession->RegisterNotifySurfaceBoundsChangeFunc(sessionId, std::move(task));
    ASSERT_NE(nullptr, sceneSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);

    sceneSession->UnregisterNotifySurfaceBoundsChangeFunc(sessionId);
    ASSERT_EQ(nullptr, sceneSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);
}

/**
 * @tc.name: NotifyUpdateGravity
 * @tc.desc: NotifyUpdateGravity
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, NotifyUpdateGravity01, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> subSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    int32_t subSessionId = subSession->GetPersistentId();

    sptr<MainSession> mainSession = sptr<MainSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, mainSession);

    constexpr int sessionId = 10001;
    auto task = [](const WSRect& rect, bool isGlobal, bool needFlush) {};
    mainSession->RegisterNotifySurfaceBoundsChangeFunc(sessionId, std::move(task));
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[sessionId]);

    subSession->isFollowParentLayout_ = false;
    mainSession->RegisterNotifySurfaceBoundsChangeFunc(subSessionId, std::move(task));
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    subSession->isFollowParentLayout_ = true;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    sptr<MoveDragController> followController =
        sptr<MoveDragController>::MakeSptr(subSessionId, subSession->GetWindowType());
    ASSERT_NE(nullptr, followController);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_NE(nullptr, surfaceNode);
    subSession->surfaceNode_ = nullptr;
    subSession->moveDragController_ = nullptr;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    subSession->surfaceNode_ = surfaceNode;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);

    subSession->moveDragController_ = followController;
    mainSession->NotifyUpdateGravity();
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSessionId]);
}

/**
 * @tc.name: GetSceneSessionById
 * @tc.desc: GetSceneSessionById
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, GetSceneSessionById01, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> findSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sceneSession->specificCallback_ = nullptr;
    sptr<SceneSession> ret = sceneSession->GetSceneSessionById(findSession->GetPersistentId());
    ASSERT_EQ(nullptr, ret);

    sptr<SceneSession::SpecificSessionCallback> callBack = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(nullptr, callBack);
    sceneSession->specificCallback_ = callBack;
    ret = sceneSession->GetSceneSessionById(findSession->GetPersistentId());
    ASSERT_EQ(nullptr, ret);

    auto task = [&findSession](int32_t persistentId) { return findSession; };
    callBack->onGetSceneSessionByIdCallback_ = task;
    ret = sceneSession->GetSceneSessionById(findSession->GetPersistentId());
    ASSERT_EQ(findSession->GetPersistentId(), ret->GetPersistentId());
}

/**
 * @tc.name: SetFollowParentRectFunc
 * @tc.desc: SetFollowParentRectFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentRectFunc01, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sceneSession->SetFollowParentRectFunc(nullptr);
    ASSERT_EQ(nullptr, sceneSession->followParentRectFunc_);

    NotifyFollowParentRectFunc func = [](bool isFollow) {};
    sceneSession->SetFollowParentRectFunc(std::move(func));
    ASSERT_NE(nullptr, sceneSession->followParentRectFunc_);
}

/**
 * @tc.name: SetFollowParentWindowLayoutEnabled
 * @tc.desc: SetFollowParentWindowLayoutEnabled01, check the param
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentWindowLayoutEnabled01, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    WSError ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    sceneSession->property_ = property;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    property->subWindowLevel_ = 100;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);

    property->subWindowLevel_ = 1;
    ret = sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: SetFollowParentWindowLayoutEnabled
 * @tc.desc: SetFollowParentWindowLayoutEnabled02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentWindowLayoutEnabled02, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    property->subWindowLevel_ = 1;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->property_ = property;
    // test set isFollowParentLayout_
    sceneSession->isFollowParentLayout_ = false;
    sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_TRUE(sceneSession->isFollowParentLayout_);
    // test after set flag, call func
    bool isCall = false;
    NotifyFollowParentRectFunc func = [&isCall](bool isFollow) { isCall = true; };
    sceneSession->SetFollowParentRectFunc(std::move(func));
    ASSERT_NE(nullptr, sceneSession->followParentRectFunc_);
    sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_TRUE(isCall);
}

/**
 * @tc.name: SetFollowParentWindowLayoutEnabled
 * @tc.desc: SetFollowParentWindowLayoutEnabled03, test register callback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, SetFollowParentWindowLayoutEnabled03, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->subWindowLevel_ = 1;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->property_ = property;

    sptr<MainSession> parentSession = sptr<MainSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, parentSession);

    sceneSession->parentSession_ = parentSession;
    sceneSession->SetFollowParentWindowLayoutEnabled(true);
    ASSERT_NE(nullptr, parentSession->notifySurfaceBoundsChangeFuncMap_[sceneSession->GetPersistentId()]);
    WSRect rect;
    parentSession->NotifySubAndDialogFollowRectChange(rect, false, false);

    sceneSession->SetFollowParentWindowLayoutEnabled(false);
    ASSERT_EQ(nullptr, parentSession->notifySurfaceBoundsChangeFuncMap_[sceneSession->GetPersistentId()]);
}

/**
 * @tc.name: NotifyKeyboardAnimationCompleted
 * @tc.desc: NotifyKeyboardAnimationCompleted
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, NotifyKeyboardAnimationCompleted, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = property;
    sceneSession->sessionStage_ = nullptr;
    bool isShowAnimation = true;
    WSRect beginRect = { 0, 2720, 1260, 1020 };
    WSRect endRect = { 0, 1700, 1260, 1020 };
    sceneSession->NotifyKeyboardAnimationCompleted(isShowAnimation, beginRect, endRect);
    sceneSession->sessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    EXPECT_NE(nullptr, sceneSession->sessionStage_);

    sceneSession->NotifyKeyboardDidShowRegistered(true);
    sceneSession->NotifyKeyboardAnimationCompleted(isShowAnimation, beginRect, endRect);

    isShowAnimation = false;
    beginRect = { 0, 1700, 1260, 1020 };
    endRect = { 0, 2720, 1260, 1020 };
    sceneSession->NotifyKeyboardAnimationCompleted(isShowAnimation, beginRect, endRect);
    sceneSession->NotifyKeyboardDidHideRegistered(true);
    sceneSession->NotifyKeyboardAnimationCompleted(isShowAnimation, beginRect, endRect);
}

/**
 * @tc.name: UpdateNewSizeForPCWindow
 * @tc.desc: UpdateNewSizeForPCWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, UpdateNewSizeForPCWindow, Function | SmallTest | Level1)
{
    LOG_SetCallback(ScreenSessionLogCallback);
    SessionInfo info;
    info.abilityName_ = "UpdateNewSizeForPCWindow";
    info.bundleName_ = "UpdateNewSizeForPCWindow";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->UpdateNewSizeForPCWindow(true);
    EXPECT_TRUE(g_errlog.find("dip change do nothing.") != std::string::npos);
}

/**
 * @tc.name: CalcNewWindowRectIfNeed
 * @tc.desc: CalcNewWindowRectIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, CalcNewWindowRectIfNeed, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "CalcNewWindowRectIfNeed";
    info.bundleName_ = "CalcNewWindowRectIfNeed";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetLastLimitsVpr(1.9);
    sceneSession->property_ = property;
    DMRect availableArea = { 0, 0, 1920, 1080 };
    float newVpr = 2.85;
    WSRect winRect = { 0, 0, 800, 600 };
    sceneSession->CalcNewWindowRectIfNeed(availableArea, newVpr, winRect);
    WSRect result = { 0, 0, 1200, 900 };
    ASSERT_EQ(result, winRect);
}

/**
 * @tc.name: GetSystemAvoidArea
 * @tc.desc: GetSystemAvoidArea function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, GetSystemAvoidArea, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetSystemAvoidArea";
    info.bundleName_ = "GetSystemAvoidArea";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(session, nullptr);
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_EQ(WindowMode::WINDOW_MODE_FLOATING, session->GetSessionProperty()->GetWindowMode());
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);

    SystemSessionConfig systemConfig;
    systemConfig.windowUIType_ = WindowUIType::PHONE_WINDOW;
    session->SetSystemConfig(systemConfig);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    session->GetSessionProperty()->SetDisplayId(2025);
    session->SetIsMidScene(false);
    EXPECT_EQ(session->GetIsMidScene(), false);

    WSRect rect;
    AvoidArea avoidArea;
    session->GetSystemAvoidArea(rect, avoidArea);
    int32_t height = session->GetStatusBarHeight();
    EXPECT_EQ(height, avoidArea.topRect_.height_);
}

/**
 * @tc.name: NotifyWindowAttachStateListenerRegistered
 * @tc.desc: NotifyWindowAttachStateListenerRegistered about session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, NotifyWindowAttachStateListenerRegistered_session, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->NotifyWindowAttachStateListenerRegistered(true);
    EXPECT_EQ(sceneSession->needNotifyAttachState_, true);
    sceneSession->NotifyWindowAttachStateListenerRegistered(false);
    EXPECT_EQ(sceneSession->needNotifyAttachState_, false);
}

/**
 * @tc.name: UpdateFollowScreenChange
 * @tc.desc: UpdateFollowScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, UpdateFollowScreenChange, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    bool isFollowScreenChange = true;
    sceneSession->specificCallback_ = nullptr;
    WSError ret = sceneSession->UpdateFollowScreenChange(isFollowScreenChange);
    EXPECT_EQ(WSError::WS_OK, ret);

    sptr<SceneSession::SpecificSessionCallback> callback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(nullptr, callback);
    sceneSession->specificCallback_ = callback;
    ret = sceneSession->UpdateFollowScreenChange(isFollowScreenChange);
    EXPECT_EQ(WSError::WS_OK, ret);

    auto task = [] (bool isFollowScreenChange) {};
    callback->onUpdateFollowScreenChange_ = task;
    ret = sceneSession->UpdateFollowScreenChange(isFollowScreenChange);
    EXPECT_EQ(WSError::WS_OK, ret);
    EXPECT_EQ(isFollowScreenChange, sceneSession->GetFollowScreenChange());
}

/**
 * @tc.name: RegisterFollowScreenChangeCallback
 * @tc.desc: RegisterFollowScreenChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, RegisterFollowScreenChangeCallback, Function | SmallTest | Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->specificCallback_ = nullptr;
    auto task = [] (bool isFollowScreenChange) {};
    sceneSession->RegisterFollowScreenChangeCallback(std::move(task));
    EXPECT_EQ(nullptr, sceneSession->specificCallback_);

    sptr<SceneSession::SpecificSessionCallback> callback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(nullptr, callback);
    sceneSession->specificCallback_ = callback;
    EXPECT_EQ(nullptr, callback->onUpdateFollowScreenChange_);
    sceneSession->RegisterFollowScreenChangeCallback(std::move(task));
    EXPECT_NE(nullptr, callback->onUpdateFollowScreenChange_);
}

/**
 * @tc.name: GetFollowScreenChange
 * @tc.desc: GetFollowScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, GetFollowScreenChange01, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    bool isFollowScreenChange = true;
    sceneSession->SetFollowScreenChange(isFollowScreenChange);
    bool res = sceneSession->GetFollowScreenChange();
    EXPECT_EQ(res, isFollowScreenChange);

    isFollowScreenChange = false;
    sceneSession->SetFollowScreenChange(isFollowScreenChange);
    res = sceneSession->GetFollowScreenChange();
    EXPECT_EQ(res, isFollowScreenChange);

    isFollowScreenChange = true;
    sceneSession->SetFollowScreenChange(isFollowScreenChange);
    res = sceneSession->GetFollowScreenChange();
    EXPECT_EQ(res, isFollowScreenChange);
}

/**
 * @tc.name: HandleActionUpdateFollowScreenChange
 * @tc.desc: test HandleActionUpdateFollowScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, HandleActionUpdateFollowScreenChange, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdateFollowScreenChange";
    info.bundleName_ = "HandleActionUpdateFollowScreenChange";
    info.isSystem_ = true;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetFollowScreenChange(true);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_FOLLOW_SCREEN_CHANGE;
    auto res = session->HandleActionUpdateFollowScreenChange(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    EXPECT_EQ(isFollowScreenChange, session->GetFollowScreenChange());
}
} // namespace
} // namespace Rosen
} // namespace OHOS