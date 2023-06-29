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
#include "session_manager/include/scene_session_manager.h"
#include "session/host/include/scene_session.h"
#include <surface.h>

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest::SetUpTestCase()
{
}

void SceneSessionTest::TearDownTestCase()
{
}

void SceneSessionTest::SetUp()
{
}

void SceneSessionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetSceneSceneSessionProxy01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, GetSceneSceneSessionProxy01, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, SceneSession::GetInstance().Foreground());
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, SceneSession::GetInstance().Background());
}

}
}
}