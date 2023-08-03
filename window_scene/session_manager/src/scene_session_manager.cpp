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

#include "session_manager/include/scene_session_manager.h"

#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <map>
#include <sstream>
#include <unistd.h>

#include <ability_context.h>
#include <ability_info.h>
#include <ability_manager_client.h>
#include <bundle_mgr_interface.h>
#include <display_power_mgr_client.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <parameters.h>
#include <power_mgr_client.h>
#include <resource_manager.h>
#include <running_lock.h>
#include <session_info.h>
#include <start_options.h>
#include <system_ability_definition.h>
#include <want.h>
#include <hitrace_meter.h>
#include <transaction/rs_transaction.h>
#include <transaction/rs_interfaces.h>

#ifdef RES_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif

#include "ability_start_setting.h"
#include "color_parser.h"
#include "common/include/session_permission.h"
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/scene_session.h"
#include "session_helper.h"
#include "session/screen/include/screen_session.h"
#include "session_manager/include/screen_session_manager.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"
#include "xcollie/watchdog.h"
#include "zidl/window_manager_agent_interface.h"
#include "session_manager_agent_controller.h"
#include "window_manager.h"
#include "perform_reporter.h"
#include "focus_change_info.h"
#include "session_manager/include/screen_session_manager.h"

#include "window_visibility_info.h"
#ifdef MEMMGR_WINDOW_ENABLE
#include "mem_mgr_client.h"
#include "mem_mgr_window_info.h"
#endif

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };
#ifdef RES_SCHED_ENABLE
const std::string SCENE_BOARD_BUNDLE_NAME = "com.ohos.sceneboard";
#endif
const std::string SCENE_SESSION_MANAGER_THREAD = "SceneSessionManager";
const std::string WINDOW_INFO_REPORT_THREAD = "WindowInfoReportThread";
std::recursive_mutex g_instanceMutex;
constexpr uint32_t MAX_BRIGHTNESS = 255;
constexpr int32_t DEFAULT_USERID = -1;
constexpr int32_t SCALE_DIMENSION = 2;
constexpr int32_t TRANSLATE_DIMENSION = 2;
constexpr int32_t ROTAION_DIMENSION = 4;
constexpr int32_t CUBIC_CURVE_DIMENSION = 4;

constexpr int WINDOW_NAME_MAX_WIDTH = 21;
constexpr int DISPLAY_NAME_MAX_WIDTH = 10;
constexpr int VALUE_MAX_WIDTH = 5;
constexpr int ORIEN_MAX_WIDTH = 12;
constexpr int PID_MAX_WIDTH = 8;
constexpr int PARENT_ID_MAX_WIDTH = 6;
constexpr int WINDOW_NAME_MAX_LENGTH = 20;
constexpr int32_t STATUS_BAR_AVOID_AREA = 0;
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_DUMP_DISPLAY = "-d";
} // namespace

SceneSessionManager& SceneSessionManager::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static SceneSessionManager* instance = nullptr;
    if (instance == nullptr) {
        instance = new SceneSessionManager();
        instance->Init();
    }
    return *instance;
}

SceneSessionManager::SceneSessionManager() : rsInterface_(RSInterfaces::GetInstance())
{
    taskScheduler_ = std::make_shared<TaskScheduler>(SCENE_SESSION_MANAGER_THREAD);
    currentUserId_ = DEFAULT_USERID;
}

void SceneSessionManager::Init()
{
    constexpr uint64_t interval = 5 * 1000; // 5 second
    auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    auto mainEventHandler = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
    if (HiviewDFX::Watchdog::GetInstance().AddThread("MainThread", mainEventHandler, interval)) {
        WLOGFW("Add thread MainThread to watchdog failed.");
    }
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCENE_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCENE_SESSION_MANAGER_THREAD.c_str());
    }

#ifdef RES_SCHED_ENABLE
    std::unordered_map<std::string, std::string> payload {
        { "pid", std::to_string(getpid()) },
        { "tid", std::to_string(gettid()) },
        { "uid", std::to_string(getuid()) },
        { "bundleName", SCENE_BOARD_BUNDLE_NAME },
    };
    uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_REPORT_SCENE_BOARD;
    int64_t value = 0;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, payload);
#endif

    bundleMgr_ = GetBundleManager();
    LoadWindowSceneXml();
    ScreenSessionManager::GetInstance().SetSensorSubscriptionEnabled();
    sptr<IDisplayChangeListener> listener = new DisplayChangeListener();
    ScreenSessionManager::GetInstance().RegisterDisplayChangeListener(listener);

    // create handler for inner command at server
    eventLoop_ = AppExecFwk::EventRunner::Create(WINDOW_INFO_REPORT_THREAD);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (HiviewDFX::Watchdog::GetInstance().AddThread(WINDOW_INFO_REPORT_THREAD, eventHandler_)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", WINDOW_INFO_REPORT_THREAD.c_str());
    }

    listenerController_ = std::make_shared<SessionListenerController>();
    listenerController_->Init();

    StartWindowInfoReportLoop();
    WLOGI("SceneSessionManager init success.");
}

void SceneSessionManager::LoadWindowSceneXml()
{
    if (WindowSceneConfig::LoadConfigXml()) {
        if (WindowSceneConfig::GetConfig().IsMap()) {
            WindowSceneConfig::DumpConfig(*WindowSceneConfig::GetConfig().mapValue_);
        }
        ConfigWindowSceneXml();
    } else {
        WLOGFE("Load window scene xml failed");
    }
}

void SceneSessionManager::ConfigWindowSceneXml()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["windowEffect"];
    if (item.IsMap()) {
        ConfigWindowEffect(item);
    }

    item = config["decor"];
    if (item.IsMap()) {
        ConfigDecor(item);
    }

    item = config["defaultWindowMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
             numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(numbers[0]));
        }
    }

    item = config["defaultMaximizeMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<int32_t>(MaximizeMode::MODE_AVOID_SYSTEM_BAR) ||
            numbers[0] == static_cast<int32_t>(MaximizeMode::MODE_FULL_FILL))) {
            SceneSession::maximizeMode_ = static_cast<MaximizeMode>(numbers[0]);
        }
    }

    item = config["keyboardAnimation"];
    if (item.IsMap()) {
        ConfigKeyboardAnimation(item);
    }

    item = config["maxFloatingWindowSize"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.maxFloatingWindowSize_ = numbers[0];
        }
    }

    item = config["windowAnimation"];
    if (item.IsMap()) {
        ConfigWindowAnimation(item);
    }

    item = config["startWindowTransitionAnimation"];
    if (item.IsMap()) {
        ConfigStartingWindowAnimation(item);
    }
}
WSError SceneSessionManager::SetSessionContinueState(const sptr<IRemoteObject> &token,
    const ContinueState& continueState)
{
    sptr<SceneSession> sceneSession = FindSessionByToken(token);
    if (sceneSession == nullptr) {
        WLOGFI("fail to find session by token.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    sceneSession->SetSessionContinueState(continueState);
    return WSError::WS_OK;
}

void SceneSessionManager::ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig)
{
    WindowSceneConfig::ConfigItem item = decorConfig.GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isSystemDecorEnable_ = item.boolValue_;
        std::vector<std::string> supportedModes;
        item = decorConfig["supportedMode"];
        if (item.IsStrings()) {
            systemConfig_.decorModeSupportInfo_ = 0;
            supportedModes = *item.stringsValue_;
        }
        for (auto mode : supportedModes) {
            if (mode == "fullscreen") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN;
            } else if (mode == "floating") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
            } else if (mode == "pip") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
            } else if (mode == "split") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
            } else {
                WLOGFW("Invalid supporedMode");
                systemConfig_.decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
                break;
            }
        }
    }
}

static void AddAlphaToColor(float alpha, std::string& color)
{
    if (color.size() == 9 || alpha > 1.0f) { // size 9: color is ARGB
        return;
    }

    uint32_t alphaValue = 0xFF * alpha;
    std::stringstream ss;
    ss << std::hex << alphaValue;
    std::string strAlpha = ss.str();
    if (strAlpha.size() == 1) {
        strAlpha.append(1, '0');
    }

    color.insert(1, strAlpha);
}

void SceneSessionManager::ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig)
{
    AppWindowSceneConfig config;
    // config corner radius
    WindowSceneConfig::ConfigItem item = effectConfig["appWindows"]["cornerRadius"];
    if (item.IsMap()) {
        if (ConfigAppWindowCornerRadius(item["float"], config.floatCornerRadius_)) {
            appWindowSceneConfig_ = config;
        }
    }

    // config shadow
    item = effectConfig["appWindows"]["shadow"]["focused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.focusedShadow_)) {
            appWindowSceneConfig_.focusedShadow_ = config.focusedShadow_;
        }
    }

    item = effectConfig["appWindows"]["shadow"]["unfocused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.unfocusedShadow_)) {
            appWindowSceneConfig_.unfocusedShadow_ = config.unfocusedShadow_;
        }
    }

    AddAlphaToColor(appWindowSceneConfig_.focusedShadow_.alpha_, appWindowSceneConfig_.focusedShadow_.color_);
    AddAlphaToColor(appWindowSceneConfig_.unfocusedShadow_.alpha_, appWindowSceneConfig_.unfocusedShadow_.color_);

    WLOGFI("Config window effect successfully");
}

bool SceneSessionManager::ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out)
{
    std::map<std::string, float> stringToCornerRadius = {
        {"off", 0.0f}, {"defaultCornerRadiusXS", 4.0f}, {"defaultCornerRadiusS", 8.0f},
        {"defaultCornerRadiusM", 12.0f}, {"defaultCornerRadiusL", 16.0f}, {"defaultCornerRadiusXL", 24.0f}
    };

    if (item.IsString()) {
        auto value = item.stringValue_;
        if (stringToCornerRadius.find(value) != stringToCornerRadius.end()) {
            out = stringToCornerRadius[value];
            return true;
        }
    }
    return false;
}

