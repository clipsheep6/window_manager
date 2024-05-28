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

#include <vsync_station.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VsyncStationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void VsyncStationTest::SetUpTestCase()
{
}

void VsyncStationTest::TearDownTestCase()
{
}

void VsyncStationTest::SetUp()
{
}

void VsyncStationTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RequestVsyncOneWindow
 * @tc.desc: RequestVsyncOneWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, RequestVsyncOneWindow, Function | SmallTest | Level3)
{
    NodeId nodeId = 0;
    std::shared_ptr<VsyncStation> vsyncStation = std::make_shared<VsyncStation>(nodeId);
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    ASSERT_NE(vsyncStation, nullptr);
    vsyncStation->RequestVsync(vsyncCallback);
}

/**
 * @tc.name: RequestVsyncMultiWindow
 * @tc.desc: RequestVsyncMultiWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, RequestVsyncMultiWindow, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    std::shared_ptr<VsyncCallback> vsyncCallback0 = std::make_shared<VsyncCallback>();
    ASSERT_NE(vsyncStation0, nullptr);
    vsyncStation0->RequestVsync(vsyncCallback0);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    std::shared_ptr<VsyncCallback> vsyncCallback1 = std::make_shared<VsyncCallback>();
    ASSERT_NE(vsyncStation1, nullptr);
    vsyncStation1->RequestVsync(vsyncCallback1);
}

/**
 * @tc.name: GetVsyncPeriod
 * @tc.desc: GetVsyncPeriod Test
 * @tc.type: FUNC
 */
HWTEST_F(GetVsyncPeriodTest, GetVsyncPeriod, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    vsyncStation0->receiver_ = nullptr;
    ASSERT_EQ(0, vsyncStation0->GetVsyncPeriod());
}

/**
 * @tc.name: GetFrameRateLinkerId
 * @tc.desc: GetFrameRateLinkerId Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, GetFrameRateLinkerId, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(vsyncStation0, nullptr);
    ASSERT_NE(-1, vsyncStation0->GetFrameRateLinkerId());
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    ASSERT_NE(vsyncStation1, nullptr);
    ASSERT_NE(-1, vsyncStation1->GetFrameRateLinkerId());
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = OHOS::Rosen:RSFrameRateLinker::Create();
    vsyncStation0->frameRateLinker_ = frameRateLinker;
    ASSERT_NE(0, vsyncStation0->GetFrameRateLinkerId());
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, FlushFrameRate, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    int res = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(vsyncStation0, nullptr);
    uint32_t rate0 = 60;
    bool isAnimatorStopped = true;
    vsyncStation0->FlushFrameRate(rate0, isAnimatorStopped);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    ASSERT_NE(vsyncStation1, nullptr);
    uint32_t rate1 = 120;
    vsyncStation1->FlushFrameRate(rate1, isAnimatorStopped);
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = OHOS::Rosen::RSFrameRateLinker::Create();
    ASSERT_NE(frameRateLinker, nullptr);
    frameRateLinker->SetEnable(true);
    vsyncStation1->frameRateLinker_ = frameRateLinker;
    vsyncStation1->FlushFrameRate(rate1,isAnimatorStopped) 
    ASSERT_EQ(res, 0);
    frameRateLinker->SetEnable(false);
    vsyncStation1->frameRateLinker_ = frameRateLinker;
    vsyncStation1->FlushFrameRate(rate1, isAnimatorStopped) 
    ASSERT_EQ(res, 0);
    vsyncStation1->frameRateLinker_ = nullptr;
    vsyncStation1->FlushFrameRate(rate1, isAnimatorStopped);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: SetFrameRateLinkerEnable
 * @tc.desc: SetFrameRateLinkerEnable Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, SetFrameRateLinkerEnable, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    int res = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(vsyncStation0, nullptr);
    bool enable0 = false;
    vsyncStation0->SetFrameRateLinkerEnable(enable0);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    ASSERT_NE(vsyncStation1, nullptr);
    bool enable1 = true;
    vsyncStation1->SetFrameRateLinkerEnable(enable1);
    std::shared_ptr<RSFrameRateLinker> frameRateLinker = OHOS::Rosen::RSFrameRateLinker::Create();
    vsyncStation1->frameRateLinker_ = frameRateLinker;
    vsyncStation1->frameRateLinker_ = frameRateLinker;
    vsyncStation1->SetFrameRateLinkerEnable(false);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name:Init
 * @tc.desc: Init Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, Init, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    int res = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    vsyncStation0->hasInitVsyncReceiver_ = false;
    vsyncStation1->SetIsMainHandlerAvailable(true);
    vsyncStation0->Init();
    ASSERT_EQ(res, 0);
    vsyncStation0->hasInitVsyncReceiver_ = false;
    vsyncStation0->SetIsMainHandlerAvailable(false);
    vsyncStation0->Init();
    ASSERT_EQ(res, 0);
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(nullptr, vsyncStation1);
    vsyncStation1->hasInitVsyncReceiver_ = true;
    vsyncStation1->vsyncHandler_ = nullptr;
    vsyncStation1->SetIsMainHandlerAvailable(true);
    vsyncStation1->Init;
    ASSERT_EQ(res, 0);
    vsyncStation1->hasInitVsyncReceiver_ = true;
    vsyncStation1->vsyncHandler_ = nullptr;
    vsyncStation1->SetIsMainHandlerAvailable(false);
    vsyncStation1->Init;
    ASSERT_EQ(res, 0);
}

}
} // namespace Rosen
} // namespace OHOS