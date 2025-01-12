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

#ifndef OHOS_ROSEN_WINDOW_SCENE_WS_COMMON_H
#define OHOS_ROSEN_WINDOW_SCENE_WS_COMMON_H

#include <inttypes.h>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

#include <iremote_broker.h>
#include <want.h>

namespace OHOS::AAFwk {
class AbilityStartSetting;
class ProcessOptions;
class StartWindowOption;
}
namespace OHOS::AppExecFwk {
struct AbilityInfo;
enum class SupportWindowMode;
}

namespace OHOS::Rosen {
class RSTransaction;
constexpr int32_t ROTATE_ANIMATION_DURATION = 400;
constexpr int32_t INVALID_SESSION_ID = 0;
constexpr int32_t WINDOW_SUPPORT_MODE_MAX_SIZE = 4;

enum class WSError : int32_t {
    WS_OK = 0,
    WS_DO_NOTHING,
    WS_ERROR_NO_MEM,
    WS_ERROR_DESTROYED_OBJECT,
    WS_ERROR_INVALID_WINDOW,
    WS_ERROR_INVALID_WINDOW_MODE_OR_SIZE,
    WS_ERROR_INVALID_OPERATION,
    WS_ERROR_INVALID_PERMISSION,
    WS_ERROR_NOT_SYSTEM_APP,
    WS_ERROR_NO_REMOTE_ANIMATION,
    WS_ERROR_INVALID_DISPLAY,
    WS_ERROR_INVALID_PARENT,
    WS_ERROR_OPER_FULLSCREEN_FAILED,
    WS_ERROR_REPEAT_OPERATION,
    WS_ERROR_INVALID_SESSION,
    WS_ERROR_INVALID_CALLING,
    WS_ERROR_UNCLEARABLE_SESSION,
    WS_ERROR_FAIL_TO_GET_SNAPSHOT,
    WS_ERROR_INTERNAL_ERROR,
    WS_ERROR_NO_UI_CONTENT_ERROR,

    WS_ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change.It is defined on all system

    WS_ERROR_NEED_REPORT_BASE = 1000, // error code > 1000 means need report
    WS_ERROR_NULLPTR,
    WS_ERROR_INVALID_TYPE,
    WS_ERROR_INVALID_PARAM,
    WS_ERROR_SAMGR,
    WS_ERROR_IPC_FAILED,
    WS_ERROR_NEED_REPORT_END,
    WS_ERROR_START_ABILITY_FAILED,
    WS_ERROR_SET_SESSION_LABEL_FAILED,
    WS_ERROR_SET_SESSION_ICON_FAILED,
    WS_ERROR_INVALID_SESSION_LISTENER,
    WS_ERROR_START_UI_EXTENSION_ABILITY_FAILED,
    WS_ERROR_MIN_UI_EXTENSION_ABILITY_FAILED,
    WS_ERROR_TERMINATE_UI_EXTENSION_ABILITY_FAILED,
    WS_ERROR_PRE_HANDLE_COLLABORATOR_FAILED,

    WS_ERROR_EDM_CONTROLLED = 2097215, // enterprise limit
};

enum class WSErrorCode : int32_t {
    WS_OK = 0,
    WS_ERROR_NO_PERMISSION = 201,
    WS_ERROR_INVALID_PARAM = 401,
    WS_ERROR_DEVICE_NOT_SUPPORT = 801,
    WS_ERROR_TIMEOUT = 901,
    WS_ERROR_NOT_REGISTER_SYNC_CALLBACK = 100011,
    WS_ERROR_TRANSFER_DATA_FAILED       = 100012,
    WS_ERROR_REPEAT_OPERATION = 1300001,
    WS_ERROR_STATE_ABNORMALLY = 1300002,
    WS_ERROR_SYSTEM_ABNORMALLY = 1300003,
    WS_ERROR_INVALID_CALLING = 1300004,
    WS_ERROR_STAGE_ABNORMALLY = 1300005,
    WS_ERROR_CONTEXT_ABNORMALLY = 1300006,

