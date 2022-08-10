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

#include "window_manager_service.h"

#include <thread>

#include <ability_manager_client.h>
#include <cinttypes>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <parameters.h>
#include <platform/common/rs_system_properties.h>
#include <rs_iwindow_animation_controller.h>
#include <system_ability_definition.h>
#include "xcollie/watchdog.h"

#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "drag_controller.h"
#include "minimize_app.h"
#include "permission.h"
#include "remote_animation.h"
#include "singleton_container.h"
#include "ui/rs_ui_director.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManagerService)

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<WindowManagerService>());

WindowManagerService::WindowManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true),
    rsInterface_(RSInterfaces::GetInstance())
{
    windowRoot_ = new WindowRoot(
        [this](Event event, const sptr<IRemoteObject>& remoteObject) { OnWindowEvent(event, remoteObject); });
    inputWindowMonitor_ = new InputWindowMonitor(windowRoot_);
    windowController_ = new WindowController(windowRoot_, inputWindowMonitor_);
    dragController_ = new DragController(windowRoot_);
    windowDumper_ = new WindowDumper(windowRoot_);
    freezeDisplayController_ = new FreezeController();
    windowCommonEvent_ = std::make_shared<WindowCommonEvent>();
    startingOpen_ = system::GetParameter("persist.window.sw.enabled", "1") == "1"; // startingWin default enabled
    runner_ = AppExecFwk::EventRunner::Create(name_);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    snapshotController_ = new SnapshotController(windowRoot_, handler_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread(name_, handler_);
    if (ret != 0) {
        WLOGFE("Add watchdog thread failed");
    }

    // init RSUIDirector, it will handle animation callback
    rsUiDirector_ = RSUIDirector::Create();
    rsUiDirector_->SetUITaskRunner([this](const std::function<void()>& task) { PostAsyncTask(task); });
    rsUiDirector_->Init(false);
}

void WindowManagerService::OnStart()
{
    WLOGFI("WindowManagerService::OnStart start");
    if (!Init()) {
        return;
    }
    WindowInnerManager::GetInstance().Start(system::GetParameter("persist.window.holder.enable", "0") == "1");
    sptr<IDisplayChangeListener> listener = new DisplayChangeListener();
    DisplayManagerServiceInner::GetInstance().RegisterDisplayChangeListener(listener);
    sptr<IWindowInfoQueriedListener> windowInfoQueriedListener = new WindowInfoQueriedListener();
    DisplayManagerServiceInner::GetInstance().RegisterWindowInfoQueriedListener(windowInfoQueriedListener);
    RegisterSnapshotHandler();
    RegisterWindowManagerServiceHandler();
    RegisterWindowVisibilityChangeCallback();
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
}

void WindowManagerService::PostAsyncTask(Task task)
{
    if (handler_) {
        bool ret = handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostTask Failed");
        }
    }
}

void WindowManagerService::PostVoidSyncTask(Task task)
{
    if (handler_) {
        bool ret = handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostVoidSyncTask Failed");
        }
    }
}


void WindowManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    WLOGFI(" %{public}d", systemAbilityId);
    if (systemAbilityId == COMMON_EVENT_SERVICE_ID) {
        windowCommonEvent_->SubscriberEvent();
    }
}

void WindowManagerService::OnAccountSwitched(int accountId)
{
    PostAsyncTask([this, accountId]() {
        windowRoot_->RemoveSingleUserWindowNodes(accountId);
    });
    WLOGFI("called");
}

void WindowManagerService::WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusionData)
{
    WLOGFD("NotifyWindowVisibilityChange: enter");
    std::weak_ptr<RSOcclusionData> weak(occlusionData);
    PostVoidSyncTask([this, weak]() {
        auto weakOcclusionData = weak.lock();
        if (weakOcclusionData == nullptr) {
            WLOGFE("weak occlusionData is nullptr");
            return;
        }
        windowRoot_->NotifyWindowVisibilityChange(weakOcclusionData);
    });
}

