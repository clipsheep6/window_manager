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

#ifndef OHOS_ROSEN_WINDOW_SESSION_PROPERTY_H
#define OHOS_ROSEN_WINDOW_SESSION_PROPERTY_H

#include <refbase.h>
#include <string>
#include <unordered_map>
#include <parcel.h>
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "wm_common.h"
#include "dm_common.h"
#include <cfloat>
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
class WindowSessionProperty;
using HandlWritePropertyFunc = bool (WindowSessionProperty::*)(Parcel& parcel);
using HandlReadPropertyFunc = void (WindowSessionProperty::*)(Parcel& parcel);

class WindowSessionProperty : public Parcelable {
public:
    friend class HidumpController;
    WindowSessionProperty() = default;
    ~WindowSessionProperty() = default;
    explicit WindowSessionProperty(const sptr<WindowSessionProperty>& property);
    void CopyFrom(const sptr<WindowSessionProperty>& property);
    void SetWindowName(const std::string& name);
    void SetSessionInfo(const SessionInfo& info);
    void SetRequestRect(const struct Rect& rect);
    void SetRectAnimationConfig(const RectAnimationConfig& rectAnimationConfig);
    void SetWindowRect(const struct Rect& rect);
    void SetFocusable(bool isFocusable);
    void SetFocusableOnShow(bool isFocusableOnShow);
    void SetTouchable(bool isTouchable);
    void SetDragEnabled(bool dragEnabled);
    void SetHideNonSystemFloatingWindows(bool hide);
    void SetSkipSelfWhenShowOnVirtualScreen(bool isSkip);
    void SetSkipEventOnCastPlus(bool isSkip);
    void SetForceHide(bool hide);
    void SetRaiseEnabled(bool raiseEnabled);
    void SetSystemCalling(bool isSystemCalling);
    void SetTurnScreenOn(bool turnScreenOn);
    void SetKeepScreenOn(bool keepScreenOn);
    void SetViewKeepScreenOn(bool keepScreenOn);
    void SetRequestedOrientation(Orientation orientation, bool needAnimation = true);
    void SetDefaultRequestedOrientation(Orientation orientation);
    void SetPrivacyMode(bool isPrivate);
    void SetSystemPrivacyMode(bool isSystemPrivate);
    void SetSnapshotSkip(bool isSkip);
    void SetBrightness(float brightness);
    void SetDisplayId(uint64_t displayId);
    void SetWindowType(WindowType type);
    void SetParentId(int32_t parentId);
    void SetPersistentId(int32_t persistentId);
    void SetParentPersistentId(int32_t persistentId);
    void SetAccessTokenId(uint32_t accessTokenId);
    void SetTokenState(bool hasToken);
    void SetMaximizeMode(MaximizeMode mode);
    void SetWindowMode(WindowMode mode);
    void SetWindowLimits(const WindowLimits& windowLimits);
    void SetUserWindowLimits(const WindowLimits& windowLimits);
    void SetConfigWindowLimitsVP(const WindowLimits& windowLimitsVP);
    void SetLastLimitsVpr(float vpr);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& property);
    void SetKeyboardLayoutParams(const KeyboardLayoutParams& params);
    void SetDecorEnable(bool isDecorEnable);
    void SetAnimationFlag(uint32_t animationFlag);
    void SetTransform(const Transform& trans);
    void SetWindowFlags(uint32_t flags);
    void SetTopmost(bool topmost);
    bool IsTopmost() const;
    void SetMainWindowTopmost(bool isTopmost);
    bool IsMainWindowTopmost() const;
    void AddWindowFlag(WindowFlag flag);
    void SetWindowModeSupportType(uint32_t windowModeSupportType);
    void SetFloatingWindowAppType(bool isAppType);
    void SetTouchHotAreas(const std::vector<Rect>& rects);
    void SetKeyboardTouchHotAreas(const KeyboardTouchHotAreas& keyboardTouchHotAreas);
    void KeepKeyboardOnFocus(bool keepKeyboardFlag);
    void SetIsNeedUpdateWindowMode(bool isNeedUpdateWindowMode);
    void SetCallingSessionId(uint32_t sessionId);
    void SetPiPTemplateInfo(const PiPTemplateInfo& pipTemplateInfo);
    void SetWindowMask(const std::shared_ptr<Media::PixelMap>& windowMask);
    void SetIsShaped(bool isShaped);
    void SetCompatibleModeInPc(bool compatibleModeInPc);
    void SetCompatibleModeInPcTitleVisible(bool compatibleModeInPcTitleVisible);
    void SetCompatibleWindowSizeInPc(int32_t portraitWidth, int32_t portraitHeight,
        int32_t landscapeWidth, int32_t landscapeHeight);
    void SetIsAppSupportPhoneInPc(bool isSupportPhone);
    void SetIsSupportDragInPcCompatibleMode(bool isSupportDragInPcCompatibleMode);
    void SetIsPcAppInPad(bool isPcAppInPad);
    void SetCompatibleModeEnableInPad(bool enable);
    void SetIsAtomicService(bool isAtomicService);
    
    /*
     * Window Immersive
     */
    void SetAvoidAreaOption(uint32_t avoidAreaOption);
    uint32_t GetAvoidAreaOption() const;

    bool GetIsNeedUpdateWindowMode() const;
    const std::string& GetWindowName() const;
    const SessionInfo& GetSessionInfo() const;
    SessionInfo& EditSessionInfo();
    Rect GetWindowRect() const;
    Rect GetRequestRect() const;
    RectAnimationConfig GetRectAnimationConfig() const;
    WindowType GetWindowType() const;
    bool GetDragEnabled() const;
    bool GetTouchable() const;
    bool GetHideNonSystemFloatingWindows() const;
    bool GetSkipSelfWhenShowOnVirtualScreen() const;
    bool GetSkipEventOnCastPlus() const;
    bool GetForceHide() const;
    bool GetRaiseEnabled() const;
    bool GetSystemCalling() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    bool IsViewKeepScreenOn() const;
    Orientation GetRequestedOrientation() const;
    bool GetRequestedAnimation() const;
    Orientation GetDefaultRequestedOrientation() const;
    bool GetPrivacyMode() const;
    bool GetSystemPrivacyMode() const;
    bool GetSnapshotSkip() const;
    float GetBrightness() const;
    int32_t GetParentId() const;
    uint32_t GetWindowFlags() const;
    uint64_t GetDisplayId() const;
    int32_t GetPersistentId() const;
    int32_t GetParentPersistentId() const;
    uint32_t GetAccessTokenId() const;
    bool GetTokenState() const;
    MaximizeMode GetMaximizeMode() const;
    WindowMode GetWindowMode() const;
    WindowLimits GetWindowLimits() const;
    WindowLimits GetUserWindowLimits() const;
    WindowLimits GetConfigWindowLimitsVP() const;
    float GetLastLimitsVpr() const;
    uint32_t GetWindowModeSupportType() const;
    std::unordered_map<WindowType, SystemBarProperty> GetSystemBarProperty() const;
    bool IsDecorEnable();
    uint32_t GetAnimationFlag() const;
    const Transform& GetTransform() const;
    bool IsFloatingWindowAppType() const;
    void GetTouchHotAreas(std::vector<Rect>& rects) const;
    KeyboardTouchHotAreas GetKeyboardTouchHotAreas() const;
    bool GetKeepKeyboardFlag() const;
    uint32_t GetCallingSessionId() const;
    PiPTemplateInfo GetPiPTemplateInfo() const;
    std::shared_ptr<Media::PixelMap> GetWindowMask() const;
    bool GetIsShaped() const;
    KeyboardLayoutParams GetKeyboardLayoutParams() const;
    bool GetCompatibleModeInPc() const;
    bool GetCompatibleModeInPcTitleVisible() const;
    int32_t GetCompatibleInPcPortraitWidth() const;
    int32_t GetCompatibleInPcPortraitHeight() const;
    int32_t GetCompatibleInPcLandscapeWidth() const;
    int32_t GetCompatibleInPcLandscapeHeight() const;
    bool GetIsAppSupportPhoneInPc() const;
    bool GetIsPcAppInPad() const;
    bool GetIsSupportDragInPcCompatibleMode() const;
    bool GetCompatibleModeEnableInPad() const;
    bool GetIsAtomicService() const;

    bool MarshallingWindowLimits(Parcel& parcel) const;
    static void UnmarshallingWindowLimits(Parcel& parcel, WindowSessionProperty* property);
    bool MarshallingSystemBarMap(Parcel& parcel) const;
    static void UnMarshallingSystemBarMap(Parcel& parcel, WindowSessionProperty* property);
    bool MarshallingPiPTemplateInfo(Parcel& parcel) const;
    static void UnmarshallingPiPTemplateInfo(Parcel& parcel, WindowSessionProperty* property);
    bool Marshalling(Parcel& parcel) const override;
    static WindowSessionProperty* Unmarshalling(Parcel& parcel);
    bool MarshallingWindowMask(Parcel& parcel) const;
    static void UnmarshallingWindowMask(Parcel& parcel, WindowSessionProperty* property);
    bool MarshallingMainWindowTopmost(Parcel& parcel) const;
    static void UnmarshallingMainWindowTopmost(Parcel& parcel, WindowSessionProperty* property);
    bool MarshallingSessionInfo(Parcel& parcel) const;
    static bool UnmarshallingSessionInfo(Parcel& parcel, WindowSessionProperty* property);

    void SetTextFieldPositionY(double textFieldPositionY);
    void SetTextFieldHeight(double textFieldHeight);

    WindowState GetWindowState() const;
    void SetWindowState(WindowState state);

    uint8_t GetBackgroundAlpha() const;
    void SetBackgroundAlpha(uint8_t alpha);

    double GetTextFieldPositionY() const;
    double GetTextFieldHeight() const;

    void SetSessionPropertyChangeCallback(std::function<void()>&& callback);
    bool IsLayoutFullScreen() const;
    void SetIsLayoutFullScreen(bool isLayoutFullScreen);
    int32_t GetCollaboratorType() const;
    void SetCollaboratorType(int32_t collaboratorType);
    bool Write(Parcel& parcel, WSPropertyChangeAction action);
    void Read(Parcel& parcel, WSPropertyChangeAction action);
    void SetFullScreenStart(bool fullScreenStart);
    bool GetFullScreenStart() const;
    void SetApiVersion(uint32_t version);
    uint32_t GetApiVersion() const;

    /*
     * Sub Window
     */
    void SetSubWindowLevel(uint32_t subWindowLevel);
    uint32_t GetSubWindowLevel() const;

    /*
     * Window Hierarchy
     */
    void SetSubWindowZLevel(int32_t zLevel);
    int32_t GetSubWindowZLevel() const;

    /*
     * Window Property
     */
    void SetWindowCornerRadius(float cornerRadius);
    float GetWindowCornerRadius() const;

    /*
     * UIExtension
     */
    void SetRealParentId(int32_t realParentId);
    int32_t GetRealParentId() const;
    void SetUIExtensionUsage(UIExtensionUsage uiExtensionUsage);
    UIExtensionUsage GetUIExtensionUsage() const;
    void SetIsUIExtFirstSubWindow(bool isUIExtFirstSubWindow);
    bool GetIsUIExtFirstSubWindow() const;
    void SetIsUIExtensionAbilityProcess(bool isUIExtensionAbilityProcess);
    bool GetIsUIExtensionAbilityProcess() const;
    void SetParentWindowType(WindowType parentWindowType);
    WindowType GetParentWindowType() const;
    void SetIsUIExtAnySubWindow(bool isUIExtAnySubWindow);
    bool GetIsUIExtAnySubWindow() const;
    void SetConstrainedModal(bool isConstrainedModal);
    bool IsConstrainedModal() const;

    /*
     * Multi Instance
     */
    void SetAppInstanceKey(const std::string& appInstanceKey);
    std::string GetAppInstanceKey() const;

    /*
     * PC Window
     */
    void SetSupportedWindowModes(const std::vector<AppExecFwk::SupportWindowMode>& supportedWindowModes);
    void GetSupportedWindowModes(std::vector<AppExecFwk::SupportWindowMode>& supportedWindowModes) const;
    void SetWindowDelayRaiseEnabled(bool isEnabled);
    bool IsWindowDelayRaiseEnabled() const;
    void SetWindowSizeLimits(const WindowSizeLimits& windowSizeLimits);
    WindowSizeLimits GetWindowSizeLimits() const;
    void SetIsFullScreenWaterfallMode(bool isFullScreenWaterfallMode);
    bool GetIsFullScreenWaterfallMode() const;

    /*
     * Keyboard
     */
    void SetIsSystemKeyboard(bool isSystemKeyboard);
    bool IsSystemKeyboard() const;
    void SetKeyboardViewMode(KeyboardViewMode mode);
    KeyboardViewMode GetKeyboardViewMode() const;

    /*
     * Window focus
     */
    bool GetFocusable() const;
    bool GetFocusableOnShow() const;
    bool GetExclusivelyHighlighted() const;
    void SetExclusivelyHighlighted(bool isExclusivelyHighlighted);
    std::mutex windowMaskMutex_;

