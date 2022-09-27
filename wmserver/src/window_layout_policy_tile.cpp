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

#include "window_layout_policy_tile.h"
#include <ability_manager_client.h>
#include <hitrace_meter.h>

#include "minimize_app.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicyTile"};
    constexpr uint32_t EDGE_INTERVAL = 48;
    constexpr uint32_t MID_INTERVAL = 24;
}

WindowLayoutPolicyTile::WindowLayoutPolicyTile(const sptr<DisplayGroupInfo>& displayGroupInfo,
    DisplayGroupWindowTree& displayGroupWindowTree)
    : WindowLayoutPolicy(displayGroupInfo, displayGroupWindowTree)
{
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        maxTileWinNumMap_.insert(std::make_pair(iter.first, static_cast<uint32_t>(1)));
    }
}

void WindowLayoutPolicyTile::Launch()
{
    // compute limit rect
    InitAllRects();
    // select app min win in queue, and minimize others
    InitForegroundNodeQueue();
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        AssignNodePropertyForTileWindows(displayId);
        LayoutForegroundNodeQueue(displayId);
        auto& displayWindowTree = displayGroupWindowTree_[displayId];
        LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::BELOW_WINDOW_NODE]));
    }
    WLOGFI("WindowLayoutPolicyTile::Launch");
}

void WindowLayoutPolicyTile::InitAllRects()
{
    displayGroupLimitRect_ = displayGroupRect_;
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        limitRectMap_[displayId] = iter.second;
        auto& displayWindowTree = displayGroupWindowTree_[displayId];
        LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]));
        InitTileWindowRects(displayId);
    }
}

void WindowLayoutPolicyTile::InitTileWindowRects(DisplayId displayId)
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    uint32_t edgeIntervalVp = static_cast<uint32_t>(EDGE_INTERVAL * virtualPixelRatio);
    uint32_t midIntervalVp = static_cast<uint32_t>(MID_INTERVAL * virtualPixelRatio);

    constexpr float ratio = DEFAULT_ASPECT_RATIO;
    const Rect& limitRect = limitRectMap_[displayId];
    const Rect& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    constexpr int half = 2;
    maxTileWinNumMap_[displayId]  = GetMaxTileWinNum(displayId);
    WLOGFI("set max tile window num %{public}u", maxTileWinNumMap_[displayId]);
    auto& presetRectsForAllLevel = presetRectsMap_[displayId];
    presetRectsForAllLevel.clear();
    uint32_t w = displayRect.width_ * ratio;
    uint32_t h = displayRect.height_ * ratio;
    w = w > limitRect.width_ ? limitRect.width_ : w;
    h = h > limitRect.height_ ? limitRect.height_ : h;
    int x = limitRect.posX_ + ((limitRect.width_ - w) / half);
    int y = limitRect.posY_ + ((limitRect.height_ - h) / half);

    std::vector<Rect> single = {{ x, y, w, h }};
    presetRectsForAllLevel.emplace_back(single);
    for (uint32_t num = 2; num <= maxTileWinNumMap_[displayId]; num++) { // start calc preset with 2 windows
        w = (limitRect.width_ - edgeIntervalVp * half - midIntervalVp * (num - 1)) / num;
        std::vector<Rect> curLevel;
        for (uint32_t i = 0; i < num; i++) {
            int curX = limitRect.posX_ + edgeIntervalVp + i * (w + midIntervalVp);
            Rect curRect = { curX, y, w, h };
            WLOGFI("presetRectsForAllLevel: level %{public}u, id %{public}u, tileRect: [%{public}d %{public}d "
                "%{public}u %{public}u]", num, i, curX, y, w, h);
            curLevel.emplace_back(curRect);
        }
        presetRectsForAllLevel.emplace_back(curLevel);
    }
}

uint32_t WindowLayoutPolicyTile::GetMaxTileWinNum(DisplayId displayId) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    constexpr uint32_t half = 2;
    uint32_t edgeIntervalVp = static_cast<uint32_t>(EDGE_INTERVAL * half * virtualPixelRatio);
    uint32_t midIntervalVp = static_cast<uint32_t>(MID_INTERVAL * virtualPixelRatio);
    uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t drawableW = limitRectMap_[displayId].width_ - edgeIntervalVp + midIntervalVp;
    return static_cast<uint32_t>(drawableW / (minFloatingW + midIntervalVp));
}

void WindowLayoutPolicyTile::InitForegroundNodeQueue()
{
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        foregroundNodesMap_[displayId].clear();
        const auto& appWindowNodes = *(displayGroupWindowTree_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
        for (auto& node : appWindowNodes) {
            if (WindowHelper::IsMainWindow(node->GetWindowType())) {
                ForegroundNodeQueuePushBack(node, displayId);
            }
        }
    }
}

bool WindowLayoutPolicyTile::IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node)
{
    if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
        return true;
    }
    const auto& displayId = node->GetDisplayId();
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    auto num = foregroundNodes.size();
    if (num > maxTileWinNumMap_[displayId]) {
        return false;
    }

    // find if node already exits in foreground nodes map
    if (IsWindowAlreadyInTileQueue(node)) {
        return true;
    }

    UpdateWindowSizeLimits(node);
    const auto& presetRectsForAllLevel = presetRectsMap_[displayId];
    Rect tileRect;
    // if size of foreground nodes is equal to or more than max tile window number
    if (num == maxTileWinNumMap_[displayId]) {
        tileRect = *(presetRectsForAllLevel[num - 1].begin());
    } else {  // if size of foreground nodes is less than max tile window number
        tileRect = *(presetRectsForAllLevel[num].begin());
    }
    WLOGFI("id %{public}u, tileRect: [%{public}d %{public}d %{public}u %{public}u]",
        node->GetWindowId(), tileRect.posX_, tileRect.posY_, tileRect.width_, tileRect.height_);
    return WindowHelper::IsRectSatisfiedWithSizeLimits(tileRect, node->GetWindowUpdatedSizeLimits());
}

void WindowLayoutPolicyTile::AddWindowNode(const sptr<WindowNode>& node)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        DisplayId displayId = node->GetDisplayId();
        ForegroundNodeQueuePushBack(node, displayId);
        AssignNodePropertyForTileWindows(displayId);
        LayoutForegroundNodeQueue(displayId);
    } else {
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicyTile::RemoveWindowNode(const sptr<WindowNode>& node)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    WLOGFI("RemoveWindowNode %{public}u in tile", node->GetWindowId());

    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        ForegroundNodeQueueRemove(node);
        AssignNodePropertyForTileWindows(node->GetDisplayId());
        LayoutForegroundNodeQueue(node->GetDisplayId());
    }
    // update client rect when hide window
    UpdateClientRect(node->GetRequestRect(), node, WindowSizeChangeReason::HIDE);
}

void WindowLayoutPolicyTile::LayoutForegroundNodeQueue(DisplayId displayId)
{
    for (auto& node : foregroundNodesMap_[displayId]) {
        LayoutWindowNode(node);
    }
}

bool WindowLayoutPolicyTile::IsWindowAlreadyInTileQueue(const sptr<WindowNode>& node)
{
    auto& foregroundNodes = foregroundNodesMap_[node->GetDisplayId()];
    auto iter = std::find_if(foregroundNodes.begin(), foregroundNodes.end(),
                             [node](sptr<WindowNode> foregroundNode) {
                                 return foregroundNode->GetWindowId() == node->GetWindowId();
                             });
    if (iter != foregroundNodes.end()) {
        WLOGFD("Window is already in tile queue, windowId: %{public}d", node->GetWindowId());
        return true;
    }
    return false;
}

void WindowLayoutPolicyTile::ForegroundNodeQueuePushBack(const sptr<WindowNode>& node, DisplayId displayId)
{
    if (node == nullptr) {
        return;
    }
    if (IsWindowAlreadyInTileQueue(node)) {
        return;
    }

    if (!WindowHelper::IsWindowModeSupported(node->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FLOATING)) {
        WLOGFD("Window don't support tile mode that should be minimized, winId: %{public}d", node->GetWindowId());
        MinimizeApp::AddNeedMinimizeApp(node, MinimizeReason::LAYOUT_TILE);
        return;
    }
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    while (foregroundNodes.size() >= maxTileWinNumMap_[displayId]) {
        auto removeNode = foregroundNodes.front();
        foregroundNodes.pop_front();
        WLOGFI("Minimize win in queue head for add new win, windowId: %{public}d", removeNode->GetWindowId());
        MinimizeApp::AddNeedMinimizeApp(removeNode, MinimizeReason::LAYOUT_TILE);
    }
    foregroundNodes.push_back(node);
    WLOGFI("Pusk back win in tile queue, displayId: %{public}" PRIu64", winId: %{public}d",
        displayId, node->GetWindowId());
}

