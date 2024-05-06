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
#include <cstdint>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <unistd.h>

#include <ability_context.h>
#include <ability_info.h>
#include <ability_manager_client.h>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <hisysevent.h>
#include <parameters.h>
#include "parameter.h"
#include <pointer_event.h>
#include <resource_manager.h>
#include <running_lock.h>
#include <session_info.h>
#include <start_options.h>
#include <system_ability_definition.h>
#include <want.h>
#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>
#include "transaction/rs_sync_transaction_controller.h"
#include "screen_manager.h"
#include "screen.h"
#include "include/core/SkRect.h"
#include "include/core/SkRegion.h"

#ifdef POWERMGR_DISPLAY_MANAGER_ENABLE
#include <display_power_mgr_client.h>
#endif

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

#ifdef RES_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif

#include "ability_start_setting.h"
#include "anr_manager.h"
#include "color_parser.h"
#include "common/include/session_permission.h"
#include "display_manager.h"
#include "image_source.h"
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "scene_input_manager.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scb_system_session.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "session_helper.h"
#include "window_helper.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"
#include "xcollie/watchdog.h"
#include "zidl/window_manager_agent_interface.h"
#include "session_manager_agent_controller.h"
#include "distributed_client.h"
#include "softbus_bus_center.h"
#include "window_manager.h"
#include "perform_reporter.h"
#include "focus_change_info.h"
#include "anr_manager.h"
#include "dms_reporter.h"

#include "window_visibility_info.h"
#include "window_drawing_content_info.h"
#include "anomaly_detection.h"
#ifdef MEMMGR_WINDOW_ENABLE
#include "mem_mgr_client.h"
#include "mem_mgr_window_info.h"
#endif

#ifdef EFFICIENCY_MANAGER_ENABLE
#include "suspend_manager_client.h"
#endif // EFFICIENCY_MANAGER_ENABLE

#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
#include "sec_comp_enhance_kit.h"
#endif

#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };
#ifdef RES_SCHED_ENABLE
const std::string SCENE_BOARD_BUNDLE_NAME = "com.ohos.sceneboard";
#endif
const std::string SCENE_SESSION_MANAGER_THREAD = "OS_SceneSessionManager";
const std::string WINDOW_INFO_REPORT_THREAD = "OS_WindowInfoReportThread";
constexpr const char* PREPARE_TERMINATE_ENABLE_PARAMETER = "persist.sys.prepare_terminate";
std::recursive_mutex g_instanceMutex;
constexpr uint32_t MAX_BRIGHTNESS = 255;
constexpr int32_t PREPARE_TERMINATE_ENABLE_SIZE = 6;
constexpr int32_t DEFAULT_USERID = -1;
constexpr int32_t SCALE_DIMENSION = 2;
constexpr int32_t TRANSLATE_DIMENSION = 2;
constexpr int32_t ROTAION_DIMENSION = 4;
constexpr int32_t CURVE_PARAM_DIMENSION = 4;
const std::string DM_PKG_NAME = "ohos.distributedhardware.devicemanager";
constexpr int32_t NON_ANONYMIZE_LENGTH = 6;
const std::string EMPTY_DEVICE_ID = "";
const int32_t MAX_NUMBER_OF_DISTRIBUTED_SESSIONS = 20;

constexpr int WINDOW_NAME_MAX_WIDTH = 21;
constexpr int DISPLAY_NAME_MAX_WIDTH = 10;
constexpr int VALUE_MAX_WIDTH = 5;
constexpr int ORIEN_MAX_WIDTH = 12;
constexpr int OFFSET_MAX_WIDTH = 8;
constexpr int PID_MAX_WIDTH = 8;
constexpr int PARENT_ID_MAX_WIDTH = 6;
constexpr int SCALE_MAX_WIDTH = 8;
constexpr int WINDOW_NAME_MAX_LENGTH = 20;
constexpr int32_t STATUS_BAR_AVOID_AREA = 0;
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_DUMP_DISPLAY = "-d";
constexpr uint64_t NANO_SECOND_PER_SEC = 1000000000; // ns
const int32_t LOGICAL_DISPLACEMENT_32 = 32;
std::string GetCurrentTime()
{
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    uint64_t uTime = static_cast<uint64_t>(tn.tv_sec) * NANO_SECOND_PER_SEC +
        static_cast<uint64_t>(tn.tv_nsec);
    return std::to_string(uTime);
}
int Comp(const std::pair<uint64_t, WindowVisibilityState> &a, const std::pair<uint64_t, WindowVisibilityState> &b)
{
    return a.first < b.first;
}

bool GetSingleIntItem(const WindowSceneConfig::ConfigItem& item, int32_t& value)
{
    if (item.IsInts() && item.intsValue_ && item.intsValue_->size() == 1) {
        value = (*item.intsValue_)[0];
        return true;
    }
    return false;
}

class BundleStatusCallback : public IRemoteStub<AppExecFwk::IBundleStatusCallback> {
public:
    BundleStatusCallback() = default;
    virtual ~BundleStatusCallback() = default;

    void OnBundleStateChanged(const uint8_t installType,
        const int32_t resultCode, const std::string& resultMsg, const std::string& bundleName) override {}

    void OnBundleAdded(const std::string& bundleName, const int userId) override
    {
        SceneSessionManager::GetInstance().OnBundleUpdated(bundleName, userId);
    }

    void OnBundleUpdated(const std::string& bundleName, const int userId) override
    {
        SceneSessionManager::GetInstance().OnBundleUpdated(bundleName, userId);
    }

    void OnBundleRemoved(const std::string& bundleName, const int userId) override
    {
        SceneSessionManager::GetInstance().OnBundleUpdated(bundleName, userId);
    }
};
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
    launcherService_ = new AppExecFwk::LauncherService();
    if (!launcherService_->RegisterCallback(new BundleStatusCallback())) {
        WLOGFE("Failed to register bundle status callback.");
    }
}

void SceneSessionManager::Init()
{
    constexpr uint64_t interval = 5 * 1000; // 5 second
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCENE_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCENE_SESSION_MANAGER_THREAD.c_str());
    }

    InitScheduleUtils();

    bundleMgr_ = GetBundleManager();
    LoadWindowSceneXml();
    sptr<IDisplayChangeListener> listener = new DisplayChangeListener();
    ScreenSessionManagerClient::GetInstance().RegisterDisplayChangeListener(listener);
    InitPrepareTerminateConfig();

    // create handler for inner command at server
    eventLoop_ = AppExecFwk::EventRunner::Create(WINDOW_INFO_REPORT_THREAD);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        WLOGFE("Invalid eventHander");
        return ;
    }
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread(WINDOW_INFO_REPORT_THREAD, eventHandler_);
    if (ret != 0) {
        WLOGFW("Add thread %{public}s to watchdog failed.", WINDOW_INFO_REPORT_THREAD.c_str());
    }

    listenerController_ = std::make_shared<SessionListenerController>();
    listenerController_->Init();
    scbSessionHandler_ = new ScbSessionHandler();
    AAFwk::AbilityManagerClient::GetInstance()->RegisterSessionHandler(scbSessionHandler_);
    StartWindowInfoReportLoop();
    WLOGI("SceneSessionManager init success.");
    RegisterAppListener();
    openDebugTrace = std::atoi((system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str()) != 0;
    isKeyboardPanelEnabled_ = system::GetParameter("persist.sceneboard.keyboardPanel.enabled", "0")  == "1";
}

void SceneSessionManager::InitScheduleUtils()
{
#ifdef RES_SCHED_ENABLE
    std::unordered_map<std::string, std::string> payload {
        { "pid", std::to_string(getprocpid()) },
        { "tid", std::to_string(getproctid()) },
        { "uid", std::to_string(getuid()) },
        { "bundleName", SCENE_BOARD_BUNDLE_NAME },
    };
    uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_REPORT_SCENE_BOARD;
    int64_t value = 0;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, payload);
    auto task = []() {
        const int32_t userInteraction = 2;
        std::unordered_map<std::string, std::string> payload{
            {"pid", std::to_string(getpid())},
            {"tid", std::to_string(gettid())},
            {"uid", std::to_string(getuid())},
            {"extType", "10002"},
            {"cgroupPrio", "1"},
            {"isSa", "0"},
            {"threadName", "OS_SceneSession"}
        };
        uint32_t type = ResourceSchedule::ResType::RES_TYPE_KEY_PERF_SCENE;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, userInteraction, payload);
        TLOGI(WmsLogTag::WMS_LIFE, "set RES_TYPE_KEY_PERF_SCENE success");
    };
    taskScheduler_->PostAsyncTask(task, "changeQosTask");
#endif
}

void SceneSessionManager::RegisterAppListener()
{
    appAnrListener_ = new (std::nothrow) AppAnrListener();
    auto appMgrClient_ = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient_ == nullptr) {
        WLOGFE("appMgrClient_ is nullptr.");
    } else {
        if (appAnrListener_ != nullptr) {
            auto flag = static_cast<int32_t>(appMgrClient_->RegisterAppDebugListener(appAnrListener_));
            if (flag != ERR_OK) {
                WLOGFE("Register app debug listener failed.");
            } else {
                WLOGFI("Register app debug listener success.");
            }
        }
    }
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
    ConfigDefaultKeyboardAnimation();
}

void SceneSessionManager::InitPrepareTerminateConfig()
{
    char value[PREPARE_TERMINATE_ENABLE_SIZE] = "false";
    int32_t retSysParam = GetParameter(PREPARE_TERMINATE_ENABLE_PARAMETER, "false", value,
        PREPARE_TERMINATE_ENABLE_SIZE);
    WLOGFI("InitPrepareTerminateConfig, %{public}s value is %{public}s.", PREPARE_TERMINATE_ENABLE_PARAMETER, value);
    if (retSysParam > 0 && !std::strcmp(value, "true")) {
        isPrepareTerminateEnable_ = true;
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

    item = config["backgroundswitch"];
    int32_t param = -1;
    systemConfig_.backgroundswitch = GetSingleIntItem(item, param) && param == 1;
    WLOGFD("Load ConfigWindowSceneXml backgroundswitch%{public}d", systemConfig_.backgroundswitch);
    item = config["defaultWindowMode"];
    if (GetSingleIntItem(item, param) &&
        (param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
        param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
        systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(param));
    }
    item = config["defaultMaximizeMode"];
    if (GetSingleIntItem(item, param) &&
        (param == static_cast<int32_t>(MaximizeMode::MODE_AVOID_SYSTEM_BAR) ||
         param == static_cast<int32_t>(MaximizeMode::MODE_FULL_FILL))) {
        SceneSession::maximizeMode_ = static_cast<MaximizeMode>(param);
    }
    item = config["keyboardAnimation"];
    if (item.IsMap()) {
        ConfigKeyboardAnimation(item);
    }
    item = config["maxFloatingWindowSize"];
    if (GetSingleIntItem(item, param)) {
        systemConfig_.maxFloatingWindowSize_ = static_cast<uint32_t>(param);
    }
    item = config["windowAnimation"];
    if (item.IsMap()) {
        ConfigWindowAnimation(item);
    }
    item = config["startWindowTransitionAnimation"];
    if (item.IsMap()) {
        ConfigStartingWindowAnimation(item);
    }
    ConfigFreeMultiWindow();
    ConfigWindowSizeLimits();
    ConfigSnapshotScale();

    item = config["systemUIStatusBar"];
    if (item.IsMap()) {
        ConfigSystemUIStatusBar(item);
    }
}

void SceneSessionManager::ConfigFreeMultiWindow()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem freeMultiWindowConfig = config["freeMultiWindow"];
    if (freeMultiWindowConfig.IsMap()) {
        auto supportItem = freeMultiWindowConfig.GetProp("enable");
        if (supportItem.IsBool()) {
            systemConfig_.freeMultiWindowSupport_ = supportItem.boolValue_;
        }
        auto item = freeMultiWindowConfig["decor"];
        if (item.IsMap()) {
            ConfigDecor(item, false);
        }
        int32_t param = -1;
        item = freeMultiWindowConfig["defaultWindowMode"];
        if (GetSingleIntItem(item, param) &&
            (param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
            param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.freeMultiWindowConfig_.defaultWindowMode_ =
                static_cast<WindowMode>(static_cast<uint32_t>(param));
        }
        item = freeMultiWindowConfig["maxMainFloatingWindowNumber"];
        if (GetSingleIntItem(item, param) && (param > 0)) {
            systemConfig_.freeMultiWindowConfig_.maxMainFloatingWindowNumber_ = param;
        }
    }
}

void SceneSessionManager::LoadFreeMultiWindowConfig(bool enable)
{
    FreeMultiWindowConfig freeMultiWindowConfig = systemConfig_.freeMultiWindowConfig_;
    if (enable) {
        systemConfig_.defaultWindowMode_ = freeMultiWindowConfig.defaultWindowMode_;
        systemConfig_.decorModeSupportInfo_ = freeMultiWindowConfig.decorModeSupportInfo_;
        systemConfig_.isSystemDecorEnable_ = freeMultiWindowConfig.isSystemDecorEnable_;
    } else {
        const auto& config = WindowSceneConfig::GetConfig();
        auto item = config["decor"];
        if (item.IsMap()) {
            ConfigDecor(item, true);
        }
        int32_t param = -1;
        item = config["defaultWindowMode"];
        if (GetSingleIntItem(item, param) &&
            (param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
            param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(param));
        }
    }
    systemConfig_.freeMultiWindowEnable_ = enable;
}

const SystemSessionConfig& SceneSessionManager::GetSystemSessionConfig() const
{
    return systemConfig_;
}

WSError SceneSessionManager::SwitchFreeMultiWindow(bool enable)
{
    if (!systemConfig_.freeMultiWindowSupport_) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "device not support");
        return WSError::WS_ERROR_DEVICE_NOT_SUPPORT;
    }
    LoadFreeMultiWindowConfig(enable);
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto item = sceneSessionMap_.begin(); item != sceneSessionMap_.end(); ++item) {
        auto sceneSession = item->second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
            continue;
        }
        sceneSession->SwitchFreeMultiWindow(enable);
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::SetSessionContinueState(const sptr<IRemoteObject> &token,
    const ContinueState& continueState)
{
    WLOGFI("run SetSessionContinueState");
    auto task = [this, token, continueState]() {
        sptr <SceneSession> sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoContinueState(continueState);
        DistributedClient dmsClient;
        dmsClient.SetMissionContinueState(sceneSession->GetPersistentId(),
            static_cast<AAFwk::ContinueState>(continueState));
        WLOGFI("SetSessionContinueState sessionId:%{public}d, continueState:%{public}d",
            sceneSession->GetPersistentId(), continueState);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionContinueState");
}

void SceneSessionManager::ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig, bool mainConfig)
{
    WindowSceneConfig::ConfigItem item = decorConfig.GetProp("enable");
    if (item.IsBool()) {
        if (mainConfig) {
            systemConfig_.isSystemDecorEnable_ = item.boolValue_;
        } else {
            systemConfig_.freeMultiWindowConfig_.isSystemDecorEnable_ = item.boolValue_;
        }
        bool decorEnable = item.boolValue_;
        uint32_t support = 0;
        std::vector<std::string> supportedModes;
        item = decorConfig["supportedMode"];
        if (item.IsStrings()) {
            supportedModes = *item.stringsValue_;
        }
        for (auto mode : supportedModes) {
            if (mode == "fullscreen") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN;
            } else if (mode == "floating") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
            } else if (mode == "pip") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
            } else if (mode == "split") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
            } else {
                WLOGFW("Invalid supporedMode");
                support = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
                break;
            }
        }
        if (mainConfig && item.IsStrings()) {
            systemConfig_.decorModeSupportInfo_ = support;
        }
        if (!mainConfig && item.IsStrings()) {
            systemConfig_.freeMultiWindowConfig_.decorModeSupportInfo_ = support;
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

void SceneSessionManager::SetEnableInputEvent(bool enabled)
{
    WLOGFI("[WMSRecover] Set enable input event: %{public}u", enabled);
    enableInputEvent_ = enabled;
}

bool SceneSessionManager::IsInputEventEnabled()
{
    return enableInputEvent_;
}

void SceneSessionManager::ClearUnrecoveredSessions(const std::vector<int32_t>& recoveredPersistentIds)
{
    for (const auto& persistentId : alivePersistentIds_) {
        auto it = std::find(recoveredPersistentIds.begin(), recoveredPersistentIds.end(), persistentId);
        if (it == recoveredPersistentIds.end()) {
            TLOGI(WmsLogTag::WMS_RECOVER, "Clear unrecovered session with persistentId=%{public}d", persistentId);
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.erase(persistentId);
        }
    }
}

void SceneSessionManager::UpdateRecoveredSessionInfo(const std::vector<int32_t>& recoveredPersistentIds)
{
    WLOGFI("[WMSRecover] Number of persistentIds recovered = %{public}zu. CurrentUserId = "
           "%{public}d",
        recoveredPersistentIds.size(), currentUserId_);

    auto task = [this, recoveredPersistentIds]() {
        ClearUnrecoveredSessions(recoveredPersistentIds);
        std::list<AAFwk::SessionInfo> abilitySessionInfos;
        for (const auto& persistentId : recoveredPersistentIds) {
            auto sceneSession = GetSceneSession(persistentId);
            if (sceneSession == nullptr) {
                continue;
            }
            WLOGFD("[WMSRecover] recovered persistentId = %{public}d", persistentId);
            const auto& abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
            if (!abilitySessionInfo) {
                WLOGFW("[WMSRecover] abilitySessionInfo is null");
                continue;
            }
            abilitySessionInfos.emplace_back(*abilitySessionInfo);
        }
        std::vector<int32_t> unrecoverableSessionIds;
        AAFwk::AbilityManagerClient::GetInstance()->UpdateSessionInfoBySCB(
            abilitySessionInfos, currentUserId_, unrecoverableSessionIds);
        WLOGFI("[WMSRecover] Number of unrecoverableSessionIds = %{public}zu", unrecoverableSessionIds.size());
        for (const auto& sessionId : unrecoverableSessionIds) {
            auto sceneSession = GetSceneSession(sessionId);
            if (sceneSession == nullptr) {
                WLOGFW("[WMSRecover]There is no session corresponding to sessionId = %{public}d ", sessionId);
                continue;
            }
            WLOGFI("[WMSRecover] unrecoverable sessionId = %{public}d", sessionId);
            const auto& scnSessionInfo = SetAbilitySessionInfo(sceneSession);
            if (!scnSessionInfo) {
                WLOGFW("[WMSRecover] scnSessionInfo is null");
                continue;
            }
            sceneSession->NotifySessionException(scnSessionInfo, false);
        }
    };
    return taskScheduler_->PostAsyncTask(task, "UpdateSessionInfoBySCB");
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
    LoadKeyboardAnimation(animationConfig["animationIn"]["timing"], appWindowSceneConfig_.keyboardAnimationIn_);
    LoadKeyboardAnimation(animationConfig["animationOut"]["timing"], appWindowSceneConfig_.keyboardAnimationOut_);

    const auto& defaultAnimation = appWindowSceneConfig_.keyboardAnimationIn_;
    systemConfig_.keyboardAnimationConfig_.curveType_ = defaultAnimation.curveType_;
    systemConfig_.keyboardAnimationConfig_.curveParams_.assign({
        defaultAnimation.ctrlX1_,
        defaultAnimation.ctrlY1_,
        defaultAnimation.ctrlX2_,
        defaultAnimation.ctrlY2_,
    });
    systemConfig_.keyboardAnimationConfig_.durationIn_ = appWindowSceneConfig_.keyboardAnimationIn_.duration_;
    systemConfig_.keyboardAnimationConfig_.durationOut_ = appWindowSceneConfig_.keyboardAnimationOut_.duration_;
}

void SceneSessionManager::LoadKeyboardAnimation(const WindowSceneConfig::ConfigItem& item,
    KeyboardSceneAnimationConfig& config)
{
    if (item.IsMap() && item.mapValue_->count("curve")) {
        const auto& [curveType, curveParams] = CreateCurve(item["curve"]);
        config.curveType_ = curveType;
        if (curveParams.size() == CURVE_PARAM_DIMENSION) {
            config.ctrlX1_ = curveParams[0]; // 0: ctrl x1 index
            config.ctrlY1_ = curveParams[1]; // 1: ctrl y1 index
            config.ctrlX2_ = curveParams[2]; // 2: ctrl x2 index
            config.ctrlY2_ = curveParams[3]; // 3: ctrl y2 index
        }
    }

    const WindowSceneConfig::ConfigItem& duration = item["duration"];
    if (duration.IsInts()) {
        auto numbers = *duration.intsValue_;
        if (numbers.size() == 1) {
            config.duration_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigDefaultKeyboardAnimation()
{
    constexpr char CURVETYPE[] = "interpolatingSpring";
    constexpr float CTRLX1 = 0;
    constexpr float CTRLY1 = 1;
    constexpr float CTRLX2 = 342;
    constexpr float CTRLY2 = 37;
    constexpr uint32_t DURATION = 150;

    if (!systemConfig_.keyboardAnimationConfig_.curveType_.empty()) {
        return;
    }

    systemConfig_.keyboardAnimationConfig_.curveType_ = CURVETYPE;
    std::vector<float> keyboardCurveParams = {CTRLX1, CTRLY1, CTRLX2, CTRLY2};
    systemConfig_.keyboardAnimationConfig_.curveParams_.assign(
        keyboardCurveParams.begin(), keyboardCurveParams.end());;
    systemConfig_.keyboardAnimationConfig_.durationIn_ = DURATION;
    systemConfig_.keyboardAnimationConfig_.durationOut_ = DURATION;
}

void SceneSessionManager::ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig)
{
    WindowSceneConfig::ConfigItem item = windowAnimationConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        const auto& [curveType, curveParams] = CreateCurve(item["curve"]);
        appWindowSceneConfig_.windowAnimation_.curveType_ = curveType;
        if (curveParams.size() == CURVE_PARAM_DIMENSION) {
            appWindowSceneConfig_.windowAnimation_.ctrlX1_ = curveParams[0]; // 0: ctrl x1 index
            appWindowSceneConfig_.windowAnimation_.ctrlY1_ = curveParams[1]; // 1: ctrl y1 index
            appWindowSceneConfig_.windowAnimation_.ctrlX2_ = curveParams[2]; // 2: ctrl x2 index
            appWindowSceneConfig_.windowAnimation_.ctrlY2_ = curveParams[3]; // 3: ctrl y2 index
        }
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
        config.curve_ = std::get<std::string>(CreateCurve(item["curve"]));
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

std::tuple<std::string, std::vector<float>> SceneSessionManager::CreateCurve(
    const WindowSceneConfig::ConfigItem& curveConfig)
{
    static std::unordered_set<std::string> curveSet = { "easeOut", "ease", "easeIn", "easeInOut", "default",
        "linear", "spring", "interactiveSpring", "interpolatingSpring" };
    static std::unordered_set<std::string> paramCurveSet = {
        "spring", "interactiveSpring", "interpolatingSpring", "cubic" };

    std::string curveName = "easeOut";
    const auto& nameItem = curveConfig.GetProp("name");
    if (!nameItem.IsString()) {
        return {curveName, {}};
    }

    std::string name = nameItem.stringValue_;
    std::vector<float> curveParams;

    if (paramCurveSet.find(name) != paramCurveSet.end()) {
        curveName = name;
        curveParams = std::vector<float>(CURVE_PARAM_DIMENSION);
        if (curveConfig.IsFloats() && curveConfig.floatsValue_->size() <= CURVE_PARAM_DIMENSION) {
            std::copy(curveConfig.floatsValue_->begin(), curveConfig.floatsValue_->end(),
                curveParams.begin());
        }
    } else {
        auto iter = curveSet.find(name);
        if (iter != curveSet.end()) {
            curveName = name;
        }
    }

    return {curveName, curveParams};
}

void SceneSessionManager::ConfigWindowSizeLimits()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["mainWindowSizeLimits"];
    if (item.IsMap()) {
        ConfigMainWindowSizeLimits(item);
    }

    item = config["subWindowSizeLimits"];
    if (item.IsMap()) {
        ConfigSubWindowSizeLimits(item);
    }
}

void SceneSessionManager::ConfigMainWindowSizeLimits(const WindowSceneConfig::ConfigItem& mainWindowSizeConifg)
{
    auto item = mainWindowSizeConifg["miniWidth"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniWidthOfMainWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }

    item = mainWindowSizeConifg["miniHeight"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniHeightOfMainWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigSubWindowSizeLimits(const WindowSceneConfig::ConfigItem& subWindowSizeConifg)
{
    auto item = subWindowSizeConifg["miniWidth"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniWidthOfSubWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }

    item = subWindowSizeConifg["miniHeight"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniHeightOfSubWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigSnapshotScale()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["snapshotScale"];
    if (item.IsFloats()) {
        auto snapshotScale = *item.floatsValue_;
        if (snapshotScale.size() != 1 || snapshotScale[0] <= 0 || snapshotScale[0] > 1) {
            return;
        }
        snapshotScale_ = snapshotScale[0];
    }
}

void SceneSessionManager::ConfigSystemUIStatusBar(const WindowSceneConfig::ConfigItem& statusBarConfig)
{
    TLOGI(WmsLogTag::WMS_IMMS, "load ConfigSystemUIStatusBar");
    WindowSceneConfig::ConfigItem item = statusBarConfig["showInLandscapeMode"];
    if (item.IsInts() && item.intsValue_->size() == 1) {
        bool showInLandscapeMode = (*item.intsValue_)[0] > 0;
        appWindowSceneConfig_.systemUIStatusBarConfig_.showInLandscapeMode_ = showInLandscapeMode;
        TLOGI(WmsLogTag::WMS_IMMS, "ConfigSystemUIStatusBar showInLandscapeMode:%{public}d",
            appWindowSceneConfig_.systemUIStatusBarConfig_.showInLandscapeMode_);
    }

    item = statusBarConfig["immersiveStatusBarBgColor"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return;
        }
        appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarBgColor_ = color;
        TLOGI(WmsLogTag::WMS_IMMS, "ConfigSystemUIStatusBar immersiveStatusBarBgColor:%{public}s",
            appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarBgColor_.c_str());
    }

    item = statusBarConfig["immersiveStatusBarContentColor"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return;
        }
        appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarContentColor_ = color;
        TLOGI(WmsLogTag::WMS_IMMS, "ConfigSystemUIStatusBar immersiveStatusBarContentColor:%{public}s",
            appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarContentColor_.c_str());
    }
}

void SceneSessionManager::SetRootSceneContext(const std::weak_ptr<AbilityRuntime::Context>& contextWeak)
{
    rootSceneContextWeak_ = contextWeak;
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this]() -> sptr<RootSceneSession> {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }
        system::SetParameter("bootevent.wms.fullscreen.ready", "true");
        rootSceneSession_ = new RootSceneSession();
        rootSceneSession_->SetEventHandler(taskScheduler_->GetEventHandler());
        AAFwk::AbilityManagerClient::GetInstance()->SetRootSceneSession(rootSceneSession_->AsObject());
        return rootSceneSession_;
    };

    return taskScheduler_->PostSyncTask(task, "GetRootSceneSession");
}

sptr<SceneSession> SceneSessionManager::GetSceneSession(int32_t persistentId)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = sceneSessionMap_.find(persistentId);
    if (iter == sceneSessionMap_.end()) {
        WLOGFD("Error found scene session with id: %{public}d", persistentId);
        return nullptr;
    }
    return iter->second;
}

sptr<SceneSession> SceneSessionManager::GetSceneSessionByName(const std::string& bundleName,
    const std::string& moduleName, const std::string& abilityName, const int32_t appIndex)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetSessionInfo().bundleName_ == bundleName &&
            sceneSession->GetSessionInfo().moduleName_ == moduleName &&
            sceneSession->GetSessionInfo().abilityName_ == abilityName &&
            sceneSession->GetSessionInfo().appIndex_ == appIndex) {
            return sceneSession;
        }
    }
    return nullptr;
}

std::vector<sptr<SceneSession>> SceneSessionManager::GetSceneSessionVectorByType(
    WindowType type, uint64_t displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_LIFE, "displayId is invalid");
        return {};
    }
    std::vector<sptr<SceneSession>> sceneSessionVector;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetWindowType() == type &&
            sceneSession->GetSessionProperty() &&
            sceneSession->GetSessionProperty()->GetDisplayId() == displayId) {
            sceneSessionVector.emplace_back(sceneSession);
        }
    }

    return sceneSessionVector;
}

WSError SceneSessionManager::UpdateParentSessionForDialog(const sptr<SceneSession>& sceneSession,
    sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        TLOGD(WmsLogTag::WMS_DIALOG, "Property is null, no need to update parent info");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto parentPersistentId = property->GetParentPersistentId();
    sceneSession->SetParentPersistentId(parentPersistentId);
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG && parentPersistentId != INVALID_SESSION_ID) {
        auto parentSession = GetSceneSession(parentPersistentId);
        if (parentSession == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Parent session is nullptr, parentId:%{public}d", parentPersistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        parentSession->BindDialogSessionTarget(sceneSession);
        parentSession->BindDialogToParentSession(sceneSession);
        sceneSession->SetParentSession(parentSession);
        TLOGI(WmsLogTag::WMS_DIALOG, "Update parent of dialog success, id %{public}d, parentId %{public}d",
            sceneSession->GetPersistentId(), parentPersistentId);
    }
    return WSError::WS_OK;
}

sptr<SceneSession::SpecificSessionCallback> SceneSessionManager::CreateSpecificSessionCallback()
{
    sptr<SceneSession::SpecificSessionCallback> specificCb = new (std::nothrow)SceneSession::SpecificSessionCallback();
    if (specificCb == nullptr) {
        WLOGFE("SpecificSessionCallback is nullptr");
        return nullptr;
    }
    specificCb->onCreate_ = std::bind(&SceneSessionManager::RequestSceneSession,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onDestroy_ = std::bind(&SceneSessionManager::DestroyAndDisconnectSpecificSessionInner,
        this, std::placeholders::_1);
    specificCb->onCameraFloatSessionChange_ = std::bind(&SceneSessionManager::UpdateCameraFloatWindowStatus,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onGetSceneSessionVectorByType_ = std::bind(&SceneSessionManager::GetSceneSessionVectorByType,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onUpdateAvoidArea_ = std::bind(&SceneSessionManager::UpdateAvoidArea, this, std::placeholders::_1);
    specificCb->onWindowInfoUpdate_ = std::bind(&SceneSessionManager::NotifyWindowInfoChange,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onWindowInputPidChangeCallback_ = std::bind(&SceneSessionManager::NotifyMMIWindowPidChange,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onSessionTouchOutside_ = std::bind(&SceneSessionManager::NotifySessionTouchOutside,
        this, std::placeholders::_1);
    specificCb->onGetAINavigationBarArea_ = std::bind(&SceneSessionManager::GetAINavigationBarArea,
        this, std::placeholders::_1);
    specificCb->onOutsideDownEvent_ = std::bind(&SceneSessionManager::OnOutsideDownEvent,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onHandleSecureSessionShouldHide_ = std::bind(&SceneSessionManager::HandleSecureSessionShouldHide,
        this, std::placeholders::_1);
    specificCb->onCameraSessionChange_ = std::bind(&SceneSessionManager::UpdateCameraWindowStatus,
        this, std::placeholders::_1, std::placeholders::_2);
    return specificCb;
}

sptr<KeyboardSession::KeyboardSessionCallback> SceneSessionManager::CreateKeyboardSessionCallback()
{
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow)KeyboardSession::KeyboardSessionCallback();
    if (keyboardCb == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "KeyboardSessionCallback is nullptr");
        return keyboardCb;
    }
    keyboardCb->onGetSceneSession_ = std::bind(&SceneSessionManager::GetSceneSession, this, std::placeholders::_1);
    keyboardCb->onGetFocusedSessionId_ = std::bind(&SceneSessionManager::GetFocusedSessionId, this);
    keyboardCb->onCallingSessionIdChange_ = callingSessionIdChangeFunc_;

    return keyboardCb;
}

WMError SceneSessionManager::CheckWindowId(int32_t windowId, int32_t &pid)
{
    auto task = [this, windowId, &pid]() -> WMError {
        pid = INVALID_PID;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        auto iter = sceneSessionMap_.find(windowId);
        if (iter == sceneSessionMap_.end()) {
            WLOGFE("Window(%{public}d) cannot set cursor style", windowId);
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
        auto sceneSession = iter->second;
        if (sceneSession == nullptr) {
            WLOGFE("sceneSession(%{public}d) is nullptr", windowId);
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
        pid = sceneSession->GetCallingPid();
        WLOGFD("Window(%{public}d) to set the cursor style, pid:%{public}d", windowId, pid);
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "CheckWindowId:" + std::to_string(windowId));
}

void SceneSessionManager::CreateKeyboardPanelSession(sptr<SceneSession> keyboardSession)
{
    if (!isKeyboardPanelEnabled_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "KeyboardPanel is not enabled");
        return;
    }
    if (keyboardSession == nullptr || keyboardSession->GetSessionProperty() == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "KeyboardSession or property is nullptr");
        return;
    }
    DisplayId displayId = keyboardSession->GetSessionProperty()->GetDisplayId();
    const auto& panelVec = GetSceneSessionVectorByType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL, displayId);
    sptr<SceneSession> panelSession;
    if (panelVec.size() > 1) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Error size of keyboardPanel, size: %{public}zu", panelVec.size());
        return;
    } else if (panelVec.size() == 1) {
        panelSession = panelVec.front();
        TLOGI(WmsLogTag::WMS_KEYBOARD, "KeyboardPanel is created, panelId:%{public}d", panelSession->GetPersistentId());
    } else {
        SessionInfo panelInfo = {
            .bundleName_ = "SCBKeyboardPanel",
            .moduleName_ = "SCBKeyboardPanel",
            .abilityName_ = "SCBKeyboardPanel",
            .isSystem_ = true,
            .windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_KEYBOARD_PANEL),
            .screenId_ = static_cast<uint64_t>(displayId),
            .isSystemInput_ = false,
            .isRotable_ = true,
        };
        static bool is2in1 = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
        if (is2in1) {
            panelInfo.isSystemInput_ = true;
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Set panel canvasNode");
        } else {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Set panel surfaceNode");
        }
        panelSession = RequestSceneSession(panelInfo, nullptr);
        if (panelSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "PanelSession is nullptr");
            return;
        }
    }
    keyboardSession->BindKeyboardPanelSession(panelSession);
    panelSession->BindKeyboardSession(keyboardSession);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "success, panelId:%{public}d, keyboardId:%{public}d",
        panelSession->GetPersistentId(), keyboardSession->GetPersistentId());
}

