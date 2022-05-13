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

#include "window_layout_policy.h"
#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicy"};
}

WindowLayoutPolicy::WindowLayoutPolicy(const std::map<DisplayId, Rect>& displayRectMap,
    WindowNodeMaps& windowNodeMaps, std::map<DisplayId, sptr<DisplayInfo>>& displayInfosMap)
    : displayRectMap_(displayRectMap), windowNodeMaps_(windowNodeMaps), displayInfosMap_(displayInfosMap)
{
}

void WindowLayoutPolicy::Launch()
{
    WLOGFI("WindowLayoutPolicy::Launch");
}

void WindowLayoutPolicy::Clean()
{
    WLOGFI("WindowLayoutPolicy::Clean");
}

void WindowLayoutPolicy::Reorder()
{
    WLOGFI("WindowLayoutPolicy::Reorder");
}

void WindowLayoutPolicy::UpdateFullDisplayRect()
{
    Rect newDisplayRect = { 0, 0, 0, 0 };
    // current mutiDisplay is only support left-right combination, maxNum is two
    for (auto& elem : displayRectMap_) {
        newDisplayRect.posX_ = std::min(fullDisplayRect_.posX_, elem.second.posX_);
        newDisplayRect.posY_ = std::min(fullDisplayRect_.posY_, elem.second.posY_);
        newDisplayRect.width_ += elem.second.width_;
        int32_t maxHeight = std::max(newDisplayRect.posY_ + static_cast<int32_t>(newDisplayRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newDisplayRect.height_ = maxHeight - newDisplayRect.posY_;
    }
    fullDisplayRect_ = newDisplayRect;
    WLOGFI("fullDisplayRect_: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        fullDisplayRect_.posX_, fullDisplayRect_.posY_, fullDisplayRect_.width_, fullDisplayRect_.height_);
}

void WindowLayoutPolicy::UpdateFullLimitRect()
{
    auto firstLimitRect = limitRectMap_.begin()->second;
    Rect newFullLimitRect = { firstLimitRect.posX_, firstLimitRect.posY_, 0, 0 };
    for (auto& elem : limitRectMap_) {
        newFullLimitRect.posX_ = std::min(newFullLimitRect.posX_, elem.second.posX_);
        newFullLimitRect.posY_ = std::min(newFullLimitRect.posY_, elem.second.posY_);

        int32_t maxWidth = std::max(newFullLimitRect.posX_ + static_cast<int32_t>(newFullLimitRect.width_),
                                    elem.second.posX_+ static_cast<int32_t>(elem.second.width_));

        int32_t maxHeight = std::max(newFullLimitRect.posY_ + static_cast<int32_t>(newFullLimitRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newFullLimitRect.width_  = maxWidth - newFullLimitRect.posX_;
        newFullLimitRect.height_ = maxHeight - newFullLimitRect.posY_;
    }
    fullLimitRect_ = newFullLimitRect;
    WLOGFI("fullLimitRect_: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        fullLimitRect_.posX_, fullLimitRect_.posY_, fullLimitRect_.width_, fullLimitRect_.height_);
}

bool WindowLayoutPolicy::IsMutiDisplay()
{
    return isMutiDisplay_;
}

void WindowLayoutPolicy::UpdateMutiDisplayFlag()
{
    if (displayRectMap_.size() > 1) {
        isMutiDisplay_ = true;
        WLOGFI("current mode is muti-display");
    } else {
        isMutiDisplay_ = false;
        WLOGFI("current mode is not muti-display");
    }
}

void WindowLayoutPolicy::LimitWindowToBottomRightCorner(const sptr<WindowNode>& node)
{
    WLOGFI("LimitWindowToBottomRightCorner");
}

void WindowLayoutPolicy::UpdateNodeAbsoluteCordinate(const sptr<WindowNode>& node,
                                                     const Rect& srcDisplayRect,
                                                     const Rect& dstDisplayRect)
{
    WLOGFI("UpdateNodeAbsoluteCordinate");
}

void WindowLayoutPolicy::UpdateNodesAbsoluteCordinatesInAllDisplay(DisplayId displayId,
                                                                   const Rect& srcDisplayRect,
                                                                   const Rect& dstDisplayRect)
{
    WLOGFI("UpdateNodesAbsoluteCordinatesInAllDisplay");
}

void WindowLayoutPolicy::AfterProcessDisplayChange()
{
    UpdateMutiDisplayFlag();
    UpdateFullDisplayRect();
    Launch();
    for (auto& elem : displayRectMap_) {
        LayoutWindowTree(elem.first);
        WLOGFI("LayoutWindowTree, displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, "
            "%{public}d]", elem.first, elem.second.posX_, elem.second.posY_, elem.second.width_, elem.second.height_);
    }
}

void WindowLayoutPolicy::ProcessDisplayCreate(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    for (auto& elem : displayRectMap) {
        auto iter = displayRectMap_.find(elem.first);
        if (iter != displayRectMap_.end()) {
            UpdateNodesAbsoluteCordinatesInAllDisplay(elem.first, iter->second, elem.second);
            iter->second = elem.second;
        } else {
            if (elem.first != displayId) {
                WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
                return;
            }
            displayRectMap_.insert(std::make_pair(displayId, elem.second));
        }
    }

    AfterProcessDisplayChange();
    WLOGFI("Process display create, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplayDestroy(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    for (auto oriIter = displayRectMap_.begin(); oriIter != displayRectMap_.end();) {
        auto newIter = displayRectMap.find(oriIter->first);
        if (newIter != displayRectMap.end()) {
            UpdateNodesAbsoluteCordinatesInAllDisplay(oriIter->first, oriIter->second, newIter->second);
            oriIter->second = newIter->second;
            ++oriIter;
        } else {
            if (oriIter->first != displayId) {
                WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
                return;
            }
            displayRectMap_.erase(oriIter++);
        }
    }

    AfterProcessDisplayChange();
    WLOGFI("Process display destroy, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplaySizeChange(DisplayId displayId, const Rect& displayRect)
{
    auto iter = displayRectMap_.find(displayId);
    if (iter == displayRectMap_.end()) {
        WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    if (isMutiDisplay_) {
        UpdateNodesAbsoluteCordinatesInAllDisplay(displayId, displayRectMap_[displayId], displayRect);
    }

    displayRectMap_[displayId] = displayRect;

    AfterProcessDisplayChange();
    WLOGFI("Process display change, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec)
{
    if (nodeVec.empty()) {
        WLOGE("The node vector is empty!");
        return;
    }
    for (auto& node : nodeVec) {
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::LayoutWindowTree(DisplayId displayId)
{
    auto& windowNodeMap = windowNodeMaps_[displayId];
    limitRectMap_[displayId] = displayRectMap_[displayId];
    // ensure that the avoid area windows are traversed first
    LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::ABOVE_WINDOW_NODE]));
    if (IsFullScreenRecentWindowExist(*(windowNodeMap[WindowRootNodeType::ABOVE_WINDOW_NODE]))) {
        WLOGFI("recent window on top, early exit layout tree");
        return;
    }
    LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::APP_WINDOW_NODE]));
    LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::BELOW_WINDOW_NODE]));
}

void WindowLayoutPolicy::LayoutWindowNode(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    WLOGFI("LayoutWindowNode, window[%{public}u]", node->GetWindowId());
    if (node->parent_ != nullptr) { // isn't root node
        if (!node->currentVisibility_) {
            WLOGFI("window[%{public}u] currently not visible, no need layout", node->GetWindowId());
            return;
        }
        UpdateLayoutRect(node);
        if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
            UpdateLimitRect(node, limitRectMap_[node->GetDisplayId()]);
            UpdateFullLimitRect();
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

bool WindowLayoutPolicy::IsVerticalDisplay(DisplayId displayId) const
{
    return displayRectMap_[displayId].width_ < displayRectMap_[displayId].height_;
}

void WindowLayoutPolicy::UpdateClientRectAndResetReason(const sptr<WindowNode>& node,
    const Rect& lastLayoutRect, const Rect& winRect)
{
    auto reason = node->GetWindowSizeChangeReason();
    if (node->GetWindowToken()) {
        WLOGFI("notify client id: %{public}d, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u], reason: "
            "%{public}u", node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_, reason);
        node->GetWindowToken()->UpdateWindowRect(winRect, node->GetDecoStatus(), reason);
    }
    if (reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::DRAG_END) {
        node->ResetWindowSizeChangeReason();
    }
}

void WindowLayoutPolicy::RemoveWindowNode(const sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree(node->GetDisplayId());
    }
    Rect reqRect = node->GetRequestRect();
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateWindowRect(reqRect, node->GetDecoStatus(), WindowSizeChangeReason::HIDE);
    }
}

void WindowLayoutPolicy::UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree(node->GetDisplayId());
    } else { // layout single window
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::UpdateFloatingLayoutRect(Rect& limitRect, Rect& winRect)
{
    winRect.width_ = std::min(limitRect.width_, winRect.width_);
    winRect.height_ = std::min(limitRect.height_, winRect.height_);
    winRect.posX_ = std::max(limitRect.posX_, winRect.posX_);
    winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
    winRect.posX_ = std::min(
        limitRect.posX_ + static_cast<int32_t>(limitRect.width_) - static_cast<int32_t>(winRect.width_),
        winRect.posX_);
    winRect.posY_ = std::min(
        limitRect.posY_ + static_cast<int32_t>(limitRect.height_) - static_cast<int32_t>(winRect.height_),
        winRect.posY_);
}

void WindowLayoutPolicy::ComputeDecoratedRequestRect(const sptr<WindowNode>& node) const
{
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGE("window property is nullptr");
        return;
    }
    auto reqRect = property->GetRequestRect();
    if (!property->GetDecorEnable() || property->GetDecoStatus()) {
        return;
    }
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect rect;
    rect.posX_ = reqRect.posX_;
    rect.posY_ = reqRect.posY_;
    rect.width_ = reqRect.width_ + winFrameW + winFrameW;
    rect.height_ = reqRect.height_ + winTitleBarH + winFrameW;
    property->SetRequestRect(rect);
    property->SetDecoStatus(true);
}

void WindowLayoutPolicy::CalcAndSetNodeHotZone(Rect layoutOutRect, const sptr<WindowNode>& node) const
{
    Rect rect = layoutOutRect;
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t hotZone = static_cast<uint32_t>(HOTZONE * virtualPixelRatio);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        if (rect.width_ < rect.height_) {
            rect.posX_ -= hotZone;
            rect.width_ += (hotZone + hotZone);
        } else {
            rect.posY_ -= hotZone;
            rect.height_ += (hotZone + hotZone);
        }
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT) {
        rect = displayRectMap_[node->GetDisplayId()];
    } else if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        rect.posX_ -= hotZone;
        rect.posY_ -= hotZone;
        rect.width_ += (hotZone + hotZone);
        rect.height_ += (hotZone + hotZone);
    }
    node->SetHotZoneRect(rect);
}

void WindowLayoutPolicy::LimitFloatingWindowSize(const sptr<WindowNode>& node,
                                                 const Rect& displayRect,
                                                 Rect& winRect) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);

    WindowType windowType = node->GetWindowType();
    WindowMode windowMode = node->GetWindowMode();
    bool isVertical = (displayRect.height_ > displayRect.width_) ? true : false;

    if (windowMode == WindowMode::WINDOW_MODE_FLOATING) {
        // limit minimum size of floating (not system type) window
        if (!WindowHelper::IsSystemWindow(windowType)) {
            if (isVertical) {
                winRect.width_ = std::max(minVerticalFloatingW, winRect.width_);
                winRect.height_ = std::max(minVerticalFloatingH, winRect.height_);
            } else {
                winRect.width_ = std::max(minVerticalFloatingH, winRect.width_);
                winRect.height_ = std::max(minVerticalFloatingW, winRect.height_);
            }
        }
        // limit maximum size of all floating window
        winRect.width_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), winRect.width_);
        winRect.height_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), winRect.height_);
    }
}

