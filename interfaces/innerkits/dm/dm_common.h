/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DM_COMMON_H
#define OHOS_ROSEN_DM_COMMON_H

#include <refbase.h>
#include <string>
#include <map>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;
using ScreenId = uint64_t;

namespace {
constexpr DisplayId DISPLAY_ID_INVALID = -1ULL;
constexpr ScreenId SCREEN_ID_INVALID = -1ULL;
constexpr ScreenId SCREEN_ID_FAKE = 999;
constexpr DisplayId DISPLAY_ID_FAKE = 999;
constexpr ScreenId ERROR_ID_NOT_SYSTEM_APP = -202ULL;
constexpr int DOT_PER_INCH = 160;
const static std::string DEFAULT_SCREEN_NAME = "buildIn";
constexpr int DOT_PER_INCH_MAXIMUM_VALUE = 1000;
constexpr int DOT_PER_INCH_MINIMUM_VALUE = 80;
constexpr uint32_t BASELINE_DENSITY = 160;
constexpr uint32_t HALF_SCREEN_PARAM = 2;
}

/**
 * @struct HookInfo.
 *
 * @brief hook diaplayinfo deepending on the window size.
 */
struct DMHookInfo {
    uint32_t width_;
    uint32_t height_;
    float_t density_;
    uint32_t rotation_;
    bool enableHookRotation_;
};

/**
 * @brief Power state change reason.
 */
enum class PowerStateChangeReason : uint32_t {
    POWER_BUTTON = 0,
    STATE_CHANGE_REASON_INIT = 0,
    STATE_CHANGE_REASON_TIMEOUT = 1,
    STATE_CHANGE_REASON_RUNNING_LOCK = 2,
    STATE_CHANGE_REASON_BATTERY = 3,
    STATE_CHANGE_REASON_THERMAL = 4,
    STATE_CHANGE_REASON_WORK = 5,
    STATE_CHANGE_REASON_SYSTEM = 6,
    STATE_CHANGE_REASON_APPLICATION = 10,
    STATE_CHANGE_REASON_SETTINGS = 11,
    STATE_CHANGE_REASON_HARD_KEY = 12,
    STATE_CHANGE_REASON_TOUCH = 13,
    STATE_CHANGE_REASON_CABLE = 14,
    STATE_CHANGE_REASON_SENSOR = 15,
    STATE_CHANGE_REASON_LID = 16,
    STATE_CHANGE_REASON_CAMERA = 17,
    STATE_CHANGE_REASON_ACCESS = 18,
    STATE_CHANGE_REASON_RESET = 19,
    STATE_CHANGE_REASON_POWER_KEY = 20,
    STATE_CHANGE_REASON_KEYBOARD = 21,
    STATE_CHANGE_REASON_MOUSE = 22,
    STATE_CHANGE_REASON_DOUBLE_CLICK = 23,
    STATE_CHANGE_REASON_COLLABORATION = 24,
    STATE_CHANGE_REASON_SWITCH = 25,
    STATE_CHANGE_REASON_PRE_BRIGHT = 26,
    STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_SUCCESS = 27,
    STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_ON = 28,
    STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF = 29,
    STATE_CHANGE_REASON_DISPLAY_SWITCH = 30,
    STATE_CHANGE_REASON_PROXIMITY = 32,
    STATE_CHANGE_REASON_AOD_SLIDING = 40,
    STATE_CHANGE_REASON_PEN = 41,
    STATE_CHANGE_REASON_SHUT_DOWN = 42,
    STATE_CHANGE_REASON_SCREEN_CONNECT = 43,
    STATE_CHANGE_REASON_REMOTE = 100,
    STATE_CHANGE_REASON_UNKNOWN = 1000,
};

/**
 * @brief Enumerates the state of the screen power.
 */
enum class ScreenPowerState : uint32_t {
    POWER_ON,
    POWER_STAND_BY,
    POWER_SUSPEND,
    POWER_OFF,
    POWER_BUTT,
    INVALID_STATE,
    POWER_DOZE,
    POWER_DOZE_SUSPEND,
};

enum class ScreenPropertyChangeType : uint32_t {
    UNSPECIFIED = 0,
    /* Screen connection. */
    ROTATION_BEGIN,
    /* Screen disconnection. */
    ROTATION_END,
    /* Only update screen rotation property info to DMS. */
    ROTATION_UPDATE_PROPERTY_ONLY,
};

