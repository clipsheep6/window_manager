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

#ifndef OHOS_ROSEN_WM_COMMON_ENUM_H
#define OHOS_ROSEN_WM_COMMON_ENUM_H

#include <sstream>

namespace OHOS {
namespace Rosen {
/**
 * @brief Enumerates type of window.
 */
enum class WindowType : uint32_t {
    APP_WINDOW_BASE = 1,
    APP_MAIN_WINDOW_BASE = APP_WINDOW_BASE,
    WINDOW_TYPE_APP_MAIN_WINDOW = APP_MAIN_WINDOW_BASE,
    APP_MAIN_WINDOW_END,

    APP_SUB_WINDOW_BASE = 1000,
    WINDOW_TYPE_MEDIA = APP_SUB_WINDOW_BASE,
    WINDOW_TYPE_APP_SUB_WINDOW,
    WINDOW_TYPE_APP_COMPONENT,
    APP_SUB_WINDOW_END,
    APP_WINDOW_END = APP_SUB_WINDOW_END,

    SYSTEM_WINDOW_BASE = 2000,
    BELOW_APP_SYSTEM_WINDOW_BASE = SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_WALLPAPER = SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_DESKTOP,
    BELOW_APP_SYSTEM_WINDOW_END,

    ABOVE_APP_SYSTEM_WINDOW_BASE = 2100,
    WINDOW_TYPE_APP_LAUNCHING = ABOVE_APP_SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_DOCK_SLICE,
    WINDOW_TYPE_INCOMING_CALL,
    WINDOW_TYPE_SEARCHING_BAR,
    WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
    WINDOW_TYPE_INPUT_METHOD_FLOAT,
    WINDOW_TYPE_FLOAT,
    WINDOW_TYPE_TOAST,
    WINDOW_TYPE_STATUS_BAR,
    WINDOW_TYPE_PANEL,
    WINDOW_TYPE_KEYGUARD,
    WINDOW_TYPE_VOLUME_OVERLAY,
    WINDOW_TYPE_NAVIGATION_BAR,
    WINDOW_TYPE_DRAGGING_EFFECT,
    WINDOW_TYPE_POINTER,
    WINDOW_TYPE_LAUNCHER_RECENT,
    WINDOW_TYPE_LAUNCHER_DOCK,
    WINDOW_TYPE_BOOT_ANIMATION,
    WINDOW_TYPE_FREEZE_DISPLAY,
    WINDOW_TYPE_VOICE_INTERACTION,
    WINDOW_TYPE_FLOAT_CAMERA,
    WINDOW_TYPE_PLACEHOLDER,
    WINDOW_TYPE_DIALOG,
    WINDOW_TYPE_SCREENSHOT,
    WINDOW_TYPE_INPUT_METHOD_STATUS_BAR,
    WINDOW_TYPE_GLOBAL_SEARCH,
    WINDOW_TYPE_NEGATIVE_SCREEN,
    WINDOW_TYPE_SYSTEM_TOAST,
    WINDOW_TYPE_SYSTEM_FLOAT,
    WINDOW_TYPE_PIP,
    WINDOW_TYPE_THEME_EDITOR,
    WINDOW_TYPE_NAVIGATION_INDICATOR,
    WINDOW_TYPE_HANDWRITE,
    ABOVE_APP_SYSTEM_WINDOW_END,

    SYSTEM_SUB_WINDOW_BASE = 2500,
    WINDOW_TYPE_SYSTEM_SUB_WINDOW = SYSTEM_SUB_WINDOW_BASE,
    SYSTEM_SUB_WINDOW_END,

    SYSTEM_WINDOW_END = SYSTEM_SUB_WINDOW_END,

