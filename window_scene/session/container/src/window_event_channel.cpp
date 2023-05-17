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

#include "session/container/include/window_event_channel.h"

#include <axis_event.h>
#include <key_event.h>
#include <pointer_event.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannel" };
}

WMError WindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    WLOGFD("WindowEventChannel receive key event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WMError::WM_ERROR_NULLPTR;
    }
    sessionStage_->NotifyKeyEvent(keyEvent);
    return WMError::WM_OK;
}

WMError WindowEventChannel::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("WindowEventChannel receive pointer event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WMError::WM_ERROR_NULLPTR;
    }
    sessionStage_->NotifyPointerEvent(pointerEvent);
    return WMError::WM_OK;
}
} // namespace OHOS::Rosen