private:
    void setTouchHotAreasInner(const std::vector<Rect>& rects, std::vector<Rect>& touchHotAreas);
    bool MarshallingTouchHotAreasInner(const std::vector<Rect>& touchHotAreas, Parcel& parcel) const;
    bool MarshallingTouchHotAreas(Parcel& parcel) const;
    bool MarshallingKeyboardTouchHotAreas(Parcel& parcel) const;
    static void UnmarshallingTouchHotAreasInner(Parcel& parcel, std::vector<Rect>& touchHotAreas);
    static void UnmarshallingTouchHotAreas(Parcel& parcel, WindowSessionProperty* property);
    static void UnmarshallingKeyboardTouchHotAreas(Parcel& parcel, WindowSessionProperty* property);
    bool WriteActionUpdateTurnScreenOn(Parcel& parcel);
    bool WriteActionUpdateKeepScreenOn(Parcel& parcel);
    bool WriteActionUpdateViewKeepScreenOn(Parcel& parcel);
    bool WriteActionUpdateFocusable(Parcel& parcel);
    bool WriteActionUpdateTouchable(Parcel& parcel);
    bool WriteActionUpdateSetBrightness(Parcel& parcel);
    bool WriteActionUpdateOrientation(Parcel& parcel);
    bool WriteActionUpdatePrivacyMode(Parcel& parcel);
    bool WriteActionUpdateSnapshotSkip(Parcel& parcel);
    bool WriteActionUpdateMaximizeState(Parcel& parcel);
    bool WriteActionUpdateSystemBar(Parcel& parcel);
    bool WriteActionUpdateFlags(Parcel& parcel);
    bool WriteActionUpdateMode(Parcel& parcel);
    bool WriteActionUpdateAnimationFlag(Parcel& parcel);
    bool WriteActionUpdateTouchHotArea(Parcel& parcel);
    bool WriteActionUpdateKeyboardTouchHotArea(Parcel& parcel);
    bool WriteActionUpdateDecorEnable(Parcel& parcel);
    bool WriteActionUpdateWindowLimits(Parcel& parcel);
    bool WriteActionUpdateDragenabled(Parcel& parcel);
    bool WriteActionUpdateRaiseenabled(Parcel& parcel);
    bool WriteActionUpdateHideNonSystemFloatingWindows(Parcel& parcel);
    bool WriteActionUpdateTextfieldAvoidInfo(Parcel& parcel);
    bool WriteActionUpdateWindowMask(Parcel& parcel);
    bool WriteActionUpdateTopmost(Parcel& parcel);
    bool WriteActionUpdateMainWindowTopmost(Parcel& parcel);
    bool WriteActionUpdateSubWindowZLevel(Parcel& parcel);
    bool WriteActionUpdateWindowModeSupportType(Parcel& parcel);
    bool WriteActionUpdateAvoidAreaOption(Parcel& parcel);
    bool WriteActionUpdateBackgroundAlpha(Parcel& parcel);
    bool WriteActionUpdateExclusivelyHighlighted(Parcel& parcel);
    void ReadActionUpdateTurnScreenOn(Parcel& parcel);
    void ReadActionUpdateKeepScreenOn(Parcel& parcel);
    void ReadActionUpdateViewKeepScreenOn(Parcel& parcel);
    void ReadActionUpdateFocusable(Parcel& parcel);
    void ReadActionUpdateTouchable(Parcel& parcel);
    void ReadActionUpdateSetBrightness(Parcel& parcel);
    void ReadActionUpdateOrientation(Parcel& parcel);
    void ReadActionUpdatePrivacyMode(Parcel& parcel);
    void ReadActionUpdateSnapshotSkip(Parcel& parcel);
    void ReadActionUpdateMaximizeState(Parcel& parcel);
    void ReadActionUpdateSystemBar(Parcel& parcel);
    void ReadActionUpdateFlags(Parcel& parcel);
    void ReadActionUpdateMode(Parcel& parcel);
    void ReadActionUpdateAnimationFlag(Parcel& parcel);
    void ReadActionUpdateTouchHotArea(Parcel& parcel);
    void ReadActionUpdateKeyboardTouchHotArea(Parcel& parcel);
    void ReadActionUpdateDecorEnable(Parcel& parcel);
    void ReadActionUpdateWindowLimits(Parcel& parcel);
    void ReadActionUpdateDragenabled(Parcel& parcel);
    void ReadActionUpdateRaiseenabled(Parcel& parcel);
    void ReadActionUpdateHideNonSystemFloatingWindows(Parcel& parcel);
    void ReadActionUpdateTextfieldAvoidInfo(Parcel& parcel);
    void ReadActionUpdateWindowMask(Parcel& parcel);
    void ReadActionUpdateTopmost(Parcel& parcel);
    void ReadActionUpdateMainWindowTopmost(Parcel& parcel);
    void ReadActionUpdateSubWindowZLevel(Parcel& parcel);
    void ReadActionUpdateWindowModeSupportType(Parcel& parcel);
    void ReadActionUpdateAvoidAreaOption(Parcel& parcel);
    void ReadActionUpdateBackgroundAlpha(Parcel& parcel);
    void ReadActionUpdateExclusivelyHighlighted(Parcel& parcel);
    std::string windowName_;
    SessionInfo sessionInfo_;
    mutable std::mutex windowRectMutex_;
    Rect windowRect_ { 0, 0, 0, 0 }; // actual window rect
    mutable std::mutex requestRectMutex_;
    Rect requestRect_ { 0, 0, 0, 0 }; // window rect requested by the client (without decoration size)
    mutable std::mutex rectAnimationConfigMutex_;
    RectAnimationConfig rectAnimationConfig_ { 0, 0.0f, 0.0f, 0.0f, 0.0f };
    WindowType type_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW }; // type main window
    bool touchable_ { true };
    bool dragEnabled_ = { true };
    bool raiseEnabled_ = { true };
    bool isSystemCalling_ = { false };
    bool tokenState_ { false };
    bool turnScreenOn_ = false;
    bool keepScreenOn_ = false;
    bool viewKeepScreenOn_ = false;
    bool topmost_ = false;
    bool mainWindowTopmost_ = false;
    Orientation requestedOrientation_ = Orientation::UNSPECIFIED;
    bool needRotateAnimation_ = true;
    Orientation defaultRequestedOrientation_ = Orientation::UNSPECIFIED; // only accessed on SSM thread
    bool isPrivacyMode_ { false };
    bool isSystemPrivacyMode_ { false };
    bool isSnapshotSkip_ { false };
    float brightness_ = UNDEFINED_BRIGHTNESS;
    uint64_t displayId_ = 0;
    int32_t parentId_ = INVALID_SESSION_ID; // parentId of sceneSession, which is low 32 bite of parentPersistentId_
    uint32_t flags_ = 0;
    int32_t persistentId_ = INVALID_SESSION_ID;
    int32_t parentPersistentId_ = INVALID_SESSION_ID;
    uint32_t accessTokenId_ = INVALID_SESSION_ID;
    MaximizeMode maximizeMode_ = MaximizeMode::MODE_RECOVER;
    WindowMode windowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    WindowState windowState_ = WindowState::STATE_INITIAL;
    WindowLimits limits_;
    WindowLimits userLimits_;
    WindowLimits configLimitsVP_;
    float lastVpr_ = 0.0f;
    PiPTemplateInfo pipTemplateInfo_ = {};
    KeyboardLayoutParams keyboardLayoutParams_;
    uint32_t windowModeSupportType_ {WindowModeSupport::WINDOW_MODE_SUPPORT_ALL};
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,           SystemBarProperty(true, 0x00FFFFFF, 0xFF000000) },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR,       SystemBarProperty(true, 0x00FFFFFF, 0xFF000000) },
        { WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, SystemBarProperty(true, 0x00FFFFFF, 0xFF000000) },
    };
    bool isDecorEnable_ = false;
    uint32_t animationFlag_ { static_cast<uint32_t>(WindowAnimation::DEFAULT) };
    // Transform info
    Transform trans_;
    bool isFloatingWindowAppType_ = false;
    mutable std::mutex touchHotAreasMutex_;
    std::vector<Rect> touchHotAreas_;  // coordinates relative to window.
    KeyboardTouchHotAreas keyboardTouchHotAreas_;  // coordinates relative to window.
    bool hideNonSystemFloatingWindows_ = false;
    bool isSkipSelfWhenShowOnVirtualScreen_ = false;
    bool isSkipEventOnCastPlus_ = false;
    bool forceHide_ = false;
    bool keepKeyboardFlag_ = false;
    uint32_t callingSessionId_ = INVALID_SESSION_ID;

    double textFieldPositionY_ = 0.0;
    double textFieldHeight_ = 0.0;
    bool isNeedUpdateWindowMode_ = false;
    std::function<void()> touchHotAreasChangeCallback_;
    bool isLayoutFullScreen_ = false;

    bool isShaped_ = false;
    bool fullScreenStart_ = false;
    std::shared_ptr<Media::PixelMap> windowMask_ = nullptr;
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    static const std::map<uint64_t, HandlWritePropertyFunc> writeFuncMap_;
    static const std::map<uint64_t, HandlReadPropertyFunc> readFuncMap_;
    bool compatibleModeInPc_ = false;
    bool compatibleModeInPcTitleVisible_ = false;
    int32_t compatibleInPcPortraitWidth_ = 0;
    int32_t compatibleInPcPortraitHeight_ = 0;
    int32_t compatibleInPcLandscapeWidth_ = 0;
    int32_t compatibleInPcLandscapeHeight_ = 0;
    bool isAppSupportPhoneInPc_ = false;
    bool isSupportDragInPcCompatibleMode_ = false;
    bool isPcAppInPad_ = false;
    mutable std::mutex compatibleModeMutex_;
    bool compatibleModeEnableInPad_ = false;
    uint8_t backgroundAlpha_ = 0xff; // default alpha is opaque.
    mutable std::mutex atomicServiceMutex_;
    bool isAtomicService_ = false;
    uint32_t apiVersion_ = 0;

    /*
     * Sub Window
     */
    uint32_t subWindowLevel_ = 0;

    /*
     * Window Hierarchy
     */
    int32_t zLevel_ = 0;

    /*
     * UIExtension
     */
    int32_t realParentId_ = INVALID_SESSION_ID;
    UIExtensionUsage uiExtensionUsage_ { UIExtensionUsage::EMBEDDED };
    bool isUIExtFirstSubWindow_ = false;
    bool isUIExtensionAbilityProcess_ = false;
    bool isUIExtAnySubWindow_ = false;
    WindowType parentWindowType_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    bool isConstrainedModal_ = false;

    /*
     * Multi Instance
     */
    std::string appInstanceKey_;

    /*
     * PC Window
     */
    mutable std::mutex supportWindowModesMutex_;
    std::vector<AppExecFwk::SupportWindowMode> supportedWindowModes_;
    bool isWindowDelayRaiseEnabled_ = false;
    WindowSizeLimits windowSizeLimits_;
    bool isFullScreenWaterfallMode_ = false;

    /*
     * Keyboard
     */
    bool isSystemKeyboard_ = false;
    KeyboardViewMode keyboardViewMode_ = KeyboardViewMode::NON_IMMERSIVE_MODE;

    /*
     * Window Immersive
     */
    uint32_t avoidAreaOption_ = 0;

    /*
     * Window Focus
     */
    bool focusable_ { true };
    bool focusableOnShow_ { true };
    bool isExclusivelyHighlighted_ { true };
    
    /*
     * Window Property
     */
    float cornerRadius_ = 0.0f;
    mutable std::mutex cornerRadiusMutex_;
};

