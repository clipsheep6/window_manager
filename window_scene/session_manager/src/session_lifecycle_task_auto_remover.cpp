/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "session_lifecycle_task_auto_remover.h"

#include "session/host/include/scene_session.h"
#include "session/host/include/session.h"
#include "session_manager.h"
#include "session_manager/include/scene_session_manager.h"

namespace OHOS::Rosen {

SessionLifeCycleTaskAutoRemover::SessionLifeCycleTaskAutoRemover(int32_t persistentId, const LifeCycleTaskType taskType)
{
    persistentId_ = persistentId;
    taskType_ = taskType;
}

SessionLifeCycleTaskAutoRemover::~SessionLifeCycleTaskAutoRemover()
{
    SceneSessionManager::GetInstance().RemoveLifeCycleTaskByPersistentId(persistentId_, taskType_);
}
}