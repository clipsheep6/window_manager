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

#include "window_dump_info_list.h"

namespace OHOS {
namespace Rosen {
WindowDumpInfo::WindowDumpInfo(std::string name, uint32_t id, WindowMode mode, WindowType type)
    : name_ { name }, id_ { id }, mode_ { mode }, type_ { type }
{
    time(&timestamp_);
}

void WindowDumpInfoList::Add(const WindowDumpInfo& WindowDumpInfo)
{
    info_[end_] = WindowDumpInfo;
    if (size_ < MAX_DUMP_LIST_SIZE) {
        ++size_;
    } else {
        start_= (start_ + 1) % MAX_DUMP_LIST_SIZE;
    }
    end_ = (end_ + 1) % MAX_DUMP_LIST_SIZE;
}
} // namespace Rosen
} // namespace OHOS