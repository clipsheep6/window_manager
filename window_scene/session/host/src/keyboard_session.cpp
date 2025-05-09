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

#include "perform_reporter.h"
#include "session/host/include/keyboard_session.h"

#include <hitrace_meter.h>
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session_helper.h"
#include <ui/rs_surface_node.h>
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

KeyboardSession::KeyboardSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback,
    const sptr<KeyboardSessionCallback>& keyboardCallback)
    : SystemSession(info, specificCallback)
{
    keyboardCallback_ = keyboardCallback;
    scenePersistence_ = sptr<ScenePersistence>::MakeSptr(info.bundleName_, GetPersistentId());
    if (info.persistentId_ != 0 && info.persistentId_ != GetPersistentId()) {
        // persistentId changed due to id conflicts. Need to rename the old snapshot if exists
        scenePersistence_->RenameSnapshotFromOldPersistentId(info.persistentId_);
        TLOGI(WmsLogTag::WMS_KEYBOARD, "RenameSnapshotFromOldPersistentId %{public}d", info.persistentId_);
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Create KeyboardSession");
}

KeyboardSession::~KeyboardSession()
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Destroy KeyboardSession");
}

void KeyboardSession::BindKeyboardPanelSession(sptr<SceneSession> panelSession)
{
    if (panelSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "PanelSession is null");
        return;
    }
    keyboardPanelSession_ = panelSession;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Success, panelId: %{public}d", panelSession->GetPersistentId());
}

sptr<SceneSession> KeyboardSession::GetKeyboardPanelSession() const
{
    return keyboardPanelSession_;
}

SessionGravity KeyboardSession::GetKeyboardGravity() const
{
    SessionGravity gravity = static_cast<SessionGravity>(GetSessionProperty()->GetKeyboardLayoutParams().gravity_);
    TLOGD(WmsLogTag::WMS_KEYBOARD, "Gravity: %{public}d", gravity);
    return gravity;
}

WSError KeyboardSession::Show(sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!CheckPermissionWithPropertyAnimation(property)) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    PostTask([weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null, show keyboard failed");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->systemConfig_.IsPcWindow()) {
            if (auto surfaceNode = session->GetSurfaceNode()) {
                surfaceNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE);
            }
        }
        if (session->GetKeyboardGravity() == SessionGravity::SESSION_GRAVITY_BOTTOM) {
            session->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_SHOW);
        }
        session->GetSessionProperty()->SetKeyboardViewMode(property->GetKeyboardViewMode());
        session->UseFocusIdIfCallingSessionIdInvalid();
        TLOGNI(WmsLogTag::WMS_KEYBOARD,
            "Show keyboard session, id: %{public}d, calling id: %{public}d, viewMode: %{public}u",
            session->GetPersistentId(), session->GetCallingSessionId(),
            static_cast<uint32_t>(property->GetKeyboardViewMode()));
        session->MoveAndResizeKeyboard(property->GetKeyboardLayoutParams(), property, true);
        return session->SceneSession::Foreground(property);
    }, "Show");
    return WSError::WS_OK;
}

WSError KeyboardSession::Hide()
{
    if (!CheckPermissionWithPropertyAnimation(GetSessionProperty())) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null, hide keyboard failed");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }

        TLOGI(WmsLogTag::WMS_KEYBOARD, "Hide keyboard session, set callingSessionId to 0, id: %{public}d",
            session->GetPersistentId());
        auto ret = session->SetActive(false);
        if (ret != WSError::WS_OK) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Set session active state failed, ret: %{public}d", ret);
            return ret;
        }
        ret = session->SceneSession::Background();
        WSRect rect = {0, 0, 0, 0};
        session->NotifyKeyboardPanelInfoChange(rect, false);
        if (session->systemConfig_.IsPcWindow() || session->GetSessionScreenName() == "HiCar" ||
            session->GetSessionScreenName() == "SuperLauncher" ||
            session->GetSessionScreenName() == "PadWithCar") {
            TLOGD(WmsLogTag::WMS_KEYBOARD, "PC or virtual screen, restore calling session");
            !session->IsSystemKeyboard() ? session->RestoreCallingSession(session->GetCallingSessionId(), nullptr) :
                session->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_HIDE);
            auto sessionProperty = session->GetSessionProperty();
            if (sessionProperty) {
                sessionProperty->SetCallingSessionId(INVALID_WINDOW_ID);
            }
        }
        return ret;
    }, "Hide");
    return WSError::WS_OK;
}

