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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H

#include <modifier/rs_property.h>
#include <modifier/rs_property_modifier.h>

#include "display_manager.h"
#include "session/host/include/session.h"
#include "session/host/include/move_drag_controller.h"
#include "session/host/include/pc_fold_screen_controller.h"
#include "vsync_station.h"
#include "wm_common.h"

namespace OHOS::PowerMgr {
class RunningLock;
}

namespace OHOS::Rosen {
namespace PARAM_KEY {
const std::string PARAM_MISSION_AFFINITY_KEY = "ohos.anco.param.missionAffinity";
const std::string PARAM_DMS_CONTINUE_SESSION_ID_KEY = "ohos.dms.continueSessionId";
const std::string PARAM_DMS_PERSISTENT_ID_KEY = "ohos.dms.persistentId";
}

class SceneSession;

using NotifySessionLockStateChangeCallback = std::function<void(bool isLockedState)>;
using SpecificSessionCreateCallback =
  std::function<sptr<SceneSession>(const SessionInfo& info, sptr<WindowSessionProperty> property)>;
using SpecificSessionDestroyCallback = std::function<WSError(const int32_t& persistentId)>;
using CameraFloatSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using GetSceneSessionVectorByTypeAndDisplayIdCallback = std::function<std::vector<sptr<SceneSession>>(
    WindowType type, DisplayId displayId)>;
using GetSceneSessionVectorByTypeCallback = std::function<std::vector<sptr<SceneSession>>(WindowType type)>;
using UpdateAvoidAreaCallback = std::function<void(int32_t persistentId)>;
using UpdateAvoidAreaByTypeCallback = std::function<void(int32_t persistentId, AvoidAreaType type)>;
using UpdateOccupiedAreaIfNeedCallback = std::function<void(int32_t persistentId)>;
using NotifyWindowInfoUpdateCallback = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using NotifyWindowPidChangeCallback = std::function<void(int32_t windowId, bool startMoving)>;
using NotifySessionTouchOutsideCallback = std::function<void(int32_t persistentId)>;
using GetAINavigationBarArea = std::function<WSRect(uint64_t displayId)>;
using RecoveryCallback = std::function<void(int32_t persistentId, Rect rect)>;
using NotifyBindDialogSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionPiPControlStatusChangeFunc = std::function<void(WsPiPControlType controlType,
    WsPiPControlStatus status)>;
using NotifyAutoStartPiPStatusChangeFunc = std::function<void(bool isAutoStart, uint32_t priority)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId, SessionEventParam param)>;
using NotifySessionTopmostChangeFunc = std::function<void(const bool topmost)>;
using NotifySubModalTypeChangeFunc = std::function<void(SubWindowModalType subWindowModalType)>;
using NotifyMainModalTypeChangeFunc = std::function<void(bool isModal)>;
using NotifyRaiseToTopFunc = std::function<void()>;
using SetWindowPatternOpacityFunc = std::function<void(float opacity)>;
using NotifyIsCustomAnimationPlayingCallback = std::function<void(bool isFinish)>;
using NotifyWindowAnimationFlagChangeFunc = std::function<void(const bool flag)>;
using NotifySystemBarPropertyChangeFunc = std::function<void(
    const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)>;
using NotifyNeedAvoidFunc = std::function<void(bool status)>;
using NotifyShowWhenLockedFunc = std::function<void(bool showWhenLocked)>;
using NotifyReqOrientationChangeFunc = std::function<void(uint32_t orientation)>;
using NotifyRaiseAboveTargetFunc = std::function<void(int32_t subWindowId)>;
using NotifyForceHideChangeFunc = std::function<void(bool hide)>;
using NotifyTouchOutsideFunc = std::function<void()>;
using ClearCallbackMapFunc = std::function<void(bool needRemove)>;
using NotifyPrepareClosePiPSessionFunc = std::function<void()>;
using OnOutsideDownEvent = std::function<void(int32_t x, int32_t y)>;
using HandleSecureSessionShouldHideCallback = std::function<WSError(const sptr<SceneSession>& sceneSession)>;
using ClearDisplayStatusBarTemporarilyFlags = std::function<void()>;
using CameraSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using NotifyLandscapeMultiWindowSessionFunc = std::function<void(bool isLandscapeMultiWindow)>;
using NotifyKeyboardLayoutAdjustFunc = std::function<void(const KeyboardLayoutParams& params)>;
using NotifyKeyboarViewModeChangeFunc = std::function<void(const KeyboardViewMode& mode)>;
using SessionChangeByActionNotifyManagerFunc = std::function<void(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action)>;
using NotifyLayoutFullScreenChangeFunc = std::function<void(bool isLayoutFullScreen)>;
using NotifyDefaultDensityEnabledFunc = std::function<void(bool isDefaultDensityEnabled)>;
using NotifyTitleAndDockHoverShowChangeFunc = std::function<void(bool isTitleHoverShown,
    bool isDockHoverShown)>;
using NotifyRestoreMainWindowFunc = std::function<void()>;
using SetSkipSelfWhenShowOnVirtualScreenCallback = std::function<void(uint64_t surfaceNodeId, bool isSkip)>;
using NotifyForceSplitFunc = std::function<AppForceLandscapeConfig(const std::string& bundleName)>;
using UpdatePrivateStateAndNotifyFunc = std::function<void(int32_t persistentId)>;
using PiPStateChangeCallback = std::function<void(const std::string& bundleName, bool isForeground)>;
using NotifyMainWindowTopmostChangeFunc = std::function<void(bool isTopmost)>;
using NotifyPrivacyModeChangeFunc = std::function<void(uint32_t isPrivacyMode)>;
using UpdateGestureBackEnabledCallback = std::function<void(int32_t persistentId)>;
using NotifyVisibleChangeFunc = std::function<void(int32_t persistentId)>;
using IsLastFrameLayoutFinishedFunc = std::function<WSError(bool& isLayoutFinished)>;
using GetStatusBarDefaultVisibilityByDisplayIdFunc = std::function<bool(DisplayId displayId)>;
using NotifySetWindowRectAutoSaveFunc = std::function<void(bool enabled)>;
using UpdateAppUseControlFunc = std::function<void(ControlAppType type, bool isNeedControl)>;
using NotifyAvoidAreaChangeCallback = std::function<void(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)>;
using NotifySetSupportedWindowModesFunc = std::function<void(
    std::vector<AppExecFwk::SupportWindowMode>&& supportedWindowModes)>;