void WindowManagerService::RegisterWindowVisibilityChangeCallback()
{
    auto windowVisibilityChangeCb = std::bind(&WindowManagerService::WindowVisibilityChangeCallback, this,
        std::placeholders::_1);
    if (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
        WLOGFW("WindowManagerService::RegisterWindowVisibilityChangeCallback failed, create async thread!");
        auto fun = [this, windowVisibilityChangeCb]() {
            WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback async thread enter!");
            int counter = 0;
            while (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
                usleep(10000); // 10000us equals to 10ms
                counter++;
                if (counter >= 2000) { // wait for 2000 * 10ms = 20s
                    WLOGFE("WindowManagerService::RegisterWindowVisibilityChangeCallback timeout!");
                    return;
                }
            }
            WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback async thread register handler"
                " successfully!");
        };
        std::thread thread(fun);
        thread.detach();
        WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback async thread has been detached!");
    } else {
        WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback OnStart succeed!");
    }
}

void WindowManagerService::RegisterSnapshotHandler()
{
    if (snapshotController_ == nullptr) {
        snapshotController_ = new SnapshotController(windowRoot_, handler_);
    }
    if (AAFwk::AbilityManagerClient::GetInstance()->RegisterSnapshotHandler(snapshotController_) != ERR_OK) {
        WLOGFW("WindowManagerService::RegisterSnapshotHandler failed, create async thread!");
        auto fun = [this]() {
            WLOGFI("WindowManagerService::RegisterSnapshotHandler async thread enter!");
            int counter = 0;
            while (AAFwk::AbilityManagerClient::GetInstance()->RegisterSnapshotHandler(snapshotController_) != ERR_OK) {
                usleep(10000); // 10000us equals to 10ms
                counter++;
                if (counter >= 2000) { // wait for 2000 * 10ms = 20s
                    WLOGFE("WindowManagerService::RegisterSnapshotHandler timeout!");
                    return;
                }
            }
            WLOGFI("WindowManagerService::RegisterSnapshotHandler async thread register handler successfully!");
        };
        std::thread thread(fun);
        thread.detach();
        WLOGFI("WindowManagerService::RegisterSnapshotHandler async thread has been detached!");
    } else {
        WLOGFI("WindowManagerService::RegisterSnapshotHandler OnStart succeed!");
    }
}

void WindowManagerService::RegisterWindowManagerServiceHandler()
{
    if (wmsHandler_ == nullptr) {
        wmsHandler_ = new WindowManagerServiceHandler();
    }
    if (AAFwk::AbilityManagerClient::GetInstance()->RegisterWindowManagerServiceHandler(wmsHandler_) != ERR_OK) {
        WLOGFW("RegisterWindowManagerServiceHandler failed, create async thread!");
        auto fun = [this]() {
            WLOGFI("RegisterWindowManagerServiceHandler async thread enter!");
            int counter = 0;
            while (AAFwk::AbilityManagerClient::GetInstance()->
                RegisterWindowManagerServiceHandler(wmsHandler_) != ERR_OK) {
                usleep(10000); // 10000us equals to 10ms
                counter++;
                if (counter >= 2000) { // wait for 2000 * 10ms = 20s
                    WLOGFE("RegisterWindowManagerServiceHandler timeout!");
                    return;
                }
            }
            WLOGFI("RegisterWindowManagerServiceHandler async thread register handler successfully!");
        };
        std::thread thread(fun);
        thread.detach();
        WLOGFI("RegisterWindowManagerServiceHandler async thread has been detached!");
    } else {
        WLOGFI("RegisterWindowManagerServiceHandler OnStart succeed!");
    }
}

void WindowManagerServiceHandler::NotifyWindowTransition(
    sptr<AAFwk::AbilityTransitionInfo> from, sptr<AAFwk::AbilityTransitionInfo> to)
{
    sptr<WindowTransitionInfo> fromInfo = new WindowTransitionInfo(from);
    sptr<WindowTransitionInfo> toInfo = new WindowTransitionInfo(to);
    WindowManagerService::GetInstance().NotifyWindowTransition(fromInfo, toInfo, false);
}

int32_t WindowManagerServiceHandler::GetFocusWindow(sptr<IRemoteObject>& abilityToken)
{
    return static_cast<int32_t>(WindowManagerService::GetInstance().GetFocusWindowInfo(abilityToken));
}

