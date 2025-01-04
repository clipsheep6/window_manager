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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H

#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {

class WindowSceneSessionImpl : public WindowSessionImpl {
public:
    explicit WindowSceneSessionImpl(const sptr<WindowOption>& option);
    ~WindowSceneSessionImpl();
    WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession, const std::string& identityToken = "") override;
    WMError Show(uint32_t reason = 0, bool withAnimation = false, bool withFocus = true) override;
    WMError Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WMError Destroy(bool needNotifyServer, bool needClearListener = true) override;
    WMError NotifyDrawingCompleted() override;
    WMError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) override;
    void PreProcessCreate();
    void SetDefaultProperty();
    WMError Minimize() override;
    void StartMove() override;
    bool IsStartMoving() override;
    WmErrorCode StartMoveWindow() override;
    WMError Close() override;
    WindowMode GetMode() const override;

    /*
     * Window Layout
     */
    WMError MoveTo(int32_t x, int32_t y, bool isMoveToGlobal = false,
        MoveConfiguration moveConfiguration = {}) override;
    WMError MoveToAsync(int32_t x, int32_t y, MoveConfiguration moveConfiguration = {}) override;
    WMError MoveWindowToGlobal(int32_t x, int32_t y, MoveConfiguration moveConfiguration = {}) override;
    WMError GetGlobalScaledRect(Rect& globalScaledRect) override;
    WMError Resize(uint32_t width, uint32_t height,
        const RectAnimationConfig& rectAnimationConfig = {}) override;
    WMError ResizeAsync(uint32_t width, uint32_t height,
        const RectAnimationConfig& rectAnimationConfig = {}) override;

    WMError RaiseToAppTop() override;
    WMError RaiseAboveTarget(int32_t subWindowId) override;
    void PerformBack() override;
    WMError SetAspectRatio(float ratio) override;
    WMError ResetAspectRatio() override;
    WMError SetGlobalMaximizeMode(MaximizeMode mode) override;
    MaximizeMode GetGlobalMaximizeMode() const override;
    WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea, const Rect& rect = {0, 0, 0, 0}) override;
    SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    WMError SetLayoutFullScreen(bool status) override;
    WMError SetFullScreen(bool status) override;
    WMError BindDialogTarget(sptr<IRemoteObject> targetToken) override;
    WMError SetDialogBackGestureEnabled(bool isEnabled) override;
    WMError GetWindowLimits(WindowLimits& windowLimits) override;
    WMError SetWindowLimits(WindowLimits& windowLimits) override;
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    static sptr<Window> GetMainWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<WindowSessionImpl> GetMainWindowWithId(uint32_t mainWinId);
    static sptr<WindowSessionImpl> GetWindowWithId(uint32_t windId);
    // only main window, sub window, dialog window can use
    static int32_t GetParentMainWindowId(int32_t windowId);
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;
    WMError NotifyMemoryLevel(int32_t level) override;

    virtual WMError AddWindowFlag(WindowFlag flag) override;
    virtual WMError RemoveWindowFlag(WindowFlag flag) override;
    virtual WMError SetWindowFlags(uint32_t flags) override;
    virtual uint32_t GetWindowFlags() const override;

    // window effect
    virtual WMError SetCornerRadius(float cornerRadius) override;
    virtual WMError SetShadowRadius(float radius) override;
    virtual WMError SetShadowColor(std::string color) override;
    virtual WMError SetShadowOffsetX(float offsetX) override;
    virtual WMError SetShadowOffsetY(float offsetY) override;
    virtual WMError SetBlur(float radius) override;
    virtual WMError SetBackdropBlur(float radius) override;
    virtual WMError SetBackdropBlurStyle(WindowBlurStyle blurStyle) override;
    virtual WMError SetWindowMode(WindowMode mode) override;
    virtual WMError SetGrayScale(float grayScale) override;

    virtual WMError SetTransparent(bool isTransparent) override;
    virtual WMError SetTurnScreenOn(bool turnScreenOn) override;
    virtual WMError SetKeepScreenOn(bool keepScreenOn) override;
    virtual WMError SetPrivacyMode(bool isPrivacyMode) override;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) override;
    virtual WMError SetSnapshotSkip(bool isSkip) override;
    virtual std::shared_ptr<Media::PixelMap> Snapshot() override;
    WMError SetTouchHotAreas(const std::vector<Rect>& rects) override;
    WMError SetKeyboardTouchHotAreas(const KeyboardTouchHotAreas& hotAreas) override;
    virtual WmErrorCode KeepKeyboardOnFocus(bool keepKeyboardFlag) override;
    virtual WMError SetCallingWindow(uint32_t callingSessionId) override;

    virtual bool IsTransparent() const override;
    virtual bool IsTurnScreenOn() const override;
    virtual bool IsKeepScreenOn() const override;
    virtual bool IsPrivacyMode() const override;
    virtual bool IsLayoutFullScreen() const override;
    virtual bool IsFullScreen() const override;
    WMError RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener) override;
    void SetNeedDefaultAnimation(bool needDefaultAnimation) override;
    WMError SetTransform(const Transform& trans) override;
    const Transform& GetTransform() const override;
    WMError UpdateSurfaceNodeAfterCustomAnimation(bool isAdd) override;
    WMError SetAlpha(float alpha) override;
    void DumpSessionElementInfo(const std::vector<std::string>& params) override;
    WSError UpdateWindowMode(WindowMode mode) override;
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height) override;
    void NotifySessionForeground(uint32_t reason, bool withAnimation) override;
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WMError NotifyPrepareClosePiPWindow() override;
    void UpdateSubWindowState(const WindowType& type);
    WMError SetSystemBarProperties(const std::map<WindowType, SystemBarProperty>& properties,
        const std::map<WindowType, SystemBarPropertyFlag>& propertyFlags) override;
    WMError GetSystemBarProperties(std::map<WindowType, SystemBarProperty>& properties) override;
    WMError SetSpecificBarProperty(WindowType type, const SystemBarProperty& property) override;
    void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    bool PreNotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError NotifyDialogStateChange(bool isForeground) override;
    WMError SetDefaultDensityEnabled(bool enabled) override;
    bool GetDefaultDensityEnabled() override;
    WMError HideNonSecureWindows(bool shouldHide) override;
    void UpdateDensity() override;
    WSError UpdateOrientation() override;
    WSError UpdateDisplayId(uint64_t displayId) override;
    WMError AdjustKeyboardLayout(const KeyboardLayoutParams& params) override;

    /*
     * PC Window
     */
    bool IsPcOrPadCapabilityEnabled() const override;
    bool IsPcOrPadFreeMultiWindowMode() const override;
    WMError SetWindowMask(const std::vector<std::vector<uint32_t>>& windowMask) override;

    /*
     * PC Window Layout
     */
    WMError Restore() override;
    WMError SetTitleAndDockHoverShown(bool isTitleHoverShown = true,
        bool isDockHoverShown = true) override;
    WMError SetWindowRectAutoSave(bool enabled) override;
    WMError IsWindowRectAutoSave(bool& enabled) override;
    WMError MaximizeFloating() override;
    WMError Maximize() override;
    WMError Maximize(MaximizePresentation presentation) override;
    WMError Recover() override;
    WMError Recover(uint32_t reason) override;
    WSError UpdateMaximizeMode(MaximizeMode mode) override;
    WMError SetSupportWindowModes(const std::vector<AppExecFwk::SupportWindowMode>& supportWindowModes) override;
    WmErrorCode StopMoveWindow() override;

    /*
     * Compatible Mode
     */
    WSError NotifyCompatibleModeEnableInPad(bool enabled) override;
    WSError CompatibleFullScreenRecover() override;
    WSError CompatibleFullScreenMinimize() override;
    WSError CompatibleFullScreenClose() override;

    /*
     * Free Multi Window
     */
    WSError SwitchFreeMultiWindow(bool enable) override;
    virtual bool GetFreeMultiWindowModeEnabledState() override;

    void NotifyKeyboardPanelInfoChange(const KeyboardPanelInfo& keyboardPanelInfo) override;
    virtual WMError SetImmersiveModeEnabledState(bool enable) override;
    virtual bool GetImmersiveModeEnabledState() const override;
    void NotifySessionFullScreen(bool fullScreen) override;
    WMError GetWindowStatus(WindowStatus& windowStatus) override;
    bool GetIsUIExtFirstSubWindow() const override;
    bool GetIsUIExtAnySubWindow() const override;

    /*
     * Gesture Back
     */
    WMError SetGestureBackEnabled(bool enable) override;
    WMError GetGestureBackEnabled(bool& enable) override;

    /*
     * PC Fold Screen
     */
    WSError SetFullScreenWaterfallMode(bool isWaterfallMode) override;
    WSError SetSupportEnterWaterfallMode(bool isSupportEnter) override;
    WMError OnContainerModalEvent(const std::string& eventName, const std::string& value) override;

    /*
     * Window Property
     */
    static void UpdateConfigurationSyncForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    void UpdateConfigurationSync(const std::shared_ptr<AppExecFwk::Configuration>& configuration) override;
    float GetCustomDensity() const override;
    WMError SetCustomDensity(float density) override;
    WMError GetWindowDensityInfo(WindowDensityInfo& densityInfo) override;

    /*
     * Window Decor
     */
    WMError SetWindowTitle(const std::string& title) override;
    WMError DisableAppWindowDecor() override;
    bool IsDecorEnable() const override;

    /*
     * Starting Window
     */
    WMError NotifyRemoveStartingWindow() override;

