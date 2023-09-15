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

#include "anr_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t MAX_ANR_TIMER_COUNT { 64 };
class AnrManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<ANRManager> aNRManager_ = nullptr;
};

void AnrManagerTest::SetUpTestCase()
{
}

void AnrManagerTest::TearDownTestCase()
{
}

void AnrManagerTest::SetUp()
{
    aNRManager_ = new (std::nothrow) ANRManager();
    EXPECT_NE(nullptr, aNRManager_);
}

void AnrManagerTest::TearDown()
{
    aNRManager_ = nullptr;
}

namespace {
/**
 * @tc.name: Init
 * @tc.desc: Init Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, Init, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);
    
    int resultValue = 0;
    std::function<void()> Func = [this, &resultValue] {
        aNRManager_->Init();
        resultValue = 1;
    };

    Func();
    ASSERT_NE(1, resultValue);
}

/**
 * @tc.name: AddTimer01
 * @tc.desc: switcher_ is false
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, AddTimer01, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);
    
    int resultValue = 0;
    std::function<void()> Func = [this, &resultValue] {
        aNRManager_->switcher_ = false;
        aNRManager_->AddTimer(1, 1);
        resultValue = 1;
    };

    Func();
    ASSERT_NE(1, resultValue);
}

/**
 * @tc.name: AddTimer02
 * @tc.desc: anrTimerCount_ more than MAX_ANR_TIMER_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, AddTimer02, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);
    
    int resultValue = 0;
    std::function<void()> Func = [this, &resultValue] {
        aNRManager_->switcher_ = true;
        aNRManager_->anrTimerCount_ = MAX_ANR_TIMER_COUNT;
        aNRManager_->AddTimer(1, 1);
        resultValue = 1;
    };

    Func();
    ASSERT_NE(1, resultValue);
}

/**
 * @tc.name: AddTimer03
 * @tc.desc: others
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, AddTimer03, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);
    
    int resultValue = 0;
    std::function<void()> Func = [this, &resultValue] {
        aNRManager_->switcher_ = true;
        aNRManager_->anrTimerCount_ = MAX_ANR_TIMER_COUNT - 1;
        aNRManager_->AddTimer(1, 1);
        resultValue = 1;
    };

    Func();
    ASSERT_NE(1, resultValue);
}

/**
 * @tc.name: MarkProcessed
 * @tc.desc: MarkProcessed Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, MarkProcessed, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);
    
    int resultValue = 0;
    std::function<void()> Func = [this, &resultValue] {
        sptr<EventStage> eventStage_ = new EventStage();
        eventStage_->SaveANREvent(1, 111, 222);
        aNRManager_->MarkProcessed(1, 111);
        resultValue = 1;
    };

    Func();
    ASSERT_NE(1, resultValue);
}

/**
 * @tc.name: IsANRTriggered01
 * @tc.desc: IsANRTriggered Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, IsANRTriggered01, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);

    sptr<EventStage> eventStage_ = new EventStage();
    eventStage_->SetAnrStatus(1, true);

    ASSERT_EQ(true, aNRManager_->IsANRTriggered(1));
}

/**
 * @tc.name: IsANRTriggered02
 * @tc.desc: IsANRTriggered Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, IsANRTriggered02, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);

    ASSERT_EQ(false, aNRManager_->IsANRTriggered(1));
}

/**
 * @tc.name: SetAnrObserver
 * @tc.desc: SetAnrObserver Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, SetAnrObserver, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);

    int resultValue = 0;
    std::function<void(int32_t)> anrObserver =  [&resultValue](int32_t n)->void 
    {
        resultValue = 1;
    };

    aNRManager_->SetAnrObserver(anrObserver);
    ASSERT_EQ(1, resultValue);
}

/**
 * @tc.name: GetAppInfoByPersistentId01
 * @tc.desc: GetAppInfoByPersistentId Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, GetAppInfoByPersistentId01, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);

    std::string bundleName = "GetAppInfoByPersistentId";
    aNRManager_->SetApplicationInfo(1, 111, bundleName);
    ANRManager::AppInfo result = {111, "GetAppInfoByPersistentId"};
    ASSERT_EQ(result, aNRManager_->GetAppInfoByPersistentId(1));
}

/**
 * @tc.name: GetAppInfoByPersistentId02
 * @tc.desc: GetAppInfoByPersistentId Test
 * @tc.type: FUNC
 */
HWTEST_F(AnrManagerTest, GetAppInfoByPersistentId02, Function | MediumTest | Level2)
{
    ASSERT_NE(aNRManager_, nullptr);

    ANRManager::AppInfo result = ANRManager::AppInfo();
    ASSERT_EQ(result, aNRManager_->GetAppInfoByPersistentId(1));
}
} // namespace
} // namespace Rosen
} // namespace OHOS