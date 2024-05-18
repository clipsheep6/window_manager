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
    ScreenSessionManagerStub();

    ~ScreenSessionManagerStub() = default;

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    void ProcGetSnapshotByPicker(MessageParcel& reply);
    void ProcSetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply);
    void ProcGetVirtualScreenFlag(MessageParcel& data, MessageParcel& reply);
    void ProcGetAvailableArea(MessageParcel& data, MessageParcel& reply);
    int32_t GetDefaultDisplayInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t RegisterDisplayManagerAgentFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t UnregisterDisplayManagerAgentFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t WakeUpBeginFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t WakeUpEndFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SuspendBeginFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SuspendEndFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetDisplayStateFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetSpecificScreenPowerFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenPowerForAllFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDisplayStateFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t NotifyDisplayEventFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetScreenPowerFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDisplayByIdFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDisplayByScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetAllDisplayidsFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetScreenInfoByIdFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetAllScreenInfosFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetSupportedColorGamutsFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t CreateVirtualScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualScreenSurfaceFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualScreenCanvasRotationFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualScreenScaleModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t DestroyVirtualScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenMakeMirrorFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenStopMirrorFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenDisableMirrorFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenMakeExpandFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenStopExpandFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetScreenGroupInfoByIdFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t RemoveVirtualScreenFromScreenGroupFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDisplaySnapshotFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetSnapshotByPickerFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenActiveModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualPixelRatioFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualPixelRatioSystemFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetResolutionFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDensityInCurrentResolutionFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetColorGamutFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenSetColorGamutFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetGamutMapFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenSetGamutMapFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenSetColorTransformFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetPixelFormatFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenSetPixelFormatFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetSupportedHdrFormatFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetHdrFormatFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenSetHdrFormatFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetSupportedColorSpaceFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenGetColorSpaceFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ScreenSetColorSpaceFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetOrientationFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenRotationtionLockedFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t IsScreenRotationLockedFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetCutoutInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t HasPrivateWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ConvertScreenidToRsscreenidFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t HasImmersiveWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardDumpAllScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardDumpSpecialScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t DeviceIsCaptureFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardSetFoldDisplayModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardLockFoldDisplayStatusFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardGetFoldDisplayModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardIsFoldableFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardGetFoldStatusFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardGetCurrentFoldCreaseRegionFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SceneBoardMakeUniqueScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetClientFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetScreenPropertyFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDisplayNodeFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t UpdateScreenRotationPropertyFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetCurvedScreenCompressionAreaFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetPhyScreenPropertyFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t NotifyDisplayChangeInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenPrivacyStateFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenidPrivacyStateFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenPrivacyWindowListFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t ResizeVirtualScreenFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t UpdateAvailableAreaFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetScreenOffDelayTimeFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetAvailableAreaFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t NotifyFoldToExpandCompletionFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetVirtualScreenFlagFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualScreenFlagFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t GetDeviceScreenConfigFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SetVirtualScreenRefreshRateFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    int32_t SwitchUserFunc(MessageParcel& data, MessageParcel& reply, MessageOption &option);
    void InitGroup1();
    void InitGroup2();
    void InitGroup3();
    template<class F> void BindOp(DisplayManagerMessage code, F && func);
    std::unordered_map<DisplayManagerMessage,
        std::function<int32_t(MessageParcel& data, MessageParcel& reply, MessageOption& option)>> ioOps_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_INTERFACE_H