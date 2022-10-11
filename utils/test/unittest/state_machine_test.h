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
#ifndef OHOS_WM_STATE_MACHINE_TEST_H
#define OHOS_WM_STATE_MACHINE_TEST_H
#include "state_machine.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "StateMachine"};
using namespace FSM;
// define states
struct State1 : StateBase {
    template <class Event, class FSM>
    void OnEntry(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on entry State1");
    }
    template <class Event, class FSM>
    void OnExit(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on exit State1");
    }
};

struct State2 : StateBase {
    template <class Event, class FSM>
    void OnEntry(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on entry State2");
    }
    template <class Event, class FSM>
    void OnExit(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on exit State2");
    }
};

struct State3 : StateBase {
    template <class Event, class FSM>
    void OnEntry(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on entry State3");
    }
    template <class Event, class FSM>
    void OnExit(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on exit State3");
    }
};

struct State4 : StateBase {
    template <class Event, class FSM>
    void OnEntry(Event const& e, FSM& machine)
    {
        a++;
        WLOGFI("[FSM] on entry State4");
    }
    template <class Event, class FSM>
    void OnExit(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on exit State4");
    }
    int a = 0;
};

struct State5 : StateBase {
    template <class Event, class FSM>
    void OnEntry(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on entry State5");
    }
    template <class Event, class FSM>
    void OnExit(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on exit State5");
    }
};

struct State6 : StateBase {
    template <class Event, class FSM>
    void OnEntry(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on entry State6");
    }
    template <class Event, class FSM>
    void OnExit(Event const& e, FSM& machine)
    {
        WLOGFI("[FSM] on exit State6");
    }
};

// define events
struct Event1 {};
struct Event2 {};
struct Event3 {};
struct Event4 {};
struct Event5 {};

struct TestFSMDef : StateMachineDef<TestFSMDef> {
    virtual void Action1(Event1 const& ev)
    {
        WLOGFI("[FSM] on action1");
    }
    virtual void Action2(Event2 const& ev)
    {
        WLOGFI("[FSM] on action2");
    }
    virtual void Action3(Event4 const& ev)
    {
        WLOGFI("[FSM] on action3");
    }
    virtual void Action4(Event5 const& ev)
    {
        WLOGFI("[FSM] on action4");
    }
    bool Guard1(Event1 const& ev)
    {
        return true;
    }
    bool Guard2(Event1 const& ev)
    {
        return false;
    }
    struct TransitionTable : TransitionVector
        <
        row<State1, Event1, State2, &TestFSMDef::Action1, &TestFSMDef::Guard1>, // define row with action and guard.
        a_row<State2, Event2, State3, &TestFSMDef::Action2>, // define row with action.
        _row<State3, Event3, State4>, // define row with no action and no guard.
        a_row<State4, Event4, State5, &TestFSMDef::Action3>,
        a_irow<State4, Event5, &TestFSMDef::Action4>,
        _row<State5, None, State6>,
        _row<State6, None, State1>,
        g_row<State4, Event1, State1, &TestFSMDef::Guard2> // define row with guard.
        > {};

    typedef State1 InitialState;
};
/* PlantUML code of the state machine
@startuml
[*] --> State1
State1 --> State2 : Event1
State1 : Initial state
State2 --> State3 : Event2
State3 --> State4 : Event3
State4 --> State5 : Event4
State4 --> State4 : Event5
State5 -[#green]> State6
State6 -[#green]> State1
State4 -[dashed]-> State1 : Event1
@enduml
*/
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WM_STATE_MACHINE_TEST_H