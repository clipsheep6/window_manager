/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "session/host/include/keyboard_session.h"
#include "session/host/include/session.h"
#include "common/include/session_permission.h"
#include "display_manager.h"
#include <hitrace_meter.h>
#include "session_helper.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

KeyboardSession::KeyboardSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SystemSession(info, specificCallback)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "Create KeyboardSession");
}

KeyboardSession::~KeyboardSession()
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "~KeyboardSession");
}

WSError KeyboardSession::Show(sptr<WindowSessionProperty> property)
{
    auto task = [weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "session is null, show keyboard failed!");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }

        TLOGI(WmsLogTag::WMS_KEYBOARD, "Show keyboard session, id: %{public}d", session->GetPersistentId());
        auto ret = session->SceneSession::Foreground(property);
        session->OnKeyboardShown();
        return ret;
    };
    PostTask(task, "Show");
    return WSError::WS_OK;
}

WSError KeyboardSession::Hide()
{
    if (!SessionPermission::IsStartedByInputMethod()) {
        TLOGE(WmsLogTag::WMS_LIFE, "Hide permission denied, keyboard is not hidden by current input method");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "session is null, hide keyboard failed!");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }

        TLOGI(WmsLogTag::WMS_KEYBOARD, "Hide keyboard session, id: %{public}d", session->GetPersistentId());
        auto ret = session->SetActive(false);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        ret = session->SceneSession::Background();
        session->RestoreCallingWindow();
        TLOGI(WmsLogTag::WMS_KEYBOARD, "When the soft keyboard is hidden, set the callingWindowId to 0.");
        if (session->GetSessionProperty()) {
            session->GetSessionProperty()->SetCallingWindow(INVALID_WINDOW_ID);
        }
        return ret;
    };
    PostTask(task, "Hide");
    return WSError::WS_OK;
}

WSError KeyboardSession::Disconnect(bool isFromClient)
{
    auto task = [weakThis = wptr(this), isFromClient]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "session is null, disconnect keyboard session failed!");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Disconnect keyboard session, id: %{public}d", session->GetPersistentId());
        session->SceneSession::Disconnect(isFromClient);
        session->RestoreCallingWindow();
        return WSError::WS_OK;
    };
    PostTask(task, "Disconnect");
    return WSError::WS_OK;
}

WSError KeyboardSession::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    auto task = [weakThis = wptr(this), rect, reason, rsTransaction]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->winRect_ == rect) {
            TLOGD(WmsLogTag::WMS_KEYBOARD, "skip same rect update id:%{public}d!", session->GetPersistentId());
            return WSError::WS_OK;
        }
        if (rect.IsInvalid()) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "id:%{public}d rect:%{public}s is invalid", session->GetPersistentId(),
            rect.ToString().c_str());
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
            "KeyboardSession::UpdateRect %d [%d, %d, %u, %u]",
            session->GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        
        // position change no need to notify client, since frame layout finish will notify
        if (NearEqual(rect.width_, session->winRect_.width_) && NearEqual(rect.height_, session->winRect_.height_)) {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "position change no need notify client id:%{public}d, rect:%{public}s, \
                preRect: %{public}s",
                session->GetPersistentId(), rect.ToString().c_str(), session->winRect_.ToString().c_str());
            session->winRect_ = rect;
            session->isDirty_ = true;
        } else {
            session->winRect_ = rect;
            session->NotifyClientToUpdateRect(rsTransaction);
            session->RaiseCallingWindow(true);
        }
        TLOGD(WmsLogTag::WMS_KEYBOARD, "id:%{public}d, reason:%{public}d, rect:%{public}s",
            session->GetPersistentId(), session->reason_, rect.ToString().c_str());

        return WSError::WS_OK;
    };
    PostTask(task, "UpdateRect");
    return WSError::WS_OK;
}

void KeyboardSession::SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight)
{
    textFieldPositionY_ = textFieldPositionY;
    textFieldHeight_ = textFieldHeight;
}

