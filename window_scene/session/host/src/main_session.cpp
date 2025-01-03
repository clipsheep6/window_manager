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

#include "session/host/include/main_session.h"

#include "common/include/fold_screen_state_internel.h"
#include "session_helper.h"
#include "window_helper.h"
#include "session/host/include/scene_persistent_storage.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MainSession" };
} // namespace

MainSession::MainSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    scenePersistence_ = sptr<ScenePersistence>::MakeSptr(info.bundleName_, GetPersistentId());
    if (info.persistentId_ != 0 && info.persistentId_ != GetPersistentId()) {
        // persistentId changed due to id conflicts. Need to rename the old snapshot if exists
        scenePersistence_->RenameSnapshotFromOldPersistentId(info.persistentId_);
    }
    pcFoldScreenController_ = sptr<PcFoldScreenController>::MakeSptr(wptr(this), GetPersistentId());
    moveDragController_ = sptr<MoveDragController>::MakeSptr(GetPersistentId(), GetWindowType());
    if (specificCallback != nullptr &&
        specificCallback->onWindowInputPidChangeCallback_ != nullptr) {
        moveDragController_->SetNotifyWindowPidChangeCallback(specificCallback->onWindowInputPidChangeCallback_);
    }
    SetMoveDragCallback();
    std::string key = GetRatioPreferenceKey();
    if (!key.empty()) {
        if (ScenePersistentStorage::HasKey(key, ScenePersistentStorageType::ASPECT_RATIO)) {
            ScenePersistentStorage::Get(key, aspectRatio_, ScenePersistentStorageType::ASPECT_RATIO);
            WLOGFD("init aspectRatio, key %{public}s, value: %{public}f", key.c_str(), aspectRatio_);
            moveDragController_->SetAspectRatio(aspectRatio_);
        }
    }

    WLOGFD("Create MainSession");
}

MainSession::~MainSession()
{
    WLOGD("~MainSession, id: %{public}d", GetPersistentId());
}

WSError MainSession::Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token,
    int32_t pid, int32_t uid)
{
    return PostSyncTask([weakThis = wptr(this), sessionStage, eventChannel, surfaceNode, property, token, pid, uid]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WSError ret = session->Session::Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        WindowState windowState = property->GetWindowState();
        if (windowState == WindowState::STATE_SHOWN) {
            session->isActive_ = true;
            session->UpdateSessionState(SessionState::STATE_ACTIVE);
        } else {
            session->isActive_ = false;
            session->UpdateSessionState(SessionState::STATE_BACKGROUND);
            if (session->scenePersistence_) {
                session->scenePersistence_->SetHasSnapshot(true);
            }
        }
        return ret;
    });
}

WSError MainSession::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    const auto& id = GetPersistentId();
    WLOGFI("id: %{public}d, type: %{public}d", id, GetWindowType());
    auto isModal = IsModal();
    if (!isModal && CheckDialogOnForeground()) {
        HandlePointDownDialog();
        return WSError::WS_OK;
    }
    if (isModal) {
        Session::ProcessClickModalWindowOutside(posX, posY);
    }
    PresentFocusIfPointDown();
    return SceneSession::ProcessPointDownSession(posX, posY);
}

void MainSession::NotifyForegroundInteractiveStatus(bool interactive)
{
    SetForegroundInteractiveStatus(interactive);
    if (!IsSessionValid() || !sessionStage_) {
        TLOGW(WmsLogTag::WMS_MAIN, "Session or sessionStage is invalid, id: %{public}d state: %{public}u",
            GetPersistentId(), GetSessionState());
        return;
    }
    const auto& state = GetSessionState();
    if (isVisible_ || state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND) {
        WLOGFI("NotifyForegroundInteractiveStatus %{public}d", interactive);
        sessionStage_->NotifyForegroundInteractiveStatus(interactive);
    }
}

WSError MainSession::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (CheckDialogOnForeground()) {
        TLOGD(WmsLogTag::WMS_DIALOG, "Has dialog on foreground, not transfer pointer event");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    WSError ret = Session::TransferKeyEvent(keyEvent);
    return ret;
}

void MainSession::UpdatePointerArea(const WSRect& rect)
{
    if (GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING) {
        return;
    }
    Session::UpdatePointerArea(rect);
}

bool MainSession::CheckPointerEventDispatch(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const
{
    auto sessionState = GetSessionState();
    int32_t action = pointerEvent->GetPointerAction();
    if (sessionState != SessionState::STATE_FOREGROUND &&
        sessionState != SessionState::STATE_ACTIVE &&
        action != MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW) {
        WLOGFW("Current Session Info: [persistentId: %{public}d, "
            "state: %{public}d, action:%{public}d]", GetPersistentId(), GetSessionState(), action);
        return false;
    }
    return true;
}

WSError MainSession::SetTopmost(bool topmost)
{
    TLOGI(WmsLogTag::WMS_HIERARCHY, "SetTopmost id: %{public}d, topmost: %{public}d", GetPersistentId(), topmost);
    PostTask([weakThis = wptr(this), topmost]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_HIERARCHY, "session is null");
            return;
        }
        auto property = session->GetSessionProperty();
        if (property) {
            TLOGI(WmsLogTag::WMS_HIERARCHY, "Notify session topmost change, id: %{public}d, topmost: %{public}u",
                session->GetPersistentId(), topmost);
            property->SetTopmost(topmost);
            if (session->onSessionTopmostChange_) {
                session->onSessionTopmostChange_(topmost);
            }
        }
    }, "SetTopmost");
    return WSError::WS_OK;
}

