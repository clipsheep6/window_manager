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

// gtest
#include <gtest/gtest.h>

#include "future.h"
#include "window_manager.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int WAIT_FUTURE_RESULT = 20000; // 20s
    constexpr int WAIT_SLEEP_TIME = 1; // 1s
}

class TestStatusBarEnabledChangedListener : public IStatusBarEnabledChangedListener {
public:
    void OnStatusBarEnabledUpdate(bool enable) override;

    RunnableFuture<bool> future_;
};

void TestStatusBarEnabledChangedListener::OnStatusBarEnabledUpdate(bool enable)
{
    future_.SetValue(enable);
}

class StatusBarEnabledTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<TestStatusBarEnabledChangedListener> lisenter_;
};

sptr<TestStatusBarEnabledChangedListener> StatusBarEnabledTest::lisenter_ = nullptr;
void StatusBarEnabledTest::SetUpTestCase()
{
    lisenter_= new (std::nothrow)TestStatusBarEnabledChangedListener();
    if (lisenter_ == nullptr) {
        return;
    }
}

void StatusBarEnabledTest::TearDownTestCase()
{
}

void StatusBarEnabledTest::SetUp()
{
}

void StatusBarEnabledTest::TearDown()
{
}

namespace {
/**
* @tc.name: SetStatusBarEnabled
* @tc.desc: Check status bar enabled
* @tc.type: FUNC
*/
HWTEST_F(StatusBarEnabledTest, SetStatusBarEnabled, Function | MediumTest | Level1)
{
    ASSERT_NE(lisenter_, nullptr);

    auto& windowManager =  WindowManager::GetInstance();
    windowManager.SetStatusBarEnabled(false);
    sleep(WAIT_SLEEP_TIME);

    windowManager.RegisterStatusBarEnabledChangedListener(lisenter_);
    sleep(WAIT_SLEEP_TIME);
    windowManager.SetStatusBarEnabled(true);
    auto result = lisenter_->future_.GetResult(WAIT_FUTURE_RESULT);
    ASSERT_EQ(result, true);
    lisenter_->future_.Reset(true);

    windowManager.SetStatusBarEnabled(false);
    result = lisenter_->future_.GetResult(WAIT_FUTURE_RESULT);
    ASSERT_EQ(result, false);
    lisenter_->future_.Reset(false);

    windowManager.UnregisterStatusBarEnabledChangedListener(lisenter_);
    sleep(WAIT_SLEEP_TIME);
}
}
} // namespace Rosen
} // namespace OHOS

