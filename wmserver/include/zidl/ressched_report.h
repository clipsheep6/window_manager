/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_MANAGER_RESSCHED_RESSCHED_REPORT_H
#define OHOS_WINDOW_MANAGER_RESSCHED_RESSCHED_REPORT_H

#include <cstdint>
#include <string>
#include <unordered_map>

namespace OHOS::Rosen {
constexpr uint32_t RES_TYPE_SHOW_REMOTE_ANIMATION = 33;

using ReportDataFunc = void (*)(uint32_t resType, int64_t value,
    const std::unordered_map<std::string, std::string>& payload);

ReportDataFunc __attribute__((visibility("default"))) LoadReportDataFunc();

class ResSchedReport final {
public:
    static ResSchedReport& GetInstance();
    void ResSchedDataReport(uint32_t resType, int32_t value = 0,
        const std::unordered_map<std::string, std::string>& payload = {});
private:
    ResSchedReport() {}
    ~ResSchedReport() {}
    ReportDataFunc reportDataFunc_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_MANAGER_RESSCHED_RESSCHED_REPORT_H