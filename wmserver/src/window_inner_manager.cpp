/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_inner_manager.h"

#include "ui_service_mgr_client.h"
#include "window_manager_hilog.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInnerManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowInnerManager)

WindowInnerManager::WindowInnerManager() : eventHandler_(nullptr), eventLoop_(nullptr),
    state_(InnerWMRunningState::STATE_NOT_START)
{
}

WindowInnerManager::~WindowInnerManager()
{
    Stop();
}

bool WindowInnerManager::Init(const sptr<WindowController>& windowController)
{
    eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    if (eventLoop_ == nullptr) {
        return false;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        return false;
    }
    dragController_ = new WindowDragController(windowController);
    inputEventRunner_ = AppExecFwk::EventRunner::Create(INNER_WM_INPUT_THREAD_NAME);
    inputEventHandler_ = std::make_shared<AppExecFwk::EventHandler>(inputEventRunner_);
    inputListener_ = std::make_shared<InputEventListener>(InputEventListener());
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(inputListener_, inputEventHandler_);
    WLOGFI("init window inner manager service success.");

    pid_ = getpid();
    return true;
}

void WindowInnerManager::Start(bool enableRecentholder, const sptr<WindowController>& windowController)
{
    isRecentHolderEnable_ = enableRecentholder;
    if (state_ == InnerWMRunningState::STATE_RUNNING) {
        WLOGFI("window inner manager service has already started.");
    }
    if (!Init(windowController)) {
        WLOGFI("failed to init window inner manager service.");
        return;
    }
    state_ = InnerWMRunningState::STATE_RUNNING;
    eventLoop_->Run();
    WLOGFI("window inner manager service start success.");
}

void WindowInnerManager::Stop()
{
    WLOGFI("stop window inner manager service.");
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_.reset();
    }
    if (eventHandler_ != nullptr) {
        eventHandler_.reset();
    }
    state_ = InnerWMRunningState::STATE_NOT_START;
}

void WindowInnerManager::CreateInnerWindow(std::string name, DisplayId displayId, Rect rect,
    WindowType type, WindowMode mode)
{
    eventHandler_->PostTask([this, name, displayId, rect, mode, type]() {
        switch (type) {
            case WindowType::WINDOW_TYPE_PLACEHOLDER: {
                if (isRecentHolderEnable_) {
                    PlaceHolderWindow::GetInstance().Create(name, displayId, rect, mode);
                }
                break;
            }
            case WindowType::WINDOW_TYPE_DOCK_SLICE: {
                DividerWindow::GetInstance().Create(name, displayId, rect, mode);
                break;
            }
            default:
                break;
        }
    });
    return;
}

void WindowInnerManager::DestroyInnerWindow(DisplayId displayId, WindowType type)
{
    eventHandler_->PostTask([this, type]() {
        switch (type) {
            case WindowType::WINDOW_TYPE_PLACEHOLDER: {
                if (isRecentHolderEnable_) {
                    PlaceHolderWindow::GetInstance().Destroy();
                }
                break;
            }
            case WindowType::WINDOW_TYPE_DOCK_SLICE: {
                DividerWindow::GetInstance().Destroy();
                break;
            }
            default:
                break;
        }
    });
    return;
}

void InputEventListener::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return;
    }
    uint32_t windowId = static_cast<uint32_t>(keyEvent->GetAgentWindowId());
    WLOGFI("liuqi 222 Receive keyEvent, windowId: %{public}u", windowId);
    keyEvent->MarkProcessed();
}

void InputEventListener::OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const
{
    if (axisEvent == nullptr) {
        WLOGFE("AxisEvent is nullptr");
        return;
    }
    WLOGFI("liuqi 222 Receive axisEvent, windowId: %{public}d", axisEvent->GetAgentWindowId());
    axisEvent->MarkProcessed();
}

void InputEventListener::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (pointerEvent == nullptr) {
        WLOGFE("PointerEvent is nullptr");
        return;
    }
    uint32_t windowId = static_cast<uint32_t>(pointerEvent->GetAgentWindowId());
    WLOGFI("liuqi 222 Receive pointerEvent, windowId: %{public}u", windowId);
    pointerEvent->MarkProcessed();
}
} // Rosen
} // OHOS