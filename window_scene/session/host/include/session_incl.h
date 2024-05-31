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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_INCL_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_INCL_H
#include <list>
#include <mutex>
#include <shared_mutex>
#include <vector>

#include <event_handler.h>

#include "accessibility_element_info.h"
#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/host/include/zidl/session_stub.h"
#include "session/host/include/scene_persistence.h"
#include "wm_common.h"
#include "occupied_area_change_info.h"
#include "window_visibility_info.h"
#include "pattern_detach_callback_interface.h"

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
class AxisEvent;
enum class WindowArea;
} // namespace OHOS::MMI

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class RSSurfaceNode;
class RSTransaction;
class RSSyncTransactionController;
using NotifyPendingSessionActivationFunc = std::function<void(SessionInfo& info)>;
using NotifyChangeSessionVisibilityWithStatusBarFunc = std::function<void(SessionInfo& info, const bool visible)>;
using NotifySessionStateChangeFunc = std::function<void(const SessionState& state)>;
using NotifyBufferAvailableChangeFunc = std::function<void(const bool isAvailable)>;
using NotifySessionStateChangeNotifyManagerFunc = std::function<void(int32_t persistentId, const SessionState& state)>;
using NotifyRequestFocusStatusNotifyManagerFunc =
    std::function<void(int32_t persistentId, const bool isFocused, const bool byForeground, FocusChangeReason reason)>;
using NotifyBackPressedFunc = std::function<void(const bool needMoveToBackground)>;
using NotifySessionFocusableChangeFunc = std::function<void(const bool isFocusable)>;
using NotifySessionTouchableChangeFunc = std::function<void(const bool touchable)>;
using NotifyClickFunc = std::function<void()>;
using NotifyTerminateSessionFunc = std::function<void(const SessionInfo& info)>;
using NotifyTerminateSessionFuncNew =
    std::function<void(const SessionInfo& info, bool needStartCaller, bool isFromBroker)>;
using NotifyTerminateSessionFuncTotal = std::function<void(const SessionInfo& info, TerminateType terminateType)>;
using NofitySessionLabelUpdatedFunc = std::function<void(const std::string &label)>;
using NofitySessionIconUpdatedFunc = std::function<void(const std::string &iconPath)>;
using NotifySessionExceptionFunc = std::function<void(const SessionInfo& info, bool needRemoveSession)>;
using NotifySessionSnapshotFunc = std::function<void(const int32_t& persistentId)>;
using NotifyPendingSessionToForegroundFunc = std::function<void(const SessionInfo& info)>;
using NotifyPendingSessionToBackgroundForDelegatorFunc = std::function<void(const SessionInfo& info)>;
using NotifyRaiseToTopForPointDownFunc = std::function<void()>;
using NotifyUIRequestFocusFunc = std::function<void()>;
using NotifyUILostFocusFunc = std::function<void()>;
using GetStateFromManagerFunc = std::function<bool(const ManagerState key)>;
using NotifySessionInfoLockedStateChangeFunc = std::function<void(const bool lockedState)>;
using NotifySystemSessionPointerEventFunc = std::function<void(std::shared_ptr<MMI::PointerEvent> pointerEvent)>;
using NotifySessionInfoChangeNotifyManagerFunc = std::function<void(int32_t persistentid)>;
using NotifySystemSessionKeyEventFunc = std::function<bool(std::shared_ptr<MMI::KeyEvent> keyEvent,
    bool isPreImeEvent)>;
using NotifyContextTransparentFunc = std::function<void()>;

class ILifecycleListener {
public:
    virtual void OnActivation() = 0;
    virtual void OnConnect() = 0;
    virtual void OnForeground() = 0;
    virtual void OnBackground() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnExtensionDied() = 0;
    virtual void OnExtensionTimeout(int32_t errorCode) = 0;
    virtual void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) = 0;
};

enum class LifeCycleTaskType : uint32_t {
    START,
    STOP
};

enum class DetectTaskState : uint32_t {
    NO_TASK,
    ATTACH_TASK,
    DETACH_TASK
};

struct DetectTaskInfo {
    WindowMode taskWindowMode = WindowMode::WINDOW_MODE_UNDEFINED;
    DetectTaskState taskState = DetectTaskState::NO_TASK;
};

} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_INCL_H