    WINDOW_TYPE_UI_EXTENSION = 3000,
    WINDOW_TYPE_SCENE_BOARD,
    WINDOW_TYPE_KEYBOARD_PANEL
};

/**
 * @brief Enumerates mode of window.
 */
enum class WindowMode : uint32_t {
    WINDOW_MODE_UNDEFINED = 0,
    WINDOW_MODE_FULLSCREEN = 1,
    WINDOW_MODE_SPLIT_PRIMARY = 100,
    WINDOW_MODE_SPLIT_SECONDARY,
    WINDOW_MODE_FLOATING,
    WINDOW_MODE_PIP
};

/**
 * @brief Enumerates modeType of window.
 */
enum class WindowModeType : uint8_t {
    WINDOW_MODE_SPLIT_FLOATING = 0,
    WINDOW_MODE_SPLIT = 1,
    WINDOW_MODE_FLOATING = 2,
    WINDOW_MODE_FULLSCREEN = 3,
    WINDOW_MODE_OTHER = 4
};

/**
 * @brief Enumerates mode supported of window.
 */
enum WindowModeSupport : uint32_t {
    WINDOW_MODE_SUPPORT_FULLSCREEN = 1 << 0,
    WINDOW_MODE_SUPPORT_FLOATING = 1 << 1,
    WINDOW_MODE_SUPPORT_SPLIT_PRIMARY = 1 << 2,
    WINDOW_MODE_SUPPORT_SPLIT_SECONDARY = 1 << 3,
    WINDOW_MODE_SUPPORT_PIP = 1 << 4,
    WINDOW_MODE_SUPPORT_ALL = WINDOW_MODE_SUPPORT_FULLSCREEN |
                              WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                              WINDOW_MODE_SUPPORT_SPLIT_SECONDARY |
                              WINDOW_MODE_SUPPORT_FLOATING |
                              WINDOW_MODE_SUPPORT_PIP
};

/**
 * @brief Enumerates blur style of window.
 */
enum class WindowBlurStyle : uint32_t {
    WINDOW_BLUR_OFF = 0,
    WINDOW_BLUR_THIN,
    WINDOW_BLUR_REGULAR,
    WINDOW_BLUR_THICK
};

/**
 * @brief Enumerates state of window.
 */
enum class WindowState : uint32_t {
    STATE_INITIAL,
    STATE_CREATED,
    STATE_SHOWN,
    STATE_HIDDEN,
    STATE_FROZEN,
    STATE_UNFROZEN,
    STATE_DESTROYED,
    STATE_BOTTOM = STATE_DESTROYED, // Add state type after STATE_DESTROYED is not allowed
};

/**
 * @brief Enumerates error code of window.
 */
enum class WMError : int32_t {
    WM_OK = 0,
    WM_DO_NOTHING,
    WM_ERROR_NO_MEM,
    WM_ERROR_DESTROYED_OBJECT,
    WM_ERROR_INVALID_WINDOW,
    WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE,
    WM_ERROR_INVALID_OPERATION,
    WM_ERROR_INVALID_PERMISSION,
    WM_ERROR_NOT_SYSTEM_APP,
    WM_ERROR_NO_REMOTE_ANIMATION,
    WM_ERROR_INVALID_DISPLAY,
    WM_ERROR_INVALID_PARENT,
    WM_ERROR_OPER_FULLSCREEN_FAILED,
    WM_ERROR_REPEAT_OPERATION,
    WM_ERROR_INVALID_SESSION,
    WM_ERROR_INVALID_CALLING,

    WM_ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change.It is defined on all system

