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
#include "screen_manager_lite.h"

#include <map>
#include <vector>

#include "display_manager_adapter_lite.h"
#include "display_manager_agent_default.h"
#include "permission.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"


namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenManagerLite"};
}
class ScreenManagerLite::Impl : public RefBase {
public:
    Impl() = default;
    ~Impl();
    static inline SingletonDelegator<ScreenManagerLite> delegator;
    sptr<ScreenLite> GetScreen(ScreenId screenId);
    sptr<ScreenGroupLite> GetScreenGroup(ScreenId screenId);
    DMError GetAllScreens(std::vector<sptr<ScreenLite>>& screens);
    DMError RegisterScreenListener(sptr<IScreenListener> listener);
    DMError UnregisterScreenListener(sptr<IScreenListener> listener);
    DMError RegisterScreenGroupListener(sptr<IScreenGroupListener> listener);
    DMError UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener);
    DMError RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
    DMError UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
    DMError RegisterDisplayManagerAgent();
    DMError UnregisterDisplayManagerAgent();
    void OnRemoteDied();

private:
    void NotifyScreenConnect(sptr<ScreenInfo> info);
    void NotifyScreenDisconnect(ScreenId);
    void NotifyScreenChange(const sptr<ScreenInfo>& screenInfo);
    void NotifyScreenChange(const std::vector<sptr<ScreenInfo>>& screenInfos);
    bool UpdateScreenInfoLocked(sptr<ScreenInfo>);

    bool isAllListenersRemoved() const;

    class ScreenManagerListener;
    sptr<ScreenManagerListener> screenManagerListener_;
    std::map<ScreenId, sptr<ScreenLite>> screenMap_;
    std::map<ScreenId, sptr<ScreenGroupLite>> screenGroupMap_;
    std::recursive_mutex mutex_;
    std::set<sptr<IScreenListener>> screenListeners_;
    std::set<sptr<IScreenGroupListener>> screenGroupListeners_;
    std::set<sptr<IVirtualScreenGroupListener>> virtualScreenGroupListeners_;
};

class ScreenManagerLite::Impl::ScreenManagerListener : public DisplayManagerAgentDefault {
public:
    explicit ScreenManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnScreenConnect(sptr<ScreenInfo> screenInfo)
    {
        if (screenInfo == nullptr || screenInfo->GetScreenId() == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenConnect, screenInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenConnect, impl is nullptr.");
            return;
        }
        pImpl_->NotifyScreenConnect(screenInfo);
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnConnect(screenInfo->GetScreenId());
        }
    };

    void OnScreenDisconnect(ScreenId screenId)
    {
        if (screenId == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenDisconnect, screenId is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenDisconnect, impl is nullptr.");
            return;
        }
        pImpl_->NotifyScreenDisconnect(screenId);
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnDisconnect(screenId);
        }
    };

    void OnScreenChange(const sptr<ScreenInfo>& screenInfo, ScreenChangeEvent event)
    {
        if (screenInfo == nullptr) {
            WLOGFE("OnScreenChange, screenInfo is null.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenChange, impl is nullptr.");
            return;
        }
        WLOGFD("OnScreenChange. event %{public}u", event);
        pImpl_->NotifyScreenChange(screenInfo);
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener: pImpl_->screenListeners_) {
            listener->OnChange(screenInfo->GetScreenId());
        }
    };

    void OnScreenGroupChange(const std::string& trigger, const std::vector<sptr<ScreenInfo>>& screenInfos,
        ScreenGroupChangeEvent groupEvent)
    {
        if (screenInfos.empty()) {
            WLOGFE("screenInfos is empty.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("impl is nullptr.");
            return;
        }
        WLOGFD("trigger %{public}s, event %{public}u", trigger.c_str(), groupEvent);
        pImpl_->NotifyScreenChange(screenInfos);
        std::vector<ScreenId> screenIds;
        for (auto screenInfo : screenInfos) {
            if (screenInfo->GetScreenId() != SCREEN_ID_INVALID) {
                screenIds.push_back(screenInfo->GetScreenId());
            }
        }
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener: pImpl_->screenGroupListeners_) {
            listener->OnChange(screenIds, groupEvent);
        }
        NotifyVirtualScreenGroupChanged(screenInfos[0], trigger, screenIds, groupEvent);
    };
