/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "picture_in_picture_controller.h"

#include <event_handler.h>
#include <refbase.h>
#include "parameters.h"
#include <power_mgr_client.h>
#include "picture_in_picture_manager.h"
#include "picture_in_picture_option.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureController"};
    constexpr int32_t DELAY_ANIM = 500;
    const std::string PROP_DEFAULT_PAGE_NAME = "const.window.pip.debug.useDefaultPage";
    const std::string DEFAULT_PAGE_PATH = "pages/pipwindow/PiPWindow";
}

PictureInPictureController::PictureInPictureController(sptr<PipOption> pipOption, uint32_t windowId, napi_env env)
    : weakRef_(this), pipOption_(pipOption), mainWindowId_(windowId), env_(env)
{
    this->handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
}

PictureInPictureController::~PictureInPictureController()
{
}

WMError PictureInPictureController::CreatePictureInPictureWindow()
{
    WLOGI("CreatePictureInPictureWindow is called");
    sptr<PictureInPictureController> thisController = this;

    if (pipOption_ == nullptr || pipOption_->GetContext() == nullptr) {
        WLOGFE("Get PictureInPictureOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(pipOption_->GetContext());
    sptr<Window> callWindow = Window::GetTopWindowWithContext(context->lock());
    if (callWindow == nullptr) {
        WLOGFE("Get call Window failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    mainWindowId_ = callWindow->GetWindowId();

    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        WLOGFE("Get WindowOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    windowOption->SetWindowName(PIP_WINDOW_NAME);
    windowOption->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    windowOption->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    windowOption->SetWindowRect(windowRect_);
    WMError errCode;
    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption, context->lock(), errCode);
    if (window == nullptr || errCode != WMError::WM_OK) {
        WLOGFE("Window create failed, reason: %{public}d", errCode);
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    window_ = window;
    PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), thisController);
    return WMError::WM_OK;
}

WMError PictureInPictureController::ShowPictureInPictureWindow()
{
    WLOGI("ShowPictureInPictureWindow is called");
    if (window_ == nullptr) {
        WLOGFD("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStart();
    }
    bool useDefaultPage = system::GetParameter(PROP_DEFAULT_PAGE_NAME, "") != "";
    if (useDefaultPage && env_ != nullptr) {
        window_->NapiSetUIContent(DEFAULT_PAGE_PATH, env_, nullptr, false, nullptr);
    }
    WMError errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        WLOGFD("window_ show failed");
        int32_t err = static_cast<int32_t>(errCode);
        if (pipLifeCycleListener_ != nullptr) {
            pipLifeCycleListener_->OnPictureInPictureOperationError(err);
        }
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    PictureInPictureManager::SetCurrentPipController(this);
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPictureInPictureStart();
    }
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture()
{
    WLOGI("StartPictureInPicture is called");
    sptr<PictureInPictureController> thisController = this;
    if (PictureInPictureManager::GetPipWindowState() == PipWindowState::STATE_STARTING) {
        WLOGFE("Pip window is starting");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (PictureInPictureManager::IsCurrentPipControllerExist()) {
        // pip window exists
        if (PictureInPictureManager::IsCurrentPipController(weakRef_)) {
            WLOGFE("Repeat start request");
            return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
        }
        if (PictureInPictureManager::IsAttachedPipWindow(mainWindowId_)) {
            window_ = PictureInPictureManager::GetCurrentWindow();
            PictureInPictureManager::RemoveCurrentPipController();
            PictureInPictureManager::SetCurrentPipController(thisController);
            return ShowPictureInPictureWindow();
        }
        PictureInPictureManager::DoClose(false);
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTING);
    WMError errCode = CreatePictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
        WLOGFE("Create pip window failed");
        return errCode;
    }
    errCode = ShowPictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
        WLOGFE("Show pip window failed");
        return errCode;
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTED);
    return WMError::WM_OK;
}

WMError PictureInPictureController::StopPictureInPicture(bool needAnim)
{
    WLOGI("StopPictureInPicture is called, needAnim: %{public}u", needAnim);
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStop();
    }
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (PictureInPictureManager::GetPipWindowState() == PipWindowState::STATE_STOPPING) {
        WLOGFE("Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPING);
    window_->NotifyPrepareClosePiPWindow();
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
        }
        WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(session->window_->Destroy());
        if (ret != WmErrorCode::WM_OK) {
            PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
            WLOGFE("Window destroy failed");
            int32_t err = static_cast<int32_t>(ret);
            if (session->pipLifeCycleListener_ != nullptr) {
                session->pipLifeCycleListener_->OnPictureInPictureOperationError(err);
            }
            return WMError::WM_ERROR_PIP_DESTROY_FAILED;
        }
        if (session->pipLifeCycleListener_ != nullptr) {
            session->pipLifeCycleListener_->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveCurrentPipController();
        PictureInPictureManager::RemovePipControllerInfo(session->window_->GetWindowId());
        session->window_ = nullptr;
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPED);
        return WMError::WM_OK;
    };
    if (handler_ && needAnim) {
        handler_->PostTask(task, "StopPictureInPicture", DEFAULT_TIME_DELAY);
    } else {
        return task();
    }
    return WMError::WM_OK;
}

sptr<Window> PictureInPictureController::GetPipWindow()
{
    WLOGFD("GetPipWindow is called");
    return window_;
}

uint32_t PictureInPictureController::GetMainWindowId()
{
    WLOGFD("GetMainWindowId is called");
    return mainWindowId_;
}

void PictureInPictureController::SetPipWindow(sptr<Window> window)
{
    WLOGFD("SetPipWindow is called");
    window_ = window;
}

void PictureInPictureController::SetAutoStartEnabled(bool enable)
{
    isAutoStartEnabled_ = enable;
    if (isAutoStartEnabled_) {
        PictureInPictureManager::AttachActivePipController(this);
    } else {
        PictureInPictureManager::DetachActivePipController(this);
    }
}

void PictureInPictureController::IsAutoStartEnabled(bool& enable) const
{
    enable = isAutoStartEnabled_;
}

void PictureInPictureController::UpdateContentSize(uint32_t width, uint32_t height)
{
    WLOGI("UpdateContentSize is called");
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    window_->UpdatePiPRect(width, height, PiPRectUpdateReason::REASON_PIP_VIDEO_RATIO_CHANGE);
}

void PictureInPictureController::StartMove()
{
    WLOGI("StartMove is called");
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    window_->StartMove();
}

void PictureInPictureController::DoScale()
{
    WLOGI("DoScale is called");
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    window_->UpdatePiPRect(0, 0, PiPRectUpdateReason::REASON_PIP_SCALE_CHANGE);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::AfterBackground()
{
    WLOGI("PipMainWindowLifeCycleImpl AfterBackground is called");
    if (!PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGFE("screen is off");
        return;
    }
    PictureInPictureManager::AutoStartPipWindow();
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::BackgroundFailed(int32_t type)
{
    WLOGI("PipMainWindowLifeCycleImpl BackgroundFailed is called");
}

void PictureInPictureController::DoActionEvent(std::string& actionName)
{
    WLOGFD("actionName: %{public}s", actionName.c_str());
    if (pipActionObserver_ == nullptr) {
        WLOGFE("pipActionObserver is not registered");
        return;
    }
    pipActionObserver_->OnActionEvent(actionName);
}

void PictureInPictureController::RestorePictureInPictureWindow()
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->RecoveryPullPiPMainWindow();
    auto stopPipTask = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return;
        }
        session->StopPictureInPicture(false);
    };
    if (handler_ == nullptr) {
        WLOGFE("handler is nullptr");
        return;
    }
    handler_->PostTask(stopPipTask, DELAY_ANIM);
    WLOGFI("restore pip main window finished");
}

void PictureInPictureController::SetPictureInPictureLifecycle(sptr<IPiPLifeCycle> listener)
{
    WLOGFD("SetPictureInPictureLifecycle is called");
    pipLifeCycleListener_ = listener;
}

void PictureInPictureController::SetPictureInPictureActionObserver(sptr<IPiPActionObserver> listener)
{
    WLOGFD("SetPictureInPictureActionObserver is called");
    pipActionObserver_ = listener;
}

sptr<IPiPLifeCycle> PictureInPictureController::GetPictureInPictureLifecycle() const
{
    return pipLifeCycleListener_;
}

sptr<IPiPActionObserver> PictureInPictureController::GetPictureInPictureActionObserver() const
{
    return pipActionObserver_;
}
} // namespace Rosen
} // namespace OHOS