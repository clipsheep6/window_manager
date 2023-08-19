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
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int WAIT_SLEEP_TIME = 1;
    using ConfigItem = WindowSceneConfig::ConfigItem;
    ConfigItem ReadConfig(const std::string& xmlStr)
    {
        ConfigItem config;
        xmlDocPtr docPtr = xmlParseMemory(xmlStr.c_str(), xmlStr.length() + 1);
        if (docPtr == nullptr) {
            return config;
        }

        xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
        if (rootPtr == nullptr || rootPtr->name == nullptr ||
            xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
            xmlFreeDoc(docPtr);
            return config;
        }

        std::map<std::string, ConfigItem> configMap;
        config.SetValue(configMap);
        WindowSceneConfig::ReadConfig(rootPtr, *config.mapValue_);
        xmlFreeDoc(docPtr);
        return config;
    }
}
class SceneSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneSessionManagerTest::ssm_ = nullptr;

bool SceneSessionManagerTest::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};

void SceneSessionManagerTest::SetUpTestCase()
{
    ssm_ = new SceneSessionManager();
}

void SceneSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest::SetUp()
{
}

void SceneSessionManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetBrightness
 * @tc.desc: ScreenSesionManager set session brightness
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetBrightness, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result = SceneSessionManager::GetInstance().SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: SetGestureNavigaionEnabled
 * @tc.desc: SceneSessionManager set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetGestureNavigaionEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(callbackFunc_, nullptr);

    WMError result00 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result00, WMError::WM_DO_NOTHING);

    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(callbackFunc_);
    WMError result01 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, true);

    WMError result02 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, false);

    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(nullptr);
    WMError result03 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result03, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SceneSesionManager rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(WMError::WM_OK, SceneSessionManager::GetInstance().RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, SceneSessionManager::GetInstance().UnregisterWindowManagerAgent(
        type, windowManagerAgent));
}

/**
 * @tc.name: Init
 * @tc.desc: SceneSesionManager init 
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, Init, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->Init();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: LoadWindowSceneXml
 * @tc.desc: SceneSesionManager load window scene xml 
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, LoadWindowSceneXml, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->LoadWindowSceneXml();
    ASSERT_EQ(ret, 0);
}

// /**
//  * @tc.name: ConfigWindowSceneXml
//  * @tc.desc: SceneSesionManager config window scene xml run
//  * @tc.type: FUNC
//  */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSceneXml, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->ConfigWindowSceneXml(); 
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetSessionContinueState
 * @tc.desc: SceneSesionManager set session continue state
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetSessionContinueState, Function | SmallTest | Level3)
{
    MessageParcel *data = new MessageParcel();
    sptr <IRemoteObject> token = data->ReadRemoteObject();
    auto continueState = static_cast<ContinueState>(data->ReadInt32());
    WSError result01 = ssm_->SetSessionContinueState(token, continueState);
    WSError result02 = ssm_->SetSessionContinueState(nullptr, continueState);
    ASSERT_EQ(result01, WSError::WS_ERROR_INVALID_PERMISSION);
    ASSERT_EQ(result02, WSError::WS_ERROR_INVALID_PARAM);
}