/**
 * @brief Enumerates screen shape.
 */
enum class ScreenShape : uint32_t {
    RECTANGLE = 0,
    ROUND = 1,
};

/**
 * @brief Enumerates the state of the display.
 */
enum class DisplayState : uint32_t {
    UNKNOWN,
    OFF,
    ON,
    DOZE,
    DOZE_SUSPEND,
    VR,
    ON_SUSPEND,
};

/**
 * @brief Enumerates display events.
 */
enum class DisplayEvent : uint32_t {
    UNLOCK,
    KEYGUARD_DRAWN,
    SCREEN_LOCK_SUSPEND,
    SCREEN_LOCK_OFF,
    SCREEN_LOCK_FINGERPRINT,
    SCREEN_LOCK_DOZE_FINISH,
};

/**
 * @brief Enumerates DMError.
 */
enum class DMError : int32_t {
    DM_OK = 0,
    DM_ERROR_INIT_DMS_PROXY_LOCKED = 100,
    DM_ERROR_IPC_FAILED = 101,
    DM_ERROR_REMOTE_CREATE_FAILED = 110,
    DM_ERROR_NULLPTR = 120,
    DM_ERROR_INVALID_PARAM = 130,
    DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED = 140,
    DM_ERROR_DEATH_RECIPIENT = 150,
    DM_ERROR_INVALID_MODE_ID = 160,
    DM_ERROR_WRITE_DATA_FAILED = 170,
    DM_ERROR_RENDER_SERVICE_FAILED = 180,
    DM_ERROR_UNREGISTER_AGENT_FAILED = 190,
    DM_ERROR_INVALID_CALLING = 200,
    DM_ERROR_INVALID_PERMISSION = 201,
    DM_ERROR_NOT_SYSTEM_APP = 202,
    DM_ERROR_DEVICE_NOT_SUPPORT = 801,
    DM_ERROR_UNKNOWN = -1,
};

/**
 * @brief Enumerates DM error codes.
 */
enum class DmErrorCode : int32_t {
    DM_OK = 0,
    DM_ERROR_NO_PERMISSION = 201,
    DM_ERROR_NOT_SYSTEM_APP = 202,
    DM_ERROR_INVALID_PARAM = 401,
    DM_ERROR_DEVICE_NOT_SUPPORT = 801,
    DM_ERROR_INVALID_SCREEN = 1400001,
    DM_ERROR_INVALID_CALLING = 1400002,
    DM_ERROR_SYSTEM_INNORMAL = 1400003,
};

/**
 * @brief Constructs the mapping of the DM errors to the DM error codes.
 */
const std::map<DMError, DmErrorCode> DM_JS_TO_ERROR_CODE_MAP {
    {DMError::DM_OK,                                    DmErrorCode::DM_OK                          },
    {DMError::DM_ERROR_INVALID_PERMISSION,              DmErrorCode::DM_ERROR_NO_PERMISSION         },
    {DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED,           DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_IPC_FAILED,                      DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_REMOTE_CREATE_FAILED,            DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_NULLPTR,                         DmErrorCode::DM_ERROR_INVALID_SCREEN        },
    {DMError::DM_ERROR_INVALID_PARAM,                   DmErrorCode::DM_ERROR_INVALID_PARAM         },
    {DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED,    DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_DEATH_RECIPIENT,                 DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_INVALID_MODE_ID,                 DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_WRITE_DATA_FAILED,               DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_RENDER_SERVICE_FAILED,           DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_UNREGISTER_AGENT_FAILED,         DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_INVALID_CALLING,                 DmErrorCode::DM_ERROR_INVALID_CALLING       },
    {DMError::DM_ERROR_NOT_SYSTEM_APP,                  DmErrorCode::DM_ERROR_NOT_SYSTEM_APP        },
    {DMError::DM_ERROR_UNKNOWN,                         DmErrorCode::DM_ERROR_SYSTEM_INNORMAL       },
    {DMError::DM_ERROR_DEVICE_NOT_SUPPORT,              DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT    },
};

using DisplayStateCallback = std::function<void(DisplayState)>;