WSError KeyboardSession::Disconnect(bool isFromClient, const std::string& identityToken)
{
    PostTask([weakThis = wptr(this), isFromClient]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null, disconnect keyboard session failed");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Disconnect keyboard session, id: %{public}d, isFromClient: %{public}d",
            session->GetPersistentId(), isFromClient);
        session->SceneSession::Disconnect(isFromClient);
        WSRect rect = {0, 0, 0, 0};
        session->NotifyKeyboardPanelInfoChange(rect, false);
        !session->IsSystemKeyboard() ? session->RestoreCallingSession(session->GetCallingSessionId(), nullptr) :
            session->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_DISCONNECT);
        auto sessionProperty = session->GetSessionProperty();
        if (sessionProperty) {
            sessionProperty->SetCallingSessionId(INVALID_WINDOW_ID);
        }
        return WSError::WS_OK;
    }, "Disconnect");
    return WSError::WS_OK;
}

WSError KeyboardSession::NotifyClientToUpdateRect(const std::string& updateReason,
    std::shared_ptr<RSTransaction> rsTransaction)
{
    PostTask([weakThis = wptr(this), rsTransaction, updateReason]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }

        WSError ret = session->NotifyClientToUpdateRectTask(updateReason, rsTransaction);
        return ret;
    }, "NotifyClientToUpdateRect");
    return WSError::WS_OK;
}

void KeyboardSession::UpdateKeyboardAvoidArea()
{
    if (!IsSessionForeground() || !IsVisibleForeground()) {
        TLOGD(WmsLogTag::WMS_KEYBOARD, "Keyboard is not foreground, no need to update avoid area");
        return;
    }
    if (specificCallback_ != nullptr && specificCallback_->onUpdateAvoidArea_ != nullptr) {
        if (Session::IsScbCoreEnabled()) {
            dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
        } else {
            specificCallback_->onUpdateAvoidArea_(GetPersistentId());
        }
    }
}

void KeyboardSession::OnKeyboardPanelUpdated()
{
    RaiseCallingSession(GetCallingSessionId(), GetPanelRect(), true, nullptr);
    UpdateKeyboardAvoidArea();
}

void KeyboardSession::OnCallingSessionUpdated()
{
    if (!keyboardAvoidAreaActive_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, isSystemKeyboard: %{public}d, state: %{public}d, "
            "gravity: %{public}d", GetPersistentId(), IsSystemKeyboard(), GetSessionState(), GetKeyboardGravity());
        return;
    }
    if (!IsSessionForeground() || !IsVisibleForeground()) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard is not foreground");
        return;
    }
    WSRect panelRect = GetPanelRect();
    RecalculatePanelRectForAvoidArea(panelRect);
    sptr<SceneSession> callingSession = GetSceneSession(GetCallingSessionId());
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is null");
        return;
    }
    bool isCallingSessionFloating = (callingSession->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);
    if (!CheckIfNeedRaiseCallingSession(callingSession, isCallingSessionFloating)) {
        return;
    }
    WSRect callingSessionRect = callingSession->GetSessionRect();
    NotifyOccupiedAreaChangeInfo(callingSession, callingSessionRect, panelRect);

    TLOGI(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, callSession Rect: %{public}s",
        GetPersistentId(), callingSessionRect.ToString().c_str());
}

void KeyboardSession::SetCallingSessionId(uint32_t callingSessionId)
{
    PostTask([weakThis = wptr(this), callingSessionId]() mutable {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Session is null");
            return;
        }
        if (session->GetSceneSession(callingSessionId) == nullptr) {
            uint32_t focusedSessionId = static_cast<uint32_t>(session->GetFocusedSessionId());
            if (session->GetSceneSession(focusedSessionId) == nullptr) {
                TLOGE(WmsLogTag::WMS_KEYBOARD, "Focused session is null, set id: %{public}d failed", focusedSessionId);
                return;
            } else {
                TLOGI(WmsLogTag::WMS_KEYBOARD, "Using focusedSession id: %{public}d", focusedSessionId);
                callingSessionId = focusedSessionId;
            }
        }
        uint32_t curCallingId = session->GetCallingSessionId();
        TLOGI(WmsLogTag::WMS_KEYBOARD, "CurId: %{public}d, newId: %{public}d", curCallingId, callingSessionId);
        // When the keyboard is shown, if the callingId changes, restore the cur calling session.
        if (curCallingId != INVALID_WINDOW_ID && callingSessionId != curCallingId && session->IsSessionForeground()) {
            session->RestoreCallingSession(curCallingId, nullptr);
            sptr<SceneSession> callingSession = session->GetSceneSession(curCallingId);
            WSRect panelRect = session->GetPanelRect();
            if (callingSession != nullptr && session->GetKeyboardGravity() == SessionGravity::SESSION_GRAVITY_BOTTOM) {
                session->RecalculatePanelRectForAvoidArea(panelRect);
                WSRect endRect = {panelRect.posX_, panelRect.posY_ + panelRect.height_, panelRect.width_,
                    panelRect.height_};
                // panelRect as beginRect
                callingSession->NotifyKeyboardAnimationCompleted(false, panelRect, endRect);
            }
        }
        session->GetSessionProperty()->SetCallingSessionId(callingSessionId);

        if (session->keyboardCallback_ == nullptr ||
            session->keyboardCallback_->onCallingSessionIdChange == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "KeyboardCallback_, callingSessionId: %{public}d", callingSessionId);
            return;
        }
        session->keyboardCallback_->onCallingSessionIdChange(callingSessionId);
    }, "SetCallingSessionId");
    return;
}

