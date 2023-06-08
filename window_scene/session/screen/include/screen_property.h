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

namespace OHOS::Rosen {
class ScreenProperty {
public:
    ScreenProperty() = default;
    ~ScreenProperty() = default;

    RRect GetBounds() const;
    int32_t GetPhyWidth() const;
    int32_t GetPhyHeight() const;
    uint32_t GetRefreshRate() const;
    
    float GetVirtualPixelRatio() const;
    Orientation GetOrientation() const;
    float GetRotation() const;
    float GetDensity();

    void SetBounds(const RRect& bounds);
    void SetPhyWidth(uint32_t phyWidth);
    void SetPhyHeight(uint32_t phyHeight);
    void SetRefreshRate(uint32_t refreshRate);
    void SetVirtualPixelRatio(float virtualPixelRatio);
    void SetOrientation(Orientation orientation);
    void SetRotation(Rotation rotation);

    void UpdateXDpi();
    void UpdateYDpi();

private:
    RRect bounds_;
    uint32_t phyWidth_ { UINT32_MAX };
    uint32_t phyHeight_ { UINT32_MAX };
    uint32_t refreshRate_ { 0 };
    float virtualPixelRatio_ { 1.0f };
    Orientation orientation_ { Orientation::UNSPECIFIED };
    Rotation rotation_ { Rotation::ROTATION_0 };
    float density_ { 0.0f }; 
    float xDpi_ { 0.0f };
    float yDpi_ { 0.0f };
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H
