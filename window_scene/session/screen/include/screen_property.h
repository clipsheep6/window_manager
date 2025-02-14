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

enum class ScreenPropertyChangeReason : uint32_t {
    UNDEFINED = 0,
    ROTATION,
    CHANGE_MODE,
    FOLD_SCREEN_EXPAND,
    SCREEN_CONNECT,
    SCREEN_DISCONNECT,
    FOLD_SCREEN_FOLDING,
    VIRTUAL_SCREEN_RESIZE,
    RELATIVE_POSITION_CHANGE,
    SUPER_FOLD_STATUS_CHANGE,
    VIRTUAL_PIXEL_RATIO_CHANGE,
    ACCESS_INFO_CHANGE,
};
class ScreenProperty {
public:
    ScreenProperty() = default;
    ~ScreenProperty() = default;

    void SetRotation(float rotation);
    float GetRotation() const;

    void SetBounds(const RRect& bounds);
    RRect GetBounds() const;

    void SetPhyBounds(const RRect& phyBounds);
    RRect GetPhyBounds() const;

    void SetFakeBounds(const RRect& fakeBounds);
    RRect GetFakeBounds() const;

    void SetIsFakeInUse(bool isFakeInUse);
    bool GetIsFakeInUse() const;

    void SetScaleX(float scaleX);
    float GetScaleX() const;

    void SetScaleY(float scaleY);
    float GetScaleY() const;

    void SetPivotX(float pivotX);
    float GetPivotX() const;

    void SetPivotY(float pivotY);
    float GetPivotY() const;

    void SetTranslateX(float translateX);
    float GetTranslateX() const;

    void SetTranslateY(float translateY);
    float GetTranslateY() const;

    float GetDensity();
    float GetDefaultDensity();
    void SetDefaultDensity(float defaultDensity);

    float GetDensityInCurResolution() const;
    void SetDensityInCurResolution(float densityInCurResolution);

    void SetPhyWidth(uint32_t phyWidth);
    int32_t GetPhyWidth() const;

    void SetPhyHeight(uint32_t phyHeight);
    int32_t GetPhyHeight() const;

    void SetDpiPhyBounds(uint32_t phyWidth, uint32_t phyHeight);

    void SetRefreshRate(uint32_t refreshRate);
    uint32_t GetRefreshRate() const;

    void SetPropertyChangeReason(std::string propertyChangeReason);
    std::string GetPropertyChangeReason() const;

    void SetDefaultDeviceRotationOffset(uint32_t defaultRotationOffset);
    uint32_t GetDefaultDeviceRotationOffset() const;

    void UpdateVirtualPixelRatio(const RRect& bounds);
    void SetVirtualPixelRatio(float virtualPixelRatio);
    float GetVirtualPixelRatio() const;

    void SetScreenRotation(Rotation rotation);
    void SetRotationAndScreenRotationOnly(Rotation rotation);
    Rotation GetScreenRotation() const;
    void UpdateScreenRotation(Rotation rotation);

    Rotation GetDeviceRotation() const;
    void UpdateDeviceRotation(Rotation rotation);

    void SetOrientation(Orientation orientation);
    Orientation GetOrientation() const;

    void SetDisplayState(DisplayState displayState);
    DisplayState GetDisplayState() const;

    void SetDisplayOrientation(DisplayOrientation displayOrientation);
    DisplayOrientation GetDisplayOrientation() const;
    void CalcDefaultDisplayOrientation();

    void SetDeviceOrientation(DisplayOrientation displayOrientation);
    DisplayOrientation GetDeviceOrientation() const;

    void SetPhysicalRotation(float rotation);
    float GetPhysicalRotation() const;

    void SetScreenComponentRotation(float rotation);
    float GetScreenComponentRotation() const;

    float GetXDpi() const;
    float GetYDpi() const;

    void SetOffsetX(int32_t offsetX);
    int32_t GetOffsetX() const;

    void SetOffsetY(int32_t offsetY);
    int32_t GetOffsetY() const;

    void SetOffset(int32_t offsetX, int32_t offsetY);

    void SetStartX(uint32_t startX);
    uint32_t GetStartX() const;

    void SetStartY(uint32_t startY);
    uint32_t GetStartY() const;

    void SetValidHeight(uint32_t validHeight);
    int32_t GetValidHeight() const;
 
    void SetValidWidth(uint32_t validWidth);
    int32_t GetValidWidth() const;

    void SetStartPosition(uint32_t startX, uint32_t startY);

    void SetScreenType(ScreenType type);
    ScreenType GetScreenType() const;

    void SetScreenRequestedOrientation(Orientation orientation);
    Orientation GetScreenRequestedOrientation() const;

    void SetScreenShape(ScreenShape screenShape);
    ScreenShape GetScreenShape() const;

    DMRect GetAvailableArea()
    {
        return availableArea_;
    }

