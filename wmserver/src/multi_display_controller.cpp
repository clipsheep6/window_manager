/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "multi_display_controller.h"

#include "window_node_container.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MultiDisplayController"};
}

void MultiDisplayController::InitWindowNodesAndSysBarMapForDisplay(DisplayId displayId)
{
    SysBarNodeMap sysBarNodeMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     nullptr },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, nullptr },
    };
    sysBarNodeMaps_.insert(std::make_pair(displayId, sysBarNodeMap));

    SysBarTintMap sysBarTintMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarRegionTint() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarRegionTint() },
    };
    sysBarTintMaps_.insert(std::make_pair(displayId, sysBarTintMap));

    std::map<WindowRootNodeType, std::unique_ptr<std::vector<sptr<WindowNode>>>> windowRootNodeMap;
    windowRootNodeMap.insert(std::make_pair(WindowRootNodeType::APP_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    windowRootNodeMap.insert(std::make_pair(WindowRootNodeType::ABOVE_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    windowRootNodeMap.insert(std::make_pair(WindowRootNodeType::BELOW_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    windowNodeMaps_.insert(std::make_pair(displayId, std::move(windowRootNodeMap)));
}

std::vector<sptr<WindowNode>>* MultiDisplayController::FindNodeVectorOfRoot(DisplayId displayId,
                                                                            WindowRootNodeType type)
{
    if (windowNodeMaps_.find(displayId) != windowNodeMaps_.end()) {
        auto& rootNodemap = windowNodeMaps_[displayId];
        if (rootNodemap.find(type) != rootNodemap.end()) {
            return rootNodemap[type].get();
        }
    }
    return nullptr;
}

void MultiDisplayController::AddWindowNodeInRootNodeVector(sptr<WindowNode>& node, WindowRootNodeType rootType)
{
    std::vector<sptr<WindowNode>>* rootNodeVectorPtr = FindNodeVectorOfRoot(node->GetDisplayId(), rootType);
    if (rootNodeVectorPtr != nullptr) {
        rootNodeVectorPtr->push_back(node);
        WLOGFI("add node in node vector of root, displayId: %{public}" PRIu64" windowId: %{public}d, "
            "rootType: %{public}d", node->GetDisplayId(), node->GetWindowId(), rootType);
    } else {
        WLOGFE("add node failed, rootNode vector is empty, windowId: %{public}d, rootType: %{public}d",
            node->GetWindowId(), rootType);
    }
}

void MultiDisplayController::UpdateWindowNodeMaps()
{
    // clear ori windowNodeMaps
    for (auto& elem : windowNodeMaps_) {
        for (auto& nodeVec : elem.second) {
            auto emptyVector = std::vector<sptr<WindowNode>>();
            nodeVec.second->swap(emptyVector);
        }
    }
    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (size_t index = 0; index < rootNodeType.size(); ++index) {
        auto rootType = rootNodeType[index];
        auto rootNode = windowNodeContainer_->GetRootNode(rootType);
        if (rootNode == nullptr) {
            WLOGFE("rootNode is nullptr, %{public}d", rootType);
        }
        for (auto& node : rootNode->children_) {
            AddWindowNodeInRootNodeVector(node, rootType);
        }
    }
}

void MultiDisplayController::MoveCrossNodeToDefaultDisplay(const sptr<WindowNode>& node)
{
    WLOGFI("MoveCrossNodeToDefaultDisplay");
}

void MultiDisplayController::ProcessCrossDisplayNodesIfDisplayChange()
{
    WLOGFI("ProcessCrossDisplayNodesIfDisplayChange");
}

void MultiDisplayController::UpdateWindowCrossDisplayState(const sptr<WindowNode>& node, const Rect& requestRect)
{
    WLOGFI("UpdateWindowCrossDisplayState");
}

void MultiDisplayController::UpdateWindowDisplayIdIfNeeded(const sptr<WindowNode>& node,
                                                           const std::vector<DisplayId>& oldShownDisplays,
                                                           const std::vector<DisplayId>& newShownDisplays)
{
    WLOGFI("UpdateWindowDisplayIdIfNeeded");
}

void MultiDisplayController::PreProcessWindowNode(const sptr<WindowNode>& node, WindowUpdateType type)
{
    if (!windowNodeContainer_->GetLayoutPolicy()->IsMutiDisplay()) {
        if (type == WindowUpdateType::WINDOW_UPDATE_ADDED) {
            node->SetShownDisplay( {node->GetDisplayId()} );
            for (auto& childNode : node->children_) {
                PreProcessWindowNode(childNode, type);
            }
        }
        WLOGFI("Current mode is not muti-display");
        return;
    }
    // current mutiDisplay is only support left-right combination, maxNum is two
    Rect requestRect = node->GetRequestRect();
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED: {
            if (!node->isCrossDisplay_) {
                const Rect& displayRect = displayRectMap_[node->GetDisplayId()];
                requestRect.posX_ += displayRect.posX_;
                requestRect.posY_ += displayRect.posY_;
                node->SetRequestRect(requestRect);
                node->SetShownDisplay( { node->GetDisplayId() } );
            } else {
                UpdateWindowCrossDisplayState(node, requestRect);
            }
            WLOGFI("preprocess node when add window");
            break;
        }
        case WindowUpdateType::WINDOW_UPDATE_REMOVED: {
            if (!node->isCrossDisplay_) {
                const Rect& displayRect = displayRectMap_[node->GetDisplayId()];
                requestRect.posX_ -= displayRect.posX_;
                requestRect.posY_ -= displayRect.posY_;
                node->SetRequestRect(requestRect);
            }
            WLOGFI("preprocess node when destroy window");
            break;
        }
        case WindowUpdateType::WINDOW_UPDATE_ACTIVE: {
            const auto oldShownDisplays = node->GetShownDisplay();
            UpdateWindowCrossDisplayState(node, requestRect);
            const auto& newShownDisplays = node->GetShownDisplay();
            UpdateWindowDisplayIdIfNeeded(node, oldShownDisplays, newShownDisplays);
            WLOGFI("preprocess node when update window");
            break;
        }
        default:
            break;
    }

    for (auto& childNode : node->children_) {
        PreProcessWindowNode(childNode, type);
    }
}

void MultiDisplayController::MoveWindowNodeToDefaultDisplay(DisplayId displayId, const sptr<WindowNode>& node)
{
    WLOGFI("MoveWindowNodeToDefaultDisplay");
}

void MultiDisplayController::ProcessWindowNodesOnDestroiedDisplay(DisplayId displayId, std::vector<uint32_t>& windowIds)
{
    WLOGFI("ProcessWindowNodesOnDestroiedDisplay");
}

void MultiDisplayController::ProcessDisplayCreate(DisplayId displayId,
                                                  const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap)
{
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end() ||
        displayInfoMap.size() != (displayInfosMap_.size() + 1)) {
        WLOGFE("current display is exited or displayInfo map size is error, displayId: %{public}" PRIu64"", displayId);
        return;
    }

    defaultDisplayId_ = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    WLOGFI("defaultDisplay, displayId: %{public}" PRIu64"", defaultDisplayId_);

    windowNodeContainer_->GetAvoidController()->UpdateAvoidNodesMap(displayId, true);
    InitWindowNodesAndSysBarMapForDisplay(displayId);

    // modify RSTree and windowNodeMaps for cross-display nodes
    ProcessCrossDisplayNodesIfDisplayChange();
    UpdateWindowNodeMaps();

    for (auto& elem : displayInfoMap) {
        auto iter = displayInfosMap_.find(elem.first);
        const auto& displayInfo = elem.second;
        Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
        if (iter != displayInfosMap_.end()) {
            displayRectMap_[elem.first] = displayRect;
            displayInfosMap_[elem.first] = displayInfo;
        } else {
            displayRectMap_.insert(std::make_pair(elem.first, displayRect));
            displayInfosMap_.insert(std::make_pair(elem.first, displayInfo));
        }
        WLOGFI("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
            elem.first, displayRect.posX_, displayRect.posY_,displayRect.width_, displayRect.height_);
    }
    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplayCreate(displayId, displayRectMap_);
}

void MultiDisplayController::ProcessDisplayDestroy(DisplayId displayId,
                                                   const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap,
                                                   std::vector<uint32_t>& windowIds)
{
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end() ||
        (displayInfoMap.size() + 1) != displayInfosMap_.size()) {
        WLOGFE("can not find current display or displayInfo map size is error, displayId: %{public}" PRIu64"",
               displayId);
        return;
    }

    windowNodeContainer_->GetAvoidController()->UpdateAvoidNodesMap(displayId, false);

    // delete nodes and map element of deleted display
    ProcessWindowNodesOnDestroiedDisplay(displayId, windowIds);
    // modify RSTree and windowNodeMaps for cross-display nodes
    ProcessCrossDisplayNodesIfDisplayChange();
    UpdateWindowNodeMaps();

    sysBarTintMaps_.erase(displayId);
    sysBarNodeMaps_.erase(displayId);
    windowNodeMaps_.erase(displayId);
    displayRectMap_.erase(displayId);
    displayInfosMap_.erase(displayId);

    for (auto& elem : displayInfoMap) {
        auto iter = displayInfosMap_.find(elem.first);
        const auto& displayInfo = elem.second;
        Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
        if (iter != displayInfosMap_.end()) {
            displayRectMap_[elem.first] = displayRect;
            displayInfosMap_[elem.first] = displayInfo;
            WLOGFI("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
                elem.first, displayRect.posX_, displayRect.posY_, displayRect.width_, displayRect.height_);
        }
    }

    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplayDestroy(displayId, displayRectMap_);
}

void MultiDisplayController::ProcessDisplayChange(const sptr<DisplayInfo>& displayInfo, DisplayStateChangeType type)
{
    DisplayId displayId = displayInfo->GetDisplayId();
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end()) {
        WLOGFE("can not find current display, displayId: %{public}" PRIu64", type: %{public}d", displayId, type);
        return;
    }
    WLOGFI("display change, displayId: %{public}" PRIu64", type: %{public}d", displayId, type);
    switch (type) {
        case DisplayStateChangeType::UPDATE_ROTATION: {
            displayInfosMap_[displayId]->SetRotation(displayInfo->GetRotation());
        }
        case DisplayStateChangeType::SIZE_CHANGE: {
            ProcessDisplaySizeChange(displayInfo, type);
            break;
        }
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            displayInfosMap_[displayId]->SetVirtualPixelRatio(displayInfo->GetVirtualPixelRatio());
            windowNodeContainer_->GetLayoutPolicy()->LayoutWindowTree(displayId);
            break;
        }
        default: {
            break;
        }
    }
}

void MultiDisplayController::ProcessDisplaySizeChange(const sptr<DisplayInfo>& displayInfo, DisplayStateChangeType type)
{
    DisplayId displayId = displayInfo->GetDisplayId();

    displayInfosMap_[displayId]->SetWidth(displayInfo->GetWidth());
    displayInfosMap_[displayId]->SetHeight(displayInfo->GetHeight());
    // modify RSTree and windowNodeMaps for cross-display nodes
    ProcessCrossDisplayNodesIfDisplayChange();
    UpdateWindowNodeMaps();
    Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
        displayInfo->GetWidth(), displayInfo->GetHeight() };
    displayRectMap_[displayId] = displayRect;
    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplaySizeChange(displayId, displayRect);
}
}
}