void WindowLayoutPolicyTile::ForegroundNodeQueueRemove(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    DisplayId displayId = node->GetDisplayId();
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    auto iter = std::find(foregroundNodes.begin(), foregroundNodes.end(), node);
    if (iter != foregroundNodes.end()) {
        WLOGFI("Remove win in tile for win id: %{public}d", node->GetWindowId());
        foregroundNodes.erase(iter);
    }
}

bool WindowLayoutPolicyTile::IsValidTileQueueAndPresetRects(DisplayId displayId)
{
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    uint32_t num = foregroundNodes.size();
    auto& presetRectsForAllLevel = presetRectsMap_[displayId];
    if (num > maxTileWinNumMap_[displayId] || num > presetRectsForAllLevel.size() || num == 0) {
        WLOGE("Invalid tile queue, foreground tileNum: %{public}u, maxTileNum: %{public}u, presetRectsForAllLevel: "
            "%{public}u", num, maxTileWinNumMap_[displayId], presetRectsForAllLevel.size());
        return false;
    }
    auto& presetRect = presetRectsForAllLevel[num - 1];
    if (presetRect.size() != num) {
        WLOGE("Invalid preset rects, foreground tileNum: %{public}u, presetRect.size(): %{public}u", num, presetRect.size());
        return false;
    }
    return true;
}

void WindowLayoutPolicyTile::AssignNodePropertyForTileWindows(DisplayId displayId)
{
    if (!(IsValidTileQueueAndPresetRects(displayId))) {
        return;
    }

    auto& foregroundNodes = foregroundNodesMap_[displayId];
    uint32_t num = foregroundNodes.size();
    auto rectIt = presetRectsMap_[displayId][num - 1].begin();
    std::vector<sptr<WindowNode>> needMinimizeNodes;
    std::vector<sptr<WindowNode>> needRecoverNodes;
    for (auto node : foregroundNodes) {
        auto& rect = (*rectIt);
        if (WindowHelper::IsWindowModeSupported(node->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FLOATING) &&
            WindowHelper::IsRectSatisfiedWithSizeLimits(rect, node->GetWindowUpdatedSizeLimits())) {
            node->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            if (node->GetWindowToken()) {
                node->GetWindowToken()->UpdateWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            }
            node->SetRequestRect(rect);
            node->SetDecoStatus(true);
            rectIt++;
            WLOGFI("set rect for qwin id: %{public}d [%{public}d %{public}d %{public}d %{public}d]",
                node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        } else {
            WLOGFW("Window doesn't support floating mode or expect tileRect, winId: %{public}u", node->GetWindowId());
            // if foreground nodes is equal to max tileWinNum, means need recover one node before minimize cur node
            if (num == maxTileWinNumMap_[displayId]) {
                auto recoverNode = MinimizeApp::GetRecoverdNodeFromMinimizeList();
                if (recoverNode != nullptr) {
                    needRecoverNodes.push_back(recoverNode);
                }
            }
            needMinimizeNodes.push_back(node);
            MinimizeApp::AddNeedMinimizeApp(node, MinimizeReason::LAYOUT_TILE);
        }
    }
    for (auto& miniNode : needMinimizeNodes) {
        auto iter = std::find(foregroundNodes.begin(), foregroundNodes.end(), miniNode);
        if (iter != foregroundNodes.end()) {
            foregroundNodes.erase(iter);
        }
    }
    needMinimizeNodes.clear();

    for (auto& recNode : needRecoverNodes) {
        foregroundNodes.push_back(recNode);
    }
    needRecoverNodes.clear();
}

void WindowLayoutPolicyTile::UpdateLayoutRect(const sptr<WindowNode>& node)
{
    bool floatingWindow = (node->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);
    bool needAvoid = (node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    Rect lastRect = node->GetWindowRect();
    Rect winRect = node->GetRequestRect();
    WLOGFI("Id:%{public}u, mode: %{public}d type: %{public}d, needAvoid: %{public}d, requestRect: [%{public}d, "
        "%{public}d, %{public}u, %{public}u]", node->GetWindowId(), node->GetWindowMode(), node->GetWindowType(),
        needAvoid, winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);

     if (!floatingWindow) { // fullscreen window
        const auto& displayRect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
        const auto& limitDisplayRect = limitRectMap_[node->GetDisplayId()];
        winRect = needAvoid ? limitDisplayRect : displayRect;
    }
    node->SetWindowRect(winRect);
    CalcAndSetNodeHotZone(winRect, node);
    UpdateSurfaceBounds(node, winRect, lastRect);
    UpdateClientRectAndResetReason(node, winRect);
}
} // Rosen
} // OHOS
