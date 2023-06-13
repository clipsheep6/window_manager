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

#ifndef ANR_MANAGER_H
#define ANR_MANAGER_H

#include <mutex>
#include <unordered_map>

#include "nocopyable.h"
#include "singleton.h"

#include "ws_common.h"

// #include "i_anr_observer.h"

namespace OHOS {
namespace Rosen {
class ANRManager final {
    DECLARE_DELAYED_SINGLETON(ANRManager);
public:
    DISALLOW_COPY_AND_MOVE(ANRManager);
    void Init();
    void AddTimer(int32_t id, int64_t currentTime, int32_t persistentId);
    void MarkProcessed(int32_t eventId, int32_t persistentId);
    bool IsANRTriggered(int64_t time, int32_t persistentId);
    void RemoveTimers(int32_t persistentId);
    void OnSessionLost(int32_t persistentId);
    void SetApplicationPid(int32_t persistentId, int32_t applicationPid);
    int32_t GetPidByPersistentId(int32_t persistentId);
    // void SetAnrObserver(sptr<IAnrObserver> observer);
private:
    int32_t anrTimerCount_ { 0 };
    std::unordered_map<int32_t, int32_t> applicationMap_;
    std::mutex mtx_;
    // sptr<IAnrObserver> anrObserver_;
    /**
     * 加一个成员 sptr<IAnrObserver> anrObserver_; 用于向调用SetAnrObserver的进程通知 ANR
     * 本质上是一个 proxy 
    */
};
#define ANRMgr ::OHOS::DelayedSingleton<ANRManager>::GetInstance()
} // namespace Rosen
} // namespace OHOS
#endif // ANR_MANAGER_H