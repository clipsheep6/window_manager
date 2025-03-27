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

#include "application_context.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "session/host/include/main_session.h"
#include "session/host/include/keyboard_session.h"
#define private public
#define protected public
#include "session/host/include/scene_session.h"
#undef private
#undef protected
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "ui/rs_surface_node.h"
#include "window_helper.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionAnimationTest::SetUpTestCase()
{
}

void SceneSessionAnimationTest::TearDownTestCase()
{
}

void SceneSessionAnimationTest::SetUp()
{
}

void SceneSessionAnimationTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetWindowCornerRadiusCallback
 * @tc.desc: SetWindowCornerRadiusCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetWindowCornerRadiusCallback, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowCornerRadiusCallback";
    info.bundleName_ = "SetWindowCornerRadiusCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    NotifySetWindowCornerRadiusFunc func1 = [](float cornerRadius) {
        return;
    };
    sceneSession->SetWindowCornerRadiusCallback(std::move(func1));
    ASSERT_NE(nullptr, sceneSession->onSetWindowCornerRadiusFunc_);
}

/**
 * @tc.name: OnSetWindowCornerRadius
 * @tc.desc: OnSetWindowCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetWindowCornerRadius, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowCornerRadius";
    info.bundleName_ = "SetWindowCornerRadius";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(WSError::WS_OK, session->SetWindowCornerRadius(1.0f));

    NotifySetWindowCornerRadiusFunc func = [](float cornerRadius) {
        return;
    };
    session->onSetWindowCornerRadiusFunc_ = func;
    EXPECT_EQ(WSError::WS_OK, session->SetWindowCornerRadius(1.0f));
}

/**
 * @tc.name: AddSidebarBlur01
 * @tc.desc: AddSidebarBlur01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur01";
    info.bundleName_ = "AddSidebarBlur01";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);
    
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/**
 * @tc.name: AddSidebarBlur02
 * @tc.desc: AddSidebarBlur02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur02, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur02";
    info.bundleName_ = "AddSidebarBlur02";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
 
    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);
    
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());
 
    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);
 
    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);
 
    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);
 
    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/*
 * @c.name: AddSidebarBlur03
 * @c.desc: AddSidebarBlur03
 * @c.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur03, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur03";
    info.bundleName_ = "AddSidebarBlur03";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());
 
    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);
 
    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);
 
    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);
 
    session->AddSidebarBlur();
    float radiusOpenDark = session->blurRadiusValue_->Get();
    float saturationOpenDark = session->blurSaturationValue_->Get();
    float brightnessOpenDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_DARK, radiusOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_DARK, saturationOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_DARK, brightnessOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_DARK, colorOpenDark.AsArgbInt());
}

/**
 * @tc.name: AddSidebarBlur04
 * @tc.desc: AddSidebarBlur04
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur04, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur04";
    info.bundleName_ = "AddSidebarBlur04";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    float radiusOpenLight = session->blurRadiusValue_->Get();
    float saturationOpenLight = session->blurSaturationValue_->Get();
    float brightnessOpenLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_LIGHT, radiusOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_LIGHT, saturationOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT, brightnessOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT, colorOpenLight.AsArgbInt());
}

/**
 * @tc.name: SetSidebarBlur01
 * @tc.desc: SetSidebarBlur01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur01";
    info.bundleName_ = "SetSidebarBlur01";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);
    
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);
 
    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);
 
    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/**
 * @tc.name: SetSidebarBlur02
 * @tc.desc: SetSidebarBlur02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur02, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur02";
    info.bundleName_ = "SetSidebarBlur02";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);
    
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);
 
    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);
 
    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/**
 * @tc.name: SetSidebarBlur03
 * @tc.desc: SetSidebarBlur03
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur03, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur03";
    info.bundleName_ = "SetSidebarBlur03";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);
 
    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);
 
    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    session->SetSidebarBlur(false);
    float radiusCloseDark = session->blurRadiusValue_->Get();
    float saturationCloseDark = session->blurSaturationValue_->Get();
    float brightnessCloseDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorCloseDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, radiusCloseDark);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, saturationCloseDark);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, brightnessCloseDark);
    EXPECT_EQ(SIDEBAR_SNAPSHOT_MASKCOLOR_DARK, colorCloseDark.AsArgbInt());
    
    session->SetSidebarBlur(true);
    float radiusOpenDark = session->blurRadiusValue_->Get();
    float saturationOpenDark = session->blurSaturationValue_->Get();
    float brightnessOpenDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_DARK, radiusOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_DARK, saturationOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_DARK, brightnessOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_DARK, colorOpenDark.AsArgbInt());
}

/**
 * @tc.name: SetSidebarBlur04
 * @tc.desc: SetSidebarBlur04
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur04, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur04";
    info.bundleName_ = "SetSidebarBlur04";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
     
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->surfaceNode_ = surfaceNode;
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<
        AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);
 
    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);
 
    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    session->SetSidebarBlur(false);
    float radiusCloseLight = session->blurRadiusValue_->Get();
    float saturationCloseLight = session->blurSaturationValue_->Get();
    float brightnessCloseLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorCloseLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, radiusCloseLight);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, saturationCloseLight);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, brightnessCloseLight);
    EXPECT_EQ(SIDEBAR_SNAPSHOT_MASKCOLOR_LIGHT, colorCloseLight.AsArgbInt());
    
    session->SetSidebarBlur(true);
    float radiusOpenLight = session->blurRadiusValue_->Get();
    float saturationOpenLight = session->blurSaturationValue_->Get();
    float brightnessOpenLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_LIGHT, radiusOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_LIGHT, saturationOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT, brightnessOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT, colorOpenLight.AsArgbInt());
}
}
}
}