bool SceneSessionManager::ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig,
    WindowShadowConfig& outShadow)
{
    WindowSceneConfig::ConfigItem item = shadowConfig["color"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return false;
        }
        outShadow.color_ = color;
    }

    item = shadowConfig["offsetX"];
    if (item.IsFloats()) {
        auto offsetX = *item.floatsValue_;
        if (offsetX.size() != 1) {
            return false;
        }
        outShadow.offsetX_ = offsetX[0];
    }

    item = shadowConfig["offsetY"];
    if (item.IsFloats()) {
        auto offsetY = *item.floatsValue_;
        if (offsetY.size() != 1) {
            return false;
        }
        outShadow.offsetY_ = offsetY[0];
    }

    item = shadowConfig["alpha"];
    if (item.IsFloats()) {
        auto alpha = *item.floatsValue_;
        if (alpha.size() != 1 ||
            (MathHelper::LessNotEqual(alpha[0], 0.0) && MathHelper::GreatNotEqual(alpha[0], 1.0))) {
            return false;
        }
        outShadow.alpha_ = alpha[0];
    }

    item = shadowConfig["radius"];
    if (item.IsFloats()) {
        auto radius = *item.floatsValue_;
        if (radius.size() != 1 || MathHelper::LessNotEqual(radius[0], 0.0)) {
            return false;
        }
        outShadow.radius_ = radius[0];
    }

    return true;
}

void SceneSessionManager::ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig)
{
    WindowSceneConfig::ConfigItem item = animationConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        appWindowSceneConfig_.keyboardAnimation_.curveType_ = CreateCurve(item["curve"]);
    }
    item = animationConfig["timing"]["durationIn"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // durationIn
            appWindowSceneConfig_.keyboardAnimation_.durationIn_ = static_cast<uint32_t>(numbers[0]);
        }
    }
    item = animationConfig["timing"]["durationOut"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // durationOut
            appWindowSceneConfig_.keyboardAnimation_.durationOut_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig)
{
    WindowSceneConfig::ConfigItem item = windowAnimationConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        appWindowSceneConfig_.windowAnimation_.curveType_ = CreateCurve(item["curve"], "windowAnimation");
    }
    item = windowAnimationConfig["timing"]["duration"];
    if (item.IsInts() && item.intsValue_->size() == 1) {
        auto duration = *item.intsValue_;
        appWindowSceneConfig_.windowAnimation_.duration_ = duration[0];
    }
    item = windowAnimationConfig["scale"];
    if (item.IsFloats() && item.floatsValue_->size() == SCALE_DIMENSION) {
        auto scales = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.scaleX_ = scales[0];
        appWindowSceneConfig_.windowAnimation_.scaleY_ = scales[1];
    }
    item = windowAnimationConfig["rotation"];
    if (item.IsFloats() && item.floatsValue_->size() == ROTAION_DIMENSION) {
        auto rotations = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.rotationX_ = rotations[0]; // 0 ctrlX1
        appWindowSceneConfig_.windowAnimation_.rotationY_ = rotations[1]; // 1 ctrlY1
        appWindowSceneConfig_.windowAnimation_.rotationZ_ = rotations[2]; // 2 ctrlX2
        appWindowSceneConfig_.windowAnimation_.angle_ = rotations[3]; // 3 ctrlY2
    }
    item = windowAnimationConfig["translate"];
    if (item.IsFloats() && item.floatsValue_->size() == TRANSLATE_DIMENSION) {
        auto translates = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.translateX_ = translates[0];
        appWindowSceneConfig_.windowAnimation_.translateY_ = translates[1];
    }
    item = windowAnimationConfig["opacity"];
    if (item.IsFloats() && item.floatsValue_->size() == 1) {
        auto opacity = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.opacity_ = opacity[0];
    }
}

void SceneSessionManager::ConfigStartingWindowAnimation(const WindowSceneConfig::ConfigItem& configItem)
{
    auto& config = appWindowSceneConfig_.startingWindowAnimationConfig_;
    auto item = configItem.GetProp("enable");
    if (item.IsBool()) {
        config.enabled_ = item.boolValue_;
    }
    item = configItem["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        config.curve_ = CreateCurve(item["curve"]);
    }
    item = configItem["timing"]["duration"];
    if (item.IsInts() && item.intsValue_->size() == 1) {
        config.duration_ = (*item.intsValue_)[0];
    }
    item = configItem["opacityStart"];
    if (item.IsFloats() && item.floatsValue_->size() == 1) {
        config.opacityStart_ = (*item.floatsValue_)[0];
    }
    item = configItem["opacityEnd"];
    if (item.IsFloats() && item.floatsValue_->size() == 1) {
        config.opacityEnd_ = (*item.floatsValue_)[0];
    }
}

std::string SceneSessionManager::CreateCurve(const WindowSceneConfig::ConfigItem& curveConfig,
    const std::string& nodeName)
{
    static std::unordered_set<std::string> curveSet = { "easeOut", "ease", "easeIn", "easeInOut", "default",
        "linear", "spring", "interactiveSpring", "interpolatingSpring" };

    std::string curveName = "easeOut";
    const auto& nameItem = curveConfig.GetProp("name");
    if (!nameItem.IsString()) {
        return curveName;
    }
    std::string name = nameItem.stringValue_;
    if (name == "cubic" && curveConfig.IsFloats() && curveConfig.floatsValue_->size() == CUBIC_CURVE_DIMENSION) {
        const auto& numbers = *curveConfig.floatsValue_;
        curveName = name;
        if (nodeName == "windowAnimation") {
            appWindowSceneConfig_.windowAnimation_.ctrlX1_ = numbers[0]; // 0 ctrlX1
            appWindowSceneConfig_.windowAnimation_.ctrlY1_ = numbers[1]; // 1 ctrlY1
            appWindowSceneConfig_.windowAnimation_.ctrlX2_ = numbers[2]; // 2 ctrlX2
            appWindowSceneConfig_.windowAnimation_.ctrlY2_ = numbers[3]; // 3 ctrlY2
        } else {
            appWindowSceneConfig_.keyboardAnimation_.ctrlX1_ = numbers[0]; // 0 ctrlX1
            appWindowSceneConfig_.keyboardAnimation_.ctrlY1_ = numbers[1]; // 1 ctrlY1
            appWindowSceneConfig_.keyboardAnimation_.ctrlX2_ = numbers[2]; // 2 ctrlX2
            appWindowSceneConfig_.keyboardAnimation_.ctrlY2_ = numbers[3]; // 3 ctrlY2
        }
    } else {
        auto iter = curveSet.find(name);
        if (iter != curveSet.end()) {
            curveName = name;
        }
    }
    return curveName;
}

void SceneSessionManager::SetRootSceneContext(AbilityRuntime::Context* context)
{
    rootSceneContext_ = std::shared_ptr<AbilityRuntime::Context>(context);
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this]() -> sptr<RootSceneSession> {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }
        system::SetParameter("bootevent.wms.fullscreen.ready", "true");
        SessionInfo info;
        rootSceneSession_ = new (std::nothrow) RootSceneSession(info);
        if (!rootSceneSession_) {
            WLOGFE("rootSceneSession is nullptr");
            return nullptr;
        }
        sptr<ISession> iSession(rootSceneSession_);
        AAFwk::AbilityManagerClient::GetInstance()->SetRootSceneSession(iSession->AsObject());
        return rootSceneSession_;
    };

    return taskScheduler_->PostSyncTask(task);
}

sptr<SceneSession> SceneSessionManager::GetSceneSession(int32_t persistentId)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = sceneSessionMap_.find(persistentId);
    if (iter == sceneSessionMap_.end()) {
        WLOGFE("Error found scene session with id: %{public}d", persistentId);
        return nullptr;
    }
    return iter->second;
}

std::vector<sptr<SceneSession>> SceneSessionManager::GetSceneSessionVectorByType(WindowType type)
{
    std::vector<sptr<SceneSession>> sceneSessionVector;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetWindowType() == type) {
            sceneSessionVector.emplace_back(sceneSession);
        }
    }

    return sceneSessionVector;
}

WSError SceneSessionManager::UpdateParentSession(const sptr<SceneSession>& sceneSession,
    sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        WLOGFW("Property is null, no need to update parent info");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sceneSession == nullptr) {
        WLOGFE("Session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto parentPersistentId = property->GetParentPersistentId();
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
        auto parentSceneSession = GetSceneSession(parentPersistentId);
        if (!parentSceneSession) {
            WLOGFD("Session is invalid");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        sceneSession->SetParentSession(parentSceneSession);
    } else if (property->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG &&
        parentPersistentId != INVALID_SESSION_ID) {
        auto parentSession = GetSceneSession(parentPersistentId);
        if (parentSession == nullptr) {
            WLOGFE("Parent session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        WLOGFD("Add dialog id to its parent vector");
        parentSession->BindDialogToParentSession(sceneSession);
        sceneSession->SetParentSession(parentSession);
    }
    return WSError::WS_OK;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    if (sessionInfo.persistentId_ != 0) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr) {
            WLOGFI("get exist session persistentId: %{public}d", sessionInfo.persistentId_);
            return session;
        }
    }
    sptr<SceneSession::SpecificSessionCallback> specificCb = new (std::nothrow)SceneSession::SpecificSessionCallback();
    if (specificCb == nullptr) {
        WLOGFE("SpecificSessionCallback is nullptr");
        return nullptr;
    }
    specificCb->onCreate_ = std::bind(&SceneSessionManager::RequestSceneSession,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onDestroy_ = std::bind(&SceneSessionManager::DestroyAndDisconnectSpecificSession,
        this, std::placeholders::_1);
    specificCb->onCameraFloatSessionChange_ = std::bind(&SceneSessionManager::UpdateCameraFloatWindowStatus,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onGetSceneSessionVectorByType_ = std::bind(&SceneSessionManager::GetSceneSessionVectorByType,
        this, std::placeholders::_1);
    specificCb->onUpdateAvoidArea_ = std::bind(&SceneSessionManager::UpdateAvoidArea, this, std::placeholders::_1);
    auto task = [this, sessionInfo, specificCb, property]() {
        WLOGFI("sessionInfo: bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s, type %{public}u",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(),
            sessionInfo.abilityName_.c_str(), sessionInfo.windowType_);
        sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, specificCb);
        if (sceneSession == nullptr) {
            WLOGFE("sceneSession is nullptr!");
            return sceneSession;
        }
        if (sessionInfo.isSystem_) {
            sceneSession->SetCallingPid(IPCSkeleton::GetCallingPid());
            sceneSession->SetCallingUid(IPCSkeleton::GetCallingUid());
            sceneSession->SetAbilityToken(rootSceneContext_ != nullptr ? rootSceneContext_->GetToken() : nullptr);
        }
        auto persistentId = sceneSession->GetPersistentId();
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSession(%d )", persistentId);
        sceneSession->SetSystemConfig(systemConfig_);
        UpdateParentSession(sceneSession, property);
        {
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.insert({ persistentId, sceneSession });
        }
        RegisterSessionStateChangeNotifyManagerFunc(sceneSession);
        RegisterInputMethodShownFunc(sceneSession);
        RegisterInputMethodHideFunc(sceneSession);
        WLOGFI("create session persistentId: %{public}d", persistentId);
        return sceneSession;
    };

    return taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::RegisterInputMethodShownFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifyCallingSessionForegroundFunc onInputMethodShown = [this](int32_t persistentId) {
        this->OnInputMethodShown(persistentId);
    };
    sceneSession->SetNotifyCallingSessionForegroundFunc(onInputMethodShown);
    WLOGFD("RegisterInputMethodShownFunc success");
}

void SceneSessionManager::OnInputMethodShown(const int32_t& persistentId)
{
    WLOGFD("Resize input method calling window");
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("Input method is null");
        return;
    }
    callingSession_ = GetSceneSession(focusedSessionId_);
    ResizeSoftInputCallingSessionIfNeed(scnSession);
}

void SceneSessionManager::RegisterInputMethodHideFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifyCallingSessionBackgroundFunc onInputMethodHide = [this]() {
        this->RestoreCallingSessionSizeIfNeed();
    };
    sceneSession->SetNotifyCallingSessionBackgroundFunc(onInputMethodHide);
    WLOGFD("RegisterInputMethodHideFunc success");
}