struct FreeMultiWindowConfig : public Parcelable {
    bool isSystemDecorEnable_ = true;
    uint32_t decorWindowModeSupportType_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    WindowMode defaultWindowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    uint32_t maxMainFloatingWindowNumber_ = 0;

    virtual bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteBool(isSystemDecorEnable_) ||
            !parcel.WriteUint32(decorWindowModeSupportType_)) {
            return false;
        }

        if (!parcel.WriteUint32(static_cast<uint32_t>(defaultWindowMode_)) ||
            !parcel.WriteUint32(maxMainFloatingWindowNumber_)) {
            return false;
        }
        return true;
    }

    static FreeMultiWindowConfig* Unmarshalling(Parcel& parcel)
    {
        FreeMultiWindowConfig* config = new (std::nothrow) FreeMultiWindowConfig();
        if (config == nullptr) {
            return nullptr;
        }
        config->isSystemDecorEnable_ = parcel.ReadBool();
        config->decorWindowModeSupportType_ = parcel.ReadUint32();
        config->defaultWindowMode_ = static_cast<WindowMode>(parcel.ReadUint32());
        config->maxMainFloatingWindowNumber_ = parcel.ReadUint32();
        return config;
    }
};

struct AppForceLandscapeConfig : public Parcelable {
    int32_t mode_ = 0;
    std::string homePage_;