protected:
    WMError CreateAndConnectSpecificSession();
    WMError CreateSystemWindow(WindowType type);
    WMError RecoverAndConnectSpecificSession();
    WMError RecoverAndReconnectSceneSession();
    sptr<WindowSessionImpl> FindParentSessionByParentId(uint32_t parentId);
    bool IsSessionMainWindow(uint32_t parentId);
    void LimitWindowSize(uint32_t& width, uint32_t& height);
    void LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height, float& vpr);
    void UpdateFloatingWindowSizeBySizeLimits(uint32_t& width, uint32_t& height) const;
    WMError NotifyWindowSessionProperty();
    WMError NotifyWindowNeedAvoid(bool status = false);
    WMError SetLayoutFullScreenByApiVersion(bool status) override;
    void UpdateWindowSizeLimits();
    WindowLimits GetSystemSizeLimits(uint32_t displayWidth, uint32_t displayHeight, float vpr);
    void GetConfigurationFromAbilityInfo();
    float GetVirtualPixelRatio(const sptr<DisplayInfo>& displayInfo) override;
    WMError NotifySpecificWindowSessionProperty(WindowType type, const SystemBarProperty& property);
    using SessionMap = std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>;
    sptr<WindowSessionImpl> FindParentMainSession(uint32_t parentId, const SessionMap& sessionMap);