private:
    void NotifyVirtualScreenGroupChanged(sptr<ScreenInfo> screenInfo,
        const std::string trigger, std::vector<ScreenId>& ids, ScreenGroupChangeEvent groupEvent)
    {
        // check for invalid scene
        if (pImpl_->virtualScreenGroupListeners_.size() <= 0) {
            WLOGFW("no virtual screnn group listeners");
            return;
        }
        if (screenInfo->GetType() != ScreenType::VIRTUAL) {
            WLOGFW("not virtual screen type: %{public}u", screenInfo->GetType());
            return;
        }

        // get the parent of screen
        ScreenId parent = groupEvent == ScreenGroupChangeEvent::ADD_TO_GROUP ?
            screenInfo->GetParentId() : screenInfo->GetLastParentId();
        WLOGFD("parentId=[%{public}llu], lastParentId=[%{public}llu]", (unsigned long long)screenInfo->GetParentId(),
            (unsigned long long)screenInfo->GetLastParentId());
        if (parent == SCREEN_ID_INVALID) {
            WLOGFE("parentId is invalid");
            return;
        }
        auto screenGroup = pImpl_->GetScreenGroup(parent);
        if (screenGroup == nullptr) {
            WLOGFE("screenGroup is null");
            return;
        }

        // notify mirror
        ScreenCombination comb = screenGroup->GetCombination();
        WLOGFD("comb %{public}u", comb);
        IVirtualScreenGroupListener::ChangeInfo changeInfo = {groupEvent, trigger, ids};
        for (auto listener: pImpl_->virtualScreenGroupListeners_) {
            if (comb == ScreenCombination::SCREEN_MIRROR) {
                listener->OnMirrorChange(changeInfo);
            }
        }
    }
    sptr<Impl> pImpl_;
};

WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManagerLite)

ScreenManagerLite::ScreenManagerLite()
{
    pImpl_ = new Impl();
}

ScreenManagerLite::~ScreenManagerLite()
{
}

ScreenManagerLite::Impl::~Impl()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UnregisterDisplayManagerAgent();
}

sptr<ScreenLite> ScreenManagerLite::Impl::GetScreen(ScreenId screenId)
{
    auto screenInfo = SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenInfo(screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!UpdateScreenInfoLocked(screenInfo)) {
        screenMap_.erase(screenId);
        return nullptr;
    }
    return screenMap_[screenId];
}

sptr<ScreenLite> ScreenManagerLite::GetScreenById(ScreenId screenId)
{
    return pImpl_->GetScreen(screenId);
}

sptr<ScreenGroupLite> ScreenManagerLite::Impl::GetScreenGroup(ScreenId screenId)
{
    auto screenGroupInfo = SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenGroupInfoById(screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (screenGroupInfo == nullptr) {
        WLOGFE("screenGroupInfo is null");
        screenGroupMap_.erase(screenId);
        return nullptr;
    }
    auto iter = screenGroupMap_.find(screenId);
    if (iter != screenGroupMap_.end() && iter->second != nullptr) {
        iter->second->UpdateScreenGroupInfo(screenGroupInfo);
        return iter->second;
    }
    sptr<ScreenGroupLite> screenGroup = new ScreenGroupLite(screenGroupInfo);
    screenGroupMap_[screenId] = screenGroup;
    return screenGroup;
}

sptr<ScreenGroupLite> ScreenManagerLite::GetScreenGroup(ScreenId screenId)
{
    return pImpl_->GetScreenGroup(screenId);
}

DMError ScreenManagerLite::Impl::GetAllScreens(std::vector<sptr<ScreenLite>>& screens)
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    DMError ret  = SingletonContainer::Get<ScreenManagerAdapterLite>().GetAllScreenInfos(screenInfos);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto info: screenInfos) {
        if (UpdateScreenInfoLocked(info)) {
            screens.emplace_back(screenMap_[info->GetScreenId()]);
        }
    }
    screenMap_.clear();
    for (auto screen: screens) {
        screenMap_.insert(std::make_pair(screen->GetId(), screen));
    }
    return ret;
}

DMError ScreenManagerLite::GetAllScreens(std::vector<sptr<ScreenLite>>& screens)
{
    return pImpl_->GetAllScreens(screens);
}

DMError ScreenManagerLite::Impl::RegisterScreenListener(sptr<IScreenListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError regSucc = RegisterDisplayManagerAgent();
    if (regSucc == DMError::DM_OK) {
        screenListeners_.insert(listener);
    }
    return regSucc;
}

DMError ScreenManagerLite::RegisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterScreenListener(listener);
}

DMError ScreenManagerLite::Impl::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("unregister display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    auto iter = std::find(screenListeners_.begin(), screenListeners_.end(), listener);
    if (iter == screenListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    screenListeners_.erase(iter);
    return isAllListenersRemoved() ? UnregisterDisplayManagerAgent() : DMError::DM_OK;
}

DMError ScreenManagerLite::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterScreenListener(listener);
}

DMError ScreenManagerLite::Impl::RegisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError regSucc = RegisterDisplayManagerAgent();
    if (regSucc == DMError::DM_OK) {
        screenGroupListeners_.insert(listener);
    }
    return regSucc;
}

DMError ScreenManagerLite::RegisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenGroupListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterScreenGroupListener(listener);
}

DMError ScreenManagerLite::Impl::UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(screenGroupListeners_.begin(), screenGroupListeners_.end(), listener);
    if (iter == screenGroupListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    screenGroupListeners_.erase(iter);
    return isAllListenersRemoved() ? UnregisterDisplayManagerAgent() : DMError::DM_OK;
}

