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

#ifndef OHOS_WM_INCLUDE_WINDOW_DUMP_INFO_LIST_H
#define OHOS_WM_INCLUDE_WINDOW_DUMP_INFO_LIST_H

#include <string>
#include <iomanip>
#include <time.h>

#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t MAX_DUMP_LIST_SIZE = 10;
    inline auto operator "" _width(unsigned long long int n)
    {
        return std::setw(static_cast<int>(n));
    }
}

struct WindowDumpInfo {
    std::string name_;
    uint32_t id_;
    WindowMode mode_;
    WindowType type_;
    time_t timestamp_;

    WindowDumpInfo() = default;
    WindowDumpInfo(std::string name, uint32_t id, WindowMode mode, WindowType type);
};

class WindowDumpInfoList {
public:
    WindowDumpInfoList() = default;
    void Add(const WindowDumpInfo& WindowDumpInfo);

    uint32_t GetEndPos() const
    {
        return end_;
    }

    uint32_t GetSize() const
    {
        return size_;
    }

    WindowDumpInfo& operator[](uint32_t i)
    {
        return info_[i];
    };

    const WindowDumpInfo& operator[](uint32_t i) const
    {
        return info_[i];
    };
private:
    WindowDumpInfo info_[MAX_DUMP_LIST_SIZE] { };
    uint32_t start_ = 0, end_ = 0;
    uint32_t size_ = 0;
};

template<typename OutStream>
OutStream& operator<<(OutStream& os, const WindowDumpInfoList& list)
{
    if (!list.GetSize()) {
        return os;
    }
    os << "-------------------------------------Recent Destroy"
        << "-------------------------------------"
        << std::endl;
    os << "WindowName               WinId  Type Mode Time"
        << std::endl;
    uint32_t pos = list.GetEndPos();
    char timeStr[64] { };
    for (uint32_t i = list.GetSize(); i > 0; i--) {
        if (pos == 0) {
            pos += MAX_DUMP_LIST_SIZE;
        }
        pos--;
        tm* tmPtr = localtime(&list[pos].timestamp_);
        if (tmPtr == nullptr) {
            return os;
        }
        (void)strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tmPtr);
        os << std::left << 25_width << list[pos].name_
        << std::left << 7_width << list[pos].id_
        << std::left << 5_width << static_cast<uint32_t>(list[pos].type_)
        << std::left << 5_width << static_cast<uint32_t>(list[pos].mode_)
        << std::left << 7_width << timeStr
        << std::endl;
    }
    return os;
}
} // namespace OHOS
} // namespace Rosen
#endif // OHOS_WM_INCLUDE_WINDOW_DUMP_INFO_LIST_H