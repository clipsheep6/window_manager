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

        auto ret = session->Session::Foreground(property);
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
        session->NotifyCallingSessionBackground();
        return WSError::WS_OK;
    };
    PostTask(task, "Disconnect");
    return WSError::WS_OK;
}

WSError KeyboardSession::SetCallingWindowId(uint32_t callingWindowId)
{
    callingWindowId_ = callingWindowId;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "set callingWindowId_: %{public}d", callingWindowId_);
    return WSError::WS_OK;
}

uint32_t KeyboardSession::GetCallingWindowId()
{
    return callingWindowId_;
}

sptr<SceneSession> KeyboardSession::GetSceneSession(uint32_t persistendId)
{
    if (specificCallback_ == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "specificCallback_ is nullptr");
        return nullptr;
    }
    return specificCallback_->onGetSceneSession_(persistendId);
}

void KeyboardSession::GetKeyboardSessionRect(WSRect &keyboardSessionRect)
{
    auto task = [weakThis = wptr(this), &keyboardSessionRect]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        keyboardSessionRect = session->GetSessionRect();
        return WSError::WS_OK;
    };
    PostSyncTask(task, "GetKeyboardSessionRect");
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
    if (specificCallback_ == nullptr) {
        return statusBarHeight;
    }
    std::vector<sptr<SceneSession>> statusBarVector =
        specificCallback_->onGetSceneSessionVectorByType_(WindowType::WINDOW_TYPE_STATUS_BAR);
    for (auto& statusBar : statusBarVector) {
        if (statusBar == nullptr || !IsStatusBarVisible(statusBar)) {
            continue;
        }
        height = statusBar->GetSessionRect().height_;
        statusBarHeight = (statusBarHeight > height) ? statusBarHeight : height;
    }

    return statusBarHeight;
}
} // namespace OHOS::Rosen