void WindowLayoutPolicy::LimitMainFloatingWindowPosition(const sptr<WindowNode>& node, Rect& winRect) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect limitRect;
    // if is corss-display window, the limit rect should be full limitRect
    if (node->isCrossDisplay_) {
        limitRect = fullLimitRect_;
    } else {
        limitRect = limitRectMap_[node->GetDisplayId()];
    }

    // limit position of the main floating window(window which support dragging)
    if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
        winRect.posY_ = std::min(limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - windowTitleBarH),
                                 winRect.posY_);

        winRect.posX_ = std::max(limitRect.posX_ + static_cast<int32_t>(windowTitleBarH - winRect.width_),
                                 winRect.posX_);
        winRect.posX_ = std::min(limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - windowTitleBarH),
                                 winRect.posX_);
    }
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect, DisplayId displayId) const
{
    if (displayInfosMap_.find(displayId) == std::end(displayInfosMap_)) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId: %{public}" PRIu64"", displayId);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    return WindowHelper::GetAvoidPosType(rect, displayInfosMap_[displayId]->GetWidth(),
        displayInfosMap_[displayId]->GetHeight());
}

void WindowLayoutPolicy::UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect)
{
    const auto& layoutRect = node->GetWindowRect();
    int32_t limitH = static_cast<int32_t>(limitRect.height_);
    int32_t limitW = static_cast<int32_t>(limitRect.width_);
    int32_t layoutH = static_cast<int32_t>(layoutRect.height_);
    int32_t layoutW = static_cast<int32_t>(layoutRect.width_);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto avoidPosType = GetAvoidPosType(layoutRect, node->GetDisplayId());
        int32_t offsetH = 0;
        int32_t offsetW = 0;
        switch (avoidPosType) {
            case AvoidPosType::AVOID_POS_TOP:
                offsetH = layoutRect.posY_ + layoutH - limitRect.posY_;
                limitRect.posY_ += offsetH;
                limitH -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_BOTTOM:
                offsetH = limitRect.posY_ + limitH - layoutRect.posY_;
                limitH -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_LEFT:
                offsetW = layoutRect.posX_ + layoutW - limitRect.posX_;
                limitRect.posX_ += offsetW;
                limitW -= offsetW;
                break;
            case AvoidPosType::AVOID_POS_RIGHT:
                offsetW = limitRect.posX_ + limitW - layoutRect.posX_;
                limitW -= offsetW;
                break;
            default:
                WLOGFE("invalid avoidPosType: %{public}d", avoidPosType);
        }
    }
    limitRect.height_ = static_cast<uint32_t>(limitH < 0 ? 0 : limitH);
    limitRect.width_ = static_cast<uint32_t>(limitW < 0 ? 0 : limitW);
    WLOGFI("Type: %{public}d, limitRect: %{public}d %{public}d %{public}u %{public}u",
        node->GetWindowType(), limitRect.posX_, limitRect.posY_, limitRect.width_, limitRect.height_);
}