    WS_ERROR_EDM_CONTROLLED = 16000013, // enterprise limit
};

const std::map<WSError, WSErrorCode> WS_JS_TO_ERROR_CODE_MAP {
    { WSError::WS_OK,                    WSErrorCode::WS_OK },
    { WSError::WS_DO_NOTHING,            WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
    { WSError::WS_ERROR_INVALID_SESSION, WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
    { WSError::WS_ERROR_INVALID_PARAM, WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
    { WSError::WS_ERROR_IPC_FAILED,      WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY },
    { WSError::WS_ERROR_NULLPTR,         WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
    { WSError::WS_ERROR_EDM_CONTROLLED,  WSErrorCode::WS_ERROR_EDM_CONTROLLED },
    { WSError::WS_ERROR_INVALID_WINDOW,  WSErrorCode::WS_ERROR_STATE_ABNORMALLY },
};

enum class SessionState : uint32_t {
    STATE_DISCONNECT = 0,
    STATE_CONNECT,
    STATE_FOREGROUND,
    STATE_ACTIVE,
    STATE_INACTIVE,
    STATE_BACKGROUND,
    STATE_END,
};

enum ContinueState {
    CONTINUESTATE_UNKNOWN = -1,
    CONTINUESTATE_ACTIVE = 0,
    CONTINUESTATE_INACTIVE = 1,
    CONTINUESTATE_MAX
};

enum class StartMethod : int32_t {
    START_NORMAL,
    START_CALL
};

/**
 * @brief collaborator type.
 */
enum CollaboratorType : int32_t {
    DEFAULT_TYPE = 0,
    RESERVE_TYPE,
    OTHERS_TYPE,
};

enum AncoSceneState: int32_t {
    DEFAULT_STATE = 0,
    NOTIFY_START,
    NOTIFY_CREATE,
    NOTIFY_LOAD,
    NOTIFY_UPDATE,
    NOTIFY_FOREGROUND,
};

/**
 * @brief collaborator type.
 */
enum SessionOperationType : int32_t {
    TYPE_DEFAULT = 0,
    TYPE_CLEAR,
};

enum class ManagerState : uint32_t {
    MANAGER_STATE_SCREEN_LOCKED = 0,
};

/**
 * @brief Enumerates picture in picture control status.
 */
enum class WsPiPControlStatus : int32_t {
    PLAY = 1,
    PAUSE = 0,
    OPEN = 1,
    CLOSE = 0,
    ENABLED = -2,
    DISABLED = -3,
};

/**
 * @brief Enumerates picture in picture control type.
 */
enum class WsPiPControlType : uint32_t {
    VIDEO_PLAY_PAUSE = 0,
    VIDEO_PREVIOUS = 1,
    VIDEO_NEXT = 2,
    FAST_FORWARD = 3,
    FAST_BACKWARD = 4,
    HANG_UP_BUTTON = 5,
    MICROPHONE_SWITCH = 6,
    CAMERA_SWITCH = 7,
    MUTE_SWITCH = 8,
    END,
};

enum class FocusChangeReason {
    /**
     * default focus change reason
     */
    DEFAULT = 0,

    /**
     * focus change for move up
     */
    MOVE_UP,

    /**
     * focus change for click
     */
    CLICK,

    /**
     * focus change for foreground
     */
    FOREGROUND,

    /**
     * focus change for background
     */
    BACKGROUND,

    /**
     * focus change for split screen.5
     */
    SPLIT_SCREEN,

    /**
     * focus change for full screen
     */
    FULL_SCREEN,

    /**
     * focus change for global search
     */
    SCB_SESSION_REQUEST,

    /**
     * focus change for floating scene
     */
    FLOATING_SCENE,

    /**
     * focus change for losing focus
     */
    SCB_SESSION_REQUEST_UNFOCUS,

    /**
     * focus change for client requerst.10
     */
    CLIENT_REQUEST,

    /**
     * focus change for wind
     */
    WIND,

    /**
     * focus change for app foreground
     */
    APP_FOREGROUND,

    /**
     * focus change for app background
     */
    APP_BACKGROUND,

    /**
     * focus change for recent,Multitasking
     */
    RECENT,

    /**
     * focus change for inner app.
     */
    SCB_START_APP,

    /**
     * focus for setting focuable.
     */
    FOCUSABLE,

    /**
     * select last focused app when requestSessionUnFocus.
     */
    LAST_FOCUSED_APP,

    /**
     * focus for zOrder pass through VOICE_INTERACTION.
     */
    VOICE_INTERACTION,

    /**
     * focus change max.
     */
    MAX,
};

enum class SceneType : uint8_t {
    DEFAULT = 0,
    WINDOW_SCENE,
    SYSTEM_WINDOW_SCENE,
    TRANSFORM_SCENE,
    PANEL_SCENE,
    INPUT_SCENE,
};

struct SessionViewportConfig {
    bool isDensityFollowHost_ = false;
    float density_ = 1.0f; // Indicates the host's density, if following the host, use it, otherwise get it from DMS.
    uint64_t displayId_ = 0;
    int32_t orientation_ = 0;
    uint32_t transform_ = 0;
};

struct SessionInfo {
    std::string bundleName_ = "";
    std::string moduleName_ = "";
    std::string abilityName_ = "";
    int32_t appIndex_ = 0;
    bool isSystem_ = false;
    SceneType sceneType_ = SceneType::WINDOW_SCENE;
    uint32_t windowType_ = 1; // WINDOW_TYPE_APP_MAIN_WINDOW
    sptr<IRemoteObject> callerToken_ = nullptr;
    sptr<IRemoteObject> rootToken_ = nullptr;
    uint64_t screenId_ = -1;
    bool isPersistentRecover_ = false;
    bool isFromIcon_ = false;

    mutable std::shared_ptr<AAFwk::Want> want = nullptr; // want for ability start
    std::shared_ptr<AAFwk::Want> closeAbilityWant = nullptr;
    std::shared_ptr<AAFwk::AbilityStartSetting> startSetting = nullptr;
    std::shared_ptr<AAFwk::ProcessOptions> processOptions = nullptr;
    std::shared_ptr<AAFwk::StartWindowOption> startWindowOption = nullptr;
    mutable std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = nullptr;
    int32_t resultCode = -1;
    int32_t requestCode = -1;
    int32_t errorCode = -1;
    std::string errorReason = "";
    int32_t persistentId_ = INVALID_SESSION_ID;
    int32_t callerPersistentId_ = INVALID_SESSION_ID;
    std::string callerBundleName_ = "";
    std::string callerAbilityName_ = "";
    uint32_t callState_ = 0;
    uint32_t callingTokenId_ = 0;
    bool reuse = false;
    int32_t windowMode = 0;
    StartMethod startMethod = StartMethod::START_NORMAL;
    bool lockedState = false;
    std::string time = "";
    ContinueState continueState = ContinueState::CONTINUESTATE_ACTIVE;
    int64_t uiAbilityId_ = 0;
    int32_t ancoSceneState = 0;
    bool isClearSession = false;
    std::string sessionAffinity = "";
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    SessionState sessionState_ = SessionState::STATE_DISCONNECT;
    uint32_t requestOrientation_ = 0;
    bool isRotable_ = false;
    bool isSetPointerAreas_ = false;
    bool isCastSession_ = false;
    uint32_t windowInputType_ = 0;
    std::string continueSessionId_ = "";
    bool isCalledRightlyByCallerId_ = false;
    bool fullScreenStart_ = false;
    bool isAtomicService_ = false;
    bool isBackTransition_ = false;
    bool needClearInNotShowRecent_ = false;
    bool isPcOrPadEnableActivation_ = false;
    bool canStartAbilityFromBackground_ = false;
    bool isFoundationCall_ = false;
    int32_t specifiedId = 0;

    /*
     * App Use Control
     */
    bool isUseControlSession = false; // Indicates whether the session is used for controlling a main session.

    /*
     * UIExtension
     */
    int32_t realParentId_ = INVALID_SESSION_ID;
    uint32_t uiExtensionUsage_ = 0;
    bool isAsyncModalBinding_ = false;
    uint32_t parentWindowType_ = 1; // WINDOW_TYPE_APP_MAIN_WINDOW
    SessionViewportConfig config_;

    /*
     * Multi Instance
     */
    bool isNewAppInstance_ = false;
    std::string appInstanceKey_;

    /*
     * PC Window
     */
    std::vector<AppExecFwk::SupportWindowMode> supportedWindowModes;
};

enum class SessionFlag : uint32_t {
    SESSION_FLAG_NEED_AVOID = 1,
    SESSION_FLAG_PARENT_LIMIT = 1 << 1,
    SESSION_FLAG_SHOW_WHEN_LOCKED = 1 << 2,
    SESSION_FLAG_FORBID_SPLIT_MOVE = 1 << 3,
    SESSION_FLAG_WATER_MARK = 1 << 4,
    SESSION_FLAG_END = 1 << 5,
};

enum class SizeChangeReason : uint32_t {
    UNDEFINED = 0,
    MAXIMIZE,
    RECOVER,
    ROTATION,
    DRAG,
    DRAG_START,
    DRAG_END,
    RESIZE,
    RESIZE_WITH_ANIMATION,
    MOVE,
    MOVE_WITH_ANIMATION,
    HIDE,
    TRANSFORM,
    CUSTOM_ANIMATION_SHOW,
    FULL_TO_SPLIT,
    SPLIT_TO_FULL,
    FULL_TO_FLOATING,
    FLOATING_TO_FULL,
    PIP_START,
    PIP_SHOW,
    PIP_AUTO_START,
    PIP_RATIO_CHANGE,
    PIP_RESTORE,
    UPDATE_DPI_SYNC,
    DRAG_MOVE,
    END,
};

inline bool IsMoveToOrDragMove(SizeChangeReason reason)
{
    return reason == SizeChangeReason::MOVE || reason == SizeChangeReason::DRAG_MOVE ||
           reason == SizeChangeReason::MOVE_WITH_ANIMATION;
}

enum class SessionEvent : uint32_t {
    EVENT_MAXIMIZE = 100,
    EVENT_RECOVER,
    EVENT_MINIMIZE,
    EVENT_CLOSE,
    EVENT_START_MOVE,
    EVENT_END_MOVE,
    EVENT_MAXIMIZE_FLOATING,
    EVENT_TERMINATE,
    EVENT_EXCEPTION,
    EVENT_SPLIT_PRIMARY,
    EVENT_SPLIT_SECONDARY,
    EVENT_DRAG_START,
    EVENT_DRAG,
    EVENT_MAXIMIZE_WITHOUT_ANIMATION,
    EVENT_MAXIMIZE_WATERFALL,
    EVENT_END
};

enum class BrokerStates: uint32_t {
    BROKER_UNKOWN = 1,
    BROKER_STARTED = 0,
    BROKER_NOT_START = -1,
};

inline bool GreatOrEqual(double left, double right)
{
    constexpr double epsilon = -0.00001f;
    return (left - right) > epsilon;
}

inline bool LessOrEqual(double left, double right)
{
    constexpr double epsilon = 0.00001f;
    return (left - right) < epsilon;
}

inline bool NearEqual(const double left, const double right, const double epsilon)
{
    return (std::fabs(left - right) <= epsilon);
}

inline bool NearEqual(const float& left, const float& right)
{
    constexpr double epsilon = 0.001f;
    return NearEqual(left, right, epsilon);
}

inline bool NearEqual(const int32_t& left, const int32_t& right)
{
    return left == right;
}

inline bool NearZero(const double left)
{
    constexpr double epsilon = 0.001f;
    return NearEqual(left, 0.0, epsilon);
}

template<typename T>
struct WSRectT {
    T posX_ = 0;
    T posY_ = 0;
    T width_ = 0;
    T height_ = 0;

    bool operator==(const WSRectT<T>& a) const
    {
        return (NearEqual(posX_, a.posX_) && NearEqual(posY_, a.posY_) &&
                NearEqual(width_, a.width_) && NearEqual(height_, a.height_));
    }

    bool operator!=(const WSRectT<T>& a) const
    {
        return !this->operator==(a);
    }

    bool IsEmpty() const
    {
        if (NearZero(posX_) && NearZero(posY_) && NearZero(width_) && NearZero(height_)) {
            return true;
        }
        return false;
    }

    inline bool IsInRegion(int32_t pointX, int32_t pointY) const
    {
        return GreatOrEqual(pointX, posX_) && LessOrEqual(pointX, posX_ + width_) &&
               GreatOrEqual(pointY, posY_) && LessOrEqual(pointY, posY_ + height_);
    }

    inline bool IsOverlap(const WSRectT<T>& rect) const
    {
        int32_t xStart = std::max(posX_, rect.posX_);
        int32_t xEnd = std::min(posX_ + static_cast<int32_t>(width_),
            rect.posX_ + static_cast<int32_t>(rect.width_));
        int32_t yStart = std::max(posY_, rect.posY_);
        int32_t yEnd = std::min(posY_ + static_cast<int32_t>(height_),
            rect.posY_ + static_cast<int32_t>(rect.height_));
        return (yStart < yEnd) && (xStart < xEnd);
    }

    inline bool IsInvalid() const
    {
        return IsEmpty() || LessOrEqual(width_, 0) || LessOrEqual(height_, 0);
    }

    inline std::string ToString() const
    {
        constexpr int precision = 2;
        std::stringstream ss;
        ss << "[" << std::fixed << std::setprecision(precision) << posX_ << " " << posY_ << " " <<
            width_ << " " << height_ << "]";
        return ss.str();
    }
    static const WSRectT<T> EMPTY_RECT;
};

template<typename T>
inline constexpr WSRectT<T> WSRectT<T>::EMPTY_RECT { 0, 0, 0, 0 };

using WSRect = WSRectT<int32_t>;
using WSRectF = WSRectT<float>;

struct WindowShadowConfig {
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;
    float alpha_ = 0.0f;
    float radius_ = 0.0f;
    std::string color_ = "#000000";
};

struct KeyboardSceneAnimationConfig {
    std::string curveType_ = "default";
    float ctrlX1_ = 0.2f;
    float ctrlY1_ = 0.0f;
    float ctrlX2_ = 0.2f;
    float ctrlY2_ = 1.0f;
    uint32_t duration_ = 150;
};

struct WindowAnimationConfig {
    int32_t duration_ = 0;
    std::string curveType_ = "easeOut";
    float ctrlX1_ = 0.2f;
    float ctrlY1_ = 0.0f;
    float ctrlX2_ = 0.2f;
    float ctrlY2_ = 1.0f;
    float scaleX_ = 0.0f;
    float scaleY_ = 0.0f;
    float rotationX_ = 0.0f;
    float rotationY_ = 0.0f;
    float rotationZ_ = 0.0f;
    int32_t angle_ = 0;
    float translateX_ = 0.0f;
    float translateY_ = 0.0f;
    float opacity_ = 0;
};

struct StartingWindowInfo {
    int32_t startingWindowBackgroundId_;
    int32_t startingWindowIconId_;
    uint32_t startingWindowBackgroundColor_;
    std::string startingWindowIconPath_;
};

struct StartingWindowAnimationConfig {
    bool enabled_ = true;
    int duration_ = 200;
    std::string curve_ = "linear";
    float opacityStart_ = 1;
    float opacityEnd_ = 0;
};

struct SystemUIStatusBarConfig {
    bool showInLandscapeMode_ = false;
    std::string immersiveStatusBarBgColor_ = "#4c000000";
    std::string immersiveStatusBarContentColor_ = "#ffffff";
};

struct StatusBarConfig {
    bool showHide_ = false;
    std::string contentColor_;
    std::string backgroundColor_;
};

struct WindowImmersive {
    StatusBarConfig desktopStatusBarConfig_;
    StatusBarConfig leftRightStatusBarConfig_;
    StatusBarConfig upDownStatusBarConfig_;
};

struct AppWindowSceneConfig {
    float floatCornerRadius_ = 0.0f;
    std::string multiWindowUIType_ = "HandsetSmartWindow";
    bool backgroundScreenLock_ = false;
    std::string rotationMode_ = "windowRotation";
    WindowShadowConfig focusedShadow_;
    WindowShadowConfig unfocusedShadow_;
    KeyboardSceneAnimationConfig keyboardAnimationIn_;
    KeyboardSceneAnimationConfig keyboardAnimationOut_;
    WindowAnimationConfig windowAnimation_;
    StartingWindowAnimationConfig startingWindowAnimationConfig_;
    SystemUIStatusBarConfig systemUIStatusBarConfig_;
    WindowImmersive windowImmersive_;
};

struct DeviceScreenConfig {
    std::string rotationPolicy_ = "11"; // default use phone policy
    std::string defaultRotationPolicy_ = "1"; // default unspecified policy
    bool isRightPowerButton_ = true;
};

struct SceneAnimationConfig {
    std::shared_ptr<RSTransaction> rsTransaction_ = nullptr;
    int32_t animationDuration_ = ROTATE_ANIMATION_DURATION;
};

struct RotateAnimationConfig {
    int32_t duration_ = ROTATE_ANIMATION_DURATION;
};

struct SessionEventParam {
    int32_t pointerX_ = 0;
    int32_t pointerY_ = 0;
    int32_t sessionWidth_ = 0;
    int32_t sessionHeight_ = 0;
    uint32_t dragResizeType = 0;
};

/**
 * @brief Enumerates session gravity.
 */
enum class SessionGravity : uint32_t {
    SESSION_GRAVITY_FLOAT = 0,
    SESSION_GRAVITY_BOTTOM,
    SESSION_GRAVITY_DEFAULT,
};

/**
 * @brief TerminateType session terminate type.
 */
enum class TerminateType : uint32_t {
    CLOSE_AND_KEEP_MULTITASK = 0,
    CLOSE_AND_CLEAR_MULTITASK,
    CLOSE_AND_START_CALLER,
    CLOSE_BY_EXCEPTION,
};

/**
 * @brief System animaged scene type.
 */
enum class SystemAnimatedSceneType : uint32_t {
    SCENE_ENTER_MISSION_CENTER, // Enter the mission center
    SCENE_EXIT_MISSION_CENTER, // Exit the mission center
    SCENE_ENTER_TFS_WINDOW, // Three-finger sliding window recovery
    SCENE_EXIT_TFU_WINDOW, // The three-finger up window disappears
    SCENE_ENTER_WINDOW_FULL_SCREEN, // Enter the window full screen
    SCENE_EXIT_WINDOW_FULL_SCREEN, // Exit the window full screen
    SCENE_ENTER_MAX_WINDOW, // Enter the window maximization state
    SCENE_EXIT_MAX_WINDOW, // Exit the window maximization state
    SCENE_ENTER_SPLIT_SCREEN, // Enter the split screen
    SCENE_EXIT_SPLIT_SCREEN, // Exit the split screen
    SCENE_ENTER_APP_CENTER, // Enter the app center
    SCENE_EXIT_APP_CENTER, // Exit the app center
    SCENE_APPEAR_MISSION_CENTER, // A special case scenario that displays the mission center
    SCENE_ENTER_WIND_CLEAR, // Enter win+D in clear screen mode
    SCENE_ENTER_WIND_RECOVER, // Enter win+D in recover mode
    SCENE_ENTER_RECENTS, // Enter recents
    SCENE_EXIT_RECENTS, // Exit recent.
    SCENE_OTHERS, // 1.Default state 2.The state in which the animation ends
};

/**
 * @brief Session UI parameters
 */
struct SessionUIParam {
    bool interactive_ { true };
    WSRect rect_;
    float scaleX_ { 1.0f };
    float scaleY_ { 1.0f };
    float pivotX_ { 1.0f };
    float pivotY_ { 1.0f };
    float transX_ { 0.0f }; // global translateX
    float transY_ { 0.0f }; // global translateY
    uint32_t zOrder_ { 0 };
    std::string sessionName_;
    bool needSync_ { true };
};

enum class SessionUIDirtyFlag {
    NONE = 0,
    VISIBLE = 1,
    INTERACTIVE = 1 << 1,
    RECT = 1 << 2,
    SCALE = 1 << 3,
    TOUCH_HOT_AREA = 1 << 4,
    Z_ORDER = 1 << 5,
    AVOID_AREA = 1 << 6,
    DRAG_RECT = 1 << 7,
    GLOBAL_RECT = 1 << 8,
};

/**
 * @brief State for post-process focus
 */
struct PostProcessFocusState {
    bool enabled_ { false };
    bool isFocused_ { false };
    bool byForeground_ { true };
    FocusChangeReason reason_ { FocusChangeReason::DEFAULT };

    void Reset()
    {
        enabled_ = false;
        isFocused_ = false;
        byForeground_ = true;
        reason_ = FocusChangeReason::DEFAULT;
    }
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_WS_COMMON_H
