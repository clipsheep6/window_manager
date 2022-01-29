/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {
enum class PowerStateChangeReason : uint32_t {
    POWER_BUTTON,
};

enum class DisplayPowerState : uint32_t {
    POWER_ON,
    POWER_STAND_BY,
    POWER_SUSPEND,
    POWER_OFF,
    POWER_BUTT,
    INVALID_STATE,
};

enum class DisplayState : uint32_t {
    ON,
    OFF,
    UNKNOWN,
};

enum class DisplayEvent : uint32_t {
    UNLOCK,
    KEYGUARD_DRAWN,
};

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
    DM_ERROR_UNKNOWN,
};
using DisplayStateCallback = std::function<void(DisplayState)>;

enum class DisplayPowerEvent : uint32_t {
    WAKE_UP,
    SLEEP,
    DISPLAY_ON,
    DISPLAY_OFF,
    DESKTOP_READY,
};

enum class EventStatus : uint32_t {
    BEGIN,
    END,
};

class IDisplayPowerEventListener : public RefBase {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) = 0;
};

enum class ScreenChangeEvent : uint32_t {
    ADD_TO_GROUP,
    REMOVE_FROM_GROUP,
    CHANGE_GROUP,
};

enum class Rotation : uint32_t {
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
};

enum class DisplayChangeEvent : uint32_t {
    DISPLAY_SIZE_CHANGED,
};
}
}
#endif // OHOS_ROSEN_DM_COMMON_H