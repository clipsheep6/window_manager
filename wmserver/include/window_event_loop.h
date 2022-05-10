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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#ifndef OHOS_WINDOW_EVENT_LOOP_H
#define OHOS_WINDOW_EVENT_LOOP_H

namespace OHOS {
namespace Rosen {
namespace Detail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(t, t->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() {}

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace Detail

class EventLoop
{
private:
    using Task = std::function<void()>;
    mutable std::mutex mtx_;
    bool wakeUp_ = false;
    std::condition_variable taskCond_;
    mutable std::atomic<bool> running_ { false };
    std::vector<Task> taskQ_;

public:
    EventLoop()
    {
    }
    ~EventLoop()
    {
    }

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }
    
    void PostTask(const Task& task)
    {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            taskQ_.emplace_back(task);
        }
        WakeUp();
    }
    void WaitTask()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if (!wakeUp_ && taskQ_.empty()) {
            taskCond_.wait(
                lock,
                [this]() { return !taskQ_.empty() || wakeUp_;});
        }
        wakeUp_ = false;
    }
    void WakeUp()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        wakeUp_ = true;
        taskCond_.notify_one();
    }
    void Start() noexcept
    {
        if (running_) {
            return;
        }
        running_ = true;
        while (running_) {
            WaitTask();
            ExecuteAllTask();
        }
    }
    void ExecuteAllTask()
    {
        std::vector<Task> tasks;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            tasks.swap(taskQ_);
        }
        for (const auto& task : tasks) {
            if (task != nullptr) {
                task();
            }
        }
    }
};
} // namespace Rosen
} // namespace OHOS
#endif