uint32_t KeyboardSession::GetCallingSessionId()
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Session property is null");
        return INVALID_SESSION_ID;
    }
    return sessionProperty->GetCallingSessionId();
}

WSError KeyboardSession::AdjustKeyboardLayout(const KeyboardLayoutParams& params)
{
    PostTask([weakThis = wptr(this), params]() -> WSError {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Keyboard session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        // set keyboard layout params
        auto sessionProperty = session->GetSessionProperty();
        sessionProperty->SetKeyboardLayoutParams(params);
        session->MoveAndResizeKeyboard(params, sessionProperty, false);
        // handle keyboard gravity change
        if (params.gravity_ == WindowGravity::WINDOW_GRAVITY_FLOAT) {
            session->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_GRAVITY_FLOAT);
            session->SetWindowAnimationFlag(false);
            if (session->IsSessionForeground()) {
                session->RestoreCallingSession(session->GetCallingSessionId(), nullptr);
            }
        } else {
            if (session->IsSessionForeground()) {
                session->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_GRAVITY_BOTTOM);
            }
            session->SetWindowAnimationFlag(true);
        }
        // avoidHeight is set, notify avoidArea in case ui params don't flush
        if (params.landscapeAvoidHeight_ >= 0 && params.portraitAvoidHeight_ >= 0) {
            session->NotifyClientToUpdateAvoidArea();
        }
        // notify keyboard layout param
        if (session->adjustKeyboardLayoutFunc_) {
            session->adjustKeyboardLayoutFunc_(params);
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Adjust keyboard layout, keyboardId: %{public}d, gravity: %{public}u, "
            "landscapeAvoidHeight: %{public}d, portraitAvoidHeight: %{public}d, "
            "LandscapeKeyboardRect: %{public}s, PortraitKeyboardRect: %{public}s, LandscapePanelRect: %{public}s, "
            "PortraitPanelRect: %{public}s, requestRect: %{public}s", session->GetPersistentId(),
            static_cast<uint32_t>(params.gravity_), params.landscapeAvoidHeight_, params.portraitAvoidHeight_,
            params.LandscapeKeyboardRect_.ToString().c_str(), params.PortraitKeyboardRect_.ToString().c_str(),
            params.LandscapePanelRect_.ToString().c_str(), params.PortraitPanelRect_.ToString().c_str(),
            session->GetSessionRequestRect().ToString().c_str());
        return WSError::WS_OK;
    }, "AdjustKeyboardLayout");
    return WSError::WS_OK;
}

sptr<SceneSession> KeyboardSession::GetSceneSession(uint32_t persistentId)
{
    if (keyboardCallback_ == nullptr || keyboardCallback_->onGetSceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Get scene session failed, persistentId: %{public}d", persistentId);
        return nullptr;
    }
    return keyboardCallback_->onGetSceneSession(persistentId);
}

int32_t KeyboardSession::GetFocusedSessionId()
{
    if (keyboardCallback_ == nullptr || keyboardCallback_->onGetFocusedSessionId == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboardCallback is null, get focusedSessionId failed");
        return INVALID_WINDOW_ID;
    }
    return keyboardCallback_->onGetFocusedSessionId();
}

static WSRect CalculateSafeRectForMidScene(const WSRect& windowRect, const WSRect& keyboardRect, float scaleX,
    float scaleY)
{
    if (MathHelper::NearZero(scaleX) || MathHelper::NearZero(scaleY)) {
        return { 0, 0, 0, 0 };
    }
    const WSRect scaledWindowRect = {
        windowRect.posX_,
        windowRect.posY_,
        static_cast<int32_t>(windowRect.width_ * scaleX),
        static_cast<int32_t>(windowRect.height_ * scaleY)
    };

    const WSRect overlap = SessionHelper::GetOverlap(scaledWindowRect, keyboardRect, 0, 0);
    if (SessionHelper::IsEmptyRect(overlap)) {
        return { 0, 0, 0, 0 };
    }

    const WSRect result = {
        static_cast<int32_t>((overlap.posX_ - scaledWindowRect.posX_) / scaleX),
        static_cast<int32_t>((overlap.posY_ - scaledWindowRect.posY_) / scaleY),
        static_cast<int32_t>(overlap.width_ / scaleX),
        static_cast<int32_t>(overlap.height_ / scaleY)
    };
    return result;
}

