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
#ifndef OHOS_STATE_MACHINE_H
#define OHOS_STATE_MACHINE_H
#include <queue>
namespace OHOS {
namespace Rosen {
namespace FSM {
// tags
struct a_row_tag {};
struct g_row_tag {};
struct _row_tag {};
struct row_tag {};
// tags for internal transitions
struct a_irow_tag {};
struct g_irow_tag {};
struct _irow_tag {};
struct irow_tag {};

struct None {};
template<class Row>
constexpr bool HasGuard()
{
    return std::is_same<typename Row::row_type_tag, row_tag>::value ||
        std::is_same<typename Row::row_type_tag, g_row_tag>::value ||
        std::is_same<typename Row::row_type_tag, irow_tag>::value ||
        std::is_same<typename Row::row_type_tag, g_irow_tag>::value;
}

template<class Row>
constexpr bool HasAction()
{
    return std::is_same<typename Row::row_type_tag, row_tag>::value ||
        std::is_same<typename Row::row_type_tag, a_row_tag>::value ||
        std::is_same<typename Row::row_type_tag, irow_tag>::value ||
        std::is_same<typename Row::row_type_tag, a_irow_tag>::value;
}

template <class State, class U, class... Us>
struct get_state_id {
    constexpr static int value = get_state_id<State, Us...>::value == -1 ?
        (get_state_id<State, U>::value == -1 ? -1 : get_state_id<State, Us...>::end + get_state_id<State, U>::value) :
        get_state_id<State, Us...>::value;
    constexpr static int end = get_state_id<State, Us...>::end + 2;
};

template <class State, class U>
struct get_state_id<State, U> {
    constexpr static int value = std::is_same<State, typename U::Source>::value ? 0 :
        (std::is_same<State, typename U::Target>::value ? 1 : -1);
    constexpr static int end = 2;
};

struct StateBase {
    template < class Event, class FSM >
    void OnExit(Event const& evt, FSM& fsm) {}
    template < class Event, class FSM >
    void OnEntry(Event const& evt, FSM& fsm) {}
};

template<typename... Rows>
struct TransitionVector {};

template<>
struct TransitionVector<> {
    template<class FSM>
    bool ConsumeEvent(FSM& fsm, int& stateId)
    {
        return false;
    }
    template<class FSM, bool = true>
    struct EventHandler {
        static bool HandleNoneEvent(FSM& fsm, int& stateId)
        {
            return false;
        }
    };
    template<class Event>
    void PostEvent(const Event&) {}
    template<typename State>
    State* GetState()
    {
        return nullptr;
    }
    bool NoEvent()
    {
        return true;
    }
};

template<typename FirstRow, typename... RestRows>
struct TransitionVector<FirstRow, RestRows...> : TransitionVector<RestRows...> {
    // event and priority queue
    std::queue<std::pair<typename FirstRow::Evt, int>> eventQueue_;
    typename FirstRow::Source source_;
    typename FirstRow::Target target_;
    template<typename State>
    static constexpr int GetStateId()
    {
        return get_state_id<State, FirstRow, RestRows...>::value;
    }
    template<typename State>
    State* GetState()
    {
        return TransitionVector<RestRows...>::template GetState<State>();
    }
    template<>
    typename FirstRow::Source* GetState()
    {
        return &source_;
    }
    template<>
    typename FirstRow::Target* GetState()
    {
        return &target_;
    }
    template<class FSM>
    bool ConsumeEvent(FSM& fsm, int& stateId)
    {
        if (eventQueue_.empty()) {
            return TransitionVector<RestRows...>::ConsumeEvent(fsm, stateId);
        }
        bool stateChanged = false;
        std::queue<std::pair<typename FirstRow::Evt, int>> tmp;
        while (!eventQueue_.empty()) {
            auto evt = eventQueue_.front().first;
            auto pivolity = eventQueue_.front().second;
            if (get_state_id<typename FirstRow::Source, FirstRow, RestRows...>::value != stateId) {
                eventQueue_.pop();
                continue;
            }
            if (pivolity == 0) {
                if (!FirstRow::GuardCall(fsm, evt)) {
                    eventQueue_.pop();
                    continue;
                }
                if (auto source = fsm.template GetState<typename FirstRow::Source>()) {
                    source->OnExit(evt, fsm);
                } else {
                    fsm.ExceptionCaught(evt, fsm);
                }
                FirstRow::ActionCall(fsm, evt);
                if (auto target = fsm.template GetState<typename FirstRow::Target>()) {
                    target->OnEntry(evt, fsm);
                } else {
                    fsm.ExceptionCaught(evt, fsm);
                }
                if (get_state_id<typename FirstRow::Target, FirstRow, RestRows...>::value != stateId) {
                    stateChanged = true;
                }
                stateId = get_state_id<typename FirstRow::Target, FirstRow, RestRows...>::value;
            } else if (pivolity > 0) {
                tmp.emplace(evt, pivolity - 1);
            }
            eventQueue_.pop();
        }
        eventQueue_ = std::move(tmp);
        return stateChanged || TransitionVector<RestRows...>::ConsumeEvent(fsm, stateId);
    }
    template<class FSM, bool IsNoneEvent = std::is_same<None, typename FirstRow::Evt>::value>
    struct EventHandler {
        static bool HandleNoneEvent(FSM& fsm, int& stateId)
        {
            return false;
        }
    };
    template<class FSM>
    struct EventHandler<FSM, true> {
        static bool HandleNoneEvent(FSM& fsm, int& stateId)
        {
            bool stateChanged = false;
            None evt;
            if (get_state_id<typename FirstRow::Source, FirstRow, RestRows...>::value == stateId &&
                FirstRow::GuardCall(fsm, evt)) {
                if (auto source = fsm.template GetState<typename FirstRow::Source>()) {
                    source->OnExit(evt, fsm);
                } else {
                    fsm.ExceptionCaught(evt, fsm);
                }
                FirstRow::ActionCall(fsm, evt);
                if (auto target = fsm.template GetState<typename FirstRow::Target>()) {
                    target->OnEntry(evt, fsm);
                } else {
                    fsm.ExceptionCaught(evt, fsm);
                }
                if (get_state_id<typename FirstRow::Target, FirstRow, RestRows...>::value != stateId) {
                    stateChanged = true;
                } else {
                    fsm.ExceptionCaught(evt, fsm);
                }
                stateId = get_state_id<typename FirstRow::Target, FirstRow, RestRows...>::value;
            }
            return stateChanged ||
                TransitionVector<RestRows...>::template EventHandler<FSM>::HandleNoneEvent(fsm, stateId);
        }
    };
    template<class FSM>
    struct EventHandler<FSM, false> {
        static bool HandleNoneEvent(FSM& fsm, int& stateId)
        {
            return  TransitionVector<RestRows...>::template EventHandler<FSM>::HandleNoneEvent(fsm, stateId);
        }
    };
    template<class Event>
    void PostEvent(const Event& evt, int pivolity)
    {
        TransitionVector<RestRows...>::PostEvent(evt, pivolity);
    }
    template<>
    void PostEvent(const typename FirstRow::Evt& evt, int pivolity)
    {
        eventQueue_.emplace(evt, pivolity);
    }
    bool NoEvent()
    {
        return eventQueue_.empty() && TransitionVector<RestRows...>::NoEvent();
    }
};

template<class Derived>
struct StateMachineDef {
    template<
        typename T1,
        class Event,
        typename T2,
        void (Derived::* action)(Event const&)
    >
    struct a_row {
        typedef a_row_tag row_type_tag;
        typedef T1 Source;
        typedef T2 Target;
        typedef Event Evt;
        template <class FSM>
        static bool ActionCall(FSM& fsm, Event const& evt)
        {
            (fsm.*action)(evt);
            return true;
        }
        template <class FSM>
        static constexpr bool GuardCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
    };
    template<
        typename T1,
        class Event,
        typename T2
    >
    struct _row {
        typedef _row_tag row_type_tag;
        typedef T1 Source;
        typedef T2 Target;
        typedef Event Evt;
        template <class FSM>
        static constexpr bool ActionCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
        template <class FSM>
        static constexpr bool GuardCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
    };
    template<
        typename T1,
        class Event,
        typename T2,
        void (Derived::* action)(Event const&),
        bool (Derived::* guard)(Event const&)
    >
    struct row {
        typedef row_tag row_type_tag;
        typedef T1 Source;
        typedef T2 Target;
        typedef Event Evt;
        template <class FSM>
        static bool ActionCall(FSM& fsm, Event const& evt)
        {
            (fsm.*action)(evt);
            return true;
        }
        template <class FSM>
        static bool GuardCall(FSM& fsm, Event const& evt)
        {
            return (fsm.*guard)(evt);
        }
    };
    template<
        typename T1,
        class Event,
        typename T2,
        bool (Derived::* guard)(Event const&)
    >
    struct g_row {
        typedef g_row_tag row_type_tag;
        typedef T1 Source;
        typedef T2 Target;
        typedef Event Evt;
        template <class FSM>
        static constexpr bool ActionCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
        template <class FSM>
        static bool GuardCall(FSM& fsm, Event const& evt)
        {
            return (fsm.*guard)(evt);
        }
    };
    // internal transitions
    template<
        typename T1,
        class Event,
        void (Derived::* action)(Event const&)
    >
    struct a_irow {
        typedef a_irow_tag row_type_tag;
        typedef T1 Source;
        typedef T1 Target;
        typedef Event Evt;
        template <class FSM>
        static bool ActionCall(FSM& fsm, Event const& evt)
        {
            (fsm.*action)(evt);
            return true;
        }
        template <class FSM>
        static constexpr bool GuardCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
    };
    template<
        typename T1,
        class Event,
        void (Derived::* action)(Event const&),
        bool (Derived::* guard)(Event const&)
    >
    struct irow {
        typedef irow_tag row_type_tag;
        typedef T1 Source;
        typedef T1 Target;
        typedef Event Evt;
        template <class FSM>
        static bool ActionCall(FSM& fsm, Event const& evt)
        {
            (fsm.*action)(evt);
            return true;
        }
        template <class FSM>
        static bool GuardCall(FSM& fsm, Event const& evt)
        {
            return (fsm.*guard)(evt);
        }
    };
    template<
        typename T1,
        class Event,
        bool (Derived::* guard)(Event const&)
    >
    struct g_irow {
        typedef g_irow_tag row_type_tag;
        typedef T1 Source;
        typedef T1 Target;
        typedef Event Evt;
        
