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

#include "session/screen/include/screen_session.h"

#include "window_manager_hilog.h"
#include <transaction/rs_interfaces.h>
// #include <display_power_mgr_client.h>
#include <power_mgr_client.h>
#include <ipc_skeleton.h>
#include "session/host/include/scene_session.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSession" };
}

ScreenSession::ScreenSession()
{}

ScreenSession::ScreenSession(ScreenId screenId, const ScreenProperty& property)
    : screenId_(screenId), property_(property)
{
    Rosen::RSDisplayNodeConfig config = { .screenId = screenId_ };
    displayNode_ = Rosen::RSDisplayNode::Create(config);
}

ScreenSession::ScreenSession(const std::string& name, ScreenId smsId, ScreenId rsId)
    : name_(name), screenId_(smsId), rsId_(rsId)
{
    (void)rsId_;
    Rosen::RSDisplayNodeConfig config = { .screenId = screenId_ };
    displayNode_ = Rosen::RSDisplayNode::Create(config);
}

void ScreenSession::RegisterScreenChangeListener(IScreenChangeListener* screenChangeListener)
{
    if (screenChangeListener == nullptr) {
        WLOGFE("Failed to register screen change listener, listener is null!");
        return;
    }

    if (std::find(screenChangeListenerList_.begin(), screenChangeListenerList_.end(), screenChangeListener) !=
        screenChangeListenerList_.end()) {
        WLOGFE("Repeat to register screen change listener!");
        return;
    }

    screenChangeListenerList_.emplace_back(screenChangeListener);
    if (screenState_ == ScreenState::CONNECTION) {
        screenChangeListener->OnConnect();
    }
}

void ScreenSession::UnregisterScreenChangeListener(IScreenChangeListener* screenChangeListener)
{
    if (screenChangeListener == nullptr) {
        WLOGFE("Failed to unregister screen change listener, listener is null!");
        return;
    }

    screenChangeListenerList_.erase(
        std::remove_if(screenChangeListenerList_.begin(), screenChangeListenerList_.end(),
            [screenChangeListener](IScreenChangeListener* listener) { return screenChangeListener == listener; }),
        screenChangeListenerList_.end());
}

sptr<DisplayInfo> ScreenSession::ConvertToDisplayInfo()
{
    sptr<DisplayInfo> displayInfo = new(std::nothrow) DisplayInfo();
    if (displayInfo == nullptr) {
        return displayInfo;
    }

    displayInfo->SetWidth(property_.GetBounds().rect_.GetWidth());
    displayInfo->SetHeight(property_.GetBounds().rect_.GetHeight());
    displayInfo->SetScreenId(screenId_);
    displayInfo->SetDisplayId(screenId_);
    displayInfo->SetRefreshRate(60);     // use 60 temporarily, depended on property set

    return displayInfo;
}

DMError ScreenSession::GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts)
{
    return DMError::DM_OK;
}

ScreenId ScreenSession::GetScreenId()
{
    return screenId_;
}

ScreenProperty ScreenSession::GetScreenProperty() const
{
    return property_;
}

std::shared_ptr<RSDisplayNode> ScreenSession::GetDisplayNode() const
{
    return displayNode_;
}

void ScreenSession::Connect()
{
    screenState_ = ScreenState::CONNECTION;
    for (auto& listener : screenChangeListenerList_) {
        listener->OnConnect();
    }
}

void ScreenSession::Disconnect()
{
    screenState_ = ScreenState::DISCONNECTION;
    for (auto& listener : screenChangeListenerList_) {
        listener->OnDisconnect();
    }
}

