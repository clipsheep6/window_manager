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

#ifndef OHOS_ROSEN_WINDOW_QOS_PROXY_H
#define OHOS_ROSEN_WINDOW_QOS_PROXY_H

#include <stdint.h>
#include <iremote_proxy.h>

#include "platform/ohos/re_irender_service_qos.h"

namespace OHOS {
namespace Rosen {
class WindowQosProxy : public I
public:
    explicit WindowQosProxy(const sptr<IRemoteObject>& impl);
    virtual ~WindowQosProxy() noexcept = default;
    bool SetVSyncRate(uint32_t pid, int rate) override;
    bool RequestVSyncRate(std::verctor<ConnectionInfo>& appVsyncCountVec) override;
    uint64_t GetRequestTime() const;

private:
    uint64_t time_;
    std::map<uint32_t, uint64_t> appVsyncCountMap_;
    static inline BrokerDelegator<WindowQosProxy> delegator;
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_QOS_PROXY_H