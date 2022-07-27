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

#ifndef FOUNDATION_DMSERVER_HDR_INFO_H
#define FOUNDATION_DMSERVER_HDR_INFO_H

#include <parcel.h>

#include <transaction/rs_interfaces.h>

#include "class_var_definition.h"
#include "display.h"
#include "dm_common.h"

namespace OHOS::Rosen {
class HdrInfo : public Parcelable {
public:
    HdrInfo() = default;
    HdrInfo(float maxLum, float minLum, float maxAverageLum, const std::vector<ScreenHDRFormat>& formats);
    ~HdrInfo() = default;
    WM_DISALLOW_COPY_AND_MOVE(HdrInfo);

    virtual bool Marshalling(Parcel& parcel) const override;
    static HdrInfo *Unmarshalling(Parcel& parcel);

    DEFINE_VAR_DEFAULT_FUNC_GET_SET(float, MaxLum, maxLum, 0.f);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(float, MinLum, minLum, 0.f);
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(float, MaxAverageLum, maxAverageLum, 0.f);
    DEFINE_VAR_FUNC_GET_SET(std::vector<ScreenHDRFormat>, HdrFormats, hdrFormats);
private:
    bool WriteVector(const std::vector<ScreenHDRFormat>& formats, Parcel &parcel) const;
    static bool ReadVector(std::vector<ScreenHDRFormat>& unmarFormats, Parcel &parcel);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_HDR_INFO_H