void WindowLayoutPolicy::Reset()
{
}

float WindowLayoutPolicy::GetVirtualPixelRatio(DisplayId displayId) const
{
    if (displayInfosMap_.find(displayId) == std::end(displayInfosMap_)) {
        return 1.0; // 1.0 is default vpr
    }
    float virtualPixelRatio = displayInfosMap_[displayId]->GetVirtualPixelRatio();
    if (virtualPixelRatio == 0.0) {
        WLOGFE("GetVirtualPixel fail. vpr is 0.0. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }
    WLOGFI("GetVirtualPixel success. displayId:%{public}" PRIu64", vpr:%{public}f", displayId, virtualPixelRatio);
    return virtualPixelRatio;
}

bool WindowLayoutPolicy::IsFullScreenRecentWindowExist(const std::vector<sptr<WindowNode>>& nodeVec) const
{
    for (auto& node : nodeVec) {
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT &&
            node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
            return true;
        }
    }
    return false;
}

void WindowLayoutPolicy::UpdateSurfaceBounds(const sptr<WindowNode>& node, const Rect& winRect)
{
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
        WLOGFI("not need to update bounds");
        return;
    }
    if (node->leashWinSurfaceNode_) {
        node->leashWinSurfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        if (node->startingWinSurfaceNode_) {
            node->startingWinSurfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
        if (node->surfaceNode_) {
            node->surfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
    } else if (node->surfaceNode_) {
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    }
}

Rect WindowLayoutPolicy::GetFullDisplayRect() const
{
    return fullDisplayRect_;
}
}
}