using GetStatusBarAvoidHeightFunc = std::function<void(WSRect& barArea)>;

struct UIExtensionTokenInfo {
    bool canShowOnLockScreen { false };
    uint32_t callingTokenId { 0 };
    sptr<IRemoteObject> abilityToken;
};

class SceneSession : public Session {
public:
    friend class HidumpController;
    friend class PcFoldScreenController;
    // callback for notify SceneSessionManager
    struct SpecificSessionCallback : public RefBase {
        SpecificSessionCreateCallback onCreate_;
        SpecificSessionDestroyCallback onDestroy_;
        ClearDisplayStatusBarTemporarilyFlags onClearDisplayStatusBarTemporarilyFlags_;
        CameraFloatSessionChangeCallback onCameraFloatSessionChange_;
        GetSceneSessionVectorByTypeAndDisplayIdCallback onGetSceneSessionVectorByTypeAndDisplayId_;
        GetSceneSessionVectorByTypeCallback onGetSceneSessionVectorByType_;
        UpdateAvoidAreaCallback onUpdateAvoidArea_;
        UpdateAvoidAreaByTypeCallback onUpdateAvoidAreaByType_;
        GetStatusBarDefaultVisibilityByDisplayIdFunc onGetStatusBarDefaultVisibilityByDisplayId_;
        UpdateOccupiedAreaIfNeedCallback onUpdateOccupiedAreaIfNeed_;
        NotifyWindowInfoUpdateCallback onWindowInfoUpdate_;
        NotifyWindowPidChangeCallback onWindowInputPidChangeCallback_;
        NotifySessionTouchOutsideCallback onSessionTouchOutside_;
        GetAINavigationBarArea onGetAINavigationBarArea_;
        OnOutsideDownEvent onOutsideDownEvent_;
        HandleSecureSessionShouldHideCallback onHandleSecureSessionShouldHide_;
        CameraSessionChangeCallback onCameraSessionChange_;
        SetSkipSelfWhenShowOnVirtualScreenCallback onSetSkipSelfWhenShowOnVirtualScreen_;
        PiPStateChangeCallback onPiPStateChange_;
        UpdateGestureBackEnabledCallback onUpdateGestureBackEnabled_;
        NotifyAvoidAreaChangeCallback onNotifyAvoidAreaChange_;
    };

    // func for change window scene pattern property
    struct SetWindowScenePatternFunc : public RefBase {
        SetWindowPatternOpacityFunc setOpacityFunc_;
    };

    SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    virtual ~SceneSession();

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        const std::string& identityToken = "") override;
    WSError ConnectInner(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        int32_t pid = -1, int32_t uid = -1, const std::string& identityToken = "") override;
    WSError Foreground(sptr<WindowSessionProperty> property, bool isFromClient = false,
        const std::string& identityToken = "") override;
    WSError Background(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError BackgroundTask(const bool isSaveSnapshot = true);
    WSError Disconnect(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError DisconnectTask(bool isFromClient = false, bool isSaveSnapshot = true);
    void SetClientIdentityToken(const std::string& clientIdentityToken);
    virtual void BindKeyboardPanelSession(sptr<SceneSession> panelSession) {};
    virtual sptr<SceneSession> GetKeyboardPanelSession() const { return nullptr; };
    virtual void BindKeyboardSession(sptr<SceneSession> session) {};
    virtual sptr<SceneSession> GetKeyboardSession() const { return nullptr; };
    virtual SessionGravity GetKeyboardGravity() const { return SessionGravity::SESSION_GRAVITY_DEFAULT; };
    virtual void OnKeyboardPanelUpdated() {};
    virtual void OnCallingSessionUpdated() {};
    virtual uint32_t GetCallingSessionId() { return INVALID_SESSION_ID; };
    bool GetScreenWidthAndHeightFromServer(const sptr<WindowSessionProperty>& sessionProperty,
        uint32_t& screenWidth, uint32_t& screenHeight);
    bool GetScreenWidthAndHeightFromClient(const sptr<WindowSessionProperty>& sessionProperty,
        uint32_t& screenWidth, uint32_t& screenHeight);
    void NotifyTargetScreenWidthAndHeight(bool isScreenAngleMismatch, uint32_t screenWidth,
        uint32_t screenHeight);

    WSError UpdateActiveStatus(bool isActive) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError SyncSessionEvent(SessionEvent event) override;
    WSError OnLayoutFullScreenChange(bool isLayoutFullScreen) override;
    WSError RaiseToAppTop() override;

    /*
     * Window Recover
     */
    virtual WSError Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr, int32_t pid = -1, int32_t uid = -1);
    WSError ReconnectInner(sptr<WindowSessionProperty> property) REQUIRES(SCENE_GUARD);
    bool IsRecovered() const { return isRecovered_; }
    void SetRecovered(bool isRecovered) { isRecovered_ = isRecovered; }

    /*
     * Window Layout
     */
    WSError UpdateSizeChangeReason(SizeChangeReason reason) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::string& updateReason, const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    WSError UpdateSessionRect(const WSRect& rect, SizeChangeReason reason, bool isGlobal = false,
        bool isFromMoveToGlobal = false, const MoveConfiguration& moveConfiguration = {},
        const RectAnimationConfig& rectAnimationConfig = {}) override;
    WSError UpdateClientRect(const WSRect& rect) override;
    void NotifySingleHandTransformChange(const SingleHandTransform& singleHandTransform);
    void UpdateSessionState(SessionState state) override;
    WSError NotifyClientToUpdateRect(const std::string& updateReason,
        std::shared_ptr<RSTransaction> rsTransaction) override;

    virtual void OpenKeyboardSyncTransaction() {}
    virtual void CloseKeyboardSyncTransaction(const WSRect& keyboardPanelRect,
        bool isKeyboardShow, bool isRotating) {}
    WSError ChangeSessionVisibilityWithStatusBar(const sptr<AAFwk::SessionInfo> info, bool visible) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;
    bool DisallowActivationFromPendingBackground(bool isPcOrPadEnableActivation, bool isFoundationCall,
        bool canStartAbilityFromBackground, bool isFromAncoAndToAnco);
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    WSError NotifySessionException(
        const sptr<AAFwk::SessionInfo> info, bool needRemoveSession = false) override;
    WSError NotifySessionExceptionInner(
        const sptr<AAFwk::SessionInfo> info, bool needRemoveSession = false,
        bool isFromClient = false, bool startFail = false);

    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool needNotifyClient = true, bool isExecuteDelayRaise = false) override;
    WSError TransferPointerEventInner(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool needNotifyClient = true, bool isExecuteDelayRaise = false);
    WSError RequestSessionBack(bool needMoveToBackground) override;
    WSError SetAspectRatio(float ratio) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    void SetZOrder(uint32_t zOrder) override;
    std::vector<Rect> GetTouchHotAreas() const override;
    void NotifyUILostFocus() override;
    void SetScale(float scaleX, float scaleY, float pivotX, float pivotY) override;
    void SetFloatingScale(float floatingScale) override;
    WSError RaiseAboveTarget(int32_t subWindowId) override;

    /*
     * PiP Window
     */
    PiPTemplateInfo GetPiPTemplateInfo() const;
    void SetPiPTemplateInfo(const PiPTemplateInfo& pipTemplateInfo);
    WSError UpdatePiPRect(const Rect& rect, SizeChangeReason reason) override;
    WSError UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status) override;
    WSError SetAutoStartPiP(bool isAutoStart, uint32_t priority) override;
    void NotifyPiPWindowPrepareClose() override;
    void SetSessionPiPControlStatusChangeCallback(const NotifySessionPiPControlStatusChangeFunc& func);
    void SetAutoStartPiPStatusChangeCallback(const NotifyAutoStartPiPStatusChangeFunc& func);
    WSError SetPipActionEvent(const std::string& action, int32_t status);
    WSError SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status);
    void RegisterProcessPrepareClosePiPCallback(NotifyPrepareClosePiPSessionFunc&& callback);

    void RequestHideKeyboard(bool isAppColdStart = false);
    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;
    WSError SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool isExecuteDelayRaise = false) override;
    void NotifyOutsideDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    WSError NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect) override;
    void SetForegroundInteractiveStatus(bool interactive) override;
    WSError SetLandscapeMultiWindow(bool isLandscapeMultiWindow) override;

    /*
     * Window Layout
     */
    WMError ActivateDragBySystem(bool activateDrag);
    WMError SetSystemWindowEnableDrag(bool enableDrag) override;
    WMError SetWindowEnableDragBySystem(bool enableDrag);
    WSError OnDefaultDensityEnabled(bool isDefaultDensityEnabled) override;
    void RegisterDefaultDensityEnabledCallback(NotifyDefaultDensityEnabledFunc&& callback);
    void SetSessionDisplayIdChangeCallback(NotifySessionDisplayIdChangeFunc&& func);

    WSError SetKeepScreenOn(bool keepScreenOn);
    void SetParentPersistentId(int32_t parentId);
    WSError SetTurnScreenOn(bool turnScreenOn);
    void SetPrivacyMode(bool isPrivacy);
    void SetSnapshotSkip(bool isSkip);
    void SetSystemSceneOcclusionAlpha(double alpha);
    void ResetOcclusionAlpha();
    void SetSystemSceneForceUIFirst(bool forceUIFirst);
    void MarkSystemSceneUIFirst(bool isForced, bool isUIFirstEnabled);
    void SetRequestedOrientation(Orientation orientation);
    WSError SetDefaultRequestedOrientation(Orientation orientation);
    void SetWindowAnimationFlag(bool needDefaultAnimationFlag);
    void SetCollaboratorType(int32_t collaboratorType);
    void SetLastSafeRect(WSRect rect);
    void SetMovable(bool isMovable);
    void SetOriPosYBeforeRaisedByKeyboard(int32_t posY);

    /*
     * Window Hierarchy
     */
    virtual WSError SetTopmost(bool topmost) { return WSError::WS_ERROR_INVALID_CALLING; }
    virtual bool IsTopmost() const { return false; }
    virtual WSError SetMainWindowTopmost(bool isTopmost) { return WSError::WS_ERROR_INVALID_CALLING; }
    virtual bool IsMainWindowTopmost() const { return false; }
    void SetMainWindowTopmostChangeCallback(NotifyMainWindowTopmostChangeFunc&& func);

    /*
     * PC Window
     */
    void SetTitleAndDockHoverShowChangeCallback(NotifyTitleAndDockHoverShowChangeFunc&& func);
    void SetRestoreMainWindowCallback(NotifyRestoreMainWindowFunc&& func);
    void SetWindowRectAutoSaveCallback(NotifySetWindowRectAutoSaveFunc&& func);
    virtual bool IsModal() const { return false; }
    virtual bool IsApplicationModal() const { return false; }
    bool IsDialogWindow() const;
    WSError NotifySubModalTypeChange(SubWindowModalType subWindowModalType) override;
    void RegisterSubModalTypeChangeCallback(NotifySubModalTypeChangeFunc&& func);
    void RegisterMainModalTypeChangeCallback(NotifyMainModalTypeChangeFunc&& func);
    void RegisterSupportWindowModesCallback(NotifySetSupportedWindowModesFunc&& func);
    void CloneWindow(NodeId surfaceNodeId);

    /*
     * PC Window Layout
     */
    void SetIsLayoutFullScreen(bool isLayoutFullScreen);
    bool IsLayoutFullScreen() const;

    /*
     * Window Immersive
     */
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type, const WSRect& rect = WSRect::EMPTY_RECT) override;
    WSError GetAllAvoidAreas(std::map<AvoidAreaType, AvoidArea>& avoidAreas) override;
    WSError SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty);
    void SetIsStatusBarVisible(bool isVisible);
    WSError SetIsStatusBarVisibleInner(bool isVisible);
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    void UpdateRotationAvoidArea();
    bool CheckGetAvoidAreaAvailable(AvoidAreaType type) override;
    bool GetIsDisplayStatusBarTemporarily() const;
    void SetIsDisplayStatusBarTemporarily(bool isTemporary);
    void SetIsLastFrameLayoutFinishedFunc(IsLastFrameLayoutFinishedFunc&& func);
    void RetrieveStatusBarDefaultVisibility();
    void RegisterNeedAvoidCallback(NotifyNeedAvoidFunc&& callback);
    void RegisterSystemBarPropertyChangeCallback(NotifySystemBarPropertyChangeFunc&& callback);
    void MarkAvoidAreaAsDirty();
    virtual void RecalculatePanelRectForAvoidArea(WSRect& panelRect) {}
    void RegisterGetStatusBarAvoidHeightFunc(GetStatusBarAvoidHeightFunc&& func);

    void SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void SetWindowDragHotAreaListener(const NotifyWindowDragHotAreaFunc& func);
    void SetSessionEventParam(SessionEventParam param);
    void SetSessionRectChangeCallback(const NotifySessionRectChangeFunc& func);
    void SetAdjustKeyboardLayoutCallback(const NotifyKeyboardLayoutAdjustFunc& func);
    void SetSkipDraw(bool skip);
    virtual void SetSkipSelfWhenShowOnVirtualScreen(bool isSkip);
    WMError SetUniqueDensityDpi(bool useUnique, float dpi);

    bool IsAnco() const override;
    void SetBlank(bool isAddBlank) override;
    bool GetBlank() const override;
    void SetBufferAvailableCallbackEnable(bool enable);
    bool GetBufferAvailableCallbackEnable() const override;
    int32_t GetCollaboratorType() const;
    WSRect GetLastSafeRect() const;
    WSRect GetSessionTargetRectByDisplayId(DisplayId displayId) const;
    std::string GetUpdatedIconPath() const;
    int32_t GetParentPersistentId() const;
    int32_t GetMainSessionId();
    virtual int32_t GetMissionId() const { return persistentId_; };
    Orientation GetRequestedOrientation() const;
    std::vector<sptr<SceneSession>> GetSubSession() const;
    std::vector<sptr<SceneSession>> GetToastSession() const;
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo() const;
    std::string GetWindowNameAllType() const;
    SubWindowModalType GetSubWindowModalType() const;
    int32_t GetOriPosYBeforeRaisedByKeyboard() const;
    std::string GetClientIdentityToken() const;

    /*
     * Window Watermark
     */
    void SetWatermarkEnabled(const std::string& watermarkName, bool isEnabled);

    bool IsDecorEnable() const;
    bool IsAppSession() const;
    bool IsAppOrLowerSystemSession() const;
    bool IsSystemSessionAboveApp() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    bool IsShowWhenLocked() const;
    bool GetShowWhenLockedFlagValue() const;
    bool IsFloatingWindowAppType() const;
    bool IsNeedDefaultAnimation() const;
    void SetSystemTouchable(bool touchable) override;
    bool IsVisibleForAccessibility() const;
    void SetStartingWindowExitAnimationFlag(bool enable);
    bool NeedStartingWindowExitAnimation() const override;

    void NotifyWindowVisibility();
    WSError OnShowWhenLocked(bool showWhenLocked);
    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& icon);
    void NotifyTouchOutside();
    bool CheckTouchOutsideCallbackRegistered();
    void UpdateNativeVisibility(bool visible);
    void DumpSessionElementInfo(const std::vector<std::string>& params);
    void NotifyForceHideChange(bool hide);
    WSError BindDialogSessionTarget(const sptr<SceneSession>& sceneSession);
    void DumpSessionInfo(std::vector<std::string>& info) const;
    bool AddSubSession(const sptr<SceneSession>& subSession);
    bool RemoveSubSession(int32_t persistentId);
    bool AddToastSession(const sptr<SceneSession>& toastSession);
    bool RemoveToastSession(int32_t persistentId);
    void NotifySessionForeground(uint32_t reason, bool withAnimation);
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits);
    void RegisterForceSplitListener(const NotifyForceSplitFunc& func);

    /*
     * Dialog Window
     */
    void RegisterBindDialogSessionCallback(const NotifyBindDialogSessionFunc& callback);

    void SetUpdatePrivateStateAndNotifyFunc(const UpdatePrivateStateAndNotifyFunc& func);

    /*
     * Window Input Event
     */
    void RegisterTouchOutsideCallback(NotifyTouchOutsideFunc&& callback);

    /*
     * Window Rotation
     */
    void RegisterRequestedOrientationChangeCallback(NotifyReqOrientationChangeFunc&& callback);

    /*
     * Window Animation
     */
    void RegisterIsCustomAnimationPlayingCallback(NotifyIsCustomAnimationPlayingCallback&& callback);
    void RegisterDefaultAnimationFlagChangeCallback(NotifyWindowAnimationFlagChangeFunc&& callback);

    /*
     * Window Visibility
     */
    void SetNotifyVisibleChangeFunc(const NotifyVisibleChangeFunc& func);

    /*
     * Window Hierarchy
     */
    void RegisterRaiseToTopCallback(NotifyRaiseToTopFunc&& callback);
    void RegisterRaiseAboveTargetCallback(NotifyRaiseAboveTargetFunc&& callback);
    void RegisterSessionTopmostChangeCallback(NotifySessionTopmostChangeFunc&& callback);

    /*
     * Window Lifecycle
     */
    void ClearJsSceneSessionCbMap(bool needRemove); // ONLY Accessed on OS_sceneSession thread
    void ClearSpecificSessionCbMap();
    void RegisterShowWhenLockedCallback(NotifyShowWhenLockedFunc&& callback);
    void RegisterForceHideChangeCallback(NotifyForceHideChangeFunc&& callback);
    void RegisterClearCallbackMapCallback(ClearCallbackMapFunc&& callback);
    virtual WSError HideSync() { return WSError::WS_DO_NOTHING; }
    void RegisterUpdateAppUseControlCallback(UpdateAppUseControlFunc&& func);
    void NotifyUpdateAppUseControl(ControlAppType type, bool isNeedControl);
    void SetVisibilityChangedDetectFunc(VisibilityChangedDetectFunc&& func);
    virtual void RegisterSessionLockStateChangeCallback(NotifySessionLockStateChangeCallback&& callback) {}
    virtual void NotifySessionLockStateChange(bool isLockedState) {}
    virtual void SetUpdateSessionLabelAndIconListener(NofitySessionLabelAndIconUpdatedFunc&& func) {}

    void SendPointerEventToUI(std::shared_ptr<MMI::PointerEvent> pointerEvent);
    bool SendKeyEventToUI(std::shared_ptr<MMI::KeyEvent> keyEvent, bool isPreImeEvent = false);
    bool IsStartMoving() override;
    void SetIsStartMoving(bool startMoving);
    bool IsSystemSpecificSession() const;
    void SetIsSystemSpecificSession(bool isSystemSpecificSession);
    void SetShouldHideNonSecureWindows(bool shouldHide);
    void UpdateExtWindowFlags(int32_t extPersistentId, const ExtensionWindowFlags& extWindowFlags,
        const ExtensionWindowFlags& extWindowActions);
    ExtensionWindowFlags GetCombinedExtWindowFlags();
    void RemoveExtWindowFlags(int32_t extPersistentId);
    void ClearExtWindowFlags();
    void NotifyDisplayMove(DisplayId from, DisplayId to);
    void NotifySessionFullScreen(bool fullScreen);
    void SetDefaultDisplayIdIfNeed();

    void SetSessionState(SessionState state) override;
    void SetForceHideState(ForceHideState forceHideState);
    ForceHideState GetForceHideState() const;
    bool IsTemporarilyShowWhenLocked() const;
    void SetTemporarilyShowWhenLocked(bool isTemporarilyShowWhenLocked);

    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;

    static MaximizeMode maximizeMode_;
    static uint32_t GetWindowDragHotAreaType(DisplayId displayId, uint32_t type, int32_t pointerX, int32_t pointerY);
    static void AddOrUpdateWindowDragHotArea(DisplayId displayId, uint32_t type, const WSRect& area);
    WSError UpdateRectChangeListenerRegistered(bool isRegister) override;

    /*
     * Window Decor
     */
    int32_t GetCustomDecorHeight() const;
    void SetCustomDecorHeight(int32_t height) override;

    WMError UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action) override;
    void SetSessionChangeByActionNotifyManagerListener(const SessionChangeByActionNotifyManagerFunc& func);

    /*
     * UIExtension
     */
    bool IsShowOnLockScreen(uint32_t lockScreenZOrder);
    void AddExtensionTokenInfo(const UIExtensionTokenInfo& tokenInfo);
    void RemoveExtensionTokenInfo(const sptr<IRemoteObject>& abilityToken);
    void CheckExtensionOnLockScreenToClose();
    void CloseExtensionSync(const UIExtensionTokenInfo& tokenInfo);
    void OnNotifyAboveLockScreen();
    void AddModalUIExtension(const ExtensionWindowEventInfo& extensionInfo);
    void RemoveModalUIExtension(int32_t persistentId);
    void UpdateModalUIExtension(const ExtensionWindowEventInfo& extensionInfo);
    std::optional<ExtensionWindowEventInfo> GetLastModalUIExtensionEventInfo();
    Vector2f GetSessionGlobalPosition(bool useUIExtension);
    void AddUIExtSurfaceNodeId(uint64_t surfaceNodeId, int32_t persistentId);
    void RemoveUIExtSurfaceNodeId(int32_t persistentId);
    int32_t GetUIExtPersistentIdBySurfaceNodeId(uint64_t surfaceNodeId) const;
    int32_t GetStatusBarHeight() override;
    int32_t GetDockHeight();
    bool IsFreeMultiWindowMode() const
    {
        return systemConfig_.IsFreeMultiWindowMode();
    }
    WMError GetAppForceLandscapeConfig(AppForceLandscapeConfig& config) override;

    // WMSPipeline-related: only accessed on SSM thread
    virtual void SyncScenePanelGlobalPosition(bool needSync) {}
    void SetNeedSyncSessionRect(bool needSync);
    // update visible session, return dirty flags
    uint32_t UpdateUIParam(const SessionUIParam& uiParam) REQUIRES(SCENE_GUARD);
    // update invisible session, return dirty flags
    uint32_t UpdateUIParam() REQUIRES(SCENE_GUARD);
    void SetPostProcessFocusState(PostProcessFocusState state);
    PostProcessFocusState GetPostProcessFocusState() const;
    void ResetPostProcessFocusState();
    void SetPostProcessProperty(bool state);
    bool GetPostProcessProperty() const;
    void PostProcessNotifyAvoidArea();
    bool IsImmersiveType() const;
    bool IsPcOrPadEnableActivation() const;

    /*
     * Multi User
     */
    void SetMinimizedFlagByUserSwitch(bool isMinimized);
    bool IsMinimizedByUserSwitch() const;
    void UnregisterSessionChangeListeners() override;

    /*
     * Window ZOrder: PC
     */
    void SetPcScenePanel(bool isPcScenePanel) { isPcScenePanel_ = isPcScenePanel; }
    void UpdatePCZOrderAndMarkDirty(const uint32_t zOrder);

    void SetPrivacyModeChangeNotifyFunc(const NotifyPrivacyModeChangeFunc& func);

    /*
     * Multi Window
     */
    WSError SetSplitButtonVisible(bool isVisible);
    WSError SendContainerModalEvent(const std::string& eventName, const std::string& eventValue);
    void RegisterSetLandscapeMultiWindowFunc(NotifyLandscapeMultiWindowSessionFunc&& func);

    /*
     * Move Drag
     */
    void SetAppDragResizeType(DragResizeType dragResizeType) { appDragResizeType_ = dragResizeType; }
    DragResizeType GetAppDragResizeType() const { return appDragResizeType_; }
    void RegisterSessionEventCallback(NotifySessionEventFunc&& callback);
    void SetWindowMovingCallback(NotifyWindowMovingFunc&& func);
    WSError SetMoveAvailableArea(DisplayId displayId);

    /*
     * Window Layout
     */
    bool IsDirtyWindow();
    bool IsDirtyDragWindow();
    void ResetDirtyDragFlags();
    void ResetSizeChangeReasonIfDirty();
    void SetRequestNextVsyncFunc(RequestVsyncFunc&& func);
    void OnNextVsyncReceivedWhenDrag();
    void RegisterLayoutFullScreenChangeCallback(NotifyLayoutFullScreenChangeFunc&& callback);
    bool SetFrameGravity(Gravity gravity);
    void SetBehindWindowFilterEnabled(bool enabled); // Only accessed on main thread

    /*
     * Gesture Back
     */
    bool GetGestureBackEnabled();
    bool GetEnableGestureBackHadSet();

    /*
     * PC Fold Screen
     */
    void UpdateFullScreenWaterfallMode(bool isWaterfallMode);
    void RegisterFullScreenWaterfallModeChangeCallback(std::function<void(bool isWaterfallMode)>&& func);
    void OnThrowSlipAnimationStateChange(bool isAnimating);
    void RegisterThrowSlipAnimationStateChangeCallback(std::function<void(bool isAnimating)>&& func);
    bool IsMissionHighlighted();
    void MaskSupportEnterWaterfallMode();
    void SetSupportEnterWaterfallMode(bool isSupportEnter);

    /*
     * Keyboard
     */
    void SetIsSystemKeyboard(bool isSystemKeyboard);
    bool IsSystemKeyboard() const;
    void ActivateKeyboardAvoidArea(bool active, bool recalculateAvoid);
    bool IsKeyboardAvoidAreaActive() const;
    virtual void SetKeyboardViewModeChangeListener(const NotifyKeyboarViewModeChangeFunc& func) {};

