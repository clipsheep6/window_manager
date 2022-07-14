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

#include "window_input_channel.h"
#include <input_method_controller.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInputChannel"};
}
WindowInputChannel::WindowInputChannel(const sptr<Window>& window): window_(window), isAvailable_(true)
{
}

WindowInputChannel::~WindowInputChannel()
{
    WLOGFI("windowName: %{public}s, windowId: %{public}d", window_->GetWindowName().c_str(), window_->GetWindowId());
    window_->SetNeedRemoveWindowInputChannel(false);
}

void WindowInputChannel::HandleKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }
    WLOGFI("Receive key event, windowId: %{public}u, keyCode: %{public}d",
        window_->GetWindowId(), keyEvent->GetKeyCode());
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    bool inputMethodHasProcessed = false;
    if (isKeyboardEvent) {
        WLOGFI("dispatch keyEvent to input method");
        inputMethodHasProcessed = MiscServices::InputMethodController::GetInstance()->dispatchKeyEvent(keyEvent);
    }
    if (!inputMethodHasProcessed) {
        WLOGFI("dispatch keyEvent to ACE");
        if (inputListener_ != nullptr) {
            inputListener_->OnInputEvent(keyEvent);
            return;
        }
        window_->ConsumeKeyEvent(keyEvent);
    }
    keyEvent->MarkProcessed();
}

void WindowInputChannel::HandlePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is nullptr");
        return;
    }
    if (inputListener_ != nullptr) {
        // divider window consumes pointer events directly
        if (window_->GetType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            window_->ConsumePointerEvent(pointerEvent);
            inputListener_->OnInputEvent(pointerEvent);
            return;
        }
        int32_t action = pointerEvent->GetPointerAction();
        if (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
            action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
            window_->ConsumePointerEvent(pointerEvent);
        }
        inputListener_->OnInputEvent(pointerEvent);
        return;
    }

    if (!isAvailable_) {
        return;
    }
    if (pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        WLOGFI("Receive move event, windowId: %{public}u, action: %{public}d",
            window_->GetWindowId(), pointerEvent->GetPointerAction());
        window_->ConsumePointerEvent(pointerEvent);
        pointerEvent->MarkProcessed();
    } else {
        WLOGFI("Dispatch non-move event, windowId: %{public}u, action: %{public}d",
            window_->GetWindowId(), pointerEvent->GetPointerAction());
        window_->ConsumePointerEvent(pointerEvent);
        pointerEvent->MarkProcessed();
    }
}

void WindowInputChannel::SetInputListener(const std::shared_ptr<MMI::IInputEventConsumer>& listener)
{
    inputListener_ = listener;
}

void WindowInputChannel::Destroy()
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("Destroy WindowInputChannel, windowId:%{public}u", window_->GetWindowId());
    isAvailable_ = false;
    VsyncStation::GetInstance().RemoveCallback();
}

bool WindowInputChannel::IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    int32_t keyCode = keyEvent->GetKeyCode();
    bool isKeyFN = (keyCode == MMI::KeyEvent::KEYCODE_FN);
    bool isKeyboard = (keyCode >= MMI::KeyEvent::KEYCODE_0 && keyCode <= MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    WLOGFI("isKeyFN: %{public}d, isKeyboard: %{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard);
}
}
}