    AppForceLandscapeConfig() {}
    AppForceLandscapeConfig(int32_t mode, const std::string& homePage) : mode_(mode), homePage_(homePage) {}

    virtual bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteInt32(mode_) ||
            !parcel.WriteString(homePage_)) {
            return false;
        }
        return true;
    }

    static AppForceLandscapeConfig* Unmarshalling(Parcel& parcel)
    {
        AppForceLandscapeConfig* config = new (std::nothrow) AppForceLandscapeConfig();
        if (config == nullptr) {
            return nullptr;
        }
        config->mode_ = parcel.ReadInt32();
        config->homePage_ = parcel.ReadString();
        return config;
    }
};

struct SystemSessionConfig : public Parcelable {
    bool isSystemDecorEnable_ = true;
    uint32_t decorWindowModeSupportType_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    bool isStretchable_ = false;
    WindowMode defaultWindowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    KeyboardAnimationCurve animationIn_;
    KeyboardAnimationCurve animationOut_;
    // 1920: default max window size
    uint32_t maxFloatingWindowSize_ = 1920;
    // 320: default minWidth main window size
    uint32_t miniWidthOfMainWindow_ = 320;
    // 240: default minHeight main window size
    uint32_t miniHeightOfMainWindow_ = 240;
    // 320: default minWidth sub window size
    uint32_t miniWidthOfSubWindow_ = 320;
    // 240: default minHeight sub window size
    uint32_t miniHeightOfSubWindow_ = 240;
    // 320: default minWidth dialog window size
    uint32_t miniWidthOfDialogWindow_ = 320;
    // 240: default minHeight dialog window size
    uint32_t miniHeightOfDialogWindow_ = 240;
    bool backgroundswitch = false;
    bool freeMultiWindowEnable_ = false;
    bool freeMultiWindowSupport_ = false;
    FreeMultiWindowConfig freeMultiWindowConfig_;
    WindowUIType windowUIType_ = WindowUIType::INVALID_WINDOW;
    bool supportTypeFloatWindow_ = false;

