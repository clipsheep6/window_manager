/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include <functional>
#include "display_manager_agent_proxy.h"
#include "display_manager_agent_default.h"
#include "display_manager_adapter.h"

#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class DisplayManagerAgentProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy_;
};

void DisplayManagerAgentProxyTest::SetUpTestCase()
{
}

void DisplayManagerAgentProxyTest::TearDownTestCase()
{
}

void DisplayManagerAgentProxyTest::SetUp()
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    displayManagerAgentProxy_ = new DisplayManagerAgentProxy(impl);
}

void DisplayManagerAgentProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: NotifyDisplayPowerEvent
 * @tc.desc: NotifyDisplayPowerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayPowerEvent, Function | SmallTest | Level1)
{
    DisplayPowerEvent event = DisplayPowerEvent::DESKTOP_READY;
    EventStatus status = EventStatus::BEGIN;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->NotifyDisplayPowerEvent(event, status);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}
 

/**
 * @tc.name: OnScreenConnect
 * @tc.desc: OnScreenConnect
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect, Function | SmallTest | Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;


    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnScreenConnect(screenInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}


/**
 * @tc.name: OnScreenDisconnect
 * @tc.desc: OnScreenDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenDisconnect, Function | SmallTest | Level1)
{
    ScreenId screenId = 0;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnScreenDisconnect(screenId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenChange
 * @tc.desc: OnScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange, Function | SmallTest | Level1)
{
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnScreenChange(screenInfo, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenGroupChange
 * @tc.desc: OnScreenGroupChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenGroupChange, Function | SmallTest | Level1)
{
    std::string trigger = " ";
    std::vector<sptr<ScreenInfo>> screenInfos = {};
    ScreenGroupChangeEvent event = ScreenGroupChangeEvent::CHANGE_GROUP;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnScreenGroupChange(trigger, screenInfos, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayCreate
 * @tc.desc: OnDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnDisplayCreate(displayInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayDestroy, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnDisplayDestroy(displayId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnDisplayChange(displayInfo, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenshot
 * @tc.desc: OnScreenshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenshot, Function | SmallTest | Level1)
{
    sptr<ScreenshotInfo> snapshotInfo = new ScreenshotInfo();

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->OnScreenshot(snapshotInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyPrivateWindowStateChanged
 * @tc.desc: NotifyPrivateWindowStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateWindowStateChanged, Function | SmallTest | Level1)
{
    bool hasPrivate = false;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->NotifyPrivateWindowStateChanged(hasPrivate);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyFoldStatusChanged
 * @tc.desc: NotifyFoldStatusChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyFoldStatusChanged, Function | SmallTest | Level1)
{

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy_->NotifyFoldStatusChanged(FoldStatus::EXPAND);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}
}
}
}