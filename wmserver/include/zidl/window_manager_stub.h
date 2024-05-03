/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_MANAGER_STUB_H
#define OHOS_WINDOW_MANAGER_STUB_H

#include "window_manager_interface.h"
#include <iremote_stub.h>
#include <ui/rs_surface_node.h>

namespace OHOS {
namespace Rosen {
class WindowManagerStub : public IRemoteStub<IWindowManager> {
public:
    WindowManagerStub();
    ~WindowManagerStub() = default;
    virtual int32_t
        OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    void CreateWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void AddWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void RemoveWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void DestroyWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void RequestFocusFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetAvoidAreaFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void RegisterWindowManagerAgentFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void UnregisterWindowManagerAgentFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void NotifyReadyMoveOrDragFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void ProcessPointDownFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void ProcessPointUpFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetTopWindowIdFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void MinimizeAllAppWindowsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void ToggleShownStateForAllAppWindowsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void UpdateLayoutFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void UpdatePropertyFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetAccessibilityWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetVisibilityWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void AnimationSetControllerFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetSystemConfigFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void NotifyWindowTransitionFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetModeChangeHotZonesFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetAnimationCallbackFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void UpdateAvoidAreaListenerwFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void UpdateRsTreeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void BindDialogTargetFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void SetAnchorAndScaleFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void SetAnchorOffsetFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void OffWindowZoomFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void RaiseToAppTopFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetSnapshotFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void SetGestureNavigaionEnabledFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void SetWindowGravityFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void DispatchKeyEventFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void NotifyDumpInfoResultFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetWindowAnimationTargetsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void SetMaximizeModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetMaximizeModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    void GetFocusWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);

private: // data
    std::unordered_map<WindowManagerMessage,
                       std::function<void(MessageParcel& data, MessageParcel& reply, MessageOption& option)>>
        ioOps_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_STUB_H
