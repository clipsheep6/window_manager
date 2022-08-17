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

 #include "window_qos_proxy.h"

#include <cstddef>
#include <stdint>
#include <message_option.h>
#include <message_parcel.h>
#include <string>
#include <utility>
#include <array>

#include "platform/ohos/rs_irender_service_qos.h"

namespace OHOS {
namespace Rosen {
WindowQosProxy::WindowQosProxy(const sptr<IRemoteObject>& impl) : IRemoteObject<RSIRenderServiceQos>(impl) {}

bool WindowQosProxy::SetVSyncRate(uint32_t pid, int rate)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(RSIRenderServiceQos::GetDescriptor())) {
        return false;
    }
    Bytes bytes(sizeof(uint32_t) + sizeof(int), 0);
    size_t pos = 0;
    if (!WriteToBytes<uint32_t>(bytes, pos, pid) || !WriteToBytes<int>(bytes, pos, rate)) {
        return false;
    }

    data.WriteBuffer(bytes.data(), 1024);
    auto ret = Remote()->SendRequest(SET_APP_VSYNC_RATE, data, reply, option);
    return (ret == ERR_NONE) ? reply.ReadBool() : false;
}

bool WindowQosProxy::RequestVSyncRate(std::verctor<ConnectionInfo>& appVsyncCountVec)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(RSIRenderServiceQos::GetDescriptor())) {
        return false;
    }

    auto ret = Remote()->SendRequest(GET_APP_VSYNC_RATE, data, reply, option);
    const uint8_t* recvData = reply.ReadBuffer(1024);
    size_t pos = 0;
    Bytes bytes(1024, 0);
    memcpy_s(bytes.data(), 1024, recvData, 1024);
    int nums = -1;
    if (!ReadFromBytes<uint64_t>(bytes, pos, time_) ||
        !ReadFromBytes<int>(bytes, pos, nums)) {
        return false;
    }
    for (int i = 0; i < nums; i++) {
        uint32_t pid;
        uint64_t count;
        int rate;
        if (!ReadFromBytes<uint32_t>(bytes, pos, pid) ||
            !ReadFromBytes<uint64_t>(bytes, pos, count) ||
            !ReadFromBytes<int>(bytes, pos, rate)) {
            return false;
        }
    }
    return true;
}

uint64_t WindowQosProxy::GetRequestTime() const
{
    return time_;
}

} // namespace Rosen
} // namespace OHOS