double KeyboardSession::GetTextFieldPositionY()
{
    return textFieldPositionY_;
}

double KeyboardSession::GetTextFieldHeight()
{
    return textFieldHeight_;
}

WSError KeyboardSession::SetSessionGravity(SessionGravity gravity, uint32_t percent)
{
    auto task = [weakThis = wptr(this), gravity, percent]() -> WSError {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboard session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, gravity: %{public}d, percent: %{public}d",
            session->GetPersistentId(), gravity, percent);

        if (session->GetSessionProperty()) {
            session->GetSessionProperty()->SetSessionGravity(gravity, percent);
        }
        session->RelayoutKeyBoard();
        bool isKeyboardForeground = true;
        if (session->GetSessionState() != SessionState::STATE_FOREGROUND &&
            session->GetSessionState() != SessionState::STATE_ACTIVE) {
            isKeyboardForeground = false;
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard is not foreground, not need to adjust or restore callingWindow");
        }
        if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
            TLOGD(WmsLogTag::WMS_KEYBOARD, "input method is float mode");
            session->SetWindowAnimationFlag(false);
            if (isKeyboardForeground) {
                session->RestoreCallingWindow();
            }
        } else {
            TLOGD(WmsLogTag::WMS_KEYBOARD, "input method is bottom mode");
            session->SetWindowAnimationFlag(true);
            if (isKeyboardForeground) {
                session->RaiseCallingWindow();
            }
        }
        return WSError::WS_OK;
    };
    PostTask(task, "SetSessionGravity");
    return WSError::WS_OK;
}

void KeyboardSession::SetCallingWindowId(uint32_t callingWindowId)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "calling window id: %{public}d", callingWindowId);
    UpdateCallingWindowIdAndPosition(callingWindowId);
    if (specificCallback_ != nullptr) {
        specificCallback_->onCallingWindowIdChange_(callingWindowId);
    }
}

uint32_t KeyboardSession::GetCallingWindowId()
{
    return callingWindowId_;
}

sptr<SceneSession> KeyboardSession::GetSceneSession(uint32_t persistendId)
{
    if (specificCallback_ == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "specificCallback_ is nullptr, get focusedSessionId failed!");
        return nullptr;
    }
    return specificCallback_->onGetSceneSession_(persistendId);
}

int32_t KeyboardSession::GetFocusedSessionId()
{
    if (specificCallback_ == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "specificCallback_ is nullptr, get focusedSessionId failed!");
        return INVALID_WINDOW_ID;
    }
    return specificCallback_->onGetFocusedSession_();
}

bool KeyboardSession::IsStatusBarVisible(const sptr<SceneSession>& session)
{
    if (session == nullptr) {
        return false;
    }
    const auto& state = session->GetSessionState();

    if (session->IsVisible() || state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND) {
        TLOGD(WmsLogTag::WMS_KEYBOARD, "Status Bar is at foreground, id: %{public}d", session->GetPersistentId());
        return true;
    }
    TLOGD(WmsLogTag::WMS_KEYBOARD, "Status Bar is at background, id: %{public}d", session->GetPersistentId());
    return false;
}

int32_t KeyboardSession::GetStatusBarHeight()
{
    int32_t statusBarHeight = 0;
    int32_t height = 0;
    if (specificCallback_ == nullptr || specificCallback_->onGetSceneSessionVectorByType_) {
        return statusBarHeight;
    }
    std::vector<sptr<SceneSession>> statusBarVector = specificCallback_->onGetSceneSessionVectorByType_(
        WindowType::WINDOW_TYPE_STATUS_BAR, GetSessionProperty()->GetDisplayId());
    for (auto& statusBar : statusBarVector) {
        if (statusBar == nullptr || !IsStatusBarVisible(statusBar)) {
            continue;
        }
        height = statusBar->GetSessionRect().height_;
        statusBarHeight = (statusBarHeight > height) ? statusBarHeight : height;
    }

    return statusBarHeight;
}

