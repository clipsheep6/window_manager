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
#include "session/host/include/scene_session.h"

#include <pointer_event.h>

#include "interfaces/include/ws_common.h"
#include "window_manager_hilog.h"
#include "session/host/include/scene_persistent_storage.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession" };
}

SceneSession::SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : Session(info)
{
    GeneratePersistentId(!isExtension, info);
    specificCallback_ = specificCallback;
    moveDragController_ = new (std::nothrow) MoveDragController();
    hotAreaController_ = new (std::nothrow) HotAreaController;
    ProcessVsyncHandleRegister();
    //TODO: info is not always available, so comment this block first
    // std::string key = info.bundleName_ + "_" + info.abilityName_;    WLOGD("wywywy , key %{public}s, value: %{public}f", key.c_str(), aspectRatio_);
    // if (ScenePersistentStorage::HasKey(key, PersistentStorageType::ASPECT_RATIO)) {
    //     ScenePersistentStorage::Get(key, aspectRatio_, PersistentStorageType::ASPECT_RATIO);
    //     hotAreaController_->SetAspectRatio(aspectRatio_);
    // }
}

WSError SceneSession::OnSessionEvent(SessionEvent event)
{
    WLOGFD("SceneSession OnSessionEvent event: %{public}d", static_cast<int32_t>(event));
    if (event == SessionEvent::EVENT_START_MOVE) {
        if (!moveDragController_) {
            WLOGFE("moveDragController_ is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        moveDragController_->SetStartMoveFlag(true);
    }
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnSessionEvent_) {
        sessionChangeCallback_->OnSessionEvent_(static_cast<uint32_t>(event));
    }
    return WSError::WS_OK;
}

void SceneSession::RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>&
    sessionChangeCallback)
{
    sessionChangeCallback_ = sessionChangeCallback;
}

WSError SceneSession::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
    if (hotAreaController_) {
        hotAreaController_->SetAspectRatio(ratio);
    }
    return WSError::WS_OK;
}

WSError SceneSession::UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason)
{
    WLOGFI("UpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_,
        rect.width_, rect.height_);
    SetSessionRect(rect);
    NotifySessionRectChange(rect);
    UpdateRect(rect, reason);
    return WSError::WS_OK;
}

WSError SceneSession::RaiseToAppTop()
{
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRaiseToTop_) {
        sessionChangeCallback_->onRaiseToTop_();
    }
    return WSError::WS_OK;
}

WSError SceneSession::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    WLOGFI("CreateAndConnectSpecificSession id: %{public}" PRIu64 "", GetPersistentId());
    sptr<SceneSession> sceneSession;
    if (specificCallback_ != nullptr) {
        sceneSession = specificCallback_->onCreate_(sessionInfo_);
    }
    if (sceneSession == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    // connect specific session and sessionStage
    WSError errCode = sceneSession->Connect(sessionStage, eventChannel, surfaceNode, systemConfig_, property);
    if (property) {
        persistentId = property->GetPersistentId();
    }
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onCreateSpecificSession_) {
        sessionChangeCallback_->onCreateSpecificSession_(sceneSession);
    }
    session = sceneSession;
    return errCode;
}

WSError SceneSession::DestroyAndDisconnectSpecificSession(const uint64_t& persistentId)
{
    WSError ret = WSError::WS_OK;
    if (specificCallback_ != nullptr) {
        ret = specificCallback_->onDestroy_(persistentId);
    }
    return ret;
}

WSError SceneSession::Background()
{
    Session::Background();
    if (scenePersistence_ != nullptr && GetSnapshot() != nullptr) {
        scenePersistence_->SaveSnapshot(GetSnapshot());
    }
    return WSError::WS_OK;
}

WSError SceneSession::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("SceneSession TransferPointEvent");
    if (!hotAreaController_) {
        WLOGFE("hotAreaController_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    WSRect targetRect;
    if (hotAreaController_->EventConsumer(pointerEvent, property_, winRect_, targetRect)) {
        if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRectChange_) {
            // winRect_ = targetRect;
            moveDragController_->SetTargetRect(targetRect);
            moveDragController_->RequestVsync();
        }
        return WSError::WS_OK;
    }

    if (moveDragController_->GetStartMoveFlag()) {
        return ConsumeMoveOrDragEvent(pointerEvent);
    }

    return Session::TransferPointerEvent(pointerEvent);
}

void SceneSession::NotifySessionRectChange(const WSRect& rect)
{
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRectChange_) {
        sessionChangeCallback_->onRectChange_(rect);
    }
}

WSError SceneSession::ConsumeMoveOrDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t startPointerId = moveDragController_->GetMoveProperty().startPointerId_;
    if (startPointerId != -1 && startPointerId != pointerId) {
        WLOGFI("block unnecessary pointer event in the window area");
        return WSError::WS_DO_NOTHING;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    int32_t sourceType = pointerEvent->GetSourceType();
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem) ||
        (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        pointerEvent->GetButtonId() != MMI::PointerEvent::MOUSE_BUTTON_LEFT)) {
        WLOGFW("invalid pointerEvent");
        return WSError::WS_ERROR_NULLPTR;
    }

    int32_t action = pointerEvent->GetPointerAction();
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            moveDragController_->StartMove(pointerEvent);
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            moveDragController_->EndMove();
            if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnSessionEvent_) {
                sessionChangeCallback_->OnSessionEvent_(static_cast<uint32_t>(SessionEvent::EVENT_END_MOVE));
            }
            break;
        }
        default:
            break;
    }
    return WSError::WS_OK;
}

void SceneSession::ProcessVsyncHandleRegister()
{
    NotifyVsyncHandleFunc func = [this](void) {
        this->OnVsyncHandle();
    };
    moveDragController_->SetVsyncHandleListenser(func);
}

void SceneSession::OnVsyncHandle()
{
    WSRect rect = moveDragController_->GetMoveProperty().targetRect_;
    if (moveDragController_->GetStartMoveFlag()) {
        rect.width_ = winRect_.width_;
        rect.height_ = winRect_.height_;
    }

    if (rect.posX_ == 0 && rect.posY_ == 0 && rect.width_ == 0 && rect.height_ == 0) {
        return;
    }
    WLOGFD("rect: [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_, rect.width_, rect.height_);
    NotifySessionRectChange(rect);
}
} // namespace OHOS::Rosen
