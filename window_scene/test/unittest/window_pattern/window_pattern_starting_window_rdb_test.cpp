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

#include "interfaces/include/ws_common.h"
#include "session_manager/include/rdb/starting_window_rdb_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string TEST_RDB_PATH = "/data/test/";
const std::string TEST_INVALID_PATH = "";
const std::string TEST_RDB_NAME = "starting_window_config_test.db";
} // namespace

class WindowPatternStartingWindowRdbTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<StartingWindowRdbManager> startingWindowRdbMgr_;
};

std::shared_ptr<StartingWindowRdbManager> WindowPatternStartingWindowRdbTest::startingWindowRdbMgr_ = nullptr;

void WindowPatternStartingWindowRdbTest::SetUpTestCase() {}

void WindowPatternStartingWindowRdbTest::TearDownTestCase() {}

void WindowPatternStartingWindowRdbTest::SetUp()
{
    NativeRdb::RdbHelper::DeleteRdbStore(TEST_RDB_PATH + TEST_RDB_NAME);
    WmsRdbConfig config;
    config.dbName = TEST_RDB_NAME;
    config.dbPath = TEST_RDB_PATH;
    startingWindowRdbMgr_ = std::make_shared<StartingWindowRdbManager>(config);
}

void WindowPatternStartingWindowRdbTest::TearDown()
{
    NativeRdb::RdbHelper::DeleteRdbStore(TEST_RDB_PATH + TEST_RDB_NAME);
    startingWindowRdbMgr_ = nullptr;
}

namespace {
/**
 * @tc.name: GetRdbStore
 * @tc.desc: GetRdbStore
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowRdbTest, GetRdbStore, TestSize.Level1)
{
    ASSERT_NE(startingWindowRdbMgr_, nullptr);
    startingWindowRdbMgr_->wmsRdbConfig_.dbPath = TEST_INVALID_PATH;
    auto rdbStore = startingWindowRdbMgr_->GetRdbStore();
    ASSERT_EQ(rdbStore, nullptr);
    startingWindowRdbMgr_->wmsRdbConfig_.dbPath = TEST_RDB_PATH;
    rdbStore = startingWindowRdbMgr_->GetRdbStore();
    ASSERT_NE(rdbStore, nullptr);
    startingWindowRdbMgr_->Init();
    rdbStore = startingWindowRdbMgr_->GetRdbStore();
    ASSERT_NE(rdbStore, nullptr);
}

/**
 * @tc.name: Init
 * @tc.desc: Init
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowRdbTest, Init, TestSize.Level1)
{
    ASSERT_NE(startingWindowRdbMgr_, nullptr);
    startingWindowRdbMgr_->wmsRdbConfig_.dbPath = TEST_INVALID_PATH;
    bool res = startingWindowRdbMgr_->Init();
    ASSERT_EQ(res, false);
    startingWindowRdbMgr_->wmsRdbConfig_.dbPath = TEST_RDB_PATH;
    res = startingWindowRdbMgr_->Init();
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: InsertData
 * @tc.desc: InsertData
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowRdbTest, InsertData, TestSize.Level1)
{
    ASSERT_NE(startingWindowRdbMgr_, nullptr);
    StartingWindowRdbItemKey itemKey = {
        .bundleName = "testName",
        .moduleName = "testName",
        .abilityName = "testName",
        .darkMode = false,
    };
    StartingWindowInfo startingWindowInfo;
    auto res = startingWindowRdbMgr_->InsertData(itemKey, startingWindowInfo);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: BatchInsert
 * @tc.desc: BatchInsert
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowRdbTest, BatchInsert, TestSize.Level1)
{
    ASSERT_NE(startingWindowRdbMgr_, nullptr);
    StartingWindowRdbItemKey firstItemKey, secondItemKey;
    StartingWindowInfo firstStartingWindowInfo, secondStartingWindowInfo;
    firstItemKey.bundleName = "first";
    secondItemKey.bundleName = "second";
    std::vector<std::pair<StartingWindowRdbItemKey, StartingWindowInfo>> inputValues{
        std::make_pair(firstItemKey, firstStartingWindowInfo),
        std::make_pair(secondItemKey, secondStartingWindowInfo),
    };
    int64_t outInsertNum = -1;
    auto res = startingWindowRdbMgr_->BatchInsert(outInsertNum, inputValues);
    ASSERT_EQ(res, true);
    ASSERT_EQ(outInsertNum, inputValues.size());
}

/**
 * @tc.name: DeleteDataByBundleName
 * @tc.desc: DeleteDataByBundleName
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowRdbTest, DeleteDataByBundleName, TestSize.Level1)
{
    ASSERT_NE(startingWindowRdbMgr_, nullptr);
    StartingWindowRdbItemKey itemKey;
    itemKey.bundleName = "testName";
    StartingWindowInfo startingWindowInfo;
    auto res = startingWindowRdbMgr_->InsertData(itemKey, startingWindowInfo);
    ASSERT_EQ(res, true);
    res = startingWindowRdbMgr_->DeleteDataByBundleName("testName");
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: QueryData
 * @tc.desc: QueryData
 * @tc.type: FUNC
 */
HWTEST_F(WindowPatternStartingWindowRdbTest, QueryData, TestSize.Level1)
{
    ASSERT_NE(startingWindowRdbMgr_, nullptr);
    StartingWindowRdbItemKey itemKey;
    itemKey.bundleName = "testName";
    StartingWindowInfo inputInfo, resInfo;
    inputInfo.backgroundColor_ = 0;
    resInfo.backgroundColor_ = 1;
    auto res = startingWindowRdbMgr_->QueryData(itemKey, resInfo);
    ASSERT_EQ(res, false);
    ASSERT_NE(inputInfo.backgroundColor_, resInfo.backgroundColor_);
    res = startingWindowRdbMgr_->InsertData(itemKey, inputInfo);
    ASSERT_EQ(res, true);
    res = startingWindowRdbMgr_->QueryData(itemKey, resInfo);
    ASSERT_EQ(res, true);
    ASSERT_EQ(inputInfo.backgroundColor_, resInfo.backgroundColor_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS