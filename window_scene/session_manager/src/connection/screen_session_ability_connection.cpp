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

#include "connection/screen_session_ability_connection.h"

#include <chrono>
#include <want.h>

#include "ability_connection.h"
#include "window_manager_hilog.h"
#include "ipc_skeleton.h"
#include "extension_manager_client.h"

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_VALUE = -1;
constexpr int32_t RES_FAILURE = -1;
constexpr int32_t RES_SUCCESS = 0;
constexpr uint32_t SEND_MESSAGE_SYNC_OUT_TIME = 800; // ms

void ScreenSessionAbilityConnectionStub::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    TLOGI(WmsLogTag::DMS, "OnAbilityConnectDone entry");
    if (resultCode != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "ability connect failed, error code:%{public}d", resultCode);
        return;
    }
    TLOGI(WmsLogTag::DMS, "ability connect success, ability name %{public}s", element.GetAbilityName().c_str());
    if (remoteObject == nullptr) {
        TLOGE(WmsLogTag::DMS, "get remoteObject failed");
        return;
    }
    remoteObject_ = remoteObject;
    if (!AddObjectDeathRecipient()) {
        TLOGE(WmsLogTag::DMS, "AddObjectDeathRecipient failed");
        return;
    }

    isConnected_.store(true);
    connectedCv_.notify_all();
    TLOGI(WmsLogTag::DMS, "OnAbilityConnectDone exit");
}

void ScreenSessionAbilityConnectionStub::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName &element, int32_t resultCode)
{
    TLOGI(WmsLogTag::DMS, "OnAbilityDisconnectDone entry");
    if (resultCode != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "ability disconnect failed, error code:%{public}d", resultCode);
    }

    TLOGI(WmsLogTag::DMS, "bundleName:%{public}s, abilityName:%{public}s, resultCode:%{public}d",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str(), resultCode);

    if (remoteObject_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "remoteObject member is nullptr");
        return;
    }
    remoteObject_->RemoveDeathRecipient(deathRecipient_);
    remoteObject_ = nullptr;
    isConnected_.store(false);
    TLOGI(WmsLogTag::DMS, "OnAbilityDisconnectDone exit");
}

bool ScreenSessionAbilityConnectionStub::AddObjectDeathRecipient()
{
    sptr<ScreenSessionAbilityDeathRecipient> deathRecipient(
        new(std::nothrow) ScreenSessionAbilityDeathRecipient([this] {
        TLOGI(WmsLogTag::DMS, "add death recipient handler");
        remoteObject_ = nullptr;
        isConnected_.store(false);
    }));

    if (deathRecipient == nullptr) {
        TLOGE(WmsLogTag::DMS, "create ScreenSessionAbilityDeathRecipient failed");
        return false;
    }
    deathRecipient_ = deathRecipient;
    if (remoteObject_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "get the remoteObject failed");
        return false;
    }
    if (!remoteObject_->AddDeathRecipient(deathRecipient_)) {
        TLOGE(WmsLogTag::DMS, "AddDeathRecipient failed");
        return false;
    }
    return true;
}

bool ScreenSessionAbilityConnectionStub::IsAbilityConnected()
{
    return isConnected_.load();
}

bool ScreenSessionAbilityConnectionStub::IsAbilityConnectedSync()
{
    std::unique_lock<std::mutex> lock(connectedMutex_);
    connectedCv_.wait_for(lock, std::chrono::milliseconds(SEND_MESSAGE_SYNC_OUT_TIME));
    return IsAbilityConnected();
}

int32_t ScreenSessionAbilityConnectionStub::SendMessageSync(int32_t transCode,
    MessageParcel &data, MessageParcel &reply)
{
    std::unique_lock<std::mutex> lock(connectedMutex_);
    connectedCv_.wait_for(lock, std::chrono::milliseconds(SEND_MESSAGE_SYNC_OUT_TIME));
    if (!IsAbilityConnected()) {
        TLOGE(WmsLogTag::DMS, "ability connection is not established");
        lock.unlock();
        return RES_FAILURE;
    }
    lock.unlock();
    MessageOption option;
    if (remoteObject_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "remoteObject is nullptr");
        return RES_FAILURE;
    }
    int32_t ret = remoteObject_->SendRequest(transCode, data, reply, option);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "remoteObject send request failed");
        return RES_FAILURE;
    }
    return RES_SUCCESS;
}

void ScreenSessionAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
{
    TLOGI(WmsLogTag::DMS, "OnRemoteDied entry");
    if (deathHandler_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "death handler is nullptr");
        return;
    }
    deathHandler_();
}

bool ScreenSessionAbilityConnection::ScreenSessionConnectExtension(
    const std::string &bundleName, const std::string &abilityName)
{
    TLOGI(WmsLogTag::DMS, "bundleName:%{public}s, abilityName:%{public}s", bundleName.c_str(), abilityName.c_str());
    if (abilityConnectionStub_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "screen session ability extension is already connected");
        return true;
    }
    AAFwk::Want want;
    want.SetElementName(bundleName, abilityName);
    abilityConnectionStub_ = sptr<ScreenSessionAbilityConnectionStub>(new (std::nothrow)
        ScreenSessionAbilityConnectionStub());
    if (abilityConnectionStub_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "screen session aibility connection is nullptr");
        return false;
    }

    // reset current callingIdentity for screen session
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub_, nullptr, DEFAULT_VALUE);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "ConnectServiceExtensionAbility failed, result: %{public}d", ret);
        // set current callingIdentity back
        IPCSkeleton::SetCallingIdentity(identity);
        return false;
    }
    TLOGI(WmsLogTag::DMS, "ConnectServiceExtensionAbility result: %{public}d", ret);
    // set current callingIdentity back
    IPCSkeleton::SetCallingIdentity(identity);
    return true;
}

void ScreenSessionAbilityConnection::ScreenSessionDisconnectExtension()
{
    if (abilityConnectionStub_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connect failed");
        return;
    }
    auto ret = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub_);
    if (ret == NO_ERROR) {
        abilityConnectionStub_.clear();
        abilityConnectionStub_ = nullptr;
    }
    TLOGI(WmsLogTag::DMS, "screen session ability disconnected, ret: %{public}d", ret);
}

int32_t ScreenSessionAbilityConnection::SendMessage(
    const int32_t &transCode, MessageParcel &data, MessageParcel &reply)
{
    if (abilityConnectionStub_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connection is nullptr");
        return RES_FAILURE;
    }
    int32_t ret = abilityConnectionStub_->SendMessageSync(transCode, data, reply);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "send message failed");
        return RES_FAILURE;
    }
    return RES_SUCCESS;
}

bool ScreenSessionAbilityConnection::IsConnected()
{
    if (abilityConnectionStub_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connect failed");
        return false;
    }
    return abilityConnectionStub_->IsAbilityConnected();
}

bool ScreenSessionAbilityConnection::IsConnectedSync()
{
    if (abilityConnectionStub_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connect failed");
        return false;
    }
    return abilityConnectionStub_->IsAbilityConnectedSync();
}
} // namespace OHOS::Rosen