void WindowManagerServiceHandler::StartingWindow(
    sptr<AAFwk::AbilityTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap)
{
    sptr<WindowTransitionInfo> windowInfo = new WindowTransitionInfo(info);
    WLOGFI("hot start is called");
    WindowManagerService::GetInstance().StartingWindow(windowInfo, pixelMap, false);
}

void WindowManagerServiceHandler::StartingWindow(
    sptr<AAFwk::AbilityTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap, uint32_t bgColor)
{
    sptr<WindowTransitionInfo> windowInfo = new WindowTransitionInfo(info);
    WLOGFI("cold start is called");
    WindowManagerService::GetInstance().StartingWindow(windowInfo, pixelMap, true, bgColor);
}

void WindowManagerServiceHandler::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("WindowManagerServiceHandler CancelStartingWindow!");
    WindowManagerService::GetInstance().CancelStartingWindow(abilityToken);
}

bool WindowManagerService::Init()
{
    WLOGFI("WindowManagerService::Init start");
    bool ret = Publish(this);
    if (!ret) {
        WLOGFW("WindowManagerService::Init failed");
        return false;
    }
    if (WindowManagerConfig::LoadConfigXml()) {
        WindowManagerConfig::DumpConfig(WindowManagerConfig::GetConfig());
        ConfigureWindowManagerService();
    }
    WLOGFI("WindowManagerService::Init success");
    return true;
}

int WindowManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (windowDumper_ == nullptr) {
        windowDumper_ = new WindowDumper(windowRoot_);
    }

    return PostSyncTask([this, fd, &args]() {
        return static_cast<int>(windowDumper_->Dump(fd, args));
    });
}

void WindowManagerService::ConfigureWindowManagerService()
{
    const auto& config = WindowManagerConfig::GetConfig();
    if (config.count("decor") != 0 && config.at("decor").property_) {
        if (config.at("decor").property_->count("enable")) {
            systemConfig_.isSystemDecorEnable_ =
                config.at("decor").property_->at("enable").boolValue_;
        }
    }
    if (config.count("minimizeByOther") != 0 && config.at("minimizeByOther").property_) {
        if (config.at("minimizeByOther").property_->count("enable")) {
            MinimizeApp::SetMinimizedByOtherConfig(
                config.at("minimizeByOther").property_->at("enable").boolValue_);
        }
    }
    if (config.count("stretchable") != 0 && config.at("stretchable").property_) {
        if (config.at("stretchable").property_->count("enable")) {
            systemConfig_.isStretchable_ =
                config.at("stretchable").property_->at("enable").boolValue_;
        }
    }
    if (config.count("remoteAnimation") != 0 && config.at("remoteAnimation").property_) {
        if (config.at("remoteAnimation").property_->count("enable")) {
            RemoteAnimation::isRemoteAnimationEnable_ =
                config.at("remoteAnimation").property_->at("enable").boolValue_;
        }
    }
    if (config.count("maxAppWindowNumber") != 0 && config.at("maxAppWindowNumber").IsInts()) {
        auto numbers = *config.at("maxAppWindowNumber").intsValue_;
        if (numbers.size() == 1) {
            if (numbers[0] > 0) {
                windowRoot_->SetMaxAppWindowNumber(static_cast<uint32_t>(numbers[0]));
            }
        }
    }
    if (config.count("modeChangeHotZones") != 0 && config.at("modeChangeHotZones").IsInts()) {
        ConfigHotZones(*config.at("modeChangeHotZones").intsValue_);
    }
    if (config.count("splitRatios") != 0 && config.at("splitRatios").IsFloats()) {
        windowRoot_->SetSplitRatios(*config.at("splitRatios").floatsValue_);
    }
    if (config.count("exitSplitRatios") != 0 && config.at("exitSplitRatios").IsFloats()) {
        windowRoot_->SetExitSplitRatios(*config.at("exitSplitRatios").floatsValue_);
    }
    if (config.count("windowAnimation") && config.at("windowAnimation").IsMap()) {
        ConfigWindowAnimation(*config.at("windowAnimation").mapValue_);
    }
    if (config.count("keyboardAnimation") && config.at("keyboardAnimation").IsMap()) {
        ConfigKeyboardAnimation(*config.at("keyboardAnimation").mapValue_);
    }
}

