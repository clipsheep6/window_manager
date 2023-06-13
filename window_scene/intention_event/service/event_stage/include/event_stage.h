/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <list>
#include <map>
#include <vector>

#include "noncopyable.h"
#include "singleton.h"

namespace OHOS {
namespace Rosen {
class EventStage {
    DECLARE_DELAYED_SINGLETON(EventStage);
public:
    DISALLOW_COPY_AND_MOVE(EventStage);
    void SetAnrStatus(int32_t persistentId, bool status);
    bool CheckAnrStatus(int32_t persistentId);
    void SaveANREvent(int32_t persistentId, int32_t id, int64_t time, int32_t timerId);
    std::vector<int32_t> GetTimerIds(int32_t persistentId);
    std::list<int32_t> DelEvents(int32_t persistentId, int32_t id);
private:
    struct EventTime {
        int32_t id { 0 };
        int64_t eventTime { 0 };
        int32_t timerId { -1 };
    };
    std::map<int32_t, std::vector<EventTime>> events_;
    std::map<int32_t, bool> isAnrProcess_;
    // 该成员标识当前 persistentId 对应的session的事件是否有在队列中的。
    // 对 EVStage 的修改只有 ANRMgr， 而两者都是单例，因此只要在 ANRMgr 中加锁就可以保证多线程互斥
};
#define EVStage ::OHOS::DelayedSingleton<EventStage>::GetInstance()
} // namespace Rosen
} // namespace OHOS
