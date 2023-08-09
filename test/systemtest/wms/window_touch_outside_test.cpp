/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "singleton_container.h"
#include "wm_common.h"
#include "window_adapter.h"
#include "window_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
const int WAIT_CALLBACK_US = 10000;  // 10000 us

class WindowTouchOutsideTestListener : public ITouchOutsideListener {
public:
    void OnTouchOutside() const override
    {
        isTouchOutside_ = true;
    }
    mutable bool isTouchOutside_ { false };
};

class WindowTouchOutsideTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowTouchOutsideTestListener> windowlistener1_;
    static sptr<WindowTouchOutsideTestListener> windowlistener2_;
    Utils::TestWindowInfo firstWindowInfo_;
    Utils::TestWindowInfo secondWindowInfo_;
    Utils::TestWindowInfo thirdWindowInfo_;
};

sptr<WindowTouchOutsideTestListener> WindowTouchOutsideTest::windowlistener1_ =
    new WindowTouchOutsideTestListener();
sptr<WindowTouchOutsideTestListener> WindowTouchOutsideTest::windowlistener2_ =
    new WindowTouchOutsideTestListener();

void WindowTouchOutsideTest::SetUp()
{
        firstWindowInfo_ = {
        .name = "firstWindow",
        .rect = { 100, 100, 200, 200 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };

    secondWindowInfo_ = {
        .name = "secondWindow",
        .rect = { 400, 400, 200, 200 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };

    thirdWindowInfo_ = {
        .name = "thirdWindow",
        .rect = { 400, 400, 200, 200 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowTouchOutsideTest::TearDown()
{
    windowlistener1_->isTouchOutside_ = false;
    windowlistener2_->isTouchOutside_ = false;
}

void WindowTouchOutsideTest::SetUpTestCase()
{
}

void WindowTouchOutsideTest::TearDownTestCase()
{
}

namespace {
} // namespace
} // Rosen
} // OHOS