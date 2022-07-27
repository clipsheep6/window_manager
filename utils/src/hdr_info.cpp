/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hdr_info.h"

namespace OHOS::Rosen {
HdrInfo::HdrInfo(float maxLum, float minLum, float maxAverageLum,
    const std::vector<ScreenHDRFormat>& formats) : maxLum_(maxLum), minLum_(minLum),
    maxAverageLum_(maxAverageLum), hdrFormats_(formats)
{
}

bool HdrInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteFloat(maxLum_) && parcel.WriteFloat(minLum_) &&
        parcel.WriteFloat(maxAverageLum_) && WriteVector(hdrFormats_, parcel);
}

HdrInfo *HdrInfo::Unmarshalling(Parcel &parcel)
{
    float maxLum;
    float minLum;
    float maxAverageLum = false;
    std::vector<ScreenHDRFormat> formats;
    if (!parcel.ReadFloat(maxLum)) {
        maxLum = 0;
    }
    if (!parcel.ReadFloat(minLum)) {
        minLum = 0;
    }
    if (!parcel.ReadFloat(maxAverageLum)) {
        maxAverageLum = 0;
    }
    HdrInfo* hdrInfo = new HdrInfo(maxLum, minLum, maxAverageLum, formats);
    return hdrInfo;
}

bool HdrInfo::WriteVector(const std::vector<ScreenHDRFormat>& formats, Parcel &parcel) const
{
    if (!parcel.WriteUint32(static_cast<uint32_t>(formats.size()))) {
        return false;
    }
    for (ScreenHDRFormat format : formats) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(format))) {
            return false;
        }
    }
    return true;
}

bool HdrInfo::ReadVector(std::vector<ScreenHDRFormat>& unmarFormats, Parcel &parcel)
{
    uint32_t size;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    for (uint32_t index = 0; index < size; index++) {
        uint32_t format;
        if (!parcel.ReadUint32(format)) {
            return false;
        }
        unmarFormats.push_back(static_cast<ScreenHDRFormat>(format));
    }
    return true;
}
} // namespace OHOS::Rosen