void KeyboardSession::NotifyOccupiedAreaChangeInfo(const sptr<SceneSession>& callingSession, const WSRect& rect,
    const WSRect& occupiedArea, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    // if keyboard will occupy calling, notify calling window the occupied area and safe height
    const WSRect& safeRect = !callingSession->GetIsMidScene() ? SessionHelper::GetOverlap(occupiedArea, rect, 0, 0) :
        CalculateSafeRectForMidScene(rect, occupiedArea, callingSession->GetScaleX(), callingSession->GetScaleY());
    const WSRect& lastSafeRect = callingSession->GetLastSafeRect();
    if (lastSafeRect == safeRect) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "SafeRect is same to lastSafeRect: %{public}s", safeRect.ToString().c_str());
        return;
    }
    callingSession->SetLastSafeRect(safeRect);
    double textFieldPositionY = 0.0;
    double textFieldHeight = 0.0;
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        textFieldPositionY = sessionProperty->GetTextFieldPositionY();
        textFieldHeight = sessionProperty->GetTextFieldHeight();
    }
    sptr<OccupiedAreaChangeInfo> info = sptr<OccupiedAreaChangeInfo>::MakeSptr(OccupiedAreaType::TYPE_INPUT,
        SessionHelper::TransferToRect(safeRect), safeRect.height_, textFieldPositionY, textFieldHeight);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling id: %{public}d, safeRect: %{public}s, keyboardRect: %{public}s"
        ", textFieldPositionY_: %{public}f, textFieldHeight_: %{public}f", callingSession->GetPersistentId(),
        safeRect.ToString().c_str(), occupiedArea.ToString().c_str(), textFieldPositionY, textFieldHeight);
    if (callingSession->IsSystemSession()) {
        NotifyRootSceneOccupiedAreaChange(info);
    } else {
        callingSession->NotifyOccupiedAreaChangeInfo(info, rsTransaction);
    }
}

void KeyboardSession::NotifyKeyboardPanelInfoChange(WSRect rect, bool isKeyboardPanelShow)
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "SessionStage is null, notify keyboard panel rect change failed");
        return;
    }
    KeyboardPanelInfo keyboardPanelInfo;
    keyboardPanelInfo.rect_ = SessionHelper::TransferToRect(rect);
    keyboardPanelInfo.gravity_ = static_cast<WindowGravity>(GetKeyboardGravity());
    keyboardPanelInfo.isShowing_ = isKeyboardPanelShow;

    sessionStage_->NotifyKeyboardPanelInfoChange(keyboardPanelInfo);
}

bool KeyboardSession::CheckIfNeedRaiseCallingSession(sptr<SceneSession> callingSession, bool isCallingSessionFloating)
{
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is null");
        return false;
    }

    SessionGravity gravity = GetKeyboardGravity();
    if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No need to raise calling session, gravity: %{public}d", gravity);
        return false;
    }
    bool isMainOrParentFloating = WindowHelper::IsMainWindow(callingSession->GetWindowType()) ||
        (SessionHelper::IsNonSecureToUIExtension(callingSession->GetWindowType()) &&
         callingSession->GetParentSession() != nullptr &&
         callingSession->GetParentSession()->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);
    bool isFreeMultiWindowMode = callingSession->IsFreeMultiWindowMode();
    bool isMidScene = callingSession->GetIsMidScene();
    if (isCallingSessionFloating && isMainOrParentFloating && !isMidScene &&
        (systemConfig_.IsPhoneWindow() || (systemConfig_.IsPadWindow() && !isFreeMultiWindowMode))) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No need to raise calling session in float window");
        return false;
    }

    return true;
}

