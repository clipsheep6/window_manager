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

#include <gtest/gtest.h>
#include "display_group_info.h"
#include "window_layout_policy.h"
#include "window_layout_policy_cascade.h"
#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowLayoutPolicyCascadeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowNodeContainer> container_;
    static sptr<DisplayGroupInfo> displayGroupInfo_;
    static sptr<DisplayInfo> defaultDisplayInfo_;
    static sptr<WindowLayoutPolicy> layoutPolicy_;
};

sptr<WindowNodeContainer> WindowLayoutPolicyCascadeTest::container_ = nullptr;
sptr<DisplayGroupInfo> WindowLayoutPolicyCascadeTest::displayGroupInfo_ = nullptr;
sptr<DisplayInfo> WindowLayoutPolicyCascadeTest::defaultDisplayInfo_ = nullptr;
sptr<WindowLayoutPolicyCascade> WindowLayoutPolicyCascadeTest::layoutPolicy_ = nullptr;

void WindowLayoutPolicyCascadeTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));

    defaultDisplayInfo_ = display->GetDisplayInfo();
    ASSERT_TRUE((defaultDisplayInfo_ != nullptr));

    container_ = new WindowNodeContainer(defaultDisplayInfo_, display->GetScreenId());
    displayGroupInfo_ = container_->displayGroupInfo_;
    layoutPolicy_ = container_->GetLayoutPolicy();
}

void WindowLayoutPolicyCascadeTest::TearDownTestCase()
{
    container_ = nullptr;
    displayGroupInfo_ = nullptr;
    defaultDisplayInfo_ = nullptr;
}

void WindowLayoutPolicyCascadeTest::SetUp()
{
    displayGroupInfo_->AddDisplayInfo(defaultDisplayInfo_);
}

void WindowLayoutPolicyCascadeTest::TearDown()
{
    displayGroupInfo_->displayInfosMap_.clear();
}

namespace {
/**
 * @tc.name: InitCascadeRectCfg01
 * @tc.desc: test InitCascadeRectCfg
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyCascadeTest, InitCascadeRectCfg01, Function | SmallTest | Level2)
{
    std::vector<int> numbers {82, 121, 1068, 706};
    WindowLayoutPolicyCascade::SetDefaultFloatingWindow(numbers);
    ASSERT_EQ(defaultFloatingWindow_.posX_, static_cast<uint32_t>(numbers[0]));

    auto displayId = defaultDisplayInfo_->GetDisplayId();
    ASSERT_EQ(layoutPolicy_->InitCascadeRectCfg(displayId), true);

    Rect resRect = defaultFloatingWindow_;
    auto vpr = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
    resRect.width_ = static_cast<uint32_t>(vpr * resRect.width_);
    resRect.height_ = static_cast<uint32_t>(vpr * resRect.height_);
    resRect.posX_ = static_cast<int32_t>(vpr * resRect.posX_);
    resRect.posY_ = static_cast<int32_t>(vpr * resRect.posY_);

    ASSERT_EQ(cascadeRectsMap_[displayId].defaultCascadeRect_, resRect);
}
}
}
}