sptr<AAFwk::SessionInfo> SceneSessionManager::SetAbilitySessionInfo(const sptr<SceneSession>& scnSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (!abilitySessionInfo) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    sptr<ISession> iSession(scnSession);
    abilitySessionInfo->sessionToken = iSession->AsObject();
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->persistentId = scnSession->GetPersistentId();
    abilitySessionInfo->requestCode = sessionInfo.requestCode;
    abilitySessionInfo->resultCode = sessionInfo.resultCode;
    abilitySessionInfo->uiAbilityId = sessionInfo.uiAbilityId_;
    abilitySessionInfo->startSetting = sessionInfo.startSetting;
    abilitySessionInfo->callingTokenId = sessionInfo.callingTokenId_;
    abilitySessionInfo->userId = currentUserId_;
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    } else {
        abilitySessionInfo->want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    return abilitySessionInfo;
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession, bool isNewActive)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, isNewActive]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionActivation(%d )", persistentId);
        WLOGFI("active persistentId: %{public}d", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        scnSessionInfo->isNewWant = isNewActive;
        AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(scnSessionInfo);
        activeSessionId_ = persistentId;
        NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_ADDED);
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession,
    const bool isDelegator)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, isDelegator]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("background session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionBackground (%d )", persistentId);
        scnSession->SetActive(false);
        scnSession->Background();
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (persistentId == brightnessSessionId_) {
            UpdateBrightness(focusedSessionId_);
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }

        if (!isDelegator) {
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
        } else {
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo, true);
        }
        NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:DestroyDialogWithMainWindow");
    if (scnSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        WLOGFD("Begin to destroy its dialog");
        auto dialogVec = scnSession->GetDialogVector();
        for (auto dialog : dialogVec) {
            if (dialog == nullptr) {
                WLOGFE("dialog is nullptr");
                return WSError::WS_ERROR_NULLPTR;
            }
            if (!GetSceneSession(dialog->GetPersistentId())) {
                WLOGFE("session is invalid with %{public}d", dialog->GetPersistentId());
                return WSError::WS_ERROR_INVALID_SESSION;
            }
            auto sceneSession = GetSceneSession(dialog->GetPersistentId());
            WindowDestroyNotifyVisibility(sceneSession);
            dialog->NotifyDestroy();
            dialog->Disconnect();
            NotifyWindowInfoChange(dialog->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
            {
                std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
                sceneSessionMap_.erase(dialog->GetPersistentId());
            }
        }
        return WSError::WS_OK;
    }
    return WSError::WS_ERROR_INVALID_SESSION;
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        DestroyDialogWithMainWindow(scnSession);
        WLOGFI("destroy session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionDestruction (%" PRIu32" )", persistentId);
        WindowDestroyNotifyVisibility(scnSession);
        scnSession->Disconnect();
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        AAFwk::AbilityManagerClient::GetInstance()->CloseUIAbilityBySCB(scnSessionInfo);
        NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
        {
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.erase(persistentId);
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

void SceneSessionManager::AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr || sessionStage == nullptr) {
        WLOGFE("sessionStage or sceneSession is nullptr");
        return;
    }

    auto remoteObject = sessionStage->AsObject();
    remoteObjectMap_.insert(std::make_pair(remoteObject, sceneSession->GetPersistentId()));
    if (windowDeath_ == nullptr) {
        WLOGFE("failed to create death recipient");
        return;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        WLOGFE("failed to add death recipient");
        return;
    }
    WLOGFD("Id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::DestroySpecificSession(const sptr<IRemoteObject>& remoteObject)
{
    auto task = [this, remoteObject] {
        auto iter = remoteObjectMap_.find(remoteObject);
        if (iter == remoteObjectMap_.end()) {
            WLOGFE("Invalid remoteObject");
            return;
        }
        WLOGFD("Remote died, id: %{public}d", iter->second);
        DestroyAndDisconnectSpecificSession(iter->second);
        remoteObjectMap_.erase(iter);
    };
    taskScheduler_->PostAsyncTask(task);
}

WSError SceneSessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartedByInputMethod()) {
        WLOGFE("check input method permission failed");
    }
    // get pid/uid before post sync task
    int32_t pid = IPCSkeleton::GetCallingPid();
    int32_t uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property, &persistentId, &session, pid, uid]() {
        // create specific session
        SessionInfo info;
        sptr<SceneSession> sceneSession = RequestSceneSession(info, property);
        if (sceneSession == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        sceneSession->SetCallingPid(pid);
        sceneSession->SetCallingUid(uid);
        // connect specific session and sessionStage
        WSError errCode = sceneSession->ConnectImpl(sessionStage, eventChannel, surfaceNode, systemConfig_, property);
        if (property) {
            persistentId = property->GetPersistentId();
        }
        if (createSpecificSessionFunc_) {
            createSpecificSessionFunc_(sceneSession);
        }
        session = sceneSession;
        AddClientDeathRecipient(sessionStage, sceneSession);
        return errCode;
    };

    return taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::SetCreateSpecificSessionListener(const NotifyCreateSpecificSessionFunc& func)
{
    createSpecificSessionFunc_ = func;
}

void SceneSessionManager::SetGestureNavigationEnabledChangeListener(
    const ProcessGestureNavigationEnabledChangeFunc& func)
{
    WLOGFD("SetGestureNavigationEnabledChangeListener");
    if (!func) {
        WLOGFD("set func is null");
    }
    gestureNavigationEnabledChangeFunc_ = func;
}

void SceneSessionManager::OnOutsideDownEvent(int32_t x, int32_t y)
{
    WLOGFI("OnOutsideDownEvent x = %{public}d, y = %{public}d", x, y);
    if (outsideDownEventFunc_) {
        outsideDownEventFunc_(x, y);
    }
}

void SceneSessionManager::NotifySessionTouchOutside(int32_t action, int32_t x, int32_t y)
{
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        auto persistentId = sceneSession->GetPersistentId();
        auto touchHotAreaRects = sceneSession->GetTouchHotAreas();
        if (!touchHotAreaRects.empty()) {
            bool touchInsideFlag = false;
            for (auto touchHotAreaRect : touchHotAreaRects) {
                if (!SessionHelper::IsPointInRect(x, y, touchHotAreaRect)) {
                    continue;
                } else {
                    WLOGFD("TouchInside %{public}d", persistentId);
                    touchInsideFlag = true;
                    break;
                }
            }
            if (!touchInsideFlag) {
                sceneSession->NotifyTouchOutside();
            }
        } else {
            auto hotAreaRect = sceneSession->GetHotAreaRect(action);
            if (!SessionHelper::IsPointInRect(x, y, hotAreaRect)) {
                sceneSession->NotifyTouchOutside();
            } else {
                WLOGFD("TouchInside %{public}d", persistentId);
            }
        }
    }
}

void SceneSessionManager::SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func)
{
    WLOGFD("SetOutsideDownEventListener");
    outsideDownEventFunc_ = func;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSession(const int32_t& persistentId)
{
    auto task = [this, persistentId]() {
        WLOGFI("Destroy specific session persistentId: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        auto ret = sceneSession->UpdateActiveStatus(false);
        WindowDestroyNotifyVisibility(sceneSession);
        if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
            if (parentSession == nullptr) {
                WLOGFE("Dialog not bind parent");
            } else {
                parentSession->RemoveDialogToParentSession(sceneSession);
            }
            sceneSession->NotifyDestroy();
        }
        ret = sceneSession->Disconnect();
        NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
        {
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.erase(persistentId);
        }
        return ret;
    };

    return taskScheduler_->PostSyncTask(task);
}

const AppWindowSceneConfig& SceneSessionManager::GetWindowSceneConfig() const
{
    return appWindowSceneConfig_;
}

WSError SceneSessionManager::ProcessBackEvent()
{
    auto task = [this]() {
        auto session = GetSceneSession(activeSessionId_);
        if (!session) {
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        WLOGFD("ProcessBackEvent session persistentId: %{public}d", activeSessionId_);
        session->ProcessBackEvent();
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::SwitchUser(int32_t oldUserId, int32_t newUserId, std::string &fileDir)
{
    if (oldUserId != currentUserId_ || oldUserId == newUserId || fileDir.empty()) {
        WLOGFE("SwitchUser params invalid");
        return WSError::WS_DO_NOTHING;
    }
    WLOGFD("SwitchUser oldUserId : %{public}d newUserId : %{public}d path : %{public}s",
        oldUserId, newUserId, fileDir.c_str());
    auto task = [this, newUserId, &fileDir]() {
        if (!ScenePersistence::CreateSnapshotDir(fileDir)) {
            WLOGFD("snapshot dir existed");
        }
        currentUserId_ = newUserId;
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto scnSession = item.second;
            auto persistentId = scnSession->GetPersistentId();
            scnSession->SetActive(false);
            scnSession->Background();
            if (persistentId == brightnessSessionId_) {
                UpdateBrightness(focusedSessionId_);
            }
            auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
            if (!scnSessionInfo) {
                return WSError::WS_ERROR_NULLPTR;
            }
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
        }
        sceneSessionMap_.clear();
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task);
    return WSError::WS_OK;
}

sptr<AppExecFwk::IBundleMgr> SceneSessionManager::GetBundleManager()
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WLOGFE("Failed to get SystemAbilityManager.");
        return nullptr;
    }

    auto bmsObj = systemAbilityMgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bmsObj == nullptr) {
        WLOGFE("Failed to get BundleManagerService.");
        return nullptr;
    }

    return iface_cast<AppExecFwk::IBundleMgr>(bmsObj);
}

