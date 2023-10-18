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
#include "window_manager_hilog.h"
#include "window_option.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureController"};
}

PictureInPictureController::PictureInPictureController() : weakRef_(this)
{
    this->handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
}

PictureInPictureController::~PictureInPictureController()
{
}

bool PictureInPictureController::CreatePictureInPictureWindow()
{
    WLOGI("CreatePictureInPictureWindow is called");
    sptr<PictureInPictureController> thisController = this;
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        WLOGFE("Get option failed");
        return false;
    }
    winRect_.width_ = 800;
    winRect_.height_ = 600;
    windowOption->SetWindowName("pip_window");
    windowOption->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    windowOption->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    windowOption->SetWindowRect(winRect_);
    WMError errCode;
    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption, context->lock(), errCode);
    if (window == nullptr || WMError != WMError::WM_OK) {
        WLOGFE("Window::Create failed, reason: %{public}d", errCode);
        return false;
    }
    window_ = window;
    window_->SetCornerRadius(winCorner_);
    PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), thisController);
    return true;
}

bool PictureInPictureController::ShowPictureInPictureWindow()
{
    WLOGI("ShowPictureInPictureWindow is called");
    if (window_ == nullptr) {
        WLOGFD("window_ is nullptr");
        return false;
    }
    PictureInPictureManager::SetCurrentPipController(this);
    WLOGFD("ShowPictureInPictureWindow success");
    return true;
}

bool PictureInPictureController::StartPictureInPicture()
{
    WLOGI("StartPictureInPicture is called");
    sptr<PictureInPictureController> thisController = this;
    if (PictureInPictureManager::GetPipWindowState() == PipWindowState::STATE_STARTING) {
        WLOGFE("Pip window is starting");
        return false;
    }
    if (PictureInPictureManager::IsControlledNow()) {
        // pip window exists
        if (PictureInPictureManager::IsCurrentController(weakRef_)) {
            WLOGFE("Repeat start request");
            return false;
        }
        if (PictureInPictureManager::IsCurrentWindow(mainWindowId_)) {
            PictureInPictureManager::RemoveCurrentController();
            PictureInPictureManager::SetCurrentController(thisController);
            return ShowPictureInPictureWindow();
        }
        PictureInPictureManager::DoClose();
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTING);
    if (!CreatePictureInPictureWindow()) {
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
        WLOGFE("Create pip window failed");
        return false;
    }
    if (!ShowPictureInPictureWindow()) {
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
        WLOGFE("Show pip window failed");
        return false;
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTED);
    return true;
}

bool PictureInPictureController::StopPictureInPicture(bool needRestore)
{
    WLOGI("StopPictureInPicture is called, needRestore: %{public}u", needRestore);
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return false;
    }
    if (PictureInPictureManager::GetPipWindowState() == PipWindowState::STATE_STOPPING) {
        WLOGFE("Repeat stop request");
        return false;
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPING);
    auto task =
        [this]() {
            if (window_->Destroy() != WMError::WM_OK) {
                PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
                WLOGFE("Window destroy failed");
                return;
            }
            PictureInPictureManager::RemoveCurrentController();
            PictureInPictureManager::DestroyPipController(window_->GetWindowId());
            window_ = nullptr;
            PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPED);
        };
    if (handler_ && !isFromRestore) {
        WLOGFD("Window destroy async");
        handler_->PostTask(task, "pip_controller_stop_window", 400);
    } else {
        WLOGFD("Window destroy sync");
        task();
    }
    return true;
}

void PictureInPictureController::UpdateDisplaySize(uint32_t width, uint32_t height)
{
    WLOGI("UpdateDisplaySize is called");
    return;
}
} // namespace Rosen
} // namespace OHOS