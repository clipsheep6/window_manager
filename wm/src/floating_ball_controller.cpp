/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "floating_ball_controller.h"

#include <refbase.h>
#include <transaction/rs_sync_transaction_controller.h>
#include "parameters.h"
#include "singleton_container.h"
#include "window_adapter.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "floating_ball_manager.h"

namespace OHOS {
namespace Rosen {

namespace {
const std::string FB_WINDOW_NAME = "fb_window";
}

FloatingBallController::FloatingBallController(const sptr<Window>& mainWindow,
    const uint32_t& windowId, void* contextPtr)
    : weakRef_(this), mainWindow_(mainWindow), mainWindowId_(windowId), contextPtr_(contextPtr)
{
    curState_ = FbWindowState::STATE_UNDEFINED;
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr_);
    if (context == nullptr) {
        return;
    }
    const std::shared_ptr<AbilityRuntime::Context>& abilityContext = context->lock();
    if ((abilityContext == nullptr) || (abilityContext->GetApplicationInfo() == nullptr)) {
        return;
    }
    SingletonContainer::Get<FloatingballReporter>().SetCurrentPackageName(abilityContext->GetApplicationInfo()->name);
}

FloatingBallController::~FloatingBallController()
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "FloatingBallController release");
}

// LCOV_EXCL_START
FbWindowState FloatingBallController::GetControllerState() const
{
    return curState_;
}

void FloatingBallController::UpdateMainWindow(const sptr<Window>& mainWindow)
{
    if (mainWindow == nullptr) {
        return;
    }
    mainWindow_ = mainWindow;
    mainWindowId_ = mainWindow->GetWindowId();
}
// LCOV_EXCL_STOP

WMError FloatingBallController::UpdateFloatingBall(sptr<FbOption>& option)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "UpdateFloatingBall");
    if (curState_ != FbWindowState::STATE_STARTED) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "cur state is not started, can not update");
        SingletonContainer::Get<FloatingballReporter>().ReportFbUpdateContent(templateType_,
            "cur state is not started, can not update");
        return WMError::WM_ERROR_FB_INVALID_STATE;
    }
    if (option == nullptr || window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "option or window is nullptr");
        SingletonContainer::Get<FloatingballReporter>().ReportFbUpdateContent(templateType_,
            "option or window is nullptr");
        return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
    }
    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    auto errCode = window_->UpdateFloatingBall(fbTemplateBaseInfo, option->GetIcon());
    std::ostringstream ss;
    errCode == WMError::WM_OK ? (ss << "") : (ss << "Update floating ball window session failed, errCode:"
        << static_cast<uint32_t>(errCode));
    SingletonContainer::Get<FloatingballReporter>().ReportFbUpdateContent(templateType_, ss.str());
    return errCode;
}

WMError FloatingBallController::StartFloatingBall(sptr<FbOption>& option)
{
    {
        std::lock_guard<std::mutex> lock(controllerMutex_);
        if (FloatingBallManager::HasActiveController() && !FloatingBallManager::IsActiveController(this)) {
            TLOGI(WmsLogTag::WMS_SYSTEM, "OnStartFloatingBall abort");
            return WMError::WM_ERROR_FB_REPEAT_CONTROLLER;
        }
        TLOGI(WmsLogTag::WMS_SYSTEM, "called");
        if (option == nullptr) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "fbOption is null");
            return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
        }
        templateType_ = option->GetTemplate();
        if (curState_ == FbWindowState::STATE_STARTING || curState_ == FbWindowState::STATE_STARTED) {
            TLOGW(WmsLogTag::WMS_SYSTEM, "fbWindow state is: %{public}u, id: %{public}u, mainWindow: %{public}u",
                curState_, (window_ == nullptr) ? INVALID_WINDOW_ID : window_->GetWindowId(), mainWindowId_);
            SingletonContainer::Get<FloatingballReporter>().ReportFbStart(templateType_,
                "fbWindow state is starting or started");
            return WMError::WM_ERROR_FB_REPEAT_OPERATION;
        }
        if (curState_ == FbWindowState::STATE_STOPPING) {
            TLOGW(WmsLogTag::WMS_SYSTEM, "fbWindow state is: %{public}u, id: %{public}u, mainWindow: %{public}u",
                curState_, (window_ == nullptr) ? INVALID_WINDOW_ID : window_->GetWindowId(), mainWindowId_);
            SingletonContainer::Get<FloatingballReporter>().ReportFbStart(templateType_, "fbWindow state is stopping");
            return WMError::WM_ERROR_FB_INVALID_STATE;
        }
        curState_ = FbWindowState::STATE_STARTING;
        FloatingBallManager::SetActiveController(this);
    }
    auto errorCode = StartFloatingBallInner(option);
    if (errorCode != WMError::WM_OK) {
        curState_ = FbWindowState::STATE_UNDEFINED;
        FloatingBallManager::RemoveActiveController(this);
    }
    return errorCode;
}