protected:
    void NotifyIsCustomAnimationPlaying(bool isPlaying);
    void SetMoveDragCallback();
    std::string GetRatioPreferenceKey();
    WSError NotifyClientToUpdateRectTask(const std::string& updateReason, std::shared_ptr<RSTransaction> rsTransaction);
    bool CheckPermissionWithPropertyAnimation(const sptr<WindowSessionProperty>& property) const;

    std::string GetRectInfo(const WSRect& rect)
    {
        using std::to_string;
        return "[" + to_string(rect.width_) + ", " + to_string(rect.height_) + "; "
        + to_string(rect.posX_) + ", " + to_string(rect.posY_) + "]";
    }

    /*
     * Window Pipeline
     */
    bool UpdateVisibilityInner(bool visibility) REQUIRES(SCENE_GUARD);
    bool UpdateInteractiveInner(bool interactive);
    virtual void NotifyClientToUpdateInteractive(bool interactive) {}
    bool PipelineNeedNotifyClientToUpdateRect() const;
    bool UpdateRectInner(const SessionUIParam& uiParam, SizeChangeReason reason);
    bool NotifyServerToUpdateRect(const SessionUIParam& uiParam, SizeChangeReason reason);
    bool IsTransformNeedChange(float scaleX, float scaleY, float pivotX, float pivotY);
    bool UpdateScaleInner(float scaleX, float scaleY, float pivotX, float pivotY);
    bool UpdateZOrderInner(uint32_t zOrder);

    /*
     * Window Immersive
     */
    void CalculateAvoidAreaRect(const WSRect& rect, const WSRect& avoidRect, AvoidArea& avoidArea) const;
    virtual void NotifyClientToUpdateAvoidArea();
    bool PipelineNeedNotifyClientToUpdateAvoidArea(uint32_t dirty) const;
    NotifyNeedAvoidFunc onNeedAvoid_;
    NotifySystemBarPropertyChangeFunc onSystemBarPropertyChange_;
    GetStatusBarAvoidHeightFunc onGetStatusBarAvoidHeightFunc_;

    /*
     * Gesture Back
     */
    void UpdateGestureBackEnabled() override;

    sptr<SpecificSessionCallback> specificCallback_ = nullptr;

    /*
     * Dialog window
     */
    NotifyBindDialogSessionFunc onBindDialogTarget_;

    sptr<SceneSession> keyboardPanelSession_ = nullptr;
    sptr<SceneSession> keyboardSession_ = nullptr;
    NotifyKeyboardLayoutAdjustFunc adjustKeyboardLayoutFunc_;

    /*
     * Window Hierarchy
     */
    NotifyMainWindowTopmostChangeFunc mainWindowTopmostChangeFunc_;
    NotifyRaiseToTopFunc onRaiseToTop_;
    NotifyRaiseAboveTargetFunc onRaiseAboveTarget_;
    NotifySessionTopmostChangeFunc onSessionTopmostChange_;

    /*
     * PC Window
     */
    NotifyTitleAndDockHoverShowChangeFunc onTitleAndDockHoverShowChangeFunc_;
    NotifyRestoreMainWindowFunc onRestoreMainWindowFunc_;
    NotifySetWindowRectAutoSaveFunc onSetWindowRectAutoSaveFunc_;
    NotifySubModalTypeChangeFunc onSubModalTypeChange_;
    NotifyMainModalTypeChangeFunc onMainModalTypeChange_;
    NotifySetSupportedWindowModesFunc onSetSupportedWindowModesFunc_;

    /*
     * PiP Window
     */
    NotifyPrepareClosePiPSessionFunc onPrepareClosePiPSession_;

    /*
     * Window Layout
     */
    NotifyDefaultDensityEnabledFunc onDefaultDensityEnabledFunc_;
    sptr<MoveDragController> moveDragController_ = nullptr;
    virtual void NotifySessionRectChange(const WSRect& rect,
        SizeChangeReason reason = SizeChangeReason::UNDEFINED, DisplayId displayId = DISPLAY_ID_INVALID,
        const RectAnimationConfig& rectAnimationConfig = {});
    virtual void UpdateSessionRectInner(const WSRect& rect, SizeChangeReason reason,
        const MoveConfiguration& moveConfiguration, const RectAnimationConfig& rectAnimationConfig = {});
    void NotifySessionDisplayIdChange(uint64_t displayId);
    virtual void CheckAndMoveDisplayIdRecursively(uint64_t displayId);
    void SetShouldFollowParentWhenShow(bool shouldFollow) { shouldFollowParentWhenShow_ = shouldFollow; }
    bool GetShouldFollowParentWhenShow() const { return shouldFollowParentWhenShow_; }
    void CheckSubSessionShouldFollowParent(uint64_t displayId);

    /*
     * Window Lifecycle
     */
    NotifyShowWhenLockedFunc onShowWhenLockedFunc_;
    NotifyForceHideChangeFunc onForceHideChangeFunc_;
    ClearCallbackMapFunc clearCallbackMapFunc_;
    UpdateAppUseControlFunc onUpdateAppUseControlFunc_;
    std::unordered_map<ControlAppType, bool> appUseControlMap_;

    /*
     * PC Fold Screen
     */
    bool IsFullScreenWaterfallMode();
    void UpdateWaterfallMode(SessionEvent event);
    sptr<PcFoldScreenController> pcFoldScreenController_ = nullptr;
    std::atomic_bool isThrowSlipToFullScreen_ = false;
    std::function<void(bool isAnimating)> onThrowSlipAnimationStateChangeFunc_;

    /*
     * Multi Window
     */
    NotifyLandscapeMultiWindowSessionFunc onSetLandscapeMultiWindowFunc_;

    /*
     * Keyboard
     */
    virtual void EnableCallingSessionAvoidArea() {}
    virtual void RestoreCallingSession(const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) {}
    bool keyboardAvoidAreaActive_ = true;

