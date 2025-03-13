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

#include "screen_property.h"
#include <gtest/gtest.h>

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenPropertyTest : public testing::Test {
  public:
    ScreenPropertyTest() {}
    ~ScreenPropertyTest() {}
};

namespace {
/**
 * @tc.name: SetScreenRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetScreenRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int64_t ret = 0;
    Rotation rotation = Rotation::ROTATION_0;
    property->SetScreenRotation(rotation);

    rotation = Rotation::ROTATION_90;
    property->SetScreenRotation(rotation);

    rotation = Rotation::ROTATION_180;
    property->SetScreenRotation(rotation);

    rotation = Rotation::ROTATION_270;
    property->SetScreenRotation(rotation);
    ASSERT_EQ(ret, 0);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRotation end";
}

/**
 * @tc.name: SetRotationAndScreenRotationOnly001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetRotationAndScreenRotationOnly001, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetRotationAndScreenRotationOnly001 start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    Rotation ret = Rotation::ROTATION_0;
    Rotation rotation = Rotation::ROTATION_0;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);

    Rotation rotation = Rotation::ROTATION_90;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);

    Rotation rotation = Rotation::ROTATION_180;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);

    Rotation rotation = Rotation::ROTATION_270;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetRotationAndScreenRotationOnly001 end";
}
 
 /**
 * @tc.name: SetRotationAndScreenRotationOnly002
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetRotationAndScreenRotationOnly002, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetRotationAndScreenRotationOnly002 start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    Rotation ret = Rotation::ROTATION_0;
    Rotation rotation = Rotation::ROTATION_0;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetRotation();
    ASSERT_EQ(ret, rotation);

    Rotation rotation = Rotation::ROTATION_90;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetRotation();
    ASSERT_EQ(ret, rotation);

    Rotation rotation = Rotation::ROTATION_180;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetRotation();
    ASSERT_EQ(ret, rotation);

    Rotation rotation = Rotation::ROTATION_270;
    property->SetRotationAndScreenRotationOnly(rotation);
    ret = property->GetRotation();
    ASSERT_EQ(ret, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetRotationAndScreenRotationOnly002 end";
}

/**
 * @tc.name: UpdateDeviceRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, UpdateDeviceRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateDeviceRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    Rotation ret = Rotation::ROTATION_0;
    Rotation rotation = Rotation::ROTATION_0;
    property->UpdateDeviceRotation(rotation);
    ret = property->GetDeviceRotation();
    ASSERT_EQ(ret, rotation);

    rotation = Rotation::ROTATION_90;
    property->UpdateDeviceRotation(rotation);
    ret = property->GetDeviceRotation();
    ASSERT_EQ(ret, rotation);

    rotation = Rotation::ROTATION_180;
    property->UpdateDeviceRotation(rotation);
    ret = property->GetDeviceRotation();
    ASSERT_EQ(ret, rotation);

    rotation = Rotation::ROTATION_270;
    property->UpdateDeviceRotation(rotation);
    ret = property->GetDeviceRotation();
    ASSERT_EQ(ret, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateDeviceRotation end";
}

/**
 * @tc.name: SetDeviceOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetDeviceOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetDeviceOrientation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    DisplayOrientation ret = DisplayOrientation::PORTRAIT;
    DisplayOrientation displayOrientation = DisplayOrientation::PORTRAIT;
    property->SetDeviceOrientation(displayOrientation);
    ret = property->GetDeviceOrientation();
    ASSERT_EQ(ret, displayOrientation);

    displayOrientation = DisplayOrientation::LANDSCAPE;
    property->SetDeviceOrientation(displayOrientation);
    ret = property->GetDeviceOrientation();
    ASSERT_EQ(ret, displayOrientation);

    displayOrientation = DisplayOrientation::PORTRAIT_INVERTED;
    property->SetDeviceOrientation(displayOrientation);
    ret = property->GetDeviceOrientation();
    ASSERT_EQ(ret, displayOrientation);

    displayOrientation = DisplayOrientation::LANDSCAPE_INVERTED;
    property->SetDeviceOrientation(displayOrientation);
    ret = property->GetDeviceOrientation();
    ASSERT_EQ(ret, displayOrientation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetDeviceOrientation end";
}

/**
 * @tc.name: UpdateScreenRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, UpdateScreenRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateScreenRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    Rotation ret = Rotation::ROTATION_0;
    Rotation rotation = Rotation::ROTATION_0;
    property->UpdateScreenRotation(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);

    rotation = Rotation::ROTATION_90;
    property->UpdateScreenRotation(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);

    rotation = Rotation::ROTATION_180;
    property->UpdateScreenRotation(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);

    rotation = Rotation::ROTATION_270;
    property->UpdateScreenRotation(rotation);
    ret = property->GetScreenRotation();
    ASSERT_EQ(ret, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateScreenRotation end";
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOrientation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    Orientation ret = Orientation::BEGIN;
    Orientation orientation = Orientation::SENSOR;
    property->SetOrientation(orientation);
    ret = property->orientation_;
    ASSERT_EQ(ret, orientation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOrientation end";
}

/**
 * @tc.name: GetOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetOrientation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    Orientation ret = Orientation::BEGIN;
    Orientation orientation = Orientation::SENSOR;
    property->SetOrientation(orientation);
    ret = property->GetOrientation();
    ASSERT_EQ(ret, orientation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetOrientation end";
}

/**
 * @tc.name: UpdateVirtualPixelRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, UpdateVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateVirtualPixelRatio start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int64_t ret = 0;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;

    property->UpdateVirtualPixelRatio(bounds);

    bounds.rect_.height_ = 1111;
    property->UpdateVirtualPixelRatio(bounds);

    bounds.rect_.width_ = 1111;
    bounds.rect_.height_ = 2772;
    property->UpdateVirtualPixelRatio(bounds);

    bounds.rect_.width_ = 1111;
    bounds.rect_.height_ = 1111;
    property->UpdateVirtualPixelRatio(bounds);
    ASSERT_EQ(ret, 0);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateVirtualPixelRatio end";
}

/**
 * @tc.name: SetBounds
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetBounds, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetBounds start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int64_t ret = 0;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;

    uint32_t phyWidth = UINT32_MAX;
    property->SetPhyWidth(phyWidth);
    uint32_t phyHeigth = UINT32_MAX;
    property->SetPhyHeight(phyHeigth);
    property->SetBounds(bounds);

    bounds.rect_.width_ = 2772;
    bounds.rect_.height_ = 1344;

    uint32_t phyWidth1 = 2772;
    property->SetPhyWidth(phyWidth1);
    uint32_t phyHeigth1 = 1344;
    property->SetPhyHeight(phyHeigth1);
    property->SetBounds(bounds);
    ASSERT_EQ(ret, 0);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetBounds end";
}

/**
 * @tc.name: CalculateXYDpi
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, CalculateXYDpi, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: CalculateXYDpi start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    uint32_t phyWidth = 0;
    uint32_t phyHeight = 0;
    int ret = 0;
    property->CalculateXYDpi(phyWidth, phyHeight);
    phyWidth = 1;
    phyHeight = 1;
    property->CalculateXYDpi(phyWidth, phyHeight);
    ASSERT_EQ(ret, 0);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: CalculateXYDpi end";
}

/**
 * @tc.name: SetOffsetX
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOffsetX, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetX start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int32_t offsetX = 0;
    property->SetOffsetX(offsetX);
    int32_t ret = property->GetOffsetX();
    ASSERT_EQ(ret, offsetX);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetX end";
}

/**
 * @tc.name: SetOffsetY
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOffsetY, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetY start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int32_t offsetY = 0;
    property->SetOffsetY(offsetY);
    int32_t ret = property->GetOffsetY();
    ASSERT_EQ(ret, offsetY);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetY end";
}

/**
 * @tc.name: SetOffset
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOffset, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffset start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    property->SetOffset(offsetX, offsetY);
    int32_t ret_x = property->GetOffsetX();
    int32_t ret_y = property->GetOffsetY();
    ASSERT_EQ(ret_x, offsetX);
    ASSERT_EQ(ret_y, offsetY);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffset end";
}

/**
 * @tc.name: SetScreenRequestedOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetScreenRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRequestedOrientation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    Orientation orientation = Orientation::UNSPECIFIED;
    property->SetScreenRequestedOrientation(orientation);
    Orientation ret = property->GetScreenRequestedOrientation();
    ASSERT_EQ(ret, orientation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRequestedOrientation end";
}

/**
 * @tc.name: GetPhyHeight
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetPhyHeight, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPhyHeight start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    uint32_t phyHeight = 1;
    property->SetPhyHeight(phyHeight);
    int32_t ret = property->GetPhyHeight();
    ASSERT_EQ(ret, phyHeight);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPhyHeight end";
}

/**
 * @tc.name: SetRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float rotation = 2.0f;
    property->SetRotation(rotation);
    ASSERT_EQ(property->rotation_, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetRotation end";
}

/**
 * @tc.name: GetRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float rotation = property->GetRotation();
    ASSERT_EQ(property->rotation_, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetRotation end";
}

/**
 * @tc.name: SetPhysicalRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetPhysicalRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPhysicalRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float rotation = 2.0f;
    property->SetPhysicalRotation(rotation);
    ASSERT_EQ(property->physicalRotation_, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPhysicalRotation end";
}

/**
 * @tc.name: GetPhysicalRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetPhysicalRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPhysicalRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float rotation = property->GetPhysicalRotation();
    ASSERT_EQ(property->physicalRotation_, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPhysicalRotation end";
}

/**
 * @tc.name: SetScreenComponentRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetScreenComponentRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenComponentRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float rotation = 2.0f;
    property->SetScreenComponentRotation(rotation);
    ASSERT_EQ(property->screenComponentRotation_, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenComponentRotation end";
}

/**
 * @tc.name: GetScreenComponentRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetScreenComponentRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetScreenComponentRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float rotation = property->GetScreenComponentRotation();
    ASSERT_EQ(property->screenComponentRotation_, rotation);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetScreenComponentRotation end";
}

/**
 * @tc.name: GetBounds
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetBounds, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetBounds start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    RRect bounds = property->GetBounds();
    ASSERT_EQ(property->bounds_, bounds);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetBounds end";
}

/**
 * @tc.name: SetPhyBounds
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetPhyBounds, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPhyBounds start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    RRect phyBounds;
    phyBounds.rect_.width_ = 1344;
    phyBounds.rect_.height_ = 2772;
    property->SetPhyBounds(phyBounds);
    ASSERT_EQ(property->phyBounds_, phyBounds);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPhyBounds end";
}

/**
 * @tc.name: GetPhyBounds
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetPhyBounds, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPhyBounds start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    RRect phyBounds = property->GetPhyBounds();
    ASSERT_EQ(property->phyBounds_, phyBounds);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPhyBounds end";
}

/**
 * @tc.name: GetDensity
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetDensity, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetDensity start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float virtualPixelRatio = 1.0f;
    ASSERT_EQ(property->virtualPixelRatio_, virtualPixelRatio);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetDensity end";
}

/**
 * @tc.name: GetDefaultDensity
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetDefaultDensity, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetDefaultDensity start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float defaultDensity = 1.0f;
    ASSERT_EQ(property->GetDefaultDensity(), defaultDensity);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetDefaultDensity end";
}

/**
 * @tc.name: SetDefaultDensity
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetDefaultDensity, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetDefaultDensity start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float defaultDensity = 1.0f;
    property->SetDefaultDensity(defaultDensity);
    ASSERT_EQ(property->defaultDensity_, defaultDensity);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetDefaultDensity end";
}

/**
 * @tc.name: GetDensityInCurResolution
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetDensityInCurResolution, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetDensityInCurResolution start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float densityInCurResolution = 1.0f;
    ASSERT_EQ(property->GetDensityInCurResolution(), densityInCurResolution);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetDensityInCurResolution end";
}

/**
 * @tc.name: SetDensityInCurResolution
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetDensityInCurResolution, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetDensityInCurResolution start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float densityInCurResolution = 1.0f;
    property->SetDensityInCurResolution(densityInCurResolution);
    ASSERT_EQ(property->densityInCurResolution_, densityInCurResolution);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetDensityInCurResolution end";
}

/**
 * @tc.name: GetPropertyChangeReason
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetPropertyChangeReason, Function | SmallTest | Level2)
{
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    std::string propertyChangeReason = "a";
    std::string propertyChangeReason_copy = property->GetPropertyChangeReason();
    property->SetPropertyChangeReason(propertyChangeReason);
    ASSERT_EQ(propertyChangeReason, property->GetPropertyChangeReason());
    property->SetPropertyChangeReason(propertyChangeReason_copy);
}

/**
 * @tc.name: CalcDefaultDisplayOrientation
 * @tc.desc: bounds_.rect_.width_ > bounds_.rect_.height_
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, CalcDefaultDisplayOrientation, Function | SmallTest | Level2)
{
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    RRect bounds_temp = property->GetBounds();
    RRect bounds;
    bounds.rect_.width_ = 2772;
    bounds.rect_.height_ = 1344;
    property->SetBounds(bounds);
    property->CalcDefaultDisplayOrientation();
    ASSERT_EQ(DisplayOrientation::LANDSCAPE, property->GetDisplayOrientation());
    property->SetBounds(bounds_temp);
}

/**
 * @tc.name: SetStartX
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetStartX, Function | SmallTest | Level2)
{
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t ret = 100;
    uint32_t ret_copy = property->GetStartX();
    property->SetStartX(ret);
    ASSERT_EQ(ret, property->GetStartX());
    property->SetStartX(ret_copy);
}

/**
 * @tc.name: SetStartY
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetStartY, Function | SmallTest | Level2)
{
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t ret = 100;
    uint32_t ret_copy = property->GetStartY();
    property->SetStartY(ret);
    ASSERT_EQ(ret, property->GetStartY());
    property->SetStartY(ret_copy);
}

/**
 * @tc.name: SetStartPosition
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetStartPosition, Function | SmallTest | Level2)
{
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t ret_x = 100;
    uint32_t ret_y = 200;
    uint32_t retx_copy = property->GetStartX();
    uint32_t rety_copy = property->GetStartY();
    property->SetStartPosition(ret_x, ret_y);
    ASSERT_EQ(100, property->GetStartX());
    ASSERT_EQ(200, property->GetStartY());
    property->SetStartPosition(retx_copy, rety_copy);
}

/**
 * @tc.name: SetScreenShape
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetScreenShape, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenShape start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    ScreenShape screenshape = ScreenShape::ROUND;
    property->SetScreenShape(screenshape);
    ASSERT_EQ(property->screenShape_, screenshape);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenShape end";
}

/**
 * @tc.name: GetScreenShape
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetScreenShape, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetScreenShape start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    ScreenShape screenshape = ScreenShape::ROUND;
    property->SetScreenShape(screenshape);
    ASSERT_EQ(property->GetScreenShape(), screenshape);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetScreenShape end";
}

/**
 * @tc.name: SetPointerActiveWidth
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetPointerActiveWidth, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPointerActiveWidth start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t pointerActiveWidth = 123;
    property->SetPointerActiveWidth(pointerActiveWidth);
    ASSERT_EQ(property->pointerActiveWidth_, pointerActiveWidth);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPointerActiveWidth end";
}
 
/**
 * @tc.name: GetPointerActiveWidth
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetPointerActiveWidth, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPointerActiveWidth start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t pointerActiveWidth = 123;
    property->SetPointerActiveWidth(pointerActiveWidth);
    ASSERT_EQ(property->GetPointerActiveWidth(), pointerActiveWidth);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPointerActiveWidth end";
}
 
/**
 * @tc.name: SetPointerActiveHeight
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetPointerActiveHeight, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPointerActiveHeight start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t pointerActiveHeight = 321;
    property->SetPointerActiveHeight(pointerActiveHeight);
    ASSERT_EQ(property->pointerActiveHeight_, pointerActiveHeight);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetPointerActiveHeight end";
}
 
/**
 * @tc.name: GetPointerActiveHeight
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetPointerActiveHeight, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPointerActiveHeight start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    uint32_t pointerActiveHeight = 321;
    property->SetPointerActiveHeight(pointerActiveHeight);
    ASSERT_EQ(property->GetPointerActiveHeight(), pointerActiveHeight);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetPointerActiveHeight end";
}

/**
* @tc.name: SetX
* @tc.desc: normal function
* @tc.type: FUNC
*/
HWTEST_F(ScreenPropertyTest, SetX, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetX start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    int32_t ret = -1000;
    int32_t ret_copy = property->GetX();
    property->SetX(ret);
    ASSERT_EQ(ret, property->GetX());
    property->SetX(ret_copy);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetX end";
}

/**
* @tc.name: SetY
* @tc.desc: normal function
* @tc.type: FUNC
*/
HWTEST_F(ScreenPropertyTest, SetY, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetY start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    int32_t ret = -1000;
    int32_t ret_copy = property->GetY();
    property->SetY(ret);
    ASSERT_EQ(ret, property->GetY());
    property->SetY(ret_copy);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetY end";
}

/**
* @tc.name: SetXYPosition
* @tc.desc: normal function
* @tc.type: FUNC
*/
HWTEST_F(ScreenPropertyTest, SetXYPosition, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetXYPosition start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    int32_t ret_x = 1000;
    int32_t ret_y = 2000;
    int32_t retx_copy = property->GetX();
    int32_t rety_copy = property->GetY();
    property->SetXYPosition(ret_x, ret_y);
    ASSERT_EQ(1000, property->GetX());
    ASSERT_EQ(2000, property->GetY());
    property->SetXYPosition(retx_copy, rety_copy);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetXYPosition end";
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetVirtualPixelRatio start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float pixelRatio = 1.0f;
    float pixelCopy = property->GetVirtualPixelRatio();
    property->SetVirtualPixelRatio(pixelRatio);
    ASSERT_EQ(property->virtualPixelRatio_, pixelRatio);
    property->SetVirtualPixelRatio(pixelCopy);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetVirtualPixelRatio end";
}
 
/**
 * @tc.name: GetVirtualPixelRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, GetVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetVirtualPixelRatio start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    ASSERT_NE(property, nullptr);
    float pixelRatio = 1.0f;
    float pixelCopy = property->GetVirtualPixelRatio();
    property->SetVirtualPixelRatio(pixelRatio);
    ASSERT_EQ(property->GetVirtualPixelRatio(), pixelRatio);
    property->SetVirtualPixelRatio(pixelCopy);
    delete property;
    GTEST_LOG_(INFO) << "ScreenPropertyTest: GetVirtualPixelRatio end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
