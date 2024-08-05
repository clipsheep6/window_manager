/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_FUTURE_CALLBACK_STUB_H
#define OHOS_ROSEN_FUTURE_CALLBACK_STUB_H

#include <map>

#include "future_callback_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Rosen {
class FutureCallbackStub;
using FutureCallbackStubFunc = int (FutureCallbackStub::*)(MessageParcel& data, MessageParcel& reply);

class FutureCallbackStub : public IRemoteStub<IFutureCallback> {
public:
    FutureCallbackStub() = default;
    ~FutureCallbackStub() = default;

    virtual int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option) override;

private:
    static const std::map<uint32_t, FutureCallbackStubFunc> stubFuncMap_;

    int HandleUpdateSessionRect(MessageParcel& data, MessageParcel& reply);
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_FUTURE_CALLBACK_STUB_H