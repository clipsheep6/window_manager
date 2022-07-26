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

#ifndef OHOS_WM_INCLUDE_WINDOW_EVENT_HANDLER_H
#define OHOS_WM_INCLUDE_WINDOW_EVENT_HANDLER_H

#include <event_handler.h>

#include "noncopyable.h"

namespace OHOS::Rosen {
class WindowEventHandler : public AppExecFwk::EventHandler {
public:
    using Task = std::function<void()>;
    explicit WindowEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner = nullptr) : EventHandler(runner)
    {
    }
    virtual ~WindowEventHandler() = default;
    WM_DISALLOW_COPY_AND_MOVE(WindowEventHandler);

    template<typename SyncTask, typename Return = std::invoke_result_t<SyncTask>>
    Return GetSyncTaskResult(SyncTask&& task)
    {
        Return ret;
        Task syncTask([&ret, &task]() {ret = task();});
        PostSyncTask(syncTask, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        return ret;
    }

    void PostImmediateSyncTask(Task task)
    {
        PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
};
} // namespace OHOS::Rosen
#endif // OHOS_WM_INCLUDE_WINDOW_EVENT_HANDLER_H
