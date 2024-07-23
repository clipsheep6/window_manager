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

#include "modal_system_ui_extension.h"

#include <atomic>
#include <memory>

#include <message_parcel.h>
#include <ability_manager_client.h>
#include <iremote_object.h>

#include "window_manager_hilog.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INVALID_USERID = -1;
constexpr int32_t MESSAGE_PARCEL_KEY_SIZE = 3;
constexpr int32_t VALUE_TYPE_STRING = 9;
constexpr int32_t DISCONNECT_ABILITY_DELAY_TIME_MS = 5000;
const std::string MODAL_UI_EXTENSION_THREAD = "OS_ModalSystemUiExtension";
} // namespace
std::shared_ptr<TaskScheduler> ModalSystemUiExtension::DialogAbilityConnection::taskScheduler_;

ModalSystemUiExtension::ModalSystemUiExtension() {}

ModalSystemUiExtension::~ModalSystemUiExtension()
{
    dialogConnectionCallback_ = nullptr;
}

bool ModalSystemUiExtension::CreateModalUIExtension(const AAFwk::Want& want)
{
    dialogConnectionCallback_ = sptr<OHOS::AAFwk::IAbilityConnection>(new DialogAbilityConnection(want));

    auto abilityManagerClient = AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "AbilityManagerClient is nullptr");
        return false;
    }

    AAFwk::Want systemUIWant;
    systemUIWant.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.systemdialog");
    auto result = abilityManagerClient->ConnectAbility(systemUIWant, dialogConnectionCallback_, INVALID_USERID);
    if (result != ERR_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "ConnectAbility failed, result = %{public}d", result);
        return false;
    }
    TLOGI(WmsLogTag::WMS_UIEXT, "ConnectAbility success");
    return true;
}

std::string ModalSystemUiExtension::ToString(const AAFwk::WantParams& wantParams_)
{
    std::string result;
    if (wantParams_.Size() != 0) {
        result += "{";
        for (auto it: wantParams_.GetParams()) {
            int typeId = AAFwk::WantParams::GetDataType(it.second);
            result += "\"" + it.first + "\":";
            if (typeId == VALUE_TYPE_STRING && AAFwk::WantParams::GetStringByType(it.second, typeId)[0] != '{') {
                result += "\"" + AAFwk::WantParams::GetStringByType(it.second, typeId) + "\"";
            } else {
                result += AAFwk::WantParams::GetStringByType(it.second, typeId);
            }
            if (it != *wantParams_.GetParams().rbegin()) {
                result += ",";
            }
        }
        result += "}";
    } else {
        result += "{}";
    }
    return result;
}

bool ModalSystemUiExtension::DialogAbilityConnection::SendWant(const sptr<IRemoteObject>& remoteObject)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInt32(MESSAGE_PARCEL_KEY_SIZE)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write message parcel key size failed");
        return false;
    }
    if (!data.WriteString16(u"bundleName") || !data.WriteString16(Str8ToStr16(want_.GetElement().GetBundleName()))) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write bundleName failed");
        return false;
    }
    if (!data.WriteString16(u"abilityName") || !data.WriteString16(Str8ToStr16(want_.GetElement().GetAbilityName()))) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write abilityName failed");
        return false;
    }
    if (!data.WriteString16(u"parameters") ||
        !data.WriteString16(Str8ToStr16(ModalSystemUiExtension::ToString(want_.GetParams())))) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write parameters failed");
        return false;
    }
    int32_t ret = remoteObject->SendRequest(AAFwk::IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "show dialog failed");
        return false;
    }
    return true;
}

void ModalSystemUiExtension::DialogAbilityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
    if (remoteObject == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "remoteObject is nullptr");
        return;
    }
    if (!SendWant(remoteObject)) {
        return;
    }

    auto task = [weakThis = wptr(this)]() {
        auto connection = weakThis.promote();
        if (!connection) {
            TLOGE(WmsLogTag::WMS_UIEXT, "session is null");
            return;
        }

        auto abilityManagerClient = AbilityManagerClient::GetInstance();
        if (abilityManagerClient == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "AbilityManagerClient is nullptr");
            return;
        }
        auto result = abilityManagerClient->DisconnectAbility(connection);
        if (result != ERR_OK) {
            TLOGE(WmsLogTag::WMS_UIEXT, "DisconnectAbility dialog failed, result = %{public}d", result);
        } else {
            TLOGI(WmsLogTag::WMS_UIEXT, "DisconnectAbility dialog successfull.");
        }
    };

    if (taskScheduler_ == nullptr) {
        taskScheduler_ = std::make_shared<TaskScheduler>(MODAL_UI_EXTENSION_THREAD);
    }
    taskScheduler_->PostAsyncTask(task, "DisconnectAbility", DISCONNECT_ABILITY_DELAY_TIME_MS);
}

void ModalSystemUiExtension::DialogAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element,
    int resultCode)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
}
} // namespace Rosen
} // namespace OHOS