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
    int32_t ProcSetVirtualScreenSurface(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualScreenCanvasRotation(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualScreenScaleMode(MessageParcel& data, MessageParcel& reply);
    int32_t ProcDestoryVitrualScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenMakeMirror(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenStopMirror(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenDisableMirror(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenMakeExpand(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenStopExpand(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetScreenGroupInfoById(MessageParcel& data, MessageParcel& reply);
    int32_t ProcRemoveVirtualScreenFromScreenGroup(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDisplaySnapshot(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetSnapshotByPicker(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenActiveMode(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualPixelRatio(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualPixelRatioSystem(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetResolution(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDensityInCurrentResolution(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetColorGamut(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenSetColorGamut(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetGamutMap(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenSetGamutMap(MessageParcel& data, MessageParcel& reply);
    int32_t ProScreenSetColorTransform(MessageParcel& data, MessageParcel& reply);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H