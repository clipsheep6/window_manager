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
#include "iconsumer_surface.h"
#include "session_manager/include/session_manager.h"
#include <surface.h>
#include "zidl/session_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionManagerTest::SetUpTestCase()
{
}

void SessionManagerTest::TearDownTestCase()
{
}

void SessionManagerTest::SetUp()
{
}

void SessionManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetSceneSessionManagerProxy01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, GetSceneSessionManagerProxy01, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, SessionManager::GetInstance().GetSceneSessionManagerProxy());
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, SessionManager::GetInstance().InitSceneSessionManagerProxy());
}

}
}
}