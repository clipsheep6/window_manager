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

#include "session/host/include/system_session.h"

#include "common/include/session_permission.h"
#include "key_event.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "pointer_event.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SystemSession" };
} // namespace

constexpr uint32_t MIN_SYSTEM_WINDOW_WIDTH = 5;
constexpr uint32_t MIN_SYSTEM_WINDOW_HEIGHT = 5;

SystemSession::SystemSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    TLOGD(WmsLogTag::WMS_LIFE, "Create");
    pcFoldScreenController_ = sptr<PcFoldScreenController>::MakeSptr(wptr(this), GetPersistentId());
    moveDragController_ = sptr<MoveDragController>::MakeSptr(GetPersistentId(), GetWindowType());
    if (specificCallback != nullptr &&
        specificCallback->onWindowInputPidChangeCallback_ != nullptr) {
        moveDragController_->SetNotifyWindowPidChangeCallback(specificCallback_->onWindowInputPidChangeCallback_);
    }
    SetMoveDragCallback();
}

SystemSession::~SystemSession()
{
    TLOGD(WmsLogTag::WMS_LIFE, "id: %{public}d", GetPersistentId());
}

void SystemSession::UpdateCameraWindowStatus(bool isShowing)
{
    if (specificCallback_ == nullptr) {
        return;
    }
    if (GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        if (!specificCallback_->onCameraFloatSessionChange_) {
            return;
        }
        TLOGI(WmsLogTag::WMS_SYSTEM, "CameraFloat status: %{public}d, id: %{public}d", isShowing, GetPersistentId());
        specificCallback_->onCameraFloatSessionChange_(GetSessionProperty()->GetAccessTokenId(), isShowing);
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_PIP && GetWindowMode() == WindowMode::WINDOW_MODE_PIP) {
        if (!specificCallback_->onCameraSessionChange_) {
            return;
        }
        auto pipType = GetPiPTemplateInfo().pipTemplateType;
        if (pipType == static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL) ||
            pipType == static_cast<uint32_t>(PiPTemplateType::VIDEO_MEETING)) {
            TLOGI(WmsLogTag::WMS_SYSTEM, "PiPWindow status: %{public}d, id: %{public}d", isShowing, GetPersistentId());
            specificCallback_->onCameraSessionChange_(GetSessionProperty()->GetAccessTokenId(), isShowing);
        }
    } else {
        TLOGI(WmsLogTag::WMS_SYSTEM, "Skip window type, isShowing: %{public}d", isShowing);
    }
}

WSError SystemSession::Show(sptr<WindowSessionProperty> property)
{
    if (!CheckPermissionWithPropertyAnimation(property)) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto type = GetWindowType();
    if (((type == WindowType::WINDOW_TYPE_TOAST) || (type == WindowType::WINDOW_TYPE_FLOAT)) &&
        !SessionPermission::IsSystemCalling()) {
        auto parentSession = GetParentSession();
        if (parentSession == nullptr) {
            WLOGFW("parent session is null");
            return WSError::WS_ERROR_INVALID_PARENT;
        }
        if ((type == WindowType::WINDOW_TYPE_TOAST) && !parentSession->IsSessionForeground()) {
            WLOGFW("parent session is not in foreground");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
    }
    PostTask([weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "Show session, id: %{public}d", session->GetPersistentId());
        // use property from client
        if (property && property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->GetSessionProperty()->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
            session->NotifyIsCustomAnimationPlaying(true);
        }
        session->UpdateCameraWindowStatus(true);
        session->UpdatePiPWindowStateChanged(true);
        auto ret = session->SceneSession::Foreground(property);
        return ret;
    }, "Show");
    return WSError::WS_OK;
}

WSError SystemSession::Hide()
{
    if (!CheckPermissionWithPropertyAnimation(GetSessionProperty())) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto type = GetWindowType();
    if (NeedSystemPermission(type)) {
        // Do not need to verify the permission to hide the input method status bar.
        if (!SessionPermission::IsSystemCalling() && type != WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR) {
            TLOGE(WmsLogTag::WMS_LIFE, "Hide permission denied id: %{public}d type:%{public}u",
                GetPersistentId(), type);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    }
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "Hide session, id: %{public}d", session->GetPersistentId());
        auto ret = session->SetActive(false);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        // background will remove surfaceNode, custom not execute
        // not animation playing when already background; inactive may be animation playing
        auto sessionProperty = session->GetSessionProperty();
        if (sessionProperty &&
            sessionProperty->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->NotifyIsCustomAnimationPlaying(true);
            return WSError::WS_OK;
        }
        session->UpdateCameraWindowStatus(false);
        session->UpdatePiPWindowStateChanged(false);
        ret = session->SceneSession::Background();
        return ret;
    }, "Hide");
    return WSError::WS_OK;
}