void KeyboardSession::NotifyOccupiedAreaChangeInfo(const sptr<SceneSession> callingSession, const WSRect& rect, const WSRect& occupiedArea)
{
    // if keyboard will occupy calling, notify calling window the occupied area and safe height
    const WSRect& safeRect = SessionHelper::GetOverlap(occupiedArea, rect, 0, 0);
    const WSRect& lastSafeRect = callingSession->GetLastSafeRect();
    if (lastSafeRect == safeRect) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "lastSafeRect is same to safeRect");
        return;
    }
    callingSession->SetLastSafeRect(safeRect);
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT,
        SessionHelper::TransferToRect(safeRect), safeRect.height_, GetTextFieldPositionY(), GetTextFieldHeight());
    TLOGI(WmsLogTag::WMS_KEYBOARD, "OccupiedAreaChangeInfo rect: %{public}s, textFieldPositionY_: %{public}f"
        ", textFieldHeight_: %{public}f", occupiedArea.ToString().c_str(), GetTextFieldPositionY(), GetTextFieldHeight());
    callingSession->NotifyOccupiedAreaChangeInfo(info);
}

void KeyboardSession::RaiseCallingWindow(bool isInputUpdated)
{
    sptr<SceneSession> callingSession = GetSceneSession(GetCallingWindowId());
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "calling session is nullptr");
        return;
    }
    SessionGravity gravity;
    uint32_t percent = 0;
    GetSessionProperty()->GetSessionGravity(gravity, percent);
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No need to raise calling window, gravity: %{public}d", gravity);
        return;
    }

    bool isCallingSessionFloating = (callingSession->GetSessionProperty() &&
        callingSession->GetSessionProperty()->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);

    const WSRect keyboardSessionRect = GetSessionRect();
    WSRect callingSessionRect = callingSession->GetSessionRect();
    if (isInputUpdated && isCallingSessionFloating) {
        callingSessionRect = callingWindowRestoringRect_;
    }

    TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardSessionRect: %{public}s, callingSessionRect: %{public}s"
        ", isCallingSessionFloating: %{public}d, isInputUpdated: %{public}d", keyboardSessionRect.ToString().c_str(),
        callingSessionRect.ToString().c_str(), isCallingSessionFloating, isInputUpdated);
    if (SessionHelper::IsEmptyRect(SessionHelper::GetOverlap(keyboardSessionRect, callingSessionRect, 0, 0))) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "There is no overlap area");
        return;
    }

    if (!isInputUpdated) {
        callingWindowRestoringRect_ = callingSessionRect;
    }

    WSRect newRect = callingSessionRect;
    int32_t statusHeight = GetStatusBarHeight();
    if (isCallingSessionFloating && callingSessionRect.posY_ > statusHeight) {
        // calculate new rect of calling window
        newRect.posY_ = std::max(keyboardSessionRect.posY_ - static_cast<int32_t>(newRect.height_), statusHeight);
        NotifyOccupiedAreaChangeInfo(callingSession, newRect, keyboardSessionRect);
        isNeedUpdateSession_ = true;
        callingSession->UpdateSessionRect(newRect, SizeChangeReason::UNDEFINED);
        callingWindowRaisedRect_ = callingSession->GetSessionRect();
    } else {
        NotifyOccupiedAreaChangeInfo(callingSession, newRect, keyboardSessionRect);
    }
}

