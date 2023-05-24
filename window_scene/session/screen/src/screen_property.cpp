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

#include "session/screen/include/screen_property.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr int32_t PHONE_SCREEN_WIDTH = 1344;
    constexpr int32_t PHONE_SCREEN_HEIGHT = 2772;
    constexpr float PHONE_SCREEN_DENSITY = 3.5f;
    constexpr float ELSE_SCREEN_DENSITY = 1.5f;
    constexpr float INCH_2_MM = 25.4f;
}

constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "Screen_property" };

void ScreenProperty::SetRotation(Rotation rotation)
{
    rotation_ = rotation;
}

Rotation ScreenProperty::GetRotation() const
{
    return rotation_;
}

void ScreenProperty::SetBounds(const RRect& bounds)
{
    bounds_ = bounds;
}

RRect ScreenProperty::GetBounds() const
{
    return bounds_;
}

float ScreenProperty::GetDensity()
{
    int32_t width = bounds_.rect_.width_;
    int32_t height = bounds_.rect_.height_;

    if (width == PHONE_SCREEN_WIDTH && height == PHONE_SCREEN_HEIGHT) {
        density_ = PHONE_SCREEN_DENSITY;
    } else {
        density_ = ELSE_SCREEN_DENSITY;
    }
    return density_;
}

void ScreenProperty::UpdateXDpi()
{
    if (phyWidth_ != UINT32_MAX) {
        int32_t width = bounds_.rect_.width_;
        xDpi_ = width * INCH_2_MM / phyWidth_;
    }
}

void ScreenProperty::UpdateYDpi()
{
    if (phyHeight_ != UINT32_MAX) {
        int32_t height = bounds_.rect_.height_;
        yDpi_ = height * INCH_2_MM / phyHeight_;
    }
}

uint32_t ScreenProperty::GetRefreshRate() const
{
    return refreshRate_;
}

float ScreenProperty::GetVirtualPixelRatio() const
{
    return virtualPixelRatio_;
}

void ScreenProperty::SetRefreshRate(uint32_t refreshRate)
{
    refreshRate_ = refreshRate;
}

void ScreenProperty::SetVirtualPixelRatio(float virtualPixelRatio)
{
    virtualPixelRatio_ = virtualPixelRatio;
    WLOGE(" virtualPixelRatio_ value is  %{public}f",
       virtualPixelRatio_);
}

void ScreenProperty::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
}

Rotation ScreenProperty::GetRotation() const
{
    return rotation_;
}

Orientation ScreenProperty::GetOrientation() const
{
    return orientation_;
}

} // namespace OHOS::Rosen