    WM_ERROR_NEED_REPORT_BASE = 1000, // error code > 1000 means need report
    WM_ERROR_NULLPTR,
    WM_ERROR_INVALID_TYPE,
    WM_ERROR_INVALID_PARAM,
    WM_ERROR_SAMGR,
    WM_ERROR_IPC_FAILED,
    WM_ERROR_NEED_REPORT_END,
    WM_ERROR_START_ABILITY_FAILED,
    WM_ERROR_PIP_DESTROY_FAILED,
    WM_ERROR_PIP_STATE_ABNORMALLY,
    WM_ERROR_PIP_CREATE_FAILED,
    WM_ERROR_PIP_INTERNAL_ERROR,
    WM_ERROR_PIP_REPEAT_OPERATION,
};

/**
 * @brief Enumerates error code of window only used for js api.
 */
enum class WmErrorCode : int32_t {
    WM_OK = 0,
    WM_ERROR_NO_PERMISSION = 201,
    WM_ERROR_NOT_SYSTEM_APP = 202,
    WM_ERROR_INVALID_PARAM = 401,
    WM_ERROR_DEVICE_NOT_SUPPORT = 801,
    WM_ERROR_REPEAT_OPERATION = 1300001,
    WM_ERROR_STATE_ABNORMALLY = 1300002,
    WM_ERROR_SYSTEM_ABNORMALLY = 1300003,
    WM_ERROR_INVALID_CALLING = 1300004,
    WM_ERROR_STAGE_ABNORMALLY = 1300005,
    WM_ERROR_CONTEXT_ABNORMALLY = 1300006,
    WM_ERROR_START_ABILITY_FAILED = 1300007,
    WM_ERROR_INVALID_DISPLAY = 1300008,
    WM_ERROR_INVALID_PARENT = 1300009,
    WM_ERROR_OPER_FULLSCREEN_FAILED = 1300010,
    WM_ERROR_PIP_DESTROY_FAILED = 1300011,
    WM_ERROR_PIP_STATE_ABNORMALLY = 1300012,
    WM_ERROR_PIP_CREATE_FAILED = 1300013,
    WM_ERROR_PIP_INTERNAL_ERROR = 1300014,
    WM_ERROR_PIP_REPEAT_OPERATION = 1300015,
};

/**
 * @brief Enumerates status of window.
 */
enum class WindowStatus : uint32_t {
    WINDOW_STATUS_UNDEFINED = 0,
    WINDOW_STATUS_FULLSCREEN = 1,
    WINDOW_STATUS_MAXMIZE,
    WINDOW_STATUS_MINIMIZE,
    WINDOW_STATUS_FLOATING,
    WINDOW_STATUS_SPLITSCREEN
};

/**
 * @brief Enumerates setting flag of systemStatusBar
 */
enum class SystemBarSettingFlag : uint32_t {
    DEFAULT_SETTING = 0,
    COLOR_SETTING = 1,
    ENABLE_SETTING = 1 << 1,
    ALL_SETTING = 0b11
};

/**
 * @brief Enumerates flag of window.
 */
enum class WindowFlag : uint32_t {
    WINDOW_FLAG_NEED_AVOID = 1,
    WINDOW_FLAG_PARENT_LIMIT = 1 << 1,
    WINDOW_FLAG_SHOW_WHEN_LOCKED = 1 << 2,
    WINDOW_FLAG_FORBID_SPLIT_MOVE = 1 << 3,
    WINDOW_FLAG_WATER_MARK = 1 << 4,
    WINDOW_FLAG_IS_MODAL = 1 << 5,
    WINDOW_FLAG_HANDWRITING = 1 << 6,
    WINDOW_FLAG_END = 1 << 7,
};

/**
 * @brief Flag of uiextension window.
 */
union ExtensionWindowFlags {
    uint32_t bitData;
    struct {
        // Each flag should be false default, true when active
        bool hideNonSecureWindowsFlag : 1;
        bool waterMarkFlag : 1;
        bool privacyModeFlag : 1;
    };
    ExtensionWindowFlags() : bitData(0) {}
    ExtensionWindowFlags(uint32_t bits) : bitData(bits) {}
    ~ExtensionWindowFlags() {}
};

/**
 * @brief Enumerates window size change reason.
 */
enum class WindowSizeChangeReason : uint32_t {
    UNDEFINED = 0,
    MAXIMIZE,
    RECOVER,
    ROTATION,
    DRAG,
    DRAG_START,
    DRAG_END,
    RESIZE,
    MOVE,
    HIDE,
    TRANSFORM,
    CUSTOM_ANIMATION_SHOW,
    FULL_TO_SPLIT,
    SPLIT_TO_FULL,
    FULL_TO_FLOATING,
    FLOATING_TO_FULL,
    PIP_START,
    PIP_SHOW,
    PIP_RATIO_CHANGE,
    END,
};

/**
 * @brief Enumerates layout mode of window.
 */
enum class WindowLayoutMode : uint32_t {
    BASE = 0,
    CASCADE = BASE,
    TILE = 1,
    END,
};

/**
 * @brief Enumerates drag event.
 */
enum class DragEvent : uint32_t {
    DRAG_EVENT_IN  = 1,
    DRAG_EVENT_OUT,
    DRAG_EVENT_MOVE,
    DRAG_EVENT_END,
};

/**
 * @brief Enumerates window tag.
 */
enum class WindowTag : uint32_t {
    MAIN_WINDOW = 0,
    SUB_WINDOW = 1,
    SYSTEM_WINDOW = 2,
};

/**
 * @brief Enumerates window session type.
 */
enum class WindowSessionType : uint32_t {
    SCENE_SESSION = 0,
    EXTENSION_SESSION = 1,
};

/**
 * @brief Enumerates window gravity.
 */
enum class WindowGravity : uint32_t {
    WINDOW_GRAVITY_FLOAT = 0,
    WINDOW_GRAVITY_BOTTOM,
    WINDOW_GRAVITY_DEFAULT,
};

/**
 * @brief Enumerates window setuicontent type.
 */
enum class WindowSetUIContentType: uint32_t {
    DEFAULT,
    DISTRIBUTE,
    BY_NAME,
    BY_ABC,
};

/**
 * @brief Enumerates avoid area type.
 */
enum class AvoidAreaType : uint32_t {
    TYPE_SYSTEM,           // area of SystemUI
    TYPE_CUTOUT,           // cutout of screen
    TYPE_SYSTEM_GESTURE,   // area for system gesture
    TYPE_KEYBOARD,         // area for soft input keyboard
    TYPE_NAVIGATION_INDICATOR, // area for navigation indicator
};

/**
 * @brief Enumerates occupied area type.
 */
enum class OccupiedAreaType : uint32_t {
    TYPE_INPUT, // area of input window
};

/**
 * @brief Enumerates color space.
 */
enum class ColorSpace : uint32_t {
    COLOR_SPACE_DEFAULT = 0, // Default color space.
    COLOR_SPACE_WIDE_GAMUT,  // Wide gamut color space. The specific wide color gamut depends on the screen.
};

/**
 * @brief Enumerates window animation.
 */
enum class WindowAnimation : uint32_t {
    NONE,
    DEFAULT,
    INPUTE,
    CUSTOM,
};

/**
 * @brief Enumerates window maximize mode.
 */
enum class MaximizeMode : uint32_t {
    MODE_AVOID_SYSTEM_BAR,
    MODE_FULL_FILL,
    MODE_RECOVER,
};

/**
 * @brief Enumerates window update type.
 */
enum class WindowUpdateType : int32_t {
    WINDOW_UPDATE_ADDED = 1,
    WINDOW_UPDATE_REMOVED,
    WINDOW_UPDATE_FOCUSED,
    WINDOW_UPDATE_BOUNDS,
    WINDOW_UPDATE_ACTIVE,
    WINDOW_UPDATE_PROPERTY,
    WINDOW_UPDATE_ALL,
};

/**
 * @brief Enumerates picture in picture window state.
 */
enum class PiPWindowState : uint32_t {
    STATE_UNDEFINED = 0,
    STATE_STARTING = 1,
    STATE_STARTED = 2,
    STATE_STOPPING = 3,
    STATE_STOPPED = 4,
};

/**
 * @brief Enumerates picture in picture template type.
 */
enum class PiPTemplateType : uint32_t {
    VIDEO_PLAY = 0,
    VIDEO_CALL = 1,
    VIDEO_MEETING = 2,
    VIDEO_LIVE = 3,
    END,
};

/**
 * @brief Enumerates picture in picture control group.
 */
enum class PiPControlGroup : uint32_t {
    VIDEO_PLAY_START = 100,
    VIDEO_PREVIOUS_NEXT = 101,
    FAST_FORWARD_BACKWARD = 102,
    VIDEO_PLAY_END,