void KeyboardSession::RaiseCallingSession(uint32_t callingId, const WSRect& keyboardPanelRect, bool needCheckVisible,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    if (!keyboardAvoidAreaActive_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, isSystemKeyboard: %{public}d, state: %{public}d, "
            "gravity: %{public}d", GetPersistentId(), IsSystemKeyboard(), GetSessionState(), GetKeyboardGravity());
        return;
    }
    if (!IsSessionForeground() || (needCheckVisible && !IsVisibleForeground())) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard is not foreground");
        return;
    }
    sptr<SceneSession> callingSession = GetSceneSession(callingId);
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is null");
        return;
    }
    NotifyKeyboardPanelInfoChange(keyboardPanelRect, true);

    bool isCallingSessionFloating = (callingSession->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) &&
        !callingSession->GetIsMidScene();
    if (!CheckIfNeedRaiseCallingSession(callingSession, isCallingSessionFloating)) {
        return;
    }

    WSRect callingSessionRect = callingSession->GetSessionRect();
    int32_t oriPosYBeforeRaisedByKeyboard = callingSession->GetOriPosYBeforeRaisedByKeyboard();
    if (oriPosYBeforeRaisedByKeyboard != 0 && isCallingSessionFloating) {
        callingSessionRect.posY_ = oriPosYBeforeRaisedByKeyboard;
    }
    // update panel rect for avoid area caculate
    WSRect panelAvoidRect = keyboardPanelRect;
    RecalculatePanelRectForAvoidArea(panelAvoidRect);
    if (SessionHelper::IsEmptyRect(SessionHelper::GetOverlap(panelAvoidRect, callingSessionRect, 0, 0)) &&
        oriPosYBeforeRaisedByKeyboard == 0) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No overlap area, keyboardRect: %{public}s, callingRect: %{public}s",
            keyboardPanelRect.ToString().c_str(), callingSessionRect.ToString().c_str());
        NotifyOccupiedAreaChangeInfo(callingSession, callingSessionRect, panelAvoidRect, rsTransaction);
        return;
    }

    WSRect newRect = callingSessionRect;
    int32_t statusHeight = callingSession->GetStatusBarHeight();
    if (IsNeedRaiseSubWindow(callingSession, newRect) &&
        isCallingSessionFloating && callingSessionRect.posY_ > statusHeight) {
        if (oriPosYBeforeRaisedByKeyboard == 0) {
            oriPosYBeforeRaisedByKeyboard = callingSessionRect.posY_;
            callingSession->SetOriPosYBeforeRaisedByKeyboard(callingSessionRect.posY_);
        }
        // calculate new rect of calling session
        newRect.posY_ = std::max(panelAvoidRect.posY_ - newRect.height_, statusHeight);
        newRect.posY_ = std::min(oriPosYBeforeRaisedByKeyboard, newRect.posY_);
        NotifyOccupiedAreaChangeInfo(callingSession, newRect, panelAvoidRect, rsTransaction);
        if (!IsSystemKeyboard()) {
            callingSession->UpdateSessionRect(newRect, SizeChangeReason::UNDEFINED);
        }
    } else {
        NotifyOccupiedAreaChangeInfo(callingSession, newRect, panelAvoidRect, rsTransaction);
    }

    TLOGI(WmsLogTag::WMS_KEYBOARD, "KeyboardRect: %{public}s, callSession OriRect: %{public}s, newRect: %{public}s"
        ", oriPosYBeforeRaisedByKeyboard: %{public}d, isCallingSessionFloating: %{public}d",
        keyboardPanelRect.ToString().c_str(), callingSessionRect.ToString().c_str(), newRect.ToString().c_str(),
        oriPosYBeforeRaisedByKeyboard, isCallingSessionFloating);
}

void KeyboardSession::RestoreCallingSession(uint32_t callingId, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    if (!keyboardAvoidAreaActive_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, isSystemKeyboard: %{public}d, state: %{public}d, "
            "gravity: %{public}d", GetPersistentId(), IsSystemKeyboard(), GetSessionState(), GetKeyboardGravity());
        return;
    }
    sptr<SceneSession> callingSession = GetSceneSession(callingId);
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is null");
        return;
    }
    const WSRect& emptyRect = { 0, 0, 0, 0 };
    int32_t oriPosYBeforeRaisedByKeyboard = callingSession->GetOriPosYBeforeRaisedByKeyboard();
    NotifyOccupiedAreaChangeInfo(callingSession, emptyRect, emptyRect, rsTransaction);
    if (oriPosYBeforeRaisedByKeyboard != 0 &&
        callingSession->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
        WSRect callingSessionRestoringRect = callingSession->GetSessionRect();
        if (oriPosYBeforeRaisedByKeyboard != 0) {
            callingSessionRestoringRect.posY_ = oriPosYBeforeRaisedByKeyboard;
        }
        TLOGI(WmsLogTag::WMS_KEYBOARD, "OriPosYBeforeRaisedByKeyboard: %{public}d, sessionMode: %{public}d",
            oriPosYBeforeRaisedByKeyboard, callingSession->GetWindowMode());
        if (!IsSystemKeyboard()) {
            callingSession->UpdateSessionRect(callingSessionRestoringRect, SizeChangeReason::UNDEFINED);
        }
    }
    callingSession->SetOriPosYBeforeRaisedByKeyboard(0); // 0: default value
}

void KeyboardSession::NotifySessionRectChange(const WSRect& rect,
    SizeChangeReason reason, DisplayId displayId, const RectAnimationConfig& rectAnimationConfig)
{
    PostTask([weakThis = wptr(this), rect, reason, displayId, rectAnimationConfig, where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s session is null", where);
            return;
        }
        uint32_t screenWidth = 0;
        uint32_t screenHeight = 0;
        auto sessionProperty = session->GetSessionProperty();
        bool ret = session->GetScreenWidthAndHeightFromClient(sessionProperty, screenWidth, screenHeight);
        if (!ret) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s get screen size failed", where);
            return;
        }
        bool isLand = screenWidth > screenHeight;
        KeyboardLayoutParams params = sessionProperty->GetKeyboardLayoutParams();
        if (isLand) {
            params.LandscapeKeyboardRect_.posX_ = rect.posX_;
            params.LandscapeKeyboardRect_.posY_ = rect.posY_;
            params.LandscapePanelRect_.posX_ = rect.posX_;
            params.LandscapePanelRect_.posY_ = rect.posY_;
        } else {
            params.PortraitKeyboardRect_.posX_ = rect.posX_;
            params.PortraitKeyboardRect_.posY_ = rect.posY_;
            params.PortraitPanelRect_.posX_ = rect.posX_;
            params.PortraitPanelRect_.posY_ = rect.posY_;
        }
        TLOGD(WmsLogTag::WMS_KEYBOARD, "isLand:%{public}d, landRect:%{public}s, portraitRect:%{public}s", isLand,
            params.LandscapeKeyboardRect_.ToString().c_str(), params.PortraitKeyboardRect_.ToString().c_str());
        session->AdjustKeyboardLayout(params);
    }, __func__ + GetRectInfo(rect));
}

