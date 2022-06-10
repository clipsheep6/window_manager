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

#include "avoid_area_controller.h"
#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AvoidAreaController"};
}

void AvoidAreaController::UpdateAvoidAreaListener(sptr<WindowNode>& windowNode, bool haveAvoidAreaListener)
{
    if (haveAvoidAreaListener) {
        haveListenerNodes_.insert(windowNode);
    } else {
        haveListenerNodes_.erase(windowNode);
    }
}

void AvoidAreaController::ProcessWindowChange(const sptr<WindowNode>& windowNode, AvoidControlType avoidType)
{
    switch (avoidType) {
        case AvoidControlType::AVOID_NODE_ADD:
        case AvoidControlType::AVOID_NODE_REMOVE:
            AddOrRemoveOverlayWindowIfNeed(windowNode, avoidType == AvoidControlType::AVOID_NODE_ADD);
            break;
        case AvoidControlType::AVOID_NODE_UPDATE:
            UpdateOverlayWindowIfNeed(windowNode);
            break;
        default:
            break;
    }
}

void AvoidAreaController::AddOrRemoveOverlayWindowIfNeed(const sptr<WindowNode>& overlayNode, bool isAdding)
{
    if (!WindowHelper::IsOverlayWindow(overlayNode->GetWindowType())) {
        return;
    }
    WM_FUNCTION_TRACE();

    uint32_t overlayId = overlayNode->GetWindowId();
    bool isRecorded = (overlayWindowMap_.find(overlayId) != overlayWindowMap_.end());
    if (isAdding == isRecorded) {
        WLOGE("error occured in overlay. overlayId %{public}u isAdding %{public}d record flag %{public}d",
            overlayId, isAdding, isRecorded);
        return;
    }
    if (isAdding) {
        overlayWindowMap_.insert(std::make_pair(overlayId, overlayNode));
    } else {
        overlayWindowMap_.erase(overlayId);
    }

    if (overlayNode->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        // 软键盘只考虑对呼出窗口的遮挡
        return AddOrRemoveKeyboard(overlayNode, isAdding);
    }

    // TODO 判断是否有重叠
    for (auto& node : haveListenerNodes_) {
        sptr<AvoidArea> avoidArea = CalcOverlayRect(node, overlayNode, !isAdding);
        if (avoidArea != nullptr) {
            WLOGD("notify client. windowId %{public}u", node->GetWindowId()); //TODO. debug
            node->GetWindowToken()->UpdateAvoidArea(avoidArea);
        }
    }
}

void AvoidAreaController::AddOrRemoveKeyboard(const sptr<WindowNode>& keyboardNode, bool isAdding)
{
    // if ((keyboardNode->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT)
    //     || ((avoidType != AvoidControlType::AVOID_NODE_ADD) && (avoidType != AvoidControlType::AVOID_NODE_REMOVE)))
    // {
    //     WLOGFD("windowType: %{public}u, avoidType: %{public}u", keyboardNode->GetWindowType(), avoidType);
    //     return false;
    // }

    const uint32_t callingWindowId = keyboardNode->GetCallingWindow();
    auto iter = windowNodeMap_.find(callingWindowId);
    if (iter == windowNodeMap_.end()) {
        WLOGFI("callingWindow: %{public}u does not be set", keyboardNode->GetWindowId());
        iter = windowNodeMap_.find(focusedWindow_);
    }
    if (iter == windowNodeMap_.end() || iter->second == nullptr || (iter->second)->GetWindowToken() == nullptr) {
        WLOGFE("does not have correct callingWindow for input method window");
        return;
    }
    sptr<WindowNode> callingWindow = iter->second; //TODO 
    if (haveListenerNodes_.find(callingWindow) != haveListenerNodes_.end()) {
        WLOGFI("callingWindow: %{public}u have no avoidArea listener", callingWindowId);
        return;
    }

    const WindowMode callingWindowMode = callingWindow->GetWindowMode();
    if (callingWindowMode == WindowMode::WINDOW_MODE_FULLSCREEN ||
        callingWindowMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        callingWindowMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
        callingWindowMode == WindowMode::WINDOW_MODE_FLOATING) {
        const Rect keyRect = keyboardNode->GetWindowRect();
        const Rect callingRect = callingWindow->GetWindowRect();
        if (!WindowHelper::HasOverlap(callingRect, keyRect)) {
            WLOGFD("no overlap between two windows");
            return;
        }
        const Rect overlayRect = (isAdding ?
            WindowHelper::GetOverlap(keyRect, callingRect, callingRect.posX_, callingRect.posY_) : Rect{ 0, 0, 0, 0 });

        WLOGFI("keyboard size change callingWindow: [%{public}s, %{public}u], " \
            "overlap rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
            callingWindow->GetWindowName().c_str(), callingWindow->GetWindowId(),
            overlayRect.posX_, overlayRect.posY_, overlayRect.width_, overlayRect.height_);
        sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT, overlayRect);
        callingWindow->GetWindowToken()->UpdateOccupiedAreaChangeInfo(info);
        return;
    }
    WLOGFE("does not have correct callingWindowMode for input method window");
}

void AvoidAreaController::UpdateOverlayWindowIfNeed(const sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    if (WindowHelper::IsOverlayWindow(node->GetWindowType())) {
        for (auto& appNode : haveListenerNodes_) {
            sptr<AvoidArea> avoidArea = CalcOverlayRect(appNode, node, false);
            if (avoidArea != nullptr) {
                WLOGD("notify client. windowId %{public}u", node->GetWindowId()); //TODO. debug
                node->GetWindowToken()->UpdateAvoidArea(avoidArea);
            }
        }
    } else {
        for (auto& iter : overlayWindowMap_) {
            if (iter.second == nullptr) {
                WLOGE("invalid overlayw window");
                continue;
            }
            sptr<AvoidArea> avoidArea = CalcOverlayRect(node, iter.second, false);
            if (avoidArea != nullptr) {
                WLOGD("notify client. windowId %{public}u", node->GetWindowId()); //TODO. debug
                node->GetWindowToken()->UpdateAvoidArea(avoidArea);
            }
        }
    }
}