/*
 * @tc.name: ConfigWindowSizeLimits01
 * @tc.desc: call ConfigWindowSizeLimits and check the systemConfig_.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowSizeLimits01, Function | SmallTest | Level3)
{
    std::string xmlStr = "<?xml version='1.0' encoding=\"utf-8\"?>"
        "<Configs>"
        "<mainWindowSizeLimits>"
        "<miniWidth>10</miniWidth>"
        "<miniHeight>20</miniHeight>"
        "</mainWindowSizeLimits>"
        "<subWindowSizeLimits>"
        "<miniWidth>30</miniWidth>"
        "<miniHeight>40</miniHeight>"
        "</subWindowSizeLimits>"
        "</Configs>";
    WindowSceneConfig::config_ = ReadConfig(xmlStr);
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    sceneSessionManager->ConfigWindowSizeLimits();
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniWidthOfMainWindow_, 10);
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniHeightOfMainWindow_, 20);
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniWidthOfSubWindow_, 30);
    ASSERT_EQ(sceneSessionManager->systemConfig_.miniHeightOfSubWindow_, 40);
}

/**
 * @tc.name: ConfigDecor
 * @tc.desc: SceneSesionManager config decor
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigDecor, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem* item = new WindowSceneConfig::ConfigItem;
    int ret = 0;
    ssm_->ConfigDecor(*item);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigWindowEffect
 * @tc.desc: SceneSesionManager config window effect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowEffect, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem* item = new WindowSceneConfig::ConfigItem;
    int ret = 0;
    ssm_->ConfigWindowEffect(*item);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigAppWindowCornerRadius
 * @tc.desc: SceneSesionManager config app window coener radius
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigAppWindowCornerRadius, Function | SmallTest | Level3)
{
    float out = 0.0f;
    WindowSceneConfig::ConfigItem item01;
    item01.SetValue(new std::string("ConfigAppWindowCornerRadius"));
    bool result01 = ssm_->ConfigAppWindowCornerRadius(item01, out);
    ASSERT_EQ(result01, true);

    WindowSceneConfig::ConfigItem item02;
    item02.SetValue(new string("defaultCornerRadiusS"));
    bool result02 = ssm_->ConfigAppWindowCornerRadius(item02, out);
    ASSERT_EQ(result02, false);
    ASSERT_EQ(out, 24.0f);
}

/**
 * @tc.name: ConfigAppWindowShadow
 * @tc.desc: SceneSesionManager config app window shadow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigAppWindowShadow, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem item;
    WindowSceneConfig::ConfigItem shadowConfig;
    WindowShadowConfig outShadow;
    std::vector<float> floatTest = {0.0f, 0.1f, 0.2f, 0.3f};
    bool result01 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result01, true);

    item.SetValue(floatTest);
    shadowConfig.SetValue({{"radius", item}});
    bool result02 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result02, false);
    shadowConfig.SetValue({{"alpha", item}});
    bool result03 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result03, false);
    shadowConfig.SetValue({{"sffsetY", item}});
    bool result04 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result04, true);
    shadowConfig.SetValue({{"sffsetX", item}});
    bool result05 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result05, false);

    item.SetValue(new std::string("color"));
    shadowConfig.SetValue({{"color", item}});
    bool result06 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result06, false);
}

/**
 * @tc.name: ConfigKeyboardAnimation
 * @tc.desc: SceneSesionManager config keyboard animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigKeyboardAnimation, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem animationConfig;
    WindowSceneConfig::ConfigItem itemCurve;
    WindowSceneConfig::ConfigItem itemDurationIn;
    WindowSceneConfig::ConfigItem itemDurationOut;
    std::vector<int> curve = {39};
    std::vector<int> durationIn = {39};
    std::vector<int> durationOut = {39};

    itemCurve.SetValue(curve);
    itemCurve.SetValue({{"curve", itemCurve}});
    itemDurationIn.SetValue(durationIn);
    itemDurationIn.SetValue({{"durationIn", itemDurationIn}});
    itemDurationOut.SetValue(durationOut);
    itemDurationOut.SetValue({{"durationOut", itemDurationOut}});
    animationConfig.SetValue({{"timing", itemCurve}, {"timing", itemDurationIn}, {"timing", itemDurationOut}});
    
    int ret = 0;
    ssm_->ConfigKeyboardAnimation(animationConfig);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigWindowAnimation
 * @tc.desc: SceneSesionManager config window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigWindowAnimation, Function | SmallTest | Level3)
{
    WindowSceneConfig::ConfigItem windowAnimationConfig;
    WindowSceneConfig::ConfigItem item;
    std::vector<float> opacity = {0.1f};
    std::vector<float> translate = {0.1f, 0.2f};
    std::vector<float> rotation = {0.1f, 0.2f, 0.3f, 0.4f};
    std::vector<float> scale = {0.1f, 0.2f};
    std::vector<int> duration = {39};

    item.SetValue(opacity);
    windowAnimationConfig.SetValue({{"opacity", item}});
    int ret = 0;
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(rotation);
    windowAnimationConfig.SetValue({{"rotation", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(translate);
    windowAnimationConfig.SetValue({{"translate", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(scale);
    windowAnimationConfig.SetValue({{"scale", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(duration);
    item.SetValue({{"duration", item}});
    windowAnimationConfig.SetValue({{"timing", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);

    item.SetValue(duration);
    item.SetValue({{"curve", item}});
    windowAnimationConfig.SetValue({{"timing", item}});
    ssm_->ConfigWindowAnimation(windowAnimationConfig);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ConfigStartingWindowAnimation
 * @tc.desc: SceneSesionManager config start window animation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, ConfigStartingWindowAnimation, Function | SmallTest | Level3)
{
    std::vector<float> midFloat = {0.1f};
    std::vector<int> midInt = {1};
    WindowSceneConfig::ConfigItem middleFloat;
    middleFloat.SetValue(midFloat);
    WindowSceneConfig::ConfigItem middleInt;
    middleInt.SetValue(midInt);
    WindowSceneConfig::ConfigItem curve;
    curve.SetValue(midFloat);
    curve.SetValue({{"curve", curve}});
    WindowSceneConfig::ConfigItem enableConfigItem;
    enableConfigItem.SetValue(false);
    std::map<std::string, WindowSceneConfig::ConfigItem> midMap = {{"duration", middleInt}, {"curve", curve}};
    WindowSceneConfig::ConfigItem timing;
    timing.SetValue(midMap);
    std::map<std::string, WindowSceneConfig::ConfigItem> middleMap = {{"enable", enableConfigItem}, 
        {"timing", timing}, {"opacityStart", middleFloat}, {"opacityEnd", middleFloat}};
    WindowSceneConfig::ConfigItem configItem;
    configItem.SetValue(middleMap);
    int ret = 0;
    ssm_->ConfigStartingWindowAnimation(configItem);
    ASSERT_EQ(ret, 0);
    midMap.clear();
    middleMap.clear();
}

/**
 * @tc.name: CreateCurve
 * @tc.desc: SceneSesionManager create curve
 * @tc.type: FUNC
 */
 HWTEST_F(SceneSessionManagerTest, CreateCurve, Function | SmallTest | Level3)
 {
    WindowSceneConfig::ConfigItem curveConfig;
    std::string nodeName;
    std::string result01 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result01, "easeOut");

    curveConfig.SetValue(new std::string("userName"));
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result02 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result02, "easeOut");

    curveConfig.SetValue(new std::string("interactiveSpring"));
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result03 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result03, "interactiveSpring");

    curveConfig.SetValue(new std::string("cubic"));
    curveConfig.SetValue({{"name", curveConfig}});
    nodeName = "windowAnimation";
    std::string result04 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result04, "cubic");
 }

}
} // namespace Rosen
} // namespace OHOS

