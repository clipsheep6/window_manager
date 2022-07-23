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
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeStateMachine"};
} // namespace

WindowNodeStateMachine::WindowNodeStateMachine()
{
    taskCount_.store(0);
}

WindowNodeStateMachine::~WindowNodeStateMachine()
{
    taskCount_.store(0);
}

bool WindowNodeStateMachine::GetFinalStateTask(StateTask& task)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (stateTaskQ_.empty()) {
        WLOGFI("No state task need to be executed in task queue!");
        return false;
    }
    WLOGFI("There is %{public}u tasks need to be executed in task queue!", static_cast<uint32_t>(stateTaskQ_.size()));
    task = stateTaskQ_.back().second;
    WLOGFI("GetFinalTask %{public}s", stateTaskQ_.back().first.c_str());
    for (auto& task : stateTaskQ_) {
        WLOGFI("task %{public}s", task.first.c_str());
    }
    stateTaskQ_.clear();
    return true;
}

void WindowNodeStateMachine::UpdateStateTaskQueue(std::string event, const StateTask& task)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    stateTaskQ_.push_back(std::make_pair(event, task));
}

void WindowNodeStateMachine::TransitionTo(WindowNodeState state)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    currState_ = state;
}

void WindowNodeStateMachine::UpdateAnimationTaskCount(bool isAdd)
{
    if (isAdd) {
        taskCount_.fetch_add(1, std::memory_order_seq_cst);
    } else {
        taskCount_.fetch_sub(1, std::memory_order_seq_cst);
    }
    WLOGFI("UpdateAnimationTaskCount: %{public}d", GetAnimationCount());
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
} // Rosen
} // OHOS