    virtual bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteBool(isSystemDecorEnable_) || !parcel.WriteBool(isStretchable_) ||
            !parcel.WriteUint32(decorWindowModeSupportType_)) {
            return false;
        }

        if (!parcel.WriteUint32(static_cast<uint32_t>(defaultWindowMode_)) ||
            !parcel.WriteParcelable(&animationIn_) || !parcel.WriteParcelable(&animationOut_) ||
            !parcel.WriteUint32(maxFloatingWindowSize_)) {
            return false;
        }

        if (!parcel.WriteUint32(miniWidthOfMainWindow_) || !parcel.WriteUint32(miniHeightOfMainWindow_) ||
            !parcel.WriteUint32(miniWidthOfSubWindow_) || !parcel.WriteUint32(miniHeightOfSubWindow_) ||
            !parcel.WriteUint32(miniWidthOfDialogWindow_) || !parcel.WriteUint32(miniHeightOfDialogWindow_)) {
            return false;
        }

        if (!parcel.WriteBool(backgroundswitch)) {
            return false;
        }

        if (!parcel.WriteBool(freeMultiWindowEnable_)) {
            return false;
        }

        if (!parcel.WriteBool(freeMultiWindowSupport_)) {
            return false;
        }
        if (!parcel.WriteParcelable(&freeMultiWindowConfig_)) {
            return false;
        }
        if (!parcel.WriteUint8(static_cast<uint8_t>(windowUIType_))) {
            return false;
        }
        if (!parcel.WriteBool(supportTypeFloatWindow_)) {
            return false;
        }
        return true;
    }

    static SystemSessionConfig* Unmarshalling(Parcel& parcel)
    {
        SystemSessionConfig* config = new (std::nothrow) SystemSessionConfig();
        if (config == nullptr) {
            return nullptr;
        }
        config->isSystemDecorEnable_ = parcel.ReadBool();
        config->isStretchable_ = parcel.ReadBool();
        config->decorWindowModeSupportType_ = parcel.ReadUint32();
        config->defaultWindowMode_ = static_cast<WindowMode>(parcel.ReadUint32());
        sptr<KeyboardAnimationCurve> animationIn = parcel.ReadParcelable<KeyboardAnimationCurve>();
        if (animationIn == nullptr) {
            delete config;
            return nullptr;
        }
        config->animationIn_ = *animationIn;
        sptr<KeyboardAnimationCurve> animationOut = parcel.ReadParcelable<KeyboardAnimationCurve>();
        if (animationOut == nullptr) {
            delete config;
            return nullptr;
        }
        config->animationOut_ = *animationOut;
        config->maxFloatingWindowSize_ = parcel.ReadUint32();
        config->miniWidthOfMainWindow_ = parcel.ReadUint32();
        config->miniHeightOfMainWindow_ = parcel.ReadUint32();
        config->miniWidthOfSubWindow_ = parcel.ReadUint32();
        config->miniHeightOfSubWindow_ = parcel.ReadUint32();
        config->miniWidthOfDialogWindow_ = parcel.ReadUint32();
        config->miniHeightOfDialogWindow_ = parcel.ReadUint32();
        config->backgroundswitch = parcel.ReadBool();
        config->freeMultiWindowEnable_ = parcel.ReadBool();
        config->freeMultiWindowSupport_ = parcel.ReadBool();
        sptr<FreeMultiWindowConfig> freeMultiWindowConfig = parcel.ReadParcelable<FreeMultiWindowConfig>();
        if (freeMultiWindowConfig == nullptr) {
            delete config;
            return nullptr;
        }
        config->freeMultiWindowConfig_ = *freeMultiWindowConfig;
        config->windowUIType_ = static_cast<WindowUIType>(parcel.ReadUint8());
        config->supportTypeFloatWindow_ = parcel.ReadBool();
        return config;
    }

    bool IsFreeMultiWindowMode() const
    {
        return freeMultiWindowEnable_ && freeMultiWindowSupport_;
    }

    bool IsPhoneWindow() const
    {
        return windowUIType_ == WindowUIType::PHONE_WINDOW;
    }

    bool IsPcWindow() const
    {
        return windowUIType_ == WindowUIType::PC_WINDOW;
    }

    bool IsPadWindow() const
    {
        return windowUIType_ == WindowUIType::PAD_WINDOW;
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_PROPERTY_H