void WindowManagerService::ConfigHotZones(const std::vector<int>& numbers)
{
    if (numbers.size() == 3) { // 3 hot zones
        hotZonesConfig_.fullscreenRange_ = static_cast<uint32_t>(numbers[0]); // 0 fullscreen
        hotZonesConfig_.primaryRange_ = static_cast<uint32_t>(numbers[1]);    // 1 primary
        hotZonesConfig_.secondaryRange_ = static_cast<uint32_t>(numbers[2]);  // 2 secondary
        hotZonesConfig_.isModeChangeHotZoneConfigured_ = true;
    }
}

void WindowManagerService::ConfigWindowAnimation(const std::map<std::string, WindowManagerConfig::ConfigItem>& animeMap)
{
    auto& windowAnimationConfig = WindowNodeContainer::GetAnimationConfigRef().windowAnimationConfig_;
    if (animeMap.count("timing") && animeMap.at("timing").IsMap()) {
        const auto& timingMap = *animeMap.at("timing").mapValue_;
        if (timingMap.count("duration") && timingMap.at("duration").IsInts()) {
            auto numbers = *timingMap.at("duration").intsValue_;
            if (numbers.size() == 1) { // duration
                windowAnimationConfig.animationTiming_.timingProtocol_ =
                    RSAnimationTimingProtocol(numbers[0]);
            }
        }
        windowAnimationConfig.animationTiming_.timingCurve_ = CreateCurve(timingMap);
    }
    if (animeMap.count("scale") && animeMap.at("scale").IsFloats()) {
        auto numbers = *animeMap.at("scale").floatsValue_;
        if (numbers.size() == 1) { // 1 xy scale
            windowAnimationConfig.scale_.x_ =
            windowAnimationConfig.scale_.y_ = numbers[0]; // 0 xy scale
        } else if (numbers.size() == 2) { // 2 x,y sclae
            windowAnimationConfig.scale_.x_ = numbers[0]; // 0 x scale
            windowAnimationConfig.scale_.y_ = numbers[1]; // 1 y scale
        } else if (numbers.size() == 3) { // 3 x,y,z scale
            windowAnimationConfig.scale_ = Vector3f(&numbers[0]);
        }
    }
    if (animeMap.count("rotation") && animeMap.at("rotation").IsFloats()) {
        auto numbers = *animeMap.at("rotation").floatsValue_;
        if (numbers.size() == 4) { // 4 (axix,angle)
            windowAnimationConfig.rotation_ = Vector4f(&numbers[0]);
        }
    }
    if (animeMap.count("translate") && animeMap.at("translate").IsFloats()) {
        auto numbers = *animeMap.at("translate").floatsValue_;
        if (numbers.size() == 2) { // 2 translate xy
            windowAnimationConfig.translate_.x_ = numbers[0]; // 0 translate x
            windowAnimationConfig.translate_.y_ = numbers[1]; // 1 translate y
        } else if (numbers.size() == 3) { // 3 translate xyz
            windowAnimationConfig.translate_.x_ = numbers[0]; // 0 translate x
            windowAnimationConfig.translate_.y_ = numbers[1]; // 1 translate y
            windowAnimationConfig.translate_.z_ = numbers[2]; // 2 translate z
        }
    }
    if (animeMap.count("opacity") && animeMap.at("opacity").IsFloats()) {
        auto numbers = *animeMap.at("opacity").floatsValue_;
        if (numbers.size() == 1) {
            windowAnimationConfig.opacity_ = numbers[0]; // 0 opacity
        }
    }
}

void WindowManagerService::ConfigKeyboardAnimation(const std::map<std::string,
    WindowManagerConfig::ConfigItem>& animeMap)
{
    auto& animationConfig = WindowNodeContainer::GetAnimationConfigRef();
    if (animeMap.count("timing") && animeMap.at("timing").IsMap()) {
        const auto& timingMap = *animeMap.at("timing").mapValue_;
        if (timingMap.count("durationIn") && timingMap.at("durationIn").IsInts()) {
            auto numbers = *timingMap.at("durationIn").intsValue_;
            if (numbers.size() == 1) { // duration
                animationConfig.keyboardAnimationConfig_.durationIn_ =
                    RSAnimationTimingProtocol(numbers[0]);
            }
        }
        if (timingMap.count("durationOut") && timingMap.at("durationOut").IsInts()) {
            auto numbers = *timingMap.at("durationOut").intsValue_;
            if (numbers.size() == 1) { // duration
                animationConfig.keyboardAnimationConfig_.durationOut_ =
                    RSAnimationTimingProtocol(numbers[0]);
            }
        }
        animationConfig.keyboardAnimationConfig_.curve_ = CreateCurve(timingMap);
    }
}

