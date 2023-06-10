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

#include <transaction/rs_interfaces.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSession" };
}

ScreenSession::ScreenSession(ScreenId screenId, const ScreenProperty& property)
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
    displayInfo->SetRefreshRate(property_.GetRefreshRate());
    displayInfo->SetVirtualPixelRatio(property_.GetVirtualPixelRatio());
    displayInfo->SetXDpi(property_.GetXDpi());
    displayInfo->SetYDpi(property_.GetYDpi());
    displayInfo->SetDpi(property_.GetDensity());
    displayInfo->SetRotation(property_.GetScreenRotation());
    displayInfo->SetOrientation(property_.GetOrientation());
    displayInfo->SetOffsetX(property_.GetOffsetX());
    displayInfo->SetOffsetY(property_.GetOffsetY());

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

std::vector<sptr<SupportedScreenModes>> ScreenSession::GetScreenModes() const
{
    return modes_;
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
    info->SetScreenId(screenId_);
    info->SetName(name_);
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<SupportedScreenModes> ScreenSessionModes = GetActiveScreenMode();
    if (ScreenSessionModes != nullptr) {
        height = ScreenSessionModes->height_;
        width = ScreenSessionModes->width_;
    }
    float virtualPixelRatio = property_.GetVirtualPixelRatio();
    // "< 1e-6" means virtualPixelRatio is 0.
    if (fabsf(virtualPixelRatio) < 1e-6) {
        virtualPixelRatio = 1.0f;
    }
    ScreenSourceMode sourceMode = GetSourceMode();
    info->SetVirtualPixelRatio(property_.GetVirtualPixelRatio());
    info->SetVirtualHeight(height / virtualPixelRatio);
    info->SetVirtualWidth(width / virtualPixelRatio);
    info->SetRotation(property_.GetScreenRotation());
    info->SetOrientation(property_.GetOrientation());
    info->SetSourceMode(sourceMode);
    info->SetType(property_.GetScreenType());
    info->SetModeId(activeIdx_);
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

DMError ScreenSession::GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts)
{
    auto ret = RSInterfaces::GetInstance().GetScreenSupportedColorGamuts(screenId_, colorGamuts);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("GetScreenSupportedColorGamuts fail! rsId %{public}" PRIu64"", screenId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("GetScreenSupportedColorGamuts ok! rsId %{public}" PRIu64", size %{public}u",
        screenId_, static_cast<uint32_t>(colorGamuts.size()));

    return DMError::DM_OK;
}

DMError ScreenSession::GetScreenColorGamut(ScreenColorGamut& colorGamut)
{
    auto ret = RSInterfaces::GetInstance().GetScreenColorGamut(screenId_, colorGamut);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("GetScreenColorGamut fail! rsId %{public}" PRIu64"", screenId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("GetScreenColorGamut ok! rsId %{public}" PRIu64", colorGamut %{public}u",
        screenId_, static_cast<uint32_t>(colorGamut));
    return DMError::DM_OK;
}

DMError ScreenSession::SetScreenColorGamut(int32_t colorGamutIdx)
{
    std::vector<ScreenColorGamut> colorGamuts;
    DMError res = GetScreenSupportedColorGamuts(colorGamuts);
    if (res != DMError::DM_OK) {
        WLOGE("SetScreenColorGamut fail! rsId %{public}" PRIu64"", screenId_);
        return res;
    }
    if (colorGamutIdx < 0 || colorGamutIdx >= static_cast<int32_t>(colorGamuts.size())) {
        WLOGE("SetScreenColorGamut fail! rsId %{public}" PRIu64" colorGamutIdx %{public}d invalid.",
            screenId_, colorGamutIdx);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto ret = RSInterfaces::GetInstance().SetScreenColorGamut(screenId_, colorGamutIdx);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("SetScreenColorGamut fail! rsId %{public}" PRIu64"", screenId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("SetScreenColorGamut ok! rsId %{public}" PRIu64", colorGamutIdx %{public}u",
        screenId_, colorGamutIdx);
    return DMError::DM_OK;
}

DMError ScreenSession::GetScreenGamutMap(ScreenGamutMap& gamutMap)
{
    auto ret = RSInterfaces::GetInstance().GetScreenGamutMap(screenId_, gamutMap);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("GetScreenGamutMap fail! rsId %{public}" PRIu64"", screenId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("GetScreenGamutMap ok! rsId %{public}" PRIu64", gamutMap %{public}u",
        screenId_, static_cast<uint32_t>(gamutMap));
    return DMError::DM_OK;
}

DMError ScreenSession::SetScreenGamutMap(ScreenGamutMap gamutMap)
{
    if (gamutMap > GAMUT_MAP_HDR_EXTENSION) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto ret = RSInterfaces::GetInstance().SetScreenGamutMap(screenId_, gamutMap);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("SetScreenGamutMap fail! rsId %{public}" PRIu64"", screenId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("SetScreenGamutMap ok! rsId %{public}" PRIu64", gamutMap %{public}u",
        screenId_, static_cast<uint32_t>(gamutMap));
    return DMError::DM_OK;
}

DMError ScreenSession::SetScreenColorTransform()
{
    WLOGI("SetScreenColorTransform ok! rsId %{public}" PRIu64"", screenId_);
    return DMError::DM_OK;
}

} // namespace OHOS::Rosen
