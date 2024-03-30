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

#ifndef OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H

#include "session/host/include/system_session.h"

namespace OHOS::Rosen {
class KeyboardSession : public SystemSession {
public:
    KeyboardSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    ~KeyboardSession();

    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError Disconnect(bool isFromClient = false) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction) override;

    WSRect callingWindowRestoringRect_ = {0, 0, 0, 0};
    WSRect callingWindowRaisedRect_ = {0, 0, 0, 0};
    bool isNeedUpdateSession_ = false;

private:
    sptr<SceneSession> GetSceneSession(uint32_t persistendId);
    int32_t GetFocusedSessionId();

    void SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) override;
    double GetTextFieldPositionY();
    double GetTextFieldHeight();
    WSError SetSessionGravity(SessionGravity gravity, uint32_t percent) override;
    void SetCallingWindowId(uint32_t callingWindowId) override;
    uint32_t GetCallingWindowId() override;

    bool IsStatusBarVisible(const sptr<SceneSession>& session);
    int32_t GetStatusBarHeight();
    void NotifyOccupiedAreaChangeInfo(const sptr<SceneSession> callingSession, const WSRect& rect, const WSRect& occupiedArea);
    void RaiseCallingWindow(bool isInputUpdated = false);
    void RestoreCallingWindow();
    void UseFocusIdIfCallingWindowIdInvalid();
    void OnKeyboardShown();
    /**
     * @brief Adjust the position of the callingWindow when the keyboard window level is raised.
     */
    void UpdateCallingWindowIdAndPosition(uint32_t callingWindowId);
    void RelayoutKeyBoard();

    double textFieldPositionY_ = 0.0;
    double textFieldHeight_ = 0.0;
    uint32_t callingWindowId_ = INVALID_WINDOW_ID;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_SESSION_H