WSError SystemSession::Disconnect(bool isFromClient, const std::string& identityToken)
{
    PostTask([weakThis = wptr(this), isFromClient]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "Disconnect session, id: %{public}d", session->GetPersistentId());
        session->SceneSession::Disconnect(isFromClient);
        session->UpdateCameraWindowStatus(false);
        session->UpdatePiPWindowStateChanged(false);
        return WSError::WS_OK;
    }, "Disconnect");
    return WSError::WS_OK;
}

WSError SystemSession::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    const auto& id = GetPersistentId();
    const auto& type = GetWindowType();
    auto parentSession = GetParentSession();
    if (parentSession && parentSession->CheckDialogOnForeground()) {
        WLOGFI("Parent has dialog foreground, id: %{public}d, type: %{public}d", id, type);
        parentSession->HandlePointDownDialog();
        if (!IsTopDialog()) {
            return WSError::WS_OK;
        }
    }
    if (type == WindowType::WINDOW_TYPE_DIALOG) {
        Session::ProcessClickModalWindowOutside(posX, posY);
        auto sessionProperty = GetSessionProperty();
        if (sessionProperty && sessionProperty->GetRaiseEnabled()) {
            RaiseToAppTopForPointDown();
        }
    }
    TLOGD(WmsLogTag::WMS_EVENT, "id:%{public}d, type:%{public}d", id, type);
    PresentFocusIfPointDown();
    return SceneSession::ProcessPointDownSession(posX, posY);
}

int32_t SystemSession::GetMissionId() const
{
    auto parentSession = GetParentSession();
    return parentSession != nullptr ? parentSession->GetPersistentId() : SceneSession::GetMissionId();
}

WSError SystemSession::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        if (keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_BACK) {
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        auto parentSession = GetParentSession();
        if (parentSession && parentSession->CheckDialogOnForeground() &&
            !IsTopDialog()) {
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (!CheckKeyEventDispatch(keyEvent)) {
            WLOGFW("Do not dispatch the key event.");
            return WSError::WS_DO_NOTHING;
        }
    }

    WSError ret = Session::TransferKeyEvent(keyEvent);
    return ret;
}

WSError SystemSession::ProcessBackEvent()
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG && !dialogSessionBackGestureEnabled_) {
        TLOGI(WmsLogTag::WMS_DIALOG, "this is dialog, id: %{public}d", GetPersistentId());
        return WSError::WS_OK;
    }
    return Session::ProcessBackEvent();
}

WSError SystemSession::NotifyClientToUpdateRect(const std::string& updateReason,
    std::shared_ptr<RSTransaction> rsTransaction)
{
    PostTask([weakThis = wptr(this), rsTransaction, updateReason]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WSError ret = session->NotifyClientToUpdateRectTask(updateReason, rsTransaction);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        if (session->specificCallback_ != nullptr && session->specificCallback_->onUpdateAvoidArea_ != nullptr) {
            if (Session::IsScbCoreEnabled()) {
                session->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
            } else {
                session->specificCallback_->onUpdateAvoidArea_(session->GetPersistentId());
            }
        }
        return ret;
    }, "NotifyClientToUpdateRect");
    return WSError::WS_OK;
}

bool SystemSession::CheckKeyEventDispatch(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    auto currentRect = winRect_;
    if (!GetRSVisible() || currentRect.width_ == 0 || currentRect.height_ == 0) {
        WLOGE("Error size: [width: %{public}d, height: %{public}d], isRSVisible_: %{public}d,"
            " persistentId: %{public}d",
            currentRect.width_, currentRect.height_, GetRSVisible(), GetPersistentId());
        return false;
    }

    auto parentSession = GetParentSession();
    if (parentSession == nullptr) {
        WLOGFW("Dialog parent is null");
        return false;
    }
    auto parentSessionState = parentSession->GetSessionState();
    if ((parentSessionState != SessionState::STATE_FOREGROUND &&
        parentSessionState != SessionState::STATE_ACTIVE) ||
        (state_ != SessionState::STATE_FOREGROUND &&
        state_ != SessionState::STATE_ACTIVE)) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Dialog's parent info : [persistentId: %{publicd}d, state:%{public}d];"
            "Dialog info:[persistentId: %{publicd}d, state:%{public}d]",
            parentSession->GetPersistentId(), parentSessionState, GetPersistentId(), GetSessionState());
        return false;
    }
    return true;
}

