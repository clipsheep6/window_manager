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

DMError ScreenSession::GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts)
{
    auto ret = RSInterfaces::GetInstance().GetScreenSupportedColorGamuts(rsId_, colorGamuts);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("SCB: ScreenSession::GetScreenSupportedColorGamuts fail! rsId %{public}" PRIu64"", rsId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("SCB: ScreenSession::GetScreenSupportedColorGamuts ok! rsId %{public}" PRIu64", size %{public}u",
        rsId_, static_cast<uint32_t>(colorGamuts.size()));

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

void ScreenSession::ReleaseDisplayNode()
{
    displayNode_ = nullptr;
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
        WLOGE("SCB: ScreenSession::GetActiveScreenMode active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return modes_[activeIdx_];
}

Rotation ScreenSession::CalcRotation(Orientation orientation) const
{
    sptr<SupportedScreenModes> info = GetActiveScreenMode();
    if (info == nullptr) {
        return Rotation::ROTATION_0;
    }
    // vertical: phone(Plugin screen); horizontal: pad & external screen
    bool isVerticalScreen = info->width_ < info->height_;
    switch (orientation) {
        case Orientation::UNSPECIFIED: {
            return Rotation::ROTATION_0;
        }
        case Orientation::VERTICAL: {
            return isVerticalScreen ? Rotation::ROTATION_0 : Rotation::ROTATION_90;
        }
        case Orientation::HORIZONTAL: {
            return isVerticalScreen ? Rotation::ROTATION_90 : Rotation::ROTATION_0;
        }
        case Orientation::REVERSE_VERTICAL: {
            return isVerticalScreen ? Rotation::ROTATION_180 : Rotation::ROTATION_270;
        }
        case Orientation::REVERSE_HORIZONTAL: {
            return isVerticalScreen ? Rotation::ROTATION_270 : Rotation::ROTATION_180;
        }
        default: {
            WLOGE("unknown orientation %{public}u", orientation);
            return Rotation::ROTATION_0;
        }
    }
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
    sptr<SupportedScreenModes> screenSessionModes = GetActiveScreenMode();
    if (screenSessionModes != nullptr) {
        height = screenSessionModes->height_;
        width = screenSessionModes->width_;
    }
    float virtualPixelRatio = property_.GetVirtualPixelRatio();
    // "< 1e-set6" means virtualPixelRatio is 0.
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

    info->lastParent_ = lastGroupSmsId_;
    info->parent_ = groupSmsId_;
    info->isScreenGroup_ = isScreenGroup_;
    info->modes_ = modes_;
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

DMError ScreenSession::GetScreenColorGamut(ScreenColorGamut& colorGamut)
{
    auto ret = RSInterfaces::GetInstance().GetScreenColorGamut(rsId_, colorGamut);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("GetScreenColorGamut fail! rsId %{public}" PRIu64"", rsId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("GetScreenColorGamut ok! rsId %{public}" PRIu64", colorGamut %{public}u",
        rsId_, static_cast<uint32_t>(colorGamut));
    return DMError::DM_OK;
}

DMError ScreenSession::SetScreenColorGamut(int32_t colorGamutIdx)
{
    std::vector<ScreenColorGamut> colorGamuts;
    DMError res = GetScreenSupportedColorGamuts(colorGamuts);
    if (res != DMError::DM_OK) {
        WLOGE("SetScreenColorGamut fail! rsId %{public}" PRIu64"", rsId_);
        return res;
    }
    if (colorGamutIdx < 0 || colorGamutIdx >= static_cast<int32_t>(colorGamuts.size())) {
        WLOGE("SetScreenColorGamut fail! rsId %{public}" PRIu64" colorGamutIdx %{public}d invalid.",
            rsId_, colorGamutIdx);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto ret = RSInterfaces::GetInstance().SetScreenColorGamut(rsId_, colorGamutIdx);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("SetScreenColorGamut fail! rsId %{public}" PRIu64"", rsId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("SetScreenColorGamut ok! rsId %{public}" PRIu64", colorGamutIdx %{public}u",
        rsId_, colorGamutIdx);
    return DMError::DM_OK;
}

DMError ScreenSession::GetScreenGamutMap(ScreenGamutMap& gamutMap)
{
    auto ret = RSInterfaces::GetInstance().GetScreenGamutMap(rsId_, gamutMap);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("GetScreenGamutMap fail! rsId %{public}" PRIu64"", rsId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("GetScreenGamutMap ok! rsId %{public}" PRIu64", gamutMap %{public}u",
        rsId_, static_cast<uint32_t>(gamutMap));
    return DMError::DM_OK;
}

DMError ScreenSession::SetScreenGamutMap(ScreenGamutMap gamutMap)
{
    if (gamutMap > GAMUT_MAP_HDR_EXTENSION) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto ret = RSInterfaces::GetInstance().SetScreenGamutMap(rsId_, gamutMap);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("SetScreenGamutMap fail! rsId %{public}" PRIu64"", rsId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("SetScreenGamutMap ok! rsId %{public}" PRIu64", gamutMap %{public}u",
        rsId_, static_cast<uint32_t>(gamutMap));
    return DMError::DM_OK;
}

DMError ScreenSession::SetScreenColorTransform()
{
    WLOGI("SetScreenColorTransform ok! rsId %{public}" PRIu64"", rsId_);
    return DMError::DM_OK;
}

int32_t ScreenSession::GetPrivateSessionCount() const
{
    return privateSessionCount_;
}

DMError ScreenSession::SetPrivateSessionCount(int32_t count)
{
    privateSessionCount_ = count;
    return DMError::DM_OK;
}

void ScreenSession::InitRSDisplayNode(RSDisplayNodeConfig& config, Point& startPoint)
{
    if (displayNode_ != nullptr) {
        displayNode_->SetDisplayNodeMirrorConfig(config);
    } else {
        std::shared_ptr<RSDisplayNode> rsDisplayNode = RSDisplayNode::Create(config);
        if (rsDisplayNode == nullptr) {
            WLOGE("fail to add child. create rsDisplayNode fail!");
            return;
        }
        displayNode_ = rsDisplayNode;
    }
    WLOGFI("SetDisplayOffset: posX:%{public}d, posY:%{public}d", startPoint.posX_, startPoint.posY_);
    displayNode_->SetDisplayOffset(startPoint.posX_, startPoint.posY_);
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<SupportedScreenModes> abstractScreenModes = GetActiveScreenMode();
    if (abstractScreenModes != nullptr) {
        height = abstractScreenModes->height_;
        width = abstractScreenModes->width_;
    }
    RSScreenType screenType;
    auto ret = RSInterfaces::GetInstance().GetScreenType(rsId_, screenType);
    if (ret == StatusCode::SUCCESS && screenType == RSScreenType::VIRTUAL_TYPE_SCREEN) {
        displayNode_->SetSecurityDisplay(true);
        WLOGFI("virtualScreen SetSecurityDisplay success");
    }
    // If setDisplayOffset is not valid for SetFrame/SetBounds
    displayNode_->SetFrame(0, 0, width, height);
    displayNode_->SetBounds(0, 0, width, height);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

// void ScreenSession::UpdatePrivateStateAndNotify(bool isAddingPrivateSession)
// {
//     uint32_t prePrivateSessionCount = privateSessionCount_;
//     WLOGFD("before update : privateWindow count: %{public}u", prePrivateSessionCount);
//     privateSessionCount_ += (isAddingPrivateSession ? 1 : -1);
//     if (prePrivateSessionCount == 0 && privateSessionCount_ == 1) {
//         ScreenSessionManager::GetInstance().NotifyPrivateSessionStateChanged(true);
//     } else if (prePrivateSessionCount == 1 && privateSessionCount_ == 0) {
//         ScreenSessionManager::GetInstance().NotifyPrivateSessionStateChanged(false);
//     }
// }

ScreenSessionGroup::ScreenSessionGroup(ScreenId screenId, ScreenId rsId,
    std::string name, ScreenCombination combination) : combination_(combination)
{
    name_ = name;
    screenId_ = screenId;
    rsId_ = rsId;
    GetScreenProperty().SetScreenType(ScreenType::UNDEFINED);
    isScreenGroup_ = true;
}

ScreenSessionGroup::~ScreenSessionGroup()
{
    ReleaseDisplayNode();
    screenSessionMap_.clear();
}

bool ScreenSessionGroup::GetRSDisplayNodeConfig(sptr<ScreenSession>& screenSession, struct RSDisplayNodeConfig& config,
                                                sptr<ScreenSession> defaultScreenSession)
{
    if (screenSession == nullptr) {
        WLOGE("screenSession is nullptr.");
        return false;
    }
    config = { screenSession->rsId_ };
    switch (combination_) {
        case ScreenCombination::SCREEN_ALONE:
            [[fallthrough]];
        case ScreenCombination::SCREEN_EXPAND:
            break;
        case ScreenCombination::SCREEN_MIRROR: {
            if (GetChildCount() == 0 || mirrorScreenId_ == screenSession->screenId_) {
                WLOGI("AddChild, SCREEN_MIRROR, config is not mirror");
                break;
            }
            std::shared_ptr<RSDisplayNode> displayNode = defaultScreenSession->GetDisplayNode();
            if (displayNode == nullptr) {
                WLOGFE("AddChild fail, displayNode is nullptr, cannot get DisplayNode");
                break;
            }
            NodeId nodeId = displayNode->GetId();
            WLOGI("AddChild, mirrorScreenId_:%{public}" PRIu64", rsId_:%{public}" PRIu64", nodeId:%{public}" PRIu64"",
                mirrorScreenId_, screenSession->rsId_, nodeId);
            config = {screenSession->rsId_, true, nodeId};
            break;
        }
        default:
            WLOGE("fail to add child. invalid group combination:%{public}u", combination_);
            return false;
    }
    return true;
}

bool ScreenSessionGroup::AddChild(sptr<ScreenSession>& smsScreen, Point& startPoint,
                                  sptr<ScreenSession> defaultScreenSession)
{
    if (smsScreen == nullptr) {
        WLOGE("AddChild, smsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = smsScreen->screenId_;
    auto iter = screenSessionMap_.find(screenId);
    if (iter != screenSessionMap_.end()) {
        WLOGE("AddChild, screenSessionMap_ has smsScreen:%{public}" PRIu64"", screenId);
        return false;
    }
    struct RSDisplayNodeConfig config;
    if (!GetRSDisplayNodeConfig(smsScreen, config, defaultScreenSession)) {
        return false;
    }
    smsScreen->InitRSDisplayNode(config, startPoint);
    smsScreen->lastGroupSmsId_ = smsScreen->groupSmsId_;
    smsScreen->groupSmsId_ = screenId_;
    screenSessionMap_.insert(std::make_pair(screenId, std::make_pair(smsScreen, startPoint)));
    return true;
}

bool ScreenSessionGroup::AddChildren(std::vector<sptr<ScreenSession>>& smsScreens, std::vector<Point>& startPoints)
{
    size_t size = smsScreens.size();
    if (size != startPoints.size()) {
        WLOGE("AddChildren, unequal size.");
        return false;
    }
    bool res = true;
    for (size_t i = 0; i < size; i++) {
        res = AddChild(smsScreens[i], startPoints[i], nullptr) && res;
    }
    return res;
}

bool ScreenSessionGroup::RemoveChild(sptr<ScreenSession>& smsScreen)
{
    if (smsScreen == nullptr) {
        WLOGE("RemoveChild, smsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = smsScreen->screenId_;
    smsScreen->lastGroupSmsId_ = smsScreen->groupSmsId_;
    smsScreen->groupSmsId_ = SCREEN_ID_INVALID;
    if (smsScreen->GetDisplayNode() != nullptr) {
        smsScreen->GetDisplayNode()->SetDisplayOffset(0, 0);
        smsScreen->GetDisplayNode()->RemoveFromTree();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        smsScreen->ReleaseDisplayNode();
    }
    return screenSessionMap_.erase(screenId);
}

bool ScreenSessionGroup::HasChild(ScreenId childScreen) const
{
    return screenSessionMap_.find(childScreen) != screenSessionMap_.end();
}

std::vector<sptr<ScreenSession>> ScreenSessionGroup::GetChildren() const
{
    std::vector<sptr<ScreenSession>> res;
    for (auto iter = screenSessionMap_.begin(); iter != screenSessionMap_.end(); iter++) {
        res.push_back(iter->second.first);
    }
    return res;
}

std::vector<Point> ScreenSessionGroup::GetChildrenPosition() const
{
    std::vector<Point> res;
    for (auto iter = screenSessionMap_.begin(); iter != screenSessionMap_.end(); iter++) {
        res.push_back(iter->second.second);
    }
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
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    if (screenGroupInfo == nullptr) {
        return nullptr;
    }
    FillScreenInfo(screenGroupInfo);
    screenGroupInfo->combination_ = combination_;
    for (auto iter = screenSessionMap_.begin(); iter != screenSessionMap_.end(); iter++) {
        screenGroupInfo->children_.push_back(iter->first);
    }
    auto positions = GetChildrenPosition();
    screenGroupInfo->position_.insert(screenGroupInfo->position_.end(), positions.begin(), positions.end());
    return screenGroupInfo;
}


} // namespace OHOS::Rosen