std::shared_ptr<Global::Resource::ResourceManager> SceneSessionManager::CreateResourceManager(
    const AppExecFwk::AbilityInfo& abilityInfo)
{
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    std::shared_ptr<Global::Resource::ResourceManager> resourceMgr(Global::Resource::CreateResourceManager());
    resourceMgr->UpdateResConfig(*resConfig);

    std::string loadPath;
    if (!abilityInfo.hapPath.empty()) { // zipped hap
        loadPath = abilityInfo.hapPath;
    } else {
        loadPath = abilityInfo.resourcePath;
    }

    if (!resourceMgr->AddResource(loadPath.c_str())) {
        WLOGFE("Add resource %{private}s failed.", loadPath.c_str());
        return nullptr;
    }
    return resourceMgr;
}

void SceneSessionManager::GetStartPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo,
    std::string& path, uint32_t& bgColor)
{
    auto resourceMgr = CreateResourceManager(abilityInfo);
    if (resourceMgr == nullptr) {
        WLOGFE("resource manager is nullptr.");
        return;
    }

    if (resourceMgr->GetColorById(abilityInfo.startWindowBackgroundId, bgColor) != Global::Resource::RState::SUCCESS) {
        WLOGFW("Failed to get background color id %{private}d.", abilityInfo.startWindowBackgroundId);
    }

    if (resourceMgr->GetMediaById(abilityInfo.startWindowIconId, path) != Global::Resource::RState::SUCCESS) {
        WLOGFE("Failed to get icon id %{private}d.", abilityInfo.startWindowIconId);
        return;
    }

    if (!abilityInfo.hapPath.empty()) { // zipped hap
        auto pos = path.find_last_of('.');
        if (pos == std::string::npos) {
            WLOGFE("Format error, path %{private}s.", path.c_str());
            return;
        }
        path = "resource:///" + std::to_string(abilityInfo.startWindowIconId) + path.substr(pos);
    }
}

void SceneSessionManager::GetStartPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor)
{
    if (!bundleMgr_) {
        WLOGFE("bundle manager is nullptr.");
        return;
    }

    AAFwk::Want want;
    want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret = bundleMgr_->QueryAbilityInfo(
        want, AppExecFwk::GET_ABILITY_INFO_DEFAULT, AppExecFwk::Constants::ANY_USERID, abilityInfo);
    if (!ret) {
        WLOGFE("Get ability info from BMS failed!");
        return;
    }

    GetStartPageFromResource(abilityInfo, path, bgColor);
}

