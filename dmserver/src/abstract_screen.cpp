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

#include "abstract_screen.h"

#include <cmath>
#include "abstract_screen_controller.h"
#include "display_manager_service.h"
#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractScreenGroup"};
}

AbstractScreen::AbstractScreen(sptr<AbstractScreenController> screenController, const std::string& name, ScreenId dmsId,
    ScreenId rsId) : name_(name), dmsId_(dmsId), rsId_(rsId), screenController_(screenController)
{
}

AbstractScreen::~AbstractScreen()
{
}

sptr<SupportedScreenModes> AbstractScreen::GetActiveScreenMode() const
{
    if (activeIdx_ < 0 || activeIdx_ >= modes_.size()) {
        WLOGE("active mode index is wrong: %{public}d", activeIdx_);
        return nullptr;
    }
    return modes_[activeIdx_];
}

std::vector<sptr<SupportedScreenModes>> AbstractScreen::GetAbstractScreenModes() const
{
    return modes_;
}

sptr<AbstractScreenGroup> AbstractScreen::GetGroup() const
{
    return screenController_->GetAbstractScreenGroup(groupDmsId_);
}

sptr<ScreenInfo> AbstractScreen::ConvertToScreenInfo() const
{
    sptr<ScreenInfo> info = new(std::nothrow) ScreenInfo();
    if (info == nullptr) {
        return nullptr;
    }
    FillScreenInfo(info);
    return info;
}

void AbstractScreen::UpdateRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd)
{
    if (rsDisplayNode_ == nullptr) {
        WLOGFE("rsDisplayNode_ is nullptr");
        return;
    }
    WLOGFI("AbstractScreen::UpdateRSTree");

    if (isAdd) {
        rsDisplayNode_->AddChild(surfaceNode, -1);
    } else {
        rsDisplayNode_->RemoveChild(surfaceNode);
    }
}

void AbstractScreen::InitRSDisplayNode(RSDisplayNodeConfig& config)
{
    std::shared_ptr<RSDisplayNode> rsDisplayNode = RSDisplayNode::Create(config);
    if (rsDisplayNode == nullptr) {
        WLOGE("fail to add child. create rsDisplayNode fail!");
        return;
    }
    rsDisplayNode_ = rsDisplayNode;
    rSDisplayNodeConfig_ = config;
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
}

DMError AbstractScreen::GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts)
{
    auto ret = RSInterfaces::GetInstance().GetScreenSupportedColorGamuts(rsId_, colorGamuts);
    if (ret != StatusCode::SUCCESS) {
        WLOGE("GetScreenSupportedColorGamuts fail! rsId %{public}" PRIu64"", rsId_);
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("GetScreenSupportedColorGamuts ok! rsId %{public}" PRIu64", size %{public}u",
        rsId_, static_cast<uint32_t>(colorGamuts.size()));

    return DMError::DM_OK;
}

DMError AbstractScreen::GetScreenColorGamut(ScreenColorGamut& colorGamut)
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

DMError AbstractScreen::SetScreenColorGamut(int32_t colorGamutIdx)
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

DMError AbstractScreen::GetScreenGamutMap(ScreenGamutMap& gamutMap)
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

DMError AbstractScreen::SetScreenGamutMap(ScreenGamutMap gamutMap)
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

DMError AbstractScreen::SetScreenColorTransform()
{
    WLOGI("SetScreenColorTransform ok! rsId %{public}" PRIu64"", rsId_);

    return DMError::DM_OK;
}

void AbstractScreen::FillScreenInfo(sptr<ScreenInfo> info) const
{
    info->id_ = dmsId_;
    uint32_t width = 0;
    uint32_t height = 0;
    if (activeIdx_ >= 0 && activeIdx_ < modes_.size()) {
        sptr<SupportedScreenModes> abstractScreenModes = modes_[activeIdx_];
        height = abstractScreenModes->height_;
        width = abstractScreenModes->width_;
    }
    float virtualPixelRatio = virtualPixelRatio_;
    // "< 1e-6" means virtualPixelRatio is 0.
    if (fabsf(virtualPixelRatio) < 1e-6) {
        virtualPixelRatio = 1.0f;
    }
    info->virtualPixelRatio_ = virtualPixelRatio;
    info->virtualHeight_ = height / virtualPixelRatio;
    info->virtualWidth_ = width / virtualPixelRatio;
    info->parent_ = groupDmsId_;
    info->isScreenGroup_ = isScreenGroup_;
    info->rotation_ = rotation_;
    info->orientation_ = orientation_;
    info->type_ = type_;
    info->modeId_ = activeIdx_;
    info->modes_ = modes_;
}

bool AbstractScreen::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
    return true;
}