RSAnimationTimingCurve WindowManagerService::CreateCurve(
    const std::map<std::string, WindowManagerConfig::ConfigItem>& timingMap)
{
    if (timingMap.count("curve") && timingMap.at("curve").property_) {
        auto curveProp = *timingMap.at("curve").property_;
        std::string name;
        if (curveProp.count("name") && curveProp.at("name").IsString()) {
                name = curveProp.at("name").stringValue_;
        }
        if (name == "easeOut") {
            return RSAnimationTimingCurve::EASE_OUT;
        } else if (name == "ease") {
            return RSAnimationTimingCurve::EASE;
        } else if (name == "easeIn") {
            return RSAnimationTimingCurve::EASE_IN;
        } else if (name == "easeInOut") {
            return RSAnimationTimingCurve::EASE_IN_OUT;
        } else if (name == "default") {
            return RSAnimationTimingCurve::DEFAULT;
        } else if (name == "linear") {
            return RSAnimationTimingCurve::LINEAR;
        } else if (name == "spring") {
            return RSAnimationTimingCurve::SPRING;
        } else if (name == "interactiveSpring") {
            return RSAnimationTimingCurve::INTERACTIVE_SPRING;
        } else if (name == "cubic" && timingMap.at("curve").IsFloats() &&
            timingMap.at("curve").floatsValue_->size() == 4) { // 4 curve parameter
            auto numbers = *timingMap.at("curve").floatsValue_;
            return RSAnimationTimingCurve::CreateCubicCurve(numbers[0], // 0 ctrlX1
                numbers[1], // 1 ctrlY1
                numbers[2], // 2 ctrlX2
                numbers[3]); // 3 ctrlY2
        }
    }
    return RSAnimationTimingCurve::EASE_OUT;
}

void WindowManagerService::OnStop()
{
    windowCommonEvent_->UnSubscriberEvent();
    WindowInnerManager::GetInstance().Stop();
    WLOGFI("ready to stop service.");
}

WMError WindowManagerService::NotifyWindowTransition(
    sptr<WindowTransitionInfo>& fromInfo, sptr<WindowTransitionInfo>& toInfo, bool isFromClient)
{
    if (!isFromClient) {
        WLOGFI("NotifyWindowTransition asynchronously.");
        PostAsyncTask([this, fromInfo, toInfo]() mutable {
            return windowController_->NotifyWindowTransition(fromInfo, toInfo);
        });
        return WMError::WM_OK;
    } else {
        WLOGFI("NotifyWindowTransition synchronously.");
        return PostSyncTask([this, &fromInfo, &toInfo]() {
            return windowController_->NotifyWindowTransition(fromInfo, toInfo);
        });
    }
}

WMError WindowManagerService::GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken)
{
    return PostSyncTask([this, &abilityToken]() {
        return windowController_->GetFocusWindowInfo(abilityToken);
    });
}

void WindowManagerService::StartingWindow(sptr<WindowTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap,
    bool isColdStart, uint32_t bkgColor)
{
    if (!startingOpen_) {
        WLOGFI("startingWindow not open!");
        return;
    }
    PostAsyncTask([this, info, pixelMap, isColdStart, bkgColor]() {
        windowController_->StartingWindow(info, pixelMap, bkgColor, isColdStart);
    });
}

void WindowManagerService::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("begin CancelStartingWindow!");
    if (!startingOpen_) {
        WLOGFI("startingWindow not open!");
        return;
    }
    PostAsyncTask([this, abilityToken]() {
        windowController_->CancelStartingWindow(abilityToken);
    });
}