sptr<SceneSession> SceneSessionManager::CreateSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    sptr<SceneSession::SpecificSessionCallback> specificCb = CreateSpecificSessionCallback();
    sptr<SceneSession> sceneSession = nullptr;
    if (sessionInfo.isSystem_) {
        sceneSession = new (std::nothrow) SCBSystemSession(sessionInfo, specificCb);
        WLOGFI("[WMSSCB]Create SCBSystemSession, type: %{public}d", sessionInfo.windowType_);
    } else if (property == nullptr && SessionHelper::IsMainWindow(static_cast<WindowType>(sessionInfo.windowType_))) {
        sceneSession = new (std::nothrow) MainSession(sessionInfo, specificCb);
        if (sceneSession != nullptr) {
            TLOGI(WmsLogTag::WMS_MAIN, "Create MainSession, id: %{public}d", sceneSession->GetPersistentId());
        }
    } else if (property != nullptr && SessionHelper::IsSubWindow(property->GetWindowType())) {
        sceneSession = new (std::nothrow) SubSession(sessionInfo, specificCb);
        TLOGI(WmsLogTag::WMS_SUB, "Create SubSession, type: %{public}d", property->GetWindowType());
    } else if (property != nullptr && property->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb = CreateKeyboardSessionCallback();
        sceneSession = new (std::nothrow) KeyboardSession(sessionInfo, specificCb, keyboardCb);
        CreateKeyboardPanelSession(sceneSession);
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Create KeyboardSession, type: %{public}d", property->GetWindowType());
    } else if (property != nullptr && SessionHelper::IsSystemWindow(property->GetWindowType())) {
        sceneSession = new (std::nothrow) SystemSession(sessionInfo, specificCb);
        TLOGI(WmsLogTag::WMS_SYSTEM, "Create SystemSession, type: %{public}d", property->GetWindowType());
    } else {
        TLOGE(WmsLogTag::WMS_LIFE, "Invalid window type");
    }
    if (sceneSession != nullptr) {
        sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
        sceneSession->isKeyboardPanelEnabled_ = isKeyboardPanelEnabled_;
    }
    return sceneSession;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    if (sessionInfo.persistentId_ != 0 && !sessionInfo.isPersistentRecover_) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr) {
            NotifySessionUpdate(sessionInfo, ActionType::SINGLE_START);
            TLOGD(WmsLogTag::WMS_LIFE, "get exist session persistentId: %{public}d", sessionInfo.persistentId_);
            return session;
        }
    }

    auto task = [this, sessionInfo, property]() {
        TLOGI(WmsLogTag::WMS_LIFE, "RequestSceneSession, appName: [%{public}s %{public}s %{public}s]"
            "appIndex: %{public}d, type %{public}u isSystem:%{public}u, isPersistentRecover: %{public}u",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(),
            sessionInfo.abilityName_.c_str(), sessionInfo.appIndex_, sessionInfo.windowType_,
            static_cast<uint32_t>(sessionInfo.isSystem_), static_cast<uint32_t>(sessionInfo.isPersistentRecover_));
        sptr<SceneSession> sceneSession = CreateSceneSession(sessionInfo, property);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is nullptr!");
            return sceneSession;
        }
        if (sceneSession->GetSessionProperty()) {
            sceneSession->GetSessionProperty()->SetDisplayId(sessionInfo.screenId_);
            TLOGD(WmsLogTag::WMS_LIFE, "RequestSceneSession, synchronous screenId with displayid %{public}" PRIu64"",
                sessionInfo.screenId_);
        }
        sceneSession->SetEventHandler(taskScheduler_->GetEventHandler(), eventHandler_);
        auto windowModeCallback = [this]() {
            ProcessSplitFloating();
        };
        auto isScreenLockedCallback = [this]() {
            return IsScreenLocked();
        };
        sceneSession->RegisterWindowModeChangedCallback(windowModeCallback);
        sceneSession->RegisterIsScreenLockedCallback(isScreenLockedCallback);
        if (sessionInfo.isSystem_) {
            sceneSession->SetCallingPid(IPCSkeleton::GetCallingRealPid());
            sceneSession->SetCallingUid(IPCSkeleton::GetCallingUid());
            auto rootContext = rootSceneContextWeak_.lock();
            sceneSession->SetAbilityToken(rootContext != nullptr ? rootContext->GetToken() : nullptr);
        } else {
            TLOGD(WmsLogTag::WMS_LIFE, "RequestSceneSession, id: %{public}d, bundleName: %{public}s, \
                moduleName: %{public}s, abilityName: %{public}s want:%{public}s", sceneSession->GetPersistentId(),
                sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(),
                sessionInfo.want == nullptr ? "nullptr" : sessionInfo.want->ToString().c_str());
        }
        RegisterSessionExceptionFunc(sceneSession);
        FillSessionInfo(sceneSession);
        auto persistentId = sceneSession->GetPersistentId();
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSession(%d )", persistentId);
        if (WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
            WindowInfoReporter::GetInstance().InsertCreateReportInfo(sessionInfo.bundleName_);
        }
        if (property != nullptr && WindowHelper::IsPipWindow(property->GetWindowType())) {
            sceneSession->SetPiPTemplateInfo(property->GetPiPTemplateInfo());
        }
        sceneSession->SetSystemConfig(systemConfig_);
        sceneSession->SetSnapshotScale(snapshotScale_);
        UpdateParentSessionForDialog(sceneSession, property);
        if (CheckCollaboratorType(sceneSession->GetCollaboratorType())) {
            WLOGFD("ancoSceneState: %{public}d", sceneSession->GetSessionInfo().ancoSceneState);
            PreHandleCollaborator(sceneSession);
        }
        {
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.insert({ persistentId, sceneSession });
        }
        PerformRegisterInRequestSceneSession(sceneSession);
        NotifySessionUpdate(sessionInfo, ActionType::SINGLE_START);
        TLOGI(WmsLogTag::WMS_LIFE, "RequestSceneSession, id: %{public}d, type: %{public}d",
            persistentId, sceneSession->GetWindowType());
        return sceneSession;
    };
    return taskScheduler_->PostSyncTask(task, "RequestSceneSession:PID" + std::to_string(sessionInfo.persistentId_));
}

void SceneSessionManager::NotifySessionUpdate(const SessionInfo& sessionInfo, ActionType action, ScreenId fromScreenId)
{
    sptr<DisplayChangeInfo> info = new (std::nothrow) DisplayChangeInfo();
    if (info == nullptr) {
        WLOGFE("new info failed");
        return;
    }
    info->action_ = action;
    info->abilityName_ = sessionInfo.abilityName_;
    info->bundleName_ = sessionInfo.bundleName_;
    info->toScreenId_ = sessionInfo.screenId_;
    info->fromScreenId_ = fromScreenId;
    ScreenSessionManagerClient::GetInstance().NotifyDisplayChangeInfoChanged(info);
    WLOGFI("Notify ability %{public}s bundle %{public}s update,toScreen id: %{public}" PRIu64"",
        info->abilityName_.c_str(), info->bundleName_.c_str(), info->toScreenId_);
}

void SceneSessionManager::PerformRegisterInRequestSceneSession(sptr<SceneSession>& sceneSession)
{
    RegisterSessionSnapshotFunc(sceneSession);
    RegisterSessionStateChangeNotifyManagerFunc(sceneSession);
    RegisterSessionInfoChangeNotifyManagerFunc(sceneSession);
    RegisterRequestFocusStatusNotifyManagerFunc(sceneSession);
    RegisterGetStateFromManagerFunc(sceneSession);
}

void SceneSessionManager::UpdateSceneSessionWant(const SessionInfo& sessionInfo)
{
    if (sessionInfo.persistentId_ != 0) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr && sessionInfo.want != nullptr) {
            WLOGFI("get exist session persistentId: %{public}d", sessionInfo.persistentId_);
            if (!CheckCollaboratorType(session->GetCollaboratorType())) {
                session->SetSessionInfoWant(sessionInfo.want);
                WLOGFI("RequestSceneSession update want, persistentId:%{public}d", sessionInfo.persistentId_);
            } else {
                UpdateCollaboratorSessionWant(session, sessionInfo.persistentId_);
            }
        }
    }
}

void SceneSessionManager::UpdateCollaboratorSessionWant(sptr<SceneSession>& session, int32_t persistentId)
{
    if (session != nullptr) {
        if (session->GetSessionInfo().ancoSceneState < AncoSceneState::NOTIFY_CREATE) {
            FillSessionInfo(session);
            if (CheckCollaboratorType(session->GetCollaboratorType())) {
                PreHandleCollaborator(session, persistentId);
            }
        }
    }
}

bool SceneSessionManager::CheckAppIsInDisplay(const sptr<SceneSession>& scnSession, DisplayId displayId)
{
    if (!scnSession) {
        WLOGFE("scenesession is null!");
        return false;
    }
    if (!scnSession->IsAppSession()) {
        WLOGFE("scenesession is not app");
        return false;
    }
    if (!scnSession->GetSessionProperty()) {
        WLOGFE("property is null");
        return false;
    }
    return scnSession->GetSessionProperty()->GetDisplayId() == displayId;
}

WSError SceneSessionManager::UpdateConfig(const SessionInfo& sessionInfo, AppExecFwk::Configuration config,
    bool informAllAPP)
{
    auto systemAbility = GetAppManager();
    if (!systemAbility) {
        WLOGFE("app manager is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    ScreenId defScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    if (sessionInfo.screenId_ == defScreenId) {
        config.AddItem(AAFwk::GlobalConfigurationKey::COLORMODE_NEED_REMOVE_SET_BY_SA,
            AppExecFwk::ConfigurationInner::NEED_REMOVE_SET_BY_SA);
    }
    config.AddItem(AAFwk::GlobalConfigurationKey::COLORMODE_IS_SET_BY_SA,
        AppExecFwk::ConfigurationInner::IS_SET_BY_SA);

    auto task = [this, sessionInfo, config, informAllAPP, systemAbility]() -> WSError {
        if (informAllAPP) {
            std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            for (auto iter : sceneSessionMap_) {
                auto scnSession = iter.second;
                if (CheckAppIsInDisplay(scnSession, sessionInfo.screenId_)) {
                    systemAbility->UpdateConfigurationByBundleName(config, scnSession->GetSessionInfo().bundleName_);
                }
            }
        } else {
            systemAbility->UpdateConfigurationByBundleName(config, sessionInfo.bundleName_);
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "UpdateConfig");
    return WSError::WS_OK;
}

sptr<AAFwk::SessionInfo> SceneSessionManager::SetAbilitySessionInfo(const sptr<SceneSession>& scnSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    sptr<ISession> iSession(scnSession);
    abilitySessionInfo->sessionToken = iSession->AsObject();
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->sessionName = SessionUtils::ConvertSessionName(sessionInfo.bundleName_,
        sessionInfo.abilityName_, sessionInfo.moduleName_, sessionInfo.appIndex_);
    abilitySessionInfo->persistentId = scnSession->GetPersistentId();
    abilitySessionInfo->requestCode = sessionInfo.requestCode;
    abilitySessionInfo->resultCode = sessionInfo.resultCode;
    abilitySessionInfo->uiAbilityId = sessionInfo.uiAbilityId_;
    abilitySessionInfo->startSetting = sessionInfo.startSetting;
    abilitySessionInfo->callingTokenId = sessionInfo.callingTokenId_;
    abilitySessionInfo->userId = currentUserId_;
    abilitySessionInfo->isClearSession = sessionInfo.isClearSession;
    abilitySessionInfo->processOptions = sessionInfo.processOptions;
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    } else {
        abilitySessionInfo->want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    if (scnSession->GetSessionProperty()) {
        abilitySessionInfo->want.SetParam(AAFwk::Want::PARAM_RESV_DISPLAY_ID,
            static_cast<int>(scnSession->GetSessionProperty()->GetDisplayId()));
    }
    return abilitySessionInfo;
}

WSError SceneSessionManager::PrepareTerminate(int32_t persistentId, bool& isPrepareTerminate)
{
    auto task = [this, persistentId, &isPrepareTerminate]() {
        if (!isPrepareTerminateEnable_) { // not support prepareTerminate
            isPrepareTerminate = false;
            WLOGE("not support prepareTerminate, persistentId%{public}d", persistentId);
            return WSError::WS_OK;
        }
        auto scnSession = GetSceneSession(persistentId);
        if (scnSession == nullptr) {
            WLOGFE("scnSession is nullptr persistentId:%{public}d", persistentId);
            isPrepareTerminate = false;
            return WSError::WS_ERROR_NULLPTR;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (scnSessionInfo == nullptr) {
            WLOGFE("scnSessionInfo is nullptr, persistentId:%{public}d", persistentId);
            isPrepareTerminate = false;
            return WSError::WS_ERROR_NULLPTR;
        }
        TLOGI(WmsLogTag::WMS_MAIN, "PrepareTerminateAbilityBySCB persistentId:%{public}d", persistentId);
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->
            PrepareTerminateAbilityBySCB(scnSessionInfo, isPrepareTerminate);
        TLOGI(WmsLogTag::WMS_MAIN, "PrepareTerminateAbilityBySCB isPrepareTerminate:%{public}d errorCode:%{public}d",
            isPrepareTerminate, errorCode);
        return WSError::WS_OK;
    };

    taskScheduler_->PostSyncTask(task, "PrepareTerminate:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

std::future<int32_t> SceneSessionManager::RequestSceneSessionActivation(
    const sptr<SceneSession>& sceneSession, bool isNewActive)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    std::shared_ptr<std::promise<int32_t>> promise = std::make_shared<std::promise<int32_t>>();
    auto future = promise->get_future();
    auto task = [this, weakSceneSession, isNewActive, promise]() {
        sptr<SceneSession> scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr");
            promise->set_value(static_cast<int32_t>(WSError::WS_ERROR_NULLPTR));
            return WSError::WS_ERROR_INVALID_WINDOW;
        }

        auto persistentId = scnSession->GetPersistentId();
        scnSession->NotifyForegroundInteractiveStatus(true);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionActivation(%d )", persistentId);
        TLOGI(WmsLogTag::WMS_MAIN, "active persistentId: %{public}d isSystem_:%{public}u, isNewActive:%{public}d",
            persistentId, static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_), isNewActive);
        if (!GetSceneSession(persistentId)) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is invalid with %{public}d", persistentId);
            promise->set_value(static_cast<int32_t>(WSError::WS_ERROR_INVALID_SESSION));
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
            WLOGFI("collaborator use native session");
            scnSession = GetSceneSession(persistentId);
        }
        auto ret = RequestSceneSessionActivationInner(scnSession, isNewActive, promise);
        scnSession->RemoveLifeCycleTask(LifeCycleTaskType::START);
        return ret;
    };
    std::string taskName = "RequestSceneSessionActivation:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return future;
}

bool SceneSessionManager::IsKeyboardForeground()
{
    bool isKeyboardForeground = false;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession != nullptr && sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            isKeyboardForeground = sceneSession->IsSessionForeground();
            break;
        }
    }

    return isKeyboardForeground;
}

void SceneSessionManager::RequestInputMethodCloseKeyboard(const int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr");
        return;
    }
    // Hide keyboard when app is cold started, if keyboard is showing and screen is unlocked.
    if (!sceneSession->IsSessionValid() && IsKeyboardForeground() &&
        !sceneSession->GetStateFromManager(ManagerState::MANAGER_STATE_SCREEN_LOCKED)) {
        sceneSession->RequestHideKeyboard(true);
    }
}

int32_t SceneSessionManager::StartUIAbilityBySCB(sptr<SceneSession>& scnSession)
{
    auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
    if (abilitySessionInfo == nullptr) {
        return ERR_NULL_OBJECT;
    }
    return StartUIAbilityBySCB(abilitySessionInfo);
}

int32_t SceneSessionManager::StartUIAbilityBySCB(sptr<AAFwk::SessionInfo>& abilitySessionInfo)
{
    return AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(abilitySessionInfo);
}

int32_t SceneSessionManager::ChangeUIAbilityVisibilityBySCB(sptr<SceneSession>& scnSession, bool visibility)
{
    auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
    if (abilitySessionInfo == nullptr) {
        return ERR_NULL_OBJECT;
    }
    return AAFwk::AbilityManagerClient::GetInstance()->ChangeUIAbilityVisibilityBySCB(abilitySessionInfo, visibility);
}

WSError SceneSessionManager::RequestSceneSessionActivationInner(
    sptr<SceneSession>& scnSession, bool isNewActive, const std::shared_ptr<std::promise<int32_t>>& promise)
{
    auto persistentId = scnSession->GetPersistentId();
    RequestInputMethodCloseKeyboard(persistentId);
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType()) && scnSession->IsFocusedOnShow()) {
        RequestSessionFocusImmediately(persistentId);
    }
    if (scnSession->GetSessionInfo().ancoSceneState < AncoSceneState::NOTIFY_CREATE) {
        FillSessionInfo(scnSession);
        PreHandleCollaborator(scnSession, persistentId);
    }
    auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
    if (!scnSessionInfo) {
        promise->set_value(static_cast<int32_t>(WSError::WS_ERROR_NULLPTR));
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    scnSession->NotifyActivation();
    scnSessionInfo->isNewWant = isNewActive;
    if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
        scnSessionInfo->want.SetParam(AncoConsts::ANCO_MISSION_ID, scnSessionInfo->persistentId);
        scnSessionInfo->collaboratorType = scnSession->GetCollaboratorType();
    }
    TLOGI(WmsLogTag::WMS_LIFE, "RequestSceneSessionActivationInner: want info - \
        abilityName: %{public}s, bundleName: %{public}s, moduleName: %{public}s, uri: %{public}s",
        scnSessionInfo->want.GetElement().GetAbilityName().c_str(),
        scnSessionInfo->want.GetElement().GetBundleName().c_str(),
        scnSessionInfo->want.GetElement().GetModuleName().c_str(),
        scnSessionInfo->want.GetElement().GetURI().c_str());
    int32_t errCode = ERR_OK;
    if (systemConfig_.backgroundswitch == false) {
        TLOGI(WmsLogTag::WMS_MAIN, "begin StartUIAbility: %{public}d isSystem:%{public}u", persistentId,
            static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
        errCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(scnSessionInfo);
    } else {
        TLOGD(WmsLogTag::WMS_MAIN, "RequestSceneSessionActivationInner: %{public}d", systemConfig_.backgroundswitch);
        if (isNewActive || scnSession->GetSessionState() == SessionState::STATE_DISCONNECT ||
            scnSession->GetSessionState() == SessionState::STATE_END) {
            TLOGI(WmsLogTag::WMS_MAIN, "begin StartUIAbility: %{public}d isSystem:%{public}u", persistentId,
                static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
            errCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(scnSessionInfo);
        } else {
            scnSession->NotifySessionForeground(1, true);
        }
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
        WindowInfoReporter::GetInstance().InsertShowReportInfo(sessionInfo.bundleName_);
    }
    NotifyCollaboratorAfterStart(scnSession, scnSessionInfo);
    promise->set_value(static_cast<int32_t>(errCode));

    if (errCode != ERR_OK) {
        WLOGFE("session activate failed. errCode: %{public}d", errCode);
        scnSession->NotifySessionException(scnSessionInfo, true);
        if (startUIAbilityErrorFunc_ && static_cast<WSError>(errCode) == WSError::WS_ERROR_EDM_CONTROLLED) {
            startUIAbilityErrorFunc_(
                static_cast<uint32_t>(WS_JS_TO_ERROR_CODE_MAP.at(WSError::WS_ERROR_EDM_CONTROLLED)));
        }
    }
    return WSError::WS_OK;
}

void SceneSessionManager::NotifyCollaboratorAfterStart(sptr<SceneSession>& scnSession,
    sptr<AAFwk::SessionInfo>& scnSessionInfo)
{
    if (scnSession == nullptr || scnSessionInfo == nullptr) {
        return;
    }
    if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
        NotifyLoadAbility(scnSession->GetCollaboratorType(),
            scnSessionInfo, scnSession->GetSessionInfo().abilityInfo);
        NotifyUpdateSessionInfo(scnSession);
        NotifyMoveSessionToForeground(scnSession->GetCollaboratorType(), scnSessionInfo->persistentId);
        scnSession->SetSessionInfoAncoSceneState(AncoSceneState::NOTIFY_FOREGROUND);
    }
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession,
    const bool isDelegator, const bool isToDesktop, const bool isSaveSnapShot)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, isDelegator, isToDesktop, isSaveSnapShot]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        TLOGI(WmsLogTag::WMS_MAIN, "background session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionBackground (%d )", persistentId);
        scnSession->SetActive(false);

        if (isToDesktop) {
            auto info = scnSession->GetSessionInfo();
            info.callerToken_ = nullptr;
            info.callingTokenId_ = 0;
            scnSession->SetSessionInfo(info);
        }

        scnSession->BackgroundTask(isSaveSnapShot);
        if (!GetSceneSession(persistentId)) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (persistentId == brightnessSessionId_) {
            UpdateBrightness(focusedSessionId_);
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }

        if (systemConfig_.backgroundswitch) {
            TLOGI(WmsLogTag::WMS_MAIN, "RequestSceneSessionBackground: %{public}d", systemConfig_.backgroundswitch);
            scnSession->NotifySessionBackground(1, true, true);
        } else {
            TLOGI(WmsLogTag::WMS_MAIN, "begin MinimzeUIAbility: %{public}d isSystem:%{public}u",
                persistentId, static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
            if (!isDelegator) {
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
            } else {
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo, true);
            }
        }

        if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
            auto sessionInfo = scnSession->GetSessionInfo();
            WindowInfoReporter::GetInstance().InsertHideReportInfo(sessionInfo.bundleName_);
        }
        return WSError::WS_OK;
    };
    std::string taskName = "RequestSceneSessionBackground:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::NotifyForegroundInteractiveStatus(const sptr<SceneSession>& sceneSession, bool interactive)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, interactive]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("notify interactive session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:NotifyForegroundInteractiveStatus (%d )", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return;
        }
        scnSession->NotifyForegroundInteractiveStatus(interactive);
    };

    taskScheduler_->PostAsyncTask(task, "NotifyForegroundInteractiveStatus");
}

WSError SceneSessionManager::DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:DestroyDialogWithMainWindow");
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "scnSession is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (scnSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        TLOGI(WmsLogTag::WMS_DIALOG, "Begin to destroy dialog, parentId: %{public}d", scnSession->GetPersistentId());
        auto dialogVec = scnSession->GetDialogVector();
        for (auto dialog : dialogVec) {
            if (dialog == nullptr) {
                TLOGE(WmsLogTag::WMS_DIALOG, "dialog is nullptr");
                continue;
            }
            auto sceneSession = GetSceneSession(dialog->GetPersistentId());
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_DIALOG, "dialog is invalid, id: %{public}d", dialog->GetPersistentId());
                return WSError::WS_ERROR_INVALID_SESSION;
            }
            WindowDestroyNotifyVisibility(sceneSession);
            dialog->NotifyDestroy();
            dialog->Disconnect();

            auto dialogSceneSession = GetSceneSession(dialog->GetPersistentId());
            if (dialogSceneSession != nullptr) {
                dialogSceneSession->ClearSpecificSessionCbMap();
            }
            {
                std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
                sceneSessionMap_.erase(dialog->GetPersistentId());
                systemTopSceneSessionMap_.erase(dialog->GetPersistentId());
                nonSystemFloatSceneSessionMap_.erase(dialog->GetPersistentId());
            }
        }
        scnSession->ClearDialogVector();
        return WSError::WS_OK;
    }
    return WSError::WS_ERROR_INVALID_SESSION;
}

void SceneSessionManager::DestroySubSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("sceneSession is nullptr");
        return;
    }
    for (const auto& elem : sceneSession->GetSubSession()) {
        if (elem != nullptr) {
            const auto& persistentId = elem->GetPersistentId();
            TLOGI(WmsLogTag::WMS_SUB, "DestroySubSession, id: %{public}d", persistentId);
            DestroyAndDisconnectSpecificSessionInner(persistentId);
        }
    }
}

void SceneSessionManager::EraseSceneSessionMapById(int32_t persistentId)
{
    std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    sceneSessionMap_.erase(persistentId);
    systemTopSceneSessionMap_.erase(persistentId);
    nonSystemFloatSceneSessionMap_.erase(persistentId);
}

WSError SceneSessionManager::RequestSceneSessionDestruction(
    const sptr<SceneSession>& sceneSession, const bool needRemoveSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, needRemoveSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        HandleCastScreenDisConnection(scnSession);
        auto persistentId = scnSession->GetPersistentId();
        RequestSessionUnfocus(persistentId);
        lastUpdatedAvoidArea_.erase(persistentId);
        DestroyDialogWithMainWindow(scnSession);
        DestroySubSession(scnSession); // destroy sub session by destruction
        TLOGI(WmsLogTag::WMS_MAIN, " destroy session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionDestruction (%" PRIu32" )", persistentId);
        if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
            auto sessionInfo = scnSession->GetSessionInfo();
            WindowInfoReporter::GetInstance().InsertDestroyReportInfo(sessionInfo.bundleName_);
        }
        WindowDestroyNotifyVisibility(scnSession);
        scnSession->Disconnect();
        if (!GetSceneSession(persistentId)) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        scnSession->GetCloseAbilityWantAndClean(scnSessionInfo->want);
        if (scnSessionInfo->isClearSession) {
            scnSessionInfo->resultCode = -1;
        }
        if (scnSessionInfo->resultCode == -1) {
            OHOS::AAFwk::Want want;
            scnSessionInfo->want = want;
        }
        return RequestSceneSessionDestructionInner(scnSession, scnSessionInfo, needRemoveSession);
    };
    std::string taskName = "RequestSceneSessionDestruction:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::HandleCastScreenDisConnection(const sptr<SceneSession> sceneSession)
{
    auto sessionInfo = sceneSession->GetSessionInfo();
    ScreenId defScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    if (defScreenId == sessionInfo.screenId_) {
        return;
    }
    auto flag = Rosen::ScreenManager::GetInstance().GetVirtualScreenFlag(sessionInfo.screenId_);
    if (flag != VirtualScreenFlag::CAST) {
        return;
    }
    std::vector<uint64_t> mirrorIds { sessionInfo.screenId_ };
    ScreenId groupId;
    Rosen::DMError ret = Rosen::ScreenManager::GetInstance().MakeMirror(0, mirrorIds, groupId);
    if (ret != Rosen::DMError::DM_OK) {
        TLOGI(WmsLogTag::WMS_LIFE, "MakeMirror failed,ret: %{public}d", ret);
        return;
    }
}

WSError SceneSessionManager::RequestSceneSessionDestructionInner(
    sptr<SceneSession> &scnSession, sptr<AAFwk::SessionInfo> scnSessionInfo, const bool needRemoveSession)
{
    auto persistentId = scnSession->GetPersistentId();
    NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::SINGLE_CLOSE);
    TLOGI(WmsLogTag::WMS_MAIN, "begin CloseUIAbility: %{public}d isSystem:%{public}u",
        persistentId,
        static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
    AAFwk::AbilityManagerClient::GetInstance()->CloseUIAbilityBySCB(scnSessionInfo);
    scnSession->SetSessionInfoAncoSceneState(AncoSceneState::DEFAULT_STATE);
    if (needRemoveSession) {
        if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
            NotifyClearSession(scnSession->GetCollaboratorType(), scnSessionInfo->persistentId);
        }
        EraseSceneSessionMapById(persistentId);
    } else {
        // if terminate, set want to null. so start from recent, start a new one.
        scnSession->SetSessionInfoWant(nullptr);
    }
    if (listenerController_ != nullptr) {
        NotifySessionForCallback(scnSession, needRemoveSession);
    }
    scnSession->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
    return WSError::WS_OK;
}

void SceneSessionManager::AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr || sessionStage == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "sessionStage or sceneSession is nullptr");
        return;
    }

    auto remoteObject = sessionStage->AsObject();
    remoteObjectMap_.insert(std::make_pair(remoteObject, sceneSession->GetPersistentId()));
    if (windowDeath_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "failed to create death recipient");
        return;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        TLOGE(WmsLogTag::WMS_LIFE, "failed to add death recipient");
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
        auto sceneSession = GetSceneSession(iter->second);
        if (sceneSession == nullptr) {
            WLOGFW("Remote died, session is nullptr, id: %{public}d", iter->second);
            return;
        }
        DestroyAndDisconnectSpecificSessionInner(iter->second);
        remoteObjectMap_.erase(iter);
    };
    taskScheduler_->PostAsyncTask(task, "DestroySpecificSession");
}

void SceneSessionManager::DestroyExtensionSession(const sptr<IRemoteObject>& remoteExtSession)
{
    auto task = [this, remoteExtSession]() {
        auto iter = remoteExtSessionMap_.find(remoteExtSession);
        if (iter == remoteExtSessionMap_.end()) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Invalid remoteExtSession");
            return;
        }
        auto persistentId = iter->second.first;
        auto parentId = iter->second.second;
        TLOGD(WmsLogTag::WMS_UIEXT, "Remote died, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(parentId);
        if (sceneSession != nullptr) {
            auto oldFlags = sceneSession->GetCombinedExtWindowFlags();
            sceneSession->RemoveExtWindowFlags(persistentId);
            if (oldFlags.hideNonSecureWindowsFlag) {
                HandleSecureSessionShouldHide(sceneSession);
            }
            if (oldFlags.waterMarkFlag) {
                CheckAndNotifyWaterMarkChangedResult();
            }
            if (oldFlags.privacyModeFlag) {
                UpdatePrivateStateAndNotify(parentId);
            }
        } else {
            HandleSCBExtWaterMarkChange(persistentId, false);
            HandleSecureExtSessionShouldHide(persistentId, false);
        }
        remoteExtSessionMap_.erase(iter);
    };
    taskScheduler_->PostAsyncTask(task, "DestroyExtensionSession");
}

WSError SceneSessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
    SystemSessionConfig& systemConfig, sptr<IRemoteObject> token)
{
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }

    if (!CheckSystemWindowPermission(property)) {
        WLOGFE("create system window permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    bool shouldBlock = (property->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT &&
                        property->IsFloatingWindowAppType() && !secureSessionSet_.empty()) ||
                       (SessionHelper::IsSubWindow(property->GetWindowType()) &&
                        secureSessionSet_.find(property->GetParentPersistentId()) != secureSessionSet_.end());
    if (shouldBlock) {
        TLOGE(WmsLogTag::WMS_UIEXT, "create non-secure window permission denied!");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }

    if (property->GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW &&
        property->GetExtensionFlag() == true && SessionPermission::IsStartedByUIExtension()) {
        auto extensionParentSession = GetSceneSession(property->GetParentPersistentId());
        if (extensionParentSession == nullptr) {
            WLOGFE("extensionParentSession is invalid with %{public}d", property->GetParentPersistentId());
            return WSError::WS_ERROR_NULLPTR;
        }
        SessionInfo sessionInfo = extensionParentSession->GetSessionInfo();
        AAFwk::UIExtensionHostInfo hostInfo;
        AAFwk::AbilityManagerClient::GetInstance()->GetUIExtensionRootHostInfo(token, hostInfo);
        if (sessionInfo.bundleName_ != hostInfo.elementName_.GetBundleName()) {
            WLOGE("The hostWindow is not this parentwindow ! parentwindow bundleName: %{public}s, "
                "hostwindow bundleName: %{public}s", sessionInfo.bundleName_.c_str(),
                hostInfo.elementName_.GetBundleName().c_str());
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
    }

    // WINDOW_TYPE_SYSTEM_ALARM_WINDOW has been deprecated, will be deleted after 5 versions.
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW) {
        WLOGFE("The alarm window has been deprecated!");
        return WSError::WS_ERROR_INVALID_WINDOW;
    }

    if (property->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
        if (!CheckPiPPriority(property->GetPiPTemplateInfo())) {
            TLOGI(WmsLogTag::WMS_PIP, "skip create pip window by priority");
            return WSError::WS_DO_NOTHING;
        }
        auto parentSession = GetSceneSession(property->GetParentPersistentId());
        if (parentSession != nullptr && parentSession->GetSessionState() == SessionState::STATE_DISCONNECT) {
            TLOGI(WmsLogTag::WMS_PIP, "skip create pip window as parent window disconnected");
            return WSError::WS_DO_NOTHING;
        }
    }
    TLOGI(WmsLogTag::WMS_LIFE, "create specific start, name: %{public}s, type: %{public}d",
        property->GetWindowName().c_str(), property->GetWindowType());

    // Get pid and uid before posting task.
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property,
                    &persistentId, &session, &systemConfig, token, pid, uid]() {
        if (property == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        const auto& type = property->GetWindowType();
        // create specific session
        SessionInfo info;
        info.windowType_ = static_cast<uint32_t>(type);
        info.bundleName_ = property->GetSessionInfo().bundleName_;
        info.abilityName_ = property->GetSessionInfo().abilityName_;
        info.moduleName_ = property->GetSessionInfo().moduleName_;

        ClosePipWindowIfExist(type);
        sptr<SceneSession> newSession = RequestSceneSession(info, property);
        if (newSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[WMSSub][WMSSystem] session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errCode = newSession->Connect(
            sessionStage, eventChannel, surfaceNode, systemConfig_, property, token, pid, uid);
        systemConfig = systemConfig_;
        if (property) {
            persistentId = property->GetPersistentId();
        }

        NotifyCreateSpecificSession(newSession, property, type);
        session = newSession;
        AddClientDeathRecipient(sessionStage, newSession);
        TLOGI(WmsLogTag::WMS_LIFE, "create specific session success, id: %{public}d, \
            parentId: %{public}d, type: %{public}d",
            newSession->GetPersistentId(), newSession->GetParentPersistentId(), type);
        return errCode;
    };

    return taskScheduler_->PostSyncTask(task, "CreateAndConnectSpecificSession");
}

void SceneSessionManager::ClosePipWindowIfExist(WindowType type)
{
    if (type != WindowType::WINDOW_TYPE_PIP) {
        return;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
            session->NotifyCloseExistPipWindow();
            break;
        }
    }
}

bool SceneSessionManager::CheckPiPPriority(const PiPTemplateInfo& pipTemplateInfo)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowMode() == WindowMode::WINDOW_MODE_PIP &&
            pipTemplateInfo.priority < session->GetPiPTemplateInfo().priority &&
            IsSessionVisible(session)) {
            TLOGE(WmsLogTag::WMS_PIP, "create pip window failed, reason: low priority.");
            return false;
        }
    }
    return true;
}

bool SceneSessionManager::CheckSystemWindowPermission(const sptr<WindowSessionProperty>& property)
{
    WindowType type = property->GetWindowType();
    if (!WindowHelper::IsSystemWindow(type)) {
        // type is not system
        return true;
    }
    if ((type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT || type == WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR)
        && SessionPermission::IsStartedByInputMethod()) {
        // WINDOW_TYPE_INPUT_METHOD_FLOAT could be created by input method app
        WLOGFD("check create permission success, input method app create input method window.");
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_DRAGGING_EFFECT ||
        type == WindowType::WINDOW_TYPE_TOAST || type == WindowType::WINDOW_TYPE_DIALOG ||
        type == WindowType::WINDOW_TYPE_PIP) {
        // some system types could be created by normal app
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_FLOAT &&
        SessionPermission::VerifyCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
        auto isPC = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
        // WINDOW_TYPE_FLOAT could be created with the corresponding permission
        if (isPC) {
            WLOGFD("check create float window permission success on 2in1 device.");
            return true;
        }
    }
    if (SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd()) {
        WLOGFD("check create permission success, create with system calling.");
        return true;
    }
    WLOGFE("check system window permission failed.");
    return false;
}

SessionInfo SceneSessionManager::RecoverSessionInfo(const sptr<WindowSessionProperty>& property)
{
    SessionInfo sessionInfo;
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "property is nullptr");
        return sessionInfo;
    }
    sessionInfo = property->GetSessionInfo();
    sessionInfo.persistentId_ = property->GetPersistentId();
    sessionInfo.windowMode = static_cast<int32_t>(property->GetWindowMode());
    sessionInfo.windowType_ = static_cast<uint32_t>(property->GetWindowType());
    sessionInfo.requestOrientation_ = static_cast<uint32_t>(property->GetRequestedOrientation());
    sessionInfo.sessionState_ = (property->GetWindowState() == WindowState::STATE_SHOWN)
                                    ? SessionState::STATE_ACTIVE
                                    : SessionState::STATE_BACKGROUND;
    TLOGI(WmsLogTag::WMS_RECOVER,
        "Recover and reconnect session with: bundleName=%{public}s, moduleName=%{public}s, "
        "abilityName=%{public}s, windowMode=%{public}d, windowType=%{public}u, persistentId=%{public}d, "
        "windowState=%{public}u",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(),
        sessionInfo.windowMode, sessionInfo.windowType_, sessionInfo.persistentId_, sessionInfo.sessionState_);
    return sessionInfo;
}

void SceneSessionManager::SetAlivePersistentIds(const std::vector<int32_t>& alivePersistentIds)
{
    WLOGFI("[WMSRecover] Number of persistentIds need to be recovered = %{public}zu. CurrentUserId = "
           "%{public}d", alivePersistentIds.size(), currentUserId_);
    alivePersistentIds_ = alivePersistentIds;
}

bool SceneSessionManager::isNeedRecover(const int32_t persistentId)
{
    auto it = std::find(alivePersistentIds_.begin(), alivePersistentIds_.end(), persistentId);
    if (it == alivePersistentIds_.end()) {
        WLOGFW("[WMSRecover] recovered persistentId=%{public}d is not in alivePersistentIds_", persistentId);
        return false;
    }
    return true;
}

WSError SceneSessionManager::RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    if (property == nullptr) {
        WLOGFE("[WMSRecover] property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (property->GetParentPersistentId() > 0 && !isNeedRecover(property->GetParentPersistentId())) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property, &session, token, pid, uid]() {
        if (recoveringFinished_) {
            TLOGW(WmsLogTag::WMS_RECOVER, "Recover finished, not recovery anymore");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        // recover specific session
        const auto& type = property->GetWindowType();
        SessionInfo info = RecoverSessionInfo(property);
        info.isPersistentRecover_ = true;
        TLOGI(WmsLogTag::WMS_RECOVER, "callingSessionId = %{public}" PRIu32, property->GetCallingSessionId());
        ClosePipWindowIfExist(type);
        sptr<SceneSession> sceneSession = RequestSceneSession(info, property);
        if (sceneSession == nullptr) {
            WLOGFE("[WMSRecover] RequestSceneSession failed");
            return WSError::WS_ERROR_NULLPTR;
        }

        auto persistentId = sceneSession->GetPersistentId();
        if (persistentId != info.persistentId_) {
            WLOGFW("[WMSRecover] PersistentId changed, from %{public}" PRId32 " to %{public}" PRId32,
                info.persistentId_, persistentId);
        }

        auto errCode = sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid);
        if (errCode != WSError::WS_OK) {
            WLOGFE("[WMSRecover] SceneSession reconnect failed");
            EraseSceneSessionMapById(persistentId);
            return errCode;
        }

        NotifyCreateSpecificSession(sceneSession, property, type);
        CacheSubSessionForRecovering(sceneSession, property);
        NotifySessionUnfocusedToClient(persistentId);
        AddClientDeathRecipient(sessionStage, sceneSession);
        session = sceneSession;
        return errCode;
    };
    return taskScheduler_->PostSyncTask(task, "RecoverAndConnectSpecificSession");
}

void SceneSessionManager::NotifyRecoveringFinished()
{
    taskScheduler_->PostAsyncTask([this]() {
            WLOGFI("[WMSRecover] RecoverFinished clear recoverSubSessionCacheMap");
            recoveringFinished_ = true;
            recoverSubSessionCacheMap_.clear();
        }, "NotifyRecoveringFinished");
}

void SceneSessionManager::CacheSubSessionForRecovering(
    sptr<SceneSession> sceneSession, const sptr<WindowSessionProperty>& property)
{
    if (recoveringFinished_) {
        WLOGFW("[WMSRecover] recovering is finished");
        return;
    }

    if (sceneSession == nullptr || property == nullptr) {
        WLOGFE("[WMSRecover] sceneSession or property is nullptr");
        return;
    }

    auto windowType = property->GetWindowType();
    if (!SessionHelper::IsSubWindow(windowType)) {
        return;
    }

    auto persistentId = property->GetParentPersistentId();
    if (createSubSessionFuncMap_.find(persistentId) != createSubSessionFuncMap_.end()) {
        return;
    }

    WLOGFI("[WMSRecover] Cache subsession persistentId = %{public}" PRId32 ", parent persistentId = %{public}" PRId32,
        sceneSession->GetPersistentId(), persistentId);

    if (recoverSubSessionCacheMap_.find(persistentId) == recoverSubSessionCacheMap_.end()) {
        recoverSubSessionCacheMap_[persistentId] = std::vector{ sceneSession };
    } else {
        recoverSubSessionCacheMap_[persistentId].emplace_back(sceneSession);
    }
}

void SceneSessionManager::RecoverCachedSubSession(int32_t persistentId)
{
    auto iter = recoverSubSessionCacheMap_.find(persistentId);
    if (iter == recoverSubSessionCacheMap_.end()) {
        return;
    }

    WLOGFI("[WMSRecover] RecoverCachedSubSession persistentId = %{public}" PRId32, persistentId);
    for (auto& sceneSession : iter->second) {
        NotifyCreateSubSession(persistentId, sceneSession);
    }
    recoverSubSessionCacheMap_.erase(iter);
}

void SceneSessionManager::NotifySessionUnfocusedToClient(int32_t persistentId)
{
    if (listenerController_ != nullptr) {
        WLOGFI("[WMSRecover] NotifySessionUnfocused persistentId = %{public}" PRId32, persistentId);
        listenerController_->NotifySessionUnfocused(persistentId);
    }
}

WSError SceneSessionManager::RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<ISession>& session, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    if (property == nullptr) {
        WLOGFE("[WMSRecover] property is nullptr!");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!isNeedRecover(property->GetPersistentId())) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, &session, property, token, pid, uid]() {
        if (recoveringFinished_) {
            TLOGW(WmsLogTag::WMS_RECOVER, "Recover finished, not recovery anymore");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        SessionInfo sessionInfo = RecoverSessionInfo(property);
        sptr<SceneSession> sceneSession = nullptr;
        if (SessionHelper::IsMainWindow(property->GetWindowType())) {
            sceneSession = RequestSceneSession(sessionInfo, nullptr);
        } else {
            sceneSession = RequestSceneSession(sessionInfo, property);
        }
        if (sceneSession == nullptr) {
            WLOGFE("[WMSRecover] Request sceneSession failed");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto ret = sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid);
        if (ret != WSError::WS_OK) {
            WLOGFE("[WMSRecover] Reconnect failed");
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.erase(sessionInfo.persistentId_);
            return ret;
        }
        if (recoverSceneSessionFunc_) {
            recoverSceneSessionFunc_(sceneSession, sessionInfo);
        } else {
            WLOGFE("[WMSRecover] recoverSceneSessionFunc_ is null");
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.erase(sessionInfo.persistentId_);
            return WSError::WS_ERROR_NULLPTR;
        }
        NotifySessionUnfocusedToClient(sceneSession->GetPersistentId());
        session = sceneSession;
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "RecoverAndReconnectSceneSession");
}

void SceneSessionManager::SetRecoverSceneSessionListener(const NotifyRecoverSceneSessionFunc& func)
{
    WLOGFI("[WMSRecover] SetRecoverSceneSessionListener");
    recoverSceneSessionFunc_ = func;
}

void SceneSessionManager::SetCreateSystemSessionListener(const NotifyCreateSystemSessionFunc& func)
{
    createSystemSessionFunc_ = func;
}

void SceneSessionManager::SetCreateKeyboardSessionListener(const NotifyCreateKeyboardSessionFunc& func)
{
    createKeyboardSessionFunc_ = func;
}

void SceneSessionManager::RegisterCreateSubSessionListener(int32_t persistentId,
    const NotifyCreateSubSessionFunc& func)
{
    TLOGI(WmsLogTag::WMS_SUB, "RegisterCreateSubSessionListener, id: %{public}d", persistentId);
    auto task = [this, persistentId, func]() {
        auto iter = createSubSessionFuncMap_.find(persistentId);
        if (iter == createSubSessionFuncMap_.end()) {
            createSubSessionFuncMap_.insert(std::make_pair(persistentId, func));
            RecoverCachedSubSession(persistentId);
        } else {
            TLOGW(WmsLogTag::WMS_SUB, "CreateSubSessionListener is existed, id: %{public}d", persistentId);
        }
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task, "RegisterCreateSubSessionListener");
}

void SceneSessionManager::NotifyCreateSpecificSession(sptr<SceneSession> newSession,
    sptr<WindowSessionProperty> property, const WindowType& type)
{
    if (newSession == nullptr) {
        WLOGFE("newSession is nullptr");
        return;
    }
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return;
    }
    if (SessionHelper::IsSystemWindow(type)) {
        if ((type == WindowType::WINDOW_TYPE_TOAST) || (type == WindowType::WINDOW_TYPE_FLOAT)) {
            auto parentSession = GetSceneSession(property->GetParentPersistentId());
            if (parentSession != nullptr) {
                newSession->SetParentSession(parentSession);
            }
        }
        if (type != WindowType::WINDOW_TYPE_DIALOG) {
            if (WindowHelper::IsSystemSubWindow(type)) {
                NotifyCreateSubSession(property->GetParentPersistentId(), newSession);
            } else if (isKeyboardPanelEnabled_ && type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT
                && createKeyboardSessionFunc_) {
                createKeyboardSessionFunc_(newSession, newSession->GetKeyboardPanelSession());
            } else if (createSystemSessionFunc_) {
                createSystemSessionFunc_(newSession);
            }
            TLOGD(WmsLogTag::WMS_LIFE, "Create system session, id:%{public}d, type: %{public}d",
                newSession->GetPersistentId(), type);
        } else {
            TLOGW(WmsLogTag::WMS_LIFE, "Didn't create jsSceneSession for this system type, id:%{public}d, "
                "type:%{public}d", newSession->GetPersistentId(), type);
            return;
        }
    } else if (SessionHelper::IsSubWindow(type)) {
        NotifyCreateSubSession(property->GetParentPersistentId(), newSession);
        TLOGD(WmsLogTag::WMS_LIFE, "Notify sub jsSceneSession, id:%{public}d, parentId:%{public}d, type:%{public}d",
            newSession->GetPersistentId(), property->GetParentPersistentId(), type);
    } else {
        TLOGW(WmsLogTag::WMS_LIFE, "Invalid session type, id:%{public}d, type:%{public}d",
            newSession->GetPersistentId(), type);
    }
}

void SceneSessionManager::NotifyCreateSubSession(int32_t persistentId, sptr<SceneSession> session)
{
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "SubSession is nullptr");
        return;
    }
    auto iter = createSubSessionFuncMap_.find(persistentId);
    if (iter == createSubSessionFuncMap_.end()) {
        TLOGW(WmsLogTag::WMS_LIFE, "Can't find CreateSubSessionListener, parentId: %{public}d", persistentId);
        return;
    }

    auto parentSession = GetSceneSession(persistentId);
    if (parentSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Can't find CreateSubSessionListener, parentId: %{public}d, subId: %{public}d",
            persistentId, session->GetPersistentId());
        return;
    }
    parentSession->AddSubSession(session);
    session->SetParentSession(parentSession);
    if (iter->second) {
        iter->second(session);
    }
    TLOGD(WmsLogTag::WMS_LIFE, "NotifyCreateSubSession success, parentId: %{public}d, subId: %{public}d",
        persistentId, session->GetPersistentId());
}

void SceneSessionManager::UnregisterCreateSubSessionListener(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_SUB, "UnregisterCreateSubSessionListener, id: %{public}d", persistentId);
    auto task = [this, persistentId]() {
        auto iter = createSubSessionFuncMap_.find(persistentId);
        if (iter != createSubSessionFuncMap_.end()) {
            createSubSessionFuncMap_.erase(persistentId);
        } else {
            TLOGW(WmsLogTag::WMS_SUB, "Can't find CreateSubSessionListener, id: %{public}d", persistentId);
        }
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::NotifyStatusBarEnabledChange(bool enable)
{
    WLOGFI("NotifyStatusBarEnabledChange enable %{public}d", enable);
    auto task = [this, enable]() {
        if (statusBarEnabledChangeFunc_) {
            statusBarEnabledChangeFunc_(enable);
        }
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task, "NotifyStatusBarEnabledChange");
}

void SceneSessionManager::SetStatusBarEnabledChangeListener(const ProcessStatusBarEnabledChangeFunc& func)
{
    WLOGFD("SetStatusBarEnabledChangeListener");
    if (!func) {
        WLOGFD("set func is null");
    }
    statusBarEnabledChangeFunc_ = func;
    NotifyStatusBarEnabledChange(gestureNavigationEnabled_);
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
    WLOGFD("OnOutsideDownEvent x = %{public}d, y = %{public}d", x, y);
    if (outsideDownEventFunc_) {
        outsideDownEventFunc_(x, y);
    }
}

void SceneSessionManager::NotifySessionTouchOutside(int32_t persistentId)
{
    auto task = [this, persistentId]() {
        int32_t callingSessionId = INVALID_SESSION_ID;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto sceneSession = item.second;
            if (sceneSession == nullptr) {
                continue;
            }
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
                sceneSession->GetSessionProperty() != nullptr) {
                callingSessionId = static_cast<int32_t>(sceneSession->GetSessionProperty()->GetCallingSessionId());
                TLOGI(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, callingSessionId: %{public}d",
                    persistentId, callingSessionId);
            }
            if (!(sceneSession->IsVisible() ||
                sceneSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                sceneSession->GetSessionState() == SessionState::STATE_ACTIVE)) {
                continue;
            }
            auto sessionId = sceneSession->GetPersistentId();
            if ((!sceneSession->CheckOutTouchOutsideRegister()) &&
                (touchOutsideListenerSessionSet_.find(sessionId) == touchOutsideListenerSessionSet_.end())) {
                WLOGFD("id: %{public}d is not in touchOutsideListenerNodes, don't notify.", sessionId);
                continue;
            }
            if (sessionId == callingSessionId || sessionId == persistentId) {
                WLOGFD("No need to notify touch window, id: %{public}d", sessionId);
                continue;
            }
            sceneSession->NotifyTouchOutside();
        }
    };

    taskScheduler_->PostAsyncTask(task, "NotifySessionTouchOutside:PID" + std::to_string(persistentId));
    return;
}

void SceneSessionManager::SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func)
{
    WLOGFD("SetOutsideDownEventListener");
    outsideDownEventFunc_ = func;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSessionInner(const int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    auto ret = sceneSession->UpdateActiveStatus(false);
    WindowDestroyNotifyVisibility(sceneSession);
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Dialog not bind parent");
        } else {
            parentSession->RemoveDialogToParentSession(sceneSession);
        }
        sceneSession->NotifyDestroy();
    }
    ret = sceneSession->Disconnect();
    sceneSession->ClearSpecificSessionCbMap();
    if (SessionHelper::IsSubWindow(sceneSession->GetWindowType())) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession != nullptr) {
            TLOGD(WmsLogTag::WMS_SUB, "Find parentSession, id: %{public}d", persistentId);
            parentSession->RemoveSubSession(persistentId);
        } else {
            TLOGW(WmsLogTag::WMS_SUB, "ParentSession is nullptr, id: %{public}d", persistentId);
        }
    }
    {
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMap_.erase(persistentId);
        systemTopSceneSessionMap_.erase(persistentId);
        nonSystemFloatSceneSessionMap_.erase(persistentId);
        UnregisterCreateSubSessionListener(persistentId);
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Destroy specific session end, id: %{public}d", persistentId);
    return ret;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSession(const int32_t persistentId)
{
    const auto& callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, callingPid]() {
        TLOGI(WmsLogTag::WMS_LIFE, "Destroy specific session start, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }

        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Permission denied, not destroy by the same process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        return DestroyAndDisconnectSpecificSessionInner(persistentId);
    };

    return taskScheduler_->PostSyncTask(task, "DestroyAndDisConnect:PID:" + std::to_string(persistentId));
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSessionWithDetachCallback(const int32_t persistentId,
    const sptr<IRemoteObject>& callback)
{
    if (callback == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, callingPid, callback]() {
        TLOGI(WmsLogTag::WMS_LIFE, "Destroy specific session start, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }

        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Permission denied, not destroy by the same process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        sceneSession->RegisterDetachCallback(iface_cast<IPatternDetachCallback>(callback));
        return DestroyAndDisconnectSpecificSessionInner(persistentId);
    };

    return taskScheduler_->PostSyncTask(task, "DestroyAndDisConnect:PID:" + std::to_string(persistentId));
}

const AppWindowSceneConfig& SceneSessionManager::GetWindowSceneConfig() const
{
    return appWindowSceneConfig_;
}

WSError SceneSessionManager::ProcessBackEvent()
{
    auto task = [this]() {
        auto session = GetSceneSession(focusedSessionId_);
        if (!session) {
            WLOGFE("session is nullptr: %{public}d", focusedSessionId_);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        WLOGFI("ProcessBackEvent session persistentId: %{public}d", focusedSessionId_);
        if (needBlockNotifyFocusStatusUntilForeground_) {
            WLOGFD("RequestSessionBack when start session");
            session->RequestSessionBack(false);
            return WSError::WS_OK;
        }
        session->ProcessBackEvent();
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "ProcessBackEvent");
    return WSError::WS_OK;
}

WSError SceneSessionManager::InitUserInfo(int32_t userId, std::string &fileDir)
{
    if (userId == DEFAULT_USERID || fileDir.empty()) {
        TLOGE(WmsLogTag::WMS_MAIN, "params invalid");
        return WSError::WS_DO_NOTHING;
    }
    TLOGI(WmsLogTag::WMS_MAIN, "userId : %{public}d, path : %{public}s", userId, fileDir.c_str());
    auto task = [this, userId, &fileDir]() {
        ScenePersistence::CreateSnapshotDir(fileDir);
        ScenePersistence::CreateUpdatedIconDir(fileDir);
        currentUserId_ = userId;
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "InitUserInfo");
}

void SceneSessionManager::HandleSwitchingToAnotherUser()
{
    auto task = [this]() {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "Handle switching to another user");
        SceneInputManager::GetInstance().SetUserBackground(true);
        if (switchingToAnotherUserFunc_ != nullptr) {
            switchingToAnotherUserFunc_();
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "HandleSwitchingToAnotherUser");
}

void SceneSessionManager::NotifySwitchingToCurrentUser()
{
    auto task = [this]() {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "Notify switching to current user");
        SceneInputManager::GetInstance().SetUserBackground(false);
        // notify screenSessionManager to recover current user
        ScreenSessionManagerClient::GetInstance().SwitchingCurrentUser();
        FlushWindowInfoToMMI(true);
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "NotifySwitchingToCurrentUser");
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

sptr<AppExecFwk::IAppMgr> SceneSessionManager::GetAppManager()
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WLOGFE("Failed to get SystemAbilityManager.");
        return nullptr;
    }

    auto appObject = systemAbilityMgr->GetSystemAbility(APP_MGR_SERVICE_ID);
    if (appObject == nullptr) {
        WLOGFE("Failed to get AppManagerService.");
        return nullptr;
    }

    return iface_cast<AppExecFwk::IAppMgr>(appObject);
}

std::shared_ptr<Global::Resource::ResourceManager> SceneSessionManager::GetResourceManager(
    const AppExecFwk::AbilityInfo& abilityInfo)
{
    auto context = rootSceneContextWeak_.lock();
    if (!context) {
        WLOGFE("context is nullptr.");
        return nullptr;
    }
    auto resourceMgr = context->GetResourceManager();
    if (!resourceMgr) {
        WLOGFE("resourceMgr is nullptr.");
        return nullptr;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (!resConfig) {
        WLOGFE("resConfig is nullptr.");
        return nullptr;
    }
    resourceMgr->GetResConfig(*resConfig);
    resourceMgr = Global::Resource::CreateResourceManager(
        abilityInfo.bundleName, abilityInfo.moduleName, "", {}, *resConfig);
    if (!resourceMgr) {
        WLOGFE("resourceMgr is nullptr.");
        return nullptr;
    }
    resourceMgr->UpdateResConfig(*resConfig);

    std::string loadPath;
    if (!abilityInfo.hapPath.empty()) { // zipped hap
        loadPath = abilityInfo.hapPath;
    } else {
        loadPath = abilityInfo.resourcePath;
    }

    if (!resourceMgr->AddResource(loadPath.c_str(), Global::Resource::SELECT_COLOR | Global::Resource::SELECT_MEDIA)) {
        WLOGFW("Add resource %{private}s failed.", loadPath.c_str());
    }
    return resourceMgr;
}

bool SceneSessionManager::GetStartupPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo,
    std::string& path, uint32_t& bgColor)
{
    auto resourceMgr = GetResourceManager(abilityInfo);
    if (!resourceMgr) {
        WLOGFE("resourceMgr is nullptr.");
        return false;
    }

    if (resourceMgr->GetColorById(abilityInfo.startWindowBackgroundId, bgColor) != Global::Resource::RState::SUCCESS) {
        WLOGFE("Failed to get background color, id %{public}d.", abilityInfo.startWindowBackgroundId);
        return false;
    }

    if (resourceMgr->GetMediaById(abilityInfo.startWindowIconId, path) != Global::Resource::RState::SUCCESS) {
        WLOGFE("Failed to get icon, id %{public}d.", abilityInfo.startWindowIconId);
        return false;
    }

    if (!abilityInfo.hapPath.empty()) { // zipped hap
        auto pos = path.find_last_of('.');
        if (pos == std::string::npos) {
            WLOGFE("Format error, path %{private}s.", path.c_str());
            return false;
        }
        path = "resource:///" + std::to_string(abilityInfo.startWindowIconId) + path.substr(pos);
    }
    return true;
}

void SceneSessionManager::GetStartupPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor)
{
    if (!bundleMgr_) {
        WLOGFE("bundleMgr_ is nullptr.");
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetStartupPage");
    if (GetStartingWindowInfoFromCache(sessionInfo, path, bgColor)) {
        WLOGFI("Found in cache: %{public}s, %{public}x", path.c_str(), bgColor);
        return;
    }
    AAFwk::Want want;
    want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
    AppExecFwk::AbilityInfo abilityInfo;
    if (!bundleMgr_->QueryAbilityInfo(
        want, AppExecFwk::GET_ABILITY_INFO_DEFAULT, AppExecFwk::Constants::ANY_USERID, abilityInfo)) {
        WLOGFE("Get ability info from BMS failed!");
        return;
    }

    if (GetStartupPageFromResource(abilityInfo, path, bgColor)) {
        CacheStartingWindowInfo(abilityInfo, path, bgColor);
    }
    WLOGFI("%{public}d, %{public}d, %{public}s, %{public}x",
        abilityInfo.startWindowIconId, abilityInfo.startWindowBackgroundId, path.c_str(), bgColor);
}

bool SceneSessionManager::GetStartingWindowInfoFromCache(
    const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetStartingWindowInfoFromCache");
    std::shared_lock<std::shared_mutex> lock(startingWindowMapMutex_);
    auto iter = startingWindowMap_.find(sessionInfo.bundleName_);
    if (iter == startingWindowMap_.end()) {
        return false;
    }
    auto key = sessionInfo.moduleName_ + sessionInfo.abilityName_;
    const auto& infoMap = iter->second;
    auto infoMapIter = infoMap.find(key);
    if (infoMapIter == infoMap.end()) {
        return false;
    }
    path = infoMapIter->second.startingWindowIconPath_;
    bgColor = infoMapIter->second.startingWindowBackgroundColor_;
    return true;
}

void SceneSessionManager::CacheStartingWindowInfo(
    const AppExecFwk::AbilityInfo& abilityInfo, const std::string& path, const uint32_t& bgColor)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:CacheStartingWindowInfo");
    auto key = abilityInfo.moduleName + abilityInfo.name;
    StartingWindowInfo info = {
        .startingWindowBackgroundId_ = abilityInfo.startWindowBackgroundId,
        .startingWindowIconId_ = abilityInfo.startWindowIconId,
        .startingWindowBackgroundColor_ = bgColor,
        .startingWindowIconPath_ = path,
    };
    std::unique_lock<std::shared_mutex> lock(startingWindowMapMutex_);
    auto iter = startingWindowMap_.find(abilityInfo.bundleName);
    if (iter != startingWindowMap_.end()) {
        auto& infoMap = iter->second;
        infoMap.emplace(key, info);
        return;
    }
    if (startingWindowMap_.size() >= MAX_CACHE_COUNT) {
        startingWindowMap_.erase(startingWindowMap_.begin());
    }
    std::map<std::string, StartingWindowInfo> infoMap({{ key, info }});
    startingWindowMap_.emplace(abilityInfo.bundleName, infoMap);
}

void SceneSessionManager::OnBundleUpdated(const std::string& bundleName, int userId)
{
    taskScheduler_->PostAsyncTask([this, bundleName]() {
        std::unique_lock<std::shared_mutex> lock(startingWindowMapMutex_);
        auto iter = startingWindowMap_.find(bundleName);
        if (iter != startingWindowMap_.end()) {
            startingWindowMap_.erase(iter);
        }
    },
        "OnBundleUpdated");
}

void SceneSessionManager::OnConfigurationUpdated(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    taskScheduler_->PostAsyncTask([this]() {
        std::unique_lock<std::shared_mutex> lock(startingWindowMapMutex_);
        startingWindowMap_.clear();
    },
        "OnConfigurationUpdated");
}

void SceneSessionManager::FillSessionInfo(sptr<SceneSession>& sceneSession)
{
    auto sessionInfo = sceneSession->GetSessionInfo();
    if (sessionInfo.bundleName_.empty()) {
        WLOGFE("FillSessionInfo bundleName_ is null");
        return;
    }
    if (sessionInfo.isSystem_) {
        WLOGFD("FillSessionInfo systemScene!");
        return;
    }
    auto abilityInfo = QueryAbilityInfoFromBMS(currentUserId_, sessionInfo.bundleName_, sessionInfo.abilityName_,
        sessionInfo.moduleName_);
    if (abilityInfo == nullptr) {
        WLOGFE("FillSessionInfo abilityInfo is nullptr!");
        return;
    }
    sceneSession->SetSessionInfoAbilityInfo(abilityInfo);
    sceneSession->SetSessionInfoTime(GetCurrentTime());
    if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE)) {
        sceneSession->SetCollaboratorType(CollaboratorType::RESERVE_TYPE);
    } else if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE)) {
        sceneSession->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    }
    WLOGFI("FillSessionInfo end, removeMissionAfterTerminate: %{public}d excludeFromMissions: %{public}d "
           " label:%{public}s iconPath:%{public}s collaboratorType:%{public}s",
           abilityInfo->removeMissionAfterTerminate, abilityInfo->excludeFromMissions,
           abilityInfo->label.c_str(), abilityInfo->iconPath.c_str(), abilityInfo->applicationInfo.codePath.c_str());
}

std::shared_ptr<AppExecFwk::AbilityInfo> SceneSessionManager::QueryAbilityInfoFromBMS(const int32_t uId,
    const std::string& bundleName, const std::string& abilityName, const std::string& moduleName)
{
    AAFwk::Want want;
    want.SetElementName("", bundleName, abilityName, moduleName);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    if (abilityInfo == nullptr) {
        WLOGFE("QueryAbilityInfoFromBMS abilityInfo is nullptr!");
        return nullptr;
    }
    auto abilityInfoFlag = (AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA);
    bool ret = bundleMgr_->QueryAbilityInfo(want, abilityInfoFlag, uId, *abilityInfo);
    if (!ret) {
        WLOGFE("Get ability info from BMS failed!");
        return nullptr;
    }
    return abilityInfo;
}

