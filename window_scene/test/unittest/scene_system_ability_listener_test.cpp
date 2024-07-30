/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "session_manager/include/scene_system_ability_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSystemAbilityListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSystemAbilityListenerTest::SetUpTestCase()
{
}

void SceneSystemAbilityListenerTest::TearDownTestCase()
{
}

void SceneSystemAbilityListenerTest::SetUp()
{
}

void SceneSystemAbilityListenerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnAddSystemAbility
 * @tc.desc: OnAddSystemAbility Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSystemAbilityListenerTest, OnAddSystemAbility, Function | MediumTest | Level2)
{
    SCBThreadInfo threadInfo = {
        .scbUid_ = "scbUid", .scbPid_ = "scbPid",
        .scbTid_ = "scbTid", .scbBundleName_ = "scbBundleName"
    };
    auto RES_SCHED_SYS_ABILITY_ID = 1901;
    auto statusChangeListener = sptr<SceneSystemAbilityListener>::MakeSptr(threadInfo);
    ASSERT_NE(nullptr, statusChangeListener);
    auto systemAbilityId = RES_SCHED_SYS_ABILITY_ID;
    statusChangeListener->OnAddSystemAbility(systemAbilityId, 0);
    ASSERT_EQ(systemAbilityId, RES_SCHED_SYS_ABILITY_ID);
}
} // namespace
} // namespace Rosen
} // namespace OHOS