bool MainSession::IsTopmost() const
{
    return GetSessionProperty()->IsTopmost();
}

WSError MainSession::SetMainWindowTopmost(bool isTopmost)
{
    GetSessionProperty()->SetMainWindowTopmost(isTopmost);
    TLOGD(WmsLogTag::WMS_HIERARCHY, "id: %{public}d, isTopmost: %{public}u",
        GetPersistentId(), isTopmost);
    if (mainWindowTopmostChangeFunc_) {
        mainWindowTopmostChangeFunc_(isTopmost);
    }
    return WSError::WS_OK;
}

bool MainSession::IsMainWindowTopmost() const
{
    return GetSessionProperty()->IsMainWindowTopmost();
}

void MainSession::RectCheck(uint32_t curWidth, uint32_t curHeight)
{
    uint32_t minWidth = GetSystemConfig().miniWidthOfMainWindow_;
    uint32_t minHeight = GetSystemConfig().miniHeightOfMainWindow_;
    uint32_t maxFloatingWindowSize = GetSystemConfig().maxFloatingWindowSize_;
    RectSizeCheckProcess(curWidth, curHeight, minWidth, minHeight, maxFloatingWindowSize);
}

void MainSession::SetExitSplitOnBackground(bool isExitSplitOnBackground)
{
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "id: %{public}d, isExitSplitOnBackground: %{public}d", persistentId_,
        isExitSplitOnBackground);
    isExitSplitOnBackground_ = isExitSplitOnBackground;
}

bool MainSession::IsExitSplitOnBackground() const
{
    return isExitSplitOnBackground_;
}

void MainSession::NotifyClientToUpdateInteractive(bool interactive)
{
    if (!sessionStage_) {
        return;
    }
    const auto state = GetSessionState();
    if (state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND) {
        WLOGFI("%{public}d", interactive);
        sessionStage_->NotifyForegroundInteractiveStatus(interactive);
        isClientInteractive_ = interactive;
    }
}

/*
 * Notify when updating highlight instead after hightlight functionality enabled
 */
WSError MainSession::UpdateFocus(bool isFocused)
{
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice() && !IsFocused() && isFocused && pcFoldScreenController_) {
        pcFoldScreenController_->UpdateSupportEnterWaterfallMode();
    }
    return Session::UpdateFocus(isFocused);
}

WSError MainSession::OnTitleAndDockHoverShowChange(bool isTitleHoverShown, bool isDockHoverShown)
{
    const char* const funcName = __func__;
    PostTask([weakThis = wptr(this), isTitleHoverShown, isDockHoverShown, funcName] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session is null", funcName);
            return;
        }
        TLOGND(WmsLogTag::WMS_LAYOUT_PC, "%{public}s isTitleHoverShown: %{public}d, isDockHoverShown: %{public}d",
            funcName, isTitleHoverShown, isDockHoverShown);
        if (session->onTitleAndDockHoverShowChangeFunc_) {
            session->onTitleAndDockHoverShowChangeFunc_(isTitleHoverShown, isDockHoverShown);
        }
    }, funcName);
    return WSError::WS_OK;
}

WSError MainSession::OnRestoreMainWindow()
{
    PostTask([weakThis = wptr(this)] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "session is null");
            return;
        }
        if (session->onRestoreMainWindowFunc_) {
            session->onRestoreMainWindowFunc_();
        }
    }, __func__);
    return WSError::WS_OK;
}

WSError MainSession::OnSetWindowRectAutoSave(bool enabled)
{
    PostTask([weakThis = wptr(this), enabled] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_MAIN, "session is null");
            return;
        }
        if (session->onSetWindowRectAutoSaveFunc_) {
            session->onSetWindowRectAutoSaveFunc_(enabled);
        }
    }, __func__);
    return WSError::WS_OK;
}

WSError MainSession::NotifySupportWindowModesChange(
    const std::vector<AppExecFwk::SupportWindowMode>& supportWindowModes)
{
    const char* const where = __func__;
    PostTask([weakThis = wptr(this), supportWindowModes = supportWindowModes, where]() mutable {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session is null", where);
            return;
        }
        if (session->onSetSupportWindowModesFunc_) {
            session->onSetSupportWindowModesFunc_(std::move(supportWindowModes));
        }
    }, __func__);
    return WSError::WS_OK;
}

WSError MainSession::NotifyMainModalTypeChange(bool isModal)
{
    const char* const where = __func__;
    PostTask([weakThis = wptr(this), isModal, where] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_HIERARCHY, "%{public}s main window isModal:%{public}d", where, isModal);
        if (session->onMainModalTypeChange_) {
            session->onMainModalTypeChange_(isModal);
        }
    }, __func__);
    return WSError::WS_OK;
}

bool MainSession::IsModal() const
{
    return WindowHelper::IsModalMainWindow(GetSessionProperty()->GetWindowType(),
                                           GetSessionProperty()->GetWindowFlags());
}

bool MainSession::IsApplicationModal() const
{
    return IsModal();
}
} // namespace OHOS::Rosen