WMError SceneSessionManager::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    const auto& callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, mainWinId, &topWinId, callingPid]() {
        const auto& mainSession = GetSceneSession(mainWinId);
        if (mainSession == nullptr) {
            return WMError::WM_ERROR_INVALID_WINDOW;
        }

        if (callingPid != mainSession->GetCallingPid()) {
            WLOGFE("Permission denied, not destroy by the same process");
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
        const auto& subVec = mainSession->GetSubSession();
        uint32_t zOrder = mainSession->GetZOrder();
        topWinId = mainWinId;
        for (const auto& subSession : subVec) {
            if (subSession != nullptr && (subSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                subSession->GetSessionState() == SessionState::STATE_ACTIVE) && subSession->GetZOrder() > zOrder) {
                topWinId = static_cast<uint32_t>(subSession->GetPersistentId());
                zOrder = subSession->GetZOrder();
                WLOGFD("[GetTopWin] Current zorder is larger than mainWin, mainId: %{public}d, topWinId: %{public}d, "
                    "zOrder: %{public}d", mainWinId, topWinId, zOrder);
            }
        }
        WLOGFD("[GetTopWin] Get top window, mainId: %{public}d, topWinId: %{public}d, zOrder: %{public}d",
            mainWinId, topWinId, zOrder);
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetTopWindowId");
}

WMError SceneSessionManager::UpdateSessionProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (action == WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE) {
        if (!SessionPermission::VerifyCallingPermission("ohos.permission.PRIVACY_WINDOW")) {
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
    }

    bool isSystemCalling = SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd();
    property->SetSystemCalling(isSystemCalling);
    wptr<SceneSessionManager> weak = this;
    auto task = [weak, property, action]() -> WMError {
        auto weakSession = weak.promote();
        if (weakSession == nullptr) {
            WLOGFE("the session is nullptr");
            return WMError::WM_DO_NOTHING;
        }
        if (property == nullptr) {
            WLOGFE("the property is nullptr");
            return WMError::WM_DO_NOTHING;
        }
        auto sceneSession = weakSession->GetSceneSession(property->GetPersistentId());
        if (sceneSession == nullptr) {
            WLOGFW("the scene session is nullptr, persistentId: %{public}d", property->GetPersistentId());
            return WMError::WM_DO_NOTHING;
        }
        WLOGD("Id: %{public}d, action: %{public}u", sceneSession->GetPersistentId(), action);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:UpdateProperty");
        return weakSession->HandleUpdateProperty(property, action, sceneSession);
    };
    return taskScheduler_->PostSyncTask(task, "UpdateProperty");
}

WMError SceneSessionManager::UpdatePropertyDragEnabled(const sptr<WindowSessionProperty>& property,
                                                       const sptr<SceneSession>& sceneSession)
{
    if (!property->GetSystemCalling()) {
        WLOGFE("Update property dragEnabled permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    if (sceneSession->GetSessionProperty() != nullptr) {
        sceneSession->GetSessionProperty()->SetDragEnabled(property->GetDragEnabled());
    }
    return WMError::WM_OK;
}

WMError SceneSessionManager::UpdatePropertyRaiseEnabled(const sptr<WindowSessionProperty>& property,
                                                        const sptr<SceneSession>& sceneSession)
{
    if (!property->GetSystemCalling()) {
        WLOGFE("Update property raiseEnabled permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    if (sceneSession->GetSessionProperty() != nullptr) {
        sceneSession->GetSessionProperty()->SetRaiseEnabled(property->GetRaiseEnabled());
    }
    return WMError::WM_OK;
}

void SceneSessionManager::HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    auto systemBarProperties = property->GetSystemBarProperty();
    for (auto iter : systemBarProperties) {
        if (iter.first == type) {
            sceneSession->SetSystemBarProperty(iter.first, iter.second);
            TLOGD(WmsLogTag::WMS_IMMS, "SetSystemBarProperty: %{public}d, enable: %{public}d",
                static_cast<int32_t>(iter.first), iter.second.enable_);
        }
    }
    NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
}

WMError SceneSessionManager::HandleUpdateProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action, const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFI("sceneSession is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (property == nullptr) {
        WLOGFI("property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
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
                return WMError::WM_OK;
            }
            // @todo if sceneSession is inactive, return
            SetBrightness(sceneSession, property->GetBrightness());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION: {
            sceneSession->SetRequestedOrientation(property->GetRequestedOrientation());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE: {
            bool isPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
            sceneSession->SetPrivacyMode(isPrivacyMode);
            UpdatePrivateStateAndNotify(sceneSession->GetPersistentId());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE: {
            bool isPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
            sceneSession->SetPrivacyMode(isPrivacyMode);
            UpdatePrivateStateAndNotify(sceneSession->GetPersistentId());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetMaximizeMode(property->GetMaximizeMode());
                sceneSession->GetSessionProperty()->SetIsLayoutFullScreen(property->IsLayoutFullScreen());
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
        case WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS: {
            HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS: {
            HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS: {
            HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_FLAGS: {
            SetWindowFlags(sceneSession, property);
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_MODE: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetWindowMode(property->GetWindowMode());
                ProcessSplitFloating();
            }
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
        case WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE: {
            if (property != nullptr && !property->GetSystemCalling()) {
                WLOGFE("update decor enable permission denied!");
                break;
            }
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetDecorEnable(property->IsDecorEnable());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetWindowLimits(property->GetWindowLimits());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED: {
            return UpdatePropertyDragEnabled(property, sceneSession);
        }
        case WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED: {
            return UpdatePropertyRaiseEnabled(property, sceneSession);
        }
        case WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS: {
            UpdateHideNonSystemFloatingWindows(property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetTextFieldPositionY(property->GetTextFieldPositionY());
                sceneSession->GetSessionProperty()->SetTextFieldHeight(property->GetTextFieldHeight());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetWindowMask(property->GetWindowMask());
                sceneSession->GetSessionProperty()->SetIsShaped(property->GetIsShaped());
                FlushWindowInfoToMMI();
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_TOPMOST: {
            return UpdateTopmostProperty(property, sceneSession);
        }
        default:
            break;
    }
    return WMError::WM_OK;
}

WMError SceneSessionManager::UpdateTopmostProperty(const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "UpdateTopmostProperty permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    sceneSession->SetTopmost(property->IsTopmost());
    return WMError::WM_OK;
}

void SceneSessionManager::UpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("Update property hideNonSystemFloatingWindows permission denied!");
        return;
    }

    auto propertyOld = sceneSession->GetSessionProperty();
    if (propertyOld == nullptr) {
        WLOGFI("UpdateHideNonSystemFloatingWindows, session property null");
        return;
    }

    bool hideNonSystemFloatingWindowsOld = propertyOld->GetHideNonSystemFloatingWindows();
    bool hideNonSystemFloatingWindowsNew = property->GetHideNonSystemFloatingWindows();
    if (hideNonSystemFloatingWindowsOld == hideNonSystemFloatingWindowsNew) {
        WLOGFI("property hideNonSystemFloatingWindows not change");
        return;
    }

    if (IsSessionVisible(sceneSession)) {
        if (hideNonSystemFloatingWindowsOld) {
            UpdateForceHideState(sceneSession, propertyOld, false);
        } else {
            UpdateForceHideState(sceneSession, property, true);
        }
    }
    propertyOld->SetHideNonSystemFloatingWindows(hideNonSystemFloatingWindowsNew);
}

void SceneSessionManager::UpdateForceHideState(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property, bool add)
{
    if (property == nullptr) {
        WLOGFD("property is nullptr");
        return;
    }
    auto persistentId = sceneSession->GetPersistentId();
    bool forceHideFloatOld = !systemTopSceneSessionMap_.empty();
    bool notifyAll = false;
    if (add) {
        if (property->GetHideNonSystemFloatingWindows()) {
            systemTopSceneSessionMap_.insert({ persistentId, sceneSession });
            notifyAll = !forceHideFloatOld;
        } else if (property->IsFloatingWindowAppType()) {
            nonSystemFloatSceneSessionMap_.insert({ persistentId, sceneSession });
            if (forceHideFloatOld) {
                sceneSession->NotifyForceHideChange(true);
            }
        }
    } else {
        if (property->GetHideNonSystemFloatingWindows()) {
            systemTopSceneSessionMap_.erase(persistentId);
            notifyAll = forceHideFloatOld && systemTopSceneSessionMap_.empty();
        } else if (property->IsFloatingWindowAppType()) {
            nonSystemFloatSceneSessionMap_.erase(persistentId);
            if (property->GetForceHide()) {
                sceneSession->NotifyForceHideChange(false);
            }
        }
    }
    if (notifyAll) {
        bool forceHideFloatNew = !systemTopSceneSessionMap_.empty();
        for (const auto &item : nonSystemFloatSceneSessionMap_) {
            auto forceHideSceneSession = item.second;
            auto forceHideProperty = forceHideSceneSession->GetSessionProperty();
            if (forceHideProperty && forceHideFloatNew != forceHideProperty->GetForceHide()) {
                forceHideSceneSession->NotifyForceHideChange(forceHideFloatNew);
            }
        }
    }
}

void SceneSessionManager::HandleTurnScreenOn(const sptr<SceneSession>& sceneSession)
{
#ifdef POWER_MANAGER_ENABLE
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
    taskScheduler_->PostAsyncTask(task, "HandleTurnScreenOn");

#else
    WLOGFD("Can not found the sub system of PowerMgr");
#endif
}

void SceneSessionManager::HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock)
{
#ifdef POWER_MANAGER_ENABLE
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, requireLock]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is invalid");
            return;
        }
        if (requireLock && scnSession->keepScreenLock_ == nullptr) {
            // reset ipc identity
            std::string identity = IPCSkeleton::ResetCallingIdentity();
            scnSession->keepScreenLock_ =
                PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(scnSession->GetWindowName(),
                PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
            // set ipc identity to raw
            IPCSkeleton::SetCallingIdentity(identity);
        }
        if (scnSession->keepScreenLock_ == nullptr) {
            return;
        }
        bool shouldLock = requireLock && IsSessionVisible(scnSession);
        WLOGI("keep screen on: [%{public}s, %{public}d]", scnSession->GetWindowName().c_str(), shouldLock);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:HandleKeepScreenOn");
        ErrCode res;
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        if (shouldLock) {
            res = scnSession->keepScreenLock_->Lock();
        } else {
            res = scnSession->keepScreenLock_->UnLock();
        }
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
        if (res != ERR_OK) {
            WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]",
                requireLock, res);
        }
    };
    taskScheduler_->PostAsyncTask(task, "HandleKeepScreenOn");
#else
    WLOGFD("Can not found the sub system of PowerMgr");
#endif
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
#ifdef POWERMGR_DISPLAY_MANAGER_ENABLE
    if (GetDisplayBrightness() != brightness && eventHandler_ != nullptr) {
        bool setBrightnessRet = false;
        if (std::fabs(brightness - UNDEFINED_BRIGHTNESS) < std::numeric_limits<float>::min()) {
            auto task = []() {
                DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            };
            setBrightnessRet = eventHandler_->PostTask(task, "DisplayPowerMgr:RestoreBrightness", 0);
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        } else {
            auto task = [brightness]() {
                DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                    static_cast<uint32_t>(brightness * MAX_BRIGHTNESS));
            };
            setBrightnessRet = eventHandler_->PostTask(task, "DisplayPowerMgr:OverrideBrightness", 0);
            SetDisplayBrightness(brightness);
        }
        if (!setBrightnessRet) {
            WLOGFE("Report post listener callback task failed. the task name is SetBrightness");
        }
    }
#else
    WLOGFD("Can not found the sub system of DisplayPowerMgr");
#endif
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
    if (!(sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW ||
            sceneSession->GetSessionInfo().isSystem_)) {
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
    gestureNavigationEnabled_ = enable;
    auto task = [this, enable]() {
        if (!gestureNavigationEnabledChangeFunc_ && !statusBarEnabledChangeFunc_) {
            WLOGFE("callback func is null");
            return WMError::WM_DO_NOTHING;
        }
        if (gestureNavigationEnabledChangeFunc_) {
            gestureNavigationEnabledChangeFunc_(enable);
        }
        if (statusBarEnabledChangeFunc_) {
            statusBarEnabledChangeFunc_(enable);
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetGestureNavigaionEnabled");
}

WSError SceneSessionManager::SetFocusedSessionId(int32_t persistentId)
{
    if (focusedSessionId_ == persistentId) {
        WLOGI("Focus scene not change, id: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    lastFocusedSessionId_ = focusedSessionId_;
    focusedSessionId_ = persistentId;
    return WSError::WS_OK;
}

int32_t SceneSessionManager::GetFocusedSessionId() const
{
    return focusedSessionId_;
}

void SceneSessionManager::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    if (!SessionPermission::IsSACalling()) {
        WLOGFE("GetFocusWindowInfo permission denied!");
        return;
    }
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

void SceneSessionManager::RegisterSessionExceptionFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifySessionExceptionFunc sessionExceptionFunc = [this](const SessionInfo& info, bool needRemoveSession = false) {
        auto task = [this, info]() {
            auto scnSession = GetSceneSession(info.persistentId_);
            if (scnSession == nullptr) {
                TLOGW(WmsLogTag::WMS_LIFE, "NotifySessionExceptionFunc, Not found session, id: %{public}d",
                    info.persistentId_);
                return;
            }
            if (listenerController_ == nullptr) {
                TLOGW(WmsLogTag::WMS_LIFE, "NotifySessionExceptionFunc, listenerController_ is nullptr");
                return;
            }
            if (scnSession->GetSessionInfo().isSystem_) {
                TLOGW(WmsLogTag::WMS_LIFE, "NotifySessionExceptionFunc, id: %{public}d is system",
                    scnSession->GetPersistentId());
                return;
            }
            TLOGI(WmsLogTag::WMS_LIFE, "NotifySessionExceptionFunc, errorCode: %{public}d, id: %{public}d",
                info.errorCode, info.persistentId_);
            if (info.errorCode == static_cast<int32_t>(AAFwk::ErrorLifecycleState::ABILITY_STATE_LOAD_TIMEOUT) ||
                info.errorCode == static_cast<int32_t>(AAFwk::ErrorLifecycleState::ABILITY_STATE_FOREGROUND_TIMEOUT)) {
                TLOGD(WmsLogTag::WMS_LIFE, "NotifySessionClosed when ability load timeout \
                    or foreground timeout, id: %{public}d", info.persistentId_);
                listenerController_->NotifySessionClosed(info.persistentId_);
            }
        };
        taskScheduler_->PostVoidSyncTask(task, "sessionException");
    };
    sceneSession->SetSessionExceptionListener(sessionExceptionFunc, false);
    TLOGD(WmsLogTag::WMS_LIFE, "RegisterSessionExceptionFunc success, id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::RegisterSessionSnapshotFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifySessionSnapshotFunc sessionSnapshotFunc = [this](int32_t persistentId) {
        auto task = [this, persistentId]() {
            auto scnSession = GetSceneSession(persistentId);
            if (scnSession == nullptr) {
                WLOGFW("NotifySessionSnapshotFunc, Not found session, id: %{public}d", persistentId);
                return;
            }
            if (scnSession->GetSessionInfo().isSystem_) {
                WLOGFW("NotifySessionSnapshotFunc, id: %{public}d is system", scnSession->GetPersistentId());
                return;
            }
            auto abilityInfoPtr = scnSession->GetSessionInfo().abilityInfo;
            if (abilityInfoPtr == nullptr) {
                WLOGFW("NotifySessionSnapshotFunc, abilityInfoPtr is nullptr");
                return;
            }
            if (listenerController_ == nullptr) {
                WLOGFW("NotifySessionSnapshotFunc, listenerController_ is nullptr");
                return;
            }
            if (!(abilityInfoPtr->excludeFromMissions)) {
                listenerController_->NotifySessionSnapshotChanged(persistentId);
            }
        };
        taskScheduler_->PostVoidSyncTask(task, "sessionSnapshot");
    };
    sceneSession->SetSessionSnapshotListener(sessionSnapshotFunc);
    WLOGFD("RegisterSessionSnapshotFunc success, id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::NotifySessionForCallback(const sptr<SceneSession>& scnSession, const bool needRemoveSession)
{
    if (scnSession == nullptr) {
        WLOGFW("NotifySessionForCallback, scnSession is nullptr");
        return;
    }
    if (scnSession->GetSessionInfo().isSystem_) {
        WLOGFW("NotifySessionForCallback, id: %{public}d is system", scnSession->GetPersistentId());
        return;
    }
    WLOGFI("NotifySessionForCallback, id: %{public}d, needRemoveSession: %{public}u", scnSession->GetPersistentId(),
           static_cast<uint32_t>(needRemoveSession));
    if (scnSession->GetSessionInfo().appIndex_ != 0) {
        WLOGFI("NotifySessionDestroy, appIndex_: %{public}d, id: %{public}d",
               scnSession->GetSessionInfo().appIndex_, scnSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(scnSession->GetPersistentId());
        return;
    }
    if (needRemoveSession) {
        WLOGFI("NotifySessionDestroy, needRemoveSession, id: %{public}d", scnSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(scnSession->GetPersistentId());
        return;
    }
    if (scnSession->GetSessionInfo().abilityInfo == nullptr) {
        WLOGFW("abilityInfo is nullptr, id: %{public}d", scnSession->GetPersistentId());
    } else if ((scnSession->GetSessionInfo().abilityInfo)->removeMissionAfterTerminate ||
               (scnSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
        WLOGFI("NotifySessionDestroy, removeMissionAfterTerminate or excludeFromMissions, id: %{public}d",
            scnSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(scnSession->GetPersistentId());
        return;
    }
    WLOGFI("NotifySessionClosed, id: %{public}d", scnSession->GetPersistentId());
    listenerController_->NotifySessionClosed(scnSession->GetPersistentId());
}


void SceneSessionManager::NotifyWindowInfoChangeFromSession(int32_t persistentId)
{
    WLOGFD("NotifyWindowInfoChange, persistentId = %{public}d", persistentId);
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession nullptr");
        return;
    }

    SceneInputManager::GetInstance().NotifyWindowInfoChangeFromSession(sceneSession);
}

bool SceneSessionManager::IsSessionVisible(const sptr<SceneSession>& session)
{
    if (session == nullptr) {
        return false;
    }
    const auto& state = session->GetSessionState();
    if (WindowHelper::IsSubWindow(session->GetWindowType())) {
        const auto& parentSceneSession = GetSceneSession(session->GetParentPersistentId());
        if (parentSceneSession == nullptr) {
            WLOGFW("Can not find parent for this sub window, id: %{public}d", session->GetPersistentId());
            return false;
        }
        const auto& parentState = parentSceneSession->GetSessionState();
        if (session->IsVisible() || (state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND)) {
            if (parentState == SessionState::STATE_INACTIVE || parentState == SessionState::STATE_BACKGROUND) {
                WLOGFD("Parent of this sub window is at background, id: %{public}d", session->GetPersistentId());
                return false;
            }
            WLOGFD("Sub window is at foreground, id: %{public}d", session->GetPersistentId());
            return true;
        }
        WLOGFD("Sub window is at background, id: %{public}d", session->GetPersistentId());
        return false;
    }

    if (session->IsVisible() || state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND) {
        WLOGFD("Window is at foreground, id: %{public}d", session->GetPersistentId());
        return true;
    }
    WLOGFD("Window is at background, id: %{public}d", session->GetPersistentId());
    return false;
}

void SceneSessionManager::DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss)
{
    if (session == nullptr) {
        return;
    }
    int32_t zOrder = IsSessionVisible(session) ? static_cast<int32_t>(session->GetZOrder()) : -1;
    WSRect rect = session->GetSessionRect();
    std::string sName;
    if (session->GetSessionInfo().isSystem_) {
        sName = session->GetSessionInfo().abilityName_;
    } else {
        sName = session->GetWindowName();
    }
    uint32_t flag = 0;
    uint64_t displayId = INVALID_SCREEN_ID;
    if (session->GetSessionProperty()) {
        flag = session->GetSessionProperty()->GetWindowFlags();
        displayId = session->GetSessionProperty()->GetDisplayId();
    }
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
        << " [ "
        << std::left << std::setw(OFFSET_MAX_WIDTH) << session->GetOffsetX()
        << std::left << std::setw(OFFSET_MAX_WIDTH) << session->GetOffsetY()
        << "]"
        << " [ "
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetScaleX()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetScaleY()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetPivotX()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetPivotY()
        << "]"
        << std::endl;
}

void SceneSessionManager::DumpAllAppSessionInfo(std::ostringstream& oss,
    const std::map<int32_t, sptr<SceneSession>>& sceneSessionMap)
{
    oss << std::endl << "Current mission lists:" << std::endl;
    oss << " MissionList Type #NORMAL" << std::endl;
    for (const auto& elem : sceneSessionMap) {
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
        << " [ OffsetX OffsetY ] [ ScaleX  ScaleY  PivotX  PivotY  ]"
        << std::endl;

    std::vector<sptr<SceneSession>> allSession;
    std::vector<sptr<SceneSession>> backgroundSession;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    for (const auto& elem : sceneSessionMapCopy) {
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
    oss << "Focus window: " << GetFocusedSessionId() << std::endl;
    oss << "Total window num: " << sceneSessionMapCopy.size() << std::endl;
    DumpAllAppSessionInfo(oss, sceneSessionMapCopy);
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
    oss << "FirstFrameCallbackCalled: " << isFirstFrameAvailable << std::endl;
    oss << "IsVisible: " << isVisible << std::endl;
    oss << "Focusable: "  << Focusable << std::endl;
    oss << "DecoStatus: "  << DecoStatus << std::endl;
    oss << "isPrivacyMode: "  << isPrivacyMode << std::endl;
    oss << "WindowRect: " << "[ "
        << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_
        << " ]" << std::endl;
    oss << "Offset: " << "[ "
        << session->GetOffsetX() << ", " << session->GetOffsetY() << " ]" << std::endl;
    oss << "Scale: " << "[ "
        << session->GetScaleX() << ", " << session->GetScaleY() << ", "
        << session->GetPivotX() << ", " << session->GetPivotY()
        << " ]" << std::endl;
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
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        WLOGFE("GetSessionDumpInfo permission denied!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, params, &dumpInfo]() {
        if (params.size() == 1 && params[0] == ARG_DUMP_ALL) { // 1: params num
            return GetAllSessionDumpInfo(dumpInfo);
        }
        if (params.size() >= 2 && params[0] == ARG_DUMP_WINDOW && IsValidDigitString(params[1])) { // 2: params num
            return GetSpecifiedSessionDumpInfo(dumpInfo, params, params[1]);
        }
        return WSError::WS_ERROR_INVALID_OPERATION;
    };
    return taskScheduler_->PostSyncTask(task);
}

void FocusIDChange(int32_t persistentId, sptr<SceneSession>& sceneSession)
{
    // notify RS
    WLOGFD("current focus session: windowId: %{public}d, windowName: %{public}s, bundleName: %{public}s,"
        " abilityName: %{public}s, pid: %{public}d, uid: %{public}d", persistentId,
        sceneSession->GetSessionProperty()->GetWindowName().c_str(),
        sceneSession->GetSessionInfo().bundleName_.c_str(),
        sceneSession->GetSessionInfo().abilityName_.c_str(),
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid());
    uint64_t focusNodeId = 0; // 0 means invalid
    if (sceneSession->GetSurfaceNode() == nullptr) {
        WLOGFW("focused window surfaceNode is null");
    } else {
        focusNodeId = sceneSession->GetSurfaceNode()->GetId();
    }
    FocusAppInfo appInfo = {
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid(),
        sceneSession->GetSessionInfo().bundleName_,
        sceneSession->GetSessionInfo().abilityName_, focusNodeId};
    RSInterfaces::GetInstance().SetFocusAppInfo(appInfo);
}

// ordered vector by compare func
std::vector<std::pair<int32_t, sptr<SceneSession>>> SceneSessionManager::GetSceneSessionVector(CmpFunc cmp)
{
    std::vector<std::pair<int32_t, sptr<SceneSession>>> ret;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter : sceneSessionMap_) {
            ret.push_back(iter);
        }
    }
    std::sort(ret.begin(), ret.end(), cmp);
    return ret;
}

void SceneSessionManager::TraverseSessionTree(TraverseFunc func, bool isFromTopToBottom)
{
    if (isFromTopToBottom) {
        TraverseSessionTreeFromTopToBottom(func);
    } else {
        TraverseSessionTreeFromBottomToTop(func);
    }
    return;
}

void SceneSessionManager::TraverseSessionTreeFromTopToBottom(TraverseFunc func)
{
    CmpFunc cmp = [](std::pair<int32_t, sptr<SceneSession>>& lhs, std::pair<int32_t, sptr<SceneSession>>& rhs) {
        uint32_t lhsZOrder = lhs.second != nullptr ? lhs.second->GetZOrder() : 0;
        uint32_t rhsZOrder = rhs.second != nullptr ? rhs.second->GetZOrder() : 0;
        return lhsZOrder < rhsZOrder;
    };
    auto sceneSessionVector = GetSceneSessionVector(cmp);

    for (auto iter = sceneSessionVector.rbegin(); iter != sceneSessionVector.rend(); ++iter) {
        auto session = iter->second;
        if (session == nullptr) {
            WLOGFE("session is nullptr");
            continue;
        }
        if (func(session)) {
            return;
        }
    }
    return;
}

void SceneSessionManager::TraverseSessionTreeFromBottomToTop(TraverseFunc func)
{
    // std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    CmpFunc cmp = [](std::pair<int32_t, sptr<SceneSession>>& lhs, std::pair<int32_t, sptr<SceneSession>>& rhs) {
        uint32_t lhsZOrder = lhs.second != nullptr ? lhs.second->GetZOrder() : 0;
        uint32_t rhsZOrder = rhs.second != nullptr ? rhs.second->GetZOrder() : 0;
        return lhsZOrder < rhsZOrder;
    };
    auto sceneSessionVector = GetSceneSessionVector(cmp);
    // std::map<int32_t, sptr<SceneSession>>::iterator iter;
    for (auto iter = sceneSessionVector.begin(); iter != sceneSessionVector.end(); ++iter) {
        auto session = iter->second;
        if (session == nullptr) {
            WLOGFE("session is nullptr");
            continue;
        }
        if (func(session)) {
            return;
        }
    }
    return;
}

WMError SceneSessionManager::RequestFocusStatus(int32_t persistentId, bool isFocused, bool byForeground,
    FocusChangeReason reason)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "id: %{public}d, reason: %{public}d", persistentId, reason);
    auto task = [this, persistentId, isFocused, byForeground, reason]() {
        if (isFocused) {
            RequestSessionFocus(persistentId, byForeground, reason);
        } else {
            RequestSessionUnfocus(persistentId, reason);
        }
    };
    taskScheduler_->PostAsyncTask(task, "RequestFocusStatus" + std::to_string(persistentId));
    return WMError::WM_OK;
}

void SceneSessionManager::ResetFocusedOnShow(int32_t persistentId)
{
    auto task = [this, persistentId]() {
        TLOGI(WmsLogTag::WMS_FOCUS, "ResetFocusedOnShow, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_FOCUS, "session is nullptr");
            return;
        }
        if (sceneSession->IsSessionForeground()) {
            TLOGI(WmsLogTag::WMS_FOCUS, "SetFocusedOnShow to true, id: %{public}d", persistentId);
            sceneSession->SetFocusedOnShow(true);
        }
    };
    taskScheduler_->PostAsyncTask(task, "ResetFocusedOnShow" + std::to_string(persistentId));
}

void SceneSessionManager::RequestAllAppSessionUnfocus()
{
    auto task = [this]() {
        RequestAllAppSessionUnfocusInner();
    };
    taskScheduler_->PostAsyncTask(task, "RequestAllAppSessionUnfocus");
    return;
}

/**
 * request focus and ignore its state
 * only used when app main window start before foreground
*/
WSError SceneSessionManager::RequestSessionFocusImmediately(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "RequestSessionFocusImmediately, id: %{public}d", persistentId);
    // base block
    WSError basicCheckRet = RequestFocusBasicCheck(persistentId);
    if (basicCheckRet != WSError::WS_OK) {
        return basicCheckRet;
    }
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!sceneSession->GetFocusable()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focusable!");
        return WSError::WS_DO_NOTHING;
    }
    if (!sceneSession->IsFocusedOnShow()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focused on show!");
        return WSError::WS_DO_NOTHING;
    }

    // specific block
    WSError specificCheckRet = RequestFocusSpecificCheck(sceneSession, true);
    if (specificCheckRet != WSError::WS_OK) {
        return specificCheckRet;
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    if (!IsSessionVisible(sceneSession)) {
        needBlockNotifyFocusStatusUntilForeground_ = true;
    }
    ShiftFocus(sceneSession, FocusChangeReason::SCB_START_APP);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSessionFocus(int32_t persistentId, bool byForeground, FocusChangeReason reason)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "id: %{public}d, by foreground: %{public}d, reason: %{public}d",
        persistentId, byForeground, reason);
    WSError basicCheckRet = RequestFocusBasicCheck(persistentId);
    if (basicCheckRet != WSError::WS_OK) {
        return basicCheckRet;
    }
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!sceneSession->GetFocusable() || !IsSessionVisible(sceneSession)) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focusable or not visible!");
        return WSError::WS_DO_NOTHING;
    }
    if (!sceneSession->IsFocusedOnShow()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focused on show!");
        return WSError::WS_DO_NOTHING;
    }

    // subwindow/dialog state block
    if ((WindowHelper::IsSubWindow(sceneSession->GetWindowType()) ||
        sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
        GetSceneSession(sceneSession->GetParentPersistentId()) &&
        !IsSessionVisible(GetSceneSession(sceneSession->GetParentPersistentId()))) {
            TLOGD(WmsLogTag::WMS_FOCUS, "parent session id: %{public}d is not visible!",
                sceneSession->GetParentPersistentId());
            return WSError::WS_DO_NOTHING;
    }
    // specific block
    WSError specificCheckRet = RequestFocusSpecificCheck(sceneSession, byForeground);
    if (specificCheckRet != WSError::WS_OK) {
        return specificCheckRet;
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    ShiftFocus(sceneSession, reason);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSessionUnfocus(int32_t persistentId, FocusChangeReason reason)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "RequestSessionUnfocus, id: %{public}d", persistentId);
    if (persistentId == INVALID_SESSION_ID) {
        WLOGFE("id is invalid");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (persistentId != focusedSessionId_ &&
        !(focusedSession && focusedSession->GetParentPersistentId() == persistentId)) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session unfocused!");
        return WSError::WS_DO_NOTHING;
    }
    // if pop menu created by desktop request unfocus, back to desktop
    auto lastSession = GetSceneSession(lastFocusedSessionId_);
    if (focusedSession && focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_SYSTEM_FLOAT &&
        lastSession && lastSession->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP &&
        RequestSessionFocus(lastFocusedSessionId_, false) == WSError::WS_OK) {
            TLOGD(WmsLogTag::WMS_FOCUS, "focus is back to desktop");
            return WSError::WS_OK;
    }
    auto nextSession = GetNextFocusableSession(persistentId);

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    return ShiftFocus(nextSession, reason);
}

WSError SceneSessionManager::RequestAllAppSessionUnfocusInner()
{
    TLOGI(WmsLogTag::WMS_FOCUS, "RequestAllAppSessionUnfocus");
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (!focusedSession) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focused session is null");
        return WSError::WS_DO_NOTHING;
    }
    if (!focusedSession->IsAppSession()) {
        WLOGW("[WMFocus]Focused session is non app session: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    auto nextSession = GetTopFocusableNonAppSession();

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    return ShiftFocus(nextSession);
}

WSError SceneSessionManager::RequestFocusBasicCheck(int32_t persistentId)
{
    // basic focus rule
    if (persistentId == INVALID_SESSION_ID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "id is invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (persistentId == focusedSessionId_) {
        TLOGD(WmsLogTag::WMS_FOCUS, "request id has been focused!");
        return WSError::WS_DO_NOTHING;
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestFocusSpecificCheck(sptr<SceneSession>& sceneSession, bool byForeground)
{
    int32_t persistentId = sceneSession->GetPersistentId();
    // dialog get focus
    if ((sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW ||
        SessionHelper::IsSubWindow(sceneSession->GetWindowType())) &&
        ProcessDialogRequestFocusImmdediately(sceneSession) == WSError::WS_OK) {
            TLOGD(WmsLogTag::WMS_FOCUS, "dialog get focused");
            return WSError::WS_DO_NOTHING;
    }
    // blocking-type session will block lower zOrder request focus
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (focusedSession) {
        if (focusedSession->IsTopmost() && sceneSession->IsAppSession()) {
            // return ok if focused session is topmost
            return WSError::WS_OK;
        }
        bool isBlockingType = focusedSession->IsAppSession() ||
            (focusedSession->GetSessionInfo().isSystem_ && focusedSession->GetBlockingFocus());
        if (byForeground && isBlockingType && sceneSession->GetZOrder() < focusedSession->GetZOrder()) {
            TLOGD(WmsLogTag::WMS_FOCUS, "session %{public}d is lower than focused session %{public}d",
                persistentId, focusedSessionId_);
            return WSError::WS_DO_NOTHING;
        }
        // temp check
        if (isBlockingType && focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD &&
            sceneSession->GetZOrder() < focusedSession->GetZOrder()) {
                TLOGD(WmsLogTag::WMS_FOCUS, "Lower session %{public}d cannot request focus from keyguard!",
                    persistentId);
                return WSError::WS_DO_NOTHING;
        }
    }
    return WSError::WS_OK;
}

sptr<SceneSession> SceneSessionManager::GetNextFocusableSession(int32_t persistentId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "GetNextFocusableSession, id: %{public}d", persistentId);
    bool previousFocusedSessionFound = false;
    sptr<SceneSession> ret = nullptr;
    auto func = [this, persistentId, &previousFocusedSessionFound, &ret](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        if (session->GetForceHideState()) {
            TLOGD(WmsLogTag::WMS_FOCUS, "the window hide id: %{public}d", persistentId);
            return false;
        }
        bool parentVisible = true;
        if ((WindowHelper::IsSubWindow(session->GetWindowType()) ||
            session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
            GetSceneSession(session->GetParentPersistentId()) &&
            !IsSessionVisible(GetSceneSession(session->GetParentPersistentId()))) {
                parentVisible = false;
        }
        if (previousFocusedSessionFound && session->GetFocusable() && IsSessionVisible(session) && parentVisible) {
            ret = session;
            return true;
        }
        if (session->GetPersistentId() == persistentId) {
            previousFocusedSessionFound = true;
        }
        return false;
    };
    TraverseSessionTree(func, true);
    return ret;
}

sptr<SceneSession> SceneSessionManager::GetTopFocusableNonAppSession()
{
    TLOGD(WmsLogTag::WMS_FOCUS, "GetTopFocusableNonAppSession.");
    sptr<SceneSession> ret = nullptr;
    auto func = [this, &ret](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        if (session->IsAppSession()) {
            return true;
        }
        if (session->GetFocusable() && IsSessionVisible(session)) {
            ret = session;
        }
        return false;
    };
    TraverseSessionTree(func, false);
    return ret;
}

void SceneSessionManager::SetShiftFocusListener(const ProcessShiftFocusFunc& func)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "SetShiftFocusListener");
    shiftFocusFunc_ = func;
}

void SceneSessionManager::SetSCBFocusedListener(const NotifySCBAfterUpdateFocusFunc& func)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "SetSCBFocusedListener");
    notifySCBAfterFocusedFunc_ = func;
}

void SceneSessionManager::SetSCBUnfocusedListener(const NotifySCBAfterUpdateFocusFunc& func)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "SetSCBUnfocusedListener");
    notifySCBAfterUnfocusedFunc_ = func;
}

void SceneSessionManager::SetCallingSessionIdSessionListenser(const ProcessCallingSessionIdChangeFunc& func)
{
    WLOGFD("SetCallingSessionIdSessionListenser");
    callingSessionIdChangeFunc_ = func;
}

void SceneSessionManager::SetSwitchingToAnotherUserListener(const ProcessSwitchingToAnotherUserFunc& func)
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "Set switching to another user listener");
    switchingToAnotherUserFunc_ = func;
}