void KeyboardSession::RestoreCallingWindow()
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "RestoreCallingWindow");
    sptr<SceneSession> callingSession = GetSceneSession(GetCallingWindowId());
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is nullptr");
        return;
    }

    TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling Window, RestoringRect_: %{public}s, RaisedRect_: %{public}s"
        ", curRect_: %{public}s, isNeedUpdateSession_: %{public}d", callingWindowRestoringRect_.ToString().c_str(),
        callingWindowRaisedRect_.ToString().c_str(), callingSession->GetSessionRect().ToString().c_str(),
        isNeedUpdateSession_);
    WSRect overlapRect = { 0, 0, 0, 0 };
    NotifyOccupiedAreaChangeInfo(callingSession, callingWindowRestoringRect_, overlapRect);
    if (!SessionHelper::IsEmptyRect(callingWindowRestoringRect_)) {
        if (isNeedUpdateSession_ && callingSession->GetSessionProperty() &&
            callingSession->GetSessionProperty()->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
            callingSession->GetSessionRect() == callingWindowRaisedRect_) {
            callingSession->UpdateSessionRect(callingWindowRestoringRect_, SizeChangeReason::UNDEFINED);
        }
    }
    isNeedUpdateSession_ = false;
    callingWindowRestoringRect_ = { 0, 0, 0, 0 };
    callingWindowRaisedRect_ = { 0, 0, 0, 0 };
}

// Use focused session id when calling window id is invalid.
void KeyboardSession::UseFocusIdIfCallingWindowIdInvalid()
{
    uint32_t callingWindowId = GetCallingWindowId();
    if (GetSceneSession(callingWindowId) == nullptr) {
        uint32_t focusedSessionId = GetFocusedSessionId();
        if (GetSceneSession(focusedSessionId) == nullptr) {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Focused session is null, id: %{public}d", focusedSessionId);
        } else {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Using focusedSession id: %{public}d", focusedSessionId);
            callingWindowId_ = focusedSessionId;
        }
    }
}

void KeyboardSession::OnKeyboardShown()
{
    if (GetSessionProperty() == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Keyboard session property is nullptr, raise calling window failed.");
        return;
    }
    UseFocusIdIfCallingWindowIdInvalid();
    TLOGD(WmsLogTag::WMS_KEYBOARD, "show keyboard, persistentId: %{public}d, callingWindowId: %{public}d",
        GetPersistentId(), GetCallingWindowId());
    RaiseCallingWindow();
}

void KeyboardSession::UpdateCallingWindowIdAndPosition(uint32_t callingWindowId)
{
    uint32_t curWindowId = GetCallingWindowId();
    TLOGI(WmsLogTag::WMS_KEYBOARD, "CallingWindow curId: %{public}d, newId: %{public}d", curWindowId, callingWindowId);
    // When calling window id changes, restore the old calling window, raise the new calling window.
    if (curWindowId != INVALID_WINDOW_ID && callingWindowId != curWindowId) {
        RestoreCallingWindow();

        callingWindowId_ = callingWindowId;
        UseFocusIdIfCallingWindowIdInvalid();
        RaiseCallingWindow();
    } else {
        callingWindowId_ = callingWindowId;
    }
}

void KeyboardSession::RelayoutKeyBoard()
{
    SessionGravity gravity = SessionGravity::SESSION_GRAVITY_DEFAULT;
    uint32_t percent = 0;
    if (GetSessionProperty() != nullptr) {
        GetSessionProperty()->GetSessionGravity(gravity, percent);
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Gravity: %{public}d, percent: %{public}d", gravity, percent);
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        return;
    }

    auto defaultDisplayInfo = DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "screenSession is null");
        return;
    }

    auto requestRect = GetSessionProperty()->GetRequestRect();
    if (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM) {
        requestRect.width_ = static_cast<uint32_t>(defaultDisplayInfo->GetWidth());
        requestRect.posX_ = 0;
        if (percent != 0) {
            // 100: for calc percent.
            requestRect.height_ = static_cast<uint32_t>(defaultDisplayInfo->GetHeight()) * percent / 100u;
        }
    }
    requestRect.posY_ = defaultDisplayInfo->GetHeight() - static_cast<int32_t>(requestRect.height_);
    GetSessionProperty()->SetRequestRect(requestRect);
    TLOGD(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, rect: %{public}s", GetPersistentId(),
        SessionHelper::TransferToWSRect(requestRect).ToString().c_str());
    UpdateSessionRect(SessionHelper::TransferToWSRect(requestRect), SizeChangeReason::UNDEFINED);
}
} // namespace OHOS::Rosen
