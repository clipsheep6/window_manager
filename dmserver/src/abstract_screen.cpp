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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DMS_DM_SERVER, "AbstractScreenGroup"};
    constexpr float MAX_ZORDER = 100000.0f;
}

AbstractScreen::AbstractScreen(sptr<AbstractScreenController> screenController, const std::string& name, ScreenId dmsId,
    ScreenId rsId) : dmsId_(dmsId), rsId_(rsId), screenController_(screenController)
{
    if (name != "") {
        name_ = name;
    }
}

AbstractScreen::~AbstractScreen()
{
}

sptr<SupportedScreenModes> AbstractScreen::GetActiveScreenMode() const
{
    if (activeIdx_ < 0 || activeIdx_ >= static_cast<int32_t>(modes_.size())) {
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
    if (screenController_ == nullptr) {
        return nullptr;
    }
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

void AbstractScreen::UpdateRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd, bool needToUpdate)
{
    if (rsDisplayNode_ == nullptr || surfaceNode == nullptr) {
        WLOGFE("node is nullptr");
        return;
    }
    WLOGFD("%{public}s surface: %{public}s, %{public}" PRIu64"", (isAdd ? "add" : "remove"),
        surfaceNode->GetName().c_str(), surfaceNode->GetId());

    if (isAdd) {
        surfaceNode->SetVisible(true);
        rsDisplayNode_->AddChild(surfaceNode, -1);
    } else {
        rsDisplayNode_->RemoveChild(surfaceNode);
    }

    if (needToUpdate) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (isAdd) {
            appSurfaceNodes_.push_back(surfaceNode);
        } else {
            auto iter = std::find_if(appSurfaceNodes_.begin(), appSurfaceNodes_.end(),
                [surfaceNode] (std::shared_ptr<RSSurfaceNode> node) {
                    return surfaceNode->GetId() == node->GetId();
                });
            if (iter != appSurfaceNodes_.end()) {
                appSurfaceNodes_.erase(iter);
            }
        }
    }
}