void SceneSessionManager::SetStartUIAbilityErrorListener(const ProcessStartUIAbilityErrorFunc& func)
{
    WLOGFD("SetStartUIAbilityErrorListener");
    startUIAbilityErrorFunc_ = func;
}

WSError SceneSessionManager::ShiftFocus(sptr<SceneSession>& nextSession, FocusChangeReason reason)
{
    changeReason_ = reason;
    // unfocus
    int32_t focusedId = focusedSessionId_;
    auto focusedSession = GetSceneSession(focusedSessionId_);
    UpdateFocusStatus(focusedSession, false);
    // focus
    int32_t nextId = INVALID_SESSION_ID;
    if (nextSession == nullptr) {
        std::string sessionLog(GetAllSessionFocusInfo());
        TLOGW(WmsLogTag::WMS_FOCUS, "ShiftFocus to nullptr! id: %{public}d, info: %{public}s",
            focusedSessionId_, sessionLog.c_str());
    } else {
        nextId = nextSession->GetPersistentId();
    }
    UpdateFocusStatus(nextSession, true);
    bool scbPrevFocus = focusedSession && focusedSession->GetSessionInfo().isSystem_;
    bool scbCurrFocus = nextSession && nextSession->GetSessionInfo().isSystem_;
    AnomalyDetection::FocusCheckProcess(focusedId, nextId);
    if (!scbPrevFocus && scbCurrFocus) {
        if (notifySCBAfterFocusedFunc_ != nullptr) {
            notifySCBAfterFocusedFunc_();
        }
    } else if (scbPrevFocus && !scbCurrFocus) {
        if (notifySCBAfterUnfocusedFunc_ != nullptr) {
            notifySCBAfterUnfocusedFunc_();
        }
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "ShiftFocus, focusedId: %{public}d, nextId: %{public}d, reason: %{public}d",
        focusedId, nextId, reason);
    return WSError::WS_OK;
}

FocusChangeReason SceneSessionManager::GetFocusChangeReason() const
{
    return changeReason_;
}

void SceneSessionManager::UpdateFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused)
{
    if (sceneSession == nullptr) {
        if (isFocused) {
            SetFocusedSessionId(INVALID_SESSION_ID);
        }
        return;
    }
    TLOGD(WmsLogTag::WMS_FOCUS, "UpdateFocusStatus, name: %{public}s, id: %{public}d, isFocused: %{public}d",
        sceneSession->GetWindowNameAllType().c_str(), sceneSession->GetPersistentId(), isFocused);
    // set focused
    if (isFocused) {
        SetFocusedSessionId(sceneSession->GetPersistentId());
    }
    sceneSession->UpdateFocus(isFocused);
    if ((isFocused && !needBlockNotifyFocusStatusUntilForeground_) || (!isFocused && !needBlockNotifyUnfocusStatus_)) {
        NotifyFocusStatus(sceneSession, isFocused);
    }
}

void SceneSessionManager::NotifyFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused)
{
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return;
    }
    int32_t persistentId = sceneSession->GetPersistentId();

    if (isFocused) {
        if (IsSessionVisible(sceneSession)) {
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
        }
        UpdateBrightness(focusedSessionId_);
        FocusIDChange(sceneSession->GetPersistentId(), sceneSession);
        if (shiftFocusFunc_ != nullptr) {
            shiftFocusFunc_(persistentId);
        }
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
#ifdef EFFICIENCY_MANAGER_ENABLE
        SuspendManager::SuspendManagerClient::GetInstance().ThawOneApplication(focusChangeInfo->uid_,
            "", "THAW_BY_FOCUS_CHANGED");
#endif // EFFICIENCY_MANAGER_ENABLE
    SessionManagerAgentController::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, isFocused);
    sceneSession->NotifyFocusStatus(isFocused);
    std::string sName = "FoucusWindow:";
    if (sceneSession->GetSessionInfo().isSystem_) {
        sName += sceneSession->GetSessionInfo().abilityName_;
    } else {
        sName += sceneSession->GetWindowName();
    }
    if (isFocused) {
        StartAsyncTrace(HITRACE_TAG_WINDOW_MANAGER, sName, sceneSession->GetPersistentId());
    } else {
        FinishAsyncTrace(HITRACE_TAG_WINDOW_MANAGER, sName, sceneSession->GetPersistentId());
    }

    // notify listenerController
    auto prevSession = GetSceneSession(lastFocusedSessionId_);
    if (isFocused && MissionChanged(prevSession, sceneSession)) {
        NotifyFocusStatusByMission(prevSession, sceneSession);
    }
}

void SceneSessionManager::NotifyFocusStatusByMission(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession)
{
    if (listenerController_ != nullptr) {
        if (prevSession && !prevSession->GetSessionInfo().isSystem_) {
            TLOGD(WmsLogTag::WMS_FOCUS, "NotifyMissionUnfocused, id: %{public}d", prevSession->GetMissionId());
            listenerController_->NotifySessionUnfocused(prevSession->GetMissionId());
        }
        if (currSession && !currSession->GetSessionInfo().isSystem_) {
            TLOGD(WmsLogTag::WMS_FOCUS, "NotifyMissionFocused, id: %{public}d", currSession->GetMissionId());
            listenerController_->NotifySessionFocused(currSession->GetMissionId());
        }
    }
}

bool SceneSessionManager::MissionChanged(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession)
{
    if (prevSession == nullptr && currSession == nullptr) {
        return false;
    }
    if (prevSession == nullptr || currSession == nullptr) {
        return true;
    }
    return prevSession->GetMissionId() != currSession->GetMissionId();
}

std::string SceneSessionManager::GetAllSessionFocusInfo()
{
    std::ostringstream os;
    auto func = [&os](sptr<SceneSession> session) {
        if (session == nullptr) {
            WLOGE("sceneSession is nullptr");
            return false;
        }
        os << "WindowName: " << session->GetWindowName() << ", id: " << session->GetPersistentId() <<
           " ,focusable: "<< session->GetFocusable() << ";";
        return false;
    };
    TraverseSessionTree(func, true);
    return os.str();
}

WSError SceneSessionManager::UpdateFocus(int32_t persistentId, bool isFocused)
{
    auto task = [this, persistentId, isFocused]() {
        // notify session and client
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("UpdateFocus could not find window, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        WLOGFI("UpdateFocus, name: %{public}s, id: %{public}d, isFocused: %{public}u",
            sceneSession->GetWindowName().c_str(), persistentId, static_cast<uint32_t>(isFocused));
        // focusId change
        if (isFocused) {
            SetFocusedSessionId(persistentId);
            UpdateBrightness(focusedSessionId_);
            FocusIDChange(persistentId, sceneSession);
        } else if (persistentId == GetFocusedSessionId()) {
            SetFocusedSessionId(INVALID_SESSION_ID);
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
        WLOGFI("UpdateFocus, id: %{public}d, isSystem: %{public}d", sceneSession->GetPersistentId(),
               sceneSession->GetSessionInfo().isSystem_);
        if (listenerController_ != nullptr && !sceneSession->GetSessionInfo().isSystem_) {
            if (isFocused) {
                WLOGFD("NotifySessionFocused, id: %{public}d", sceneSession->GetPersistentId());
                listenerController_->NotifySessionFocused(sceneSession->GetPersistentId());
            } else {
                WLOGFD("NotifySessionUnfocused, id: %{public}d", sceneSession->GetPersistentId());
                listenerController_->NotifySessionUnfocused(sceneSession->GetPersistentId());
            }
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "UpdateFocus" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateWindowMode(int32_t persistentId, int32_t windowMode)
{
    WLOGFD("update window mode, id: %{public}d, mode: %{public}d", persistentId, windowMode);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("could not find window, persistentId:%{public}d", persistentId);
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    WindowMode mode = static_cast<WindowMode>(windowMode);
    ProcessBackHomeStatus();
    return sceneSession->UpdateWindowMode(mode);
}

#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
static void FillSecCompEnhanceData(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    MMI::PointerEvent::PointerItem& pointerItem)
{
    struct PointerEventData {
        double x;
        double y;
        uint64_t time;
    } pointerEventData = {
        .x = pointerItem.GetDisplayX(),
        .y = pointerItem.GetDisplayY(),
        .time = pointerEvent->GetActionTime()
    };

    const uint32_t MAX_HMAC_SIZE = 64;
    uint8_t outBuf[MAX_HMAC_SIZE] = { 0 };
    uint8_t *enhanceData = reinterpret_cast<uint8_t *>(&outBuf[0]);
    uint32_t enhanceDataLen = MAX_HMAC_SIZE;
    if (Security::SecurityComponent::SecCompEnhanceKit::GetPointerEventEnhanceData(&pointerEventData,
        sizeof(pointerEventData), enhanceData, enhanceDataLen) == 0) {
        pointerEvent->SetEnhanceData(std::vector<uint8_t>(outBuf, outBuf + enhanceDataLen));
    }
}
#endif // SECURITY_COMPONENT_MANAGER_ENABLE

WSError SceneSessionManager::SendTouchEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, uint32_t zIndex)
{
    if (!pointerEvent) {
        WLOGFE("pointerEvent is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        WLOGFE("Failed to get pointerItem");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
    FillSecCompEnhanceData(pointerEvent, pointerItem);
#endif
    WLOGFI("[EventDispatch] SendTouchEvent PointerId = %{public}d, action = %{public}d",
        pointerEvent->GetPointerId(), pointerEvent->GetPointerAction());
    MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent, static_cast<float>(zIndex));
    return WSError::WS_OK;
}

void SceneSessionManager::SetScreenLocked(const bool isScreenLocked)
{
    isScreenLocked_ = isScreenLocked;
    DeleteStateDetectTask();
}

void SceneSessionManager::DeleteStateDetectTask()
{
    if (!IsScreenLocked()) {
        return;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto iter : sceneSessionMap_) {
        auto& session = iter.second;
        if (session->GetDetectTaskInfo().taskState != DetectTaskState::NO_TASK) {
            taskScheduler_->GetEventHandler()->RemoveTask(session->GetWindowDetectTaskName());
            DetectTaskInfo detectTaskInfo;
            session->SetDetectTaskInfo(detectTaskInfo);
        }
    }
}

bool SceneSessionManager::IsScreenLocked() const
{
    return isScreenLocked_;
}

void SceneSessionManager::RegisterWindowChanged(const WindowChangedFunc& func)
{
    WindowChangedFunc_ = func;
}

void SceneSessionManager::UpdatePrivateStateAndNotify(uint32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "update privacy state failed, scene is nullptr, wid = %{public}u.", persistentId);
        return;
    }

    auto sessionProperty = sceneSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "get session property failed, wid = %{public}u.", persistentId);
        return;
    }
    auto displayId = sessionProperty->GetDisplayId();
    std::vector<std::string> privacyBundleList;
    GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);

    ScreenSessionManagerClient::GetInstance().SetPrivacyStateByDisplayId(displayId, !privacyBundleList.empty());
    ScreenSessionManagerClient::GetInstance().SetScreenPrivacyWindowList(displayId, privacyBundleList);
}

void SceneSessionManager::GetSceneSessionPrivacyModeBundles(DisplayId displayId,
    std::vector<std::string>& privacyBundles)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& item : sceneSessionMap_) {
        sptr<SceneSession> sceneSession = item.second;
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "scene session is nullptr, wid = %{public}d.", item.first);
            continue;
        }
        auto sessionProperty = sceneSession->GetSessionProperty();
        if (sessionProperty == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "scene session property is nullptr, wid = %{public}d.", item.first);
            continue;
        }
        auto currentDisplayId = sessionProperty->GetDisplayId();
        if (displayId != currentDisplayId) {
            continue;
        }
        bool isForeground =  sceneSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
            sceneSession->GetSessionState() == SessionState::STATE_ACTIVE;
        if (isForeground && sceneSession->GetParentSession() != nullptr) {
            isForeground = isForeground &&
                (sceneSession->GetParentSession()->GetSessionState() == SessionState::STATE_FOREGROUND ||
                sceneSession->GetParentSession()->GetSessionState() == SessionState::STATE_ACTIVE);
        }
        bool isPrivate = sessionProperty->GetPrivacyMode() ||
            sceneSession->GetCombinedExtWindowFlags().privacyModeFlag;
        bool IsSystemWindowVisible = sceneSession->GetSessionInfo().isSystem_ && sceneSession->IsVisible();
        if ((isForeground || IsSystemWindowVisible) && isPrivate) {
            privacyBundles.push_back(sceneSession->GetSessionInfo().bundleName_);
        }
    }
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

void SceneSessionManager::RegisterSessionInfoChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    wptr<SceneSessionManager> weakSessionManager = this;
    NotifySessionInfoChangeNotifyManagerFunc func = [weakSessionManager](int32_t persistentId) {
        auto sceneSessionManager = weakSessionManager.promote();
        if (sceneSessionManager == nullptr) {
            return;
        }
        sceneSessionManager->NotifyWindowInfoChangeFromSession(persistentId);
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetSessionInfoChangeNotifyManagerListener(func);
}

void SceneSessionManager::RegisterRequestFocusStatusNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    NotifyRequestFocusStatusNotifyManagerFunc func =
    [this](int32_t persistentId, const bool isFocused, const bool byForeground) {
        FocusChangeReason reason = FocusChangeReason::CLICK;
        this->RequestFocusStatus(persistentId, isFocused, byForeground, reason);
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetRequestFocusStatusNotifyManagerListener(func);
    WLOGFD("RegisterSessionUpdateFocusStatusFunc success");
}

void SceneSessionManager::RegisterGetStateFromManagerFunc(sptr<SceneSession>& sceneSession)
{
    GetStateFromManagerFunc func = [this](const ManagerState key) {
        switch (key)
        {
        case ManagerState::MANAGER_STATE_SCREEN_LOCKED:
            return this->IsScreenLocked();
            break;
        default:
            return false;
            break;
        }
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetGetStateFromManagerListener(func);
    WLOGFD("RegisterGetStateFromManagerFunc success");
}

__attribute__((no_sanitize("cfi"))) void SceneSessionManager::OnSessionStateChange(
    int32_t persistentId, const SessionState& state)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:OnSessionStateChange%d", persistentId);
    WLOGFD("Session state change, id: %{public}d, state:%{public}u", persistentId, state);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    switch (state) {
        case SessionState::STATE_FOREGROUND:
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
                persistentId == focusedSessionId_) {
                if (needBlockNotifyFocusStatusUntilForeground_) {
                    needBlockNotifyUnfocusStatus_ = false;
                    needBlockNotifyFocusStatusUntilForeground_ = false;
                    NotifyFocusStatus(sceneSession, true);
                }
            } else if (!sceneSession->IsFocusedOnShow()) {
                sceneSession->SetFocusedOnShow(true);
            } else {
                RequestSessionFocus(persistentId, true, FocusChangeReason::APP_FOREGROUND);
            }
            UpdateForceHideState(sceneSession, sceneSession->GetSessionProperty(), true);
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_ADDED);
            HandleKeepScreenOn(sceneSession, sceneSession->IsKeepScreenOn());
            UpdatePrivateStateAndNotify(persistentId);
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                ProcessSubSessionForeground(sceneSession);
            }
            break;
        case SessionState::STATE_BACKGROUND:
            RequestSessionUnfocus(persistentId, FocusChangeReason::APP_BACKGROUND);
            UpdateForceHideState(sceneSession, sceneSession->GetSessionProperty(), false);
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
            HandleKeepScreenOn(sceneSession, false);
            UpdatePrivateStateAndNotify(persistentId);
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                ProcessSubSessionBackground(sceneSession);
            }
            ProcessBackHomeStatus();
            break;
        default:
            break;
    }
    ProcessSplitFloating();
}

void SceneSessionManager::ProcessSplitFloating()
{
    if (isScreenLocked_) {
        return;
    }
    bool inSplit = false;
    bool inFloating = false;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& session : sceneSessionMap_) {
            if (session.second == nullptr ||
                !WindowHelper::IsMainWindow(session.second->GetWindowType()) ||
                !Rosen::SceneSessionManager::GetInstance().IsSessionVisible(session.second)) {
                continue;
            }
            auto mode = session.second->GetWindowMode();
            if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
                inSplit = true;
            }
            if (mode == WindowMode::WINDOW_MODE_FLOATING) {
                inFloating = true;
            }
        }
    }
    NotifyRSSWindowModeTypeUpdate(inSplit, inFloating);
}

void SceneSessionManager::NotifyRSSWindowModeTypeUpdate(bool inSplit, bool inFloating)
{
    WindowModeType type;
    if (inSplit) {
        if (inFloating) {
            type = WindowModeType::WINDOW_MODE_SPLIT_FLOATING;
        } else {
            type = WindowModeType::WINDOW_MODE_SPLIT;
        }
    } else {
        if (inFloating) {
            type = WindowModeType::WINDOW_MODE_FLOATING;
        } else {
            type = WindowModeType::WINDOW_MODE_OTHER;
        }
    }
    if (lastWindowModeType_ == type) {
        return;
    }
    lastWindowModeType_ = type;
    TLOGI(WmsLogTag::WMS_MAIN, "Notify RSS Window Mode Type Update, type : %{public}d",
        static_cast<uint8_t>(type));
    SessionManagerAgentController::GetInstance().UpdateWindowModeTypeInfo(type);
}

void SceneSessionManager::ProcessBackHomeStatus()
{
    TLOGD(WmsLogTag::WMS_MAIN, "ProcessBackHomeStatus");
    if (IsBackHomeStatus()) {
        SessionManagerAgentController::GetInstance().UpdateWindowBackHomeStatus(true);
        TLOGI(WmsLogTag::WMS_MAIN, "ProcessBackHomeStatus go back home status");
    }
}

bool SceneSessionManager::IsBackHomeStatus()
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "IsBackHomeStatus, sceneSession is null");
            continue;
        }
        auto mode = sceneSession->GetWindowMode();
        auto state = sceneSession->GetSessionState();
        if ((WindowHelper::IsMainWindow(sceneSession->GetWindowType())) &&
            (WindowHelper::IsFullScreenWindow(mode) || WindowHelper::IsSplitWindowMode(mode)) &&
            (state == SessionState::STATE_FOREGROUND || state == SessionState::STATE_ACTIVE)) {
            return false;
        }
    }
    TLOGI(WmsLogTag::WMS_MAIN, "IsBackHomeStatus, true");
    return true;
}

void SceneSessionManager::ProcessSubSessionForeground(sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    for (const auto& subSession : sceneSession->GetSubSession()) {
        if (subSession == nullptr) {
            WLOGFD("sub session is nullptr");
            continue;
        }
        const auto& state = subSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            WLOGFD("sub session is not active");
            continue;
        }
        RequestSessionFocus(subSession->GetPersistentId(), true);
        NotifyWindowInfoChange(subSession->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        HandleKeepScreenOn(subSession, subSession->IsKeepScreenOn());
    }
    std::vector<sptr<Session>> dialogVec = sceneSession->GetDialogVector();
    for (const auto& dialog : dialogVec) {
        if (dialog == nullptr) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog is nullptr");
            continue;
        }
        const auto& state = dialog->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog is not active");
            continue;
        }
        auto dialogSession = GetSceneSession(dialog->GetPersistentId());
        if (dialogSession == nullptr) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialogSession is null");
            continue;
        }
        NotifyWindowInfoChange(dialog->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        if (dialog->GetPersistentId() == focusedSessionId_ && needBlockNotifyFocusStatusUntilForeground_) {
            needBlockNotifyUnfocusStatus_ = false;
            needBlockNotifyFocusStatusUntilForeground_ = false;
            NotifyFocusStatus(dialogSession, true);
        }
        HandleKeepScreenOn(dialogSession, dialogSession->IsKeepScreenOn());
    }
}

WSError SceneSessionManager::ProcessDialogRequestFocusImmdediately(sptr<SceneSession>& sceneSession)
{
    // focus must on dialog when APP_MAIN_WINDOW or sub winodw request focus
    sptr<SceneSession> mainSession = nullptr;
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        mainSession = sceneSession;
    } else if (SessionHelper::IsSubWindow(sceneSession->GetWindowType())) {
        mainSession = GetSceneSession(sceneSession->GetParentPersistentId());
    }
    if (mainSession == nullptr) {
        TLOGD(WmsLogTag::WMS_FOCUS, "main window is nullptr");
        return WSError::WS_DO_NOTHING;
    }
    std::vector<sptr<Session>> dialogVec = mainSession->GetDialogVector();
    if (std::find_if(dialogVec.begin(), dialogVec.end(),
        [this](sptr<Session>& iter) { return iter && iter->GetPersistentId() == focusedSessionId_; })
        != dialogVec.end()) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog id: %{public}d has been focused!", focusedSessionId_);
            return WSError::WS_OK;
    }
    WSError ret = WSError::WS_DO_NOTHING;
    for (auto dialog : dialogVec) {
        if (dialog == nullptr) {
            continue;
        }
        // no need to consider order, since rule of zOrder
        if (RequestSessionFocusImmediately(dialog->GetPersistentId()) == WSError::WS_OK) {
            ret = WSError::WS_OK;
        }
    }
    return ret;
}

void SceneSessionManager::ProcessSubSessionBackground(sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    for (const auto& subSession : sceneSession->GetSubSession()) {
        if (subSession == nullptr) {
            WLOGFD("sub session is nullptr");
            continue;
        }
        const auto& state = subSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            WLOGFD("sub session is not active");
            continue;
        }
        NotifyWindowInfoChange(subSession->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        HandleKeepScreenOn(subSession, false);
        UpdatePrivateStateAndNotify(subSession->GetPersistentId());
    }
    std::vector<sptr<Session>> dialogVec = sceneSession->GetDialogVector();
    for (const auto& dialog : dialogVec) {
        if (dialog == nullptr) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog is nullptr");
            continue;
        }
        auto dialogSession = GetSceneSession(dialog->GetPersistentId());
        NotifyWindowInfoChange(dialog->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        HandleKeepScreenOn(dialogSession, false);
        UpdatePrivateStateAndNotify(dialog->GetPersistentId());
    }
}

WSError SceneSessionManager::SetWindowFlags(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto sessionProperty = sceneSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    uint32_t flags = property->GetWindowFlags();
    uint32_t oldFlags = sessionProperty->GetWindowFlags();
    if (((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED) ||
        (oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) &&
        !property->GetSystemCalling()) {
            WLOGFE("Set window flags permission denied");
            return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    sessionProperty->SetWindowFlags(flags);
    CheckAndNotifyWaterMarkChangedResult();
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) {
        sceneSession->OnShowWhenLocked(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    }
    WLOGFI("SetWindowFlags end, flags: %{public}u", flags);
    return WSError::WS_OK;
}

void SceneSessionManager::CheckAndNotifyWaterMarkChangedResult()
{
    bool currentWaterMarkShowState = false;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter: sceneSessionMap_) {
            auto& session = iter.second;
            if (!session || !session->GetSessionProperty()) {
                continue;
            }
            bool hasWaterMark = session->GetSessionProperty()->GetWindowFlags()
                & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
            bool isExtWindowHasWaterMarkFlag = session->GetCombinedExtWindowFlags().waterMarkFlag;
            if ((hasWaterMark && session->GetVisible()) || isExtWindowHasWaterMarkFlag) {
                currentWaterMarkShowState = true;
                break;
            }
        }
        if (waterMarkSessionSet_.size() != 0) {
            TLOGI(WmsLogTag::WMS_UIEXT, "CheckAndNotifyWaterMarkChangedResult scb uiext has water mark");
            currentWaterMarkShowState = true;
        }
    }
    if (lastWaterMarkShowState_ != currentWaterMarkShowState) {
        lastWaterMarkShowState_ = currentWaterMarkShowState;
        NotifyWaterMarkFlagChangedResult(currentWaterMarkShowState);
    }
    return;
}

WSError SceneSessionManager::NotifyWaterMarkFlagChangedResult(bool hasWaterMark)
{
    WLOGFI("WaterMark status : %{public}u", static_cast<uint32_t>(hasWaterMark));
    SessionManagerAgentController::GetInstance().NotifyWaterMarkFlagChangedResult(hasWaterMark);
    return WSError::WS_OK;
}

void SceneSessionManager::ProcessPreload(const AppExecFwk::AbilityInfo &abilityInfo) const
{
    if (!bundleMgr_) {
        WLOGFE("bundle manager is nullptr.");
        return;
    }

    AAFwk::Want want;
    want.SetElementName(abilityInfo.deviceId, abilityInfo.bundleName, abilityInfo.name, abilityInfo.moduleName);
    auto uid = abilityInfo.uid;
    want.SetParam("uid", uid);
    bundleMgr_->ProcessPreload(want);
}

void SceneSessionManager::NotifyCompleteFirstFrameDrawing(int32_t persistentId)
{
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, " scnSession is nullptr.");
        return;
    }

    const auto& sessionInfo = scnSession->GetSessionInfo();
    TLOGI(WmsLogTag::WMS_MAIN, " id: %{public}d, app info: [%{public}s %{public}s %{public}s]",
        scnSession->GetPersistentId(), sessionInfo.bundleName_.c_str(),
        sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str());
    auto abilityInfoPtr = sessionInfo.abilityInfo;
    if (abilityInfoPtr == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, " abilityInfoPtr is nullptr, persistentId: %{public}d", persistentId);
        return;
    }
    if ((listenerController_ != nullptr) && !scnSession->GetSessionInfo().isSystem_ &&
        !(abilityInfoPtr->excludeFromMissions)) {
        WLOGFD("NotifySessionCreated, id: %{public}d", persistentId);
        listenerController_->NotifySessionCreated(persistentId);
    }

    if (eventHandler_ != nullptr) {
        auto task = [persistentId]() {
            AAFwk::AbilityManagerClient::GetInstance()->CompleteFirstFrameDrawing(persistentId);
        };
        WLOGFI("Post CompleteFirstFrameDrawing task.");
        bool ret = eventHandler_->PostTask(task, "wms:CompleteFirstFrameDrawing", 0);
        if (!ret) {
            WLOGFE("Report post first frame task failed. the task name is CompleteFirstFrameDrawing");
        }
    }

    if (taskScheduler_ == nullptr) {
        return;
    }
    auto task = [this, abilityInfoPtr]() {
        ProcessPreload(*abilityInfoPtr);
    };
    return taskScheduler_->PostAsyncTask(task, "NotifyCompleteFirstFrameDrawing" + std::to_string(persistentId));
}

void SceneSessionManager::NotifySessionMovedToFront(int32_t persistentId)
{
    WLOGFI("NotifySessionMovedToFront, persistentId: %{public}d", persistentId);
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("session is invalid with %{public}d", persistentId);
        return;
    }
    WLOGFI("NotifySessionMovedToFront, id: %{public}d, isSystem: %{public}d", scnSession->GetPersistentId(),
           scnSession->GetSessionInfo().isSystem_);
    if (listenerController_ != nullptr &&
        !scnSession->GetSessionInfo().isSystem_ &&
        (scnSession->GetSessionInfo().abilityInfo) != nullptr &&
        !(scnSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
        listenerController_->NotifySessionMovedToFront(persistentId);
    }
}

WSError SceneSessionManager::SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label)
{
    WLOGFI("run SetSessionLabel");

    auto task = [this, &token, &label]() {
        auto sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token");
            return WSError::WS_ERROR_SET_SESSION_LABEL_FAILED;
        }
        sceneSession->SetSessionLabel(label);
        if (sessionListener_ != nullptr) {
            WLOGFI("try to run OnSessionLabelChange");
            sessionListener_->OnSessionLabelChange(sceneSession->GetPersistentId(), label);
        }
        WLOGFI("NotifySessionLabelUpdated, id: %{public}d, isSystem: %{public}d", sceneSession->GetPersistentId(),
            sceneSession->GetSessionInfo().isSystem_);
        if (listenerController_ != nullptr && !sceneSession->GetSessionInfo().isSystem_) {
            WLOGFD("NotifySessionLabelUpdated, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionLabelUpdated(sceneSession->GetPersistentId());
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionLabel");
}

WSError SceneSessionManager::SetSessionIcon(const sptr<IRemoteObject> &token,
    const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFI("run SetSessionIcon");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, &token, &icon]() {
        auto sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token");
            return WSError::WS_ERROR_SET_SESSION_LABEL_FAILED;
        }
        sceneSession->SetSessionIcon(icon);
        if (sessionListener_ != nullptr) {
            WLOGFI("try to run OnSessionIconChange.");
            sessionListener_->OnSessionIconChange(sceneSession->GetPersistentId(), icon);
        }
        WLOGFI("NotifySessionIconChanged, id: %{public}d, isSystem: %{public}d", sceneSession->GetPersistentId(),
            sceneSession->GetSessionInfo().isSystem_);
        if (listenerController_ != nullptr &&
            !sceneSession->GetSessionInfo().isSystem_ &&
            (sceneSession->GetSessionInfo().abilityInfo) != nullptr &&
            !(sceneSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
            WLOGFD("NotifySessionIconChanged, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionIconChanged(sceneSession->GetPersistentId(), icon);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionIcon");
}

WSError SceneSessionManager::IsValidSessionIds(
    const std::vector<int32_t> &sessionIds, std::vector<bool> &results)
{
    WLOGFI("run IsValidSessionIds");
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto i = 0; i < static_cast<int32_t>(sessionIds.size()); ++i) {
        auto search = sceneSessionMap_.find(sessionIds.at(i));
        if (search == sceneSessionMap_.end() || search->second == nullptr) {
            results.push_back(false);
            continue;
        }
        results.push_back(true);
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::RegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run RegisterSessionListener");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &listener]() {
        WSError ret = WSError::WS_DO_NOTHING;
        if (listenerController_ != nullptr) {
            ret = listenerController_->AddSessionListener(listener);
        } else {
            WLOGFE("The listenerController is nullptr");
        }

        // app continue report for distributed scheduled service
        SingletonContainer::Get<DmsReporter>().ReportContinueApp(ret == WSError::WS_OK,
            static_cast<int32_t>(ret));

        return ret;
    };
    return taskScheduler_->PostSyncTask(task, "AddSessionListener");
}

WSError SceneSessionManager::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run UnRegisterSessionListener");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &listener]() {
        if (listenerController_ != nullptr) {
            listenerController_->DelSessionListener(listener);
            return WSError::WS_OK;
        } else {
            WLOGFE("The listenerController is nullptr");
            return WSError::WS_DO_NOTHING;
        }
    };
    return taskScheduler_->PostSyncTask(task, "DelSessionListener");
}

WSError SceneSessionManager::GetSessionInfos(const std::string& deviceId, int32_t numMax,
                                             std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("run GetSessionInfos");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &deviceId, numMax, &sessionInfos]() {
        if (CheckIsRemote(deviceId)) {
            int ret = GetRemoteSessionInfos(deviceId, numMax, sessionInfos);
            if (ret != ERR_OK) {
                return WSError::WS_ERROR_INVALID_PARAM;
            } else {
                return WSError::WS_OK;
            }
        }
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::vector<sptr<SceneSession>> sceneSessionInfos;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
            auto sceneSession = iter->second;
            if (sceneSession == nullptr) {
                WLOGFE("session is nullptr");
                continue;
            }
            auto sessionInfo = sceneSession->GetSessionInfo();
            if (sessionInfo.isSystem_) {
                WLOGFD("sessionId: %{public}d  isSystemScene", sceneSession->GetPersistentId());
                continue;
            }
            auto want = sessionInfo.want;
            if (want == nullptr || sessionInfo.bundleName_.empty() || want->GetElement().GetBundleName().empty()) {
                WLOGFE("session: %{public}d, want is null or bundleName is empty or want bundleName is empty",
                    sceneSession->GetPersistentId());
                continue;
            }
            if (static_cast<int>(sceneSessionInfos.size()) >= numMax) {
                break;
            }
            WLOGFD("GetSessionInfos session: %{public}d, bundleName:%{public}s", sceneSession->GetPersistentId(),
                sessionInfo.bundleName_.c_str());
            sceneSessionInfos.emplace_back(sceneSession);
        }
        return SceneSessionConverter::ConvertToMissionInfos(sceneSessionInfos, sessionInfos);
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionInfos");
}

int SceneSessionManager::GetRemoteSessionInfos(const std::string& deviceId, int32_t numMax,
                                               std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("GetRemoteSessionInfos From Dms begin");
    DistributedClient dmsClient;
    int result = dmsClient.GetMissionInfos(deviceId, numMax, sessionInfos);
    if (result != ERR_OK) {
        WLOGFE("GetRemoteMissionInfos failed, result = %{public}d", result);
        return result;
    }
    return ERR_OK;
}

