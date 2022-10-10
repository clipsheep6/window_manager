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

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "state_machine_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class TestFSM : public StateMachine<TestFSMDef> {
public:
    void Action2(Event2 const& ev) override
    {
        TestFSMDef::Action2(ev);
        ProcessEvent(Event3());
    }
};
class TestFSMMock : public TestFSM {
public:
    MOCK_METHOD1(Action1, void(Event1 const&));
    MOCK_METHOD1(Action3, void(Event4 const&));
    MOCK_METHOD1(Action4, void(Event5 const&));
};


class StateMachineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void StateMachineTest::SetUpTestCase()
{
}

void StateMachineTest::TearDownTestCase()
{
}

void StateMachineTest::SetUp()
{
}

void StateMachineTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ProcessEvent
 * @tc.desc: ProcessEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(StateMachineTest, ProcessEvent, Function | SmallTest | Level2)
{
    TestFSMMock machine;
    machine.Start(); // enter State1
    ASSERT_EQ(true, machine.GetCurStateId() == TestFSM::GetIdOfState<State1>());
    EXPECT_CALL(machine, Action1(_)).Times(1);
    machine.ProcessEvent(Event1()); // State1 -> State2 (Action1)
    ASSERT_EQ(true, machine.GetCurStateId() == TestFSM::GetIdOfState<State2>());
    ASSERT_EQ(0, machine.GetState<State4>()->a);
    machine.ProcessEvent(Event2()); // State2 -> State3 (Action2) -> State4
    ASSERT_EQ(true, machine.GetCurStateId() == TestFSM::GetIdOfState<State4>());
    ASSERT_EQ(1, machine.GetState<State4>()->a);
    machine.ProcessEvent(Event1()); // Guard2 return false, no transition
    ASSERT_EQ(true, machine.GetCurStateId() == TestFSM::GetIdOfState<State4>());
    EXPECT_CALL(machine, Action4(_)).Times(1);
    machine.ProcessEvent(Event5()); // internal transition
    ASSERT_EQ(2, machine.GetState<State4>()->a);
    EXPECT_CALL(machine, Action3(_)).Times(1);
    machine.ProcessEvent(Event4()); // State4 -> State5 -> State6 -> State1
    ASSERT_EQ(true, machine.GetCurStateId() == TestFSM::GetIdOfState<State1>());
    machine.ProcessEvent(Event2()); // no transition
    ASSERT_EQ(true, machine.GetCurStateId() == TestFSM::GetIdOfState<State1>());
}
}
}
}