// Use focused session id when calling session id is invalid.
void KeyboardSession::UseFocusIdIfCallingSessionIdInvalid()
{
    if (GetSceneSession(GetCallingSessionId()) != nullptr) {
        return;
    }
    uint32_t focusedSessionId = static_cast<uint32_t>(GetFocusedSessionId());
    if (GetSceneSession(focusedSessionId) == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Focused session is null, id: %{public}d", focusedSessionId);
    } else {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Using focusedSession id: %{public}d", focusedSessionId);
        GetSessionProperty()->SetCallingSessionId(focusedSessionId);
    }
}

void KeyboardSession::EnableCallingSessionAvoidArea()
{
    RaiseCallingSession(GetCallingSessionId(), GetPanelRect(), true, nullptr);
}

void KeyboardSession::NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason reason)
{
    if (!systemConfig_.IsPcWindow()) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "This device is not pc");
        return;
    }
    if (!IsSystemKeyboard()) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "This is not system keyboard, id: %{public}d", GetPersistentId());
        return;
    }
    if (keyboardCallback_ == nullptr || keyboardCallback_->onSystemKeyboardAvoidChange == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "SystemKeyboardAvoidChange callback is null, id: %{public}d",
            GetPersistentId());
        return;
    }
    keyboardCallback_->onSystemKeyboardAvoidChange(GetScreenId(), reason);
}

void KeyboardSession::OpenKeyboardSyncTransaction()
{
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "Keyboard session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->isKeyboardSyncTransactionOpen_) {
            TLOGNI(WmsLogTag::WMS_KEYBOARD, "Keyboard sync transaction is already open");
            return WSError::WS_OK;
        }
        TLOGNI(WmsLogTag::WMS_KEYBOARD, "Open keyboard sync");
        session->isKeyboardSyncTransactionOpen_ = true;
        auto transactionController = RSSyncTransactionController::GetInstance();
        if (transactionController) {
            transactionController->OpenSyncTransaction(session->GetEventHandler());
        }
        session->PostKeyboardAnimationSyncTimeoutTask();
        return WSError::WS_OK;
    };
    PostSyncTask(task);
}

void KeyboardSession::CloseKeyboardSyncTransaction(uint32_t callingId, const WSRect& keyboardPanelRect,
    bool isKeyboardShow)
{
    PostTask([weakThis = wptr(this), callingId, keyboardPanelRect, isKeyboardShow]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Keyboard session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGNI(WmsLogTag::WMS_KEYBOARD, "Close keyboard sync, callingId: %{public}d, isKeyboardShow: %{public}d",
            callingId, isKeyboardShow);
        std::shared_ptr<RSTransaction> rsTransaction = nullptr;
        if (session->isKeyboardSyncTransactionOpen_) {
            rsTransaction = session->GetRSTransaction();
        }
        // The callingId may change in WindowManager. Use scb's callingId to properly handle callingWindow raise/restore.
        if (isKeyboardShow) {
            session->RaiseCallingSession(callingId, keyboardPanelRect, false, rsTransaction);
            session->UpdateKeyboardAvoidArea();
        } else {
            session->RestoreCallingSession(callingId, rsTransaction);
            session->GetSessionProperty()->SetCallingSessionId(INVALID_WINDOW_ID);
        }

        if (!session->isKeyboardSyncTransactionOpen_) {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard sync transaction is closed");
            return WSError::WS_OK;
        }
        session->isKeyboardSyncTransactionOpen_ = false;
        auto handler = session->GetEventHandler();
        if (handler) {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Keyboard anim_sync event cancelled");
            handler->RemoveTask(KEYBOARD_ANIM_SYNC_EVENT_NAME);
        }
        auto transactionController = RSSyncTransactionController::GetInstance();
        if (transactionController) {
            transactionController->CloseSyncTransaction(session->GetEventHandler());
        }
        return WSError::WS_OK;
    }, "CloseKeyboardSyncTransaction");
}

std::shared_ptr<RSTransaction> KeyboardSession::GetRSTransaction()
{
    auto transactionController = RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    if (transactionController) {
        rsTransaction = transactionController->GetRSTransaction();
    }
    return rsTransaction;
}

std::string KeyboardSession::GetSessionScreenName()
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        auto displayId = sessionProperty->GetDisplayId();
        auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(displayId);
        if (screenSession != nullptr) {
            return screenSession->GetName();
        }
    }
    return "";
}

