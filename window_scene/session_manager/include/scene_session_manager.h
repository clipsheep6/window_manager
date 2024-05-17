/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H

#include "scene_session_manager_incl.h"

namespace OHOS::Rosen {
using namespace std;
class SceneSessionManager : public SceneSessionManagerStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneSessionManager)

public:
    friend class AnomalyDetection;
    bool IsSessionVisible(const sptr<SceneSession>& session);
    sptr<SceneSession> RequestSceneSession(const SessionInfo& sessionInfo,
        sptr<WindowSessionProperty> property = nullptr);
    void UpdateSceneSessionWant(const SessionInfo& sessionInfo);
    future<int32_t> RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession, bool isNewActive);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession, const bool isDelegator = false,
        const bool isToDesktop = false, const bool isSaveSnapshot = true);
    WSError RequestSceneSessionDestruction(
        const sptr<SceneSession>& sceneSession, const bool needRemoveSession = true);
    WSError RequestSceneSessionDestructionInner(sptr<SceneSession> &scnSession, sptr<AAFwk::SessionInfo> scnSessionInfo,
        const bool needRemoveSession);
    void NotifyForegroundInteractiveStatus(const sptr<SceneSession>& sceneSession, bool interactive);
    WSError RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession);
    void StartAbilityBySpecified(const SessionInfo& sessionInfo);
    void SetRootSceneContext(const weak_ptr<AbilityRuntime::Context>& contextWeak);
    sptr<RootSceneSession> GetRootSceneSession();
    sptr<SceneSession> GetSceneSession(int32_t persistentId);
    sptr<SceneSession> GetSceneSessionByName(const string& bundleName,
        const string& moduleName, const string& abilityName, const int32_t appIndex);
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        SystemSessionConfig& systemConfig, sptr<IRemoteObject> token = nullptr) override;
    WSError RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token = nullptr) override;
    WSError RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<ISession>& session, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr) override;
    WSError DestroyAndDisconnectSpecificSession(const int32_t persistentId) override;
    WSError DestroyAndDisconnectSpecificSessionWithDetachCallback(const int32_t persistentId,
        const sptr<IRemoteObject>& callback) override;
    WMError UpdateSessionProperty(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action) override;
    void SetCreateSystemSessionListener(const NotifyCreateSystemSessionFunc& func);
    void SetCreateKeyboardSessionListener(const NotifyCreateKeyboardSessionFunc& func);
    void SetStatusBarEnabledChangeListener(const ProcessStatusBarEnabledChangeFunc& func);
    void SetStartUIAbilityErrorListener(const ProcessStartUIAbilityErrorFunc& func);
    void SetRecoverSceneSessionListener(const NotifyRecoverSceneSessionFunc& func);
    void SetGestureNavigationEnabledChangeListener(const ProcessGestureNavigationEnabledChangeFunc& func);
    void SetDumpRootSceneElementInfoListener(const DumpRootSceneElementInfoFunc& func);
    void SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func);
    void SetShiftFocusListener(const ProcessShiftFocusFunc& func);
    void SetSCBFocusedListener(const NotifySCBAfterUpdateFocusFunc& func);
    void SetSCBUnfocusedListener(const NotifySCBAfterUpdateFocusFunc& func);
    void SetCallingSessionIdSessionListenser(const ProcessCallingSessionIdChangeFunc& func);
    const AppWindowSceneConfig& GetWindowSceneConfig() const;
    WSError ProcessBackEvent();
    WSError BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken) override;
    void GetStartupPage(const SessionInfo& sessionInfo, string& path, uint32_t& bgColor);
    WMError SetGestureNavigaionEnabled(bool enable) override;
    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    WSError SetFocusedSessionId(int32_t persistentId);
    int32_t GetFocusedSessionId() const;
    FocusChangeReason GetFocusChangeReason() const;
    WSError GetAllSessionDumpInfo(string& info);
    WSError GetSpecifiedSessionDumpInfo(string& dumpInfo, const vector<string>& params, const string& strId);
    WSError GetSessionDumpInfo(const vector<string>& params, string& info) override;
    WMError RequestFocusStatus(int32_t persistentId, bool isFocused, bool byForeground = true,
        FocusChangeReason reason = FocusChangeReason::DEFAULT) override;
    void RequestAllAppSessionUnfocus();
    WSError UpdateFocus(int32_t persistentId, bool isFocused);
    WSError UpdateWindowMode(int32_t persistentId, int32_t windowMode);
    WSError SendTouchEvent(const shared_ptr<MMI::PointerEvent>& pointerEvent, uint32_t zIndex);
    void SetScreenLocked(const bool isScreenLocked);
    bool IsScreenLocked() const;
    WSError RaiseWindowToTop(int32_t persistentId) override;
    WSError InitUserInfo(int32_t userId, string &fileDir);
    void NotifySwitchingUser(const bool isUserActive);
    int32_t GetCurrentUserId() const;
    void StartWindowInfoReportLoop();
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo) override;
    void NotifyCompleteFirstFrameDrawing(int32_t persistentId);
    void NotifySessionMovedToFront(int32_t persistentId);
    WSError SetSessionLabel(const sptr<IRemoteObject> &token, const string &label) override;
    WSError SetSessionIcon(const sptr<IRemoteObject> &token, const shared_ptr<Media::PixelMap> &icon) override;
    WSError IsValidSessionIds(const vector<int32_t> &sessionIds, vector<bool> &results) override;
    WSError RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener) override;
    void UnregisterSessionListener() override;
    void HandleTurnScreenOn(const sptr<SceneSession>& sceneSession);
    void HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock);
    void InitWithRenderServiceAdded();
    WSError PendingSessionToForeground(const sptr<IRemoteObject> &token) override;
    WSError PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token) override;
    WSError GetFocusSessionToken(sptr<IRemoteObject> &token) override;
    WSError GetFocusSessionElement(AppExecFwk::ElementName& element) override;
    WSError RegisterSessionListener(const sptr<ISessionListener>& listener) override;
    WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener) override;
    WSError GetSessionInfos(const string& deviceId, int32_t numMax, vector<SessionInfoBean>& sessionInfos) override;
    WSError GetSessionInfo(const string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo) override;
    WSError GetSessionInfoByContinueSessionId(const string& continueSessionId, SessionInfoBean& sessionInfo) override;
    WSError DumpSessionAll(vector<string> &infos) override;
    WSError DumpSessionWithId(int32_t persistentId, vector<string> &infos) override;
    WSError GetAllAbilityInfos(const AAFwk::Want &want, int32_t userId, vector<SCBAbilityInfo> &scbAbilityInfos);
    WSError PrepareTerminate(int32_t persistentId, bool& isPrepareTerminate);
    WSError GetIsLayoutFullScreen(bool& isLayoutFullScreen);
    WSError TerminateSessionNew(
        const sptr<AAFwk::SessionInfo> info, bool needStartCaller, bool isFromBroker = false) override;
    WSError UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener) override;
    WSError UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener) override;
    WSError GetSessionSnapshot(const string& deviceId, int32_t persistentId,
                               SessionSnapshot& snapshot, bool isLowResolution) override;
    WMError GetSessionSnapshotById(int32_t persistentId, SessionSnapshot& snapshot) override;
    WSError SetSessionContinueState(const sptr<IRemoteObject> &token, const ContinueState& continueState) override;
    WSError ClearSession(int32_t persistentId) override;
    WSError ClearAllSessions() override;
    WSError LockSession(int32_t sessionId) override;
    WSError UnlockSession(int32_t sessionId) override;
    WSError MoveSessionsToForeground(const vector<int32_t>& sessionIds, int32_t topSessionId) override;
    WSError MoveSessionsToBackground(const vector<int32_t>& sessionIds, vector<int32_t>& result) override;
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) override;
    map<int32_t, sptr<SceneSession>>& GetSessionMapByScreenId(ScreenId id);
    void UpdatePrivateStateAndNotify(uint32_t persistentId);
    void InitPersistentStorage();
    string GetSessionSnapshotFilePath(int32_t persistentId);
    void OnOutsideDownEvent(int32_t x, int32_t y);
    void NotifySessionTouchOutside(int32_t persistentId);
    WMError GetAccessibilityWindowInfo(vector<sptr<AccessibilityWindowInfo>>& infos) override;
    WMError GetUnreliableWindowInfo(int32_t windowId, vector<sptr<UnreliableWindowInfo>>& infos) override;
    WSError SetWindowFlags(const sptr<SceneSession>& sceneSession, const sptr<WindowSessionProperty>& property);
    void OnScreenshot(DisplayId displayId);
    void NotifyDumpInfoResult(const vector<string>& info) override;
    void SetVirtualPixelRatioChangeListener(const ProcessVirtualPixelRatioChangeFunc& func);
    void ProcessVirtualPixelRatioChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    void ProcessUpdateRotationChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    RunnableFuture<vector<string>> dumpInfoFuture_;
    void RegisterWindowChanged(const WindowChangedFunc& func);
    WSError RegisterIAbilityManagerCollaborator(int32_t type,
        const sptr<AAFwk::IAbilityManagerCollaborator> &impl) override;
    WSError UnregisterIAbilityManagerCollaborator(int32_t type) override;
    bool IsInputEventEnabled();
    void SetEnableInputEvent(bool enabled);
    void UpdateRecoveredSessionInfo(const vector<int32_t>& recoveredPersistentIds);
    void SetAlivePersistentIds(const vector<int32_t>& alivePersistentIds);
    void NotifyRecoveringFinished();
    WMError CheckWindowId(int32_t windowId, int32_t &pid) override;
    void GetSceneSessionPrivacyModeBundles(DisplayId displayId, unordered_set<string>& privacyBundles);
    BrokerStates CheckIfReuseSession(SessionInfo& sessionInfo);
    bool CheckCollaboratorType(int32_t type);
    sptr<SceneSession> FindSessionByAffinity(string affinity);
    void PreloadInLakeApp(const string& bundleName);
    void AddWindowDragHotArea(uint32_t type, WSRect& area);
    WSError UpdateMaximizeMode(int32_t persistentId, bool isMaximize);
    WSError UpdateSessionDisplayId(int32_t persistentId, uint64_t screenId);
    void NotifySessionUpdate(const SessionInfo& sessionInfo, ActionType type,
        ScreenId fromScreenId = SCREEN_ID_INVALID);
    WSError NotifyAINavigationBarShowStatus(bool isVisible, WSRect barArea, uint64_t displayId);
    WSRect GetAINavigationBarArea(uint64_t displayId);
    bool UpdateImmersiveState();
    WMError GetSurfaceNodeIdsFromMissionIds(vector<uint64_t>& missionIds, vector<uint64_t>& surfaceNodeIds);
    WSError UpdateTitleInTargetPos(int32_t persistentId, bool isShow, int32_t height);
    void RegisterCreateSubSessionListener(int32_t persistentId, const NotifyCreateSubSessionFunc& func);
    void UnregisterCreateSubSessionListener(int32_t persistentId);
    WSError NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible) override;
    void DealwithVisibilityChange(const vector<pair<uint64_t, WindowVisibilityState>>& visibilityChangeInfos);
    void DealwithDrawingContentChange(const vector<pair<uint64_t, bool>>& drawingChangeInfos);
    void NotifyUpdateRectAfterLayout();
    WSError UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener) override;
    WMError SetSystemAnimatedScenes(SystemAnimatedSceneType sceneType);
    WSError ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId) override;
    shared_ptr<Media::PixelMap> GetSessionSnapshotPixelMap(const int32_t persistentId, const float scaleParam);
    void RequestInputMethodCloseKeyboard(int32_t persistentId);
    const map<int32_t, sptr<SceneSession>> GetSceneSessionMap();
    void GetAllSceneSession(vector<sptr<SceneSession>>& sceneSessions);
    WMError GetVisibilityWindowInfo(vector<sptr<WindowVisibilityInfo>>& infos) override;
    void GetAllWindowVisibilityInfos(vector<pair<int32_t, uint32_t>>& windowVisibilityInfos);
    void FlushWindowInfoToMMI(const bool forceFlush = false);
    void PostFlushWindowInfoTask(FlushWindowInfoTask &&task, const string taskName, const int delayTime);
    void AddExtensionWindowStageToSCB(const sptr<ISessionStage>& sessionStage, int32_t persistentId,
        int32_t parentId) override;
    WSError AddOrRemoveSecureSession(int32_t persistentId, bool shouldHide) override;
    WSError UpdateExtWindowFlags(int32_t parentId, int32_t persistentId, uint32_t extWindowFlags,
        uint32_t extWindowActions) override;
    void CheckSceneZOrder();
    int32_t StartUIAbilityBySCB(sptr<AAFwk::SessionInfo>& abilitySessionInfo);
    int32_t StartUIAbilityBySCB(sptr<SceneSession>& sceneSessions);
    int32_t ChangeUIAbilityVisibilityBySCB(sptr<SceneSession>& sceneSessions, bool visibility);
    WSError GetHostWindowRect(int32_t hostWindowId, Rect& rect) override;
    int32_t ReclaimPurgeableCleanMem();
    WMError GetCallingWindowWindowStatus(int32_t persistentId, WindowStatus& windowStatus) override;
    WMError GetCallingWindowRect(int32_t persistentId, Rect& rect) override;
    WMError GetWindowModeType(WindowModeType& windowModeType) override;
    void OnBundleUpdated(const string& bundleName, int userId);
    void OnConfigurationUpdated(const shared_ptr<AppExecFwk::Configuration>& configuration);
    shared_ptr<TaskScheduler> GetTaskScheduler() {return taskScheduler_;};
    WSError SwitchFreeMultiWindow(bool enable);
    const SystemSessionConfig& GetSystemSessionConfig() const;
    void ClearDisplayStatusBarTemporarilyFlags();
    int32_t GetCustomDecorHeight(int32_t persistentId);
    WMError GetMainWindowInfos(int32_t topNum, vector<MainWindowInfo>& topNInfo);
    WSError NotifyEnterRecentTask(bool enterRecent);
    WMError GetAllMainWindowInfos(vector<MainWindowInfo>& infos) const;
    WMError ClearMainSessions(const vector<int32_t>& persistentIds, vector<int32_t>& clearFailedIds);

