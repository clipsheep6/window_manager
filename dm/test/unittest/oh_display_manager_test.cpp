/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "display.h"
#include "display_info.h"
#include "oh_display_manager.h"
#include "oh_display_manager_inner.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHDisplayManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};


void OHDisplayManagerTest::SetUpTestCase()
{
}

void OHDisplayManagerTest::TearDownTestCase()
{
}

void OHDisplayManagerTest::SetUp()
{
}

void OHDisplayManagerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayId
 * @tc.desc: displayId == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayId01, Function | SmallTest | Level2)
{
    uint64_t *displayId = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayId(displayId);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayId
 * @tc.desc: displayId != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayId02, Function | SmallTest | Level2)
{
    uint64_t testParm = 4000;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayId(&testParm);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayWidth
 * @tc.desc: displayWidth == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayWidth01, Function | SmallTest | Level2)
{
    int32_t *displayWidth = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayWidth(displayWidth);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayWidth
 * @tc.desc: displayWidth != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayWidth02, Function | SmallTest | Level2)
{
    int32_t testWidth = 200;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayWidth(&testWidth);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayHeight
 * @tc.desc: displayHeight == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayHeight01, Function | SmallTest | Level2)
{
    int32_t *displayHeight = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayHeight(displayHeight);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayHeight
 * @tc.desc: displayHeight != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayHeight02, Function | SmallTest | Level2)
{
    int32_t testHeight = 200;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayHeight(&testHeight);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRotation
 * @tc.desc: displayRotation == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRotation01, Function | SmallTest | Level2)
{
    NativeDisplayManager_Rotation *displayRotation = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRotation(displayRotation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRotation
 * @tc.desc: displayRotation != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRotation02, Function | SmallTest | Level2)
{
    NativeDisplayManager_Rotation testRotation = NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_180;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRotation(&testRotation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayOrientation
 * @tc.desc: displayOrientation == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayOrientation01, Function | SmallTest | Level2)
{
    NativeDisplayManager_Orientation *displayOrientation = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayOrientation(displayOrientation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayOrientation
 * @tc.desc: displayOrientation != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayOrientation02, Function | SmallTest | Level2)
{
    NativeDisplayManager_Orientation testOrientation = NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayOrientation(&testOrientation);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);;
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio
 * @tc.desc: virtualPixel == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio01,
    Function | SmallTest | Level2)
{
    float *virtualPixel = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio(virtualPixel);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio
 * @tc.desc: virtualPixel != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio02,
    Function | SmallTest | Level2)
{
    float testPixel = 3.14;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio(&testPixel);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRefreshRate
 * @tc.desc: refreshRate == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRefreshRate01, Function | SmallTest | Level2)
{
    uint32_t *refreshRate = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRefreshRate(refreshRate);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayRefreshRate
 * @tc.desc: refreshRate != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayRefreshRate02, Function | SmallTest | Level2)
{
    uint32_t testRate = 60;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayRefreshRate(&testRate);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityDpi
 * @tc.desc: densityDpi == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityDpi01, Function | SmallTest | Level2)
{
    int32_t *densityDpi = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(densityDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityDpi
 * @tc.desc: densityDpi != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityDpi02, Function | SmallTest | Level2)
{
    int32_t testDpi = 160;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(&testDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityPixels
 * @tc.desc: densityPixels == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityPixels01, Function | SmallTest | Level2)
{
    float *densityPixels = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityPixels(densityPixels);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityPixels
 * @tc.desc: densityPixels != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityPixels02, Function | SmallTest | Level2)
{
    float testPixels = 1.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityPixels(&testPixels);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayScaledDensity
 * @tc.desc: scaledDensity == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayScaledDensity01, Function | SmallTest | Level2)
{
    float *scaledDensity = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayScaledDensity(scaledDensity);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayScaledDensity
 * @tc.desc: scaledDensity != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayScaledDensity02, Function | SmallTest | Level2)
{
    float testDensity = 1.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayScaledDensity(&testDensity);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi
 * @tc.desc: xDpi == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi01, Function | SmallTest | Level2)
{
    float *xDpi = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(xDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi
 * @tc.desc: xDpi != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi02, Function | SmallTest | Level2)
{
    float testXDpi = 2.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityXdpi(&testXDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi
 * @tc.desc: yDpi == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi01, Function | SmallTest | Level2)
{
    float *yDpi = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(yDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi
 * @tc.desc: yDpi != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi02, Function | SmallTest | Level2)
{
    float testYDpi = 2.0;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetDefaultDisplayDensityYdpi(&testYDpi);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetFoldDisplayMode
 * @tc.desc: foldDisplayMode == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetFoldDisplayMode01, Function | SmallTest | Level2)
{
    NativeDisplayManager_FoldDisplayMode *foldDisplayMode = nullptr;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetFoldDisplayMode(foldDisplayMode);
    EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: OH_NativeDisplayManager_GetFoldDisplayMode
 * @tc.desc: foldDisplayMode != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(OHDisplayManagerTest, OH_NativeDisplayManager_GetFoldDisplayMode02, Function | SmallTest | Level2)
{
    NativeDisplayManager_FoldDisplayMode testDisplayMode =
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN;
    NativeDisplayManager_ErrorCode ret = OH_NativeDisplayManager_GetFoldDisplayMode(&testDisplayMode);
    if (OH_NativeDisplayManager_IsFoldable()) {
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK);
    } else {
        EXPECT_EQ(ret, NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED);
    }
}
}
} // namespace Rosen
} // namespace OHOS