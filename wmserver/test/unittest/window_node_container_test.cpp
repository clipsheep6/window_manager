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

#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowNodeContainerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowNodeContainerTest::SetUpTestCase()
{
}

void WindowNodeContainerTest::TearDownTestCase()
{
}

void WindowNodeContainerTest::SetUp()
{
}

void WindowNodeContainerTest::TearDown()
{
}
namespace {
/**
 * @tc.name: NotifyDockWindowStateChanged01
 * @tc.desc: notify dock window state change
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, NotifyDockWindowStateChanged01, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    ScreenId displayGroupId = displayInfo->GetScreenGroupId();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo, displayGroupId);
    sptr<WindowNode> node = new WindowNode();
    node->SetDisplayId(displayId);
    WMError res = container->NotifyDockWindowStateChanged(node, false);
    ASSERT_EQ(res, WMError::WM_OK);
}
}
}
}