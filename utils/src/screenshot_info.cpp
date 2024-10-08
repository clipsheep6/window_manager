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

#include "window_manager_hilog.h"
#include "screenshot_info.h"

namespace OHOS::Rosen {
bool ScreenshotInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteString(trigger_) && parcel.WriteUint64(displayId_);
}

ScreenshotInfo *ScreenshotInfo::Unmarshalling(Parcel &parcel)
{
    ScreenshotInfo *info = new(std::nothrow) ScreenshotInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "failed to allocate memory");
        return nullptr;
    }
    bool res = parcel.ReadString(info->trigger_) && parcel.ReadUint64(info->displayId_);
    if (!res) {
        TLOGE(WmsLogTag::DMS, "failed to read data from parcel");
        delete info;
        return nullptr;
    }
    return info;
}
} // namespace OHOS::Rosen