WSError SceneSessionManager::UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action)
{
    wptr<SceneSessionManager> weak = this;
    auto task = [weak, property, action]() {
        auto weakSession = weak.promote();
        if (weakSession == nullptr) {
            return;
        }
        if (property == nullptr) {
            return;
        }
        auto sceneSession = weakSession->GetSceneSession(property->GetPersistentId());
        if (sceneSession == nullptr) {
            return;
        }
        WLOGI("Id: %{public}d, action: %{public}u", sceneSession->GetPersistentId(), action);
        weakSession->HandleUpdateProperty(property, action, sceneSession);
    };
    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

void SceneSessionManager::HandleUpdateProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action, const sptr<SceneSession>& sceneSession)
{
    switch (action) {
        case WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON: {
            sceneSession->SetTurnScreenOn(property->IsTurnScreenOn());
            HandleTurnScreenOn(sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON: {
            sceneSession->SetKeepScreenOn(property->IsKeepScreenOn());
            HandleKeepScreenOn(sceneSession, property->IsKeepScreenOn());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE: {
            sceneSession->SetFocusable(property->GetFocusable());
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE: {
            sceneSession->SetTouchable(property->GetTouchable());
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS: {
            if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                WLOGW("only app main window can set brightness");
                return;
            }
            // @todo if sceneSession is inactive, return
            SetBrightness(sceneSession, property->GetBrightness());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION: {
            ScreenSessionManager::GetInstance().
                SetOrientationFromWindow(property->GetDisplayId(), property->GetRequestedOrientation());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE: {
            bool prePrivacyMode = sceneSession->GetWindowSessionProperty()->GetPrivacyMode() ||
                sceneSession->GetWindowSessionProperty()->GetSystemPrivacyMode();
            bool isPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
            if (prePrivacyMode != isPrivacyMode) {
                sceneSession->GetWindowSessionProperty()->SetPrivacyMode(isPrivacyMode);
                sceneSession->GetWindowSessionProperty()->SetSystemPrivacyMode(isPrivacyMode);
                sceneSession->GetSurfaceNode()->SetSecurityLayer(isPrivacyMode);
                RSTransaction::FlushImplicitTransaction();
                UpdatePrivateStateAndNotify(isPrivacyMode);
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetMaximizeMode(property->GetMaximizeMode());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS: {
            auto systemBarProperties = property->GetSystemBarProperty();
            for (auto iter : systemBarProperties) {
                sceneSession->SetSystemBarProperty(iter.first, iter.second);
            }
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_FLAGS: {
            SetWindowFlags(sceneSession, property->GetWindowFlags());
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_MODE: {
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetAnimationFlag(property->GetAnimationFlag());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                std::vector<Rect> touchHotAreas;
                property->GetTouchHotAreas(touchHotAreas);
                sceneSession->GetSessionProperty()->SetTouchHotAreas(touchHotAreas);
            }
            break;
        }
        default:
            break;
    }
}

void SceneSessionManager::HandleTurnScreenOn(const sptr<SceneSession>& sceneSession)
{
    auto task = [this, sceneSession]() {
        if (sceneSession == nullptr) {
            WLOGFE("session is invalid");
            return;
        }
        WLOGFD("Win: %{public}s, is turn on%{public}d",
            sceneSession->GetWindowName().c_str(), sceneSession->IsTurnScreenOn());
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        if (sceneSession->IsTurnScreenOn() && !PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
            WLOGI("turn screen on");
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
    };
    taskScheduler_->PostAsyncTask(task);
}

void SceneSessionManager::HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock)
{
    auto task = [this, sceneSession, requireLock]() {
        if (sceneSession == nullptr) {
            WLOGFE("session is invalid");
            return;
        }
        if (requireLock && sceneSession->keepScreenLock_ == nullptr) {
            // reset ipc identity
            std::string identity = IPCSkeleton::ResetCallingIdentity();
            sceneSession->keepScreenLock_ =
                PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(sceneSession->GetWindowName(),
                PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
            // set ipc identity to raw
            IPCSkeleton::SetCallingIdentity(identity);
        }
        if (sceneSession->keepScreenLock_ == nullptr) {
            return;
        }
        WLOGI("keep screen on: [%{public}s, %{public}d]", sceneSession->GetWindowName().c_str(), requireLock);
        ErrCode res;
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        if (requireLock) {
            res = sceneSession->keepScreenLock_->Lock();
        } else {
            res = sceneSession->keepScreenLock_->UnLock();
        }
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
        if (res != ERR_OK) {
            WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]",
                requireLock, res);
        }
    };
    taskScheduler_->PostAsyncTask(task);
}

WSError SceneSessionManager::SetBrightness(const sptr<SceneSession>& sceneSession, float brightness)
{
    if (!sceneSession->IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (brightness == sceneSession->GetBrightness()) {
        WLOGFD("Session brightness do not change: [%{public}f]", brightness);
        return WSError::WS_DO_NOTHING;
    }
    sceneSession->SetBrightness(brightness);
    if (GetDisplayBrightness() != brightness) {
        DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
            static_cast<uint32_t>(brightness * MAX_BRIGHTNESS));
        SetDisplayBrightness(brightness);
    }
    brightnessSessionId_ = sceneSession->GetPersistentId();
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateBrightness(int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        WLOGW("only app main window can set brightness");
        return WSError::WS_DO_NOTHING;
    }
    auto brightness = sceneSession->GetBrightness();
    WLOGI("Brightness: [%{public}f, %{public}f]", GetDisplayBrightness(), brightness);
    if (std::fabs(brightness - UNDEFINED_BRIGHTNESS) < std::numeric_limits<float>::min()) {
        if (GetDisplayBrightness() != brightness) {
            WLOGI("adjust brightness with default value");
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        }
        brightnessSessionId_ = INVALID_WINDOW_ID;
    } else {
        if (GetDisplayBrightness() != brightness) {
            WLOGI("adjust brightness with value");
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                static_cast<uint32_t>(brightness * MAX_BRIGHTNESS));
            SetDisplayBrightness(brightness);
        }
        brightnessSessionId_ = sceneSession->GetPersistentId();
    }
    return WSError::WS_OK;
}

int32_t SceneSessionManager::GetCurrentUserId() const
{
    return currentUserId_;
}

void SceneSessionManager::SetDisplayBrightness(float brightness)
{
    displayBrightness_ = brightness;
}

float SceneSessionManager::GetDisplayBrightness() const
{
    return displayBrightness_;
}

WMError SceneSessionManager::SetGestureNavigaionEnabled(bool enable)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SetGestureNavigationEnabled permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFD("SetGestureNavigationEnabled, enable: %{public}d", enable);
    auto task = [this, enable]() {
        if (!gestureNavigationEnabledChangeFunc_) {
            WLOGFE("callback func is null");
            return WMError::WM_DO_NOTHING;
        } else {
            gestureNavigationEnabledChangeFunc_(enable);
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task);
}

WSError SceneSessionManager::SetFocusedSession(int32_t persistentId)
{
    if (focusedSessionId_ == persistentId) {
        WLOGI("Focus scene not change, id: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    focusedSessionId_ = persistentId;
    NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    return WSError::WS_OK;
}

int32_t SceneSessionManager::GetFocusedSession() const
{
    return focusedSessionId_;
}

void SceneSessionManager::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    auto sceneSession = GetSceneSession(focusedSessionId_);
    if (sceneSession) {
        WLOGFD("Get focus session info success");
        focusInfo.windowId_ = sceneSession->GetWindowId();
        focusInfo.displayId_ = static_cast<DisplayId>(0);
        focusInfo.pid_ = sceneSession->GetCallingPid();
        focusInfo.uid_ = sceneSession->GetCallingUid();
        focusInfo.windowType_ = sceneSession->GetWindowType();
        focusInfo.abilityToken_ = sceneSession->GetAbilityToken();
    }
    return;
}

static bool IsValidDigitString(const std::string& windowIdStr)
{
    if (windowIdStr.empty()) {
        return false;
    }
    for (char ch : windowIdStr) {
        if ((ch >= '0' && ch <= '9')) {
            continue;
        }
        WLOGFE("invalid window id");
        return false;
    }
    return true;
}

bool SceneSessionManager::IsSessionVisible(const sptr<SceneSession>& session)
{
    if (session->IsVisible() || session->GetSessionState() == SessionState::STATE_ACTIVE ||
        session->GetSessionState() == SessionState::STATE_FOREGROUND) {
        return true;
    }
    return false;
}

void SceneSessionManager::DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss)
{
    if (session == nullptr) {
        return;
    }
    int zOrder = IsSessionVisible(session) ? session->GetZOrder() : -1;
    WSRect rect = session->GetSessionRect();
    std::string sName;
    if (session->GetSessionInfo().isSystem_) {
        sName = session->GetSessionInfo().abilityName_;
    } else {
        sName = session->GetWindowName();
    }
    uint32_t displayId = 0;
    uint32_t flag = 0;
    uint32_t orientation = 0;
    const std::string& windowName = sName.size() <= WINDOW_NAME_MAX_LENGTH ?
        sName : sName.substr(0, WINDOW_NAME_MAX_LENGTH);
    // std::setw is used to set the output width and different width values are set to keep the format aligned.
    oss << std::left << std::setw(WINDOW_NAME_MAX_WIDTH) << windowName
        << std::left << std::setw(DISPLAY_NAME_MAX_WIDTH) << displayId
        << std::left << std::setw(PID_MAX_WIDTH) << session->GetCallingPid()
        << std::left << std::setw(PARENT_ID_MAX_WIDTH) << session->GetPersistentId()
        << std::left << std::setw(VALUE_MAX_WIDTH) << static_cast<uint32_t>(session->GetWindowType())
        << std::left << std::setw(VALUE_MAX_WIDTH) << static_cast<uint32_t>(session->GetWindowMode())
        << std::left << std::setw(VALUE_MAX_WIDTH) << flag
        << std::left << std::setw(VALUE_MAX_WIDTH) << zOrder
        << std::left << std::setw(ORIEN_MAX_WIDTH) << orientation
        << "[ "
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.posX_
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.posY_
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.width_
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.height_
        << "]"
        << std::endl;
}

void SceneSessionManager::DumpAllAppSessionInfo(std::ostringstream& oss)
{
    oss << std::endl << "Current mission lists:" << std::endl;
    oss << " MissionList Type #NORMAL" << std::endl;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& elem : sceneSessionMap_) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            WLOGFW("curSession is nullptr");
            continue;
        }
        if (curSession->GetSessionInfo().isSystem_ ||
            curSession->GetWindowType() < WindowType::APP_MAIN_WINDOW_BASE ||
            curSession->GetWindowType() >= WindowType::APP_MAIN_WINDOW_END) {
            WLOGFW("No need to dump, id: %{public}d, isSystem: %{public}d, windowType: %{public}d",
                curSession->GetPersistentId(), curSession->GetSessionInfo().isSystem_, curSession->GetWindowType());
            continue;
        }

        const auto& sessionInfo = curSession->GetSessionInfo();
        std::string isActive = curSession->IsActive() ? "FOREGROUND" : "BACKGROUND";
        oss << "    Mission ID #" << curSession->GetPersistentId() << "  mission name #" << "[#"
            << sessionInfo.bundleName_ << ":" << sessionInfo.moduleName_ << ":" << sessionInfo.abilityName_
            << "]" << "    lockedState #0" << std::endl;
        oss << "    app name [" << sessionInfo.bundleName_ << "]" << std::endl;
        oss << "    main name [" << sessionInfo.abilityName_ << "]" << std::endl;
        oss << "    bundle name [" << sessionInfo.bundleName_ << "]" << std::endl;
        oss << "    ability type [PAGE]" << std::endl;
        oss << "    state #" << isActive.c_str() << std::endl;
        oss << "    app state #" << isActive.c_str() << std::endl;
        oss << "    callee connections:" << std::endl;
    }
}

WSError SceneSessionManager::GetAllSessionDumpInfo(std::string& dumpInfo)
{
    int32_t screenGroupId = 0;
    std::ostringstream oss;
    oss << "-------------------------------------ScreenGroup " << screenGroupId
        << "-------------------------------------" << std::endl;
    oss << "WindowName           DisplayId Pid     WinId Type Mode Flag ZOrd Orientation [ x    y    w    h    ]"
        << std::endl;

    std::vector<sptr<SceneSession>> allSession;
    std::vector<sptr<SceneSession>> backgroundSession;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& elem : sceneSessionMap_) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            continue;
        }
        if (IsSessionVisible(curSession)) {
            allSession.push_back(curSession);
        } else {
            backgroundSession.push_back(curSession);
        }
    }
    lock.unlock();
    allSession.insert(allSession.end(), backgroundSession.begin(), backgroundSession.end());
    uint32_t count = 0;
    for (const auto& session : allSession) {
        if (session == nullptr) {
            continue;
        }
        if (count == static_cast<uint32_t>(allSession.size() - backgroundSession.size())) {
            oss << "---------------------------------------------------------------------------------------"
                << std::endl;
        }
        DumpSessionInfo(session, oss);
        count++;
    }
    oss << "Focus window: " << GetFocusedSession() << std::endl;
    oss << "Total window num: " << sceneSessionMap_.size() << std::endl;
    DumpAllAppSessionInfo(oss);
    dumpInfo.append(oss.str());
    return WSError::WS_OK;
}

void SceneSessionManager::SetDumpRootSceneElementInfoListener(const DumpRootSceneElementInfoFunc& func)
{
    dumpRootSceneFunc_ = func;
}

void SceneSessionManager::DumpSessionElementInfo(const sptr<SceneSession>& session,
    const std::vector<std::string>& params, std::string& dumpInfo)
{
    std::vector<std::string> resetParams;
    resetParams.assign(params.begin() + 2, params.end()); // 2: params num
    if (resetParams.empty()) {
        WLOGI("do not dump ui info");
        return;
    }

    if (!session->GetSessionInfo().isSystem_) {
        WLOGFI("Dump normal session, not system");
        dumpInfoFuture_.ResetLock({});
        session->DumpSessionElementInfo(resetParams);
        std::vector<std::string> infos = dumpInfoFuture_.GetResult(2000); // 2000: wait for 2000ms
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    } else {
        WLOGFI("Dump system session");
        std::vector<std::string> infos;
        dumpRootSceneFunc_(resetParams, infos);
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    }
}

WSError SceneSessionManager::GetSpecifiedSessionDumpInfo(std::string& dumpInfo, const std::vector<std::string>& params,
    const std::string& strId)
{
    uint64_t persistentId = std::stoull(strId);
    auto session = GetSceneSession(persistentId);
    if (session == nullptr) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    WSRect rect = session->GetSessionRect();
    std::string isShown_ = "-";
    std::string isVisible = session->GetVisible() ? "true" : "false";
    std::string Focusable = session->GetFocusable() ? "true" : "false";
    std::string DecoStatus = session->GetSessionProperty()->IsDecorEnable() ? "true" : "false";
    bool PrivacyMode = session->GetSessionProperty()->GetSystemPrivacyMode() ||
        session->GetSessionProperty()->GetPrivacyMode();
    std::string isPrivacyMode = PrivacyMode ? "true" : "false";
    bool isFirstFrameAvailable = true;
    std::ostringstream oss;
    oss << "WindowName: " << session->GetWindowName()  << std::endl;
    oss << "DisplayId: " << 0 << std::endl;
    oss << "WinId: " << session->GetPersistentId() << std::endl;
    oss << "Pid: " << session->GetCallingPid() << std::endl;
    oss << "Type: " << static_cast<uint32_t>(session->GetWindowType()) << std::endl;
    oss << "Mode: " << static_cast<uint32_t>(session->GetWindowMode()) << std::endl;
    oss << "Flag: " << session->GetSessionProperty()->GetWindowFlags() << std::endl;
    oss << "Orientation: " << static_cast<uint32_t>(session->GetRequestedOrientation()) << std::endl;
    oss << "IsStartingWindow: " << isShown_ << std::endl;
    oss << "FirstFrameCallbackCalled: " << isFirstFrameAvailable << std::endl;
    oss << "IsVisible: " << isVisible << std::endl;
    oss << "Focusable: "  << Focusable << std::endl;
    oss << "DecoStatus: "  << DecoStatus << std::endl;
    oss << "isPrivacyMode: "  << isPrivacyMode << std::endl;
    oss << "WindowRect: " << "[ "
        << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_
        << " ]" << std::endl;
    oss << "TouchHotAreas: ";
    std::vector<Rect> touchHotAreas;
    oss << std::endl;
    dumpInfo.append(oss.str());

    DumpSessionElementInfo(session, params, dumpInfo);
    return WSError::WS_OK;
}

void SceneSessionManager::NotifyDumpInfoResult(const std::vector<std::string>& info)
{
    dumpInfoFuture_.SetValue(info);
    WLOGFD("NotifyDumpInfoResult");
}

WSError SceneSessionManager::GetSessionDumpInfo(const std::vector<std::string>& params, std::string& dumpInfo)
{
    if (params.size() == 1 && params[0] == ARG_DUMP_ALL) { // 1: params num
        return GetAllSessionDumpInfo(dumpInfo);
    }
    if (params.size() >= 2 && params[0] == ARG_DUMP_WINDOW && IsValidDigitString(params[1])) { // 2: params num
        return GetSpecifiedSessionDumpInfo(dumpInfo, params, params[1]);
    }
    return WSError::WS_ERROR_INVALID_OPERATION;
}

WSError SceneSessionManager::UpdateFocus(int32_t persistentId, bool isFocused)
{
    auto task = [this, persistentId, isFocused]() {
        WLOGFD("Update focus, id: %{public}d, isFocused: %{public}u", persistentId, static_cast<uint32_t>(isFocused));
        // notify session and client
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("could not find window");
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        // focusId change
        if (isFocused) {
            SetFocusedSession(persistentId);
        } else if (persistentId == GetFocusedSession()) {
            SetFocusedSession(INVALID_SESSION_ID);
        }
        // notify window manager
        sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo(
            sceneSession->GetWindowId(),
            static_cast<DisplayId>(0),
            sceneSession->GetCallingPid(),
            sceneSession->GetCallingUid(),
            sceneSession->GetWindowType(),
            sceneSession->GetAbilityToken()
        );
        SessionManagerAgentController::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, isFocused);
        WSError res = WSError::WS_OK;
        res = sceneSession->UpdateFocus(isFocused);
        if (res != WSError::WS_OK) {
            return res;
        }
        if (windowFocusChangedFunc_ != nullptr) {
            windowFocusChangedFunc_(persistentId, isFocused);
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

void SceneSessionManager::RegisterWindowFocusChanged(const WindowFocusChangedFunc& func)
{
    WLOGFE("RegisterWindowFocusChanged in");
    windowFocusChangedFunc_ = func;
}

void SceneSessionManager::UpdatePrivateStateAndNotify(bool isAddingPrivateSession)
{
    auto screenSession = ScreenSessionManager::GetInstance().GetScreenSession(0);
    if (screenSession == nullptr) {
        WLOGFE("screen session is null");
        return;
    }
    ScreenSessionManager::GetInstance().UpdatePrivateStateAndNotify(screenSession, isAddingPrivateSession);
}

void SceneSessionManager::RegisterSessionStateChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    NotifySessionStateChangeNotifyManagerFunc func = [this](int32_t persistentId, const SessionState& state) {
        this->OnSessionStateChange(persistentId, state);
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetSessionStateChangeNotifyManagerListener(func);
    WLOGFD("RegisterSessionStateChangeFunc success");
}

void SceneSessionManager::OnSessionStateChange(int32_t persistentId, const SessionState& state)
{
    WLOGFD("Session state change, id: %{public}d", persistentId);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    switch (state) {
        case SessionState::STATE_FOREGROUND:
            HandleKeepScreenOn(sceneSession, sceneSession->IsKeepScreenOn());
            if (sceneSession->GetWindowSessionProperty()->GetPrivacyMode()) {
                UpdatePrivateStateAndNotify(true);
            }
            break;
        case SessionState::STATE_BACKGROUND:
            HandleKeepScreenOn(sceneSession, false);
            if (sceneSession->GetWindowSessionProperty()->GetPrivacyMode()) {
                UpdatePrivateStateAndNotify(false);
            }
            break;
        default:
            break;
    }
}

void SceneSessionManager::SetWaterMarkSessionCount(int32_t count)
{
    waterMarkSessionCount_ = count;
}

int32_t SceneSessionManager::GetWaterMarkSessionCount() const
{
    return waterMarkSessionCount_;
}

WSError SceneSessionManager::SetWindowFlags(const sptr<SceneSession>& sceneSession, uint32_t flags)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto property = sceneSession->GetWindowSessionProperty();
    if (property == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    uint32_t oldFlags = property->GetWindowFlags();
    property->SetWindowFlags(flags);
    // notify when visibility change
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) {
        CheckAndNotifyWaterMarkChangedResult(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK));
    }
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) {
        sceneSession->OnShowWhenLocked(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    }
    WLOGFI("SetWindowFlags end, flags: %{public}u", flags);
    return WSError::WS_OK;
}

void SceneSessionManager::CheckAndNotifyWaterMarkChangedResult(bool isAddingWaterMark)
{
    int32_t preWaterMarkSessionCount = GetWaterMarkSessionCount();
    WLOGFD("before update : water mark count: %{public}u", preWaterMarkSessionCount);
    SetWaterMarkSessionCount(preWaterMarkSessionCount + (isAddingWaterMark ? 1 : -1));
    if (preWaterMarkSessionCount == 0 && isAddingWaterMark) {
        NotifyWaterMarkFlagChangedResult(true);
        return;
    }
    if (preWaterMarkSessionCount == 1 && !isAddingWaterMark) {
        NotifyWaterMarkFlagChangedResult(false);
        return;
    }
}

WSError SceneSessionManager::NotifyWaterMarkFlagChangedResult(bool hasWaterMark)
{
    WLOGFI("WaterMark status : %{public}u", static_cast<uint32_t>(hasWaterMark));
    SessionManagerAgentController::GetInstance().NotifyWaterMarkFlagChangedResult(hasWaterMark);
    return WSError::WS_OK;
}

WSError SceneSessionManager::SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label)
{
    WLOGFI("run SetSessionLabel");
    if (sessionListener_ == nullptr) {
        WLOGFI("sessionListener not register, skip.");
        return WSError::WS_OK;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto iter : sceneSessionMap_) {
        auto& sceneSession = iter.second;
        if (sceneSession->GetAbilityToken() == token) {
            WLOGFI("try to update session label.");
            sessionListener_->OnSessionLabelChange(iter.first, label);
            return WSError::WS_OK;
        }
    }
    return WSError::WS_ERROR_SET_SESSION_LABEL_FAILED;
}

