/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_H_
#define INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_H_

#include "parcel.h"

namespace OHOS {
namespace Media {
class PixelMap : public Parcelable {
public:
    int32_t GetWidth()
    {
        return 0;
    }

    int32_t GetHeight()
    {
        return 0;
    }
};
}
}
#endif // INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_H_