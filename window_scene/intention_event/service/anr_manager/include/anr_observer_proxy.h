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

#ifndef ANR_OBSERVER_PROXY_H
#define ANR_OBSERVER_PROXY_H

#include <iremote_proxy.h>
#include "nocopyable.h"

#include "i_anr_observer.h"

namespace OHOS {
namespace Rosen {
class AnrObserverProxy : public IRemoteProxy<IAnrObserver> {
public:
    explicit AnrObserverProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IAnrObserver>(impl) {}
    ~AnrObserverProxy() = default;
    DISALLOW_COPY_AND_MOVE(AnrObserverProxy);
    virtual int32_t OnAnr(int32_t pid) override;

private:
    static inline BrokerDelegator<AnrObserverProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_OBSERVER_PROXY_H