WSError SceneSessionManager::SetSessionIcon(const sptr<IRemoteObject> &token,
    const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFI("run SetSessionIcon");
    if (sessionListener_ == nullptr) {
        WLOGFI("sessionListener not register, skip.");
        return WSError::WS_OK;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto iter : sceneSessionMap_) {
        auto& sceneSession = iter.second;
        if (sceneSession->GetAbilityToken() == token) {
            WLOGFI("try to update session icon.");
            sessionListener_->OnSessionIconChange(iter.first, icon);
            return WSError::WS_OK;
        }
    }
    return WSError::WS_ERROR_SET_SESSION_ICON_FAILED;
}

WSError SceneSessionManager::RegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run RegisterSessionListener");
    return listenerController_->AddSessionListener(listener);
}

WSError SceneSessionManager::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run UnRegisterSessionListener");
    listenerController_->DelSessionListener(listener);
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetSessionInfos(int32_t numMax, std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("run GetSessionInfos");
    std::map<int32_t, sptr<SceneSession>>::iterator iter;
    std::vector<sptr<SceneSession>> sceneSessionInfos;
    for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
        if (static_cast<int>(sceneSessionInfos.size()) >= numMax) {
            break;
        }
        sceneSessionInfos.emplace_back(iter->second);
    }
    return SceneSessionConverter::ConvertToMissionInfos(sceneSessionInfos, sessionInfos);
}