/**
 * @brief Enumerates display power events.
 */
enum class DisplayPowerEvent : uint32_t {
    WAKE_UP,
    SLEEP,
    DISPLAY_ON,
    DISPLAY_OFF,
    DESKTOP_READY,
    DISPLAY_DOZE,
    DISPLAY_DOZE_SUSPEND,
    DISPLAY_OFF_CANCELED,
};

/**
 * @brief Enumerates event status.
 */
enum class EventStatus : uint32_t {
    BEGIN,
    END,
};

enum class VirtualScreenFlag : uint32_t {
    DEFAULT = 0,
    CAST = 1,
    MAX = 2,
};

class IDisplayPowerEventListener : public RefBase {
public:
    /**
     * @brief Notify when display power event status changed.
     */
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) = 0;
};

/**
 * @brief Enumerates screen change events.
 */
enum class ScreenChangeEvent : uint32_t {
    UPDATE_ORIENTATION,
    UPDATE_ROTATION,
    CHANGE_MODE,
    VIRTUAL_PIXEL_RATIO_CHANGED,
    SCREEN_SWITCH_CHANGE,
    UNKNOWN,
};

/**
 * @brief Enumerates screen group change events.
 */
enum class ScreenGroupChangeEvent : uint32_t {
    ADD_TO_GROUP,
    REMOVE_FROM_GROUP,
    CHANGE_GROUP,
};

/**
 * @brief Enumerates rotations.
 */
enum class Rotation : uint32_t {
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
};

/**
 * @brief Enumerates orientations.
 */
enum class Orientation : uint32_t {
    BEGIN = 0,
    UNSPECIFIED = BEGIN,
    VERTICAL = 1,
    HORIZONTAL = 2,
    REVERSE_VERTICAL = 3,
    REVERSE_HORIZONTAL = 4,
    SENSOR = 5,
    SENSOR_VERTICAL = 6,
    SENSOR_HORIZONTAL = 7,
    AUTO_ROTATION_RESTRICTED = 8,
    AUTO_ROTATION_PORTRAIT_RESTRICTED = 9,
    AUTO_ROTATION_LANDSCAPE_RESTRICTED = 10,
    LOCKED = 11,
    FOLLOW_RECENT = 12,
    AUTO_ROTATION_UNSPECIFIED = 13,
    USER_ROTATION_PORTRAIT = 14,
    USER_ROTATION_LANDSCAPE = 15,
    USER_ROTATION_PORTRAIT_INVERTED = 16,
    USER_ROTATION_LANDSCAPE_INVERTED = 17,
    FOLLOW_DESKTOP = 18,
    END = FOLLOW_DESKTOP,
};

/**
 * @brief Enumerates display orientations.
 */
enum class DisplayOrientation : uint32_t {
    PORTRAIT = 0,
    LANDSCAPE,
    PORTRAIT_INVERTED,
    LANDSCAPE_INVERTED,
    UNKNOWN,
};

/**
 * @brief Enumerates display change events.
 */
enum class DisplayChangeEvent : uint32_t {
    UPDATE_ORIENTATION,
    UPDATE_ROTATION,
    DISPLAY_SIZE_CHANGED,
    DISPLAY_FREEZED,
    DISPLAY_UNFREEZED,
    DISPLAY_VIRTUAL_PIXEL_RATIO_CHANGED,
    UPDATE_ORIENTATION_FROM_WINDOW,
    UPDATE_ROTATION_FROM_WINDOW,
    UPDATE_REFRESHRATE,
    SUPER_FOLD_RESOLUTION_CHANGED,
    UNKNOWN,
};

/**
 * @brief Enumerates display state change type.
 */
enum class DisplayStateChangeType : uint32_t {
    BEFORE_SUSPEND,
    BEFORE_UNLOCK,
    UPDATE_ROTATION,
    UPDATE_ROTATION_FROM_WINDOW,
    SIZE_CHANGE,
    CREATE,
    DESTROY,
    FREEZE,
    UNFREEZE,
    VIRTUAL_PIXEL_RATIO_CHANGE,
    DISPLAY_COMPRESS,
    UPDATE_SCALE,
    UNKNOWN,
};

/**
 * @brief Enumerates screen source mode.
 */
