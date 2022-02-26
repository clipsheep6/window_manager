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

#ifndef FRAMEWORKS_WM_TEST_UT_WINDOW_INNER_MANAGER_TEST_H
#define FRAMEWORKS_WM_TEST_UT_WINDOW_INNER_MANAGER_TEST_H

#include <gtest/gtest.h>

#include "window_inner_manager.h"

namespace OHOS {
namespace Rosen {
class WindowInnerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    DisplayId displayId_ = 0;
    static WindowInnerManager& innerManager_;
    static constexpr uint32_t SEND_MSG_SLEEP_S = 1; // send message sleep time
};
} // namespace ROSEN
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_WINDOW_INNER_MANAGER_TEST_H