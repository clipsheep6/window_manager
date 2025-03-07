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

#ifndef OHOS_ROSEN_TEST_COMMON_MOCK_IREMOTE_OBJECT_MOCKER
#define OHOS_ROSEN_TEST_COMMON_MOCK_IREMOTE_OBJECT_MOCKER

#include <iremote_object.h>
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class IRemoteObjectMocker : public IRemoteObject {
public:
    IRemoteObjectMocker() : IRemoteObject {u"IRemoteObjectMocker"}
    {
    }

    ~IRemoteObjectMocker()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
    {
        return 0;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string>& args)
    {
        return 0;
    }
};
class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject {u"MockIRemoteObject"}
    {
    }

    ~MockIRemoteObject()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
    {
        return sendRequestResult_;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string>& args)
    {
        return 0;
    }

    void SetRequestResult(int result)
    {
        sendRequestResult_ = result;
    }

    int sendRequestResult_ = 0;
    int count_ = 0;
};

class RemoteObjectMocker : public IRemoteObject {
public:
    RemoteObjectMocker() : IRemoteObject{u"RemoteObjectMocker"} {}
    ~RemoteObjectMocker() {}

    MOCK_METHOD(int32_t, GetObjectRefCount, (), (override));
    MOCK_METHOD(int, SendRequest, (uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option),
        (override));
    MOCK_METHOD(bool, IsProxyObject, (), (const, override));
    MOCK_METHOD(bool, IsObjectDead, (), (const, override));
    MOCK_METHOD(std::u16string, GetInterfaceDescriptor, (), (override));
    MOCK_METHOD(bool, CheckObjectLegality, (), (const, override));
    MOCK_METHOD(bool, AddDeathRecipient, (const sptr<DeathRecipient>& recipient), (override));
    MOCK_METHOD(bool, RemoveDeathRecipient, (const sptr<DeathRecipient>& recipient), (override));
    MOCK_METHOD(bool, Marshalling, (Parcel& parcel), (const, override));
    MOCK_METHOD(sptr<IRemoteBroker>, AsInterface, (), (override));
    MOCK_METHOD(int, Dump, (int fd, const std::vector<std::u16string>& args), (override));
};
}
}

#endif