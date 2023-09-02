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
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "context.h"

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

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest::SetUpTestCase()
{
}

void SceneSessionManagerTest::TearDownTestCase()
{
}

void SceneSessionManagerTest::SetUp()
{
    ssm_ = new SceneSessionManager();
}

void SceneSessionManagerTest::TearDown()
{
    ssm_ = nullptr;
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
    delete sceneSession;
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
 * @tc.name: DumpSessionAll
 * @tc.desc: ScreenSesionManager dump all session info
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, DumpSessionAll, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest";
    sessionInfo.abilityName_ = "DumpSessionAll";
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    std::vector<std::string> infos;
    WSError result = ssm_->DumpSessionAll(infos);
    ASSERT_EQ(WSError::WS_OK, result);
    ASSERT_FALSE(infos.empty());
}

/**
 * @tc.name: DumpSessionWithId
 * @tc.desc: ScreenSesionManager dump session with id
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, DumpSessionWithId, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    std::vector<std::string> infos;
    WSError result = ssm_->DumpSessionWithId(sceneSession->GetPersistentId(), infos);
    ASSERT_EQ(WSError::WS_OK, result);
    ASSERT_FALSE(infos.empty());
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

/**
 * @tc.name: ConfigWindowSceneXml
 * @tc.desc: SceneSesionManager config window scene xml run
 * @tc.type: FUNC
 */
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
    WSError result02 = ssm_->SetSessionContinueState(nullptr, continueState);
    WSError result01 = ssm_->SetSessionContinueState(token, continueState);
    ASSERT_EQ(result02, WSError::WS_ERROR_INVALID_PARAM);
    ASSERT_EQ(result01, WSError::WS_ERROR_INVALID_PARAM);
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
    std::string str = "defaultCornerRadiusM";
    item01.SetValue(str);
    bool result01 = ssm_->ConfigAppWindowCornerRadius(item01, out);
    ASSERT_EQ(result01, true);

    WindowSceneConfig::ConfigItem item02;
    item02.SetValue(new string("defaultCornerRadiusS"));
    bool result02 = ssm_->ConfigAppWindowCornerRadius(item02, out);
    ASSERT_EQ(result02, false);
    ASSERT_EQ(out, 12.0f);
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
    ASSERT_EQ(result05, true);

    item.SetValue(new std::string("color"));
    shadowConfig.SetValue({{"color", item}});
    bool result06 = ssm_->ConfigAppWindowShadow(shadowConfig, outShadow);
    ASSERT_EQ(result06, true);
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

    std::string value02 = "userName";
    curveConfig.SetValue(value02);
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result02 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result02, "easeOut");

    std::string value03 = "interactiveSpring";
    curveConfig.SetValue(value03);
    curveConfig.SetValue({{"name", curveConfig}});
    std::string result03 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result03, "easeOut");

    std::string value04 = "cubic";
    curveConfig.SetValue(value04);
    curveConfig.SetValue({{"name", curveConfig}});
    nodeName = "windowAnimation";
    std::string result04 = ssm_->CreateCurve(curveConfig, nodeName);
    ASSERT_EQ(result04, "easeOut");
}

