/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "wm_common.h"

namespace OHOS {
namespace Rosen {
inline int32_t GetUserIdByUid(int32_t uid)
{
    return uid / BASE_USER_RANGE;
}

inline bool Transform::NearZero(float val)
{
    return val < 0.001f && val > -0.001f;
}

inline std::string Rect::ToString() const
{
    std::stringstream ss;
    ss << "[" << posX_ << " " << posY_ << " " << width_ << " " << height_ << "]";
    return ss.str();
}

inline bool AvoidArea::WriteParcel(Parcel& parcel, const Rect& rect)
{
    return parcel.WriteInt32(rect.posX_) && parcel.WriteInt32(rect.posY_) &&
        parcel.WriteUint32(rect.width_) && parcel.WriteUint32(rect.height_);
}

inline bool AvoidArea::ReadParcel(Parcel& parcel, Rect& rect)
{
    return parcel.ReadInt32(rect.posX_) && parcel.ReadInt32(rect.posY_) &&
        parcel.ReadUint32(rect.width_) && parcel.ReadUint32(rect.height_);
}

inline bool KeyboardLayoutParams::WriteParcel(Parcel& parcel, const Rect& rect)
{
    return parcel.WriteInt32(rect.posX_) && parcel.WriteInt32(rect.posY_) &&
        parcel.WriteUint32(rect.width_) && parcel.WriteUint32(rect.height_);
}

inline bool KeyboardLayoutParams::ReadParcel(Parcel& parcel, Rect& rect)
{
    return parcel.ReadInt32(rect.posX_) && parcel.ReadInt32(rect.posY_) &&
        parcel.ReadUint32(rect.width_) && parcel.ReadUint32(rect.height_);
}
}
}