WSError SceneSessionManager::GetSessionInfo(const std::string& deviceId,
                                            int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFI("run GetSessionInfo");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &deviceId, persistentId, &sessionInfo]() {
        if (CheckIsRemote(deviceId)) {
            int ret = GetRemoteSessionInfo(deviceId, persistentId, sessionInfo);
            if (ret != ERR_OK) {
                return WSError::WS_ERROR_INVALID_PARAM;
            } else {
                return WSError::WS_OK;
            }
        }
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        iter = sceneSessionMap_.find(persistentId);
        if (iter != sceneSessionMap_.end()) {
            auto sceneSession = iter->second;
            if (sceneSession == nullptr) {
                WLOGFE("session: %{public}d is nullptr", persistentId);
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            auto sceneSessionInfo = sceneSession->GetSessionInfo();
            if (sceneSessionInfo.isSystem_) {
                WLOGFD("sessionId: %{public}d  isSystemScene", persistentId);
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            auto want = sceneSessionInfo.want;
            if (want == nullptr || sceneSessionInfo.bundleName_.empty() ||
                want->GetElement().GetBundleName().empty()) {
                WLOGFE("session: %{public}d, want is null or bundleName is empty or want bundleName is empty",
                    persistentId);
                return WSError::WS_ERROR_INTERNAL_ERROR;
            }
            WLOGFD("GetSessionInfo sessionId:%{public}d bundleName:%{public}s", persistentId,
                sceneSessionInfo.bundleName_.c_str());
            return SceneSessionConverter::ConvertToMissionInfo(iter->second, sessionInfo);
        } else {
            WLOGFW("sessionId: %{public}d not found", persistentId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionInfo");
}

WSError SceneSessionManager::GetSessionInfoByContinueSessionId(const std::string& continueSessionId,
    SessionInfoBean& sessionInfo)
{
    TLOGI(WmsLogTag::WMS_LIFE, "query session info with continueSessionId: %{public}s",
        continueSessionId.c_str());
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The interface only support for system service.");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted.");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, continueSessionId, &sessionInfo]() {
        WSError ret = WSError::WS_ERROR_INVALID_SESSION;
        {
            std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            for (auto& [persistentId, sceneSession] : sceneSessionMap_) {
                if (sceneSession && sceneSession->GetSessionInfo().continueSessionId_ == continueSessionId) {
                    ret = SceneSessionConverter::ConvertToMissionInfo(sceneSession, sessionInfo);
                    break;
                }
            }
        }

        TLOGI(WmsLogTag::WMS_LIFE, "get session info finished with ret code: %{public}d", ret);
        // app continue report for distributed scheduled service
        SingletonContainer::Get<DmsReporter>().ReportQuerySessionInfo(ret == WSError::WS_OK,
            static_cast<int32_t>(ret));
        return ret;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionInfoByContinueSessionId");
}

int SceneSessionManager::GetRemoteSessionInfo(const std::string& deviceId,
                                              int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFI("GetRemoteSessionInfoFromDms begin");
    std::vector<SessionInfoBean> sessionVector;
    int result = GetRemoteSessionInfos(deviceId, MAX_NUMBER_OF_DISTRIBUTED_SESSIONS, sessionVector);
    if (result != ERR_OK) {
        return result;
    }
    for (auto iter = sessionVector.begin(); iter != sessionVector.end(); iter++) {
        if (iter->id == persistentId) {
            sessionInfo = *iter;
            return ERR_OK;
        }
    }
    WLOGFW("missionId not found");
    return ERR_INVALID_VALUE;
}

bool SceneSessionManager::CheckIsRemote(const std::string& deviceId)
{
    if (deviceId.empty()) {
        WLOGFI("CheckIsRemote: deviceId is empty.");
        return false;
    }
    std::string localDeviceId;
    if (!GetLocalDeviceId(localDeviceId)) {
        WLOGFE("CheckIsRemote: get local deviceId failed");
        return false;
    }
    if (localDeviceId == deviceId) {
        WLOGFI("CheckIsRemote: deviceId is local.");
        return false;
    }
    WLOGFD("CheckIsRemote, deviceId = %{public}s", AnonymizeDeviceId(deviceId).c_str());
    return true;
}

bool SceneSessionManager::GetLocalDeviceId(std::string& localDeviceId)
{
    auto localNode = std::make_unique<NodeBasicInfo>();
    int32_t errCode = GetLocalNodeDeviceInfo(DM_PKG_NAME.c_str(), localNode.get());
    if (errCode != ERR_OK) {
        WLOGFE("GetLocalNodeDeviceInfo errCode = %{public}d", errCode);
        return false;
    }
    if (localNode != nullptr) {
        localDeviceId = localNode->networkId;
        WLOGFD("get local deviceId, deviceId = %{public}s", AnonymizeDeviceId(localDeviceId).c_str());
        return true;
    }
    WLOGFE("localDeviceId null");
    return false;
}

std::string SceneSessionManager::AnonymizeDeviceId(const std::string& deviceId)
{
    if (deviceId.length() < NON_ANONYMIZE_LENGTH) {
        return EMPTY_DEVICE_ID;
    }
    std::string anonDeviceId = deviceId.substr(0, NON_ANONYMIZE_LENGTH);
    anonDeviceId.append("******");
    return anonDeviceId;
}

WSError SceneSessionManager::DumpSessionAll(std::vector<std::string> &infos)
{
    WLOGFI("Dump all session.");
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("DumpSessionAll permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, &infos]() {
        std::string dumpInfo = "User ID #" + std::to_string(currentUserId_);
        infos.push_back(dumpInfo);
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto& session = item.second;
            if (session) {
                session->DumpSessionInfo(infos);
            }
        }
        return WSError::WS_OK;
    };

    return taskScheduler_->PostSyncTask(task, "DumpSessionAll");
}

WSError SceneSessionManager::DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos)
{
    WLOGFI("Dump session with id %{public}d", persistentId);
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("DumpSessionWithId permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, persistentId, &infos]() {
        std::string dumpInfo = "User ID #" + std::to_string(currentUserId_);
        infos.push_back(dumpInfo);
        auto session = GetSceneSession(persistentId);
        if (session) {
            session->DumpSessionInfo(infos);
        } else {
            infos.push_back("error: invalid mission number, please see 'aa dump --mission-list'.");
        }
        return WSError::WS_OK;
    };

    return taskScheduler_->PostSyncTask(task, "DumpSessionWithId");
}

WSError SceneSessionManager::GetAllAbilityInfos(const AAFwk::Want &want, int32_t userId,
    std::vector<AppExecFwk::AbilityInfo> &abilityInfos)
{
    if (bundleMgr_ == nullptr) {
        WLOGFE("bundleMgr_ is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto elementName = want.GetElement();
    int32_t ret{0};
    auto flag = (AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_ABILITY) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE));
    std::vector<AppExecFwk::BundleInfo> bundleInfos;
    if (elementName.GetBundleName().empty() && elementName.GetAbilityName().empty()) {
        WLOGFD("want is empty queryAllAbilityInfos");
        ret = static_cast<int32_t>(bundleMgr_->GetBundleInfosV9(flag, bundleInfos, userId));
        if (ret) {
            WLOGFE("Query all ability infos from BMS failed!");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    } else if (!elementName.GetBundleName().empty()) {
        AppExecFwk::BundleInfo bundleInfo;
        WLOGFD("bundleName is not empty, query abilityInfo of %{public}s", elementName.GetBundleName().c_str());
        ret = static_cast<int32_t>(bundleMgr_->GetBundleInfoV9(elementName.GetBundleName(), flag, bundleInfo, userId));
        if (ret) {
            WLOGFE("Query ability info from BMS failed!");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        bundleInfos.push_back(bundleInfo);
    } else {
        WLOGFE("invalid want:%{public}s", want.ToString().c_str());
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    return GetAbilityInfosFromBundleInfo(bundleInfos, abilityInfos);
}

WSError SceneSessionManager::GetAbilityInfosFromBundleInfo(std::vector<AppExecFwk::BundleInfo> &bundleInfos,
    std::vector<AppExecFwk::AbilityInfo> &abilityInfos)
{
    if (bundleInfos.empty()) {
        WLOGFE("bundleInfos is empty");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    for (auto bundleInfo: bundleInfos) {
        auto hapModulesList = bundleInfo.hapModuleInfos;
        if (hapModulesList.empty()) {
            WLOGFD("hapModulesList is empty");
            continue;
        }
        for (auto hapModule: hapModulesList) {
            auto abilityInfoList = hapModule.abilityInfos;
            abilityInfos.insert(abilityInfos.end(), abilityInfoList.begin(), abilityInfoList.end());
        }
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::TerminateSessionNew(
    const sptr<AAFwk::SessionInfo> info, bool needStartCaller, bool isFromBroker)
{
    if (info == nullptr) {
        TLOGI(WmsLogTag::WMS_LIFE, "sessionInfo is nullptr.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    TLOGI(WmsLogTag::WMS_LIFE,
        "bundleName=%{public}s, needStartCaller=%{public}d, isFromBroker=%{public}d",
        info->want.GetElement().GetBundleName().c_str(), needStartCaller, isFromBroker);
    auto task = [this, info, needStartCaller, isFromBroker]() {
        sptr<SceneSession> sceneSession = FindSessionByToken(info->sessionToken);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "TerminateSessionNew:fail to find session by token.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        const WSError& errCode = sceneSession->TerminateSessionNew(info, needStartCaller, isFromBroker);
        return errCode;
    };
    return taskScheduler_->PostSyncTask(task, "TerminateSessionNew");
}

WSError SceneSessionManager::GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                                                SessionSnapshot& snapshot, bool isLowResolution)
{
    WLOGFI("run GetSessionSnapshot");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &deviceId, persistentId, &snapshot, isLowResolution]() {
        if (CheckIsRemote(deviceId)) {
            int ret = GetRemoteSessionSnapshotInfo(deviceId, persistentId, snapshot);
            if (ret != ERR_OK) {
                return WSError::WS_ERROR_INVALID_PARAM;
            } else {
                return WSError::WS_OK;
            }
        }
        sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
        if (!sceneSession) {
            WLOGFE("fail to find session by persistentId: %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        auto sessionInfo = sceneSession->GetSessionInfo();
        if (sessionInfo.abilityName_.empty() || sessionInfo.moduleName_.empty() || sessionInfo.bundleName_.empty()) {
            WLOGFW("sessionInfo: %{public}d, abilityName or moduleName or bundleName is empty",
                   sceneSession->GetPersistentId());
        }
        snapshot.topAbility.SetElementBundleName(&(snapshot.topAbility), sessionInfo.bundleName_.c_str());
        snapshot.topAbility.SetElementModuleName(&(snapshot.topAbility), sessionInfo.moduleName_.c_str());
        snapshot.topAbility.SetElementAbilityName(&(snapshot.topAbility), sessionInfo.abilityName_.c_str());
        auto oriSnapshot = sceneSession->Snapshot();
        if (oriSnapshot != nullptr) {
            if (isLowResolution) {
                OHOS::Media::InitializationOptions options;
                options.size.width = oriSnapshot->GetWidth() / 2; // low resolution ratio
                options.size.height = oriSnapshot->GetHeight() / 2; // low resolution ratio
                std::unique_ptr<OHOS::Media::PixelMap> reducedPixelMap = OHOS::Media::PixelMap::Create(*oriSnapshot, options);
                snapshot.snapshot = std::shared_ptr<OHOS::Media::PixelMap>(reducedPixelMap.release());
            } else {
                snapshot.snapshot = oriSnapshot;
            }
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshot");
}

int SceneSessionManager::GetRemoteSessionSnapshotInfo(const std::string& deviceId, int32_t sessionId,
                                                      AAFwk::MissionSnapshot& sessionSnapshot)
{
    WLOGFI("GetRemoteSessionSnapshotInfo begin");
    std::unique_ptr<AAFwk::MissionSnapshot> sessionSnapshotPtr = std::make_unique<AAFwk::MissionSnapshot>();
    DistributedClient dmsClient;
    int result = dmsClient.GetRemoteMissionSnapshotInfo(deviceId, sessionId, sessionSnapshotPtr);
    if (result != ERR_OK) {
        WLOGFE("GetRemoteMissionSnapshotInfo failed, result = %{public}d", result);
        return result;
    }
    sessionSnapshot = *sessionSnapshotPtr;
    return ERR_OK;
}

WSError SceneSessionManager::RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener)
{
    WLOGFI("run RegisterSessionListener");
    if (sessionListener == nullptr) {
        return WSError::WS_ERROR_INVALID_SESSION_LISTENER;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionListener]() {
        sessionListener_ = sessionListener;
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "RegisterSessionListener");
}

void SceneSessionManager::UnregisterSessionListener()
{
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return;
    }
    auto task = [this]() {
        sessionListener_ = nullptr;
    };
    taskScheduler_->PostAsyncTask(task, "UnregisterSessionListener");
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
        TLOGI(WmsLogTag::WMS_MAIN, "RequestSceneSessionByCall persistentId: %{public}d", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto sessionInfo = scnSession->GetSessionInfo();
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
        TLOGI(WmsLogTag::WMS_MAIN, "RequestSceneSessionByCall callState:%{public}d, persistentId: %{public}d",
            sessionInfo.callState_, persistentId);
        AAFwk::AbilityManagerClient::GetInstance()->CallUIAbilityBySCB(abilitySessionInfo);
        scnSession->RemoveLifeCycleTask(LifeCycleTaskType::START);
        return WSError::WS_OK;
    };
    std::string taskName = "RequestSceneSessionByCall:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::StartAbilityBySpecified(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        WLOGFI("StartAbilityBySpecified: bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str());
        AAFwk::Want want;
        want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
        if (sessionInfo.want != nullptr) {
            want.SetParams(sessionInfo.want->GetParams());
        }
        AAFwk::AbilityManagerClient::GetInstance()->StartSpecifiedAbilityBySCB(want);
    };

    taskScheduler_->PostAsyncTask(task, "StartAbilityBySpecified:PID:" + sessionInfo.bundleName_);
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
            if (pair.second->IsTerminated()) {
                return false;
            }
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

WSError SceneSessionManager::BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_DIALOG, "BindDialogSessionTarget permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (targetToken == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Target token is null");
        return WSError::WS_ERROR_NULLPTR;
    }

    auto task = [this, persistentId, targetToken]() {
        auto scnSession = GetSceneSession(static_cast<int32_t>(persistentId));
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Session is nullptr, persistentId:%{public}" PRIu64, persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (scnSession->GetWindowType() != WindowType::WINDOW_TYPE_DIALOG) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Session is not dialog, type:%{public}u", scnSession->GetWindowType());
            return WSError::WS_OK;
        }
        scnSession->dialogTargetToken_ = targetToken;
        sptr<SceneSession> parentSession = FindMainWindowWithToken(targetToken);
        if (parentSession == nullptr) {
            scnSession->NotifyDestroy();
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        scnSession->SetParentSession(parentSession);
        scnSession->SetParentPersistentId(parentSession->GetPersistentId());
        UpdateParentSessionForDialog(scnSession, scnSession->GetSessionProperty());
        TLOGI(WmsLogTag::WMS_DIALOG, "Bind dialog success, dialog id %{public}" PRIu64 ", parentId %{public}d",
            persistentId, parentSession->GetPersistentId());
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "BindDialogTarget:PID:" + std::to_string(persistentId));
}

void DisplayChangeListener::OnGetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds)
{
    SceneSessionManager::GetInstance().GetSurfaceNodeIdsFromMissionIds(missionIds, surfaceNodeIds);
}

WMError SceneSessionManager::GetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds)
{
    auto isSaCall = SessionPermission::IsSACalling();
    if (!isSaCall) {
        WLOGFE("The interface only support for sa call");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &missionIds, &surfaceNodeIds]() {
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto missionId : missionIds) {
            iter = sceneSessionMap_.find(static_cast<int32_t>(missionId));
            if (iter == sceneSessionMap_.end()) {
                continue;
            }
            auto sceneSession = iter->second;
            if (sceneSession == nullptr) {
                continue;
            }
            if (sceneSession->GetSurfaceNode() == nullptr) {
                continue;
            }
            surfaceNodeIds.push_back(sceneSession->GetSurfaceNode()->GetId());
            if (sceneSession->GetLeashWinSurfaceNode()) {
                surfaceNodeIds.push_back(sceneSession->GetLeashWinSurfaceNode()->GetId());
            }
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetSurfaceNodeIdsFromMissionIds");
}

WMError SceneSessionManager::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG) {
        if (!SessionPermission::IsSystemCalling()) {
            WLOGFE("RegisterWindowManagerAgent permission denied!");
            return WMError::WM_ERROR_NOT_SYSTEM_APP;
        }
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto task = [this, &windowManagerAgent, type]() {
        return SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(windowManagerAgent, type);
    };
    return taskScheduler_->PostSyncTask(task, "RegisterWindowManagerAgent");
}

WMError SceneSessionManager::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG) {
        if (!SessionPermission::IsSystemCalling()) {
            WLOGFE("UnregisterWindowManagerAgent permission denied!");
            return WMError::WM_ERROR_NOT_SYSTEM_APP;
        }
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto task = [this, &windowManagerAgent, type]() {
        return SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(windowManagerAgent, type);
    };
    return taskScheduler_->PostSyncTask(task, "UnregisterWindowManagerAgent");
}

void SceneSessionManager::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SessionManagerAgentController::GetInstance().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}

void SceneSessionManager::UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SessionManagerAgentController::GetInstance().UpdateCameraWindowStatus(accessTokenId, isShowing);
}

void SceneSessionManager::StartWindowInfoReportLoop()
{
    WLOGFD("Report loop");
    if (eventHandler_ == nullptr) {
        WLOGFE("Report event null");
        return ;
    }
    if (isReportTaskStart_) {
        WLOGFE("Report is ReportTask Start");
        return;
    }
    auto task = [this]() {
        WindowInfoReporter::GetInstance().ReportRecordedInfos();
        ReportWindowProfileInfos();
        isReportTaskStart_ = false;
        StartWindowInfoReportLoop();
    };
    int64_t delayTime = 1000 * 60 * 60; // an hour.
    bool ret = eventHandler_->PostTask(task, "wms:WindowInfoReport", delayTime);
    if (!ret) {
        WLOGFE("Report post listener callback task failed. the task name is WindowInfoReport");
        return;
    }
    isReportTaskStart_ = true;
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
    WLOGFD("GetAccessibilityWindowInfo Called.");
    if (!SessionPermission::IsSystemServiceCalling()) {
        WLOGFE("The interface only support for system service.");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, &infos]() {
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
            sptr<SceneSession> sceneSession = iter->second;
            if (sceneSession == nullptr) {
                WLOGFW("null scene session");
                continue;
            }
            WLOGFD("name = %{public}s, isSystem = %{public}d, persistentId = %{public}d, winType = %{public}d, "
                "state = %{public}d, visible = %{public}d", sceneSession->GetWindowName().c_str(),
                sceneSession->GetSessionInfo().isSystem_, iter->first, sceneSession->GetWindowType(),
                sceneSession->GetSessionState(), sceneSession->IsVisibleForAccessibility());
            if (sceneSession->IsVisibleForAccessibility()) {
                FillWindowInfo(infos, iter->second);
            }
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetAccessibilityWindowInfo");
}

void SceneSessionManager::NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type)
{
    WLOGFD("NotifyWindowInfoChange, persistentId = %{public}d, updateType = %{public}d", persistentId, type);
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("NotifyWindowInfoChange sceneSession nullptr!");
        return;
    }
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, type]() {
        auto scnSession = weakSceneSession.promote();
        NotifyAllAccessibilityInfo();
        if (WindowChangedFunc_ != nullptr && scnSession != nullptr &&
            scnSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            WindowChangedFunc_(scnSession->GetPersistentId(), type);
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyWindowInfoChange:PID:" + std::to_string(persistentId));

    auto notifySceneInputTask = [weakSceneSession, type]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            return;
        }
        SceneInputManager::GetInstance().NotifyWindowInfoChange(scnSession, type);
    };
    taskScheduler_->PostAsyncTask(notifySceneInputTask);
}

bool SceneSessionManager::FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("null scene session.");
        return false;
    }
    if (sceneSession->GetSessionInfo().bundleName_.find("SCBGestureBack") != std::string::npos
        || sceneSession->GetSessionInfo().bundleName_.find("SCBGestureNavBar") != std::string::npos) {
        WLOGFD("filter gesture window.");
        return false;
    }
    sptr<AccessibilityWindowInfo> info = new (std::nothrow) AccessibilityWindowInfo();
    if (info == nullptr) {
        WLOGFE("null info.");
        return false;
    }
    if (sceneSession->GetSessionInfo().isSystem_) {
        info->wid_ = 1;
        info->innerWid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    } else {
        info->wid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    }
    info->uiNodeId_ = sceneSession->GetUINodeId();
    WSRect wsrect = sceneSession->GetSessionRect();
    info->windowRect_ = {wsrect.posX_, wsrect.posY_, wsrect.width_, wsrect.height_ };
    info->focused_ = sceneSession->GetPersistentId() == focusedSessionId_;
    info->type_ = sceneSession->GetWindowType();
    info->mode_ = sceneSession->GetWindowMode();
    info->layer_ = sceneSession->GetZOrder();
    info->scaleVal_ = sceneSession->GetFloatingScale();
    info->scaleX_ = sceneSession->GetScaleX();
    info->scaleY_ = sceneSession->GetScaleY();
    info->bundleName_ = sceneSession->GetSessionInfo().bundleName_;
    info->touchHotAreas_ = sceneSession->GetTouchHotAreas();
    auto property = sceneSession->GetSessionProperty();
    if (property != nullptr) {
        info->displayId_ = property->GetDisplayId();
        info->isDecorEnable_ = property->IsDecorEnable();
    }
    infos.emplace_back(info);
    TLOGD(WmsLogTag::WMS_MAIN, "wid = %{public}d, inWid = %{public}d, uiNId = %{public}d, bundleName = %{public}s",
        info->wid_, info->innerWid_, info->uiNodeId_, info->bundleName_.c_str());
    return true;
}

std::string SceneSessionManager::GetSessionSnapshotFilePath(int32_t persistentId)
{
    WLOGFI("GetSessionSnapshotFilePath persistentId %{public}d", persistentId);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("GetSessionSnapshotFilePath sceneSession nullptr!");
        return "";
    }
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return std::string("");
        }
        std::string filePath = scnSession->GetSessionSnapshotFilePath();
        return filePath;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshotFilePath" + std::to_string(persistentId));
}

sptr<SceneSession> SceneSessionManager::SelectSesssionFromMap(const uint64_t& surfaceId)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (sceneSession->GetSurfaceNode() == nullptr) {
            continue;
        }
        if (surfaceId == sceneSession->GetSurfaceNode()->GetId()) {
            return sceneSession;
        }
    }
    return nullptr;
}

void SceneSessionManager::WindowLayerInfoChangeCallback(std::shared_ptr<RSOcclusionData> occlusiontionData)
{
    WLOGFD("WindowLayerInfoChangeCallback: entry");
    std::weak_ptr<RSOcclusionData> weak(occlusiontionData);

    auto task = [this, weak]() {
        auto weakOcclusionData = weak.lock();
        if (weakOcclusionData == nullptr) {
            WLOGFE("weak occlusionData is nullptr");
            return;
        }
        std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
        std::vector<std::pair<uint64_t, bool>> currDrawingContentData;
        GetWindowLayerChangeInfo(weakOcclusionData, currVisibleData, currDrawingContentData);
        std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
        if (currVisibleData.size() != 0) {
            visibilityChangeInfos = GetWindowVisibilityChangeInfo(currVisibleData);
        }
        if (visibilityChangeInfos.size() != 0) {
            DealwithVisibilityChange(visibilityChangeInfos);
            CacVisibleWindowNum();
        }

        std::vector<std::pair<uint64_t, bool>> drawingContentChangeInfos;
        if (currDrawingContentData.size() != 0) {
            drawingContentChangeInfos = GetWindowDrawingContentChangeInfo(currDrawingContentData);
        }
        if (drawingContentChangeInfos.size() != 0) {
            DealwithDrawingContentChange(drawingContentChangeInfos);
        }
    };
    taskScheduler_->PostVoidSyncTask(task, "WindowLayerInfoChangeCallback");
}

void SceneSessionManager::GetWindowLayerChangeInfo(std::shared_ptr<RSOcclusionData> occlusiontionData,
    std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData,
    std::vector<std::pair<uint64_t, bool>>& currDrawingContentData)
{
    VisibleData& rsVisibleData = occlusiontionData->GetVisibleData();
    for (auto iter = rsVisibleData.begin(); iter != rsVisibleData.end(); iter++) {
        WindowLayerState windowLayerState = static_cast<WindowLayerState>(iter->second);
        switch (windowLayerState) {
            case WINDOW_ALL_VISIBLE:
            case WINDOW_SEMI_VISIBLE:
            case WINDOW_IN_VISIBLE:
                currVisibleData.emplace_back(iter->first, static_cast<WindowVisibilityState>(iter->second));
                break;
            case WINDOW_LAYER_DRAWING:
                currDrawingContentData.emplace_back(iter->first, true);
                break;
            case WINDOW_LAYER_NO_DRAWING:
                currDrawingContentData.emplace_back(iter->first, false);
                break;
            default:
                break;
        }
    }
}

std::vector<std::pair<uint64_t, WindowVisibilityState>> SceneSessionManager::GetWindowVisibilityChangeInfo(
    std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfo;
    std::sort(currVisibleData.begin(), currVisibleData.end(), Comp);
    uint32_t i, j;
    i = j = 0;
    for (; i < lastVisibleData_.size() && j < currVisibleData.size();) {
        if (lastVisibleData_[i].first < currVisibleData[j].first) {
            visibilityChangeInfo.emplace_back(lastVisibleData_[i].first, WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
            i++;
        } else if (lastVisibleData_[i].first > currVisibleData[j].first) {
            visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
            j++;
        } else {
            if (lastVisibleData_[i].second != currVisibleData[j].second) {
                visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
            }
            i++;
            j++;
        }
    }
    for (; i < lastVisibleData_.size(); ++i) {
        visibilityChangeInfo.emplace_back(lastVisibleData_[i].first, WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
    }
    for (; j < currVisibleData.size(); ++j) {
        visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
    }
    lastVisibleData_ = currVisibleData;
    return visibilityChangeInfo;
}

void SceneSessionManager::DealwithVisibilityChange(const std::vector<std::pair<uint64_t, WindowVisibilityState>>&
    visibilityChangeInfo)
{
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
    std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif

    std::string visibilityInfo = "WindowVisibilityInfos [name, winId, visibleState]: ";
    for (const auto& elem : visibilityChangeInfo) {
        uint64_t surfaceId = elem.first;
        WindowVisibilityState visibleState = elem.second;
        bool isVisible = visibleState < WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
        sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
        if (session == nullptr) {
            continue;
        }
        if ((WindowHelper::IsSubWindow(session->GetWindowType()) ||
            session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) && isVisible == true) {
            if (session->GetParentSession() != nullptr &&
                !session->GetParentSession()->IsSessionForeground()) {
                    continue;
                }
        }
        session->SetVisible(isVisible);
        session->SetVisibilityState(visibleState);
        int32_t windowId = session->GetWindowId();
        if (windowVisibilityListenerSessionSet_.find(windowId) != windowVisibilityListenerSessionSet_.end()) {
            session->NotifyWindowVisibility();
        }
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(windowId, session->GetCallingPid(),
            session->GetCallingUid(), visibleState, session->GetWindowType()));
#ifdef MEMMGR_WINDOW_ENABLE
    memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(session->GetWindowId(), session->GetCallingPid(),
            session->GetCallingUid(), isVisible));
#endif
        visibilityInfo += "[" + session->GetWindowName() + ", " + std::to_string(windowId) + ", " +
            std::to_string(visibleState) + "], ";
        CheckAndNotifyWaterMarkChangedResult();
    }
    if (windowVisibilityInfos.size() != 0) {
        WLOGI("Notify windowvisibilityinfo changed start, size: %{public}zu, %{public}s", windowVisibilityInfos.size(),
            visibilityInfo.c_str());
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
#ifdef MEMMGR_WINDOW_ENABLE
    if (memMgrWindowInfos.size() != 0) {
        WLOGD("Notify memMgrWindowInfos changed start");
        Memory::MemMgrClient::GetInstance().OnWindowVisibilityChanged(memMgrWindowInfos);
    }
#endif
}

void SceneSessionManager::DealwithDrawingContentChange(const std::vector<std::pair<uint64_t, bool>>&
    drawingContentChangeInfo)
{
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContenInfos;
    for (const auto& elem : drawingContentChangeInfo) {
        uint64_t surfaceId = elem.first;
        bool drawingState = elem.second;
        sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
        if (session == nullptr) {
            continue;
        }
        windowDrawingContenInfos.emplace_back(new WindowDrawingContentInfo(session->GetWindowId(),
            session->GetCallingPid(), session->GetCallingUid(), drawingState, session->GetWindowType()));
        if (openDebugTrace) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Drawing status changed pid:(%d ) surfaceId:(%" PRIu64 ")"
                "drawingState:(%d )", session->GetCallingPid(), surfaceId, drawingState);
        }
        WLOGFD("NotifyWindowDrawingContenInfoChange: drawing status changed pid:%{public}d,"
            "surfaceId:%{public}" PRIu64", drawingState:%{public}d", session->GetCallingPid(), surfaceId, drawingState);
    }
    if (windowDrawingContenInfos.size() != 0) {
        WLOGFD("Notify WindowDrawingContenInfo changed start");
        SessionManagerAgentController::GetInstance().UpdateWindowDrawingContentInfo(windowDrawingContenInfos);
    }
}

std::vector<std::pair<uint64_t, bool>> SceneSessionManager::GetWindowDrawingContentChangeInfo(
    std::vector<std::pair<uint64_t, bool>> currDrawingContentData)
{
    std::vector<std::pair<uint64_t, bool>> processDrawingContentChangeInfo;
    for (const auto& data : currDrawingContentData) {
        uint64_t windowId = data.first;
        bool currentDrawingContentState = data.second;
        int32_t pid = 0;
        bool isChange = false;
        if (GetPreWindowDrawingState(windowId, pid, currentDrawingContentState) == currentDrawingContentState) {
            continue;
        } else {
            isChange = GetProcessDrawingState(windowId, pid, currentDrawingContentState);
        }

        if (isChange) {
            processDrawingContentChangeInfo.emplace_back(windowId, currentDrawingContentState);
        }
    }
    return processDrawingContentChangeInfo;
}

bool SceneSessionManager::GetPreWindowDrawingState(uint64_t windowId, int32_t& pid, bool currentDrawingContentState)
{
    bool preWindowDrawingState = true;
    sptr<SceneSession> session = SelectSesssionFromMap(windowId);
    if (session == nullptr) {
        return false;
    }
    pid = session->GetCallingPid();
    preWindowDrawingState = session->GetDrawingContentState();
    session->SetDrawingContentState(currentDrawingContentState);
    return preWindowDrawingState;
}

bool SceneSessionManager::GetProcessDrawingState(uint64_t windowId, int32_t pid, bool currentDrawingContentState)
{
    bool isChange = true;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (sceneSession->GetCallingPid() == pid && sceneSession->GetSurfaceNode() != nullptr &&
            windowId != sceneSession->GetSurfaceNode()->GetId()) {
                if (sceneSession->GetDrawingContentState()) {
                    return false;
                }
            }
        }
    return isChange;
}


void SceneSessionManager::InitWithRenderServiceAdded()
{
    auto windowVisibilityChangeCb = std::bind(&SceneSessionManager::WindowLayerInfoChangeCallback, this,
        std::placeholders::_1);
    WLOGI("RegisterWindowVisibilityChangeCallback");
    if (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
        WLOGFE("RegisterWindowVisibilityChangeCallback failed");
    }
}

WMError SceneSessionManager::SetSystemAnimatedScenes(SystemAnimatedSceneType sceneType)
{
    if (sceneType > SystemAnimatedSceneType::SCENE_OTHERS) {
        WLOGFE("The input scene type is valid, scene type is %{public}d", sceneType);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    auto task = [this, sceneType]() {
        WLOGFD("Set system animated scene %{public}d.", sceneType);
        bool ret = rsInterface_.SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(sceneType));
        if (!ret) {
            WLOGFE("Set system animated scene failed.");
        }
    };
    taskScheduler_->PostAsyncTask(task, "SetSystemAnimatedScenes");
    return WMError::WM_OK;
}

WSError SceneSessionManager::NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("NotifyWindowExtensionVisibilityChange permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("Notify WindowExtension visibility change to %{public}s for pid: %{public}d, uid: %{public}d",
        visible ? "VISIBLE" : "INVISIBLE", pid, uid);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(INVALID_WINDOW_ID, pid, uid,
        visible ? WINDOW_VISIBILITY_STATE_NO_OCCLUSION : WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION,
        WindowType::WINDOW_TYPE_APP_COMPONENT));
    SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    return WSError::WS_OK;
}

void SceneSessionManager::WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr!");
        return;
    }
    if (sceneSession->GetVisible()) {
        std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
        std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif
        sceneSession->SetVisible(false);
        sceneSession->SetVisibilityState(WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
        sceneSession->ClearExtWindowFlags();
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(sceneSession->GetWindowId(),
            sceneSession->GetCallingPid(), sceneSession->GetCallingUid(),
            WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION, sceneSession->GetWindowType()));
#ifdef MEMMGR_WINDOW_ENABLE
        memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(sceneSession->GetWindowId(),
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid(), false));
#endif
        WLOGFD("NotifyWindowVisibilityChange: covered status changed window:%{public}u, isVisible:%{public}d",
            sceneSession->GetWindowId(), sceneSession->GetVisible());
        CheckAndNotifyWaterMarkChangedResult();
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
#ifdef MEMMGR_WINDOW_ENABLE
        WLOGD("Notify memMgrWindowInfos changed start");
        Memory::MemMgrClient::GetInstance().OnWindowVisibilityChanged(memMgrWindowInfos);
