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

#include "zidl/window_manager_stub.h"
#include <ipc_skeleton.h>
#include <key_event.h>
#include <rs_iwindow_animation_controller.h>
#include <rs_window_animation_target.h>

#include "marshalling_helper.h"
#include "memory_guard.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerStub"};
}

template<class F>
void WindowManagerStub::BindOp(WindowManagerMessage code, F&& func)
{
    using namespace std::placeholders;
    ioOps_[code] = std::bind(func, this, _1, _2, _3);
}

WindowManagerStub::WindowManagerStub()
{
    BindOp(WindowManagerMessage::TRANS_ID_CREATE_WINDOW, &WindowManagerStub::CreateWindowFunc);
    BindOp(WindowManagerMessage::TRANS_ID_ADD_WINDOW, &WindowManagerStub::AddWindowFunc);
    BindOp(WindowManagerMessage::TRANS_ID_REMOVE_WINDOW, &WindowManagerStub::RemoveWindowFunc);
    BindOp(WindowManagerMessage::TRANS_ID_DESTROY_WINDOW, &WindowManagerStub::DestroyWindowFunc);
    BindOp(WindowManagerMessage::TRANS_ID_REQUEST_FOCUS, &WindowManagerStub::RequestFocusFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_AVOID_AREA, &WindowManagerStub::GetAvoidAreaFunc);
    BindOp(WindowManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT,
           &WindowManagerStub::RegisterWindowManagerAgentFunc);
    BindOp(WindowManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT,
           &WindowManagerStub::UnregisterWindowManagerAgentFunc);
    BindOp(WindowManagerMessage::TRANS_ID_NOTIFY_READY_MOVE_OR_DRAG, &WindowManagerStub::NotifyReadyMoveOrDragFunc);
    BindOp(WindowManagerMessage::TRANS_ID_PROCESS_POINT_DOWN, &WindowManagerStub::ProcessPointDownFunc);
    BindOp(WindowManagerMessage::TRANS_ID_PROCESS_POINT_UP, &WindowManagerStub::ProcessPointUpFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_TOP_WINDOW_ID, &WindowManagerStub::GetTopWindowIdFunc);
    BindOp(WindowManagerMessage::TRANS_ID_MINIMIZE_ALL_APP_WINDOWS, &WindowManagerStub::MinimizeAllAppWindowsFunc);
    BindOp(WindowManagerMessage::TRANS_ID_TOGGLE_SHOWN_STATE_FOR_ALL_APP_WINDOWS,
           &WindowManagerStub::ToggleShownStateForAllAppWindowsFunc);
    BindOp(WindowManagerMessage::TRANS_ID_UPDATE_LAYOUT_MODE, &WindowManagerStub::UpdateLayoutFunc);
    BindOp(WindowManagerMessage::TRANS_ID_UPDATE_PROPERTY, &WindowManagerStub::UpdatePropertyFunc);
    BindOp((WindowManagerMessage)19U, // restricted word, need add exception so usage by value not needed
           &WindowManagerStub::GetAccessibilityWindowInfoFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_VISIBILITY_WINDOW_INFO_ID,
           &WindowManagerStub::GetVisibilityWindowInfoFunc);
    BindOp(WindowManagerMessage::TRANS_ID_ANIMATION_SET_CONTROLLER, &WindowManagerStub::AnimationSetControllerFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_SYSTEM_CONFIG, &WindowManagerStub::GetSystemConfigFunc);
    BindOp(WindowManagerMessage::TRANS_ID_NOTIFY_WINDOW_TRANSITION, &WindowManagerStub::NotifyWindowTransitionFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_FULLSCREEN_AND_SPLIT_HOT_ZONE,
           &WindowManagerStub::GetModeChangeHotZonesFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_ANIMATION_CALLBACK, &WindowManagerStub::GetAnimationCallbackFunc);
    BindOp(WindowManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER, &WindowManagerStub::UpdateAvoidAreaListenerwFunc);
    BindOp(WindowManagerMessage::TRANS_ID_UPDATE_RS_TREE, &WindowManagerStub::UpdateRsTreeFunc);
    BindOp(WindowManagerMessage::TRANS_ID_BIND_DIALOG_TARGET, &WindowManagerStub::BindDialogTargetFunc);
    BindOp(WindowManagerMessage::TRANS_ID_SET_ANCHOR_AND_SCALE, &WindowManagerStub::SetAnchorAndScaleFunc);
    BindOp(WindowManagerMessage::TRANS_ID_SET_ANCHOR_OFFSET, &WindowManagerStub::SetAnchorOffsetFunc);
    BindOp(WindowManagerMessage::TRANS_ID_OFF_WINDOW_ZOOM, &WindowManagerStub::OffWindowZoomFunc);
    BindOp(WindowManagerMessage::TRANS_ID_RAISE_WINDOW_Z_ORDER, &WindowManagerStub::RaiseToAppTopFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_SNAPSHOT, &WindowManagerStub::GetSnapshotFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GESTURE_NAVIGATION_ENABLED,
           &WindowManagerStub::SetGestureNavigaionEnabledFunc);
    BindOp(WindowManagerMessage::TRANS_ID_SET_WINDOW_GRAVITY, &WindowManagerStub::SetWindowGravityFunc);
    BindOp(WindowManagerMessage::TRANS_ID_DISPATCH_KEY_EVENT, &WindowManagerStub::DispatchKeyEventFunc);
    BindOp(WindowManagerMessage::TRANS_ID_NOTIFY_DUMP_INFO_RESULT, &WindowManagerStub::NotifyDumpInfoResultFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_WINDOW_ANIMATION_TARGETS,
           &WindowManagerStub::GetWindowAnimationTargetsFunc);
    BindOp(WindowManagerMessage::TRANS_ID_SET_MAXIMIZE_MODE, &WindowManagerStub::SetMaximizeModeFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_MAXIMIZE_MODE, &WindowManagerStub::GetMaximizeModeFunc);
    BindOp(WindowManagerMessage::TRANS_ID_GET_FOCUS_WINDOW_INFO, &WindowManagerStub::GetFocusWindowInfoFunc);
}

