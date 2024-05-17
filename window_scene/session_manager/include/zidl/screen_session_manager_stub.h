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
    int32_t ProcScreenGetPixelFormat(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenSetPixelFormat(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetSupportedHDRFormats(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetHDRFormat(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenSetHDRFormat(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetSupprtedColorSpace(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetColorSpace(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenSetColorSpace(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetOrientation(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenRotationLocked(MessageParcel& data, MessageParcel& reply);
    int32_t ProcIsScreenRotationLocked(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetCutoutInfo(MessageParcel& data, MessageParcel& reply);
    int32_t ProcHasPrivateWindow(MessageParcel& data, MessageParcel& reply);
    int32_t ProcConvertScreenIdToRsScreenId(MessageParcel& data, MessageParcel& reply);
    int32_t ProcHasImmersiveWindow(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardDumpAllScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardDumpSpecialScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcDeviceIsCapture(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardSetFoldDisplayMode(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardLockFoldDisplayStatus(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardGetFoldDisplayMode(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardIsFoldable(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardGetFoldStatus(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSceneBoardGetCurrentFoldCreaseRegion(MessageParcel& data, MessageParcel& reply);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H