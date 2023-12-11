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
	int32_t DoGetDefaultDisplayInfo();
	int32_t DoGetDisplayInfoById(MessageParcel &data);
	int32_t DoGetDisplayInfoByScreen(MessageParcel &data);
	int32_t DoCreateVirtualScreen(MessageParcel &data);
	int32_t DoDestroyVirtualScreen(MessageParcel &data);
	int32_t DoSetVirtualScreenSurface(MessageParcel &data);
	int32_t DoSetOrientation(MessageParcel &data);
	int32_t DoGetDisplaySnapshot(MessageParcel &data);
	int32_t DoRegisterDisplayManagerAgent(MessageParcel &data);
	int32_t DoUnregisterDisplayManagerAgent(MessageParcel &data);
	int32_t DoWakeUpBegin(MessageParcel &data);
	int32_t DoWakeUpEnd();
	int32_t DoSuspendBegin(MessageParcel &data);
	int32_t DoSuspendEnd();
	int32_t DoSetSpecifiedScreenPower(MessageParcel &data);
	int32_t DoSetScreenPowerForAll(MessageParcel &data);
	int32_t DoGetScreenPower(MessageParcel &data);
	int32_t DoSetDisplayState(MessageParcel &data);
	int32_t DoGetDisplayState(MessageParcel &data);
	int32_t DoNotifyDisplayEvent(MessageParcel &data);
	int32_t DoSetFreeze(MessageParcel &data);
	int32_t DoMakeMirror(MessageParcel &data);
	int32_t DoGetScreenInfoById(MessageParcel &data);
	int32_t DoGetScreenGroupInfoById(MessageParcel &data);
	int32_t DoGetAllScreenInfos();
	int32_t DoGetAllDisplayIds();
	int32_t DoScreenMakeExpand(MessageParcel &data);
	int32_t DoRemoveVirtualScreenFromGroup(MessageParcel &data);
	int32_t DoSetScreenActiveMode(MessageParcel &data);
	int32_t DoSetVirtualPixelRatio(MessageParcel &data);
	int32_t DoGetScreenSupportedColorGamuts(MessageParcel &data);
	int32_t DoGetScreenColorGamut(MessageParcel &data);
	int32_t DoSetScreenColorGamut(MessageParcel &data);
	int32_t DoGetScreenGamutMap(MessageParcel &data);
	int32_t DoSetScreenGamutMap(MessageParcel &data);
	int32_t DoSetScreenColorTransform(MessageParcel &data);
	int32_t DoIsScreenRotationLocked(MessageParcel &data);
	int32_t DoSetScreenRotationLocked(MessageParcel &data);
	int32_t DoHasPrivateWindow(MessageParcel &data);
	int32_t DoGetCutoutInfo(MessageParcel &data);
	int32_t DoAddSurfaceNodeToDisplay(MessageParcel &data);
	int32_t DoRemoveSurfaceNodeFromDisplay(MessageParcel &data);
	int32_t DoStopMirror(MessageParcel &data);
	int32_t DoStopExpand(MessageParcel &data);
	int32_t DoResizeVirtualScreen(MessageParcel &data);
	int32_t DoMakeUniqueScreen(MessageParcel &data);
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_STUB_H