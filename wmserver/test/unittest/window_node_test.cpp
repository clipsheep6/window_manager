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
#include "window_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowNode> node;
};

void WindowNodeTest::SetUpTestCase()
{
}

void WindowNodeTest::TearDownTestCase()
{
}

void WindowNodeTest::SetUp()
{
    node = new WindowNode();
}

void WindowNodeTest::TearDown()
{
}
namespace {
/**
 * @tc.name: SetDisplayId01
 * @tc.desc: set display id
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetDisplayId01, Function | SmallTest | Level2)
{
    node->SetDisplayId(1);
    ASSERT_EQ(node->GetDisplayId(), 1);
}

/**
 * @tc.name: SetFullWindowHotArea01
 * @tc.desc: set display id
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeTest, SetFullWindowHotArea01, Function | SmallTest | Level2)
{
    Rect rect = {0, 0, 100, 100};
    node->SetFullWindowHotArea(rect);
    ASSERT_EQ(node->GetFullWindowHotArea(), rect);
}
}
}
}