sptr<AvoidArea> AvoidAreaController::CalcOverlayRect(const sptr<WindowNode>& node,
    const sptr<WindowNode>& overlayNode, bool isRemoving) const
{
    if (node->GetWindowId() == overlayNode->GetWindowId()) {
        WLOGE("overlay not support self. windowId %{public}u", node->GetWindowId());
        return nullptr;
    }

    // const WindowMode windowMode = node->GetWindowMode();
    // if (windowMode != WindowMode::WINDOW_MODE_FULLSCREEN &&
    //     windowMode != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
    //     windowMode != WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
    //     return nullptr;
    // }

    const Rect rect = node->GetWindowRect();
    const Rect overlayRect = WindowHelper::GetOverlap(overlayNode->GetWindowRect(), rect, rect.posX_, rect.posY_); // TODO. 考虑z-order
    WLOGFD("rect: [%{public}d, %{public}d, %{public}u, %{public}u]", 
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    WLOGFD("Rect2: [%{public}d, %{public}d, %{public}u, %{public}u]", 
        overlayNode->GetWindowRect().posX_, overlayNode->GetWindowRect().posY_, overlayNode->GetWindowRect().width_, overlayNode->GetWindowRect().height_);
    WLOGFD("overlayRect: [%{public}d, %{public}d, %{public}u, %{public}u]", 
        overlayRect.posX_, overlayRect.posY_, overlayRect.width_, overlayRect.height_);
    return (isRemoving ? BuildAvoidArea(0, 0, {0, 0, 0, 0}) : BuildAvoidArea(rect.width_, rect.height_, overlayRect));
}

sptr<AvoidArea> AvoidAreaController::BuildAvoidArea(const uint32_t width, const uint32_t height,
    const Rect& overlayRect) const
{
    sptr<AvoidArea> avoidArea = new(std::nothrow) AvoidArea();
    // Calculate which side the overlayRect in rect
    if (overlayRect.width_ >= overlayRect.height_) {
        if (overlayRect.posY_ * 2 <= static_cast<int32_t>(height)) {
            avoidArea->topRect_ = overlayRect;
        } else {
            avoidArea->bottomRect_ = overlayRect;
        }
    } else {
        if (overlayRect.posX_ * 2 <= static_cast<int32_t>(width)) {
            avoidArea->leftRect_ = overlayRect;
        } else {
            avoidArea->rightRect_ = overlayRect;
        }
    }
    // TODO debug info
    WLOGFD("avoidArea: top[%{public}d, %{public}d, %{public}u, %{public}u], left[%{public}d, %{public}d, %{public}u, %{public}u], " \
        "right[%{public}d, %{public}d, %{public}u, %{public}u], bottom[%{public}d, %{public}d, %{public}u, %{public}u]",
        avoidArea->topRect_.posX_, avoidArea->topRect_.posY_, avoidArea->topRect_.width_, avoidArea->topRect_.height_,
        avoidArea->leftRect_.posX_, avoidArea->leftRect_.posY_, avoidArea->leftRect_.width_, avoidArea->leftRect_.height_,
        avoidArea->rightRect_.posX_, avoidArea->rightRect_.posY_, avoidArea->rightRect_.width_, avoidArea->rightRect_.height_,
        avoidArea->bottomRect_.posX_, avoidArea->bottomRect_.posY_, avoidArea->bottomRect_.width_, avoidArea->bottomRect_.height_);
    return avoidArea;
}

AvoidArea AvoidAreaController::GetAvoidAreaByType(const sptr<WindowNode>& node, AvoidAreaType avoidAreaType) const
{
    
    if (avoidAreaType == AvoidAreaType::TYPE_SYSTEM) {
        AvoidArea systemAvoidArea;
        sptr<AvoidArea> statusBarAvoidArea;
        sptr<AvoidArea> navigationBarAvoidArea;
        for (auto& iter : overlayWindowMap_) {
            if (iter.second != nullptr) {
                if (iter.second->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR) {
                    sptr<WindowNode> statusBarNode = iter.second;
                    statusBarAvoidArea = CalcOverlayRect(node, statusBarNode, false);
                    if (statusBarAvoidArea != nullptr) {
                        systemAvoidArea.topRect_ = statusBarAvoidArea->topRect_;
                    }
                }
                if (iter.second->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
                    sptr<WindowNode> navigationBarNode = iter.second;
                    navigationBarAvoidArea = CalcOverlayRect(node, navigationBarNode, false);
                    if (navigationBarAvoidArea != nullptr) {
                        systemAvoidArea.bottomRect_ = navigationBarAvoidArea->bottomRect_;
                    }
                }
            }
        }
        return systemAvoidArea;
    } else if (avoidAreaType == AvoidAreaType::TYPE_KEYBOARD) {
        for (auto& iter : overlayWindowMap_) {
            if (iter.second != nullptr && iter.second->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
                sptr<WindowNode> avoidAreaNode = iter.second; 
                return *CalcOverlayRect(node, avoidAreaNode, false);
            }
        }
        return *(BuildAvoidArea(0, 0, {0, 0, 0, 0}));
    } else {
        WLOGFI("cannot find avoidAreaType: %{public}u", avoidAreaType);
        return *(BuildAvoidArea(0, 0, {0, 0, 0, 0}));
    }
}
}
}
