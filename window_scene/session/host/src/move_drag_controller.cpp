/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "session/host/include/move_drag_controller.h"

#include <pointer_event.h>

#include "session/host/include/session_vsync_station.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MoveDragController" };
}

MoveDragController::MoveDragController()
{
    vsyncCallback_->onCallback = std::bind(&MoveDragController::OnReceiveVsync, this, std::placeholders::_1);
}

void MoveDragController::SetVsyncHandleListenser(const NotifyVsyncHandleFunc& func)
{
    vsyncHandleFunc_ = func;
}

void MoveDragController::SetStartMoveFlag(bool flag)
{
    moveProperty_.isStartMove_ = flag;
}

bool MoveDragController::GetStartMoveFlag()
{
    return moveProperty_.isStartMove_;
}

void MoveDragController::SetTargetRect(const WSRect &rect)
{
    moveProperty_.targetRect_ = rect;
}

MoveDragController::MoveProperty MoveDragController::GetMoveProperty() const
{
    return moveProperty_;
}

void MoveDragController::StartMove(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointerId = pointerEvent->GetPointerId();
    pointerEvent->GetPointerItem(pointerId, pointerItem);
    int32_t pointerDisplayX = pointerItem.GetDisplayX();
    int32_t pointerDisplayY = pointerItem.GetDisplayY();
    if (moveProperty_.startPointerDisplayX_ == -1 && moveProperty_.startPointerDisplayY_ == -1) {
        moveProperty_.startPointerId_ = pointerId;
        moveProperty_.startPointerDisplayX_ = pointerDisplayX;
        moveProperty_.startPointerDisplayY_ = pointerDisplayY;
        int32_t pointerWindowX = pointerItem.GetWindowX();
        int32_t pointerWindowY = pointerItem.GetWindowY();
        moveProperty_.startWindowDisplayX_ = pointerDisplayX - pointerWindowX;
        moveProperty_.startWindowDisplayY_ = pointerDisplayY - pointerWindowY;
    } else {
        int32_t offsetX = pointerDisplayX - moveProperty_.startPointerDisplayX_;
        int32_t offsetY = pointerDisplayY - moveProperty_.startPointerDisplayY_;
        moveProperty_.targetRect_ = { moveProperty_.startWindowDisplayX_ + offsetX,
            moveProperty_.startWindowDisplayY_ + offsetY, 0, 0 };
        WLOGFD("move rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
            moveProperty_.targetRect_.posX_, moveProperty_.targetRect_.posY_,
            moveProperty_.targetRect_.width_, moveProperty_.targetRect_.height_);
        RequestVsync();
    }
}

void MoveDragController::EndMove()
{
    WLOGFD("end move and set initialization value");
    moveProperty_ = { false, -1, -1, -1, -1, -1, { 0, 0, 0, 0 } };
}

void MoveDragController::RequestVsync()
{
    VsyncStation::GetInstance().RequestVsync(vsyncCallback_);
}

void MoveDragController::RemoveVsync()
{
    VsyncStation::GetInstance().RemoveCallback();
}

void MoveDragController::OnReceiveVsync(int64_t timeStamp)
{
    WLOGFD("[OnReceiveVsync] receive event, time: %{public}" PRId64"", timeStamp);
    if (vsyncHandleFunc_) {
        vsyncHandleFunc_();
    }
}
} // namespace OHOS::Rosen
