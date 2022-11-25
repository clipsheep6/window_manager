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
#include "zidl/ressched_report.h"
#include <dlfcn.h>

namespace OHOS::Rosen {
namespace {
#ifdef __aarch64__
    const std::string RES_SCHED_CLIENT_SO = "/system/lib64/libressched_client.z.so";
#else
    const std::string RES_SCHED_CLIENT_SO = "/system/lib/libressched_client.z.so";
#endif
}

ReportDataFunc LoadReportDataFunc()
{
    auto handle = dlopen(RES_SCHED_CLIENT_SO.c_str(), RTLD_NOW);
    if (handle == nullptr) {
        return nullptr;
    }
    auto func = reinterpret_cast<ReportDataFunc>(dlsym(handle, "ReportData"));
    if (func == nullptr) {
        dlclose(handle);
        return nullptr;
    }
    return func;
}

ResSchedReport& ResSchedReport::GetInstance()
{
    static ResSchedReport instance;
    return instance;
}

void ResSchedReport::ResSchedDataReport(uint32_t resType, int32_t value,
    const std::unordered_map<std::string, std::string>& payload)
{
    if (reportDataFunc_ == nullptr) {
        reportDataFunc_ = LoadReportDataFunc();
    }
    if (reportDataFunc_ != nullptr) {
        reportDataFunc_(resType, value, payload);
    }
}
} // namespace OHOS::Rosen