        template <class FSM>
        static constexpr bool ActionCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
        template <class FSM>
        static bool GuardCall(FSM& fsm, Event const& evt)
        {
            return (fsm.*guard)(evt);
        }
    };
    // internal row withou action or guard. Does nothing except forcing the event to be ignored.
    template<
        typename T1,
        class Event
    >
    struct _irow {
        typedef _irow_tag row_type_tag;
        typedef T1 Source;
        typedef T1 Target;
        typedef Event Evt;
        template <class FSM>
        static constexpr bool ActionCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
        template <class FSM>
        static constexpr bool GuardCall(FSM& fsm, Event const& evt)
        {
            return true;
        }
    };
    // Default no-transition handler. Can be replaced in the Derived SM class.
    template <class FSM, class Event>
    void NoTransition(Event const&, FSM&)
    {
    }
    // Default exception handler. Can be replaced in the Derived SM class.
    template <class FSM, class Event>
    void ExceptionCaught(Event const&, FSM&)
    {
    }
};

template<class A>
class StateMachine : public A {
public:
    StateMachine() : curStateId_(A::TransitionTable::template GetStateId<typename A::InitialState>())
    {
    }
    void Start()
    {
        if (auto state = GetState<typename A::InitialState>()) {
            state->OnEntry(None(), *this);
        }
        while (A::TransitionTable::template EventHandler<StateMachine<A>>::HandleNoneEvent(*this, curStateId_));
    }
    template<typename State>
    State* GetState()
    {
        return table_.template GetState<State>();
    }
    template<typename State>
    static constexpr int GetIdOfState()
    {
        return A::TransitionTable::template GetStateId<State>();
    }
    int GetCurStateId()
    {
        return curStateId_;
    }
    template<class Event>
    void ProcessEvent(Event const& evt)
    {
        if (!table_.NoEvent()) {
            PostEvent(evt);
            return;
        }
        PostEvent(evt);
        while (!table_.NoEvent()) {
            if (table_.ConsumeEvent(*this, curStateId_)) {
                while (A::TransitionTable::template EventHandler<StateMachine<A>>::HandleNoneEvent(*this, curStateId_));
            }
        }
        // After process event, event queue should be empty, so reset the priority.
        priority_ = 0;
    }
private:
    int curStateId_;
    template<class Event>
    void PostEvent(Event const& evt)
    {
        table_.PostEvent(evt, priority_++);
    }
    typename A::TransitionTable table_;
    int priority_ = 0;
};
} // namespace FSM
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_STATE_MACHINE_H

