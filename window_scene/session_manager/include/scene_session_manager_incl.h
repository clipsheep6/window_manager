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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_INCL_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_INCL_H

#include <cstdint>
#include <mutex>
#include <shared_mutex>

#include "mission_snapshot.h"
#include "transaction/rs_interfaces.h"

#include "agent_death_recipient.h"
#include "common/include/task_scheduler.h"
#include "future.h"
#include "interfaces/include/ws_common.h"
#include "session_listener_controller.h"
#include "scene_session_converter.h"
#include "scb_session_handler.h"
#include "session/host/include/root_scene_session.h"
#include "session/host/include/keyboard_session.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"
#include "wm_single_instance.h"
#include "window_scene_config.h"
#include "display_info.h"
#include "display_change_info.h"
#include "display_change_listener.h"
#include "app_debug_listener_interface.h"
#include "app_mgr_client.h"
#include "include/core/SkRegion.h"
#include "ability_info.h"

namespace OHOS::AAFwk {
class SessionInfo;
} // namespace OHOS::AAFwk

namespace OHOS::AppExecFwk {
class IBundleMgr;
struct AbilityInfo;
struct BundleInfo;
class LauncherService;
} // namespace OHOS::AppExecFwk

namespace OHOS::Global::Resource {
class ResourceManager;
} // namespace OHOS::Global::Resource

namespace OHOS::Rosen {
namespace AncoConsts {
    constexpr const char* ANCO_MISSION_ID = "ohos.anco.param.missionId";
    constexpr const char* ANCO_SESSION_ID = "ohos.anco.param.sessionId";
}
struct SCBAbilityInfo {
    AppExecFwk::AbilityInfo abilityInfo_;
    uint32_t sdkVersion_;
};
class SceneSession;
class AccessibilityWindowInfo;
class UnreliableWindowInfo;
using NotifyCreateSystemSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifyCreateKeyboardSessionFunc = std::function<void(const sptr<SceneSession>& keyboardSession,
    const sptr<SceneSession>& panelSession)>;
using NotifyCreateSubSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifyRecoverSceneSessionFunc =
    std::function<void(const sptr<SceneSession>& session, const SessionInfo& sessionInfo)>;
using ProcessStatusBarEnabledChangeFunc = std::function<void(bool enable)>;
using ProcessGestureNavigationEnabledChangeFunc = std::function<void(bool enable)>;
using ProcessOutsideDownEventFunc = std::function<void(int32_t x, int32_t y)>;
using ProcessShiftFocusFunc = std::function<void(int32_t persistentId)>;
using NotifySetFocusSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using DumpRootSceneElementInfoFunc = std::function<void(const std::vector<std::string>& params,
    std::vector<std::string>& infos)>;
using WindowChangedFunc = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using TraverseFunc = std::function<bool(const sptr<SceneSession>& session)>;
using CmpFunc = std::function<bool(std::pair<int32_t, sptr<SceneSession>>& lhs,
    std::pair<int32_t, sptr<SceneSession>>& rhs)>;
using ProcessStartUIAbilityErrorFunc = std::function<void(int32_t startUIAbilityError)>;
using NotifySCBAfterUpdateFocusFunc = std::function<void()>;
using ProcessCallingSessionIdChangeFunc = std::function<void(uint32_t callingSessionId)>;
using FlushWindowInfoTask = std::function<void()>;
using ProcessVirtualPixelRatioChangeFunc = std::function<void(float density, const Rect& rect)>;

class AppAnrListener : public IRemoteStub<AppExecFwk::IAppDebugListener> {
public:
    void OnAppDebugStarted(const std::vector<AppExecFwk::AppDebugInfo> &debugInfos) override;

    void OnAppDebugStoped(const std::vector<AppExecFwk::AppDebugInfo> &debugInfos) override;
};

class DisplayChangeListener : public IDisplayChangeListener {
public:
    virtual void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    virtual void OnScreenshot(DisplayId displayId) override;
    virtual void OnImmersiveStateChange(bool& immersive) override;
    virtual void OnGetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds) override;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_INCL_H
