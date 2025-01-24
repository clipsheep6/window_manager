/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

/**
 * @file oh_window.h
 *
 * @brief Declares APIs for window
 *
 * @syscap SystemCapability.Window.SessionManager
 * @library libnative_window_manager.so
 * @kit ArkUI
 * @since 16
 */
#ifndef OH_WINDOW_H
#define OH_WINDOW_H

#include <stdint.h>
#include "oh_window_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks whether the window is displayed.
 *
 * @param windowId WindowId when window is created.
 * @param isShow Whether the window is displayed. The value true means that the window is displayed,
          and false means the opposite.
 * @return Returns the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INVALID_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL} this window state is abnormal.
 * @since 16
 */
int32_t OH_WindowManager_IsWindowShown(int32_t windowId, bool* isShow);
 
/**
 * @brief Show window.
 *
 * @param windowId WindowId when window is created.
 * @return Returns the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INVALID_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL} this window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMAL} the window manager service works abnormally.
 * @since 16
 */
int32_t OH_WindowManager_ShowWindow(int32_t windowId);

#ifdef __cplusplus
}
#endif
#endif  // OH_WINDOW_H