WMError FloatingBallController::StartFloatingBallInner(const sptr<FbOption>& option)
{
    WMError errCode = CreateFloatingBallWindow(option);
    if (errCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Create fb window failed, err: %{public}u", errCode);
        return errCode;
    }
    // LCOV_EXCL_START
    errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Show fb window failed, err: %{public}u", errCode);
        (void)window_->Destroy();
        return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
    }
    mainWindowLifeCycleListener_ = sptr<FloatingBallController::WindowLifeCycleListener>::MakeSptr();
    mainWindow_->RegisterLifeCycleListener(mainWindowLifeCycleListener_);
    curState_ = FbWindowState::STATE_STARTED;
    SingletonContainer::Get<FloatingballReporter>().ReportFbStart(templateType_, "");
    OnFloatingBallStart();
    return WMError::WM_OK;
    // LCOV_EXCL_STOP
}

void FloatingBallController::WindowLifeCycleListener::AfterDestroyed()
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "fb AfterDestroyed");
    FloatingBallManager::DoDestroy();
}

WMError FloatingBallController::CreateFloatingBallWindow(const sptr<FbOption>& option)
{
    if (option == nullptr || contextPtr_ == nullptr || mainWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Create fb failed, invalid fbOption");
        return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
    }
    // LCOV_EXCL_START
    auto uid = getuid();
    auto mainWindowState = mainWindow_->GetWindowState();
    TLOGI(WmsLogTag::WMS_SYSTEM, "mainWindow:%{public}u, mainWindowState:%{public}u, uid %{public}d",
        mainWindowId_, mainWindowState, uid);
    if (mainWindowState != WindowState::STATE_SHOWN) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "mainWindow:%{public}u is not shown", mainWindowId_);
        return WMError::WM_ERROR_FB_CREATE_FAILED;
    }
    auto windowOption = sptr<WindowOption>::MakeSptr();
    windowOption->SetWindowName(FB_WINDOW_NAME + "_" + std::to_string(uid));
    windowOption->SetWindowType(WindowType::WINDOW_TYPE_FB);
    windowOption->SetWindowMode(WindowMode::WINDOW_MODE_FB);
    windowOption->SetTouchable(false);
    FloatingBallTemplateBaseInfo fbTemplateBaseInfo;
    option->GetFbTemplateBaseInfo(fbTemplateBaseInfo);
    WMError errCode = WMError::WM_OK;
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr_);
    sptr<Window> window = Window::CreateFb(windowOption, fbTemplateBaseInfo, option->GetIcon(),
        context->lock(), errCode);
    if (window == nullptr || errCode != WMError::WM_OK) {
        TLOGW(WmsLogTag::WMS_SYSTEM, "Window create failed, reason: %{public}d", errCode);
        return WMError::WM_ERROR_FB_CREATE_FAILED;
    }
    window_ = window;
    return WMError::WM_OK;
    // LCOV_EXCL_STOP
}

// LCOV_EXCL_START
WMError FloatingBallController::StopFloatingBallFromClient()
{
    {
        std::lock_guard<std::mutex> lock(controllerMutex_);
        TLOGI(WmsLogTag::WMS_SYSTEM, "StopFloatingBallFromClient");
        if (curState_ == FbWindowState::STATE_STOPPING ||
            curState_ == FbWindowState::STATE_STOPPED) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "Repeat stop request, curState: %{public}u", curState_);
            return WMError::WM_ERROR_FB_REPEAT_OPERATION;
        }
        if (window_ == nullptr) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "window is nullptr when stop fb");
            return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
        }
        stopFromClient_ = true;
        curState_ = FbWindowState::STATE_STOPPING;
    }
    window_->NotifyPrepareCloseFloatingBall();
    return WMError::WM_OK;
}

WMError FloatingBallController::StopFloatingBall()
{
    {
        std::lock_guard<std::mutex> lock(controllerMutex_);
        TLOGI(WmsLogTag::WMS_SYSTEM, "StopFloatingBall in");
        if ((!stopFromClient_ && curState_ == FbWindowState::STATE_STOPPING) ||
            curState_ == FbWindowState::STATE_STOPPED) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "Repeat stop request, curState: %{public}u", curState_);
            SingletonContainer::Get<FloatingballReporter>().ReportFbRemove(templateType_,
                "Repeat stop request, curState: stopping or stopped");
            return WMError::WM_ERROR_FB_REPEAT_OPERATION;
        }
        if (window_ == nullptr) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "window is nullptr when stop fb");
            return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
        }
        curState_ = FbWindowState::STATE_STOPPING;
    }
    SingletonContainer::Get<FloatingballReporter>().ReportFbRemove(templateType_, "");
    return DestroyFloatingBallWindow();
}
// LCOV_EXCL_STOP