bool SystemSession::NeedSystemPermission(WindowType type)
{
    return !(type == WindowType::WINDOW_TYPE_SCENE_BOARD || type == WindowType::WINDOW_TYPE_SYSTEM_FLOAT ||
        type == WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW || type == WindowType::WINDOW_TYPE_TOAST ||
        type == WindowType::WINDOW_TYPE_DRAGGING_EFFECT || type == WindowType::WINDOW_TYPE_APP_LAUNCHING ||
        type == WindowType::WINDOW_TYPE_PIP || type == WindowType::WINDOW_TYPE_FLOAT);
}

bool SystemSession::CheckPointerEventDispatch(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const
{
    auto sessionState = GetSessionState();
    int32_t action = pointerEvent->GetPointerAction();
    auto isPC = systemConfig_.IsPcWindow();
    bool isDialog = WindowHelper::IsDialogWindow(GetWindowType());
    if (isPC && isDialog && sessionState != SessionState::STATE_FOREGROUND &&
        sessionState != SessionState::STATE_ACTIVE &&
        action != MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW) {
        WLOGFW("false, Current Session Info: [persistentId: %{public}d, "
            "state: %{public}d, action:%{public}d]", GetPersistentId(), GetSessionState(), action);
        return false;
    }
    return true;
}

void SystemSession::UpdatePointerArea(const WSRect& rect)
{
    auto property = GetSessionProperty();
    if (!(property->IsDecorEnable() && GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING)) {
        return;
    }
    Session::UpdatePointerArea(rect);
}

void SystemSession::RectCheck(uint32_t curWidth, uint32_t curHeight)
{
    uint32_t minWidth = MIN_SYSTEM_WINDOW_WIDTH;
    uint32_t minHeight = MIN_SYSTEM_WINDOW_HEIGHT;
    uint32_t maxFloatingWindowSize = GetSystemConfig().maxFloatingWindowSize_;
    RectSizeCheckProcess(curWidth, curHeight, minWidth, minHeight, maxFloatingWindowSize);
}

bool SystemSession::IsVisibleForeground() const
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG &&
        parentSession_ && WindowHelper::IsMainWindow(parentSession_->GetWindowType())) {
        return parentSession_->IsVisibleForeground() && Session::IsVisibleForeground();
    }
    return Session::IsVisibleForeground();
}

bool SystemSession::IsVisibleNotBackground() const
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG &&
        parentSession_ && WindowHelper::IsMainWindow(parentSession_->GetWindowType())) {
        return parentSession_->IsVisibleNotBackground() && Session::IsVisibleNotBackground();
    }
    return Session::IsVisibleNotBackground();
}

WSError SystemSession::SetDialogSessionBackGestureEnabled(bool isEnabled)
{
    return PostSyncTask([weakThis = wptr(this), isEnabled]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WindowType windowType = session->GetWindowType();
        if (windowType != WindowType::WINDOW_TYPE_DIALOG) {
            TLOGE(WmsLogTag::WMS_DIALOG, "windowType not support. WinId:%{public}u, WindowType:%{public}u",
                session->GetWindowId(), static_cast<uint32_t>(windowType));
            return WSError::WS_ERROR_INVALID_CALLING;
        }
        session->dialogSessionBackGestureEnabled_ = isEnabled;
        return WSError::WS_OK;
    });
}

void SystemSession::UpdatePiPWindowStateChanged(bool isForeground)
{
    if (specificCallback_ == nullptr || !specificCallback_->onPiPStateChange_) {
        return;
    }
    if (GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
        TLOGI(WmsLogTag::WMS_PIP, "pip state changed, bundleName:%{public}s, state:%{public}d",
            GetSessionInfo().bundleName_.c_str(), isForeground);
        specificCallback_->onPiPStateChange_(GetSessionInfo().bundleName_, isForeground);
    } else {
        TLOGD(WmsLogTag::WMS_PIP, "skip type");
    }
}

int32_t SystemSession::GetSubWindowZLevel() const
{
    int32_t zLevel = 0;
    auto sessionProperty = GetSessionProperty();
    zLevel = sessionProperty->GetSubWindowZLevel();
    return zLevel;
}
} // namespace OHOS::Rosen