void ScreenSession::HandleTurnScreenOn(const sptr<SceneSession>& sceneSession)
{
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (sceneSession->IsTurnScreenOn() && !PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGI("turn screen on");
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
}

void ScreenSession::HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock)
{
    if (requireLock && sceneSession->keepScreenLock_ == nullptr) {
        // reset ipc identtiy
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        sceneSession->keepScreenLock_ = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(sceneSession->GetWindowName(),
            PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
    }
    if (sceneSession->keepScreenLock_ == nullptr) {
        return;
    }
    WLOGI("keep screen on: [%{public}s, %{public}d]", sceneSession->GetWindowName().c_str(), requireLock);
    // HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "screenSession:HandleKeepScreenOn(%s, %d)",
    //     sceneSession->GetWindowName().c_str(), requireLock);
    ErrCode res;
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (requireLock) {
        res = sceneSession->keepScreenLock_->Lock();
    } else {
        res = sceneSession->keepScreenLock_->UnLock();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    if (res != ERR_OK) {
        WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]", requireLock, res);
    }
}

sptr<SupportedScreenModes> ScreenSession::GetActiveScreenMode() const
{
    if (activeIdx_ < 0 || activeIdx_ >= static_cast<int32_t>(modes_.size())) {
        WLOGE("SCB: ScreenSession::GetActiveScreenMode active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return modes_[activeIdx_];
}

Rotation ScreenSession::CalcRotation(Orientation orientation) const
{
    return Rotation::ROTATION_0;
}

sptr<ScreenInfo> ScreenSession::ConvertToScreenInfo() const
{
    sptr<ScreenInfo> info = new(std::nothrow) ScreenInfo();
    if (info == nullptr) {
        return nullptr;
    }
    FillScreenInfo(info);
    return info;
}

void ScreenSession::FillScreenInfo(sptr<ScreenInfo> info) const
{
}

bool ScreenSession::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
    return true;
}

void ScreenSession::InitRSDisplayNode(RSDisplayNodeConfig& config, Point& startPoint)
{
}

ScreenSessionGroup::ScreenSessionGroup(ScreenId dmsId, ScreenId rsId,
    std::string name, ScreenCombination combination) : combination_(combination)
{
    name_ = name;
    screenId_ = dmsId;
    rsId_ = rsId;
    type_ = ScreenType::UNDEFINED;
    isScreenGroup_ = true;
}

ScreenSessionGroup::~ScreenSessionGroup()
{
    displayNode_ = nullptr;
    screenSessionMap_.clear();
}

bool ScreenSessionGroup::GetRSDisplayNodeConfig(sptr<ScreenSession>& dmsScreen, struct RSDisplayNodeConfig& config,
                                                sptr<ScreenSession> defaultScreenSession)
{
    return true;
}

bool ScreenSessionGroup::AddChild(sptr<ScreenSession>& smsScreen, Point& startPoint,
                                  sptr<ScreenSession> defaultScreenSession)
{
    return true;
}

bool ScreenSessionGroup::AddChildren(std::vector<sptr<ScreenSession>>& smsScreens, std::vector<Point>& startPoints)
{
    return true;
}

bool ScreenSessionGroup::RemoveChild(sptr<ScreenSession>& smsScreen)
{
    return true;
}

bool ScreenSessionGroup::HasChild(ScreenId childScreen) const
{
    return screenSessionMap_.find(childScreen) != screenSessionMap_.end();
}

std::vector<sptr<ScreenSession>> ScreenSessionGroup::GetChildren() const
{
    std::vector<sptr<ScreenSession>> res;
    return res;
}

std::vector<Point> ScreenSessionGroup::GetChildrenPosition() const
{
    std::vector<Point> res;
    return res;
}

Point ScreenSessionGroup::GetChildPosition(ScreenId screenId) const
{
    Point point;
    auto iter = screenSessionMap_.find(screenId);
    if (iter != screenSessionMap_.end()) {
        point = iter->second.second;
    }
    return point;
}

size_t ScreenSessionGroup::GetChildCount() const
{
    return screenSessionMap_.size();
}

ScreenCombination ScreenSessionGroup::GetScreenCombination() const
{
    return combination_;
}

sptr<ScreenGroupInfo> ScreenSessionGroup::ConvertToScreenGroupInfo() const
{
    return nullptr;
}


} // namespace OHOS::Rosen
