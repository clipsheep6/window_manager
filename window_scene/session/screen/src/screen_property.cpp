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

namespace OHOS::Rosen {
namespace {
constexpr int32_t PHONE_SCREEN_WIDTH = 1344;
constexpr int32_t PHONE_SCREEN_HEIGHT = 2772;
constexpr float PHONE_SCREEN_DENSITY = 3.5f;
constexpr float ELSE_SCREEN_DENSITY = 1.5f;
constexpr float INCH_2_MM = 25.4f;
}

void ScreenProperty::SetRotation(float rotation)
{
    rotation_ = rotation;
}

float ScreenProperty::GetRotation() const
{
    return rotation_;
}

void ScreenProperty::SetBounds(const RRect& bounds)
{
    bounds_ = bounds;
    UpdateXDpi();
    UpdateYDpi();
}

RRect ScreenProperty::GetBounds() const
{
    return bounds_;
}

float ScreenProperty::GetDensity()
{
    int32_t width = bounds_.rect_.width_;
    int32_t height = bounds_.rect_.height_;

    if (width == PHONE_SCREEN_WIDTH && height == PHONE_SCREEN_HEIGHT) { // telephone
        density_ = PHONE_SCREEN_DENSITY;
    } else {
        density_ = ELSE_SCREEN_DENSITY;
    }
    return density_;
}

void ScreenProperty::SetPhyWidth(uint32_t phyWidth)
{
    phyWidth_ = phyWidth;
}

int32_t ScreenProperty::GetPhyWidth() const
{
    return phyWidth_;
}

void ScreenProperty::SetPhyHeight(uint32_t phyHeight)
{
    phyHeight_ = phyHeight;
}

int32_t ScreenProperty::GetPhyHeight() const
{
    return phyHeight_;
}

void ScreenProperty::SetRefreshRate(uint32_t refreshRate)
{
    refreshRate_ = refreshRate;
}

uint32_t ScreenProperty::GetRefreshRate() const
{
    return refreshRate_;
}

void ScreenProperty::SetVirtualPixelRatio(float virtualPixelRatio)
{
    virtualPixelRatio_ = virtualPixelRatio;
}

float ScreenProperty::GetVirtualPixelRatio() const
{
    return virtualPixelRatio_;
}

Rotation ScreenProperty::GetScreenRotation() const
{
    return screenRotation_;
}

void ScreenProperty::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
}

Orientation ScreenProperty::GetOrientation() const
{
    return orientation_;
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
        int32_t height_ = bounds_.rect_.height_;
        yDpi_ = height_ * INCH_2_MM / phyHeight_;
    }
}

void ScreenProperty::CalculateXYDpi(uint32_t phyWidth, uint32_t phyHeight)
{
    if (phyWidth == 0 || phyHeight == 0) {
        return;
    }

    phyWidth_ = phyWidth;
    phyHeight_ = phyHeight;
    int32_t width_ = bounds_.rect_.width_;
    int32_t height_ = bounds_.rect_.height_;
    xDpi_ = width_ * INCH_2_MM / phyWidth_;
    yDpi_ = height_ * INCH_2_MM / phyHeight_;
}

float ScreenProperty::GetXDpi()
{
    return xDpi_;
}

float ScreenProperty::GetYDpi()
{
    return yDpi_;
}

void ScreenProperty::SetOffsetX(int32_t offsetX)
{
    offsetX_ = offsetX;
}

int32_t ScreenProperty::GetOffsetX() const
{
    return offsetX_;
}

void ScreenProperty::SetOffsetY(int32_t offsetY)
{
    offsetY_ = offsetY;
}

int32_t ScreenProperty::GetOffsetY() const
{
    return offsetY_;
}

void ScreenProperty::SetOffset(int32_t offsetX, int32_t offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
}

void ScreenProperty::SetScreenType(ScreenType type)
{
    type_ = type;
}

ScreenType ScreenProperty::GetScreenType() const
{
    return type_;
}

} // namespace OHOS::Rosen
