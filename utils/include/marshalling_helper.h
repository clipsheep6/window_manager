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

#ifndef FOUNDATION_MARSHALLING_HELPER_H
#define FOUNDATION_MARSHALLING_HELPER_H

#include <parcel.h>

#include "wm_common_inner.h"

namespace OHOS::Rosen {
class MarshallingHelper : public Parcelable {
public:
    MarshallingHelper() = delete;
    static bool MarshallingSurfaceNodeInfo(Parcel &parcel, const SurfaceNodeInfo& data)
    {
        if (!parcel.WriteUint64(data.nodeId_)) {
            return false;
        }
        if (!parcel.WriteString(data.nodeName_)) {
            return false;
        }
        if (!parcel.WriteBool(data.isRenderNode_)) {
            return false;
        }
        return true;
    }

    static bool UnmarshallingSurfaceNodeInfo(Parcel &parcel, SurfaceNodeInfo& data)
    {
        data.nodeId_ = parcel.ReadUint64();
        data.nodeName_ = parcel.ReadString();
        data.isRenderNode_ = parcel.ReadBool();
        return true;
    }

    // static bool MarshallingAnimInfo(Parcel &parcel, sptr<WindowAnimationTargetInfo>& data)
    // {
    //     if (!parcel.WriteUint64(data->nodeId_)) {
    //         return false;
    //     }
    //     if (!parcel.WriteString(data->nodeName_)) {
    //         return false;
    //     }
    //     // if (!parcel.WriteBool(data->isRenderNode_)) {
    //     //     return false;
    //     // }
    //     if (!parcel.WriteString(data->bundleName_)) {
    //         return false;
    //     }
    //     if (!parcel.WriteString(data->abilityName_)) {
    //         return false;
    //     }
    //     if (!parcel.WriteInt32(data->missionId_)) {
    //         return false;
    //     }
    //     if (!parcel.WriteUint32(data->windowId_)) {
    //         return false;
    //     }
    //     if (!parcel.WriteUint64(data->displayId_)) {
    //         return false;
    //     }
    //     if (!parcel.WriteFloat(data->radius_)) {
    //         return false;
    //     }
    //     return true;
    // }

    // static bool UnmarshallingAnimInfo(Parcel &parcel, sptr<WindowAnimationTargetInfo>& data)
    // {
    //     data->nodeId_ = parcel.ReadUint64(data->nodeId_);
    //     data->nodeName_ = parcel.ReadString(data->nodeName_);
    //     // data->isRenderNode_ = parcel.ReadBool();
    //     data->bundleName_ = parcel.ReadString(data->bundleName_);
    //     data->abilityName_ = parcel.ReadString();
    //     data->missionId_ = parcel.ReadInt32();
    //     data->windowId_ = parcel.ReadUint32();
    //     data->displayId_ = parcel.ReadUint64();
    //     data->radius_ = parcel.ReadFloat();
    //     return true;
    // }

    template<class T>
    static bool MarshallingVectorParcelableObj(Parcel &parcel, const std::vector<sptr<T>>& data)
    {
        if (data.size() > INT_MAX) {
            return false;
        }
        if (!parcel.WriteInt32(static_cast<int32_t>(data.size()))) {
            return false;
        }
        for (const auto &v : data) {
            if (!parcel.WriteParcelable(v)) {
                return false;
            }
        }
        return true;
    }

    template<class T>
    static bool UnmarshallingVectorParcelableObj(Parcel &parcel, std::vector<sptr<T>>& data)
    {
        int32_t len = parcel.ReadInt32();
        if (len < 0) {
            return false;
        }

        size_t readAbleSize = parcel.GetReadableBytes();
        size_t size = static_cast<size_t>(len);
        if ((size > readAbleSize) || (size > data.max_size())) {
            return false;
        }
        data.resize(size);
        if (data.size() < size) {
            return false;
        }
        size_t minDesireCapacity = sizeof(int32_t);
        for (size_t i = 0; i < size; i++) {
            readAbleSize = parcel.GetReadableBytes();
            if (minDesireCapacity > readAbleSize) {
                return false;
            }
            data[i] = parcel.ReadParcelable<T>();
        }
        return true;
    }

    template<class T>
    static bool MarshallingVectorObj(Parcel &parcel, const std::vector<T>& data,
        std::function<bool(Parcel&, const T&)> func)
    {
        if (data.size() > INT_MAX) {
            return false;
        }
        if (func == nullptr) {
            return false;
        }
        if (!parcel.WriteInt32(static_cast<int32_t>(data.size()))) {
            return false;
        }
        for (const auto &v : data) {
            if (!func(parcel, v)) {
                return false;
            }
        }
        return true;
    }

    template<class T>
    static bool UnmarshallingVectorObj(Parcel &parcel, std::vector<T>& data, std::function<bool(Parcel&, T&)> func)
    {
        if (func == nullptr) {
            return false;
        }
        int32_t len = parcel.ReadInt32();
        if (len < 0) {
            return false;
        }

        size_t readAbleSize = parcel.GetReadableBytes();
        size_t size = static_cast<size_t>(len);
        if ((size > readAbleSize) || (size > data.max_size())) {
            return false;
        }
        data.resize(size);
        if (data.size() < size) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            if (!func(parcel, data[i])) {
                return false;
            }
        }
        return true;
    }
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_MARSHALLING_HELPER_H