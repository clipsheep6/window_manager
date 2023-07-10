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

#include "anr_handler.h"

#include <cinttypes>

#include "entrance_log.h"
#include "proto.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRHandler" };
} // namespace

ANRHandler::ANRHandler() {}

ANRHandler::~ANRHandler() {}

void ANRHandler::SetSessionStage(const wptr<ISessionStage> &sessionStage)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    sessionStage_ = sessionStage;
}

void ANRHandler::MarkProcessed(int32_t eventId, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    if (lastProcessedEventId_ > eventId) {
        WLOGFD("Current event is not needed to report, current time %{public}" PRId64 "", actionTime);
        return;
    }
    if (sessionStage_ == nullptr) {
        WLOGFE("sessionStage is nullptr");
        return;
    }
    if (WSError ret = sessionStage_->MarkProcessed(eventId); ret != WSError::WS_OK) {
        WLOGFE("Send to sceneBoard failed, ret:%{public}d", ret);
        return;
    }
    lastProcessedEventId_ = eventId;
}
} // namespace Rosen
} // namespace OHOS