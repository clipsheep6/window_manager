/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "common/include/task_scheduler.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "TaskScheduler" };
}

TaskScheduler::TaskScheduler(const std::string& threadName)
{
    auto runner = AppExecFwk::EventRunner::Create(threadName);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

std::shared_ptr<AppExecFwk::EventHandler> TaskScheduler::GetEventHandler()
{
    return handler_;
}

void TaskScheduler::PostVoidSyncTask(Task task)
{
    if (handler_ == nullptr) {
        WLOGFE("PostVoidSyncTask Failed to post task, handler is null!");
        return;
    }
    handler_->PostSyncTask(std::move(task), AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void TaskScheduler::PostAsyncTask(Task task, int64_t delayTime)
{
    if (handler_ == nullptr) {
        WLOGFE("PostVoidSyncTask Failed to post task, handler is null!");
        return;
    }
    handler_->PostTask(std::move(task), delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
} // namespace OHOS::Rosen