    VIDEO_CALL_START = 200,
    VIDEO_CALL_MICROPHONE_SWITCH = 201,
    VIDEO_CALL_HANG_UP_BUTTON = 202,
    VIDEO_CALL_CAMERA_SWITCH = 203,
    VIDEO_CALL_END,

    VIDEO_MEETING_START = 300,
    VIDEO_MEETING_HANG_UP_BUTTON = 301,
    VIDEO_MEETING_CAMERA_SWITCH = 302,
    VIDEO_MEETING_MUTE_SWITCH = 303,
    VIDEO_MEETING_END,
    END,
};

/**
 * @brief Enumerates picture in picture state.
 */
enum class PiPState : int32_t {
    ABOUT_TO_START = 1,
    STARTED = 2,
    ABOUT_TO_STOP = 3,
    STOPPED = 4,
    ABOUT_TO_RESTORE = 5,
    ERROR = 6,
};

enum class CaseType {
    CASE_WINDOW_MANAGER = 0,
    CASE_WINDOW,
    CASE_STAGE
};

/**
 * maximize layout show type
 */
enum ShowType : int32_t {
    SHOW, // normally show
    HIDE, // show when hover, but hide normally
    FORBIDDEN // hide always
};

}
}
#endif // OHOS_ROSEN_WM_COMMON_ENUM_H