/**
 * @tc.name: SetRootSceneContext
 * @tc.desc: SceneSesionManager set root scene context
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetRootSceneContext, Function | SmallTest | Level3)
{
    int ret = 0;
    std::weak_ptr<AbilityRuntime::Context> contextWeakPtr;
    ssm_->SetRootSceneContext(contextWeakPtr);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetRootSceneSession
 * @tc.desc: SceneSesionManager get root scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetRootSceneSession, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->GetRootSceneSession();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetSceneSession
 * @tc.desc: SceneSesionManager get scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 65535;
    ASSERT_EQ(ssm_->GetSceneSession(persistentId), nullptr);
}

/**
 * @tc.name: GetSceneSessionByName
 * @tc.desc: SceneSesionManager get scene session by name
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSessionByName, Function | SmallTest | Level3)
{
    std::string bundleName = "movie";
    std::string moduleName = "button";
    std::string abilityName = "userAccess";
    ASSERT_EQ(ssm_->GetSceneSessionByName(bundleName, moduleName, abilityName, 0), nullptr);
}

/**
 * @tc.name: GetSceneSessionVectorByType
 * @tc.desc: SceneSesionManager get scene session vector by type
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSceneSessionVectorByType, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->GetSceneSessionVectorByType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateParentSession
 * @tc.desc: SceneSesionManager update parent session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateParentSession, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_EQ(ssm_->UpdateParentSession(scensession, nullptr), WSError::WS_ERROR_NULLPTR);
    ASSERT_EQ(ssm_->UpdateParentSession(nullptr, property), WSError::WS_ERROR_NULLPTR);
    ASSERT_EQ(ssm_->UpdateParentSession(scensession, property), WSError::WS_OK);
    delete scensession;
    delete property;
}

/**
 * @tc.name: RegisterInputMethodShownFunc
 * @tc.desc: SceneSesionManager register input method show func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterInputMethodShownFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->RegisterInputMethodShownFunc(nullptr);
    ASSERT_EQ(ret, 0);
    ssm_->RegisterInputMethodShownFunc(sceneSession);
    ASSERT_EQ(ret, 0);
    delete sceneSession;
}

/**
 * @tc.name: OnInputMethodShown
 * @tc.desc: SceneSesionManager on input method shown
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, OnInputMethodShown, Function | SmallTest | Level3)
{
    int32_t persistentId = 65535;
    int ret = 0;
    ssm_->OnInputMethodShown(persistentId);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RegisterInputMethodHideFunc
 * @tc.desc: SceneSesionManager register input method hide func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterInputMethodHideFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->RegisterInputMethodHideFunc(nullptr);
    ASSERT_EQ(ret, 0);
    ssm_->RegisterInputMethodHideFunc(sceneSession);
    ASSERT_EQ(ret, 0);
    delete sceneSession;
}

/**
 * @tc.name: SetAbilitySessionInfo
 * @tc.desc: SceneSesionManager set ability session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetAbilitySessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<OHOS::AAFwk::SessionInfo> ret = ssm_->SetAbilitySessionInfo(scensession);
    OHOS::AppExecFwk::ElementName retElementName = ret->want.GetElement();
    ASSERT_EQ(retElementName.GetAbilityName(), info.abilityName_);
    ASSERT_EQ(retElementName.GetBundleName(), info.bundleName_);
    delete scensession;
}

/**
 * @tc.name: PrepareTerminate
 * @tc.desc: SceneSesionManager prepare terminate
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, PrepareTerminate, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    bool isPrepareTerminate = false;
    ASSERT_EQ(WSError::WS_OK, ssm_->PrepareTerminate(persistentId, isPrepareTerminate));
}

/**
 * @tc.name: RequestSceneSessionBackground
 * @tc.desc: SceneSesionManager request scene session background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionBackground, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(scensession, isDelegator));
    delete scensession;
}

/**
 * @tc.name: DestroyDialogWithMainWindow
 * @tc.desc: SceneSesionManager destroy dialog with main window
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroyDialogWithMainWindow, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->DestroyDialogWithMainWindow(scensession));
    delete scensession;
}

/**
 * @tc.name: RequestSceneSessionDestruction
 * @tc.desc: SceneSesionManager request scene session destruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionDestruction, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionDestruction(scensession, needRemoveSession));
    delete scensession;
}

/**
 * @tc.name: AddClientDeathRecipient
 * @tc.desc: SceneSesionManager add client death recipient
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AddClientDeathRecipient, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    int ret = 0;
    ssm_->AddClientDeathRecipient(nullptr, scensession);
    ASSERT_EQ(ret, 0);
    delete scensession;
}

/**
 * @tc.name: DestroySpecificSession
 * @tc.desc: SceneSesionManager destroy specific session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroySpecificSession, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    int ret = 0;
    ssm_->DestroySpecificSession(remoteObject);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetCreateSpecificSessionListener
 * @tc.desc: SceneSesionManager set create specific session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetCreateSpecificSessionListener, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->SetCreateSpecificSessionListener(nullptr);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetGestureNavigationEnabledChangeListener
 * @tc.desc: SceneSesionManager set gesture navigation enabled change listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetGestureNavigationEnabledChangeListener, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->SetGestureNavigationEnabledChangeListener(nullptr);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: OnOutsideDownEvent
 * @tc.desc: SceneSesionManager on out side down event
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, OnOutsideDownEvent, Function | SmallTest | Level3)
{
    int32_t x = 32;
    int32_t y = 32;
    int ret = 0;
    ssm_->OnOutsideDownEvent(x, y);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifySessionTouchOutside
 * @tc.desc: SceneSesionManager notify session touch outside
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySessionTouchOutside, Function | SmallTest | Level3)
{
    int32_t x = 32;
    int32_t y = 32;
    int32_t action = MMI::PointerEvent::POINTER_ACTION_DOWN;
    int ret = 0;
    ssm_->NotifySessionTouchOutside(action, x, y);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetOutsideDownEventListener
 * @tc.desc: SceneSesionManager set outside down event listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetOutsideDownEventListener, Function | SmallTest | Level3)
{
    ProcessOutsideDownEventFunc func = [](int32_t x, int32_t y) {
        ssm_->OnOutsideDownEvent(x, y);
    };
    int ret = 0;
    ssm_->SetOutsideDownEventListener(func);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSession
 * @tc.desc: SceneSesionManager destroy and disconnect specific session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroyAndDisconnectSpecificSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    WSError result = ssm_->DestroyAndDisconnectSpecificSession(persistentId);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: GetWindowSceneConfig
 * @tc.desc: SceneSesionManager get window scene config
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetWindowSceneConfig, Function | SmallTest | Level3)
{
    int ret = 0;
    AppWindowSceneConfig appWindowSceneConfig_ = ssm_->GetWindowSceneConfig();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ProcessBackEvent
 * @tc.desc: SceneSesionManager process back event
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, ProcessBackEvent, Function | SmallTest | Level3)
{
    WSError result = ssm_->ProcessBackEvent();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: CleanUserMap
 * @tc.desc: SceneSesionManager clear user map
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CleanUserMap, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->CleanUserMap();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SwitchUser
 * @tc.desc: SceneSesionManager switch user
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SwitchUser, Function | SmallTest | Level3)
{
    int32_t oldUserId = 10086;
    int32_t newUserId = 10086;
    std::string fileDir;
    WSError result01 = ssm_->SwitchUser(oldUserId, newUserId, fileDir);
    ASSERT_EQ(result01, WSError::WS_DO_NOTHING);
    fileDir = "newFileDir";
    oldUserId = ssm_->GetCurrentUserId();
    WSError result02 = ssm_->SwitchUser(oldUserId, newUserId, fileDir);
    ASSERT_EQ(result02, WSError::WS_OK);
}

/**
 * @tc.name: PreHandleCollaborator
 * @tc.desc: SceneSesionManager prehandle collaborator
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, PreHandleCollaborator, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession = nullptr;
    ssm_->PreHandleCollaborator(scensession);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    scensession = nullptr;
    AppExecFwk::ApplicationInfo applicationInfo_;
    applicationInfo_.codePath = std::to_string(CollaboratorType::RESERVE_TYPE);
    AppExecFwk::AbilityInfo abilityInfo_;
    abilityInfo_.applicationInfo = applicationInfo_;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    scensession = nullptr;
    applicationInfo_.codePath = std::to_string(CollaboratorType::OTHERS_TYPE);
    abilityInfo_.applicationInfo = applicationInfo_;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    EXPECT_EQ(scensession->GetSessionInfo().want, nullptr);
    scensession = nullptr;
    info.want = std::make_shared<AAFwk::Want>();
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->PreHandleCollaborator(scensession);
    ASSERT_NE(scensession->GetSessionInfo().want, nullptr);
    delete scensession;
}

/**
 * @tc.name: CheckCollaboratorType
 * @tc.desc: SceneSesionManager check collborator type
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CheckCollaboratorType, Function | SmallTest | Level3)
{
    int32_t type_ = CollaboratorType::RESERVE_TYPE;
    EXPECT_TRUE(ssm_->CheckCollaboratorType(type_));
    type_ = CollaboratorType::OTHERS_TYPE;
    EXPECT_TRUE(ssm_->CheckCollaboratorType(type_));
    type_ = CollaboratorType::DEFAULT_TYPE;
    ASSERT_FALSE(ssm_->CheckCollaboratorType(type_));
}

/**
 * @tc.name: NotifyUpdateSessionInfo
 * @tc.desc: SceneSesionManager notify update session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyUpdateSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifyUpdateSessionInfo(nullptr);
    ASSERT_EQ(scensession->GetSessionInfo().want, nullptr);
    ssm_->NotifyUpdateSessionInfo(scensession);
    int32_t collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    int32_t persistentId_ = 10086;
    ssm_->NotifyMoveSessionToForeground(collaboratorType_, persistentId_);
    ssm_->NotifyClearSession(collaboratorType_, persistentId_);
    delete scensession;
}

/**
 * @tc.name: NotifySessionCreate
 * @tc.desc: SceneSesionManager notify session create
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifySessionCreate, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionCreate(nullptr, info);
    EXPECT_EQ(info.want, nullptr);
    ssm_->NotifySessionCreate(scensession, info);
    info.want = std::make_shared<AAFwk::Want>();
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionCreate(scensession, info);
    EXPECT_NE(info.want, nullptr);
    AppExecFwk::AbilityInfo aInfo_;
    sptr<AAFwk::SessionInfo> abilitySessionInfo_ = new AAFwk::SessionInfo();
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo_ = std::make_shared<AppExecFwk::AbilityInfo>(aInfo_);
    int32_t collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    ssm_->NotifyLoadAbility(collaboratorType_, abilitySessionInfo_, abilityInfo_);
    delete scensession;
}

/**
 * @tc.name: QueryAbilityInfoFromBMS
 * @tc.desc: SceneSesionManager QueryAbilityInfoFromBMS NotifyStartAbility
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, QueryAbilityInfoFromBMS, Function | SmallTest | Level3)
{
    const int32_t uId_ = 32;
    SessionInfo sessionInfo_;
    sessionInfo_.bundleName_ = "BundleName";
    sessionInfo_.abilityName_ = "AbilityName";
    sessionInfo_.moduleName_ = "ModuleName";
    AppExecFwk::AbilityInfo abilityInfo_;
    int32_t collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    ssm_->QueryAbilityInfoFromBMS(uId_, sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    EXPECT_EQ(sessionInfo_.want, nullptr);
    ssm_->Init();
    ssm_->QueryAbilityInfoFromBMS(uId_, sessionInfo_.bundleName_, sessionInfo_.abilityName_, sessionInfo_.moduleName_);
    ssm_->NotifyStartAbility(collaboratorType_, sessionInfo_);
    sessionInfo_.want = std::make_shared<AAFwk::Want>();
    collaboratorType_ = CollaboratorType::OTHERS_TYPE;
    ssm_->NotifyStartAbility(collaboratorType_, sessionInfo_);
    ASSERT_NE(sessionInfo_.want, nullptr);
}

/**
 * @tc.name: IsSessionClearable
 * @tc.desc: SceneSesionManager is session clearable
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, IsSessionClearable, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(nullptr));
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    AppExecFwk::AbilityInfo abilityInfo_;
    abilityInfo_.excludeFromMissions = true;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    abilityInfo_.excludeFromMissions = false;
    abilityInfo_.unclearableMission = true;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    abilityInfo_.unclearableMission = false;
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo_);
    info.lockedState = true;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_TRUE(ssm_->IsSessionClearable(scensession));
    info.lockedState = false;
    info.isSystem_ = true;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_FALSE(ssm_->IsSessionClearable(scensession));
    info.isSystem_ = false;
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_TRUE(ssm_->IsSessionClearable(scensession));
    delete scensession;
}

/**
 * @tc.name: UpdateProperty
 * @tc.desc: SceneSesionManager update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateProperty, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE;
    WSError result = ssm_->UpdateProperty(property, action);
    ASSERT_EQ(result, WSError::WS_OK);
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->UpdatePropertyRaiseEnabled(property, scensession);
    delete scensession;
    delete property;
}

/**
 * @tc.name: HandleUpdateProperty01
 * @tc.desc: SceneSesionManager handle update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleUpdateProperty01, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action;
    action = WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    WSError result = ssm_->UpdateProperty(property, action);
    EXPECT_EQ(result, WSError::WS_OK);
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    delete scensession;
    delete property;
}

/**
 * @tc.name: HandleUpdateProperty02
 * @tc.desc: SceneSesionManager handle update property
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleUpdateProperty02, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action;
    action = WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_FLAGS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_MODE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED;
    ssm_->HandleUpdateProperty(property, action, scensession);
    action = WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED;
    ssm_->HandleUpdateProperty(property, action, scensession);
    WSError result = ssm_->UpdateProperty(property, action);
    EXPECT_EQ(result, WSError::WS_OK);
    action = WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE;
    ssm_->HandleUpdateProperty(property, action, scensession);
    delete scensession;
    delete property;
}

/**
 * @tc.name: HandleTurnScreenOn
 * @tc.desc: SceneSesionManager handle turn screen on and keep screen on
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleTurnScreenOn, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE;
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ssm_->HandleTurnScreenOn(scensession);
    bool requireLock = true;
    ssm_->HandleKeepScreenOn(scensession, requireLock);
    requireLock = false;
    ssm_->HandleKeepScreenOn(scensession, requireLock);
    WSError result = ssm_->UpdateProperty(property, action);
    ASSERT_EQ(result, WSError::WS_OK);
    delete scensession;
    delete property;
}

/**
 * @tc.name: UpdateHideNonSystemFloatingWindows
 * @tc.desc: SceneSesionManager update hide non system floating windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateHideNonSystemFloatingWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    ssm_->UpdateForceHideState(scensession, property, true);
    property = new WindowSessionProperty();
    ssm_->UpdateHideNonSystemFloatingWindows(property, scensession);
    property->SetHideNonSystemFloatingWindows(true);
    ssm_->UpdateForceHideState(scensession, property, true);
    ssm_->UpdateForceHideState(scensession, property, false);
    property->SetHideNonSystemFloatingWindows(false);
    property->SetFloatingWindowAppType(true);
    ssm_->UpdateForceHideState(scensession, property, true);
    ssm_->UpdateForceHideState(scensession, property, false);
    uint32_t result = property->GetModeSupportInfo();
    ASSERT_EQ(result, WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
    delete scensession;
    delete property;
}

/**
 * @tc.name: UpdateBrightness
 * @tc.desc: SceneSesionManager update brightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateBrightness, Function | SmallTest | Level3)
{
    int32_t persistentId_ = 10086;
    WSError result01 = ssm_->UpdateBrightness(persistentId_);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: SetDisplayBrightness
 * @tc.desc: SceneSesionManager set display brightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetDisplayBrightness, Function | SmallTest | Level3)
{
    float brightness = 2.0f;
    float result01 = ssm_->GetDisplayBrightness();
    EXPECT_EQ(result01, UNDEFINED_BRIGHTNESS);
    ssm_->SetDisplayBrightness(brightness);
    float result02 = ssm_->GetDisplayBrightness();
    ASSERT_EQ(result02, 2.0f);
}

/**
 * @tc.name: SetGestureNavigaionEnabled02
 * @tc.desc: SceneSesionManager set gesture navigaion enable
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetGestureNavigaionEnabled02, Function | SmallTest | Level3)
{
    bool enable = true;
    WMError result01 = ssm_->SetGestureNavigaionEnabled(enable);
    EXPECT_EQ(result01, WMError::WM_DO_NOTHING);
    ProcessGestureNavigationEnabledChangeFunc funcGesture_ = SceneSessionManagerTest::callbackFunc_;
    ssm_->SetGestureNavigationEnabledChangeListener(funcGesture_);
    WMError result02 = ssm_->SetGestureNavigaionEnabled(enable);
    EXPECT_EQ(result02, WMError::WM_OK);
    ProcessStatusBarEnabledChangeFunc funcStatus_ = ProcessStatusBarEnabledChangeFuncTest;
    ssm_->SetStatusBarEnabledChangeListener(funcStatus_);
    WMError result03 = ssm_->SetGestureNavigaionEnabled(enable);
    ASSERT_EQ(result03, WMError::WM_OK);
}

/**
 * @tc.name: SetFocusedSession
 * @tc.desc: SceneSesionManager set focused session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetFocusedSession, Function | SmallTest | Level3)
{
    int32_t focusedSession_ = ssm_->GetFocusedSession();
    EXPECT_EQ(focusedSession_, INVALID_SESSION_ID);
    int32_t persistendId_ = INVALID_SESSION_ID;
    WSError result01 = ssm_->SetFocusedSession(persistendId_);
    EXPECT_EQ(result01, WSError::WS_DO_NOTHING);
    persistendId_ = 10086;
    WSError result02 = ssm_->SetFocusedSession(persistendId_);
    EXPECT_EQ(result02, WSError::WS_OK);
    ASSERT_EQ(ssm_->GetFocusedSession(), 10086);
}

/**
 * @tc.name: RegisterSessionExceptionFunc
 * @tc.desc: SceneSesionManager register session expection func
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterSessionExceptionFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RegisterSessionExceptionFunc(scensession);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->RegisterSessionExceptionFunc(scensession);
    bool result01 = ssm_->IsSessionVisible(scensession);
    EXPECT_FALSE(result01);
    scensession->UpdateNativeVisibility(true);
    bool result02 = ssm_->IsSessionVisible(scensession);
    ASSERT_TRUE(result02);
    delete scensession;
}

/**
 * @tc.name: DumpSessionInfo
 * @tc.desc: SceneSesionManager dump session info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DumpSessionInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    std::ostringstream oss;
    std::string dumpInfo;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    info.isSystem_ = false;
    sptr<SceneSession> scensession = nullptr;
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    EXPECT_FALSE(scensession->IsVisible());
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionInfo(scensession, oss);
    scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpAllAppSessionInfo(oss);
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: SceneSesionManager dump session element info
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DumpSessionElementInfo, Function | SmallTest | Level3)
{
    DumpRootSceneElementInfoFunc func_ = DumpRootSceneElementInfoFuncTest;
    ssm_->SetDumpRootSceneElementInfoListener(func_);
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    info.isSystem_ = false;
    std::string strId = "10086";
    sptr<SceneSession> scensession = nullptr;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    std::vector<std::string> params_;
    std::string dumpInfo_;
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    scensession = nullptr;
    info.isSystem_ = true;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DumpSessionElementInfo(scensession, params_, dumpInfo_);
    WSError result01 = ssm_->GetSpecifiedSessionDumpInfo(dumpInfo_, params_, strId);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PARAM);
}

}
} // namespace Rosen
} // namespace OHOS