WSError SceneSessionManager::GetSessionInfo(int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFI("run GetSessionInfo");
    std::map<int32_t, sptr<SceneSession>>::iterator iter;
    sptr<SceneSession> sceneSession;
    for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
        if (persistentId == iter->first) {
            return SceneSessionConverter::ConvertToMissionInfo(iter->second, sessionInfo);
        }
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller)
{
    WLOGFI("run SetSessionIcon");
    if (info == nullptr) {
        WLOGFI("sessionInfo is nullptr.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    sptr<SceneSession> sceneSession = FindSessionByToken(info->sessionToken);
    if (sceneSession == nullptr) {
        WLOGFI("fail to find session by token.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    TerminateType terminateType = needStartCaller? TerminateType::CLOSE_AND_START_CALLER : TerminateType::CLOSE_AND_KEEP_MULTITASK;
    const WSError& errCode = sceneSession->TerminateSession(info, terminateType);
    return errCode;
}

WSError SceneSessionManager::ClearSession(int32_t persistentId)
{
    WLOGFI("run ClearSession with persistentId: %{public}d", persistentId);
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    return ClearSession(sceneSession);
}

WSError SceneSessionManager::ClearSession(sptr<SceneSession> sceneSession)
{
    WLOGFI("run ClearSession");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!IsSessionClearable(sceneSession)) {
        WLOGFI("sceneSession cannot be clear, persistentId %{public}d.", sceneSession -> GetPersistentId());
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    const WSError& errCode = sceneSession->Clear();
    return errCode;
}

WSError SceneSessionManager::ClearAllSessions()
{
    WLOGFI("run ClearSession");
    std::vector<sptr<SceneSession>> sessionVector;
    GetAllClearableSessions(sessionVector);
    for (int32_t i = 0; i < sessionVector.size(); i++) {
        ClearSession(sessionVector[i]);
    }
    return WSError::WS_OK;
}

void SceneSessionManager::GetAllClearableSessions(std::vector<sptr<SceneSession>> sessionVector)
{
    WLOGFI("run GetAllClearableSessions");
    for (const auto &item : sceneSessionMap_) {
        auto scnSession = item.second;
        if (IsSessionClearable(scnSession)) {
            sessionVector.push_back(scnSession);
        }
    }
}

bool SceneSessionManager::IsSessionClearable(sptr<SceneSession> scnSession)
{
    if (scnSession == nullptr) {
        WLOGFI("scnSession is nullptr");
        return false;
    }
    SessionInfo sessionInfo = scnSession -> GetSessionInfo();
    if (sessionInfo.excludeFromMissions) {
        WLOGFI("persistentId %{public}d is excludeFromMissions", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.unclearable) {
        WLOGFI("persistentId %{public}d is unclearable", scnSession->GetPersistentId());
        return false;
    }
    return true;
}

WSError SceneSessionManager::GetSessionSnapshot(int32_t persistentId, std::shared_ptr<Media::PixelMap> &snapshot)
{
    WLOGFI("run GetSessionSnapshot");
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    if (!sceneSession) {
        WLOGFI("fail to find session by persistentId: %{public}d", persistentId);
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    snapshot = sceneSession->Snapshot();
    return WSError::WS_OK;
}

WSError SceneSessionManager::RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener)
{
    WLOGFI("run RegisterSessionListener");
    if (sessionListener == nullptr) {
        return WSError::WS_ERROR_INVALID_SESSION_LISTENER;
    }
    sessionListener_ = sessionListener;
    return WSError::WS_OK;
}

void SceneSessionManager::UnregisterSessionListener()
{
    WLOGFI("run UnregisterSessionListener");
    sessionListener_ = nullptr;
}

WSError SceneSessionManager::RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }


        auto persistentId = scnSession->GetPersistentId();

        WLOGFI("RequestSceneSessionByCall persistentId: %{public}d", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto sessionInfo = scnSession->GetSessionInfo();
        WLOGFI("RequestSceneSessionByCall callState:%{public}d, persistentId: %{public}d",
            sessionInfo.callState_, persistentId);
        auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
        if (!abilitySessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        if (sessionInfo.callState_ == static_cast<uint32_t>(AAFwk::CallToState::BACKGROUND)) {
            scnSession->SetActive(false);
        } else if (sessionInfo.callState_ == static_cast<uint32_t>(AAFwk::CallToState::FOREGROUND)) {
            scnSession->SetActive(true);
        } else {
            WLOGFE("wrong callState_");
        }

        AAFwk::AbilityManagerClient::GetInstance()->CallUIAbilityBySCB(abilitySessionInfo);
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

void SceneSessionManager::StartAbilityBySpecified(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        WLOGFI("StartAbilityBySpecified: bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str());
        AAFwk::Want want;
        want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
        AAFwk::AbilityManagerClient::GetInstance()->StartSpecifiedAbilityBySCB(want);
    };

    taskScheduler_->PostAsyncTask(task);
}

sptr<SceneSession> SceneSessionManager::FindMainWindowWithToken(sptr<IRemoteObject> targetToken)
{
    if (!targetToken) {
        WLOGFE("Token is null, cannot find main window");
        return nullptr;
    }

    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(),
        [targetToken](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
            if (pair.second->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                return pair.second->GetAbilityToken() == targetToken;
            }
            return false;
        });
    if (iter == sceneSessionMap_.end()) {
        WLOGFE("Cannot find session");
        return nullptr;
    }
    return iter->second;
}

WSError SceneSessionManager::BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken)
{
    if (targetToken == nullptr) {
        WLOGFE("Target token is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto scnSession = GetSceneSession(static_cast<int32_t>(persistentId));
    if (scnSession == nullptr) {
        WLOGFE("Session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (scnSession->GetWindowType() != WindowType::WINDOW_TYPE_DIALOG) {
        WLOGFE("Session is not dialog window");
        return WSError::WS_OK;
    }
    scnSession->dialogTargetToken_ = targetToken;
    sptr<SceneSession> parentSession = FindMainWindowWithToken(targetToken);
    if (parentSession == nullptr) {
        scnSession->NotifyDestroy();
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    scnSession->SetParentSession(parentSession);
    WLOGFD("Bind dialog success, dialog id %{public}" PRIu64 ", parent id %{public}d",
        persistentId, parentSession->GetPersistentId());
    return WSError::WS_OK;
}

WMError SceneSessionManager::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto task = [this, &windowManagerAgent, type]() {
        return SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(windowManagerAgent, type);
    };
    return taskScheduler_->PostSyncTask(task);
}

WMError SceneSessionManager::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto task = [this, &windowManagerAgent, type]() {
        return SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(windowManagerAgent, type);
    };
    return taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SessionManagerAgentController::GetInstance().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}

void SceneSessionManager::StartWindowInfoReportLoop()
{
    if (isReportTaskStart_ || eventHandler_ == nullptr) {
        return;
    }
    auto task = [this]() {
        WindowInfoReporter::GetInstance().ReportRecordedInfos();
        isReportTaskStart_ = false;
        StartWindowInfoReportLoop();
    };
    int64_t delayTime = 1000 * 60 * 60; // an hour.
    bool ret = eventHandler_->PostTask(task, "WindowInfoReport", delayTime);
    if (!ret) {
        WLOGFE("post listener callback task failed. the task name is WindowInfoReport");
        return;
    }
    isReportTaskStart_ = true;
}

void SceneSessionManager::ResizeSoftInputCallingSessionIfNeed(const sptr<SceneSession>& sceneSession)
{
    if (callingSession_ == nullptr) {
        WLOGFE("calling session is nullptr");
        return;
    }
    SessionGravity gravity;
    uint32_t percent = 0;
    sceneSession->GetSessionProperty()->GetSessionGravity(gravity, percent);
    if (gravity != SessionGravity::SESSION_GRAVITY_BOTTOM) {
        WLOGFI("input method window gravity is not bottom, no need to raise calling window");
        return;
    }

    const WSRect& softInputSessionRect = sceneSession->GetSessionRect();
    const WSRect& callingSessionRect = callingSession_->GetSessionRect();
    if (SessionHelper::IsEmptyRect(SessionHelper::GetOverlap(softInputSessionRect, callingSessionRect, 0, 0))) {
        WLOGFD("There is no overlap area");
        return;
    }

    // calculate new rect of calling window
    WSRect newRect = callingSessionRect;
    newRect.posY_ = softInputSessionRect.posY_ - static_cast<int32_t>(newRect.height_);
    newRect.posY_ = std::max(newRect.posY_, STATUS_BAR_AVOID_AREA);

    callingWindowRestoringRect_ = callingSessionRect;
    NotifyOccupiedAreaChangeInfo(callingSession_, newRect, softInputSessionRect);
    callingSession_->UpdateSessionRect(newRect, SizeChangeReason::UNDEFINED);
}

void SceneSessionManager::NotifyOccupiedAreaChangeInfo(const sptr<SceneSession> callingSession,
    const WSRect& rect, const WSRect& occupiedArea)
{
    // if keyboard will occupy calling, notify calling window the occupied area and safe height
    const WSRect& safeRect = SessionHelper::GetOverlap(occupiedArea, rect, 0, 0);
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT,
        SessionHelper::TransferToRect(safeRect), safeRect.height_);
    WLOGFD("OccupiedAreaChangeInfo rect: %{public}u %{public}u %{public}u %{public}u",
        occupiedArea.posX_, occupiedArea.posY_, occupiedArea.width_, occupiedArea.height_);
    callingSession->NotifyOccupiedAreaChangeInfo(info);
}

void SceneSessionManager::RestoreCallingSessionSizeIfNeed()
{
    WLOGFD("RestoreCallingSessionSizeIfNeed");
    if (callingSession_ == nullptr) {
        WLOGFE("Calling session is nullptr");
        return;
    }
    if (!SessionHelper::IsEmptyRect(callingWindowRestoringRect_)) {
        WSRect overlapRect = { 0, 0, 0, 0 };
        NotifyOccupiedAreaChangeInfo(callingSession_, callingWindowRestoringRect_, overlapRect);
        callingSession_->UpdateSessionRect(callingWindowRestoringRect_, SizeChangeReason::UNDEFINED);
    }
    callingWindowRestoringRect_ = { 0, 0, 0, 0 };
    callingSession_ = nullptr;
}

WSError SceneSessionManager::SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (!sceneSession) {
        WLOGFE("scene session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        WLOGFE("scene session is not input method");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    sceneSession->GetSessionProperty()->SetSessionGravity(gravity, percent);
    RelayoutKeyBoard(sceneSession);
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        WLOGFD("input method is float mode");
        RestoreCallingSessionSizeIfNeed();
    } else {
        WLOGFD("input method is bottom mode");
        ResizeSoftInputCallingSessionIfNeed(sceneSession);
    }
    return WSError::WS_OK;
}

void SceneSessionManager::RelayoutKeyBoard(sptr<SceneSession> sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    SessionGravity gravity;
    uint32_t percent = 0;
    sceneSession->GetSessionProperty()->GetSessionGravity(gravity, percent);
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT ||
        gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        return;
    }

    auto defaultDisplayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("screenSession is null");
        return;
    }

    auto requestRect = sceneSession->GetSessionProperty()->GetRequestRect();
    if (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM) {
        if (percent != 0) {
            requestRect.width_ = static_cast<uint32_t>(defaultDisplayInfo->GetWidth());
            requestRect.height_ =
                static_cast<uint32_t>(defaultDisplayInfo->GetHeight()) * percent / 100u; // 100: for calc percent.
            requestRect.posX_ = 0;
        }
    }
    requestRect.posY_ = defaultDisplayInfo->GetHeight() -
        static_cast<int32_t>(requestRect.height_);
    sceneSession->GetSessionProperty()->SetRequestRect(requestRect);
    sceneSession->UpdateSessionRect(SessionHelper::TransferToWSRect(requestRect), SizeChangeReason::RESIZE);
}

void SceneSessionManager::InitPersistentStorage()
{
    if (ScenePersistentStorage::HasKey("maximize_state", ScenePersistentStorageType::MAXIMIZE_STATE)) {
        int32_t storageMode = -1;
        ScenePersistentStorage::Get("maximize_state", storageMode, ScenePersistentStorageType::MAXIMIZE_STATE);
        if (storageMode == static_cast<int32_t>(MaximizeMode::MODE_AVOID_SYSTEM_BAR) ||
            storageMode == static_cast<int32_t>(MaximizeMode::MODE_FULL_FILL)) {
            WLOGFI("init MaximizeMode as %{public}d from persistent storage", storageMode);
            SceneSession::maximizeMode_ = static_cast<MaximizeMode>(storageMode);
        }
    }
}

WMError SceneSessionManager::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    WLOGFI("GetAccessibilityWindowInfo Called.");
    std::map<int32_t, sptr<SceneSession>>::iterator iter;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
        sptr<SceneSession> sceneSession = iter->second;
        if (sceneSession == nullptr) {
            WLOGFW("null scene session");
            continue;
        }
        WLOGFD("name = %{public}s, isSystem = %{public}d, persistendId = %{public}d, winType = %{public}d, "
            "state = %{public}d, visible = %{public}d", sceneSession->GetWindowName().c_str(),
            sceneSession->GetSessionInfo().isSystem_, iter->first, sceneSession->GetWindowType(),
            sceneSession->GetSessionState(), sceneSession->IsVisible());
        if (IsSessionVisible(sceneSession)) {
            FillWindowInfo(infos, iter->second);
        }
    }
    return WMError::WM_OK;
}

void SceneSessionManager::NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type)
{
    WLOGFI("NotifyWindowInfoChange, persistentId = %{public}d, updateType = %{public}d", persistentId, type);
    std::vector<sptr<AccessibilityWindowInfo>> infos;

    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    FillWindowInfo(infos, sceneSession);
    SessionManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(infos, type);
}

void SceneSessionManager::FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("null scene session.");
        return;
    }
    sptr<AccessibilityWindowInfo> info = new (std::nothrow) AccessibilityWindowInfo();
    if (info == nullptr) {
        WLOGFE("null info.");
        return;
    }
    if (sceneSession->GetSessionInfo().isSystem_) {
        info->wid_ = 1;
    } else {
        info->wid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    }
    WSRect wsrect = sceneSession->GetSessionRect();
    info->windowRect_ = {wsrect.posX_, wsrect.posY_, wsrect.width_, wsrect.height_ };
    info->focused_ = sceneSession->GetPersistentId() == focusedSessionId_;
    info->type_ = sceneSession->GetWindowType();
    info->mode_ = sceneSession->GetWindowMode();
    info->layer_ = sceneSession->GetZOrder();
    auto property = sceneSession->GetSessionProperty();
    if (property != nullptr) {
        info->displayId_ = property->GetDisplayId();
        info->isDecorEnable_ = property->IsDecorEnable();
    }
    infos.emplace_back(info);
}

std::string SceneSessionManager::GetSessionSnapshotFilePath(int32_t persistentId)
{
    WLOGFI("GetSessionSnapshot persistentId %{public}d", persistentId);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("GetSessionSnapshot sceneSession nullptr!");
        return "";
    }
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return std::string("");
        }
        return scnSession->GetSessionSnapshotFilePath();
    };
    return taskScheduler_->PostSyncTask(task);
}

