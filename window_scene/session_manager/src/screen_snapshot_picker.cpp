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

#include "screen_snapshot_picker.h"

#include <string>

#include "screen_snapshot_ability_connection.h"
#include "window_manager_hilog.h"
#include "ipc_skeleton.h"
#include "extension_manager_client.h"

namespace OHOS::Rosen {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSnapshotPicker" };
constexpr int32_t TRANS_CMD_SEND_SNAPSHOT_RECT = 2;
constexpr int32_t DEFAULT_VALUE = -1;
constexpr int32_t RES_FAILURE = -1;
constexpr int32_t RES_SUCCESS = 0;
const std::string SCREEN_SNAPSHOT_BUNDLE_NAME = "com.huawei.ohos.screenshot";
const std::string SCREEN_SNAPSHOT_SERVICE_ABILITY_NAME = "com.huawei.ohos.screenshot.ServiceExtAbility";

ScreenSnapshotPicker &ScreenSnapshotPicker::GetInstance()
{
    static ScreenSnapshotPicker screenSnapshotPicker;
    return screenSnapshotPicker;
}

bool ScreenSnapshotPicker::SnapshotPickerConnectExtension()
{
    Want want;
    want.SetElementName(SCREEN_SNAPSHOT_BUNDLE_NAME, SCREEN_SNAPSHOT_SERVICE_ABILITY_NAME);
    bool ret = SnapshotPickerConnectExtensionAbility(want);
    if (!ret) {
        WLOGFE("SnapshotPickerConnectExtensionAbility failed.");
        return false;
    }
    WLOGFI("SnapshotPickerConnectExtension succeed.");
    return true;
}

bool ScreenSnapshotPicker::SnapshotPickerConnectExtensionAbility(const Want &want)
{
    if (abilityConnection_ != nullptr) {
        WLOGFI("screen snapshot already connected");
        return true;
    }
    abilityConnection_ = sptr<ScreenSnapshotAbilityConnection> (new(std::nothrow)
        ScreenSnapshotAbilityConnection());
    if (!abilityConnection_) {
        WLOGFE("connection is nullptr.");
        return false;
    }

    WLOGFI("calling pid: %{public}d, uid: %{public}d, fullTokenId: %{public}" PRIu64"",
        IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingFullTokenID());

    // reset current callingIdentity to screen snaptshot.
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnection_, nullptr, DEFAULT_VALUE);
    if (ret != ERR_OK) {
        WLOGFE("ConnectServiceExtensionAbility failed, result: %{public}d", ret);
        // set current callingIdentity back.
        IPCSkeleton::SetCallingIdentity(identity);
        return false;
    }
    WLOGFI("ConnectServiceExtensionAbility result: %{public}d", ret);
    // set current callingIdentity back.
    IPCSkeleton::SetCallingIdentity(identity);
    return true;
}

int32_t ScreenSnapshotPicker::GetScreenSnapshotInfo(Media::Rect &rect, ScreenId &screenId)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(Str8ToStr16("SA"));
    data.WriteString16(Str8ToStr16("ScreenSessionManager"));

    if (abilityConnection_ == nullptr) {
        WLOGFE("ability connection is nullptr");
        return RES_FAILURE;
    }
    int32_t ret = abilityConnection_->SendMessageSync(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply);
    if (ret != ERR_OK) {
        WLOGFE("send message failed");
        return RES_FAILURE;
    }
    screenId = static_cast<ScreenId>(reply.ReadInt32());
    rect.left = reply.ReadInt32();
    rect.top = reply.ReadInt32();
    rect.width = reply.ReadInt32();
    rect.height = reply.ReadInt32();
    WLOGFI("snapshot area info screenId:%{public}" PRIu64", \
        left:%{public}d, top:%{public}d, width:%{public}d, height:%{public}d",
        screenId, rect.left, rect.top, rect.width, rect.height);

    return RES_SUCCESS;
}

void ScreenSnapshotPicker::SnapshotPickerDisconnectExtension()
{
    if (abilityConnection_ == nullptr) {
        WLOGFE("ability connect failed");
        return;
    }

    auto ret = ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnection_);
    if (ret == NO_ERROR) {
        abilityConnection_.clear();
        abilityConnection_ = nullptr;
    }
    WLOGFI("SnapshotPickerDisconnectExtension exit, ret:%{public}d", ret);
}

} // namespace OHOS::Rosen