    void SetAvailableArea(DMRect area)
    {
        availableArea_ = area;
    }

    DMRect GetExpandAvailableArea()
    {
        return expandAvailableArea_;
    }

    void SetExpandAvailableArea(DMRect area)
    {
        expandAvailableArea_ = area;
    }

    std::vector<DMRect> GetCreaseRects()
    {
        return creaseRects_;
    }

    void SetCreaseRects(std::vector<DMRect> creaseRects)
    {
        creaseRects_ = creaseRects;
    }

    RRect GetPhysicalTouchBounds();

    void SetPhysicalTouchBounds(bool isSecondaryDevice);

    int32_t GetInputOffsetX();

    int32_t GetInputOffsetY();

    void SetInputOffsetY(bool isSecondaryDevice, FoldDisplayMode foldDisplayMode);

    float CalculatePPI();
    uint32_t CalculateDPI();

    // OffScreenRender
    void SetCurrentOffScreenRendering(bool enable) { isCurrentOffScreenRendering_ = enable; }
    bool GetCurrentOffScreenRendering() { return isCurrentOffScreenRendering_; }
    void SetScreenRealWidth(uint32_t width) { screenRealWidth_ = width; }
    uint32_t GetScreenRealWidth() { return screenRealWidth_; }
    void SetScreenRealHeight(uint32_t height) { screenRealHeight_ = height; }
    uint32_t GetScreenRealHeight() { return screenRealHeight_; }
    void SetScreenRealPPI() { screenRealPPI_ = CalculatePPI(); }
    float GetScreenRealPPI() { return screenRealPPI_; }
    void SetScreenRealDPI() { screenRealDPI_ = CalculateDPI(); }
    uint32_t GetScreenRealDPI() { return screenRealDPI_; }

private:
    static inline bool IsVertical(Rotation rotation)
    {
        return (rotation == Rotation::ROTATION_0 || rotation == Rotation::ROTATION_180);
    }
    float rotation_ { 0.0f };
    float physicalRotation_ { 0.0f };
    float screenComponentRotation_ { 0.0f };
    RRect bounds_;
    RRect phyBounds_;
    RRect fakeBounds_;
    bool isFakeInUse_ = false;  // is fake bounds in use

    float scaleX_ { 1.0f };
    float scaleY_ { 1.0f };
    float pivotX_ { 0.5f };
    float pivotY_ { 0.5f };
    float translateX_ { 0.0f };
    float translateY_ { 0.0f };

    uint32_t phyWidth_ { UINT32_MAX };
    uint32_t phyHeight_ { UINT32_MAX };

    uint32_t dpiPhyWidth_ { UINT32_MAX };
    uint32_t dpiPhyHeight_ { UINT32_MAX };

    uint32_t refreshRate_ { 0 };
    uint32_t defaultDeviceRotationOffset_ { 0 };

    std::string propertyChangeReason_ { "" };

    float virtualPixelRatio_ { 1.0f };
    float defaultDensity_ { 1.0f };
    float densityInCurResolution_ { 1.0f };

    Orientation orientation_ { Orientation::UNSPECIFIED };
    DisplayOrientation displayOrientation_ { DisplayOrientation::UNKNOWN };
    DisplayOrientation deviceOrientation_ { DisplayOrientation::UNKNOWN };
    Rotation screenRotation_ { Rotation::ROTATION_0 };
    Rotation deviceRotation_ { Rotation::ROTATION_0 };
    Orientation screenRequestedOrientation_ { Orientation::UNSPECIFIED };
    DisplayState displayState_ { DisplayState::UNKNOWN };

    float xDpi_ { 0.0f };
    float yDpi_ { 0.0f };

    int32_t offsetX_ { 0 };
    int32_t offsetY_ { 0 };

    uint32_t startX_ { 0 };
    uint32_t startY_ { 0 };

    uint32_t validWidth_ { UINT32_MAX };
    uint32_t validHeight_ { UINT32_MAX };

    ScreenShape screenShape_ { ScreenShape::RECTANGLE };

    ScreenType type_ { ScreenType::REAL };

    void UpdateXDpi();
    void UpdateYDpi();
    void CalculateXYDpi(uint32_t phyWidth, uint32_t phyHeight);
    DMRect availableArea_;
    DMRect expandAvailableArea_;
    std::vector<DMRect> creaseRects_;

    RRect physicalTouchBounds_;
    int32_t inputOffsetX_ { 0 };
    int32_t inputOffsetY_ { 0 };

    // OffScreenRender
    bool isCurrentOffScreenRendering_ { false };
    uint32_t screenRealWidth_ { UINT32_MAX };
    uint32_t screenRealHeight_ { UINT32_MAX };
    float screenRealPPI_ { 0.0f };
    uint32_t screenRealDPI_ { 0 };
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_PROPERTY_H
