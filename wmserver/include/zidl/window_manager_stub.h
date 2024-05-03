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
    int32_t CreateWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t AddWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t RemoveWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DestroyWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t RequestFocusFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetAvoidAreaFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t RegisterWindowManagerAgentFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t UnregisterWindowManagerAgentFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t NotifyReadyMoveOrDragFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t ProcessPointDownFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t ProcessPointUpFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetTopWindowIdFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t MinimizeAllAppWindowsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t ToggleShownStateForAllAppWindowsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t UpdateLayoutFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t UpdatePropertyFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetAccessibilityWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetVisibilityWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t AnimationSetControllerFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetSystemConfigFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t NotifyWindowTransitionFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetModeChangeHotZonesFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetAnimationCallbackFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t UpdateAvoidAreaListenerwFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t UpdateRsTreeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t BindDialogTargetFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t SetAnchorAndScaleFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t SetAnchorOffsetFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t OffWindowZoomFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t RaiseToAppTopFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetSnapshotFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t SetGestureNavigaionEnabledFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t SetWindowGravityFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DispatchKeyEventFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t NotifyDumpInfoResultFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetWindowAnimationTargetsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t SetMaximizeModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetMaximizeModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t GetFocusWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option);

private: // data
    std::unordered_map<WindowManagerMessage,
                       std::function<int32_t(MessageParcel& data, MessageParcel& reply, MessageOption& option)>>
        ioOps_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_STUB_H
