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

#ifndef OHOS_ROSEN_WINDOW_NODE_STATE_MACHINE_H
#define OHOS_ROSEN_WINDOW_NODE_STATE_MACHINE_H

#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <vector>

namespace OHOS {
namespace Rosen {
namespace {
    using StateTask = std::function<void()>;
}
enum class WindowNodeState : uint32_t {
    INITIAL,
    STARTING_CREATED,
    SHOW_ANIMATION_PLAYING,
    SHOW_ANIMATION_DONE,
    HIDE_ANIMATION_PLAYING,
    HIDE_ANIMATION_DONE,
    SHOWN,
    HIDDEN,
    DESTROYED
};

class WindowNodeStateMachine {
public:
    WindowNodeStateMachine();

    ~WindowNodeStateMachine();

    bool GetFinalStateTask(StateTask& task);

    void UpdateStateTaskQueue(std::string event, const StateTask& task);

    void TransitionTo(WindowNodeState state);

    void UpdateAnimationTaskCount(bool isAdd);

    int32_t GetAnimationCount();

    bool IsWindowNodeShownOrShowing();

    bool IsRemoteAnimationPlaying();

    bool IsWindowNodeHiddenOrHiding();

    WindowNodeState GetCurrentState();

    bool IsValidTask(std::string taskName);

    bool IsShowAnimationPlaying();

    bool IsHideAnimationPlaying();

    // WindowNodeState GetLastState();
    bool animationFirst_ = false;
    // test
    void PrintHistoryState();
    void SetWindowId(uint32_t id) {
        windowId_ = id;
    }
private:
    std::deque<std::pair<std::string, StateTask>> stateTaskQ_;
    WindowNodeState currState_ = WindowNodeState::INITIAL;
    // WindowNodeState lastState_ = WindowNodeState::INITIAL;
    std::atomic_int taskCount_;
    std::recursive_mutex mutex_;
    // just for test
    std::vector<WindowNodeState> stateRecord_;
    uint32_t windowId_;
};
} // namespace Rosen
} // namespace OHOS
#endif