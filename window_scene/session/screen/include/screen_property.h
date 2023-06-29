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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H

#include "common/rs_rect.h"
#include "dm_common.h"
#include "class_var_definition.h"
#include "screen_info.h"

namespace OHOS::Rosen {
class ScreenProperty {
public:
    ScreenProperty() = default;
    ~ScreenProperty() = default;

    void SetRotation(float rotation);
    float GetRotation() const;

    void SetBounds(const RRect& bounds);
    RRect GetBounds() const;

    float GetDensity();

    void SetPhyWidth(uint32_t phyWidth);
    int32_t GetPhyWidth() const;

    void SetPhyHeight(uint32_t phyHeight);
    int32_t GetPhyHeight() const;

    void SetRefreshRate(uint32_t refreshRate);
    uint32_t GetRefreshRate() const;

    void UpdateVirtualPixelRatio(const RRect& bounds);
    void SetVirtualPixelRatio(float virtualPixelRatio);
    float GetVirtualPixelRatio() const;

    Rotation GetScreenRotation() const;

    void SetOrientation(Orientation orientation);
    Orientation GetOrientation() const;

    void SetDisplayOrientation(DisplayOrientation displayOrientation);
    DisplayOrientation GetDisplayOrientation() const;

    float GetXDpi();
    float GetYDpi();

    void SetOffsetX(int32_t offsetX);
    int32_t GetOffsetX() const;
    
    void SetOffsetY(int32_t offsetY);
    int32_t GetOffsetY() const;

    void SetOffset(int32_t offsetX, int32_t offsetY);

    void SetScreenType(ScreenType type);
    ScreenType GetScreenType() const;

private:
    float rotation_ { 0.0f };
    RRect bounds_;

    uint32_t phyWidth_ { UINT32_MAX };
    uint32_t phyHeight_ { UINT32_MAX };

    uint32_t refreshRate_ { 0 };
    float virtualPixelRatio_ { 1.0f };

    Orientation orientation_ { Orientation::UNSPECIFIED };
    DisplayOrientation displayOrientation_ { DisplayOrientation::UNKNOWN };
    Rotation screenRotation_ { Rotation::ROTATION_0 };

    float xDpi_ { 0.0f };
    float yDpi_ { 0.0f };

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };

    ScreenType type_ { ScreenType::REAL };

    void UpdateXDpi();
    void UpdateYDpi();
    void UpdateDisplayOrientation();
    void CalculateXYDpi(uint32_t phyWidth, uint32_t phyHeight);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H
