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
#include "native_window_event_filter.h"

#include "window.h"
#include "native_window_comm.h"
#include "cstdint"
#include "functional"
#include "window_manager_hilog.h"
#include "key_event.h"
#include "wm_common.h"

using namespace OHOS::Rosen;

KeyEventFilterFunc convert2Func(KeyEventFilter filter)
{
    std::function<bool(OHOS::MMI::KeyEvent&)> func = [filter](OHOS::MMI::KeyEvent& keyEvent) {
        OH_WMKeyEvent* windowEvent = new OH_WMKeyEvent();
        windowEvent->action = (OH_WMKeyAction) keyEvent.GetKeyAction();
        windowEvent->code = (OH_WMKeyCode) keyEvent.GetKeyCode();
        windowEvent->timestamp = keyEvent.GetActionTime();
        auto pressedKeys = keyEvent.GetPressedKeys();
        windowEvent->pressedKeySize = pressedKeys.size();
        if (!pressedKeys.empty()) {
            windowEvent->pressedKeys = new OH_WMKeyCode[pressedKeys.size()];
            int32_t i = 0;
            for (auto key : pressedKeys) {
                windowEvent->pressedKeys[i++] = (OH_WMKeyCode) key;
            }
        }
        return filter(windowEvent);
    };
    return func ;
}

OH_WMErrCode OH_NativeWindowManager_RegisterKeyEventFilter(int32_t windowId, KeyEventFilter filter)
{
    WLOG_I("[WMNDK] register keyEventCallback,windowId:%{public}d", windowId);
    auto mainWindow = OHOS::Rosen::Window::GetMainWindowWithId(windowId);
    if (mainWindow == nullptr) {
        WLOG_E("[WMNDK]mainWindow is null,windowId:%{public}d", windowId);
        return OH_WMErrCode::WS_INVAILD_WINID;
    }
    auto res = mainWindow->SetKeyEventFilter(convert2Func(filter));

    return res == WMError::WM_OK? OH_WMErrCode::WS_OK:OH_WMErrCode::WS_ERR;
}

OH_WMErrCode OH_NativeWindowManager_UnRegisterKeyEventFilter(int32_t windowId)
{
    WLOG_I("[WMNDK] clear keyEventCallback,windowId:%{public}d", windowId);
    auto mainWindow = OHOS::Rosen::Window::GetMainWindowWithId(windowId);
    if (mainWindow == nullptr) {
        WLOG_E("[WMNDK] mainWindow is null,windowId:%{public}d", windowId);
        return OH_WMErrCode::WS_INVAILD_WINID;
    }
    auto res = mainWindow->ClearKeyEventFilter();
    return res == WMError::WM_OK? OH_WMErrCode::WS_OK:OH_WMErrCode::WS_ERR;
}

// #endif // INCLUDE_NATIVE_WINDOW_EVENT_FILTER_H