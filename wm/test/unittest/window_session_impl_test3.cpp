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

#include "window_session_impl.h"

#include <gtest/gtest.h>

#include "display_info.h"
#include "ability_context_impl.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTwoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowSessionImpl> window_;
};

void WindowSessionImplTwoTest::SetUpTestCase()
{
}

void WindowSessionImplTwoTest::TearDownTestCase()
{
}

void WindowSessionImplTwoTest::SetUp()
{
}

void WindowSessionImplTwoTest::TearDown()
{
    if (window_ != nullptr) {
        window_->Destroy();
    }
}

namespace {
sptr<WindowSessionImpl> GetTestWindowImpl(const std::string& name)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    option->SetWindowName(name);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    if (window == nullptr) {
        return nullptr;
    }

    SessionInfo sessionInfo = {name, name, name};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    if (session == nullptr) {
        return nullptr;
    }

    window->hostSession_ = session;
    return window;
}

/**
 * @tc.name: GetTitleButtonVisible
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTwoTest, GetTitleButtonVisible, Function | SmallTest | Level2)
{

}

}
} // namespace Rosen
} // namespace OHOS