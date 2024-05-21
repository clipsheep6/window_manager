/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_STUB_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_STUB_H

#include "display_manager_interface.h"

#include <iremote_stub.h>

namespace OHOS::Rosen {
class DisplayManagerStub : public IRemoteStub<IDisplayManager> {
public:
    DisplayManagerStub() = default;
    ~DisplayManagerStub() = default;
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    int32_t ProcGetfaultDisplayInfo(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetDisplayById(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetDisplayByScreen(MessageParcel &data, MessageParcel &reply);
    int32_t ProcCreateVirtualScreen(MessageParcel &data, MessageParcel &reply);
    int32_t ProcDestroyVirtualScreen(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetVirtualScreenSurface(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetOrirntation(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetDisplaySnapshot(MessageParcel &data, MessageParcel &reply);
    int32_t ProcRegisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply);
    int32_t ProcUnregisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply);
    int32_t ProcWakeUpBegin(MessageParcel &data, MessageParcel &reply);
    int32_t ProcWakeUpEnd(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSuspendBegin(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSuspendEnd(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetSpecifiedScreenPower(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetScreenPowerForAll(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetScreenPower(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetDisplayState(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetDisplayState(MessageParcel &data, MessageParcel &reply);
    int32_t ProcNotifyDisplayEvent(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetFreezeEvent(MessageParcel &data, MessageParcel &reply);
    int32_t ProcMakeScreenMirror(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetScreenInfoById(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetScreenGroupInfoById(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetAllScreenInfos(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetAllDisplayIds(MessageParcel &data, MessageParcel &reply);
    int32_t ProcMakeScreenExpand(MessageParcel &data, MessageParcel &reply);
    int32_t ProcRemoveVirtualScreenFromScreenGroup(MessageParcel &data, MessageParcel &reply);
    int32_t ProSetScreenActiveMode(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetVirtualPixelRatio(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetScreenSupportedColorGamuts(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetScreenColorGamut(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetScreenColorGamut(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetScreenGamutMap(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetScreenGamutMap(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetScreenColorTransfrom(MessageParcel &data, MessageParcel &reply);
    int32_t ProcIsScreenRotationLocked(MessageParcel &data, MessageParcel &reply);
    int32_t ProcSetScreenRotationLocked(MessageParcel &data, MessageParcel &reply);
    int32_t ProcHasPrivateWindow(MessageParcel &data, MessageParcel &reply);
    int32_t ProcGetCutoutInfo(MessageParcel &data, MessageParcel &reply);
    int32_t ProcAddSurfaceNode(MessageParcel &data, MessageParcel &reply);
    int32_t ProcRemoveSurfaceNode(MessageParcel &data, MessageParcel &reply);
    int32_t ProcStopScreenMirror(MessageParcel &data, MessageParcel &reply);
    int32_t ProcStopScreenExpand(MessageParcel &data, MessageParcel &reply);
    int32_t ProcResizeVirtualScreen(MessageParcel &data, MessageParcel &reply);
    int32_t ProcMakeUniqueScreen(MessageParcel &data, MessageParcel &reply);
    using StubFunc = int(DisplayManagerStub::*)(MessageParcel& data, MessageParcel& reply);
    static const std::map<DisplayManagerMessage, StubFunc> stubFuncMap_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_STUB_H