WMError WindowManagerService::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token)
{
    if (window == nullptr || property == nullptr || surfaceNode == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    if ((!window) || (!window->AsObject())) {
        WLOGFE("failed to get window agent");
        return WMError::WM_ERROR_NULLPTR;
    }
    int pid = IPCSkeleton::GetCallingPid();
    int uid = IPCSkeleton::GetCallingUid();
    WMError ret = PostSyncTask([this, pid, uid, &window, &property, &surfaceNode, &windowId, &token]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:CreateWindow(%u)", windowId);
        return windowController_->CreateWindow(window, property, surfaceNode, windowId, token, pid, uid);
    });
    accessTokenIdMaps_.insert(std::pair(windowId, IPCSkeleton::GetCallingTokenID()));
    return ret;
}

WMError WindowManagerService::AddWindow(sptr<WindowProperty>& property)
{
    return PostSyncTask([this, &property]() {
        if (property == nullptr) {
            WLOGFE("property is nullptr");
            return WMError::WM_ERROR_NULLPTR;
        }
        Rect rect = property->GetRequestRect();
        uint32_t windowId = property->GetWindowId();
        WLOGFI("[WMS] Add: %{public}5d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
            "%{public}4d %{public}4d]", windowId, property->GetWindowType(), property->GetWindowMode(),
            property->GetWindowFlags(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:AddWindow(%u)", windowId);
        WMError res = windowController_->AddWindowNode(property);
        if (property->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
            dragController_->StartDrag(windowId);
        }
        return res;
    });
}

WMError WindowManagerService::RemoveWindow(uint32_t windowId)
{
    return PostSyncTask([this, windowId]() {
        WLOGFI("[WMS] Remove: %{public}u", windowId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:RemoveWindow(%u)", windowId);
        WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
        WMError res = windowController_->RecoverInputEventToClient(windowId);
        if (res != WMError::WM_OK) {
            return res;
        }
        return windowController_->RemoveWindowNode(windowId);
    });
}

WMError WindowManagerService::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    if (!accessTokenIdMaps_.isExistAndRemove(windowId, IPCSkeleton::GetCallingTokenID())) {
        WLOGFI("Operation rejected");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    return PostSyncTask([this, windowId, onlySelf]() {
        WLOGFI("[WMS] Destroy: %{public}u", windowId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:DestroyWindow(%u)", windowId);
        WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
        auto node = windowRoot_->GetWindowNode(windowId);
        if (node != nullptr && node->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
            dragController_->FinishDrag(windowId);
        }
        return windowController_->DestroyWindow(windowId, onlySelf);
    });
}

WMError WindowManagerService::RequestFocus(uint32_t windowId)
{
    return PostSyncTask([this, windowId]() {
        WLOGFI("[WMS] RequestFocus: %{public}u", windowId);
        return windowController_->RequestFocus(windowId);
    });
}

AvoidArea WindowManagerService::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    return PostSyncTask([this, windowId, avoidAreaType]() {
        WLOGFI("[WMS] GetAvoidAreaByType: %{public}u, Type: %{public}u", windowId,
            static_cast<uint32_t>(avoidAreaType));
        return windowController_->GetAvoidAreaByType(windowId, avoidAreaType);
    });
}

void WindowManagerService::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return;
    }
    PostVoidSyncTask([this, &windowManagerAgent, type]() {
        WindowManagerAgentController::GetInstance().RegisterWindowManagerAgent(windowManagerAgent, type);
        if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR) { // if system bar, notify once
            windowController_->NotifySystemBarTints();
        }
    });
}

void WindowManagerService::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return;
    }
    PostVoidSyncTask([this, &windowManagerAgent, type]() {
        WindowManagerAgentController::GetInstance().UnregisterWindowManagerAgent(windowManagerAgent, type);
    });
}

WMError WindowManagerService::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: Failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    sptr<AgentDeathRecipient> deathRecipient = new AgentDeathRecipient(
        [this](sptr<IRemoteObject>& remoteObject) {
            PostVoidSyncTask([&remoteObject]() {
                RemoteAnimation::OnRemoteDie(remoteObject);
            });
        }
    );
    controller->AsObject()->AddDeathRecipient(deathRecipient);
    return PostSyncTask([this, &controller]() {
        return windowController_->SetWindowAnimationController(controller);
    });
}