private:
    WMError DestroyInner(bool needNotifyServer);
    WMError MainWindowCloseInner();
    WMError SyncDestroyAndDisconnectSpecificSession(int32_t persistentId);
    bool IsValidSystemWindowType(const WindowType& type);
    WMError CheckParmAndPermission();
    static uint32_t maxFloatingWindowSize_;
    void TransformSurfaceNode(const Transform& trans);
    void AdjustWindowAnimationFlag(bool withAnimation = false);
    void RegisterSessionRecoverListener(bool isSpecificSession);
    WMError UpdateAnimationFlagProperty(bool withAnimation);
    WMError UpdateWindowModeImmediately(WindowMode mode);
    uint32_t UpdateConfigVal(uint32_t minVal, uint32_t maxVal, uint32_t configVal, uint32_t defaultVal, float vpr);
    void UpdateWindowState();
    void UpdateNewSize();
    void fillWindowLimits(WindowLimits& windowLimits);
    void ConsumePointerEventInner(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        MMI::PointerEvent::PointerItem& pointerItem);
    void HandleEventForCompatibleMode(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        MMI::PointerEvent::PointerItem& pointerItem);
    void HandleDownForCompatibleMode(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        MMI::PointerEvent::PointerItem& pointerItem);
    void HandleMoveForCompatibleMode(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        MMI::PointerEvent::PointerItem& pointerItem);
    void HandleUpForCompatibleMode(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        MMI::PointerEvent::PointerItem& pointerItem);
    void ConvertPointForCompatibleMode(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        MMI::PointerEvent::PointerItem& pointerItem, int32_t transferX);
    bool IsInMappingRegionForCompatibleMode(int32_t displayX, int32_t displayY);
    bool CheckTouchSlop(int32_t pointerId, int32_t x, int32_t y, int32_t threshold);
    void IgnoreClickEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    bool HandlePointDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const MMI::PointerEvent::PointerItem& pointerItem, int32_t sourceType, float vpr, const WSRect& rect);
    std::unique_ptr<Media::PixelMap> HandleWindowMask(const std::vector<std::vector<uint32_t>>& windowMask);
    void CalculateNewLimitsByLimits(
        WindowLimits& newLimits, WindowLimits& customizedLimits, float& virtualPixelRatio);
    void CalculateNewLimitsByRatio(WindowLimits& newLimits, WindowLimits& customizedLimits);
    void NotifyDisplayInfoChange(const sptr<DisplayInfo>& info = nullptr);
    void UpdateDensityInner(const sptr<DisplayInfo>& info = nullptr);

    /*
     * Window Layout
     */
    void CheckMoveConfiguration(MoveConfiguration& moveConfiguration);

    /*
     * Window Immersive
     */
    void UpdateDefaultStatusBarColor();
    WMError MoveAndResizeKeyboard(const KeyboardLayoutParams& params);
    bool userLimitsSet_ = false;
    bool enableDefaultAnimation_ = true;
    sptr<IAnimationTransitionController> animationTransitionController_;
    uint32_t setSameSystembarPropertyCnt_ = 0;
    std::atomic<bool> isDefaultDensityEnabled_ = false;
    std::atomic<uint32_t> getAvoidAreaCnt_ = 0;
    bool enableImmersiveMode_ = false;
    bool titleHoverShowEnabled_ = true;
    bool dockHoverShowEnabled_ = true;
    void PreLayoutOnShow(WindowType type, const sptr<DisplayInfo>& info = nullptr);

    /*
     * Gesture Back
     */
    bool gestureBackEnabled_ = true;

    /*
     * Window Property
     */
    void InitSystemSessionDragEnable();
    bool IsSystemDensityChanged(const sptr<DisplayInfo>& displayInfo);
    bool IsDefaultDensityEnabled();
    float GetMainWindowCustomDensity();
    float customDensity_ = UNDEFINED_DENSITY;

    /*
     * Sub Window
     */
    void AddSubWindowMapForExtensionWindow();
    WMError GetParentSessionAndVerify(bool isToast, sptr<WindowSessionImpl>& parentSession);

    WMError RegisterKeyboardPanelInfoChangeListener(const sptr<IKeyboardPanelInfoChangeListener>& listener) override;
    WMError UnregisterKeyboardPanelInfoChangeListener(const sptr<IKeyboardPanelInfoChangeListener>& listener) override;
    static std::mutex keyboardPanelInfoChangeListenerMutex_;
    sptr<IKeyboardPanelInfoChangeListener> keyboardPanelInfoChangeListeners_ = nullptr;
    bool isOverTouchSlop_ = false;
    bool isDown_ = false;
    std::unordered_map<int32_t, std::vector<bool>> eventMapTriggerByDisplay_;
    std::unordered_map<int32_t, std::vector<int32_t>> eventMapDeltaXByDisplay_;
    std::unordered_map<int32_t, std::vector<PointInfo>> downPointerByDisplay_;

    /*
     * PC Fold Screen
     */
    std::atomic_bool isFullScreenWaterfallMode_ { false };
    std::atomic<WindowMode> lastWindowModeBeforeWaterfall_ { WindowMode::WINDOW_MODE_UNDEFINED };
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H