std::vector<std::pair<uint64_t, bool>> SceneSessionManager::GetWindowVisibilityChangeInfo(
    std::shared_ptr<RSOcclusionData> occlusionData)
{
    std::vector<std::pair<uint64_t, bool>> visibilityChangeInfo;
    VisibleData& currentVisibleWindow = occlusionData->GetVisibleData();
    std::sort(currentVisibleWindow.begin(), currentVisibleWindow.end());
    VisibleData& lastVisibleWindow = lastOcclusionData_->GetVisibleData();
    uint32_t i, j;
    i = j = 0;
    for (; i < lastVisibleWindow.size() && j < currentVisibleWindow.size();) {
        if (lastVisibleWindow[i] < currentVisibleWindow[j]) {
            visibilityChangeInfo.emplace_back(lastVisibleWindow[i], false);
            i++;
        } else if (lastVisibleWindow[i] > currentVisibleWindow[j]) {
            visibilityChangeInfo.emplace_back(currentVisibleWindow[j], true);
            j++;
        } else {
            i++;
            j++;
        }
    }
    for (; i < lastVisibleWindow.size(); ++i) {
        visibilityChangeInfo.emplace_back(lastVisibleWindow[i], false);
    }
    for (; j < currentVisibleWindow.size(); ++j) {
        visibilityChangeInfo.emplace_back(currentVisibleWindow[j], true);
    }
    lastOcclusionData_ = occlusionData;
    return visibilityChangeInfo;
}

void SceneSessionManager::WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusiontionData)
{
    WLOGFI("WindowVisibilityChangeCallback: entry");
    std::weak_ptr<RSOcclusionData> weak(occlusiontionData);

    taskScheduler_->PostVoidSyncTask([this, weak]() {
    auto weakOcclusionData = weak.lock();
    if (weakOcclusionData == nullptr) {
        WLOGFE("weak occlusionData is nullptr");
        return;
    }

    std::vector<std::pair<uint64_t, bool>> visibilityChangeInfo = GetWindowVisibilityChangeInfo(weakOcclusionData);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
    std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif
    for (const auto& elem : visibilityChangeInfo) {
        uint64_t surfaceId = elem.first;
        bool isVisible = elem.second;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        auto iter = sceneSessionMap_.begin();
        for (; iter != sceneSessionMap_.end(); iter++) {
            if (iter->second == nullptr || iter->second->GetSurfaceNode() == nullptr) {
                continue;
            }
            if (surfaceId == iter->second->GetSurfaceNode()->GetId()) {
                break;
            }
        }
        if (iter == sceneSessionMap_.end()) {
            continue;
        }
        sptr<SceneSession> session = iter->second;
        session->SetVisible(isVisible);
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(session->GetWindowId(), session->GetCallingPid(),
            session->GetCallingUid(), isVisible, session->GetWindowType()));
#ifdef MEMMGR_WINDOW_ENABLE
        memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(session->GetWindowId(), session->GetCallingPid(),
            session->GetCallingUid(), isVisible));
#endif
        WLOGFD("NotifyWindowVisibilityChange: covered status changed window:%{public}u, isVisible:%{public}d",
            session->GetWindowId(), isVisible);
        if (session->GetWindowSessionProperty()->GetWindowFlags() &
        static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) {
            CheckAndNotifyWaterMarkChangedResult(isVisible);
        }
}
        if (windowVisibilityInfos.size() != 0) {
            WLOGI("Notify windowvisibilityinfo changed start");
            SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
        }
#ifdef MEMMGR_WINDOW_ENABLE
        if (memMgrWindowInfos.size() != 0) {
            WLOGI("Notify memMgrWindowInfos changed start");
            Memory::MemMgrClient::GetInstance().OnWindowVisibilityChanged(memMgrWindowInfos);
        }
#endif
    });
}

void SceneSessionManager::InitWithRenderServiceAdded()
{
    auto windowVisibilityChangeCb = std::bind(&SceneSessionManager::WindowVisibilityChangeCallback, this,
        std::placeholders::_1);
    WLOGI("RegisterWindowVisibilityChangeCallback");
    if (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
        WLOGFE("RegisterWindowVisibilityChangeCallback failed");
    }
}

void SceneSessionManager::WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr!");
        return;
    }
    if (sceneSession->GetVisible()) {
        std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
        sceneSession->SetVisible(false);
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(sceneSession->GetWindowId(),
            sceneSession->GetCallingPid(), sceneSession->GetCallingUid(), false, sceneSession->GetWindowType()));
        WLOGFD("NotifyWindowVisibilityChange: covered status changed window:%{public}u, isVisible:%{public}d",
            sceneSession->GetWindowId(), sceneSession->GetVisible());
        if (sceneSession->GetWindowSessionProperty()->GetWindowFlags() &
        static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) {
            CheckAndNotifyWaterMarkChangedResult(false);
        }
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
}

sptr<SceneSession> SceneSessionManager::FindSessionByToken(const sptr<IRemoteObject> &token)
{
    sptr<SceneSession> session = nullptr;
    auto cmpFunc = [token](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
        if (pair.second == nullptr) {
            return false;
        }
        if (pair.second -> GetAbilityToken() == token) {
            return true;
        }
        return false;
    };
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(), cmpFunc);
    if (iter != sceneSessionMap_.end()) {
        session = iter->second;
    }
    return session;
}

WSError SceneSessionManager::PendingSessionToForeground(const sptr<IRemoteObject> &token)
{
    WLOGFI("run PendingSessionToForeground");
    auto session = FindSessionByToken(token);
    if (session != nullptr) {
        return session->PendingSessionToForeground();
    }
    WLOGFE("fail to find token");
    return WSError::WS_ERROR_INVALID_PARAM;
}

WSError SceneSessionManager::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token)
{
    WLOGFI("run PendingSessionToBackgroundForDelegator");
    auto session = FindSessionByToken(token);
    if (session != nullptr) {
        return session->PendingSessionToBackgroundForDelegator();
    }
    WLOGFE("fail to find token");
    return WSError::WS_ERROR_INVALID_PARAM;
}

WSError SceneSessionManager::GetFocusSessionToken(sptr<IRemoteObject> &token)
{
    WLOGFI("run GetFocusSessionToken with focusedSessionId: %{public}d", focusedSessionId_);
    auto sceneSession = GetSceneSession(focusedSessionId_);
    if (sceneSession) {
        token = sceneSession->GetAbilityToken();
        if (token == nullptr) {
            WLOGFE("token is nullptr");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        return WSError::WS_OK;
    }
    return WSError::WS_ERROR_INVALID_PARAM;
}

WSError SceneSessionManager::UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener)
{
    WLOGFI("UpdateSessionAvoidAreaListener persistentId: %{public}d haveListener:%{public}d",
        persistentId, haveListener);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr.");
        return WSError::WS_DO_NOTHING;
    }
    if (haveListener) {
        avoidAreaListenerSessionSet_.insert(sceneSession);
    } else {
        lastUpdatedAvoidArea_.erase(persistentId);
        avoidAreaListenerSessionSet_.erase(sceneSession);
    }
    return WSError::WS_OK;
}

bool SceneSessionManager::UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
    const AvoidArea& avoidArea, AvoidAreaType avoidAreaType)
{
    auto iter = lastUpdatedAvoidArea_.find(persistentId);
    bool needUpdate = true;

    if (iter != lastUpdatedAvoidArea_.end()) {
        auto avoidAreaIter = iter->second.find(avoidAreaType);
        if (avoidAreaIter != iter->second.end()) {
            needUpdate = avoidAreaIter->second != avoidArea;
        } else {
            if (avoidArea.isEmptyAvoidArea()) {
                needUpdate = false;
            }
        }
    } else {
        if (avoidArea.isEmptyAvoidArea()) {
            needUpdate = false;
        }
    }
    if (needUpdate) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("sceneSession is nullptr.");
            return false;
        }
        lastUpdatedAvoidArea_[persistentId][avoidAreaType] = avoidArea;
        sceneSession->UpdateAvoidArea(new AvoidArea(avoidArea), avoidAreaType);
    }

    return needUpdate;
}

bool SceneSessionManager::UpdateAvoidArea(const int32_t& persistentId)
{
    bool ret = true;
    bool needUpdate = false;
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr.");
        return false;
    }
    NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_BOUNDS);

    WindowType type = sceneSession->GetWindowType();
    SessionGravity gravity = SessionGravity::SESSION_GRAVITY_BOTTOM;
    uint32_t percent = 0;
    if (sceneSession->GetSessionProperty() != nullptr) {
        sceneSession->GetSessionProperty()->GetSessionGravity(gravity, percent);
    }
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR ||
        type == WindowType::WINDOW_TYPE_NAVIGATION_BAR ||
        (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
        gravity == SessionGravity::SESSION_GRAVITY_BOTTOM)) {
        AvoidAreaType avoidType = (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) ?
            AvoidAreaType::TYPE_KEYBOARD : AvoidAreaType::TYPE_SYSTEM;
        for (auto& session : avoidAreaListenerSessionSet_) {
            AvoidArea avoidArea = session->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
            ret = UpdateSessionAvoidAreaIfNeed(
                session->GetPersistentId(), avoidArea, static_cast<AvoidAreaType>(avoidType));
            needUpdate = needUpdate || ret;
        }
    } else {
        if (avoidAreaListenerSessionSet_.find(sceneSession) == avoidAreaListenerSessionSet_.end()) {
            WLOGD("id:%{public}d is not in avoidAreaListenerNodes, don't update avoid area.", persistentId);
            return false;
        }
        uint32_t start = static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM);
        uint32_t end = static_cast<uint32_t>(AvoidAreaType::TYPE_KEYBOARD);
        for (uint32_t avoidType = start; avoidType <= end; avoidType++) {
            AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
            ret = UpdateSessionAvoidAreaIfNeed(persistentId, avoidArea, static_cast<AvoidAreaType>(avoidType));
            needUpdate = needUpdate || ret;
        }
    }

    return needUpdate;
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    return;
}

void DisplayChangeListener::OnScreenshot(DisplayId displayId)
{
    SceneSessionManager::GetInstance().OnScreenshot(displayId);
}

void SceneSessionManager::OnScreenshot(DisplayId displayId)
{
    auto task = [this, displayId]() {
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            sceneSession->NotifyScreenshot();
        }
    };
    taskScheduler_->PostAsyncTask(task);
}
} // namespace OHOS::Rosen