int32_t WindowManagerStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
                                           MessageParcel& reply, MessageOption& option)
{
    MemoryGuard cacheGuard;
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    auto msgId = static_cast<WindowManagerMessage>(code);

    if (auto ite = ioOps_.find(msgId); ite != ioOps_.cend()) {
        return ite->second(data, reply, option);
    } else {
        WLOGFW("unknown transaction code %{public}d", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

// WindowManagerMessage::TRANS_ID_CREATE_WINDOW
int32_t WindowManagerStub::CreateWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    sptr<IRemoteObject> windowObject = data.ReadRemoteObject();
    sptr<IWindow> windowProxy = iface_cast<IWindow>(windowObject);
    sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
    uint32_t windowId;
    sptr<IRemoteObject> token = nullptr;
    if (windowProperty && windowProperty->GetTokenState()) {
        token = data.ReadRemoteObject();
    } else {
        WLOGI("accept token is nullptr");
    }
    WMError errCode = CreateWindow(windowProxy, windowProperty, surfaceNode, windowId, token);
    reply.WriteUint32(windowId);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    if (windowProperty) {
        reply.WriteUint32(windowProperty->GetWindowFlags());
        reply.WriteUint32(windowProperty->GetApiCompatibleVersion());
    }
    return 0;
}

// WindowManagerMessage::TRANS_ID_ADD_WINDOW
int32_t WindowManagerStub::AddWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
    WMError errCode = AddWindow(windowProperty);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_REMOVE_WINDOW
int32_t WindowManagerStub::RemoveWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    bool isFromInnerkits = data.ReadBool();
    WMError errCode = RemoveWindow(windowId, isFromInnerkits);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_DESTROY_WINDOW
int32_t WindowManagerStub::DestroyWindowFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    WMError errCode = DestroyWindow(windowId);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_REQUEST_FOCUS
int32_t WindowManagerStub::RequestFocusFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    WMError errCode = RequestFocus(windowId);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_Aint32_t_AREA
int32_t WindowManagerStub::GetAvoidAreaFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    auto avoidAreaType = static_cast<AvoidAreaType>(data.ReadUint32());
    AvoidArea avoidArea = GetAvoidAreaByType(windowId, avoidAreaType);
    reply.WriteParcelable(&avoidArea);
    return 0;
}

// WindowManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT
int32_t WindowManagerStub::RegisterWindowManagerAgentFunc(MessageParcel& data,
                                                          MessageParcel& reply, MessageOption& option)
{
    auto type = static_cast<WindowManagerAgentType>(data.ReadUint32());
    sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
    sptr<IWindowManagerAgent> windowManagerAgentProxy = iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
    WMError errCode = RegisterWindowManagerAgent(type, windowManagerAgentProxy);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT
int32_t WindowManagerStub::UnregisterWindowManagerAgentFunc(MessageParcel& data,
                                                            MessageParcel& reply,
                                                            MessageOption& option)
{
    auto type = static_cast<WindowManagerAgentType>(data.ReadUint32());
    sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
    sptr<IWindowManagerAgent> windowManagerAgentProxy = iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
    WMError errCode = UnregisterWindowManagerAgent(type, windowManagerAgentProxy);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_NOTIFY_READY_MOVE_OR_DRAG
int32_t WindowManagerStub::NotifyReadyMoveOrDragFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
    sptr<MoveDragProperty> moveDragProperty = data.ReadStrongParcelable<MoveDragProperty>();
    NotifyServerReadyToMoveOrDrag(windowId, windowProperty, moveDragProperty);
    return 0;
}

// WindowManagerMessage::TRANS_ID_PROCESS_POINT_DOWN
int32_t WindowManagerStub::ProcessPointDownFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    bool isPointDown = data.ReadBool();
    ProcessPointDown(windowId, isPointDown);
    return 0;
}

// WindowManagerMessage::TRANS_ID_PROCESS_POINT_UP
int32_t WindowManagerStub::ProcessPointUpFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    ProcessPointUp(windowId);
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_TOP_WINDOW_ID
int32_t WindowManagerStub::GetTopWindowIdFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t mainWinId = data.ReadUint32();
    uint32_t topWinId;
    WMError errCode = GetTopWindowId(mainWinId, topWinId);
    reply.WriteUint32(topWinId);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_MINIMIZE_ALL_APP_WINDOWS
int32_t WindowManagerStub::MinimizeAllAppWindowsFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    WMError errCode = MinimizeAllAppWindows(data.ReadUint64());
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_TOGGLE_SHOWN_STATE_FOR_ALL_APP_WINDOWS
int32_t WindowManagerStub::ToggleShownStateForAllAppWindowsFunc(MessageParcel& data,
                                                                MessageParcel& reply,
                                                                MessageOption& option)
{
    WMError errCode = ToggleShownStateForAllAppWindows();
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_UPDATE_LAYOUT_MODE
int32_t WindowManagerStub::UpdateLayoutFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto mode = static_cast<WindowLayoutMode>(data.ReadUint32());
    WMError errCode = SetWindowLayoutMode(mode);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_UPDATE_PROPERTY
int32_t WindowManagerStub::UpdatePropertyFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    auto action = static_cast<PropertyChangeAction>(data.ReadUint32());
    sptr<WindowProperty> windowProperty = new WindowProperty();
    windowProperty->Read(data, action);
    WMError errCode = UpdateProperty(windowProperty, action);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_ACCESS...
int32_t WindowManagerStub::GetAccessibilityWindowInfoFunc(MessageParcel& data,
                                                          MessageParcel& reply, MessageOption& option)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    WMError errCode = GetAccessibilityWindowInfo(infos);
    if (!MarshallingHelper::MarshallingVectorParcelableObj<AccessibilityWindowInfo>(reply, infos)) {
        WLOGFE("Write accessib window infos failed");
        return -1;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_VISIBILITY_WINDOW_INFO_ID
int32_t WindowManagerStub::GetVisibilityWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    std::vector<sptr<WindowVisibilityInfo>> infos;
    WMError errCode = GetVisibilityWindowInfo(infos);
    if (!MarshallingHelper::MarshallingVectorParcelableObj<WindowVisibilityInfo>(reply, infos)) {
        WLOGFE("Write visibility window infos failed");
        return -1;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_ANIMATION_SET_CONTROLLER
int32_t WindowManagerStub::AnimationSetControllerFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    sptr<IRemoteObject> controllerObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationController> controller = iface_cast<RSIWindowAnimationController>(controllerObject);
    WMError errCode = SetWindowAnimationController(controller);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_SYSTEM_CONFIG
int32_t WindowManagerStub::GetSystemConfigFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    SystemConfig config;
    WMError errCode = GetSystemConfig(config);
    reply.WriteParcelable(&config);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_NOTIFY_WINDOW_TRANSITION
int32_t WindowManagerStub::NotifyWindowTransitionFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    sptr<WindowTransitionInfo> from = data.ReadParcelable<WindowTransitionInfo>();
    sptr<WindowTransitionInfo> to = data.ReadParcelable<WindowTransitionInfo>();
    bool isFromClient = data.ReadBool();
    WMError errCode = NotifyWindowTransition(from, to, isFromClient);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_FULLSCREEN_AND_SPLIT_HOT_ZONE
int32_t WindowManagerStub::GetModeChangeHotZonesFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    DisplayId displayId = data.ReadUint64();
    ModeChangeHotZones hotZones = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    WMError errCode = GetModeChangeHotZones(displayId, hotZones);
    reply.WriteInt32(static_cast<int32_t>(errCode));

    reply.WriteInt32(hotZones.fullscreen_.posX_);
    reply.WriteInt32(hotZones.fullscreen_.posY_);
    reply.WriteUint32(hotZones.fullscreen_.width_);
    reply.WriteUint32(hotZones.fullscreen_.height_);

    reply.WriteInt32(hotZones.primary_.posX_);
    reply.WriteInt32(hotZones.primary_.posY_);
    reply.WriteUint32(hotZones.primary_.width_);
    reply.WriteUint32(hotZones.primary_.height_);

    reply.WriteInt32(hotZones.secondary_.posX_);
    reply.WriteInt32(hotZones.secondary_.posY_);
    reply.WriteUint32(hotZones.secondary_.width_);
    reply.WriteUint32(hotZones.secondary_.height_);
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_ANIMATION_CALLBACK
int32_t WindowManagerStub::GetAnimationCallbackFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    std::vector<uint32_t> windowIds;
    data.ReadUInt32Vector(&windowIds);
    bool isAnimated = data.ReadBool();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback = nullptr;
    MinimizeWindowsByLauncher(windowIds, isAnimated, finishedCallback);
    if (finishedCallback == nullptr) {
        if (!reply.WriteBool(false)) {
            WLOGFE("finishedCallback is nullptr and failed to write!");
        }
    } else {
        if (!reply.WriteBool(true) || !reply.WriteRemoteObject(finishedCallback->AsObject())) {
            WLOGFE("finishedCallback is not nullptr and failed to write!");
        }
    }
    return 0;
}

// WindowManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER
int32_t WindowManagerStub::UpdateAvoidAreaListenerwFunc(MessageParcel& data, MessageParcel& reply,
                                                        MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    bool haveAvoidAreaListener = data.ReadBool();
    WMError errCode = UpdateAvoidAreaListener(windowId, haveAvoidAreaListener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_UPDATE_RS_TREE
int32_t WindowManagerStub::UpdateRsTreeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    bool isAdd = data.ReadBool();
    WMError errCode = UpdateRsTree(windowId, isAdd);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_BIND_DIALOG_TARGET
int32_t WindowManagerStub::BindDialogTargetFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    sptr<IRemoteObject> targetToken = data.ReadRemoteObject();
    WMError errCode = BindDialogTarget(windowId, targetToken);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_SET_ANCHOR_AND_SCALE
int32_t WindowManagerStub::SetAnchorAndScaleFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int32_t x = data.ReadInt32();
    int32_t y = data.ReadInt32();
    float scale = data.ReadFloat();
    SetAnchorAndScale(x, y, scale);
    return 0;
}

// WindowManagerMessage::TRANS_ID_SET_ANCHOR_OFFSET
int32_t WindowManagerStub::SetAnchorOffsetFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    int32_t deltaX = data.ReadInt32();
    int32_t deltaY = data.ReadInt32();
    SetAnchorOffset(deltaX, deltaY);
    return 0;
}

// WindowManagerMessage::TRANS_ID_OFF_WINDOW_ZOOM
int32_t WindowManagerStub::OffWindowZoomFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    OffWindowZoom();
    return 0;
}

// WindowManagerMessage::TRANS_ID_RAISE_WINDOW_Z_ORDER
int32_t WindowManagerStub::RaiseToAppTopFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    WmErrorCode errCode = RaiseToAppTop(windowId);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_SNAPSHOT
int32_t WindowManagerStub::GetSnapshotFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    std::shared_ptr<Media::PixelMap> pixelMap = GetSnapshot(windowId);
    reply.WriteParcelable(pixelMap.get());
    return 0;
}

// WindowManagerMessage::TRANS_ID_GESTURE_NAVIGATION_ENABLED
int32_t WindowManagerStub::SetGestureNavigaionEnabledFunc(MessageParcel& data,
                                                          MessageParcel& reply, MessageOption& option)
{
    bool enable = data.ReadBool();
    WMError errCode = SetGestureNavigaionEnabled(enable);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_SET_WINDOW_GRAVITY
int32_t WindowManagerStub::SetWindowGravityFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    WindowGravity gravity = static_cast<WindowGravity>(data.ReadUint32());
    uint32_t percent = data.ReadUint32();
    WMError errCode = SetWindowGravity(windowId, gravity, percent);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_DISPATCH_KEY_EVENT
int32_t WindowManagerStub::DispatchKeyEventFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    uint32_t windowId = data.ReadUint32();
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->ReadFromParcel(data);
    DispatchKeyEvent(windowId, event);
    return 0;
}

// WindowManagerMessage::TRANS_ID_NOTIFY_DUMP_INFO_RESULT
int32_t WindowManagerStub::NotifyDumpInfoResultFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    std::vector<std::string> info;
    data.ReadStringVector(&info);
    NotifyDumpInfoResult(info);
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_WINDOW_ANIMATION_TARGETS
int32_t WindowManagerStub::GetWindowAnimationTargetsFunc(MessageParcel& data,
                                                         MessageParcel& reply, MessageOption& option)
{
    std::vector<uint32_t> missionIds;
    data.ReadUInt32Vector(&missionIds);
    std::vector<sptr<RSWindowAnimationTarget>> targets;
    auto errCode = GetWindowAnimationTargets(missionIds, targets);
    if (!MarshallingHelper::MarshallingVectorParcelableObj<RSWindowAnimationTarget>(reply, targets)) {
        WLOGFE("Write window animation targets failed");
        return -1;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_SET_MAXIMIZE_MODE
int32_t WindowManagerStub::SetMaximizeModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    MaximizeMode maximizeMode = static_cast<MaximizeMode>(data.ReadUint32());
    SetMaximizeMode(maximizeMode);
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_MAXIMIZE_MODE
int32_t WindowManagerStub::GetMaximizeModeFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    MaximizeMode maximizeMode = GetMaximizeMode();
    reply.WriteInt32(static_cast<int32_t>(maximizeMode));
    return 0;
}

// WindowManagerMessage::TRANS_ID_GET_FOCUS_WINDOW_INFO
int32_t WindowManagerStub::GetFocusWindowInfoFunc(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    FocusChangeInfo focusInfo;
    GetFocusWindowInfo(focusInfo);
    reply.WriteParcelable(&focusInfo);
    return 0;
}

} // namespace Rosen
} // namespace OHOS
