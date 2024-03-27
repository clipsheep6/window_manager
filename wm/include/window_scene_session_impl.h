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

const std::map<OHOS::AppExecFwk::DisplayOrientation, Orientation> ABILITY_TO_SESSION_ORIENTATION_MAP {
    {OHOS::AppExecFwk::DisplayOrientation::UNSPECIFIED,                         Orientation::UNSPECIFIED},
    {OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE,                           Orientation::HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::PORTRAIT,                            Orientation::VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::FOLLOWRECENT,                        Orientation::LOCKED},
    {OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE_INVERTED,                  Orientation::REVERSE_HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::PORTRAIT_INVERTED,                   Orientation::REVERSE_VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION,                       Orientation::SENSOR},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE,             Orientation::SENSOR_HORIZONTAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT,              Orientation::SENSOR_VERTICAL},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_RESTRICTED,            Orientation::AUTO_ROTATION_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED,
        Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED,
        Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED},
    {OHOS::AppExecFwk::DisplayOrientation::LOCKED,                              Orientation::LOCKED},
};

class WindowSceneSessionImpl : public WindowSessionImpl {
public:
    explicit WindowSceneSessionImpl(const sptr<WindowOption>& option);
    ~WindowSceneSessionImpl();
    WMError Create(const std::shared_ptr<AbilityRuntime::Context>& context,
        const sptr<Rosen::ISession>& iSession) override;
    WMError Show(uint32_t reason = 0, bool withAnimation = false) override;
    WMError Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WMError Destroy(bool needNotifyServer, bool needClearListener = true) override;
    WMError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) override;
    void PreProcessCreate();
    void SetDefaultProperty();
    WMError DisableAppWindowDecor() override;
    bool IsDecorEnable() const override;
    WMError Minimize() override;
    WMError MaximizeFloating() override;
    WMError Maximize() override;
    WMError Maximize(MaximizeLayoutOption option) override;
    WMError Recover() override;
    WMError Recover(uint32_t reason) override;
    void StartMove() override;
    WMError Close() override;
    WindowMode GetMode() const override;
    WMError MoveTo(int32_t x, int32_t y) override;
    WMError Resize(uint32_t width, uint32_t height) override;
    WmErrorCode RaiseToAppTop() override;
    WmErrorCode RaiseAboveTarget(int32_t subWindowId) override;
    WSError HandleBackEvent() override;
    void PerformBack() override;
    WMError SetAspectRatio(float ratio) override;
    WMError ResetAspectRatio() override;
    WMError SetGlobalMaximizeMode(MaximizeMode mode) override;
    MaximizeMode GetGlobalMaximizeMode() const override;
    WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) override;
    SystemBarProperty GetSystemBarPropertyByType(WindowType type) const override;
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) override;
    WMError SetLayoutFullScreen(bool status) override;
    WMError SetFullScreen(bool status) override;
    WMError BindDialogTarget(sptr<IRemoteObject> targetToken) override;
    WMError GetWindowLimits(WindowLimits& windowLimits) override;
    WMError SetWindowLimits(WindowLimits& windowLimits) override;
    static void UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    static sptr<Window> GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> GetTopWindowWithId(uint32_t mainWinId);
    static sptr<Window> GetMainWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<WindowSessionImpl> GetMainWindowWithId(uint32_t mainWinId);
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

    virtual WMError SetTransparent(bool isTransparent) override;
    virtual WMError SetTurnScreenOn(bool turnScreenOn) override;
    virtual WMError SetKeepScreenOn(bool keepScreenOn) override;
    virtual WMError SetPrivacyMode(bool isPrivacyMode) override;
    virtual void SetSystemPrivacyMode(bool isSystemPrivacyMode) override;
    virtual WMError SetSnapshotSkip(bool isSkip) override;
    virtual std::shared_ptr<Media::PixelMap> Snapshot() override;
    WMError SetTouchHotAreas(const std::vector<Rect>& rects) override;
    virtual WmErrorCode KeepKeyboardOnFocus(bool keepKeyboardFlag) override;
    virtual WMError SetCallingWindow(uint32_t callingWindowId) override;

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
    WSError UpdateMaximizeMode(MaximizeMode mode) override;
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height) override;
    void NotifySessionForeground(uint32_t reason, bool withAnimation) override;
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WMError NotifyPrepareClosePiPWindow() override;
    void UpdateSubWindowState(const WindowType& type);
    WMError SetSpecificBarProperty(WindowType type, const SystemBarProperty& property) override;
    void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError NotifyDialogStateChange(bool isForeground) override;
    WMError HideNonSecureWindows(bool shouldHide) override;

protected:
    void DestroySubWindow();
    WMError CreateAndConnectSpecificSession();
    WMError CreateSystemWindow(WindowType type);
    WMError RecoverAndConnectSpecificSession();
    WMError RecoverAndReconnectSceneSession();
    sptr<WindowSessionImpl> FindParentSessionByParentId(uint32_t parentId);
    bool isSessionMainWindow(uint32_t parentId);
    sptr<WindowSessionImpl> FindMainWindowWithContext();
    void UpdateSubWindowStateAndNotify(int32_t parentPersistentId, const WindowState& newState);
    void LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height);
    void UpdateFloatingWindowSizeBySizeLimits(uint32_t& width, uint32_t& height) const;
    WMError NotifyWindowSessionProperty();
    WMError NotifyWindowNeedAvoid(bool status = false);
    WMError SetLayoutFullScreenByApiVersion(bool status) override;
    void UpdateWindowSizeLimits();
    WindowLimits GetSystemSizeLimits(uint32_t displayWidth, uint32_t displayHeight, float vpr);
    void GetConfigurationFromAbilityInfo();
    WMError NotifySpecificWindowSessionProperty(WindowType type, const SystemBarProperty& property);

private:
    WMError DestroyInner(bool needNotifyServer);
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
        const MMI::PointerEvent::PointerItem& pointerItem);
    bool HandlePointDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        const MMI::PointerEvent::PointerItem& pointerItem, int32_t sourceType, float vpr, const WSRect& rect);
    bool enableDefaultAnimation_ = true;
    sptr<IAnimationTransitionController> animationTransitionController_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_IMPL_H
