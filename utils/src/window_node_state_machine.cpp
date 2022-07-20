/*
 * Copyright (c) 2022Huawei Device Co., Ltd.
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

#include "window_node_state_machine.h"

#include "parameters.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeStateMachine"};
} // namespace

WindowNodeStateMachine::WindowNodeStateMachine()
{
    animationFirst_ = system::GetParameter("persist.window.af.enabled", "0") == "1";
    WLOGFI("animationFirst_: %{public}d", animationFirst_);
    taskCount_.store(0);
}

WindowNodeStateMachine::~WindowNodeStateMachine()
{
    taskCount_.store(0);
}

bool WindowNodeStateMachine::IsValidTask(std::string taskName)
{
    if (currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING && (taskName == "Hide" || taskName == "Destroy")) {
        return true;
    }
    if ((currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING) && (taskName == "Show")) {
        return true;
    }
    return false;
}

bool WindowNodeStateMachine::GetFinalStateTask(StateTask& task)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (stateTaskQ_.empty()) {
        WLOGFI("No state task need to be executed in task queue!");
        return false;
    }
    WLOGFI("There is %{public}u tasks need to be executed in task queue!", static_cast<uint32_t>(stateTaskQ_.size()));
    // task = stateTaskQ_.back().second;
    bool findFinalTask = false;
    for (auto testTask : stateTaskQ_) {
        WLOGFI("chy current State:%{public}u, task %{public}s", static_cast<uint32_t>(currState_), testTask.first.c_str());
    }
    for (auto iter = stateTaskQ_.rbegin(); iter != stateTaskQ_.rend(); ++iter) {
        WLOGFI("current State:%{public}u, task %{public}s", static_cast<uint32_t>(currState_), iter->first.c_str());
        if (IsValidTask(iter->first)) {
            WLOGFI("GetFinalTask %{public}s", iter->first.c_str());
            task = iter->second;
            findFinalTask = true;
            break;
        }
    }
    stateTaskQ_.clear();
    WLOGFI("GetFinalTask num %{public}lu", stateTaskQ_.size());
    return findFinalTask;
}

void WindowNodeStateMachine::UpdateStateTaskQueue(std::string event, const StateTask& task)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    stateTaskQ_.push_back(std::make_pair(event, task));
    WLOGFI("after UpdateStateTaskQueue num %{public}lu", stateTaskQ_.size());
}

void WindowNodeStateMachine::TransitionTo(WindowNodeState state)
{
    if (!animationFirst_) {
        WLOGFD("not enable animation first");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    // lastState_ = currState_;
    currState_ = state;
    stateRecord_.push_back(currState_);
    PrintHistoryState();
    WLOGFD("after PrintHistoryState currentState:%{public}u", static_cast<uint32_t>(currState_));
}

void WindowNodeStateMachine::UpdateAnimationTaskCount(bool isAdd)
{
    WLOGFI("animationFirst_: %{public}d", animationFirst_);
    if (!animationFirst_) {
        return;
    }
    PrintHistoryState();
    if (isAdd) {
        taskCount_.fetch_add(1, std::memory_order_seq_cst);
        WLOGFI("after add UpdateAnimationTaskCount: %{public}d", GetAnimationCount());
    } else {
        taskCount_.fetch_sub(1, std::memory_order_seq_cst);
        WLOGFI("after sub UpdateAnimationTaskCount: %{public}d", GetAnimationCount());
    }
}

int32_t WindowNodeStateMachine::GetAnimationCount()
{
    return taskCount_.load();
}

bool WindowNodeStateMachine::IsWindowNodeShownOrShowing()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING ||
        currState_ == WindowNodeState::SHOW_ANIMATION_DONE || currState_ == WindowNodeState::SHOWN) {
            return true; // not play show animation again when
    }
    return false;
}

bool WindowNodeStateMachine::IsRemoteAnimationPlaying()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING ||
        currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING) {
            return true;
    }
    return false;
}

bool WindowNodeStateMachine::IsShowAnimationPlaying()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return currState_ == WindowNodeState::SHOW_ANIMATION_PLAYING;
}

bool WindowNodeStateMachine::IsHideAnimationPlaying()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING;
}

bool WindowNodeStateMachine::IsWindowNodeHiddenOrHiding()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (currState_ == WindowNodeState::HIDE_ANIMATION_PLAYING ||
        currState_ == WindowNodeState::HIDE_ANIMATION_DONE || currState_ == WindowNodeState::HIDDEN) {
            return true; // not play show animation again when
    }
    return false;
}

WindowNodeState WindowNodeStateMachine::GetCurrentState()
{
    return currState_;
}

// WindowNodeState WindowNodeStateMachine::GetLastState()
// {
//     return lastState_;
// }

void WindowNodeStateMachine::PrintHistoryState()
{
    for (auto state : stateRecord_) {
        WLOGFD("windowId:%{public}u history state: %{public}d", windowId_, static_cast<uint32_t>(state));
    }
}
} // Rosen
} // OHOS