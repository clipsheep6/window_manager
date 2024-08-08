/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_ACCESSIBILITY_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_ACCESSIBILITY_CONTROLLER_H

#include <cstdint>
#include "wm_single_instance.h"

namespace OHOS::Rosen {
/**
 * @class WindowAccessibilityController
 *
 * @brief Controller of window accessibility.
 */
class WindowAccessibilityController {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowAccessibilityController);
public:
    /**
     * @brief Set anchor and scale, only take effect for the main display.
     *
     * @param x Point x coordinate of Anchor.
     * @param y Point y coordinate of Anchor.
     * @param scale Parameter of zooming.
     */
    void SetAnchorAndScale(int32_t x, int32_t y, float scale);
    /**
     * @brief Set anchor offset, only take effect for the main display.
     *
     * @param deltaX X coordinate offset of anchor.
     * @param deltaY Y coordinate offset of anchor.
     */
    void SetAnchorOffset(int32_t deltaX, int32_t deltaY);
    /**
     * @brief Turn off the func of window zoom.
     */
    void OffWindowZoom();
    
private:
    WindowAccessibilityController();
    ~WindowAccessibilityController() = default;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_ACCESSIBILITY_CONTROLLER_H