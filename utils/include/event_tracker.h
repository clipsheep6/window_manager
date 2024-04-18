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

#ifndef OHOS_ROSEN_SCREEN_EVENT_TRACKER_H
#define OHOS_ROSEN_SCREEN_EVENT_TRACKER_H

#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <string>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
const int32_t OUTPUT_FREQ = 1; // 1Hz

enum class TrackSupportEvent: int32_t {
    INVALID = -1,
    DMS_CONSTRUCTION,
    DMS_ONSTART,
    DMS_REGISTER_STATUS,
    DMS_CALLBACK,
};

struct TrackInfo {
    std::string info;
    std::chrono::system_clock::time_point timestamp;
};

class EventTracker {
public:
    void RecordEvent(TrackSupportEvent event, std::string info = "")
    {
        std::lock_guard<std::mutex> lock(mutex_);
        recordMap_[event].push_back({info, std::chrono::system_clock::now()});
    }

    void ClearAllRecordedEvents()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        recordMap_.clear();
    }

    void LogWarnninAllInfos() const
    {
        auto now = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastOutputTime_).count() < OUTPUT_FREQ) {
            return ; // Output too frequent. Try again later.
        }
        lastOutputTime_ = now;
        
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [event, Infos] : recordMap_) {
            for (const auto& info : Infos) {
                TLOGW(WmsLogTag::DMS, "[EventId: %{public}d][%{public}s]: %{public}s",
                    static_cast<int32_t>(event), formatTimestamp(info.timestamp).c_str(), info.info.c_str());
            }
        }
    }

    std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint) const
    {
        std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
        std::tm tm = *std::localtime(&timeT);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    const std::map<TrackSupportEvent, std::vector<TrackInfo>>& GetRecordMap() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return recordMap_;
    }

private:
    mutable std::mutex mutex_;
    mutable std::chrono::system_clock::time_point lastOutputTime_;
    std::map<TrackSupportEvent, std::vector<TrackInfo>> recordMap_;
};


} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SCREEN_EVENT_TRACKER_H