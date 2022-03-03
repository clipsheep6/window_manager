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

#ifndef FOUNDATION_DMSERVER_DISPLAY_INFO_H
#define FOUNDATION_DMSERVER_DISPLAY_INFO_H

#include <parcel.h>

#include "class_var_definition.h"
#include "display.h"
#include "dm_common.h"

namespace OHOS::Rosen {
class DisplayInfo : public Parcelable {
friend class AbstractDisplay;
public:
    ~DisplayInfo() = default;
    DisplayInfo(const DisplayInfo&) = delete;
    DisplayInfo(DisplayInfo&&) = delete;
    DisplayInfo& operator=(const DisplayInfo&) = delete;
    DisplayInfo& operator=(DisplayInfo&&) = delete;

    virtual bool Marshalling(Parcel& parcel) const override;
    static DisplayInfo *Unmarshalling(Parcel& parcel);

    DEFINE_VAR_DEFAULT_FUNC_GET(DisplayId, DisplayId, id, DISPLAY_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET(DisplayType, DisplayType, type, DisplayType::DEFAULT);
    DEFINE_VAR_DEFAULT_FUNC_GET(int32_t, Width, width, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET(int32_t, Height, height, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET(uint32_t, FreshRate, freshRate, 0);
    DEFINE_VAR_DEFAULT_FUNC_GET(ScreenId, ScreenId, screenId, SCREEN_ID_INVALID);
    DEFINE_VAR_DEFAULT_FUNC_GET(float, XDpi, xDpi, 0.0f);
    DEFINE_VAR_DEFAULT_FUNC_GET(float, YDpi, yDpi, 0.0f);
    DEFINE_VAR_DEFAULT_FUNC_GET(Rotation, Rotation, rotation, Rotation::ROTATION_0);
    DEFINE_VAR_DEFAULT_FUNC_GET(Orientation, Orientation, orientation, Orientation::UNSPECIFIED);
protected:
    DisplayInfo() = default;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_DISPLAY_INFO_H