void WindowManagerService::OnWindowEvent(Event event, const sptr<IRemoteObject>& remoteObject)
{
    if (event == Event::REMOTE_DIED) {
        PostVoidSyncTask([this, &remoteObject, event]() {
            uint32_t windowId = windowRoot_->GetWindowIdByObject(remoteObject);
            auto node = windowRoot_->GetWindowNode(windowId);
            if (node != nullptr && node->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
                dragController_->FinishDrag(windowId);
            }
            windowController_->DestroyWindow(windowId, true);
        });
    }
}

void WindowManagerService::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:NotifyDisplayStateChange(%u)", type);
    DisplayId displayId = (displayInfo == nullptr) ? DISPLAY_ID_INVALID : displayInfo->GetDisplayId();
    if (type == DisplayStateChangeType::FREEZE) {
        freezeDisplayController_->FreezeDisplay(displayId);
    } else if (type == DisplayStateChangeType::UNFREEZE) {
        freezeDisplayController_->UnfreezeDisplay(displayId);
    } else {
        PostAsyncTask([this, defaultDisplayId, displayInfo, displayInfoMap, type]() mutable {
            windowController_->NotifyDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
        });
    }
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WindowManagerService::GetInstance().NotifyDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

void DisplayChangeListener::OnGetWindowPreferredOrientation(DisplayId displayId, Orientation &orientation)
{
    WindowManagerService::GetInstance().GetWindowPreferredOrientation(displayId, orientation);
}

void DisplayChangeListener::OnScreenshot(DisplayId displayId)
{
    WindowManagerService::GetInstance().OnScreenshot(displayId);
}

void WindowManagerService::ProcessPointDown(uint32_t windowId, sptr<WindowProperty>& windowProperty,
    sptr<MoveDragProperty>& moveDragProperty)
{
    if (windowProperty == nullptr || moveDragProperty == nullptr) {
        WLOGFE("windowProperty or moveDragProperty is invalid");
        return;
    }

    PostAsyncTask([this, windowId, windowProperty, moveDragProperty]() mutable {
        if (moveDragProperty->startDragFlag_ || moveDragProperty->startMoveFlag_) {
            bool res = WindowInnerManager::GetInstance().NotifyWindowReadyToMoveOrDrag(windowId,
                windowProperty, moveDragProperty);
            if (!res) {
                WLOGFE("invalid operation");
                return;
            }
            windowController_->InterceptInputEventToServer(windowId);
        }
        windowController_->ProcessPointDown(windowId, moveDragProperty);
    });
}

void WindowManagerService::ProcessPointUp(uint32_t windowId)
{
    PostAsyncTask([this, windowId]() {
        WindowInnerManager::GetInstance().NotifyWindowEndUpMovingOrDragging(windowId);
        windowController_->RecoverInputEventToClient(windowId);
        windowController_->ProcessPointUp(windowId);
    });
}

void WindowManagerService::NotifyWindowClientPointUp(uint32_t windowId,
    const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    PostAsyncTask([this, windowId, pointerEvent]() mutable {
        windowController_->NotifyWindowClientPointUp(windowId, pointerEvent);
    });
}

void WindowManagerService::MinimizeAllAppWindows(DisplayId displayId)
{
    PostAsyncTask([this, displayId]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:MinimizeAllAppWindows(%" PRIu64")", displayId);
        WLOGFI("displayId %{public}" PRIu64"", displayId);
        windowController_->MinimizeAllAppWindows(displayId);
    });
}

WMError WindowManagerService::ToggleShownStateForAllAppWindows()
{
    PostAsyncTask([this]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:ToggleShownStateForAllAppWindows");
        return windowController_->ToggleShownStateForAllAppWindows();
    });
    return WMError::WM_OK;
}

WMError WindowManagerService::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return PostSyncTask([this, &topWinId, mainWinId]() {
        return windowController_->GetTopWindowId(mainWinId, topWinId);
    });
}

WMError WindowManagerService::SetWindowLayoutMode(WindowLayoutMode mode)
{
    return PostSyncTask([this, mode]() {
        WLOGFI("layoutMode: %{public}u", mode);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:SetWindowLayoutMode");
        return windowController_->SetWindowLayoutMode(mode);
    });
}