#endif
    }
}

sptr<SceneSession> SceneSessionManager::FindSessionByToken(const sptr<IRemoteObject> &token)
{
    sptr<SceneSession> session = nullptr;
    auto cmpFunc = [token](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
        if (pair.second == nullptr) {
            return false;
        }
        return pair.second->GetAbilityToken() == token || pair.second->GetSelfToken() == token;
    };
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(), cmpFunc);
    if (iter != sceneSessionMap_.end()) {
        session = iter->second;
    }
    return session;
}

sptr<SceneSession> SceneSessionManager::FindSessionByAffinity(std::string affinity)
{
    if (affinity.size() == 0) {
        WLOGFI("AbilityInfo affinity is empty");
        return nullptr;
    }
    sptr<SceneSession> session = nullptr;
    auto cmpFunc = [this, affinity](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
        if (pair.second == nullptr || !CheckCollaboratorType(pair.second->GetCollaboratorType())) {
            return false;
        }
        return pair.second->GetSessionInfo().sessionAffinity == affinity;
    };
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(), cmpFunc);
    if (iter != sceneSessionMap_.end()) {
        session = iter->second;
    }
    return session;
}

void SceneSessionManager::PreloadInLakeApp(const std::string& bundleName)
{
    WLOGFI("run PreloadInLakeApp");
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(CollaboratorType::RESERVE_TYPE);
        if (iter == collaboratorMap_.end()) {
            WLOGFE("Fail to found collaborator with type: RESERVE_TYPE");
            return;
        }
        collaborator = iter->second;
    }
    if (collaborator != nullptr) {
        collaborator->NotifyPreloadAbility(bundleName);
    }
}

WSError SceneSessionManager::PendingSessionToForeground(const sptr<IRemoteObject> &token)
{
    TLOGI(WmsLogTag::WMS_LIFE, "run PendingSessionToForeground");
    auto task = [this, &token]() {
        auto session = FindSessionByToken(token);
        if (session != nullptr) {
            return session->PendingSessionToForeground();
        }
        TLOGE(WmsLogTag::WMS_LIFE, "PendingSessionToForeground:fail to find token");
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "PendingSessionToForeground");
}

WSError SceneSessionManager::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token)
{
    auto task = [this, &token]() {
        auto session = FindSessionByToken(token);
        if (session != nullptr) {
            return session->PendingSessionToBackgroundForDelegator();
        }
        TLOGE(WmsLogTag::WMS_LIFE, "PendingSessionToBackgroundForDelegator:fail to find token");
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "PendingSessionToBackgroundForDelegator");
}

WSError SceneSessionManager::GetFocusSessionToken(sptr<IRemoteObject>& token)
{
    if (!SessionPermission::IsSACalling()) {
        WLOGFE("GetFocusSessionToken permission denied!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &token]() {
        WLOGFD("run GetFocusSessionToken with focusedSessionId: %{public}d", focusedSessionId_);
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            token = sceneSession->GetAbilityToken();
            if (token == nullptr) {
                WLOGFE("token is nullptr");
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            return WSError::WS_OK;
        }
        return WSError::WS_ERROR_INVALID_SESSION;
    };
    return taskScheduler_->PostSyncTask(task, "GetFocusSessionToken");
}

WSError SceneSessionManager::GetFocusSessionElement(AppExecFwk::ElementName& element)
{
    auto task = [this, &element]() {
        WLOGFD("run GetFocusSessionElement with focusedSessionId: %{public}d", focusedSessionId_);
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            auto sessionInfo = sceneSession->GetSessionInfo();
            element = AppExecFwk::ElementName("", sessionInfo.bundleName_,
                sessionInfo.abilityName_, sessionInfo.moduleName_);
            return WSError::WS_OK;
        }
        return WSError::WS_ERROR_INVALID_SESSION;
    };
    return taskScheduler_->PostSyncTask(task, "GetFocusSessionElement");
}

WSError SceneSessionManager::UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener)
{
    auto task = [this, persistentId, haveListener]() {
        TLOGI(WmsLogTag::WMS_IMMS,
            "UpdateSessionAvoidAreaListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGD(WmsLogTag::WMS_IMMS, "sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
        }
        if (haveListener) {
            avoidAreaListenerSessionSet_.insert(persistentId);
            UpdateAvoidArea(persistentId);
        } else {
            lastUpdatedAvoidArea_.erase(persistentId);
            avoidAreaListenerSessionSet_.erase(persistentId);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UpdateSessionAvoidAreaListener:PID:" + std::to_string(persistentId));
}

bool SceneSessionManager::UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
    const sptr<SceneSession>& sceneSession, const AvoidArea& avoidArea, AvoidAreaType avoidAreaType)
{
    if (sceneSession == nullptr) {
        return false;
    }
    auto iter = lastUpdatedAvoidArea_.find(persistentId);
    bool needUpdate = true;

    if (iter != lastUpdatedAvoidArea_.end()) {
        auto avoidAreaIter = iter->second.find(avoidAreaType);
        if (avoidAreaIter != iter->second.end()) {
            needUpdate = avoidAreaIter->second != avoidArea;
        } else {
            if (avoidArea.isEmptyAvoidArea()) {
                needUpdate = false;
                return needUpdate;
            }
        }
    } else {
        if (avoidArea.isEmptyAvoidArea()) {
            needUpdate = false;
            return needUpdate;
        }
    }
    if (needUpdate ||
        avoidAreaType == AvoidAreaType::TYPE_SYSTEM || avoidAreaType == AvoidAreaType::TYPE_NAVIGATION_INDICATOR) {
        lastUpdatedAvoidArea_[persistentId][avoidAreaType] = avoidArea;
        sceneSession->UpdateAvoidArea(new AvoidArea(avoidArea), avoidAreaType);
    }

    return needUpdate;
}

void SceneSessionManager::UpdateAvoidSessionAvoidArea(WindowType type, bool& needUpdate)
{
    bool ret = true;
    AvoidAreaType avoidType = (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) ?
        AvoidAreaType::TYPE_KEYBOARD : AvoidAreaType::TYPE_SYSTEM;
    for (auto& persistentId : avoidAreaListenerSessionSet_) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr || !IsSessionVisible(sceneSession)) {
            continue;
        }
        AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
        ret = UpdateSessionAvoidAreaIfNeed(
            persistentId, sceneSession, avoidArea, static_cast<AvoidAreaType>(avoidType));
        needUpdate = needUpdate || ret;
    }

    return;
}

static bool CheckAvoidAreaForAINavigationBar(bool isVisible, const AvoidArea& avoidArea, int32_t sessionBottom)
{
    if (!avoidArea.topRect_.IsUninitializedRect() || !avoidArea.leftRect_.IsUninitializedRect() ||
        !avoidArea.rightRect_.IsUninitializedRect()) {
        return false;
    }
    if (avoidArea.bottomRect_.IsUninitializedRect()) {
        return true;
    }
    if (isVisible &&
        (avoidArea.bottomRect_.posY_ + static_cast<int32_t>(avoidArea.bottomRect_.height_) == sessionBottom)) {
        return true;
    }
    return false;
}

void SceneSessionManager::UpdateNormalSessionAvoidArea(
    const int32_t& persistentId, sptr<SceneSession>& sceneSession, bool& needUpdate)
{
    bool ret = true;
    if (sceneSession == nullptr || !IsSessionVisible(sceneSession)) {
        needUpdate = false;
        return;
    }
    if (avoidAreaListenerSessionSet_.find(persistentId) == avoidAreaListenerSessionSet_.end()) {
        TLOGD(WmsLogTag::WMS_IMMS,
            "id:%{public}d is not in avoidAreaListenerNodes, don't update avoid area.", persistentId);
        needUpdate = false;
        return;
    }
    uint32_t start = static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM);
    uint32_t end = static_cast<uint32_t>(AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    for (uint32_t avoidType = start; avoidType <= end; avoidType++) {
        AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
        if (avoidType == static_cast<uint32_t>(AvoidAreaType::TYPE_NAVIGATION_INDICATOR) &&
            !CheckAvoidAreaForAINavigationBar(isAINavigationBarVisible_, avoidArea,
                sceneSession->GetSessionRect().height_)) {
            continue;
        }
        ret = UpdateSessionAvoidAreaIfNeed(
            persistentId, sceneSession, avoidArea, static_cast<AvoidAreaType>(avoidType));
        needUpdate = needUpdate || ret;
    }

    return;
}

void SceneSessionManager::NotifyMMIWindowPidChange(int32_t windowId, bool startMoving)
{
    int32_t pid = startMoving ? static_cast<int32_t>(getpid()) : -1;
    auto sceneSession = GetSceneSession(windowId);
    if (sceneSession == nullptr) {
        WLOGFW("window not exist: %{public}d", windowId);
        return;
    }

    wptr<SceneSession> weakSceneSession(sceneSession);
    WLOGFI("SceneSessionManager NotifyMMIWindowPidChange to notify window: %{public}d, pid: %{public}d", windowId, pid);
    auto task = [weakSceneSession, startMoving]() -> WSError {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFW("session is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        SceneInputManager::GetInstance().NotifyMMIWindowPidChange(scnSession, startMoving);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostAsyncTask(task);
}

void SceneSessionManager::UpdateAvoidArea(const int32_t& persistentId)
{
    auto task = [this, persistentId]() {
        bool needUpdate = false;
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGD(WmsLogTag::WMS_IMMS, "sceneSession is nullptr.");
            return;
        }
        NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_BOUNDS);

        WindowType type = sceneSession->GetWindowType();
        SessionGravity gravity = sceneSession->GetKeyboardGravity();
        if (type == WindowType::WINDOW_TYPE_STATUS_BAR ||
            type == WindowType::WINDOW_TYPE_NAVIGATION_BAR ||
            (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
            (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM ||
            gravity == SessionGravity::SESSION_GRAVITY_DEFAULT))) {
            UpdateAvoidSessionAvoidArea(type, needUpdate);
        } else {
            UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
        }
        return;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateAvoidArea:PID:" + std::to_string(persistentId));
    return;
}

WSError SceneSessionManager::NotifyAINavigationBarShowStatus(bool isVisible, WSRect barArea, uint64_t displayId)
{
    WLOGFI("NotifyAINavigationBarShowStatus: isVisible: %{public}u, " \
        "area{%{public}d,%{public}d,%{public}d,%{public}d}, displayId: %{public}" PRIu64"",
        isVisible, barArea.posX_, barArea.posY_, barArea.width_, barArea.height_, displayId);
    auto task = [this, isVisible, barArea, displayId]() {
        bool isNeedUpdate = false;
        {
            std::unique_lock<std::shared_mutex> lock(currAINavigationBarAreaMapMutex_);
            isNeedUpdate = isAINavigationBarVisible_ != isVisible ||
                currAINavigationBarAreaMap_.count(displayId) == 0 ||
                currAINavigationBarAreaMap_[displayId] != barArea;
            if (isNeedUpdate) {
                isAINavigationBarVisible_ = isVisible;
                currAINavigationBarAreaMap_.clear();
                currAINavigationBarAreaMap_[displayId] = barArea;
            }
            if (isNeedUpdate && !isVisible && !barArea.IsEmpty()) {
                WLOGFD("NotifyAINavigationBarShowStatus: barArea should be empty if invisible");
                currAINavigationBarAreaMap_[displayId] = WSRect();
            }
        }
        if (isNeedUpdate) {
            WLOGFI("NotifyAINavigationBarShowStatus: enter: %{public}u, {%{public}d,%{public}d,%{public}d,%{public}d}",
                isVisible, barArea.posX_, barArea.posY_, barArea.width_, barArea.height_);
            for (auto persistentId : avoidAreaListenerSessionSet_) {
                NotifySessionAINavigationBarChange(persistentId);
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyAINavigationBarShowStatus");
    return WSError::WS_OK;
}

void SceneSessionManager::NotifySessionAINavigationBarChange(int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr || !IsSessionVisible(sceneSession)) {
        return;
    }
    AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    if (!CheckAvoidAreaForAINavigationBar(isAINavigationBarVisible_, avoidArea,
        sceneSession->GetSessionRect().height_)) {
        return;
    }
    WLOGFI("NotifyAINavigationBarShowStatus: persistentId: %{public}d, "
        "{%{public}d,%{public}d,%{public}d,%{public}d}", persistentId,
        avoidArea.bottomRect_.posX_, avoidArea.bottomRect_.posY_,
        avoidArea.bottomRect_.width_, avoidArea.bottomRect_.height_);
    UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea,
        AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
}

WSRect SceneSessionManager::GetAINavigationBarArea(uint64_t displayId)
{
    std::shared_lock<std::shared_mutex> lock(currAINavigationBarAreaMapMutex_);
    if (currAINavigationBarAreaMap_.count(displayId) == 0) {
        return {};
    }
    return currAINavigationBarAreaMap_[displayId];
}

WSError SceneSessionManager::UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener)
{
    auto task = [this, persistentId, haveListener]() {
        WLOGFI("UpdateSessionTouchOutsideListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
        }
        if (haveListener) {
            touchOutsideListenerSessionSet_.insert(persistentId);
        } else {
            touchOutsideListenerSessionSet_.erase(persistentId);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UpdateSessionTouchOutsideListener" + std::to_string(persistentId));
}

WSError SceneSessionManager::UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener)
{
    auto task = [this, persistentId, haveListener]() -> WSError {
        WLOGFI("UpdateSessionWindowVisibilityListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
        }
        if (haveListener) {
            windowVisibilityListenerSessionSet_.insert(persistentId);
            sceneSession->NotifyWindowVisibility();
        } else {
            windowVisibilityListenerSessionSet_.erase(persistentId);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UpdateSessionWindowVisibilityListener");
}

void SceneSessionManager::SetVirtualPixelRatioChangeListener(const ProcessVirtualPixelRatioChangeFunc& func)
{
    processVirtualPixelRatioChangeFunc_ = func;
    WLOGFI("SetVirtualPixelRatioChangeListener");
}

void SceneSessionManager::ProcessVirtualPixelRatioChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayInfo == nullptr) {
        WLOGFE("SceneSessionManager::ProcessVirtualPixelRatioChange displayInfo is nullptr.");
        return;
    }
    auto task = [this, displayInfo]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        if (processVirtualPixelRatioChangeFunc_ != nullptr &&
            displayInfo->GetVirtualPixelRatio() == displayInfo->GetDensityInCurResolution()) {
            Rect rect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
                displayInfo->GetWidth(), displayInfo->GetHeight()
            };
            processVirtualPixelRatioChangeFunc_(displayInfo->GetVirtualPixelRatio(), rect);
        }
        for (const auto &item : sceneSessionMap_) {
            auto scnSession = item.second;
            if (scnSession == nullptr) {
                WLOGFE("SceneSessionManager::ProcessVirtualPixelRatioChange null scene session");
                continue;
            }
            SessionInfo sessionInfo = scnSession->GetSessionInfo();
            if (sessionInfo.isSystem_) {
                continue;
            }
            if (scnSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                scnSession->GetSessionState() == SessionState::STATE_ACTIVE) {
                scnSession->UpdateDensity();
                WLOGFD("UpdateDensity name=%{public}s, persistentId=%{public}d, winType=%{public}d, "
                    "state=%{public}d, visible-%{public}d", scnSession->GetWindowName().c_str(), item.first,
                    scnSession->GetWindowType(), scnSession->GetSessionState(), scnSession->IsVisible());
            }
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "ProcessVirtualPixelRatioChange:DID:" + std::to_string(defaultDisplayId));
}

void SceneSessionManager::ProcessUpdateRotationChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayInfo == nullptr) {
        WLOGFE("SceneSessionManager::ProcessUpdateRotationChange displayInfo is nullptr.");
        return;
    }
    auto task = [this, displayInfo]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto scnSession = item.second;
            if (scnSession == nullptr) {
                WLOGFE("SceneSessionManager::ProcessUpdateRotationChange null scene session");
                continue;
            }
            if (scnSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                scnSession->GetSessionState() == SessionState::STATE_ACTIVE) {
                scnSession->UpdateRotationAvoidArea();
                WLOGFD("UpdateRotationAvoidArea name=%{public}s, persistentId=%{public}d, winType=%{public}d, "
                    "state=%{public}d, visible-%{public}d", scnSession->GetWindowName().c_str(), item.first,
                    scnSession->GetWindowType(), scnSession->GetSessionState(), scnSession->IsVisible());
            }
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "ProcessUpdateRotationChange" + std::to_string(defaultDisplayId));
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WLOGFD("DisplayChangeListener::OnDisplayStateChange: %{public}u", type);
    switch (type) {
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            SceneSessionManager::GetInstance().ProcessVirtualPixelRatioChange(defaultDisplayId,
                displayInfo, displayInfoMap, type);
            break;
        }
        case DisplayStateChangeType::UPDATE_ROTATION: {
            SceneSessionManager::GetInstance().ProcessUpdateRotationChange(defaultDisplayId,
                displayInfo, displayInfoMap, type);
            break;
        }
        default:
            return;
    }
}

void DisplayChangeListener::OnScreenshot(DisplayId displayId)
{
    SceneSessionManager::GetInstance().OnScreenshot(displayId);
}

void SceneSessionManager::OnScreenshot(DisplayId displayId)
{
    auto task = [this, displayId]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter : sceneSessionMap_) {
            auto sceneSession = iter.second;
            if (sceneSession == nullptr) {
                continue;
            }
            auto state = sceneSession->GetSessionState();
            if (state == SessionState::STATE_FOREGROUND || state == SessionState::STATE_ACTIVE) {
                sceneSession->NotifyScreenshot();
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "OnScreenshot:PID:" + std::to_string(displayId));
}

WSError SceneSessionManager::ClearSession(int32_t persistentId)
{
    WLOGFI("run ClearSession with persistentId: %{public}d", persistentId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, persistentId]() {
        sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
        return ClearSession(sceneSession);
    };
    taskScheduler_->PostAsyncTask(task, "ClearSession:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::ClearSession(sptr<SceneSession> sceneSession)
{
    WLOGFI("run ClearSession");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!IsSessionClearable(sceneSession)) {
        WLOGFI("sceneSession cannot be clear, persistentId %{public}d.", sceneSession->GetPersistentId());
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    const WSError& errCode = sceneSession->Clear();
    return errCode;
}

WSError SceneSessionManager::ClearAllSessions()
{
    WLOGFI("run ClearAllSessions");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this]() {
        std::vector<sptr<SceneSession>> sessionVector;
        GetAllClearableSessions(sessionVector);
        for (uint32_t i = 0; i < sessionVector.size(); i++) {
            ClearSession(sessionVector[i]);
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "ClearAllSessions");
    return WSError::WS_OK;
}

void SceneSessionManager::GetAllClearableSessions(std::vector<sptr<SceneSession>>& sessionVector)
{
    WLOGFI("run GetAllClearableSessions");
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto scnSession = item.second;
        if (IsSessionClearable(scnSession)) {
            sessionVector.push_back(scnSession);
        }
    }
}

WSError SceneSessionManager::LockSession(int32_t sessionId)
{
    WLOGFI("run LockSession with persistentId: %{public}d", sessionId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionId]() {
        auto sceneSession = GetSceneSession(sessionId);
        if (sceneSession == nullptr) {
            WLOGFE("can not find sceneSession, sessionId:%{public}d", sessionId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoLockedState(true);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "LockSession:SID:" + std::to_string(sessionId));
}

WSError SceneSessionManager::UnlockSession(int32_t sessionId)
{
    WLOGFI("run UnlockSession with persistentId: %{public}d", sessionId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionId]() {
        auto sceneSession = GetSceneSession(sessionId);
        if (sceneSession == nullptr) {
            WLOGFE("can not find sceneSession, sessionId:%{public}d", sessionId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoLockedState(false);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UnlockSession" + std::to_string(sessionId));
}

WSError SceneSessionManager::MoveSessionsToForeground(const std::vector<int32_t>& sessionIds, int32_t topSessionId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "run");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    return WSError::WS_OK;
}

WSError SceneSessionManager::MoveSessionsToBackground(const std::vector<int32_t>& sessionIds,
    std::vector<int32_t>& result)
{
    TLOGI(WmsLogTag::WMS_LIFE, "run");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    result.insert(result.end(), sessionIds.begin(), sessionIds.end());
    return WSError::WS_OK;
}

bool SceneSessionManager::IsSessionClearable(sptr<SceneSession> scnSession)
{
    if (scnSession == nullptr) {
        WLOGFI("scnSession is nullptr");
        return false;
    }
    SessionInfo sessionInfo = scnSession->GetSessionInfo();
    if (sessionInfo.abilityInfo == nullptr) {
        WLOGFI("scnSession abilityInfo is nullptr");
        return false;
    }
    if (sessionInfo.abilityInfo->excludeFromMissions) {
        WLOGFI("persistentId %{public}d is excludeFromMissions", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.abilityInfo->unclearableMission) {
        WLOGFI("persistentId %{public}d is unclearable", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.isSystem_) {
        WLOGFI("persistentId %{public}d is system app", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.lockedState) {
        WLOGFI("persistentId %{public}d is in lockedState", scnSession->GetPersistentId());
        return false;
    }

    return true;
}

WSError SceneSessionManager::RegisterIAbilityManagerCollaborator(int32_t type,
    const sptr<AAFwk::IAbilityManagerCollaborator> &impl)
{
    WLOGFI("RegisterIAbilityManagerCollaborator with type : %{public}d", type);
    auto isSaCall = SessionPermission::IsSACalling();
    auto callingUid = IPCSkeleton::GetCallingUid();
    if (!isSaCall || (callingUid != BROKER_UID && callingUid != BROKER_RESERVE_UID)) {
        WLOGFE("The interface only support for broker");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (!CheckCollaboratorType(type)) {
        WLOGFW("collaborator register failed, invalid type.");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    {
        std::unique_lock<std::shared_mutex> lock(collaboratorMapLock_);
        collaboratorMap_[type] = impl;
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::UnregisterIAbilityManagerCollaborator(int32_t type)
{
    WLOGFI("UnregisterIAbilityManagerCollaborator with type : %{public}d", type);
    auto isSaCall = SessionPermission::IsSACalling();
    auto callingUid = IPCSkeleton::GetCallingUid();
    if (!isSaCall || (callingUid != BROKER_UID && callingUid != BROKER_RESERVE_UID)) {
        WLOGFE("The interface only support for broker");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (!CheckCollaboratorType(type)) {
        WLOGFE("collaborator unregister failed, invalid type.");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    {
        std::unique_lock<std::shared_mutex> lock(collaboratorMapLock_);
        collaboratorMap_.erase(type);
    }
    return WSError::WS_OK;
}

bool SceneSessionManager::CheckCollaboratorType(int32_t type)
{
    if (type != CollaboratorType::RESERVE_TYPE && type != CollaboratorType::OTHERS_TYPE) {
        WLOGFD("type is invalid");
        return false;
    }
    return true;
}

BrokerStates SceneSessionManager::CheckIfReuseSession(SessionInfo& sessionInfo)
{
    auto abilityInfo = QueryAbilityInfoFromBMS(currentUserId_, sessionInfo.bundleName_, sessionInfo.abilityName_,
        sessionInfo.moduleName_);
    if (abilityInfo == nullptr) {
        WLOGFE("CheckIfReuseSession abilityInfo is nullptr!");
        return BrokerStates::BROKER_UNKOWN;
    }
    sessionInfo.abilityInfo = abilityInfo;
    int32_t collaboratorType = CollaboratorType::DEFAULT_TYPE;
    if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE)) {
        collaboratorType = CollaboratorType::RESERVE_TYPE;
    } else if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE)) {
        collaboratorType = CollaboratorType::OTHERS_TYPE;
    }
    if (!CheckCollaboratorType(collaboratorType)) {
        WLOGFW("CheckIfReuseSession not collaborator!");
        return BrokerStates::BROKER_UNKOWN;
    }
    BrokerStates resultValue = NotifyStartAbility(collaboratorType, sessionInfo);
    sessionInfo.collaboratorType_ = collaboratorType;
    sessionInfo.sessionAffinity = sessionInfo.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
    if (FindSessionByAffinity(sessionInfo.sessionAffinity) != nullptr) {
        WLOGFI("FindSessionByAffinity: %{public}s, try to reuse", sessionInfo.sessionAffinity.c_str());
        sessionInfo.reuse = true;
    } else {
        sessionInfo.reuse = false;
    }
    WLOGFI("CheckIfReuseSession end, affinity %{public}s type %{public}d reuse %{public}d",
        sessionInfo.sessionAffinity.c_str(), collaboratorType, sessionInfo.reuse);
    return resultValue;
}

BrokerStates SceneSessionManager::NotifyStartAbility(
    int32_t collaboratorType, const SessionInfo& sessionInfo, int32_t persistentId)
{
    WLOGFI("run NotifyStartAbility type %{public}d param id %{public}d", collaboratorType, persistentId);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(collaboratorType);
        if (iter == collaboratorMap_.end()) {
            WLOGFI("Fail to found collaborator with type: %{public}d", collaboratorType);
            return BrokerStates::BROKER_UNKOWN;
        }
        collaborator = iter->second;
    }
    if (sessionInfo.want == nullptr) {
        WLOGFI("sessionInfo.want is nullptr, init");
        sessionInfo.want = std::make_shared<AAFwk::Want>();
        sessionInfo.want->SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    auto accessTokenIDEx = sessionInfo.callingTokenId_;
    if (collaborator != nullptr) {
        containerStartAbilityTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::string affinity = sessionInfo.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
        if (!affinity.empty() && FindSessionByAffinity(affinity) != nullptr) {
            WLOGFI("NotifyStartAbility affinity exit %{public}s.", affinity.c_str());
            return BrokerStates::BROKER_UNKOWN;
        }
        sessionInfo.want->SetParam("oh_persistentId", persistentId);
        int32_t ret = collaborator->NotifyStartAbility(*(sessionInfo.abilityInfo),
            currentUserId_, *(sessionInfo.want), static_cast<uint64_t>(accessTokenIDEx));
        WLOGFI("NotifyStartAbility ret: %{public}d", ret);
        if (ret == 0) {
            return BrokerStates::BROKER_STARTED;
        } else {
            return BrokerStates::BROKER_NOT_START;
        }
    }
    return BrokerStates::BROKER_UNKOWN;
}

void SceneSessionManager::NotifySessionCreate(sptr<SceneSession> sceneSession, const SessionInfo& sessionInfo)
{
    WLOGFI("run NotifySessionCreate");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    if (sessionInfo.want == nullptr) {
        WLOGFI("sessionInfo.want is nullptr");
        return;
    }
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(sceneSession->GetCollaboratorType());
        if (iter == collaboratorMap_.end()) {
            WLOGFI("Fail to found collaborator with type: %{public}d", sceneSession->GetCollaboratorType());
            return;
        }
        collaborator = iter->second;
    }
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is nullptr");
        return;
    }
    sceneSession->SetSelfToken(abilitySessionInfo->sessionToken);
    abilitySessionInfo->want = *(sessionInfo.want);
    if (collaborator != nullptr) {
        int32_t missionId = abilitySessionInfo->persistentId;
        std::string bundleName = sessionInfo.bundleName_;
        int64_t timestamp = containerStartAbilityTime;
        WindowInfoReporter::GetInstance().ReportContainerStartBegin(missionId, bundleName, timestamp);

        collaborator->NotifyMissionCreated(abilitySessionInfo);
    }
}

void SceneSessionManager::NotifyLoadAbility(int32_t collaboratorType,
    sptr<AAFwk::SessionInfo> abilitySessionInfo, std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    WLOGFI("run NotifyLoadAbility");
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(collaboratorType);
        if (iter == collaboratorMap_.end()) {
            WLOGFE("Fail to found collaborator with type: %{public}d", collaboratorType);
            return;
        }
        collaborator = iter->second;
    }
    if (collaborator != nullptr) {
        collaborator->NotifyLoadAbility(*abilityInfo, abilitySessionInfo);
    }
}


void SceneSessionManager::NotifyUpdateSessionInfo(sptr<SceneSession> sceneSession)
{
    WLOGFI("run NotifyUpdateSessionInfo");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(sceneSession->GetCollaboratorType());
        if (iter == collaboratorMap_.end()) {
            WLOGFE("Fail to found collaborator with type: %{public}d", sceneSession->GetCollaboratorType());
            return;
        }
        collaborator = iter->second;
    }
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    if (collaborator != nullptr) {
        collaborator->UpdateMissionInfo(abilitySessionInfo);
    }
}

void SceneSessionManager::NotifyMoveSessionToForeground(int32_t collaboratorType, int32_t persistentId)
{
    WLOGFI("run NotifyMoveSessionToForeground");
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(collaboratorType);
        if (iter == collaboratorMap_.end()) {
            WLOGFE("Fail to found collaborator with type: %{public}d", collaboratorType);
            return;
        }
        collaborator = iter->second;
    }
    if (collaborator != nullptr) {
        collaborator->NotifyMoveMissionToForeground(persistentId);
    }
}

void SceneSessionManager::NotifyClearSession(int32_t collaboratorType, int32_t persistentId)
{
    WLOGFI("run NotifyClearSession with persistentId %{public}d", persistentId);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator;
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        auto iter = collaboratorMap_.find(collaboratorType);
        if (iter == collaboratorMap_.end()) {
            WLOGFE("Fail to found collaborator with type: %{public}d", collaboratorType);
            return;
        }
        collaborator = iter->second;
    }
    if (collaborator != nullptr) {
        collaborator->NotifyClearMission(persistentId);
    }
}

void SceneSessionManager::PreHandleCollaborator(sptr<SceneSession>& sceneSession, int32_t persistentId)
{
    WLOGFI("run PreHandleCollaborator");
    if (sceneSession == nullptr) {
        return;
    }
    std::string sessionAffinity;
    WLOGFI("try to run NotifyStartAbility and NotifySessionCreate");
    if (sceneSession->GetSessionInfo().want != nullptr) {
        sessionAffinity = sceneSession->GetSessionInfo().want
            ->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
    }
    if (sessionAffinity.empty()) {
        WLOGFI("PreHandleCollaborator sessionAffinity: %{public}s", sessionAffinity.c_str());
        BrokerStates notifyReturn = NotifyStartAbility(
            sceneSession->GetCollaboratorType(), sceneSession->GetSessionInfo(), persistentId);
        if (notifyReturn != BrokerStates::BROKER_STARTED) {
            WLOGFI("PreHandleCollaborator cant notify");
            return;
        }

    }
    if (sceneSession->GetSessionInfo().want != nullptr) {
        WLOGFI("broker persistentId: %{public}d",
            sceneSession->GetSessionInfo().want->GetIntParam(AncoConsts::ANCO_SESSION_ID, 0));
        sceneSession->SetSessionInfoAffinity(sceneSession->GetSessionInfo().want
            ->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY));
        WLOGFI("affinity: %{public}s", sceneSession->GetSessionInfo().sessionAffinity.c_str());
    } else {
        WLOGFI("sceneSession->GetSessionInfo().want is nullptr");
    }
    NotifySessionCreate(sceneSession, sceneSession->GetSessionInfo());
    sceneSession->SetSessionInfoAncoSceneState(AncoSceneState::NOTIFY_CREATE);
}

void SceneSessionManager::AddWindowDragHotArea(uint32_t type, WSRect& area)
{
    WLOGFI("run AddWindowDragHotArea, type: %{public}d,posX: %{public}d,posY: %{public}d,width: %{public}d,"
        "height: %{public}d", type, area.posX_, area.posY_, area.width_, area.height_);
    auto const result = SceneSession::windowDragHotAreaMap_.insert({type, area});
    if (!result.second) {
        result.first->second = area;
    }
}

WSError SceneSessionManager::UpdateMaximizeMode(int32_t persistentId, bool isMaximize)
{
    auto task = [this, persistentId, isMaximize]() -> WSError {
        WLOGFD("update maximize mode, id: %{public}d, isMaximize: %{public}d", persistentId, isMaximize);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("could not find window, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        sceneSession->UpdateMaximizeMode(isMaximize);
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateMaximizeMode:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetIsLayoutFullScreen(bool& isLayoutFullScreen)
{
    auto task = [this, &isLayoutFullScreen]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto item = sceneSessionMap_.begin(); item != sceneSessionMap_.end(); ++item) {
            auto sceneSession = item->second;
            if (sceneSession == nullptr) {
                WLOGFE("Session is nullptr");
                continue;
            }
            if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
                continue;
            }
            auto state = sceneSession->GetSessionState();
            if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
                continue;
            }
            if (sceneSession->GetWindowMode() != WindowMode::WINDOW_MODE_FULLSCREEN) {
                continue;
            }
            auto property = sceneSession->GetSessionProperty();
            if (property == nullptr) {
                WLOGFE("Property is nullptr");
                continue;
            }
            isLayoutFullScreen = property->IsLayoutFullScreen();
            auto persistentId = sceneSession->GetPersistentId();
            if (isLayoutFullScreen) {
                WLOGFD("Current window is immersive, persistentId:%{public}d", persistentId);
                return WSError::WS_OK;
            } else {
                WLOGFD("Current window is not immersive, persistentId:%{public}d", persistentId);
            }
        }
        WLOGFD("No immersive window");
        return WSError::WS_OK;
    };

    taskScheduler_->PostSyncTask(task, "GetIsLayoutFullScreen");
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateSessionDisplayId(int32_t persistentId, uint64_t screenId)
{
    auto scnSession = GetSceneSession(persistentId);
    if (!scnSession) {
        WLOGFE("session is nullptr");
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    auto fromScreenId = scnSession->GetSessionInfo().screenId_;
    scnSession->SetScreenId(screenId);
    if (!scnSession->GetSessionProperty()) {
        WLOGFE("Property is null, synchronous screenId failed");
        return WSError::WS_ERROR_NULLPTR;
    }
    scnSession->GetSessionProperty()->SetDisplayId(screenId);
    WLOGFD("Session move display %{public}" PRIu64 " from %{public}" PRIu64, screenId, fromScreenId);
    NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::MOVE_DISPLAY, fromScreenId);
    scnSession->NotifyDisplayMove(fromScreenId, screenId);
    scnSession->UpdateDensity();
    return WSError::WS_OK;
}

void DisplayChangeListener::OnImmersiveStateChange(bool& immersive)
{
    immersive = SceneSessionManager::GetInstance().UpdateImmersiveState();
}

bool SceneSessionManager::UpdateImmersiveState()
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto item = sceneSessionMap_.begin(); item != sceneSessionMap_.end(); ++item) {
        auto sceneSession = item->second;
        if (sceneSession == nullptr) {
            WLOGFE("Session is nullptr");
            continue;
        }
        if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
            continue;
        }
        auto state = sceneSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            continue;
        }
        if (sceneSession->GetWindowMode() != WindowMode::WINDOW_MODE_FULLSCREEN) {
            continue;
        }
        auto property = sceneSession->GetSessionProperty();
        if (property == nullptr) {
            WLOGFE("Property is nullptr");
            continue;
        }
        auto sysBarProperty = property->GetSystemBarProperty();
        if (sysBarProperty[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ == false) {
            WLOGFD("Current window is immersive");
            return true;
        } else {
            WLOGFD("Current window is not immersive");
            break;
        }
    }
    return false;
}