Rotation AbstractScreen::CalcRotation(Orientation orientation) const
{
    if (activeIdx_ < 0 || activeIdx_ >= modes_.size()) {
        WLOGE("active mode index is wrong: %{public}d", activeIdx_);
        return Rotation::ROTATION_0;
    }
    sptr<SupportedScreenModes> info = modes_[activeIdx_];
    // virtical: phone(Plugin screen); horizontal: pad & external screen
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

AbstractScreenGroup::AbstractScreenGroup(sptr<AbstractScreenController> screenController, ScreenId dmsId, ScreenId rsId,
    std::string name, ScreenCombination combination) : AbstractScreen(screenController, name, dmsId, rsId),
    combination_(combination)
{
    type_ = ScreenType::UNDEFINE;
    isScreenGroup_ = true;
}

AbstractScreenGroup::~AbstractScreenGroup()
{
    rsDisplayNode_ = nullptr;
    abstractScreenMap_.clear();
}

sptr<ScreenGroupInfo> AbstractScreenGroup::ConvertToScreenGroupInfo() const
{
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    if (screenGroupInfo == nullptr) {
        return nullptr;
    }
    FillScreenInfo(screenGroupInfo);
    screenGroupInfo->combination_ = combination_;
    for (auto iter = abstractScreenMap_.begin(); iter != abstractScreenMap_.end(); iter++) {
        screenGroupInfo->children_.push_back(iter->first);
    }
    auto positions = GetChildrenPosition();
    screenGroupInfo->position_.insert(screenGroupInfo->position_.end(), positions.begin(), positions.end());
    return screenGroupInfo;
}

bool AbstractScreenGroup::GetRSDisplayNodeConfig(sptr<AbstractScreen>& dmsScreen, struct RSDisplayNodeConfig& config)
{
    if (dmsScreen == nullptr) {
        WLOGE("dmsScreen is nullptr.");
        return false;
    }
    switch (combination_) {
        case ScreenCombination::SCREEN_ALONE:
        case ScreenCombination::SCREEN_EXPAND:
            config = { dmsScreen->rsId_ };
            break;
        case ScreenCombination::SCREEN_MIRROR: {
            if (GetChildCount() == 0 || mirrorScreenId_ == dmsScreen->dmsId_) {
                WLOGI("AddChild, SCREEN_MIRROR, config is not mirror");
                config = { dmsScreen->rsId_ };
                break;
            }
            if (mirrorScreenId_ == INVALID_SCREEN_ID || !HasChild(mirrorScreenId_)) {
                WLOGI("AddChild, mirrorScreenId_ is invalid, use default screen");
                mirrorScreenId_ = screenController_->GetDefaultAbstractScreenId();
            }
            std::shared_ptr<RSDisplayNode> displayNode = screenController_->GetRSDisplayNodeByScreenId(mirrorScreenId_);
            if (displayNode == nullptr) {
                WLOGFE("AddChild fail, displayNode is nullptr, cannot get DisplayNode");
                return false;
            }
            NodeId nodeId = displayNode->GetId();
            WLOGI("AddChild, mirrorScreenId_:%{public}" PRIu64", rsId_:%{public}" PRIu64", nodeId:%{public}" PRIu64"",
                mirrorScreenId_, dmsScreen->rsId_, nodeId);
            config = {dmsScreen->rsId_, true, nodeId};
            break;
        }
        default:
            WLOGE("fail to add child. invalid group combination:%{public}u", combination_);
            return false;
    }
    return true;
}

bool AbstractScreenGroup::AddChild(sptr<AbstractScreen>& dmsScreen, Point& startPoint)
{
    if (dmsScreen == nullptr) {
        WLOGE("AddChild, dmsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = dmsScreen->dmsId_;
    auto iter = abstractScreenMap_.find(screenId);
    if (iter != abstractScreenMap_.end()) {
        WLOGE("AddChild, abstractScreenMap_ has dmsScreen:%{public}" PRIu64"", screenId);
        return false;
    }
    struct RSDisplayNodeConfig config;
    if (!GetRSDisplayNodeConfig(dmsScreen, config)) {
        return false;
    }
    dmsScreen->InitRSDisplayNode(config);
    dmsScreen->groupDmsId_ = dmsId_;
    abstractScreenMap_.insert(std::make_pair(screenId, std::make_pair(dmsScreen, startPoint)));
    return true;
}

bool AbstractScreenGroup::AddChildren(std::vector<sptr<AbstractScreen>>& dmsScreens, std::vector<Point>& startPoints)
{
    size_t size = dmsScreens.size();
    if (size != startPoints.size()) {
        WLOGE("AddChildren, unequal size.");
        return false;
    }
    bool res = true;
    for (size_t i = 0; i < size; i++) {
        res = AddChild(dmsScreens[i], startPoints[i]) && res;
    }
    return res;
}

bool AbstractScreenGroup::RemoveChild(sptr<AbstractScreen>& dmsScreen)
{
    if (dmsScreen == nullptr) {
        WLOGE("RemoveChild, dmsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = dmsScreen->dmsId_;
    dmsScreen->groupDmsId_ = SCREEN_ID_INVALID;
    if (rsDisplayNode_ != nullptr) {
        rsDisplayNode_->RemoveFromTree();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
    }
    return abstractScreenMap_.erase(screenId);
}

bool AbstractScreenGroup::HasChild(ScreenId childScreen) const
{
    return abstractScreenMap_.find(childScreen) != abstractScreenMap_.end();
}

std::vector<sptr<AbstractScreen>> AbstractScreenGroup::GetChildren() const
{
    std::vector<sptr<AbstractScreen>> res;
    for (auto iter = abstractScreenMap_.begin(); iter != abstractScreenMap_.end(); iter++) {
        res.push_back(iter->second.first);
    }
    return res;
}

std::vector<Point> AbstractScreenGroup::GetChildrenPosition() const
{
    std::vector<Point> res;
    for (auto iter = abstractScreenMap_.begin(); iter != abstractScreenMap_.end(); iter++) {
        res.push_back(iter->second.second);
    }
    return res;
}

size_t AbstractScreenGroup::GetChildCount() const
{
    return abstractScreenMap_.size();
}
} // namespace OHOS::Rosen