void KeyboardSession::MoveAndResizeKeyboard(const KeyboardLayoutParams& params,
    const sptr<WindowSessionProperty>& sessionProperty, bool isShow)
{
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    bool ret = (isShow) ? GetScreenWidthAndHeightFromServer(sessionProperty, screenWidth, screenHeight) :
        GetScreenWidthAndHeightFromClient(sessionProperty, screenWidth, screenHeight);
    if (!ret) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Get screen width and height failed, isShow: %{public}d", isShow);
        return;
    }
    bool isLandscape = screenWidth > screenHeight ? true : false;
    WSRect rect = isLandscape ? SessionHelper::TransferToWSRect(params.LandscapeKeyboardRect_) :
        SessionHelper::TransferToWSRect(params.PortraitKeyboardRect_);
    SetSessionRequestRect(rect);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Id: %{public}d, rect: %{public}s, isLandscape: %{public}d"
        ", screenWidth: %{public}d, screenHeight: %{public}d", GetPersistentId(),
        rect.ToString().c_str(), isLandscape, screenWidth, screenHeight);
}

bool KeyboardSession::IsVisibleForeground() const
{
    return isVisible_;
}

void KeyboardSession::RecalculatePanelRectForAvoidArea(WSRect& panelRect)
{
    auto sessionProperty = GetSessionProperty();
    KeyboardLayoutParams params = sessionProperty->GetKeyboardLayoutParams();
    if (params.landscapeAvoidHeight_ < 0 || params.portraitAvoidHeight_ < 0) {
        return;
    }
    // need to get screen property if the landscape width is same to the portrait
    if (params.LandscapePanelRect_.width_ != params.PortraitPanelRect_.width_) {
        if (static_cast<uint32_t>(panelRect.width_) == params.LandscapePanelRect_.width_) {
            panelRect.posY_ += panelRect.height_ - params.landscapeAvoidHeight_;
            panelRect.height_ = params.landscapeAvoidHeight_;
            TLOGI(WmsLogTag::WMS_KEYBOARD, "LandscapeAvoidHeight %{public}d", panelRect.height_);
            return;
        }
        if (static_cast<uint32_t>(panelRect.width_) == params.PortraitPanelRect_.width_) {
            panelRect.posY_ += panelRect.height_ - params.portraitAvoidHeight_;
            panelRect.height_ = params.portraitAvoidHeight_;
            TLOGI(WmsLogTag::WMS_KEYBOARD, "PortraitAvoidHeight %{public}d", panelRect.height_);
            return;
        }
    }
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    bool result = GetScreenWidthAndHeightFromClient(sessionProperty, screenWidth, screenHeight);
    if (!result) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Get screen width and height failed");
        return;
    }
    bool isLandscape = screenHeight < screenWidth;
    int32_t height_ = isLandscape ? params.landscapeAvoidHeight_ : params.portraitAvoidHeight_;
    panelRect.posY_ += panelRect.height_ - height_;
    panelRect.height_ = height_;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "IsLandscape %{public}d, avoidHeight %{public}d", isLandscape, panelRect.height_);
}

WSError KeyboardSession::ChangeKeyboardViewMode(KeyboardViewMode mode)
{
    PostTask([weakThis = wptr(this), mode]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "Session is null, change keyboard view mode failed");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->GetSessionProperty()->SetKeyboardViewMode(mode);
        if (session->changeKeyboardViewModeFunc_) {
            session->changeKeyboardViewModeFunc_(mode);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

void KeyboardSession::NotifyRootSceneOccupiedAreaChange(const sptr<OccupiedAreaChangeInfo>& info)
{
    ScreenId defaultScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    auto displayId = GetSessionProperty()->GetDisplayId();
    if (displayId != defaultScreenId) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "DisplayId: %{public}" PRIu64", defaultScreenId: %{public}" PRIu64"",
            displayId, defaultScreenId);
        return;
    }
    if (keyboardCallback_ == nullptr || keyboardCallback_->onNotifyOccupiedAreaChange == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Callback is null");
        return;
    }
    keyboardCallback_->onNotifyOccupiedAreaChange(info);
}

bool KeyboardSession::IsNeedRaiseSubWindow(const sptr<SceneSession>& callingSession, const WSRect& callingSessionRect)
{
    if (!SessionHelper::IsSubWindow(callingSession->GetWindowType())) {
        TLOGD(WmsLogTag::WMS_KEYBOARD, "Not sub window");
        return true;
    }

    auto mainSession = callingSession->GetMainSession();
    if (mainSession != nullptr && WindowHelper::IsSplitWindowMode(mainSession->GetWindowMode()) &&
        callingSessionRect == mainSession->GetSessionRect()) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "No need to raise, parentId: %{public}d, rect: %{public}s",
            mainSession->GetPersistentId(), callingSessionRect.ToString().c_str());
        return false;
    }

    return true;
}