void SceneSessionManager::NotifySessionForeground(const sptr<SceneSession>& session, uint32_t reason,
    bool withAnimation)
{
    session->NotifySessionForeground(reason, withAnimation);
}

void SceneSessionManager::NotifySessionBackground(const sptr<SceneSession>& session, uint32_t reason,
    bool withAnimation, bool isFromInnerkits)
{
    session->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

WSError SceneSessionManager::UpdateTitleInTargetPos(int32_t persistentId, bool isShow, int32_t height)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("could not find window, persistentId:%{public}d", persistentId);
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    return sceneSession->UpdateTitleInTargetPos(isShow, height);
}

const std::map<int32_t, sptr<SceneSession>> SceneSessionManager::GetSceneSessionMap()
{
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        retSceneSessionMap = sceneSessionMap_;
    }
    EraseIf(retSceneSessionMap, [this](const auto& pair) {
        if (pair.second == nullptr) {
            return true;
        }

        if (pair.second->GetWindowType() == WindowType::WINDOW_TYPE_KEYBOARD_PANEL) {
            if (pair.second->IsVisible()) {
                return false;
            }
            return true;
        }

        if (pair.second->IsSystemInput()) {
            return false;
        } else if (pair.second->IsSystemSession() && pair.second->IsVisible() && pair.second->IsSystemActive()) {
            return false;
        }
        if (!Rosen::SceneSessionManager::GetInstance().IsSessionVisible(pair.second)) {
            return true;
        }
        return false;
    });
    return retSceneSessionMap;
}

void SceneSessionManager::NotifyUpdateRectAfterLayout()
{
    auto transactionController = Rosen::RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    if (transactionController) {
        rsTransaction = transactionController->GetRSTransaction();
    }
    auto task = [this, rsTransaction]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter: sceneSessionMap_) {
            auto sceneSession = iter.second;
            if (sceneSession && sceneSession->IsDirtyWindow()) {
                sceneSession->NotifyClientToUpdateRect(rsTransaction);
            }
        }
    };
    // need sync task since animation transcation need
    return taskScheduler_->PostAsyncTask(task, "NotifyUpdateRectAfterLayout");
}

WSError SceneSessionManager::RaiseWindowToTop(int32_t persistentId)
{
    WLOGFI("RaiseWindowToTop, id %{public}d", persistentId);
    auto isSaCall = SessionPermission::IsSACalling();
    if (!isSaCall) {
        WLOGFE("The interface only support for sa call");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, persistentId]() {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (!IsSessionVisible(sceneSession)) {
            WLOGFD("session is not visible!");
            return WSError::WS_DO_NOTHING;
        }
        FocusChangeReason reason = FocusChangeReason::MOVE_UP;
        RequestSessionFocus(persistentId, true, reason);
        if (WindowHelper::IsSubWindow(sceneSession->GetWindowType())) {
            sceneSession->RaiseToAppTop();
        }
        if (WindowHelper::IsSubWindow(sceneSession->GetWindowType()) ||
            sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            WLOGFD("parent session id: %{public}d", sceneSession->GetParentPersistentId());
            sceneSession = GetSceneSession(sceneSession->GetParentPersistentId());
        }
        if (sceneSession == nullptr) {
            WLOGFE("parent session is nullptr");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            sceneSession->NotifyClick();
            return WSError::WS_OK;
        } else {
            WLOGFE("session is not app main window!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
    };
    taskScheduler_->PostAsyncTask(task, "RaiseWindowToTop");
    return WSError::WS_OK;
}

WSError SceneSessionManager::ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId)
{
    WLOGI("run ShiftAppWindowFocus, form id: %{public}d to id: %{public}d", sourcePersistentId, targetPersistentId);
    if (sourcePersistentId != focusedSessionId_) {
        WLOGE("source session need be focused");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    if (targetPersistentId == focusedSessionId_) {
        WLOGE("target session has been focused");
        return WSError::WS_DO_NOTHING;
    }
    sptr<SceneSession> sourceSession = nullptr;
    WSError ret = GetAppMainSceneSession(sourceSession, sourcePersistentId);
    if (ret != WSError::WS_OK) {
        return ret;
    }
    sptr<SceneSession> targetSession = nullptr;
    ret = GetAppMainSceneSession(targetSession, targetPersistentId);
    if (ret != WSError::WS_OK) {
        return ret;
    }
    if (sourceSession->GetSessionInfo().bundleName_ != targetSession->GetSessionInfo().bundleName_) {
        WLOGE("verify bundle name failed, source bundle name is %{public}s but target bundle name is %{public}s)",
            sourceSession->GetSessionInfo().bundleName_.c_str(), targetSession->GetSessionInfo().bundleName_.c_str());
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    if (!SessionPermission::IsSameBundleNameAsCalling(targetSession->GetSessionInfo().bundleName_)) {
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    targetSession->NotifyClick();
    FocusChangeReason reason = FocusChangeReason::CLIENT_REQUEST;
    return RequestSessionFocus(targetPersistentId, false, reason);
}

WSError SceneSessionManager::GetAppMainSceneSession(sptr<SceneSession>& sceneSession, int32_t persistentId)
{
    sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGE("session(id: %{public}d) is nullptr", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
            WLOGE("session(id: %{public}d) is not main window or sub window", persistentId);
            return WSError::WS_ERROR_INVALID_CALLING;
        }
        sceneSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (sceneSession == nullptr) {
            WLOGE("session(id: %{public}d) parent is nullptr", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
    }
    return WSError::WS_OK;
}

std::shared_ptr<Media::PixelMap> SceneSessionManager::GetSessionSnapshotPixelMap(const int32_t persistentId,
    const float scaleParam)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (!sceneSession) {
        WLOGFE("get scene session is nullptr");
        return nullptr;
    }

    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, persistentId, scaleParam, weakSceneSession]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetSessionSnapshotPixelMap(%d )", persistentId);
        auto scnSession = weakSceneSession.promote();
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return pixelMap;
        }

        if (scnSession->GetSessionState() == SessionState::STATE_ACTIVE ||
            scnSession->GetSessionState() == SessionState::STATE_FOREGROUND) {
            pixelMap = scnSession->Snapshot(scaleParam);
        }
        if (!pixelMap) {
            WLOGFI("get local snapshot pixelmap start");
            pixelMap = scnSession->GetSnapshotPixelMap (snapshotScale_, scaleParam);
        }
        return pixelMap;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshotPixelMap" + std::to_string(persistentId));
}

void AppAnrListener::OnAppDebugStarted(const std::vector<AppExecFwk::AppDebugInfo> &debugInfos)
{
    WLOGFI("AppAnrListener OnAppDebugStarted");
    if (debugInfos.empty()) {
        WLOGFE("AppAnrListener OnAppDebugStarted debugInfos is empty");
        return;
    }
    DelayedSingleton<ANRManager>::GetInstance()->SwitchAnr(false);
}

void AppAnrListener::OnAppDebugStoped(const std::vector<AppExecFwk::AppDebugInfo> &debugInfos)
{
    WLOGFI("AppAnrListener OnAppDebugStoped");
    if (debugInfos.empty()) {
        WLOGFE("AppAnrListener OnAppDebugStoped debugInfos is empty");
        return;
    }
    DelayedSingleton<ANRManager>::GetInstance()->SwitchAnr(true);
}

void SceneSessionManager::FlushWindowInfoToMMI(const bool forceFlush)
{
    if (SceneInputManager::GetInstance().IsUserBackground()) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "The user is in the background, no need to flush info to MMI");
        return;
    }
    auto task = [forceFlush]()-> WSError {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::FlushWindowInfoToMMI");
        SceneInputManager::GetInstance().FlushDisplayInfoToMMI(forceFlush);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostAsyncTask(task);
}

WMError SceneSessionManager::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("GetVisibilityWindowInfo permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, &infos]() {
        for (auto [surfaceId, _] : lastVisibleData_) {
            sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
            if (session == nullptr) {
                continue;
            }
            infos.emplace_back(new WindowVisibilityInfo(session->GetWindowId(), session->GetCallingPid(),
                session->GetCallingUid(), session->GetVisibilityState(), session->GetWindowType()));
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetVisibilityWindowInfo");
}

void SceneSessionManager::PostFlushWindowInfoTask(FlushWindowInfoTask &&task,
    const std::string taskName, const int delayTime)
{
    taskScheduler_->PostAsyncTask(std::move(task), taskName, delayTime);
}

void SceneSessionManager::AddExtensionWindowStageToSCB(const sptr<ISessionStage>& sessionStage, int32_t persistentId,
    int32_t parentId)
{
    auto task = [this, sessionStage, persistentId, parentId]() {
        if (sessionStage == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "sessionStage is nullptr");
            return;
        }

        auto remoteExtSession = sessionStage->AsObject();
        remoteExtSessionMap_.insert(std::make_pair(remoteExtSession, std::make_pair(persistentId, parentId)));

        if (extensionDeath_ == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "failed to create death recipient");
            return;
        }
        if (!remoteExtSession->AddDeathRecipient(extensionDeath_)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "failed to add death recipient");
            return;
        }
        TLOGD(WmsLogTag::WMS_UIEXT, "add extension window stage Id: %{public}d, parent Id: %{public}d",
            persistentId, parentId);
    };
    taskScheduler_->PostAsyncTask(task, "AddExtensionWindowStageToSCB");
}

void SceneSessionManager::AddSecureSession(int32_t persistentId, bool shouldHide,
    size_t& sizeBefore, size_t& sizeAfter)
{
    sizeBefore = secureSessionSet_.size();
    if (shouldHide) {
        secureSessionSet_.insert(persistentId);
    } else {
        secureSessionSet_.erase(persistentId);
    }
    sizeAfter = secureSessionSet_.size();
}

void SceneSessionManager::HideNonSecureFloatingWindows(size_t sizeBefore, size_t sizeAfter, bool shouldHide)
{
    auto stateShouldChange = (sizeBefore == 0 && sizeAfter > 0) || (sizeBefore > 0 && sizeAfter == 0);
    if (!stateShouldChange) {
        return;
    }

    for (const auto& item: nonSystemFloatSceneSessionMap_) {
        auto session = item.second;
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT) {
            session->NotifyForceHideChange(shouldHide);
            TLOGI(WmsLogTag::WMS_UIEXT, "HideNonSecureWindows name=%{public}s, persistentId=%{public}d, "
                "shouldHide=%{public}u", session->GetWindowName().c_str(), item.first, shouldHide);
        }
    }
}

void SceneSessionManager::HideNonSecureSubWindows(const sptr<SceneSession>& sceneSession,
    size_t sizeBefore, size_t sizeAfter, bool shouldHide)
{
    // don't let sub-window show when switching secure host window to background
    if (!sceneSession->IsSessionForeground() || sizeBefore == sizeAfter) {
        return;
    }

    auto subSessions = sceneSession->GetSubSession();
    for (const auto& subSession: subSessions) {
        if (subSession == nullptr) {
            TLOGD(WmsLogTag::WMS_UIEXT, "sub session is nullptr");
            continue;
        }

        subSession->NotifyForceHideChange(shouldHide);
        TLOGI(WmsLogTag::WMS_UIEXT, "HideNonSecureWindows name=%{public}s, persistentId=%{public}d, "
            "shouldHide=%{public}u", subSession->GetWindowName().c_str(), subSession->GetPersistentId(),
            shouldHide);
    }
}

WSError SceneSessionManager::HandleSecureSessionShouldHide(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "sceneSession is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    auto persistentId = sceneSession->GetPersistentId();
    auto shouldHide = sceneSession->GetCombinedExtWindowFlags().hideNonSecureWindowsFlag;
    size_t sizeBefore = 0;
    size_t sizeAfter = 0;
    AddSecureSession(persistentId, shouldHide, sizeBefore, sizeAfter);
    HideNonSecureFloatingWindows(sizeBefore, sizeAfter, shouldHide);
    HideNonSecureSubWindows(sceneSession, sizeBefore, sizeAfter, shouldHide);

    return WSError::WS_OK;
}

WSError SceneSessionManager::HandleSecureExtSessionShouldHide(int32_t persistentId, bool shouldHide)
{
    size_t sizeBefore = 0;
    size_t sizeAfter = 0;
    AddSecureSession(persistentId, shouldHide, sizeBefore, sizeAfter);
    HideNonSecureFloatingWindows(sizeBefore, sizeAfter, shouldHide);

    return WSError::WS_OK;
}

WSError SceneSessionManager::HandleSCBExtWaterMarkChange(int32_t persistentId, bool isWaterMarkEnable)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "check watermark for scb uiext");
    if (isWaterMarkEnable) {
        waterMarkSessionSet_.insert(persistentId);
    } else {
        waterMarkSessionSet_.erase(persistentId);
    }
    CheckAndNotifyWaterMarkChangedResult();
    return WSError::WS_OK;
}

void SceneSessionManager::HandleSpecialExtWindowFlagChange(int32_t persistentId, ExtensionWindowFlags flags,
    ExtensionWindowFlags actions)
{
    if (actions.waterMarkFlag) {
        HandleSCBExtWaterMarkChange(persistentId, flags.waterMarkFlag);
    }
    if (actions.hideNonSecureWindowsFlag) {
        HandleSecureExtSessionShouldHide(persistentId, flags.hideNonSecureWindowsFlag);
    }
}

WSError SceneSessionManager::AddOrRemoveSecureSession(int32_t persistentId, bool shouldHide)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "persistentId=%{public}d, shouldHide=%{public}u", persistentId, shouldHide);
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "HideNonSecureWindows permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, persistentId, shouldHide]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        auto iter = sceneSessionMap_.find(persistentId);
        if (iter == sceneSessionMap_.end()) {
            TLOGE(WmsLogTag::WMS_UIEXT, "AddOrRemoveSecureSession: Session with persistentId %{public}d not found",
                persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }

        auto sceneSession = iter->second;
        sceneSession->SetShouldHideNonSecureWindows(shouldHide);
        return HandleSecureSessionShouldHide(sceneSession);
    };

    taskScheduler_->PostAsyncTask(task, "AddOrRemoveSecureSession");
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateExtWindowFlags(int32_t parentId, int32_t persistentId, uint32_t extWindowFlags,
    uint32_t extWindowActions)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "parentId=%{public}d, persistentId=%{public}d, extWindowFlags=%{public}d, "
        "actions=%{public}d", parentId, persistentId, extWindowFlags, extWindowActions);

    ExtensionWindowFlags actions(extWindowActions);
    auto ret = WSError::WS_OK;
    bool needSystemCalling = actions.hideNonSecureWindowsFlag || actions.waterMarkFlag;
    if (needSystemCalling && !SessionPermission::IsSystemCalling()) {
        actions.hideNonSecureWindowsFlag = false;
        actions.waterMarkFlag = false;
        TLOGE(WmsLogTag::WMS_UIEXT, "system calling permission denied!");
        ret = WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto needPrivacyWindow = actions.privacyModeFlag;
    if (needPrivacyWindow && !SessionPermission::VerifyCallingPermission("ohos.permission.PRIVACY_WINDOW")) {
        actions.privacyModeFlag = false;
        TLOGE(WmsLogTag::WMS_UIEXT, "privacy window permission denied!");
        ret = WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (actions.bitData == 0) {
        return ret;
    }

    ExtensionWindowFlags flags(extWindowFlags);
    auto task = [this, parentId, persistentId, flags, actions]() {
        auto sceneSession = GetSceneSession(parentId);
        if (sceneSession == nullptr) {
            TLOGD(WmsLogTag::WMS_UIEXT, "UpdateExtWindowFlags: Parent session with persistentId %{public}d not found",
                parentId);
            HandleSpecialExtWindowFlagChange(persistentId, flags, actions);
            return WSError::WS_OK;
        }

        auto oldFlags = sceneSession->GetCombinedExtWindowFlags();
        sceneSession->UpdateExtWindowFlags(persistentId, flags, actions);
        auto newFlags = sceneSession->GetCombinedExtWindowFlags();
        if (oldFlags.hideNonSecureWindowsFlag != newFlags.hideNonSecureWindowsFlag) {
            HandleSecureSessionShouldHide(sceneSession);
        }
        if (oldFlags.waterMarkFlag != newFlags.waterMarkFlag) {
            CheckAndNotifyWaterMarkChangedResult();
        }
        if (oldFlags.privacyModeFlag != newFlags.privacyModeFlag) {
            UpdatePrivateStateAndNotify(parentId);
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "UpdateExtWindowFlags");
    return ret;
}

void SceneSessionManager::ReportWindowProfileInfos()
{
    enum class WindowVisibleState : int32_t {
        FOCUSBLE = 0,
        VISIBLE,
        MINIMIZED,
        OCCLUSION
    };
    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    auto focusWindowId = GetFocusedSessionId();
    for (const auto& elem : sceneSessionMapCopy) {
        auto curSession = elem.second;
        if (curSession == nullptr || curSession->GetSessionInfo().isSystem_ ||
            curSession->GetWindowType() !=  WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            continue;
        }
        WindowProfileInfo windowProfileInfo;
        windowProfileInfo.bundleName = curSession->GetSessionInfo().bundleName_;
        windowProfileInfo.windowLocatedScreen = static_cast<int32_t>(
            curSession->GetSessionProperty()->GetDisplayId());
        windowProfileInfo.windowSceneMode = static_cast<int32_t>(curSession->GetWindowMode());
        if (focusWindowId == static_cast<int32_t>(curSession->GetWindowId())) {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::FOCUSBLE);
        } else if (curSession->GetSessionState() == SessionState::STATE_BACKGROUND) {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::MINIMIZED);
        } else if (!curSession->GetVisible()) {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::OCCLUSION);
        } else {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::VISIBLE);
        }
        WindowInfoReporter::GetInstance().ReportWindowProfileInfo(windowProfileInfo);
        WLOGFD("ReportWindowProfileInfo, bundleName:%{public}s, windowVisibleState:%{public}d, "
            "windowLocatedScreen:%{public}d, windowSceneMode:%{public}d",
            windowProfileInfo.bundleName.c_str(), windowProfileInfo.windowVisibleState,
            windowProfileInfo.windowLocatedScreen, windowProfileInfo.windowSceneMode);
    }
}

bool SceneSessionManager::IsVectorSame(const std::vector<VisibleWindowNumInfo>& lastInfo,
    const std::vector<VisibleWindowNumInfo>& currentInfo)
{
    if (lastInfo.size() != currentInfo.size()) {
        WLOGFE("last and current info is not Same");
        return false;
    }
    int sizeOfLastInfo = static_cast<int>(lastInfo.size());
    for (int i = 0; i < sizeOfLastInfo; i++) {
        if (lastInfo[i].displayId != currentInfo[i].displayId ||
            lastInfo[i].visibleWindowNum != currentInfo[i].visibleWindowNum) {
            WLOGFE("last and current visible window num is not Same");
            return false;
        }
    }
    return true;
}

void SceneSessionManager::CacVisibleWindowNum()
{
    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    for (const auto& elem : sceneSessionMapCopy) {
        auto curSession = elem.second;
        bool isTargetWindow = (WindowHelper::IsMainWindow(curSession->GetWindowType()) ||
            curSession->GetWindowType() == WindowType::WINDOW_TYPE_WALLPAPER);
        if (curSession == nullptr || !isTargetWindow ||
            curSession->GetSessionState() == SessionState::STATE_BACKGROUND) {
            continue;
        }

        bool isWindowVisible = curSession->GetVisible();
        if (isWindowVisible) {
            int32_t displayId = static_cast<int32_t>(curSession->GetSessionProperty()->GetDisplayId());
            auto it = std::find_if(visibleWindowNumInfo.begin(), visibleWindowNumInfo.end(),
                [=](const VisibleWindowNumInfo& info) {
                    return (static_cast<int32_t>(info.displayId)) == displayId;
            });
            if (it == visibleWindowNumInfo.end()) {
                visibleWindowNumInfo.push_back({displayId, 1});
            } else {
                it->visibleWindowNum++;
            }
        }
    }
    std::unique_lock<std::shared_mutex> lock(lastInfoMutex_);
    if (visibleWindowNumInfo.size() > 0 && !IsVectorSame(lastInfo_, visibleWindowNumInfo)) {
        SessionManagerAgentController::GetInstance().UpdateVisibleWindowNum(visibleWindowNumInfo);
        lastInfo_ = visibleWindowNumInfo;
    }
}

WSError SceneSessionManager::GetHostWindowRect(int32_t hostWindowId, Rect& rect)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "hostWindowId:%{public}d", hostWindowId);
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "GetHostWindowRect permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, hostWindowId, &rect]() {
        auto sceneSession = GetSceneSession(hostWindowId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Session with persistentId %{public}d not found", hostWindowId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        WSRect hostRect = sceneSession->GetSessionRect();
        rect = {hostRect.posX_, hostRect.posY_, hostRect.width_, hostRect.height_};
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "GetHostWindowRect");
    return WSError::WS_OK;
}

std::shared_ptr<SkRegion> SceneSessionManager::GetDisplayRegion(DisplayId displayId)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "get display object failed of display: %{public}" PRIu64, displayId);
        return nullptr;
    }
    int32_t displayWidth = display->GetWidth();
    int32_t displayHeight = display->GetHeight();
    if (displayWidth == 0 || displayHeight == 0) {
        TLOGE(WmsLogTag::WMS_MAIN, "invalid display size of display: %{public}" PRIu64, displayId);
        return nullptr;
    }

    SkIRect rect {.fLeft = 0, .fTop = 0, .fRight = displayWidth, .fBottom = displayHeight};
    return std::make_shared<SkRegion>(rect);
}

void SceneSessionManager::GetAllSceneSessionForAccessibility(std::vector<sptr<SceneSession>>& sceneSessionList)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (!sceneSession->IsVisibleForAccessibility()) {
            continue;
        }
        if (sceneSession->GetSessionInfo().bundleName_.find("SCBGestureBack") != std::string::npos
            || sceneSession->GetSessionInfo().bundleName_.find("SCBGestureNavBar") != std::string::npos) {
            continue;
        }
        sceneSessionList.push_back(sceneSession);
    }
}

void SceneSessionManager::FillAccessibilityInfo(std::vector<sptr<SceneSession>>& sceneSessionList,
    std::vector<sptr<AccessibilityWindowInfo>>& accessibilityInfo)
{
    for (const auto& sceneSession : sceneSessionList) {
        if (!FillWindowInfo(accessibilityInfo, sceneSession)) {
            TLOGW(WmsLogTag::WMS_MAIN, "fill accessibilityInfo failed");
        }
    }
}

void SceneSessionManager::FilterSceneSessionCovered(std::vector<sptr<SceneSession>>& sceneSessionList)
{
    std::sort(sceneSessionList.begin(), sceneSessionList.end(), [](sptr<SceneSession> a, sptr<SceneSession> b) {
        return a->GetZOrder() > b->GetZOrder();
    });
    std::vector<sptr<SceneSession>> result;
    std::unordered_map<DisplayId, std::shared_ptr<SkRegion>> unaccountedSpaceMap;
    for (const auto& sceneSession : sceneSessionList) {
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "invalid scene session");
            continue;
        }
        auto sessionProperty = sceneSession->GetSessionProperty();
        if (sessionProperty == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "get property of session: %{public}d", sceneSession->GetPersistentId());
            continue;
        }
        std::shared_ptr<SkRegion> unaccountedSpace = nullptr;
        auto displayId = sessionProperty->GetDisplayId();
        if (unaccountedSpaceMap.find(displayId) != unaccountedSpaceMap.end()) {
            unaccountedSpace = unaccountedSpaceMap[displayId];
        } else {
            unaccountedSpace = GetDisplayRegion(displayId);
            if (unaccountedSpace == nullptr) {
                TLOGE(WmsLogTag::WMS_MAIN, "get display region of display: %{public}" PRIu64, displayId);
                continue;
            }
            unaccountedSpaceMap[displayId] = unaccountedSpace;
        }
        WSRect wsRect = sceneSession->GetSessionRect();
        SkIRect windowBounds {.fLeft = wsRect.posX_, .fTop = wsRect.posY_,
                              .fRight = wsRect.posX_ + wsRect.width_, .fBottom = wsRect.posY_ + wsRect.height_};
        SkRegion windowRegion(windowBounds);
        if (unaccountedSpace->quickReject(windowRegion)) {
            TLOGD(WmsLogTag::WMS_MAIN, "quick reject: [l=%{public}d,t=%{public}d,r=%{public}d,b=%{public}d]",
                windowBounds.fLeft, windowBounds.fTop, windowBounds.fRight, windowBounds.fBottom);
            continue;
        }
        if (!unaccountedSpace->intersects(windowRegion)) {
            TLOGD(WmsLogTag::WMS_MAIN, "no intersects: [l=%{public}d,t=%{public}d,r=%{public}d,b=%{public}d]",
                windowBounds.fLeft, windowBounds.fTop, windowBounds.fRight, windowBounds.fBottom);
            continue;
        }
        result.push_back(sceneSession);
        unaccountedSpace->op(windowRegion, SkRegion::Op::kDifference_Op);
        if (unaccountedSpace->isEmpty()) {
            break;
        }
    }
    sceneSessionList = result;
}

void SceneSessionManager::NotifyAllAccessibilityInfo()
{
    std::vector<sptr<SceneSession>> sceneSessionList;
    GetAllSceneSessionForAccessibility(sceneSessionList);
    FilterSceneSessionCovered(sceneSessionList);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    FillAccessibilityInfo(sceneSessionList, accessibilityInfo);

    for (const auto& item : accessibilityInfo) {
        TLOGD(WmsLogTag::WMS_MAIN, "notify accessibilityWindow wid = %{public}d, inWid = %{public}d, \
            bundle=%{public}s,bounds=(x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d)",
            item->wid_, item->innerWid_, item->bundleName_.c_str(),
            item->windowRect_.posX_, item->windowRect_.posY_, item->windowRect_.width_, item->windowRect_.height_);
        for (const auto& rect : item->touchHotAreas_) {
            TLOGD(WmsLogTag::WMS_MAIN, "window touch hot areas rect[x=%{public}d,y=%{public}d," \
            "w=%{public}d,h=%{public}d]", rect.posX_, rect.posY_, rect.width_, rect.height_);
        }
    }

    SessionManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(accessibilityInfo,
        WindowUpdateType::WINDOW_UPDATE_ALL);
}

int32_t SceneSessionManager::ReclaimPurgeableCleanMem()
{
#ifdef MEMMGR_WINDOW_ENABLE
    return Memory::MemMgrClient::GetInstance().ReclaimPurgeableCleanMem();
#else
    return -1;
#endif
}

WindowStatus SceneSessionManager::GetWindowStatus(WindowMode mode, SessionState sessionState,
    const sptr<WindowSessionProperty>& property)
{
    auto windowStatus = WindowStatus::WINDOW_STATUS_UNDEFINED;
    if (property == nullptr) {
        return windowStatus;
    }
    if (mode == WindowMode::WINDOW_MODE_FLOATING) {
        windowStatus = WindowStatus::WINDOW_STATUS_FLOATING;
        if (property->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) { // maximize floating
            windowStatus = WindowStatus::WINDOW_STATUS_MAXMIZE;
        }
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        windowStatus = WindowStatus::WINDOW_STATUS_SPLITSCREEN;
    } else if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        windowStatus = WindowStatus::WINDOW_STATUS_FULLSCREEN;
    } else if (sessionState != SessionState::STATE_FOREGROUND && sessionState != SessionState::STATE_ACTIVE) {
        windowStatus = WindowStatus::WINDOW_STATUS_MINIMIZE;
    }
    return windowStatus;
}

WMError SceneSessionManager::GetCallingWindowWindowStatus(int32_t persistentId, WindowStatus& windowStatus)
{
    if (!SessionPermission::IsStartedByInputMethod()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "permission is not allowed persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "scnSession is null, persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_NULLPTR;
    }

    TLOGD(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, windowType: %{public}d",
        persistentId, scnSession->GetWindowType());

    if (scnSession->GetSessionProperty() == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session property is null");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t callingWindowId = scnSession->GetSessionProperty()->GetCallingSessionId();
    auto callingSession = GetSceneSession(callingWindowId);
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "callingsSession is null");
        callingSession = GetSceneSession(focusedSessionId_);
        if (callingSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "callingsSession obtained through focusedSession fail");
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
    }
    if (callingSession->IsSystemSession()) {
        windowStatus = WindowStatus::WINDOW_STATUS_FULLSCREEN;
    } else {
        windowStatus = GetWindowStatus(callingSession->GetWindowMode(), callingSession->GetSessionState(),
            callingSession->GetSessionProperty());
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Get WindowStatus persistentId: %{public}d windowstatus: %{public}d",
        persistentId, windowStatus);
    return WMError::WM_OK;
}

WMError SceneSessionManager::GetCallingWindowRect(int32_t persistentId, Rect& rect)
{
    if (!SessionPermission::IsStartedByInputMethod()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "permission is not allowed persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "scnSession is null, persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_NULLPTR;
    }
    TLOGD(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, windowType: %{public}d",
        persistentId, scnSession->GetWindowType());
    if (scnSession->GetSessionProperty() == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session property is null");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t callingWindowId = scnSession->GetSessionProperty()->GetCallingSessionId();
    auto callingSession = GetSceneSession(callingWindowId);
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "callingsSession is null");
        callingSession = GetSceneSession(focusedSessionId_);
        if (callingSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "callingsSession obtained through focusedSession fail");
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
    }
    WSRect sessionRect = callingSession->GetSessionRect();
    rect = {sessionRect.posX_, sessionRect.posY_, sessionRect.width_, sessionRect.height_};
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Get Rect persistentId: %{public}d, x: %{public}d, y: %{public}d, "
        "height: %{public}u, width: %{public}u", persistentId, rect.posX_, rect.posY_, rect.width_, rect.height_);
    return WMError::WM_OK;
}

void SceneSessionManager::CheckSceneZOrder()
{
    auto task = [this]() {
        AnomalyDetection::SceneZOrderCheckProcess();
    };
    taskScheduler_->PostAsyncTask(task, "CheckSceneZOrder");
}

WMError SceneSessionManager::GetWindowBackHomeStatus(bool &isBackHome)
{
    if (!SessionPermission::IsSACalling()) {
        WLOGFE("GetWindowBackHomeStatus permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    isBackHome = IsBackHomeStatus();
    WLOGFI("Get back home status success, isBackHome: %{public}d", isBackHome);
    return WMError::WM_OK;
}

int32_t SceneSessionManager::GetCustomDecorHeight(int32_t persistentId)
{
    int32_t height = 0;
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Session with persistentId %{public}d not found", persistentId);
        return 0;
    }
    height = sceneSession->GetCustomDecorHeight();
    TLOGD(WmsLogTag::WMS_LAYOUT, "GetCustomDecorHeight: %{public}d", height);
    return height;
}

WMError SceneSessionManager::GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo)
{
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        TLOGE(WmsLogTag::WMS_MAIN, "permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }

    if (!topNInfo.empty() || (topNum <= 0)) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    TLOGD(WmsLogTag::WMS_MAIN, "topNum: %{public}d", topNum);
    auto func = [this, &topNum, &topNInfo](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }

        if (topNum == 0) {
            return true;
        }

        if (!WindowHelper::IsMainWindow(session->GetWindowType()) || !IsSessionVisible(session)) {
            TLOGD(WmsLogTag::WMS_MAIN, "not main window %{public}d", session->GetWindowType());
            return false;
        }

        MainWindowInfo info;
        info.pid_ = session->GetCallingPid();
        info.bundleName_ = session->GetSessionInfo().bundleName_;
        topNInfo.push_back(info);
        topNum--;
        TLOGE(WmsLogTag::WMS_MAIN, "topnNum: %{public}d, pid: %{public}d, bundleName: %{public}s",
            topNum, info.pid_, info.bundleName_.c_str());
        return false;
    };
    TraverseSessionTree(func, true);

    return WMError::WM_OK;
}
} // namespace OHOS::Rosen
