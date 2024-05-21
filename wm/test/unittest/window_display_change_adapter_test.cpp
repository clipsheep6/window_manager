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

#include "window_display_change_adapter.h"
#include "window_manager_hilog.h"
#include "iremote_object_mocker.h"

#include "window_display_change_adapter.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class TestDisplayInfoChangedListener : public IDisplayInfoChangedListener {
public:
    void OnDisplayInfoChange(const sptr<IRemoteObject>& token, DisplayId displayId, float density,
        DisplayOrientation orientation) override
    {
        TLOGI(WmsLogTag::DMS, "TestDisplayInfoChangedListener");
    }
};

class WindowDisplayChangeAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void WindowDisplayChangeAdapterTest::SetUpTestCase()
{
}

void WindowDisplayChangeAdapterTest::TearDownTestCase()
{
}

void WindowDisplayChangeAdapterTest::SetUp()
{
}

void WindowDisplayChangeAdapterTest::TearDown()
{
}

namespace {
/**
 *@tc.name: OnChange
 *@tc.desc: Window Display Information Change
 *@tc.type: FUNC
 */
HWTEST_F(WindowDisplayChangeAdapterTest, OnChange, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> targetToken = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, targetToken);
    sptr<IDisplayInfoChangedListener> listener = new (std::nothrow) TestDisplayInfoChangedListener();
    ASSERT_NE(nullptr, listener);
    sptr<WindowDisplayChangeAdapter> window = new WindowDisplayChangeAdapter(targetToken, listener);
    ASSERT_NE(nullptr, window);
    DisplayId displayId = 0;
    int res = 0;
    window->OnChange(displayId);
    ASSERT_EQ(0, res);
}

/**
 *@tc.name: OnDisplayInfoChange
 *@tc.desc: notify Display Information Change
 *@tc.type: FUNC
 */
HWTEST_F(WindowDisplayChangeAdapterTest, OnDisplayInfoChange, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> targetToken = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(nullptr, targetToken);
    sptr<IDisplayInfoChangedListener> listener = new (std::nothrow) TestDisplayInfoChangedListener();
    ASSERT_NE(nullptr, listener);
    sptr<WindowDisplayChangeAdapter> window = new WindowDisplayChangeAdapter(targetToken, listener);
    ASSERT_NE(nullptr, window);

    DisplayId displayId = 0;
    float density = 0.2f;
    DisplayOrientation orientation = DisplayOrientation::UNKNOWN;

    int res = 0;
    window->OnDisplayInfoChange(targetToken, displayId, density, orientation);
    // not need notify display info, data is not changed
    window->OnDisplayInfoChange(targetToken, displayId, density, orientation);
    ASSERT_EQ(0, res);
}

}
} // namespace Rosen
} // namespace OHOS