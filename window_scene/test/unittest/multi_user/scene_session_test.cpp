/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "scene_session.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionTest : public Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SetMinimizedFlagByUserSwitch
 * @tc.desc: SetMinimizedFlagByUserSwitch
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest, SetMinimizedFlagByUserSwitch, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetMinimizedFlagByUserSwitch";
    info.bundleName_ = "SetMinimizedFlagByUserSwitch";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    bool isMinimized = true;
    session->SetMinimizedFlagByUserSwitch(isMinimized);
    bool ret = session->IsMinimizedByUserSwitch();
    ASSERT_EQ(ret, true);

    isMinimized = false;
    session->SetMinimizedFlagByUserSwitch(isMinimized);
    ret = session->IsMinimizedByUserSwitch();
    ASSERT_EQ(ret, false);
}
} // namespace Rosen
} // namespace OHOS