enum class ScreenSourceMode: uint32_t {
    SCREEN_MAIN = 0,
    SCREEN_MIRROR = 1,
    SCREEN_EXTEND = 2,
    SCREEN_ALONE = 3,
    SCREEN_UNIQUE = 4,
};

/**
 * @brief Enumerates the fold status.
 */
enum class FoldStatus: uint32_t {
    UNKNOWN = 0,
    EXPAND = 1,
    FOLDED = 2,
    HALF_FOLD = 3,
    FOLD_STATE_EXPAND_WITH_SECOND_EXPAND = 11,
    FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED = 21,
    FOLD_STATE_FOLDED_WITH_SECOND_EXPAND = 12,
    FOLD_STATE_FOLDED_WITH_SECOND_HALF_FOLDED = 22,
    FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND = 13,
    FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED = 23,
};

/**
 * @brief Enumerates the super fold state change events.
 */
enum class SuperFoldStatusChangeEvents : uint32_t {
    UNDEFINED = 0,
    ANGLE_CHANGE_EXPANDED,
    ANGLE_CHANGE_HALF_FOLDED,
    ANGLE_CHANGE_FOLDED,
    KEYBOARD_ON,
    KEYBOARD_OFF,
    INVALID,
};

/**
 * @brief Enumerates the super fold state.
 */
enum class SuperFoldStatus : uint32_t {
    UNKNOWN,
    FOLDED,
    HALF_FOLDED,
    EXPANDED,
    KEYBOARD,
};

/**
 * @brief Enumerates the fold display mode.
 */
enum class FoldDisplayMode: uint32_t {
    UNKNOWN = 0,
    FULL = 1,
    MAIN = 2,
    SUB = 3,
    COORDINATION = 4,
    GLOBAL_FULL = 5,
};

enum class DisplayType : uint32_t {
    DEFAULT = 0,
};

enum class ScreenCombination : uint32_t {
    SCREEN_ALONE,
    SCREEN_EXPAND,
    SCREEN_MIRROR,
    SCREEN_UNIQUE,
    SCREEN_EXTEND,
    SCREEN_MAIN,
};

enum class MultiScreenMode : uint32_t {
    SCREEN_MIRROR = 0,
    SCREEN_EXTEND = 1,
};

struct Point {
    int32_t posX_;
    int32_t posY_;
    Point() : posX_(0), posY_(0) {};
    Point(int32_t posX, int32_t posY) : posX_(posX), posY_(posY) {};
};

struct SupportedScreenModes : public RefBase {
    uint32_t id_;
    uint32_t width_;
    uint32_t height_;
    uint32_t refreshRate_;
};

struct CaptureOption {
    DisplayId displayId_ = DISPLAY_ID_INVALID;
    bool isNeedNotify_ = true;
    bool isNeedPointer_ = true;
};

struct ExpandOption {
    ScreenId screenId_;
    uint32_t startX_;
    uint32_t startY_;
};

struct MultiScreenRecoverOption {
    ScreenId screenId_;
    uint32_t first_;
    uint32_t second_;
};

struct MultiScreenPositionOptions {
    ScreenId screenId_;
    uint32_t startX_;
    uint32_t startY_;
};

/**
 * @brief fold display physical resolution
 */
struct DisplayPhysicalResolution {
    FoldDisplayMode foldDisplayMode_;
    uint32_t physicalWidth_;
    uint32_t physicalHeight_;
};

/**
 * @brief scrollable param
 */
struct ScrollableParam {
    std::string velocityScale_;
    std::string friction_;
};

/**
 * @brief screen direction info
 */
struct ScreenDirectionInfo {
    int32_t notifyRotation_;
    int32_t screenRotation_;
    int32_t rotation_;
    int32_t phyRotation_;
};

/**
 * @brief displayRect
 */
struct DMRect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;

    bool operator==(const DMRect& a) const
    {
        return (posX_ == a.posX_ && posY_ == a.posY_ && width_ == a.width_ && height_ == a.height_);
    }

    bool operator!=(const DMRect& a) const
    {
        return !this->operator==(a);
    }

    bool IsUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const DMRect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }
    static DMRect NONE()
    {
        return {0, 0, 0, 0};
    }
};
}
}
#endif // OHOS_ROSEN_DM_COMMON_H
