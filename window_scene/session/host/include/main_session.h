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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MAIN_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_MAIN_SESSION_H

#include "session/host/include/scene_session.h"

namespace OHOS::Rosen {
class MainSession : public SceneSession {
public:
    MainSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    ~MainSession();

    WSError Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr, int32_t pid = -1, int32_t uid = -1) override;
    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;
    void NotifyForegroundInteractiveStatus(bool interactive) override;
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    void RectCheck(uint32_t curWidth, uint32_t curHeight) override;

    /**
     * Window Hierarchy
     */
    WSError SetTopmost(bool topmost) override;
    bool IsTopmost() const override;
    WSError SetMainWindowTopmost(bool isTopmost) override;
    bool IsMainWindowTopmost() const override;

    void SetExitSplitOnBackground(bool isExitSplitOnBackground) override;
    bool IsExitSplitOnBackground() const override;

    /**
     * PC Window
     */
    WSError OnTitleAndDockHoverShowChange(bool isTitleHoverShown = true,
        bool isDockHoverShown = true) override;
    WSError OnRestoreMainWindow() override;
    WSError OnSetWindowRectAutoSave(bool enabled) override;

protected:
    void UpdatePointerArea(const WSRect& rect) override;
    bool CheckPointerEventDispatch(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const override;
    void NotifyClientToUpdateInteractive(bool interactive) override;
    bool isClientInteractive_ = true;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MAIN_SESSION_H
