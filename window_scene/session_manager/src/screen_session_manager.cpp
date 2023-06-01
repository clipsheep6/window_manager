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

#include "session_manager/include/screen_session_manager.h"

#include <transaction/rs_interfaces.h>
#include "window_manager_hilog.h"
#include "screen_session_manager.h"
#include "permission.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManager" };
const std::string SCREEN_SESSION_MANAGER_THREAD = "ScreenSessionManager";
} // namespace

ScreenSessionManager& ScreenSessionManager::GetInstance()
{
    static ScreenSessionManager screenSessionManager;
    return screenSessionManager;
}

ScreenSessionManager::ScreenSessionManager() : rsInterface_(RSInterfaces::GetInstance())
{
    Init();
}

void ScreenSessionManager::RegisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener)
{
    if (screenConnectionListener == nullptr) {
        WLOGFE("Failed to register screen connection callback, callback is null!");
        return;
    }

    if (std::find(screenConnectionListenerList_.begin(), screenConnectionListenerList_.end(),
        screenConnectionListener) != screenConnectionListenerList_.end()) {
        WLOGFE("Repeat to register screen connection callback!");
        return;
    }

    screenConnectionListenerList_.emplace_back(screenConnectionListener);
    for (auto sessionIt : screenSessionMap_) {
        screenConnectionListener->OnScreenConnect(sessionIt.second);
    }
}

void ScreenSessionManager::UnregisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener)
{
    if (screenConnectionListener == nullptr) {
        WLOGFE("Failed to unregister screen connection listener, listener is null!");
        return;
    }

    screenConnectionListenerList_.erase(
        std::remove_if(screenConnectionListenerList_.begin(), screenConnectionListenerList_.end(),
            [screenConnectionListener](
                sptr<IScreenConnectionListener> listener) { return screenConnectionListener == listener; }),
        screenConnectionListenerList_.end());
}

DMError ScreenSessionManager::RegisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !Permission::IsSystemCalling()
        && !Permission::IsStartByHdcd()) {
        WLOGFE("register display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }

    return dmAgentContainer_.RegisterAgent(displayManagerAgent, type) ? DMError::DM_OK :DMError::DM_ERROR_NULLPTR;
}

DMError ScreenSessionManager::UnregisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !Permission::IsSystemCalling()
        && !Permission::IsStartByHdcd()) {
        WLOGFE("unregister display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }

    return dmAgentContainer_.UnregisterAgent(displayManagerAgent, type) ? DMError::DM_OK :DMError::DM_ERROR_NULLPTR;
}

void ScreenSessionManager::Init()
{
    msgScheduler_ = std::make_shared<MessageScheduler>(SCREEN_SESSION_MANAGER_THREAD);
    RegisterScreenChangeListener();
}

void ScreenSessionManager::RegisterScreenChangeListener()
{
    WLOGFD("Register screen change listener.");
    auto res = rsInterface_.SetScreenChangeCallback(
        [this](ScreenId screenId, ScreenEvent screenEvent) { OnScreenChange(screenId, screenEvent); });
    if (res != StatusCode::SUCCESS) {
        auto task = [this]() { RegisterScreenChangeListener(); };
        WS_CHECK_NULL_SCHE_VOID(msgScheduler_, task);
        msgScheduler_->PostAsyncTask(task, 50); // Retry after 50 ms.
    }
}

void ScreenSessionManager::OnScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    WLOGFI("On screen change. ScreenId: %{public}" PRIu64 ", ScreenEvent: %{public}d", screenId,
        static_cast<int>(screenEvent));
    auto screenSession = GetOrCreateScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is nullptr");
        return;
    }
    if (screenEvent == ScreenEvent::CONNECTED) {
        for (auto listener : screenConnectionListenerList_) {
            listener->OnScreenConnect(screenSession);
        }
        screenSession->Connect();
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        screenSession->Disconnect();
        for (auto listener : screenConnectionListenerList_) {
            listener->OnScreenDisconnect(screenSession);
        }
        screenSessionMap_.erase(screenId);
    } else {
        WLOGE("Unknown ScreenEvent: %{public}d", static_cast<int>(screenEvent));
    }
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSession(ScreenId screenId)
{
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        WLOGFE("Error found screen session with id: %{public}" PRIu64, screenId);
        return nullptr;
    }
    return iter->second;
}

sptr<DisplayInfo> ScreenSessionManager::GetDefaultDisplayInfo()
{
    GetDefaultScreenId();
    sptr<ScreenSession> screenSession = GetScreenSession(defaultScreenId_);
    if (screenSession) {
        return screenSession->ConvertToDisplayInfo();
    } else {
        WLOGFE("Get default screen session failed.");
        return nullptr;
    }
}

DMError ScreenSessionManager::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set screen active permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetScreenActiveMode(%" PRIu64", %u)", screenId, modeId);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession) {
        return screenSession->SetScreenActiveMode(modeId);
    } else {
        WLOGFE("get screen session faile.");
        return DMError::DM_ERROR_UNKNOWN;
    }
}

sptr<SupportedScreenModes> ScreenSessionManager::GetActiveScreenMode() const
{
    if (activeIdx_ < 0 || activeIdx_ >= static_cast<int32_t>(modes_.size())) {
        WLOGE("active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return modes_[activeIdx_];
}

ScreenSourceMode ScreenSessionManager::GetSourceMode() const
{
  return ScreenSourceMode::SCREEN_ALONE;
}

void ScreenSessionManager::FillScreenInfo(sptr<ScreenInfo> info) const
{
    if (info == nullptr) {
        WLOGE("FillScreenInfo failed! info is nullptr");
        return;
    }
    info->id_ = dmsId_;
    info->name_ = name_;
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<SupportedScreenModes> ScreenSessionModes = GetActiveScreenMode();
    if (ScreenSessionModes != nullptr) {
        height = ScreenSessionModes->height_;
        width = ScreenSessionModes->width_;
    }
    float virtualPixelRatio = virtualPixelRatio_;
    // "< 1e-6" means virtualPixelRatio is 0.
    if (fabsf(virtualPixelRatio) < 1e-6) {
        virtualPixelRatio = 1.0f;
    }
    ScreenSourceMode sourceMode = GetSourceMode();
    info->virtualPixelRatio_ = virtualPixelRatio;
    info->virtualHeight_ = height / virtualPixelRatio;
    info->virtualWidth_ = width / virtualPixelRatio;
    // info->lastParent_ = lastGroupDmsId_;
    // info->parent_ = groupDmsId_;
    // info->isScreenGroup_ = isScreenGroup_;
    // info->rotation_ = rotation_;
    // info->orientation_ = orientation_;
    info->sourceMode_ = sourceMode;
    // info->type_ = type_;
    // info->modeId_ = activeIdx_;
    // info->modes_ = modes_;
}

sptr<ScreenInfo> ScreenSessionManager::ConvertToScreenInfo() const
{
    sptr<ScreenInfo> info = new(std::nothrow)   ();
    if (info == nullptr) {
        return nullptr;
    }
    FillScreenInfo(info);
    return info;
}

void ScreenSessionManager::NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event) const
{
    if (screenInfo == nullptr) {
        WLOGFE("NotifyScreenChanged error, screenInfo is nullptr.");
        return;
    }
    auto task = [=] {
        WLOGFI("NotifyScreenChanged,  screenId:%{public}" PRIu64"", screenInfo->GetScreenId());
        // from display_manager_agent_controller.cpp onscreenconnect
        if (screenInfo == nullptr) 
        {
        return;
        }
        auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        if (agents.empty()) {
        return;
        }
        WLOGFI("OnScreenChange");
        for (auto& agent : agents) {
            agent->OnScreenChange(screenInfo, event);
        }   
    };
    controllerHandler_->PostTask(task, AppExecFwk::EventQueue::Priority::HIGH);
}

DMError ScreenSessionManager::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{

    // from display_manager_service.cpp
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set virtual pixel permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:SetVirtualPixelRatio(%" PRIu64", %f)", screenId,
        virtualPixelRatio);

    // from abstract_screen_controller.cpp
    WLOGD("set virtual pixel ratio. screen %{public}" PRIu64" virtualPixelRatio %{public}f",
        screenId, virtualPixelRatio);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (!session) {
        WLOGFE("screen session is nullptr"); 
        return session;
    }
    screenSession->SetVirtualPixelRatio(virtualPixelRatio);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::VIRTUAL_PIXEL_RATIO_CHANGED);

    return DMError::DM_OK;
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    auto sessionIt = screenSessionMap_.find(screenId);
    if (sessionIt != screenSessionMap_.end()) {
        return sessionIt->second;
    }

    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({ 0, 0, screenMode.GetScreenWidth(), screenMode.GetScre enHeight() }, 0.0f, 0.0f);
    auto screenRefreshRate = screenMode.GetScreenRefreshRate();
    RSScreenCapability screenCapability = rsInterface_.GetScreenCapability(rsScreenId);
    ScreenProperty property;
  // property.SetRotation(0.0f);
    property.SetBounds(screenBounds);
    property.SetRefreshRate(screenRefreshRate);
    property.SetPhyWidth(screenCapability.GetPhyWidth());
    property->SetPhyHeight(screenCapability.GetPhyHeight());
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, property);
    if (!session) {
        WLOGFE("screen session is nullptr"); 
        return session;
    }
    screenSessionMap_[screenId] = session;
    return session;
}
ScreenId ScreenSessionManager::GetDefaultScreenId()
{
    if (defaultScreenId_ == INVALID_SCREEN_ID) {
        defaultScreenId_ = rsInterface_.GetDefaultScreenId();
    }
    return defaultScreenId_;
}
} // namespace OHOS::Rosen