DMError ScreenManagerLite::UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenGroupListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterScreenGroupListener(listener);
}

DMError ScreenManagerLite::Impl::RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError regSucc = RegisterDisplayManagerAgent();
    if (regSucc == DMError::DM_OK) {
        virtualScreenGroupListeners_.insert(listener);
    }
    return regSucc;
}

DMError ScreenManagerLite::RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterVirtualScreenGroupListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterVirtualScreenGroupListener(listener);
}

DMError ScreenManagerLite::Impl::UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(virtualScreenGroupListeners_.begin(), virtualScreenGroupListeners_.end(), listener);
    if (iter == virtualScreenGroupListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    virtualScreenGroupListeners_.erase(iter);
    return isAllListenersRemoved() ? UnregisterDisplayManagerAgent() : DMError::DM_OK;
}

DMError ScreenManagerLite::UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterVirtualScreenGroupListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterVirtualScreenGroupListener(listener);
}

DMError ScreenManagerLite::Impl::RegisterDisplayManagerAgent()
{
    DMError regSucc = DMError::DM_OK;
    if (screenManagerListener_ == nullptr) {
        screenManagerListener_ = new ScreenManagerListener(this);
        regSucc = SingletonContainer::Get<ScreenManagerAdapterLite>().RegisterDisplayManagerAgent(
            screenManagerListener_, DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        if (regSucc != DMError::DM_OK) {
            screenManagerListener_ = nullptr;
            WLOGFW("RegisterDisplayManagerAgent failed !");
        }
    }
    return regSucc;
}

DMError ScreenManagerLite::Impl::UnregisterDisplayManagerAgent()
{
    DMError unRegSucc = DMError::DM_OK;
    if (screenManagerListener_ != nullptr) {
        unRegSucc = SingletonContainer::Get<ScreenManagerAdapterLite>().UnregisterDisplayManagerAgent(
            screenManagerListener_, DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        screenManagerListener_ = nullptr;
        if (unRegSucc != DMError::DM_OK) {
            WLOGFW("UnregisterDisplayManagerAgent failed!");
        }
    }
    return unRegSucc;
}

bool ScreenManagerLite::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("screenId:%{public}" PRIu64 ", state:%{public}u, reason:%{public}u", screenId, state, reason);
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetSpecifiedScreenPower(screenId, state, reason);
}

bool ScreenManagerLite::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("state:%{public}u, reason:%{public}u", state, reason);
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetScreenPowerForAll(state, reason);
}

ScreenPowerState ScreenManagerLite::GetScreenPower(ScreenId dmsScreenId)
{
    return SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenPower(dmsScreenId);
}

DMError ScreenManagerLite::SetScreenRotationLocked(bool isLocked)
{
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetScreenRotationLocked(isLocked);
}

DMError ScreenManagerLite::IsScreenRotationLocked(bool& isLocked)
{
    return SingletonContainer::Get<ScreenManagerAdapterLite>().IsScreenRotationLocked(isLocked);
}

void ScreenManagerLite::Impl::NotifyScreenConnect(sptr<ScreenInfo> info)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateScreenInfoLocked(info);
}

void ScreenManagerLite::Impl::NotifyScreenDisconnect(ScreenId screenId)
{
    WLOGFI("screenId:%{public}" PRIu64".", screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    screenMap_.erase(screenId);
}

void ScreenManagerLite::Impl::NotifyScreenChange(const sptr<ScreenInfo>& screenInfo)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateScreenInfoLocked(screenInfo);
}

void ScreenManagerLite::Impl::NotifyScreenChange(const std::vector<sptr<ScreenInfo>>& screenInfos)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto screenInfo : screenInfos) {
        UpdateScreenInfoLocked(screenInfo);
    }
}

bool ScreenManagerLite::Impl::UpdateScreenInfoLocked(sptr<ScreenInfo> screenInfo)
{
    if (screenInfo == nullptr) {
        WLOGFE("displayInfo is null");
        return false;
    }
    ScreenId screenId = screenInfo->GetScreenId();
    WLOGFI("screenId:%{public}" PRIu64".", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("displayId is invalid.");
        return false;
    }
    auto iter = screenMap_.find(screenId);
    if (iter != screenMap_.end() && iter->second != nullptr) {
        WLOGFI("get screen in screen map");
        iter->second->UpdateScreenInfo(screenInfo);
        return true;
    }
    sptr<ScreenLite> screen = new ScreenLite(screenInfo);
    screenMap_[screenId] = screen;
    return true;
}

bool ScreenManagerLite::Impl::isAllListenersRemoved() const
{
    return screenListeners_.empty() && screenGroupListeners_.empty() && virtualScreenGroupListeners_.empty();
}

void ScreenManagerLite::Impl::OnRemoteDied()
{
    WLOGFI("dms is died");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    screenManagerListener_ = nullptr;
}

void ScreenManagerLite::OnRemoteDied()
{
    pImpl_->OnRemoteDied();
}
} // namespace OHOS::Rosen