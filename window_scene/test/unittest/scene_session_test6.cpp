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

#include <gtest/gtest.h>
#include <pointer_event.h>

#include "common/include/session_permission.h"
#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"

#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "screen_manager.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/system_session.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "wm_common.h"
#include "window_helper.h"
#include "ui/rs_surface_node.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SceneSessionTest6 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest6::SetUpTestCase()
{
}

void SceneSessionTest6::TearDownTestCase()
{
}

void SceneSessionTest6::SetUp()
{
}

void SceneSessionTest6::TearDown()
{
}

namespace {

/**
 * @tc.name: CheckKeyEventDispatch
 * @tc.desc: CheckKeyEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, CheckKeyEventDispatch, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "UpdateCameraWindowStatus";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SystemSession> sysSession;
    sysSession = new (std::nothrow) SystemSession(info, specificCallback_);
    EXPECT_NE(sysSession, nullptr);
    int ret = false;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    WSRect rect({0, 0, 0, 0});
    sysSession->winRect_ = rect;
    sysSession->SetVisible(false);
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->SetVisible(true);
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->winRect_.width_ = 1;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->winRect_.height_ = 1;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckKeyEventDispatch2
 * @tc.desc: CheckKeyEventDispatch2
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest6, CheckKeyEventDispatch2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "UpdateCameraWindowStatus";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SystemSession> sysSession;
    sysSession = new (std::nothrow) SystemSession(info, specificCallback_);
    EXPECT_NE(sysSession, nullptr);
    int ret = false;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    WSRect rect({0, 0, 0, 0});
    sysSession->winRect_ = rect;
    sysSession->SetVisible(true);
    sysSession->winRect_.width_ = 1;
    sysSession->winRect_.height_ = 1;
    SessionInfo info2;
    info2.abilityName_ = "dialogAbilityName";
    info2.moduleName_ = "dialogModuleName";
    info2.bundleName_ = "dialogBundleName";
    sptr<Session> parentSession = new (std::nothrow) Session(info2);
    sysSession->parentSession_  = parentSession;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    parentSession->SetSessionState(SessionState::STATE_DISCONNECT);
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    parentSession->SetSessionState(SessionState::STATE_ACTIVE);
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    parentSession->SetSessionState(SessionState::STATE_DISCONNECT);
    sysSession->SetSessionState(SessionState::STATE_ACTIVE);
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);
    
    parentSession->SetSessionState(SessionState::STATE_ACTIVE);
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, true);
}

}
}
}