void KeyboardSession::SetSurfaceBounds(const WSRect& rect, bool isGlobal, bool needFlush)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "KeyboardSession::SetSurfaceBounds id:%d [%d, %d, %d, %d] reason:%u",
        GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason_);
    TLOGD(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, rect: %{public}s isGlobal: %{public}d needFlush: %{public}d",
        GetPersistentId(), rect.ToString().c_str(), isGlobal, needFlush);
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr && needFlush) {
        rsTransaction->Begin();
    }
    if (keyboardPanelSession_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboard panel session is null");
        return;
    }
    auto surfaceNode = keyboardPanelSession_->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboard panel surfacenode is null");
        return;
    }
    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        surfaceNode->SetGlobalPositionEnabled(isGlobal);
        surfaceNode->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode->SetFrame(rect.posX_, rect.posY_, rect.width_, rect.height_);
    }
    if (rsTransaction != nullptr && needFlush) {
        rsTransaction->Commit();
    }
}

void KeyboardSession::SetKeyboardViewModeChangeListener(const NotifyKeyboarViewModeChangeFunc& func)
{
    PostTask([weakThis = wptr(this), func, where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s session or keyboardViewModeChangeFunc is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->changeKeyboardViewModeFunc_ = func;
        return WSError::WS_OK;
    }, __func__);
}

WSRect KeyboardSession::GetPanelRect() const
{
    WSRect panelRect = { 0, 0, 0, 0 };
    if (keyboardPanelSession_ != nullptr) {
        panelRect = keyboardPanelSession_->GetSessionRect();
    } else {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Panel session is null, get panel rect failed");
    }
    return panelRect;
}

void KeyboardSession::SetSkipSelfWhenShowOnVirtualScreen(bool isSkip)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Set Skip keyboard, isSkip: %{public}d", isSkip);
    PostTask([weakThis = wptr(this), isSkip, where = __func__]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s: Session is null", where);
            return;
        }
        std::shared_ptr<RSSurfaceNode> surfaceNode = session->GetSurfaceNode();
        if (!surfaceNode) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s: SurfaceNode is null", where);
            return;
        }
        if (session->specificCallback_ != nullptr
            && session->specificCallback_->onSetSkipSelfWhenShowOnVirtualScreen_ != nullptr) {
            session->specificCallback_->onSetSkipSelfWhenShowOnVirtualScreen_(surfaceNode->GetId(), isSkip);
        }
    }, __func__);
}

void KeyboardSession::PostKeyboardAnimationSyncTimeoutTask()
{
    // anim_sync_exception
    int32_t const THRESHOLD = 1000;
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "keyboard session is null");
            return;
        }
        if (!session->GetIsKeyboardSyncTransactionOpen()) {
            TLOGND(WmsLogTag::WMS_KEYBOARD, "closed anim_sync in time");
            return;
        }
        std::string msg("close anim_sync timeout");
        WindowInfoReporter::GetInstance().ReportKeyboardLifeCycleException(
            session->GetPersistentId(),
            KeyboardLifeCycleException::ANIM_SYNC_EXCEPTION,
            msg);
    };
    auto handler = GetEventHandler();
    if (!handler) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "handler is null");
        return;
    }
    handler->PostTask(task, KEYBOARD_ANIM_SYNC_EVENT_NAME, THRESHOLD);
}

void KeyboardSession::SetSkipEventOnCastPlus(bool isSkip)
{
    PostTask([weakThis = wptr(this), isSkip, where = __func__]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_SCB, "%{public}s session is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_SCB, "%{public}s wid: %{public}d, isSkip: %{public}d", where,
            session->GetPersistentId(), isSkip);
        if (session->specificCallback_ != nullptr &&
            session->specificCallback_->onSetSkipEventOnCastPlus_ != nullptr) {
            session->specificCallback_->onSetSkipEventOnCastPlus_(session->GetPersistentId(), isSkip);
        }
    }, __func__);
}

WSError KeyboardSession::UpdateSizeChangeReason(SizeChangeReason reason)
{
    PostTask([weakThis = wptr(this), reason, where = __func__]() {
        auto keyboardSession = weakThis.promote();
        if (keyboardSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (reason == SizeChangeReason::DRAG_START || reason == SizeChangeReason::DRAG_MOVE ||
            reason == SizeChangeReason::DRAG_END || reason == SizeChangeReason::UNDEFINED) {
            auto panelSession = keyboardSession->GetKeyboardPanelSession();
            if (panelSession != nullptr) {
                panelSession->UpdateSizeChangeReason(reason);
            }
        }
        TLOGD(WmsLogTag::WMS_KEYBOARD, "%{public}s Id: %{public}d, reason: %{public}d",
            where, keyboardSession->GetPersistentId(), static_cast<int32_t>(reason));
        keyboardSession->SceneSession::UpdateSizeChangeReason(reason);
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen