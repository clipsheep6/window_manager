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

#include <unordered_map>

namespace OHOS {
namespace Rosen {

class ScreenSessionManagerStub : public IRemoteStub<IScreenSessionManager> {
public:
    ScreenSessionManagerStub() = default;

    ~ScreenSessionManagerStub() = default;

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    int32_t ProcGetSnapshotByPicker(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetAvailableArea(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDefaultDisplayInfo(MessageParcel& data, MessageParcel& reply);
    int32_t ProcRegisterDisplayManagerAgent(MessageParcel& data, MessageParcel& reply);
    int32_t ProcUnregisterDisplayManagerAgent(MessageParcel& data, MessageParcel& reply);
    int32_t ProcWakeUpBegin(MessageParcel& data, MessageParcel& reply);
    int32_t ProcWakeUpEnd(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSuspendBegin(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSuspendEnd(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetDisplayState(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetSpecifiedScreenPower(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenPowerForAll(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDisplayState(MessageParcel& data, MessageParcel& reply);
    int32_t ProcNotifyDisplayEvent(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetScreenPower(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDisplayById(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDisplayByScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetAllDisplayIds(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetScreenInfoById(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetAllScreenInfos(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetSupportedColorGamuts(MessageParcel& data, MessageParcel& reply);
    int32_t ProcCreateVirtualScreen(MessageParcel& data, MessageParcel& reply);
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
    int32_t ProcScreenGetPixelFormat(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenSetPixelFormat(MessageParcel& data, MessageParcel& reply);
    int32_t ProcScreenGetSupportedHDRFormat(MessageParcel& data, MessageParcel& reply);
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
    int32_t ProcSceneBoardMakeUniqueScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetClient(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetScreenProperty(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDisplayNode(MessageParcel& data, MessageParcel& reply);
    int32_t ProcUpdateScreenRotationProperty(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetCurvedScreenCompressionArea(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetPhyScreenProperty(MessageParcel& data, MessageParcel& reply);
    int32_t ProcNotifyDisplayChangeInfo(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenPrivacyState(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenIdPrivacyState(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenPrivacyWindowList(MessageParcel& data, MessageParcel& reply);
    int32_t ProcResizeVirtualScreen(MessageParcel& data, MessageParcel& reply);
    int32_t ProcUpdateAvailableArea(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetScreenOffDelayTime(MessageParcel& data, MessageParcel& reply);
    int32_t ProcNotifyFoldToExpandCompletion(MessageParcel& data, MessageParcel& reply);
    int32_t ProcGetDeviceScreenConfig(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSetVirtualScreenRefreshRate(MessageParcel& data, MessageParcel& reply);
    int32_t ProcSwitchUser(MessageParcel& data, MessageParcel& reply);

    using ProcFuncPtr = int32_t(ScreenSessionManagerStub::*)(MessageParcel&, MessageParcel&);
    using ProcFuncMap = std::unordered_map<DisplayManagerMessage, ProcFuncPtr>;
    static const ProcFuncMap procFuncMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H