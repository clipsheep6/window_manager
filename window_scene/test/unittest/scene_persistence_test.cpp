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

#include "scene_persistence.h"
#include <gtest/gtest.h>
#include "session_info.h"
#include "ws_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScenePersistenceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

constexpr const char *UNDERLINE_SEPARATOR = "_";
std::string ScenePersistence::strPersistPath_;

void ScenePersistenceTest::SetUpTestCase()
{
}

void ScenePersistenceTest::TearDownTestCase()
{
}

void ScenePersistenceTest::SetUp()
{
}

void ScenePersistenceTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SaveSnapshot
 * @tc.desc: test function : SaveSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, SaveSnapshot, Function | SmallTest | Level1)
{
    Rosen::SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    int32_t persistendId_ = 10086;
    ScenePersistence scenePersistence_(info, persistendId_);
    scenePersistence_.SaveSnapshot(nullptr);
    ASSERT_EQ(scenePersistence_.GetSnapshotFilePath().empty(), false);
}

/**
 * @tc.name: GetSnapshotFilePath
 * @tc.desc: test function : GetSnapshotFilePath
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, GetSnapshotFilePath, Function | SmallTest | Level1)
{
    Rosen::SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    int32_t persistentId_ = 10086;
    ScenePersistence scenePersistence_(info, persistentId_);
    uint32_t fileID = static_cast<uint32_t>(persistentId_) & 0x3fffffff;
    std::string strSnapshotFile_ =
        ScenePersistence::strPersistPath_ + info.bundleName_ + UNDERLINE_SEPARATOR + std::to_string(fileID);
    ASSERT_EQ(strSnapshotFile_, scenePersistence_.GetSnapshotFilePath());
}

/**
 * @tc.name: IsSnapshotExisted
 * @tc.desc: test function : IsSnapshotExisted
 * @tc.type: FUNC
 */
HWTEST_F(ScenePersistenceTest, IsSnapshotExisted, Function | SmallTest | Level1)
{
    Rosen::SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    int32_t persistentId_ = 10086;
    ScenePersistence scenePersistence_(info, persistentId_);
    bool result = scenePersistence_.IsSnapshotExisted();
    ASSERT_EQ(result, false);
}
}
}
}