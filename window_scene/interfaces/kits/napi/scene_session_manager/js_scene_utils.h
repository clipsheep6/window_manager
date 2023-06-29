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

#ifndef OHOS_WINDOW_SCENE_JS_SCENE_UTILS_H
#define OHOS_WINDOW_SCENE_JS_SCENE_UTILS_H

#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "interfaces/include/ws_common.h"
#include "wm_common.h"

namespace OHOS::Rosen {
enum class JsSessionType : uint32_t {
    TYPE_UNDEFINED = 0,
    TYPE_APP,
    TYPE_SUB_APP,
    TYPE_SYSTEM_ALERT,
    TYPE_INPUT_METHOD,
    TYPE_STATUS_BAR,
    TYPE_PANEL,
    TYPE_KEYGUARD,
    TYPE_VOLUME_OVERLAY,
    TYPE_NAVIGATION_BAR,
    TYPE_FLOAT,
    TYPE_WALLPAPER,
    TYPE_DESKTOP,
    TYPE_LAUNCHER_DOCK,
    TYPE_FLOAT_CAMERA,
    TYPE_DIALOG,
    TYPE_SCREENSHOT,
    TYPE_TOAST
};

const std::map<WindowType, JsSessionType> WINDOW_TO_JS_SESSION_TYPE_MAP {
    { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,     JsSessionType::TYPE_APP               },
    { WindowType::WINDOW_TYPE_APP_SUB_WINDOW,      JsSessionType::TYPE_SUB_APP           },
    { WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, JsSessionType::TYPE_SYSTEM_ALERT      },
    { WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,  JsSessionType::TYPE_INPUT_METHOD      },
    { WindowType::WINDOW_TYPE_STATUS_BAR,          JsSessionType::TYPE_STATUS_BAR        },
    { WindowType::WINDOW_TYPE_PANEL,               JsSessionType::TYPE_PANEL             },
    { WindowType::WINDOW_TYPE_KEYGUARD,            JsSessionType::TYPE_KEYGUARD          },
    { WindowType::WINDOW_TYPE_VOLUME_OVERLAY,      JsSessionType::TYPE_VOLUME_OVERLAY    },
    { WindowType::WINDOW_TYPE_NAVIGATION_BAR,      JsSessionType::TYPE_NAVIGATION_BAR    },
    { WindowType::WINDOW_TYPE_FLOAT,               JsSessionType::TYPE_FLOAT             },
    { WindowType::WINDOW_TYPE_WALLPAPER,           JsSessionType::TYPE_WALLPAPER         },
    { WindowType::WINDOW_TYPE_DESKTOP,             JsSessionType::TYPE_DESKTOP           },
    { WindowType::WINDOW_TYPE_LAUNCHER_DOCK,       JsSessionType::TYPE_LAUNCHER_DOCK     },
    { WindowType::WINDOW_TYPE_FLOAT_CAMERA,        JsSessionType::TYPE_FLOAT_CAMERA      },
    { WindowType::WINDOW_TYPE_DIALOG,              JsSessionType::TYPE_DIALOG            },
    { WindowType::WINDOW_TYPE_SCREENSHOT,          JsSessionType::TYPE_SCREENSHOT        },
    { WindowType::WINDOW_TYPE_TOAST,               JsSessionType::TYPE_TOAST             }
};

const std::map<JsSessionType, WindowType> JS_SESSION_TO_WINDOW_TYPE_MAP {
    { JsSessionType::TYPE_APP,               WindowType::WINDOW_TYPE_APP_MAIN_WINDOW     },
    { JsSessionType::TYPE_SUB_APP,           WindowType::WINDOW_TYPE_APP_SUB_WINDOW      },
    { JsSessionType::TYPE_SYSTEM_ALERT,      WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW },
    { JsSessionType::TYPE_INPUT_METHOD,      WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT  },
    { JsSessionType::TYPE_STATUS_BAR,        WindowType::WINDOW_TYPE_STATUS_BAR          },
    { JsSessionType::TYPE_PANEL,             WindowType::WINDOW_TYPE_PANEL               },
    { JsSessionType::TYPE_KEYGUARD,          WindowType::WINDOW_TYPE_KEYGUARD            },
    { JsSessionType::TYPE_VOLUME_OVERLAY,    WindowType::WINDOW_TYPE_VOLUME_OVERLAY      },
    { JsSessionType::TYPE_NAVIGATION_BAR,    WindowType::WINDOW_TYPE_NAVIGATION_BAR      },
    { JsSessionType::TYPE_FLOAT,             WindowType::WINDOW_TYPE_FLOAT               },
    { JsSessionType::TYPE_WALLPAPER,         WindowType::WINDOW_TYPE_WALLPAPER           },
    { JsSessionType::TYPE_DESKTOP,           WindowType::WINDOW_TYPE_DESKTOP             },
    { JsSessionType::TYPE_LAUNCHER_DOCK,     WindowType::WINDOW_TYPE_LAUNCHER_DOCK       },
    { JsSessionType::TYPE_FLOAT_CAMERA,      WindowType::WINDOW_TYPE_FLOAT_CAMERA        },
    { JsSessionType::TYPE_DIALOG,            WindowType::WINDOW_TYPE_DIALOG              },
    { JsSessionType::TYPE_SCREENSHOT,        WindowType::WINDOW_TYPE_SCREENSHOT          },
    { JsSessionType::TYPE_TOAST,             WindowType::WINDOW_TYPE_TOAST               }
};
bool ConvertSessionInfoFromJs(NativeEngine& engine, NativeObject* jsObject, SessionInfo& sessionInfo);
NativeValue* CreateJsSessionInfo(NativeEngine& engine, const SessionInfo& sessionInfo);
NativeValue* CreateJsSessionState(NativeEngine& engine);
NativeValue* CreateJsSessionRect(NativeEngine& engine, const WSRect& rect);
NativeValue* CreateJsSystemBarPropertyArrayObject(
    NativeEngine& engine, const std::unordered_map<WindowType, SystemBarProperty>& propertyMap);
NativeValue* SessionTypeInit(NativeEngine* engine);
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCENE_UTILS_H
