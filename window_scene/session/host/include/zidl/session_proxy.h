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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_PROXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_PROXY_H

#include <iremote_proxy.h>

#include "session/host/include/zidl/session_interface.h"
#include "ws_common.h"

namespace OHOS::Rosen {
enum class SessionInterfaceCode;
class SessionProxy : public IRemoteProxy<ISession> {
public:
    explicit SessionProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISession>(impl) {}
    virtual ~SessionProxy() = default;

    WSError Foreground(sptr<WindowSessionProperty> property, bool isFromClient = false,
        const std::string& identityToken = "") override;
    WSError Background(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError Disconnect(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        const std::string& identityToken = "") override;
    WSError DrawingCompleted() override;
    WSError ChangeSessionVisibilityWithStatusBar(const sptr<AAFwk::SessionInfo> abilitySessionInfo,
        bool visible) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    WSError NotifySessionException(
        const sptr<AAFwk::SessionInfo> abilitySessionInfo, const ExceptionInfo& exceptionInfo) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError SyncSessionEvent(SessionEvent event) override;
    WSError OnLayoutFullScreenChange(bool isLayoutFullScreen) override;
    WSError OnDefaultDensityEnabled(bool isDefaultDensityEnabled) override;
    WSError OnTitleAndDockHoverShowChange(bool isTitleHoverShown = true,
        bool isDockHoverShown = true) override;
    WSError OnRestoreMainWindow() override;
    WSError OnSetWindowRectAutoSave(bool enabled) override;
    WSError RaiseToAppTop() override;
    WSError UpdateSessionRect(const WSRect& rect, SizeChangeReason reason, bool isGlobal = false,
        bool isFromMoveToGlobal = false, const MoveConfiguration& moveConfiguration = {},
        const RectAnimationConfig& rectAnimationConfig = {}) override;
    WMError GetGlobalScaledRect(Rect& globalScaledRect) override;
    WSError UpdateClientRect(const WSRect& rect) override;
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type, const WSRect& rect = WSRect::EMPTY_RECT,
        int32_t apiVersion = API_VERSION_INVALID) override;
    WSError GetAllAvoidAreas(std::map<AvoidAreaType, AvoidArea>& avoidAreas) override;
    WSError RequestSessionBack(bool needMoveToBackground) override;
    WSError MarkProcessed(int32_t eventId) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    WSError SetAspectRatio(float ratio) override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    WSError SetLandscapeMultiWindow(bool isLandscapeMultiWindow) override;
    WSError GetIsMidScene(bool& isMidScene) override;
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    WSError RaiseAboveTarget(int32_t subWindowId) override;
    WSError RaiseAppMainWindowToTop() override;
    WSError SetSessionLabelAndIcon(const std::string& label,
        const std::shared_ptr<Media::PixelMap>& icon) override;

    /*
     * UIExtension
     */
    WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) override;
    WSError TransferExtensionData(const AAFwk::WantParams& wantParams) override;
    WSError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) override;
    void NotifySyncOn() override;
    void NotifyAsyncOn() override;
    void NotifyExtensionDied() override;
    void NotifyExtensionTimeout(int32_t errorCode) override;
    void TriggerBindModalUIExtension() override;
    void NotifyExtensionEventAsync(uint32_t notifyEvent) override;
    WSError SendExtensionData(MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

    void NotifyPiPWindowPrepareClose() override;
    WSError UpdatePiPRect(const Rect& rect, SizeChangeReason reason) override;
    WSError UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status) override;
    WSError SetAutoStartPiP(bool isAutoStart, uint32_t priority, uint32_t width, uint32_t height) override;

    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;
    WSError SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool isExecuteDelayRaise = false) override;
    bool IsStartMoving() override;
    WSError UpdateRectChangeListenerRegistered(bool isRegister) override;
    void SetCallingSessionId(uint32_t callingSessionId) override;
    void NotifyKeyboardDidShowRegistered(bool registered) override;
    void NotifyKeyboardDidHideRegistered(bool registered) override;
    void SetCustomDecorHeight(int32_t height) override;
    WSError AdjustKeyboardLayout(const KeyboardLayoutParams& params) override;
    WSError ChangeKeyboardViewMode(KeyboardViewMode mode) override;
    WMError UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action) override;
    WMError GetAppForceLandscapeConfig(AppForceLandscapeConfig& config) override;
    WSError NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect) override;
    WSError SetDialogSessionBackGestureEnabled(bool isEnabled) override;
    int32_t GetStatusBarHeight() override;
    WMError SetSystemWindowEnableDrag(bool enableDrag) override;
    void NotifyExtensionDetachToDisplay() override;
    WSError RequestFocus(bool isFocused) override;
    
    /*
     * Gesture Back
     */
    WMError SetGestureBackEnabled(bool isEnabled) override;

    WSError NotifySubModalTypeChange(SubWindowModalType subWindowModalType) override;
    WSError NotifyMainModalTypeChange(bool isModal) override;

    /*
     * Starting Window
     */
    WSError RemoveStartingWindow() override;

    /*
     * Window Property
     */
    WSError SetWindowCornerRadius(float cornerRadius) override;

    WSError NotifySupportWindowModesChange(
        const std::vector<AppExecFwk::SupportWindowMode>& supportedWindowModes) override;
    WSError GetCrossAxisState(CrossAxisState& state) override;

    /*
     * PC Window
     */
    WSError StartMovingWithCoordinate(int32_t offsetX, int32_t offsetY,
        int32_t pointerPosX, int32_t pointerPosY) override;
    WSError OnContainerModalEvent(const std::string& eventName, const std::string& eventValue) override;

    /*
     * Window Pattern
     */
    void NotifyWindowAttachStateListenerRegistered(bool registered) override;

    /**
     * Window layout
     */
    WSError SetFollowParentWindowLayoutEnabled(bool isFollow) override;

private:
    static inline BrokerDelegator<SessionProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_PROXY_H
