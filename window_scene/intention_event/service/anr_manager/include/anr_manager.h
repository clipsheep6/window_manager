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

#include "nocopyable.h"
#include "singleton.h"
#include "i_anr_notifier.h"

namespace OHOS {
namespace Rosen {
class ANRManager final {
DECLARE_DELAYED_SINGLETON(ANRManager);
public:
    DISALLOW_COPY_AND_MOVE(ANRManager);
    void Init();
    void AddTimer(int32_t id, int64_t currentTime, int32_t persistentId);
    int32_t MarkProcessed(int32_t eventId, int32_t persistentId);
    bool IsANRTriggered(int64_t time, int32_t persistentId);
    void RemoveTimers(int32_t persistentId);
    void OnSessionLost(int32_t persistentId);
private:
    int32_t anrTimerCount_ { 0 };
    sptr<IANRNotifier> anrNotifier_ { nullptr };
};
#define ANRMgr ::OHOS::DelayedSingleton<ANRManager>::GetInstance()
} // namespace Rosen
} // namespace OHOS
#endif // ANR_MANAGER_H