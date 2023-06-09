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

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSession" };
}

ScreenSession::ScreenSession(ScreenId screenId, ScreenProperty& property)
    : screenId_(screenId), property_(property)
{
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
    displayInfo->SetRefreshRate(refreshRate_);
    displayInfo->SetVirtualPixelRatio(virtualPixelRatio_);
    displayInfo->SetXDpi(xDpi_);
    displayInfo->SetYDpi(yDpi_);
    displayInfo->SetDpi(density_);
    displayInfo->SetRotation(rotation_);
    displayInfo->Orientation(orientation_);
    displayInfo->SetOffsetX(0.0f);
    displayInfo->SetOffsetY(0.0f);
    // displayInfo->SetDisplayState();
    // displayInfo->SetWaterfallDisplayCompressionStatus();
    // displayInfo->SetDisplayOrientation();
    // displayInfo->SetDisplayStateChangeType();
    // displayInfo->SetisDefaultVertical();
    // displayInfo->SetAliveStatus();
    return displayInfo;
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

ScreenId ScreenSession::GetId() const
{
    return screenId_;
}

ScreenState ScreenSession::GetScreenState() const
{
    return screenState_;
}

DMError ScreenSession::SetScreenActiveMode(uint32_t modeId)
{
    activeModeIdx_ = modeId;
    return DMError::DM_OK;
}

void ScreenSession::SetVirtualPixelRatio(float virtualPixelRatio)
{
    property_.SetVirtualPixelRatio(virtualPixelRatio);
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

sptr<SupportedScreenModes> ScreenSession::GetActiveScreenMode() const
{
    if (activeIdx_ < 0 || activeIdx_ >= static_cast<int32_t>(modes_.size())) {
        WLOGE("active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return modes_[activeIdx_];
}

ScreenSourceMode ScreenSession::GetSourceMode() const
{
  return ScreenSourceMode::SCREEN_ALONE;
}

void ScreenSession::FillScreenInfo(sptr<ScreenInfo> info) const
{
    if (info == nullptr) {
        WLOGE("FillScreenInfo failed! info is nullptr");
        return;
    }
    info->id_ = screenId_;
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

sptr<ScreenInfo> ScreenSession::ConvertToScreenInfo() const
{
    sptr<ScreenInfo> info = new(std::nothrow) ScreenInfo();
    if (info == nullptr) {
        return nullptr;
    }
    FillScreenInfo(info);
    return info;
}


} // namespace OHOS::Rosen
