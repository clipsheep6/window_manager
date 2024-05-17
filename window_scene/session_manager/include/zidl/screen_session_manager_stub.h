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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_STUB_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_STUB_H

#include "screen_session_manager_interface.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Rosen {

class ScreenSessionManagerStub : public IRemoteStub<IScreenSessionManager> {
public:
    ScreenSessionManagerStub() = default;

    ~ScreenSessionManagerStub() = default;

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    void ProcGetSnapshotByPicker(MessageParcel& reply);
    void ProcSetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply);
    void ProcGetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply);
    void ProcGetAvailableArea(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardMakeUniqueScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetClient(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetScreenProperty(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDisplayNode(MessageParcel& data, MessageParcel& reply);
    int32_t ProcUpdateScreenRotationProperty(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetCurvedScreenCompressionArea(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetPhyScreenProperty(MessageParcel& data, MessageParcel& reply);
    int32_t ProcNotifyDisplayChangeInfo(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenPrivacyState(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSecScreenIdPrivacyState(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenPrivacyWindowList(MessageParcel& data, MessageParcel& reply);
    int32_t ProcResizeVirtualScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcUpdateAvailableArea(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenOffDelayTime(MessageParcel& data, MessageParcel& reply);
    int32_t ProcNotifyFoldToExpandCompletion(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDeviceScreenConfig(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualScreenRefreshRate(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSwitchUser(MessageParcel& data, MessageParcel& reply);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H