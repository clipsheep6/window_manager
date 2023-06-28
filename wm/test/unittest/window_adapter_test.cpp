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

#include <gtest/gtest.h>
#include "window_adapter.h"
#include "window_proxy.h"

#include "window_agent.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowAdapterTest::SetUpTestCase()
{
}

void WindowAdapterTest::TearDownTestCase()
{
}

void WindowAdapterTest::SetUp()
{
}

void WindowAdapterTest::TearDown()
{
}

namespace {
/**
 * @tc.name: InitSSMProxy01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, InitSSMProxy01, Function | SmallTest | Level2)
{
    bool result = WindowAdapter::GetInstance().InitSSMProxy();
    ASSERT_EQ(false, result);
    result = WindowAdapter::GetInstance().InitSSMProxy();
    ASSERT_EQ(true, result);
}

}
}
}