WMError FloatingBallController::DestroyFloatingBallWindow()
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "called");
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "window is nullptr when destroy fb");
        return WMError::WM_ERROR_FB_INTERNAL_ERROR;
    }
    // LCOV_EXCL_START
    WMError ret = window_->Destroy();
    if (ret != WMError::WM_OK) {
        curState_ = FbWindowState::STATE_UNDEFINED;
        TLOGE(WmsLogTag::WMS_SYSTEM, "window destroy failed, err:%{public}u", ret);
        return WMError::WM_ERROR_FB_INTERNAL_ERROR;
    }
    curState_ = FbWindowState::STATE_STOPPED;
    OnFloatingBallStop();
    FloatingBallManager::RemoveActiveController(weakRef_);
    if (mainWindow_ != nullptr) {
        mainWindow_->UnregisterLifeCycleListener(mainWindowLifeCycleListener_);
    }
    window_ = nullptr;
    stopFromClient_ = false;
    return WMError::WM_OK;
    // LCOV_EXCL_STOP
}

sptr<Window> FloatingBallController::GetFbWindow() const
{
    return window_;
}

WMError FloatingBallController::RestoreMainWindow(const std::shared_ptr<AAFwk::Want>& want)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "restoreMainWindow");
    if (curState_ != FbWindowState::STATE_STARTED) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "state is not started when restore main window");
        SingletonContainer::Get<FloatingballReporter>().ReportFbRestoreMainWindow(templateType_,
            "state is not started when restore main window");
        return WMError::WM_ERROR_FB_INVALID_STATE;
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "window is null when restoreMainWindow");
        SingletonContainer::Get<FloatingballReporter>().ReportFbRestoreMainWindow(templateType_,
            "window is null when restoreMainWindow");
        return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
    }
    SingletonContainer::Get<FloatingballReporter>().ReportFbRestoreMainWindow(templateType_, "");
    return window_->RestoreFbMainWindow(want);
}

// LCOV_EXCL_START
void FloatingBallController::OnFloatingBallClick()
{
    auto fbClickObservers = fbClickObservers_;
    for (auto& listener : fbClickObservers) {
        if (listener == nullptr) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "one click observer is nullptr");
            continue;
        }
        listener->OnClickEvent();
    }
}

void FloatingBallController::OnFloatingBallStart()
{
    auto fbLifeCycleListeners = fbLifeCycleListeners_;
    for (auto& listener : fbLifeCycleListeners) {
        if (listener == nullptr) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "one lifecycle listener is nullptr");
            continue;
        }
        listener->OnFloatingBallStart();
    }
}

void FloatingBallController::OnFloatingBallStop()
{
    auto fbLifeCycleListeners = fbLifeCycleListeners_;
    for (auto& listener : fbLifeCycleListeners) {
        if (listener == nullptr) {
            TLOGE(WmsLogTag::WMS_SYSTEM, "one lifecycle listener is nullptr");
            continue;
        }
        listener->OnFloatingBallStop();
    }
}
// LCOV_EXCL_STOP

WMError FloatingBallController::RegisterFbLifecycle(const sptr<IFbLifeCycle>& listener)
{
    return RegisterListener(fbLifeCycleListeners_, listener);
}

WMError FloatingBallController::RegisterFbClickObserver(const sptr<IFbClickObserver>& listener)
{
    return RegisterListener(fbClickObservers_, listener);
}

WMError FloatingBallController::UnRegisterFbLifecycle(const sptr<IFbLifeCycle>& listener)
{
    return UnRegisterListener(fbLifeCycleListeners_, listener);
}

WMError FloatingBallController::UnRegisterFbClickObserver(const sptr<IFbClickObserver>& listener)
{
    return UnRegisterListener(fbClickObservers_, listener);
}

template<typename T>
WMError FloatingBallController::RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    std::lock_guard<std::mutex> lock(listenerMutex_);
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "listener is nullptr");
        return WMError::WM_ERROR_FB_INTERNAL_ERROR;
    }
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Listener already registered");
        return WMError::WM_OK;
    }
    holder.emplace_back(listener);
    return WMError::WM_OK;
}

template<typename T>
WMError FloatingBallController::UnRegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    std::lock_guard<std::mutex> lock(listenerMutex_);
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "listener could not be null");
        return WMError::WM_ERROR_FB_INTERNAL_ERROR;
    }
    holder.erase(std::remove_if(holder.begin(), holder.end(),
        [listener](const sptr<T>& registeredListener) {
            return registeredListener == listener;
        }), holder.end());
    return WMError::WM_OK;
}

// LCOV_EXCL_START
WMError FloatingBallController::GetFloatingBallWindowInfo(uint32_t& windowId)
{
    if (curState_ != FbWindowState::STATE_STARTED) {
        return WMError::WM_ERROR_FB_INVALID_STATE;
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "window is null");
        return WMError::WM_ERROR_FB_STATE_ABNORMALLY;
    }
    return window_->GetFloatingBallWindowId(windowId);
}
// LCOV_EXCL_STOP

} // namespace Rosen
} // namespace OHOS