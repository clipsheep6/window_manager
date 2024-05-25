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

#ifndef OHOS_ROSEN_WMSERVER_TEST_IREMOTE_OBJECT_MOCKER_H
#define OHOS_ROSEN_WMSERVER_TEST_IREMOTE_OBJECT_MOCKER_H

#include <iremote_object.h>
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class IRemoteObjectMocker : public IRemoteObject {
public:
    IRemoteObjectMocker() : IRemoteObject{u"IRemoteObjectMocker"}
    {
    }

    ~IRemoteObjectMocker()
    {
    }

    MOCK_METHOD0(GetObjectRefCount, int32_t());
    MOCK_METHOD4(SendRequest, int (uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));
    MOCK_CONST_METHOD0(IsProxyObject, bool());
    MOCK_CONST_METHOD0(CheckObjectLegality, bool());
    MOCK_METHOD1(AddDeathRecipient, bool(const sptr<DeathRecipient> &recipient));
    MOCK_METHOD1(RemoveDeathRecipient, bool(const sptr<DeathRecipient> &recipient));
    MOCK_METHOD0(AsInterface, sptr<IRemoteBroker>());
    MOCK_METHOD2(AsInterface, int(int fd, const std::vector<std::u16string> &args));
};
}
}

#endif