WMError WindowManagerService::UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action)
{
    if (windowProperty == nullptr) {
        WLOGFE("windowProperty is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (action == PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY) {
        PostAsyncTask([this, windowProperty, action]() mutable {
            windowController_->UpdateProperty(windowProperty, action);
        });
        return WMError::WM_OK;
    }
    PostAsyncTask([this, windowProperty, action]() mutable {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:UpdateProperty");
        WMError res = windowController_->UpdateProperty(windowProperty, action);
        if (action == PropertyChangeAction::ACTION_UPDATE_RECT && res == WMError::WM_OK &&
            windowProperty->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
            dragController_->UpdateDragInfo(windowProperty->GetWindowId());
        }
    });
    return WMError::WM_OK;
}

WMError WindowManagerService::GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo)
{
    if (windowInfo == nullptr) {
        WLOGFE("windowInfo is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    return PostSyncTask([this, &windowInfo]() {
        return windowController_->GetAccessibilityWindowInfo(windowInfo);
    });
}

WMError WindowManagerService::GetSystemConfig(SystemConfig& systemConfig)
{
    systemConfig.isSystemDecorEnable_ = systemConfig_.isSystemDecorEnable_;
    systemConfig.isStretchable_ = systemConfig_.isStretchable_;
    return WMError::WM_OK;
}

WMError WindowManagerService::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    if (!hotZonesConfig_.isModeChangeHotZoneConfigured_) {
        return WMError::WM_DO_NOTHING;
    }

    return windowController_->GetModeChangeHotZones(displayId, hotZones, hotZonesConfig_);
}

void WindowManagerService::MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
    sptr<RSIWindowAnimationFinishedCallback>& finishCallback)
{
    PostVoidSyncTask([this, windowIds, isAnimated, &finishCallback]() mutable {
        windowController_->MinimizeWindowsByLauncher(windowIds, isAnimated, finishCallback);
    });
}

void WindowManagerService::GetWindowPreferredOrientation(DisplayId displayId, Orientation &orientation)
{
    PostVoidSyncTask([this, displayId, &orientation]() mutable {
        orientation = windowController_->GetWindowPreferredOrientation(displayId);
    });
}

WMError WindowManagerService::UpdateAvoidAreaListener(uint32_t windowId, bool haveAvoidAreaListener)
{
    return PostSyncTask([this, windowId, haveAvoidAreaListener]() {
        sptr<WindowNode> node = windowRoot_->GetWindowNode(windowId);
        if (node == nullptr) {
            WLOGFE("get window node failed. win %{public}u", windowId);
            return WMError::WM_DO_NOTHING;
        }
        sptr<WindowNodeContainer> container = windowRoot_->GetWindowNodeContainer(node->GetDisplayId());
        if (container == nullptr) {
            WLOGFE("get container failed. win %{public}u display %{public}" PRIu64"", windowId, node->GetDisplayId());
            return WMError::WM_DO_NOTHING;
        }
        container->UpdateAvoidAreaListener(node, haveAvoidAreaListener);
        return WMError::WM_OK;
    });
}

WMError WindowManagerService::UpdateRsTree(uint32_t windowId, bool isAdd)
{
    return PostSyncTask([this, windowId, isAdd]() {
        return windowRoot_->UpdateRsTree(windowId, isAdd);
    });
}

void WindowManagerService::OnScreenshot(DisplayId displayId)
{
    PostAsyncTask([this, displayId]() {
        windowController_->OnScreenshot(displayId);
    });
}

WMError WindowManagerService::BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken)
{
    return PostSyncTask([this, &windowId, targetToken]() {
        return windowController_->BindDialogTarget(windowId, targetToken);
    });
}

void WindowManagerService::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    PostVoidSyncTask([this, displayId, &hasPrivateWindow]() mutable {
        hasPrivateWindow = windowRoot_->HasPrivateWindow(displayId);
    });
    WLOGFI("called %{public}u", hasPrivateWindow);
}

void WindowInfoQueriedListener::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    WLOGFI("called");
    WindowManagerService::GetInstance().HasPrivateWindow(displayId, hasPrivateWindow);
}
} // namespace Rosen
} // namespace OHOS