protected:
    SceneSessionManager();
    virtual ~SceneSessionManager() = default;

private:
    bool isKeyboardPanelEnabled_ = false;
    atomic<bool> enterRecent_ { false };
    void Init();
    void InitScheduleUtils();
    void RegisterAppListener();
    void InitPrepareTerminateConfig();
    void LoadWindowSceneXml();
    void ConfigWindowSceneXml();
    void ConfigWindowSceneXml(const WindowSceneConfig::ConfigItem& config);
    void ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig);
    void ConfigWindowImmersive(const WindowSceneConfig::ConfigItem& immersiveConfig);
    void ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig);
    void ConfigDefaultKeyboardAnimation();
    bool ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out);
    bool ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig, WindowShadowConfig& outShadow);
    bool ConfigStatusBar(const WindowSceneConfig::ConfigItem& config, StatusBarConfig& statusBarConfig);
    void ConfigSystemUIStatusBar(const WindowSceneConfig::ConfigItem& statusBarConfig);
    void ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig, bool mainConfig = true);
    void ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig);
    void ConfigStartingWindowAnimation(const WindowSceneConfig::ConfigItem& startingWindowConfig);
    void ConfigWindowSizeLimits();
    void ConfigMainWindowSizeLimits(const WindowSceneConfig::ConfigItem& mainWindowSizeConifg);
    void ConfigSubWindowSizeLimits(const WindowSceneConfig::ConfigItem& subWindowSizeConifg);
    void ConfigSnapshotScale();
    void ConfigFreeMultiWindow();
    void LoadFreeMultiWindowConfig(bool enable);
    tuple<string, vector<float>> CreateCurve(const WindowSceneConfig::ConfigItem& curveConfig);
    void LoadKeyboardAnimation(const WindowSceneConfig::ConfigItem& item, KeyboardSceneAnimationConfig& config);
    sptr<SceneSession::SpecificSessionCallback> CreateSpecificSessionCallback();
    sptr<KeyboardSession::KeyboardSessionCallback> CreateKeyboardSessionCallback();
    void FillSessionInfo(sptr<SceneSession>& sceneSession);
    shared_ptr<AppExecFwk::AbilityInfo> QueryAbilityInfoFromBMS(const int32_t uId, const string& bundleName,
        const string& abilityName, const string& moduleName);
    vector<pair<int32_t, sptr<SceneSession>>> GetSceneSessionVector(CmpFunc cmp);
    void TraverseSessionTree(TraverseFunc func, bool isFromTopToBottom);
    void TraverseSessionTreeFromTopToBottom(TraverseFunc func);
    void TraverseSessionTreeFromBottomToTop(TraverseFunc func);
    WSError RequestSessionFocus(int32_t persistentId, bool byForeground = true,
        FocusChangeReason reason = FocusChangeReason::DEFAULT);
    WSError RequestSessionFocusImmediately(int32_t persistentId);
    WSError RequestSessionUnfocus(int32_t persistentId, FocusChangeReason reason = FocusChangeReason::DEFAULT);
    WSError RequestAllAppSessionUnfocusInner();
    WSError RequestFocusBasicCheck(int32_t persistentId);
    WSError RequestFocusSpecificCheck(sptr<SceneSession>& sceneSession, bool byForeground,
        FocusChangeReason reason = FocusChangeReason::DEFAULT);
    bool CheckFocusIsDownThroughBlockingType(sptr<SceneSession>& requestSceneSession,
        sptr<SceneSession>& focusedSession, bool includingAppSession);
    sptr<SceneSession> GetNextFocusableSession(int32_t persistentId);
    sptr<SceneSession> GetTopNearestBlockingFocusSession(uint32_t zOrder, bool includingAppSession);
    sptr<SceneSession> GetTopFocusableNonAppSession();
    WSError ShiftFocus(sptr<SceneSession>& nextSession, FocusChangeReason reason = FocusChangeReason::DEFAULT);
    void UpdateFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused);
    void NotifyFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused);
    void NotifyFocusStatusByMission(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession);
    bool MissionChanged(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession);
    string GetAllSessionFocusInfo();
    void RegisterRequestFocusStatusNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void RegisterGetStateFromManagerFunc(sptr<SceneSession>& sceneSession);
    void RegisterSessionChangeByActionNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    sptr<AAFwk::SessionInfo> SetAbilitySessionInfo(const sptr<SceneSession>& scnSession);
    WSError DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession);
    sptr<SceneSession> FindMainWindowWithToken(sptr<IRemoteObject> targetToken);
    WSError UpdateParentSessionForDialog(const sptr<SceneSession>& sceneSession, sptr<WindowSessionProperty> property);
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void UpdateFocusableProperty(int32_t persistentId);
    WMError UpdateTopmostProperty(const sptr<WindowSessionProperty> &property, const sptr<SceneSession> &sceneSession);
    vector<sptr<SceneSession>> GetSceneSessionVectorByType(WindowType type, uint64_t displayId);
    bool UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
        const sptr<SceneSession>& sceneSession, const AvoidArea& avoidArea, AvoidAreaType avoidAreaType);
    void UpdateAvoidSessionAvoidArea(WindowType type, bool& needUpdate);
    void UpdateNormalSessionAvoidArea(const int32_t& persistentId, sptr<SceneSession>& sceneSession, bool& needUpdate);
    void UpdateAvoidArea(const int32_t& persistentId);
    void NotifyMMIWindowPidChange(int32_t windowId, bool startMoving);
    sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    shared_ptr<Global::Resource::ResourceManager> GetResourceManager(const AppExecFwk::AbilityInfo& abilityInfo);
    bool GetStartupPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo, string& path, uint32_t& bgColor);
    bool GetStartingWindowInfoFromCache(const SessionInfo& sessionInfo, string& path, uint32_t& bgColor);
    void CacheStartingWindowInfo(const AppExecFwk::AbilityInfo& abilityInfo, const string& path,
        const uint32_t& bgColor);
    bool CheckIsRemote(const string& deviceId);
    bool GetLocalDeviceId(string& localDeviceId);
    string AnonymizeDeviceId(const string& deviceId);
    int GetRemoteSessionInfos(const string& deviceId, int32_t numMax, vector<SessionInfoBean> &sessionInfos);
    int GetRemoteSessionInfo(const string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo);
    void PerformRegisterInRequestSceneSession(sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionActivationInner(sptr<SceneSession>& scnSession,
        bool isNewActive, const shared_ptr<promise<int32_t>>& promise);
    WSError SetBrightness(const sptr<SceneSession>& sceneSession, float brightness);
    WSError UpdateBrightness(int32_t persistentId);
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    void HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession);
    WMError HandleUpdateProperty(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action,
        const sptr<SceneSession>& sceneSession);
    void HandleHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession);
    void UpdateForceHideState(const sptr<SceneSession>& sceneSession, const sptr<WindowSessionProperty>& property,
        bool add);
    void NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type);
    void NotifyWindowInfoChangeFromSession(int32_t persistentid);
    bool FillWindowInfo(vector<sptr<AccessibilityWindowInfo>>& infos, const sptr<SceneSession>& sceneSession);
    vector<pair<uint64_t, WindowVisibilityState>> GetWindowVisibilityChangeInfo(
        vector<pair<uint64_t, WindowVisibilityState>>& currVisibleData);
    vector<pair<uint64_t, bool>> GetWindowDrawingContentChangeInfo(
        vector<pair<uint64_t, bool>> currDrawingContentData);
    void GetWindowLayerChangeInfo(shared_ptr<RSOcclusionData> occlusionData,
        vector<pair<uint64_t, WindowVisibilityState>>& currVisibleData,
        vector<pair<uint64_t, bool>>& currDrawingContentData);
    void WindowLayerInfoChangeCallback(shared_ptr<RSOcclusionData> occlusiontionData);
    sptr<SceneSession> SelectSesssionFromMap(const uint64_t& surfaceId);
    void WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession);
    void RegisterSessionExceptionFunc(const sptr<SceneSession>& sceneSession);
    void RegisterSessionSnapshotFunc(const sptr<SceneSession>& sceneSession);
    void NotifySessionForCallback(const sptr<SceneSession>& scnSession, const bool needRemoveSession);
    void DumpSessionInfo(const sptr<SceneSession>& session, ostringstream& oss);
    void DumpAllAppSessionInfo(ostringstream& oss, const map<int32_t, sptr<SceneSession>>& sceneSessionMap);
    void DumpSessionElementInfo(const sptr<SceneSession>& session, const vector<string>& params, string& dumpInfo);
    void AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage, const sptr<SceneSession>& sceneSession);
    void DestroySpecificSession(const sptr<IRemoteObject>& remoteObject);
    void DestroyExtensionSession(const sptr<IRemoteObject>& remoteExtSession);
    void EraseSceneSessionMapById(int32_t persistentId);
    WSError GetAbilityInfosFromBundleInfo(vector<AppExecFwk::BundleInfo> &bundleInfos,
        vector<SCBAbilityInfo> &scbAbilityInfos);
    WMError UpdatePropertyDragEnabled(const sptr<WindowSessionProperty>& property,
                                      const sptr<SceneSession>& sceneSession);
    WMError UpdatePropertyRaiseEnabled(const sptr<WindowSessionProperty>& property,
                                       const sptr<SceneSession>& sceneSession);
    void ClosePipWindowIfExist(WindowType type);
    void NotifySessionAINavigationBarChange(int32_t persistentId);
    WSError DestroyAndDisconnectSpecificSessionInner(const int32_t persistentId);
    void UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing);
    void ReportWindowProfileInfos();
    shared_ptr<SkRegion> GetDisplayRegion(DisplayId displayId);
    void GetAllSceneSessionForAccessibility(vector<sptr<SceneSession>>& sceneSessionList);
    void FillAccessibilityInfo(vector<sptr<SceneSession>>& sceneSessionList,
        vector<sptr<AccessibilityWindowInfo>>& accessibilityInfo);
    void FilterSceneSessionCovered(vector<sptr<SceneSession>>& sceneSessionList);
    void NotifyAllAccessibilityInfo();
    void removeFailRecoveredSession();
    sptr<RootSceneSession> rootSceneSession_;
    weak_ptr<AbilityRuntime::Context> rootSceneContextWeak_;
    mutable shared_mutex sceneSessionMapMutex_;
    map<int32_t, sptr<SceneSession>> sceneSessionMap_, systemTopSceneSessionMap_, nonSystemFloatSceneSessionMap_;
    sptr<ScbSessionHandler> scbSessionHandler_;
    shared_ptr<SessionListenerController> listenerController_;
    map<sptr<IRemoteObject>, int32_t> remoteObjectMap_;
    map<sptr<IRemoteObject>,  pair<int32_t, int32_t>> remoteExtSessionMap_;
    set<int32_t> avoidAreaListenerSessionSet_, touchOutsideListenerSessionSet_;
    set<int32_t> windowVisibilityListenerSessionSet_;
    ExtensionWindowFlags combinedExtWindowFlags_ { 0 };
    map<int32_t, ExtensionWindowFlags> extWindowFlagsMap_;
    set<int32_t> failRecoveredPersistentIdSet_;
    map<int32_t, map<AvoidAreaType, AvoidArea>> lastUpdatedAvoidArea_;
    NotifyCreateSystemSessionFunc createSystemSessionFunc_;
    NotifyCreateKeyboardSessionFunc createKeyboardSessionFunc_;
    map<int32_t, NotifyCreateSubSessionFunc> createSubSessionFuncMap_;
    map<int32_t, vector<sptr<SceneSession>>> recoverSubSessionCacheMap_;
    bool recoveringFinished_ = false;
    NotifyRecoverSceneSessionFunc recoverSceneSessionFunc_;
    ProcessStatusBarEnabledChangeFunc statusBarEnabledChangeFunc_;
    ProcessGestureNavigationEnabledChangeFunc gestureNavigationEnabledChangeFunc_;
    ProcessOutsideDownEventFunc outsideDownEventFunc_;
    DumpRootSceneElementInfoFunc dumpRootSceneFunc_;
    ProcessShiftFocusFunc shiftFocusFunc_;
    NotifySCBAfterUpdateFocusFunc notifySCBAfterFocusedFunc_;
    NotifySCBAfterUpdateFocusFunc notifySCBAfterUnfocusedFunc_;
    ProcessCallingSessionIdChangeFunc callingSessionIdChangeFunc_;
    ProcessStartUIAbilityErrorFunc startUIAbilityErrorFunc_;
    ProcessVirtualPixelRatioChangeFunc processVirtualPixelRatioChangeFunc_ = nullptr;
    AppWindowSceneConfig appWindowSceneConfig_;
    SystemSessionConfig systemConfig_;
    FocusChangeReason changeReason_ = FocusChangeReason::DEFAULT;
    float snapshotScale_ = 0.5;
    int32_t focusedSessionId_ = INVALID_SESSION_ID;
    int32_t lastFocusedSessionId_ = INVALID_SESSION_ID;
    int32_t brightnessSessionId_ = INVALID_SESSION_ID;
    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    bool needBlockNotifyFocusStatusUntilForeground_ {false};
    bool needBlockNotifyUnfocusStatus_ {false};
    bool isScreenLocked_ {false};
    bool isPrepareTerminateEnable_ {false};
    bool openDebugTrace {false};
    int32_t currentUserId_;
    atomic<bool> enableInputEvent_ = true;
    bool gestureNavigationEnabled_ {true};
    vector<int32_t> alivePersistentIds_ = {};
    vector<VisibleWindowNumInfo> lastInfo_ = {};
    shared_mutex lastInfoMutex_;
    shared_ptr<TaskScheduler> taskScheduler_;
    sptr<AppExecFwk::IBundleMgr> bundleMgr_;
    sptr<AppAnrListener> appAnrListener_;
    sptr<AppExecFwk::LauncherService> launcherService_;
    shared_mutex startingWindowMapMutex_;
    map<string, map<string, StartingWindowInfo>> startingWindowMap_;
    mutex privacyBundleMapMutex_;
    unordered_map<DisplayId, unordered_set<string>> privacyBundleMap_;
    bool isAINavigationBarVisible_ = false;
    shared_mutex currAINavigationBarAreaMapMutex_;
    map<uint64_t, WSRect> currAINavigationBarAreaMap_;
    WindowModeType lastWindowModeType_ { WindowModeType::WINDOW_MODE_OTHER };
    shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    shared_ptr<AppExecFwk::EventHandler> eventHandler_;
    bool isReportTaskStart_ = false;
    vector<pair<uint64_t, WindowVisibilityState> > lastVisibleData_;
    RSInterfaces& rsInterface_;
    void ClearUnrecoveredSessions(const vector<int32_t>& recoveredPersistentIds);
    SessionInfo RecoverSessionInfo(const sptr<WindowSessionProperty>& property);
    bool isNeedRecover(const int32_t persistentId);
    void RegisterSessionStateChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void RegisterSessionInfoChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void OnSessionStateChange(int32_t persistentId, const SessionState& state);
    void ProcessSubSessionForeground(sptr<SceneSession>& sceneSession);
    void ProcessSubSessionBackground(sptr<SceneSession>& sceneSession);
    WSError ProcessDialogRequestFocusImmdediately(sptr<SceneSession>& sceneSession);
    sptr<ISessionChangeListener> sessionListener_;
    sptr<SceneSession> FindSessionByToken(const sptr<IRemoteObject> &token);
    void CheckAndNotifyWaterMarkChangedResult();
    WSError NotifyWaterMarkFlagChangedResult(bool hasWaterMark);
    void ProcessPreload(const AppExecFwk::AbilityInfo& abilityInfo) const;
    atomic_bool shouldHideNonSecureFloatingWindows_ { false };
    bool lastWaterMarkShowState_ { false };
    WindowChangedFunc WindowChangedFunc_;
    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(
        bind(&SceneSessionManager::DestroySpecificSession, this, placeholders::_1));
    sptr<AgentDeathRecipient> extensionDeath_ = new AgentDeathRecipient(
        bind(&SceneSessionManager::DestroyExtensionSession, this, placeholders::_1));
    WSError ClearSession(sptr<SceneSession> sceneSession);
    bool IsSessionClearable(sptr<SceneSession> scnSession);
    void GetAllClearableSessions(vector<sptr<SceneSession>>& sessionVector);
    int GetRemoteSessionSnapshotInfo(const string& deviceId, int32_t sessionId,
                                     AAFwk::MissionSnapshot& sessionSnapshot);
    sptr<AAFwk::IAbilityManagerCollaborator> GetCollaboratorByType(int32_t collaboratorType);
    shared_mutex collaboratorMapLock_;
    unordered_map<int32_t, sptr<AAFwk::IAbilityManagerCollaborator>> collaboratorMap_;
    atomic<int64_t> containerStartAbilityTime { 0 };
    BrokerStates NotifyStartAbility(
        int32_t collaboratorType, const SessionInfo& sessionInfo, int32_t persistentId = 0);
    void NotifySessionCreate(const sptr<SceneSession> sceneSession, const SessionInfo& sessionInfo);
    void NotifyLoadAbility(int32_t collaboratorType, sptr<AAFwk::SessionInfo> abilitySessionInfo,
        shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void NotifyUpdateSessionInfo(const sptr<SceneSession> sceneSession);
    void NotifyClearSession(int32_t collaboratorType, int32_t persistentId);
    void NotifyMoveSessionToForeground(int32_t collaboratorType, int32_t persistentId);
    bool PreHandleCollaborator(sptr<SceneSession>& sceneSession, int32_t persistentId = 0);
    void NotifyCollaboratorAfterStart(sptr<SceneSession>& scnSession, sptr<AAFwk::SessionInfo>& scnSessionInfo);
    void UpdateCollaboratorSessionWant(sptr<SceneSession>& session, int32_t persistentId = 0);
    bool CheckSystemWindowPermission(const sptr<WindowSessionProperty>& property);
    bool CheckPiPPriority(const PiPTemplateInfo& pipTemplateInfo);
    bool isEnablePiPCreate(const sptr<WindowSessionProperty>& property);
    void DestroySubSession(const sptr<SceneSession>& sceneSession);
    void DestroyToastSession(const sptr<SceneSession>& sceneSession);
    void NotifyStatusBarEnabledChange(bool enable);
    void NotifySessionForeground(const sptr<SceneSession>& session, uint32_t reason, bool withAnimation);
    void NotifySessionBackground(const sptr<SceneSession>& session, uint32_t reason, bool withAnimation,
                                bool isFromInnerkits);
    void NotifyCreateSubSession(int32_t persistentId, sptr<SceneSession> session);
    void NotifyCreateToastSession(int32_t persistentId, sptr<SceneSession> session);
    void CacheSubSessionForRecovering(sptr<SceneSession> sceneSession, const sptr<WindowSessionProperty>& property);
    void RecoverCachedSubSession(int32_t persistentId);
    void NotifySessionUnfocusedToClient(int32_t persistentId);
    void NotifyCreateSpecificSession(sptr<SceneSession> session,
        sptr<WindowSessionProperty> property, const WindowType& type);
    void OnSCBSystemSessionBufferAvailable(const WindowType type);
    sptr<SceneSession> CreateSceneSession(const SessionInfo& sessionInfo, sptr<WindowSessionProperty> property);
    void CreateKeyboardPanelSession(sptr<SceneSession> keyboardSession);
    bool GetPreWindowDrawingState(uint64_t windowId, int32_t& pid, bool currentDrawingContentState);
    bool GetProcessDrawingState(uint64_t windowId, int32_t pid, bool currentDrawingContentState);
    WSError GetAppMainSceneSession(sptr<SceneSession>& sceneSession, int32_t persistentId);
    void CalculateCombinedExtWindowFlags();
    void UpdateSpecialExtWindowFlags(int32_t persistentId, ExtensionWindowFlags flags, ExtensionWindowFlags actions);
    void HideNonSecureFloatingWindows();
    void HideNonSecureSubWindows(const sptr<SceneSession>& sceneSession);
    WSError HandleSecureSessionShouldHide(const sptr<SceneSession>& sceneSession);
    void HandleSpecialExtWindowFlagsChange(int32_t persistentId, ExtensionWindowFlags extWindowFlags,
        ExtensionWindowFlags extWindowActions);
    void HandleCastScreenDisConnection(const sptr<SceneSession> sceneSession);
    void ProcessWindowModeType();
    WindowModeType CheckWindowModeType();
    void NotifyRSSWindowModeTypeUpdate();
    void CacVisibleWindowNum();
    bool IsVectorSame(const vector<VisibleWindowNumInfo>& lastInfo, const vector<VisibleWindowNumInfo>& currentInfo);
    bool IsKeyboardForeground();
    WindowStatus GetWindowStatus(WindowMode mode, SessionState sessionState,
        const sptr<WindowSessionProperty>& property);
    void DeleteStateDetectTask();
    bool JudgeNeedNotifyPrivacyInfo(DisplayId displayId, unordered_set<string>& privacyBundles);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H