DMError AbstractScreen::AddSurfaceNode(std::shared_ptr<RSSurfaceNode>& surfaceNode, bool onTop, bool needToRecord)
{
    if (rsDisplayNode_ == nullptr || surfaceNode == nullptr) {
        WLOGFE("node is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    surfaceNode->SetVisible(true);
    if (onTop) {
        rsDisplayNode_->AddChild(surfaceNode, -1);
        surfaceNode->SetPositionZ(MAX_ZORDER);
    } else {
        rsDisplayNode_->AddChild(surfaceNode, -1);
    }
    if (needToRecord) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        nativeSurfaceNodes_.push_back(surfaceNode);
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    return DMError::DM_OK;
}

DMError AbstractScreen::RemoveSurfaceNode(std::shared_ptr<RSSurfaceNode>& surfaceNode)
{
    if (rsDisplayNode_ == nullptr || surfaceNode == nullptr) {
        WLOGFE("Node is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    };
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find_if(nativeSurfaceNodes_.begin(), nativeSurfaceNodes_.end(), [surfaceNode]
        (std::shared_ptr<RSSurfaceNode> node) {
        return surfaceNode->GetId() == node->GetId();
    });
    if (iter == nativeSurfaceNodes_.end()) {
        WLOGFW("Child not found");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    rsDisplayNode_->RemoveChild(*iter);
    nativeSurfaceNodes_.erase(iter);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    return DMError::DM_OK;
}

void AbstractScreen::UpdateDisplayGroupRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, NodeId parentNodeId,
    bool isAdd)
{
    if (rsDisplayNode_ == nullptr || surfaceNode == nullptr) {
        WLOGFE("node is nullptr");
        return;
    }
    WLOGFI("%{public}s surface: %{public}s, %{public}" PRIu64"", (isAdd ? "add" : "remove"),
        surfaceNode->GetName().c_str(), surfaceNode->GetId());

    if (isAdd) {
        surfaceNode->SetVisible(true);
        rsDisplayNode_->AddCrossParentChild(surfaceNode, -1);
    } else {
        rsDisplayNode_->RemoveCrossParentChild(surfaceNode, parentNodeId);
    }
}

void AbstractScreen::SetPropertyForDisplayNode(const std::shared_ptr<RSDisplayNode>& rsDisplayNode,
    const RSDisplayNodeConfig& config, const Point& startPoint)
{
    rSDisplayNodeConfig_ = config;
    startPoint_ = startPoint;
    WLOGFI("SetDisplayOffset: posX:%{public}d, posY:%{public}d", startPoint.posX_, startPoint.posY_);
    rsDisplayNode->SetDisplayOffset(startPoint.posX_, startPoint.posY_);
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
        rsDisplayNode->SetSecurityDisplay(true);
        WLOGFI("virtualScreen SetSecurityDisplay success");
    }
    // If setDisplayOffset is not valid for SetFrame/SetBounds
    rsDisplayNode->SetFrame(0, 0, width, height);
    rsDisplayNode->SetBounds(0, 0, width, height);
}

void AbstractScreen::InitRSDisplayNode(const RSDisplayNodeConfig& config, const Point& startPoint)
{
    if (rsDisplayNode_ != nullptr) {
        rsDisplayNode_->SetDisplayNodeMirrorConfig(config);
        WLOGFD("RSDisplayNode is not null");
    } else {
        WLOGFD("Create rsDisplayNode");
        std::shared_ptr<RSDisplayNode> rsDisplayNode = RSDisplayNode::Create(config);
        if (rsDisplayNode == nullptr) {
            WLOGE("fail to add child. create rsDisplayNode fail!");
            return;
        }
        rsDisplayNode_ = rsDisplayNode;
    }
    SetPropertyForDisplayNode(rsDisplayNode_, config, startPoint);

    // flush transaction
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    WLOGFD("InitRSDisplayNode success");
}

void AbstractScreen::InitRSDefaultDisplayNode(const RSDisplayNodeConfig& config, const Point& startPoint)
{
    if (rsDisplayNode_ == nullptr) {
        WLOGFD("RSDisplayNode is nullptr");
    }

    WLOGFD("Create defaultRSDisplayNode");
    std::shared_ptr<RSDisplayNode> rsDisplayNode = RSDisplayNode::Create(config);
    if (rsDisplayNode == nullptr) {
        WLOGE("fail to add child. create rsDisplayNode fail!");
        return;
    }
    rsDisplayNode_ = rsDisplayNode;
    SetPropertyForDisplayNode(rsDisplayNode_, config, startPoint);

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    // update RSTree for default display
    for (auto node: appSurfaceNodes_) {
        UpdateRSTree(node, true, false);
    }
    for (auto node: nativeSurfaceNodes_) {
        AddSurfaceNode(node, false, false);
    }

    // flush transaction
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    WLOGFD("InitRSDefaultDisplayNode success");
}

void AbstractScreen::UpdateRSDisplayNode(Point startPoint)
{
    WLOGD("update display offset from [%{public}d %{public}d] to [%{public}d %{public}d]",
        startPoint_.posX_, startPoint_.posY_, startPoint.posX_, startPoint.posY_);
    if (rsDisplayNode_ == nullptr) {
        WLOGFD("rsDisplayNode_ is nullptr");
        return;
    }
    
    startPoint_ = startPoint;
    rsDisplayNode_->SetDisplayOffset(startPoint.posX_, startPoint.posY_);
}

ScreenId AbstractScreen::GetScreenGroupId() const
{
    return groupDmsId_;
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
    if (info == nullptr) {
        WLOGE("FillScreenInfo failed! info is nullptr");
        return;
    }
    info->id_ = dmsId_;
    info->name_ = name_;
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<SupportedScreenModes> abstractScreenModes = GetActiveScreenMode();
    if (abstractScreenModes != nullptr) {
        height = abstractScreenModes->height_;
        width = abstractScreenModes->width_;
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
    info->lastParent_ = lastGroupDmsId_;
    info->parent_ = groupDmsId_;
    info->isScreenGroup_ = isScreenGroup_;
    info->rotation_ = rotation_;
    info->orientation_ = orientation_;
    info->sourceMode_ = sourceMode;
    info->type_ = type_;
    info->modeId_ = activeIdx_;
    info->modes_ = modes_;
}

bool AbstractScreen::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
    return true;
}

bool AbstractScreen::SetVirtualPixelRatio(float virtualPixelRatio)
{
    virtualPixelRatio_ = virtualPixelRatio;
    return true;
}

float AbstractScreen::GetVirtualPixelRatio() const
{
    return virtualPixelRatio_;
}

ScreenSourceMode AbstractScreen::GetSourceMode() const
{
    sptr<AbstractScreenGroup> abstractScreenGroup = GetGroup();
    if (abstractScreenGroup == nullptr || screenController_ == nullptr) {
        return ScreenSourceMode::SCREEN_ALONE;
    }
    ScreenId defaultId = screenController_->GetDefaultAbstractScreenId();
    if (dmsId_ == defaultId) {
        return ScreenSourceMode::SCREEN_MAIN;
    }
    ScreenCombination combination = abstractScreenGroup->GetScreenCombination();
    switch (combination) {
        case ScreenCombination::SCREEN_MIRROR: {
            return ScreenSourceMode::SCREEN_MIRROR;
        }
        case ScreenCombination::SCREEN_EXPAND: {
            return ScreenSourceMode::SCREEN_EXTEND;
        }
        case ScreenCombination::SCREEN_ALONE: {
            return ScreenSourceMode::SCREEN_ALONE;
        }
        default: {
            return ScreenSourceMode::SCREEN_ALONE;
        }
    }
}

Rotation AbstractScreen::CalcRotation(Orientation orientation) const
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

const std::string& AbstractScreen::GetScreenName() const
{
    return name_;
}

void AbstractScreen::SetPhyWidth(uint32_t phyWidth)
{
    phyWidth_ = phyWidth;
}

void AbstractScreen::SetPhyHeight(uint32_t phyHeight)
{
    phyHeight_ = phyHeight;
}

uint32_t AbstractScreen::GetPhyWidth() const
{
    return phyWidth_;
}

uint32_t AbstractScreen::GetPhyHeight() const
{
    return phyHeight_;
}

AbstractScreenGroup::AbstractScreenGroup(sptr<AbstractScreenController> screenController, ScreenId dmsId, ScreenId rsId,
    std::string name, ScreenCombination combination) : AbstractScreen(screenController, name, dmsId, rsId),
    combination_(combination)
{
    type_ = ScreenType::UNDEFINED;
    isScreenGroup_ = true;
}

AbstractScreenGroup::~AbstractScreenGroup()
{
    rsDisplayNode_ = nullptr;
}

sptr<ScreenGroupInfo> AbstractScreenGroup::ConvertToScreenGroupInfo() const
{
    sptr<ScreenGroupInfo> screenGroupInfo = new(std::nothrow) ScreenGroupInfo();
    if (screenGroupInfo == nullptr) {
        return nullptr;
    }
    FillScreenInfo(screenGroupInfo);
    screenGroupInfo->combination_ = combination_;
    for (auto iter = screenMap_.begin(); iter != screenMap_.end(); iter++) {
        screenGroupInfo->children_.push_back(iter->first);
        screenGroupInfo->position_.push_back(iter->second->startPoint_);
    }
    return screenGroupInfo;
}

bool AbstractScreenGroup::GetRSDisplayNodeConfig(sptr<AbstractScreen>& dmsScreen, struct RSDisplayNodeConfig& config)
{
    if (dmsScreen == nullptr) {
        WLOGE("dmsScreen is nullptr.");
        return false;
    }
    config = { dmsScreen->rsId_ };
    switch (combination_) {
        case ScreenCombination::SCREEN_ALONE:
            [[fallthrough]];
        case ScreenCombination::SCREEN_EXPAND:
            break;
        case ScreenCombination::SCREEN_MIRROR: {
            if (GetChildCount() == 0 || mirrorScreenId_ == dmsScreen->dmsId_) {
                WLOGI("AddChild, SCREEN_MIRROR, config is not mirror");
                break;
            }
            if (screenController_ == nullptr) {
                return false;
            }
            if (mirrorScreenId_ == SCREEN_ID_INVALID || !HasChild(mirrorScreenId_)) {
                WLOGI("AddChild, mirrorScreenId_ is invalid, use default screen");
                mirrorScreenId_ = screenController_->GetDefaultAbstractScreenId();
            }
            // Todo displayNode is nullptr
            std::shared_ptr<RSDisplayNode> displayNode = screenController_->GetRSDisplayNodeByScreenId(mirrorScreenId_);
            if (displayNode == nullptr) {
                WLOGFE("AddChild fail, displayNode is nullptr, cannot get DisplayNode");
                break;
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
    WLOGFD("AbstractScreenGroup AddChild dmsScreenId: %{public}" PRIu64"", screenId);
    auto iter = screenMap_.find(screenId);
    if (iter != screenMap_.end()) {
        if (dmsScreen->rsDisplayNode_ != nullptr && dmsScreen->type_ == ScreenType::REAL &&
            defaultScreenId_ == screenId) {
            WLOGFD("Add default screen, id: %{public}" PRIu64"", screenId);
        } else {
            WLOGE("AddChild, screenMap_ has dmsScreen:%{public}" PRIu64"", screenId);
            return false;
        }
    }
    struct RSDisplayNodeConfig config;
    if (!GetRSDisplayNodeConfig(dmsScreen, config)) {
        return false;
    }
    if (dmsScreen->rsDisplayNode_ != nullptr && dmsScreen->type_ == ScreenType::REAL &&
        defaultScreenId_ == screenId) {
        WLOGFD("Reconnect default screen, screenId: %{public}" PRIu64"", screenId);
        dmsScreen->InitRSDefaultDisplayNode(config, startPoint);
    } else {
        dmsScreen->InitRSDisplayNode(config, startPoint);
        dmsScreen->lastGroupDmsId_ = dmsScreen->groupDmsId_;
        dmsScreen->groupDmsId_ = dmsId_;
        screenMap_.insert(std::make_pair(screenId, dmsScreen));
    }
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
    dmsScreen->lastGroupDmsId_ = dmsScreen->groupDmsId_;
    dmsScreen->groupDmsId_ = SCREEN_ID_INVALID;
    dmsScreen->startPoint_ = Point();
    if (dmsScreen->rsDisplayNode_ != nullptr) {
        dmsScreen->rsDisplayNode_->SetDisplayOffset(0, 0);
        dmsScreen->rsDisplayNode_->RemoveFromTree();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        dmsScreen->rsDisplayNode_ = nullptr;
    }
    WLOGFD("groupDmsId:%{public}" PRIu64", screenId:%{public}" PRIu64"",
        dmsScreen->groupDmsId_, screenId);
    return screenMap_.erase(screenId);
}

bool AbstractScreenGroup::RemoveDefaultScreen(const sptr<AbstractScreen>& dmsScreen)
{
    if (dmsScreen == nullptr) {
        WLOGE("RemoveChild, dmsScreen is nullptr.");
        return false;
    }
    ScreenId screenId = dmsScreen->dmsId_;
    dmsScreen->lastGroupDmsId_ = dmsScreen->groupDmsId_;
    if (dmsScreen->rsDisplayNode_ != nullptr) {
        dmsScreen->rsDisplayNode_->SetDisplayOffset(0, 0);
        dmsScreen->rsDisplayNode_->RemoveFromTree();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
    }
    defaultScreenId_ = screenId;
    WLOGFD("groupDmsId:%{public}" PRIu64", screenId:%{public}" PRIu64"",
        dmsScreen->groupDmsId_, screenId);
    return true;
}

bool AbstractScreenGroup::HasChild(ScreenId childScreen) const
{
    return screenMap_.find(childScreen) != screenMap_.end();
}

std::vector<sptr<AbstractScreen>> AbstractScreenGroup::GetChildren() const
{
    std::vector<sptr<AbstractScreen>> res;
    for (auto iter = screenMap_.begin(); iter != screenMap_.end(); iter++) {
        res.push_back(iter->second);
    }
    return res;
}

std::vector<Point> AbstractScreenGroup::GetChildrenPosition() const
{
    std::vector<Point> res;
    for (auto iter = screenMap_.begin(); iter != screenMap_.end(); iter++) {
        res.push_back(iter->second->startPoint_);
    }
    return res;
}

Point AbstractScreenGroup::GetChildPosition(ScreenId screenId) const
{
    Point point;
    auto iter = screenMap_.find(screenId);
    if (iter != screenMap_.end()) {
        point = iter->second->startPoint_;
    }
    return point;
}

size_t AbstractScreenGroup::GetChildCount() const
{
    return screenMap_.size();
}

ScreenCombination AbstractScreenGroup::GetScreenCombination() const
{
    return combination_;
}
} // namespace OHOS::Rosen
