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

#ifndef ANR_HANDLER_H
#define ANR_HANDLER_H

#include <memory>
#include <mutex>

#include "nocopyable.h"
#include "singleton.h"
#include "session_stage_interface.h"

namespace OHOS {
namespace Rosen {
class ANRHandler {
    DECLARE_DELAYED_SINGLETON(ANRHandler);

public:
    DISALLOW_COPY_AND_MOVE(ANRHandler);

    void SetSessionStage(const wptr<ISessionStage> &sessionStage);
    void MarkProcessed(int32_t eventId, int64_t actionTime);
private:
    std::mutex mutex_;
    wptr<ISessionStage> sessionStage_ { nullptr };
    int32_t lastProcessedEventId_ { -1 };
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_HANDLER_H