private:
    void NotifyAccessibilityVisibilityChange();
    void CalculateCombinedExtWindowFlags();
    void HandleStyleEvent(MMI::WindowArea area) override;
    WSError HandleEnterWinwdowArea(int32_t windowX, int32_t windowY);

    /*
     * Window Immersive
     */
    void GetSystemAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetAINavigationBarArea(WSRect rect, AvoidArea& avoidArea) const;
    AvoidArea GetAvoidAreaByTypeInner(AvoidAreaType type, const WSRect& rect = WSRect::EMPTY_RECT);

    /*
     * Window Lifecycle
     */
    bool CheckIdentityTokenIfMatched(const std::string& identityToken);
    bool CheckPidIfMatched();

    // session lifecycle funcs
    WSError ForegroundTask(const sptr<WindowSessionProperty>& property);

    /*
     * Move Drag
     */
    void HandleMoveDragSurfaceNode(SizeChangeReason reason);
    void OnMoveDragCallback(SizeChangeReason reason);
    bool IsDragResizeWhenEnd(SizeChangeReason reason);
    void InitializeCrossMoveDrag();
    WSError InitializeMoveInputBar();
    void HandleMoveDragSurfaceBounds(WSRect& rect, WSRect& globalRect, SizeChangeReason reason);
    void HandleMoveDragEnd(WSRect& rect, SizeChangeReason reason);
    bool MoveUnderInteriaAndNotifyRectChange(WSRect& rect, SizeChangeReason reason);
    void NotifyFullScreenAfterThrowSlip(const WSRect& rect);
    void SetDragResizeTypeDuringDrag(DragResizeType dragResizeType) { dragResizeTypeDuringDrag_ = dragResizeType; }
    DragResizeType GetDragResizeTypeDuringDrag() const { return dragResizeTypeDuringDrag_; }
    void HandleSessionDragEvent(SessionEvent event);
    void HandleCompatibleModeMoveDrag(WSRect& rect, SizeChangeReason reason);
    void HandleCompatibleModeDrag(WSRect& rect, SizeChangeReason reason, bool isSupportDragInPcCompatibleMode);
    NotifySessionEventFunc onSessionEvent_;
    void ProcessWindowMoving(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    /*
     * Gesture Back
     */
    WMError SetGestureBackEnabled(bool isEnabled) override;

    /*
     * UIExtension
     */
    void UpdateAllModalUIExtensions(const WSRect& globalRect);

#ifdef DEVICE_STATUS_ENABLE
    void RotateDragWindow(std::shared_ptr<RSTransaction> rsTransaction);
#endif // DEVICE_STATUS_ENABLE
    void NotifyPropertyWhenConnect();
    WSError RaiseAppMainWindowToTop() override;
    void UpdateWinRectForSystemBar(WSRect& rect);
    bool UpdateInputMethodSessionRect(const WSRect& rect, WSRect& newWinRect, WSRect& newRequestRect);
    bool IsMovableWindowType();
    bool IsFullScreenMovable();
    bool IsMovable();
    void HandleCastScreenConnection(SessionInfo& info, sptr<SceneSession> session);
    WMError HandleUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTurnScreenOn(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateKeepScreenOn(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateFocusable(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTouchable(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateSetBrightness(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateOrientation(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdatePrivacyMode(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateSnapshotSkip(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateMaximizeState(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateOtherProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateStatusProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateNavigationProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateNavigationIndicatorProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateFlags(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateMode(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateAnimationFlag(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTouchHotArea(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateKeyboardTouchHotArea(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateDecorEnable(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowLimits(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateDragenabled(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateRaiseenabled(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTextfieldAvoidInfo(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowMask(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTopmost(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateMainWindowTopmost(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowModeSupportType(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError ProcessUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateAvoidAreaOption(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleBackgroundAlpha(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    void HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property);
    void SetWindowFlags(const sptr<WindowSessionProperty>& property);
    void NotifySessionChangeByActionNotifyManager(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    void NotifyPrivacyModeChange();

    /*
     * PiP Window
     */
    NotifySessionPiPControlStatusChangeFunc sessionPiPControlStatusChangeFunc_;
    NotifyAutoStartPiPStatusChangeFunc autoStartPiPStatusChangeFunc_;
    PiPTemplateInfo pipTemplateInfo_ = {0, 0, {}};

    NotifyForceSplitFunc forceSplitFunc_;
    UpdatePrivateStateAndNotifyFunc updatePrivateStateAndNotifyFunc_;
    NotifyPrivacyModeChangeFunc privacyModeChangeNotifyFunc_;
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    WSRect lastSafeRect = { 0, 0, 0, 0 };
    std::vector<sptr<SceneSession>> subSession_;
    std::vector<sptr<SceneSession>> toastSession_;
    std::atomic_bool needStartingWindowExitAnimation_ { true };
    bool needDefaultAnimationFlag_ = true;
    SessionEventParam sessionEventParam_ = { 0, 0, 0, 0, 0 };
    std::atomic_bool isStartMoving_ { false };
    std::atomic_bool isVisibleForAccessibility_ { true };
    bool isSystemSpecificSession_ { false };

    /*
     * UIExtension
     */
    std::atomic_bool shouldHideNonSecureWindows_ { false };
    std::shared_mutex combinedExtWindowFlagsMutex_;
    ExtensionWindowFlags combinedExtWindowFlags_ { 0 };
    std::map<int32_t, ExtensionWindowFlags> extWindowFlagsMap_;
    std::vector<UIExtensionTokenInfo> extensionTokenInfos_;

    /*
     * Window Layout
     */
    void AdjustRectByLimits(WindowLimits limits, float ratio, bool isDecor, float vpr, WSRect& rect);
    bool AdjustRectByAspectRatio(WSRect& rect);
    bool SaveAspectRatio(float ratio);
    WSError UpdateRectForDrag(const WSRect& rect);
    void UpdateSessionRectPosYFromClient(WSRect& rect);

    /*
     * Window Decor
     */
    mutable std::mutex customDecorHeightMutex_;
    int32_t customDecorHeight_ = 0;

    ForceHideState forceHideState_ { ForceHideState::NOT_HIDDEN };
    int32_t oriPosYBeforeRaisedByKeyboard_ = 0;
    std::atomic_bool isTemporarilyShowWhenLocked_ { false };
    std::shared_mutex modalUIExtensionInfoListMutex_;
    std::vector<ExtensionWindowEventInfo> modalUIExtensionInfoList_;
    mutable std::shared_mutex uiExtNodeIdToPersistentIdMapMutex_;
    std::map<uint64_t, int32_t> uiExtNodeIdToPersistentIdMap_;
    std::string clientIdentityToken_ = { "" };
    SessionChangeByActionNotifyManagerFunc sessionChangeByActionNotifyManagerFunc_;

    bool isAddBlank_ = false;
    bool bufferAvailableCallbackEnable_ = false;
    bool isScreenAngleMismatch_ = false;
    uint32_t targetScreenWidth_ = 0;
    uint32_t targetScreenHeight_ = 0;

    // WMSPipeline-related: only accessed on SSM thread
    PostProcessFocusState postProcessFocusState_;
    bool postProcessProperty_ { false };

    /*
     * Window Recover
     */
    bool isRecovered_ = false;

    /*
     * Multi User
     */
    bool isMinimizedByUserSwitch_ { false };

    /*
     * Window ZOrder: PC
     */
    bool isPcScenePanel_ { false };

    /*
     * Move Drag
     */
    static std::shared_mutex windowDragHotAreaMutex_;
    static std::map<uint64_t, std::map<uint32_t, WSRect>> windowDragHotAreaMap_;
    DragResizeType appDragResizeType_ = DragResizeType::RESIZE_TYPE_UNDEFINED;
    DragResizeType dragResizeTypeDuringDrag_ = DragResizeType::RESIZE_TYPE_UNDEFINED;
    NotifyWindowMovingFunc notifyWindowMovingFunc_;

    // Set true if either sessionProperty privacyMode or combinedExtWindowFlags_ privacyModeFlag is true.
    bool isPrivacyMode_ { false };

    /*
     * Gesture Back
     */
    bool isEnableGestureBack_ { true };
    bool isEnableGestureBackHadSet_ { false };

    /*
     * Window Visibility
     */
    NotifyVisibleChangeFunc notifyVisibleChangeFunc_;

    /*
     * Window Input Event
     */
    NotifyTouchOutsideFunc onTouchOutside_;

    /*
     * Window Rotation
     */
    NotifyReqOrientationChangeFunc onRequestedOrientationChange_;

    /*
     * Window Animation
     */
    NotifyIsCustomAnimationPlayingCallback onIsCustomAnimationPlaying_;
    NotifyWindowAnimationFlagChangeFunc onWindowAnimationFlagChange_;

    /*
     * Window Layout
     */
    void SetSurfaceBoundsWithAnimation(
        const std::pair<RSAnimationTimingProtocol, RSAnimationTimingCurve>& animationParam,
        const WSRect& rect, const std::function<void()>& finishCallback = nullptr, bool isGlobal = false);
    void SetSurfaceBounds(const WSRect& rect, bool isGlobal, bool needFlush = true);
    NotifyLayoutFullScreenChangeFunc onLayoutFullScreenChangeFunc_;
    std::atomic<bool> shouldFollowParentWhenShow_ = true;
    std::shared_ptr<RSBehindWindowFilterEnabledModifier>
        behindWindowFilterEnabledModifier_; // Only accessed on main thread
    bool isDragEnd_ = false;

    /*
     * Window Immersive
     */
    std::atomic_bool isDisplayStatusBarTemporarily_ { false };
    bool isStatusBarVisible_ = true;
    IsLastFrameLayoutFinishedFunc isLastFrameLayoutFinishedFunc_;

    /*
     * PC Window Layout
     */
    bool isLayoutFullScreen_ { false };
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
