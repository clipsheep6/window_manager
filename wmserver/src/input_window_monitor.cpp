/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "input_window_monitor.h"

#include <vector>

#include <ipc_skeleton.h>

#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "InputWindowMonitor"};
}

void InputWindowMonitor::UpdateInputWindow(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        WLOGFE("windowRoot is null.");
        return;
    }
    sptr<WindowNode> windowNode = windowRoot_->GetWindowNode(windowId);
    if (windowNode == nullptr) {
        WLOGFE("window node could not be found.");
        return;
    }
    if (windowTypeSkipped_.find(windowNode->GetWindowProperty()->GetWindowType()) != windowTypeSkipped_.end()) {
        return;
    }
    int32_t displayId = windowNode->GetDisplayId();
    UpdateInputWindowByDisplayId(displayId);
}

void InputWindowMonitor::UpdateInputWindowByDisplayId(int32_t displayId)
{
    if (displayId == INVALID_DISPLAY_ID) {
        return;
    }
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can not get window node container.");
        return;
    }
    UpdateDisplaysInfo(container);
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    auto iter = std::find_if(logicalDisplays_.begin(), logicalDisplays_.end(),
                             [displayId](MMI::LogicalDisplayInfo& logicalDisplay) {
        return logicalDisplay.id == displayId;
    });
    if (iter != logicalDisplays_.end()) {
        TraverseWindowNodes(windowNodes, iter);
        if (!iter->windowsInfo_.empty()) {
            iter->focusWindowId = container->GetFocusWindow();
        }
    } else {
        WLOGFE("There is no display for this window action.");
        return;
    }
    WLOGFI("update display info to IMS.");
    MMI::InputManager::GetInstance()->UpdateDisplayInfo(physicalDisplays_, logicalDisplays_);
}

void InputWindowMonitor::UpdateDisplaysInfo(const sptr<WindowNodeContainer>& container)
{
    MMI::PhysicalDisplayInfo physicalDisplayInfo = {
        .id = static_cast<int32_t>(container->GetScreenId()),
        .leftDisplayId = INVALID_DISPLAY_ID,
        .upDisplayId = INVALID_DISPLAY_ID,
        .topLeftX = container->GetDisplayRect().posX_,
        .topLeftY = container->GetDisplayRect().posY_,
        .width = static_cast<int32_t>(container->GetDisplayRect().width_),
        .height = static_cast<int32_t>(container->GetDisplayRect().height_),
        .name = "physical_display0",
        .seatId = "seat0",
        .seatName = "default0",
        .logicWidth = static_cast<int32_t>(container->GetDisplayRect().width_),
        .logicHeight = static_cast<int32_t>(container->GetDisplayRect().height_),
        .direction = MMI::Direction0
    };
    auto physicalDisplayIter = std::find_if(physicalDisplays_.begin(), physicalDisplays_.end(),
                                            [&physicalDisplayInfo](MMI::PhysicalDisplayInfo& physicalDisplay) {
        return physicalDisplay.id == physicalDisplayInfo.id;
    });
    if (physicalDisplayIter != physicalDisplays_.end()) {
        *physicalDisplayIter = physicalDisplayInfo;
    } else {
        physicalDisplays_.emplace_back(physicalDisplayInfo);
    }

    MMI::LogicalDisplayInfo logicalDisplayInfo = {
        .id = static_cast<int32_t>(container->GetScreenId()),
        .topLeftX = container->GetDisplayRect().posX_,
        .topLeftY = container->GetDisplayRect().posY_,
        .width = static_cast<int32_t>(container->GetDisplayRect().width_),
        .height = static_cast<int32_t>(container->GetDisplayRect().height_),
        .name = "logical_display0",
        .seatId = "seat0",
        .seatName = "default0",
        .focusWindowId = INVALID_WINDOW_ID,
        .windowsInfo_ = {},
    };
    auto logicalDisplayIter = std::find_if(logicalDisplays_.begin(), logicalDisplays_.end(),
                                           [&logicalDisplayInfo](MMI::LogicalDisplayInfo& logicalDisplay) {
        return logicalDisplay.id == logicalDisplayInfo.id;
    });
    if (logicalDisplayIter != logicalDisplays_.end()) {
        *logicalDisplayIter = logicalDisplayInfo;
    } else {
        logicalDisplays_.emplace_back(logicalDisplayInfo);
    }
}

void InputWindowMonitor::TraverseWindowNodes(const std::vector<sptr<WindowNode>> &windowNodes,
                                             std::vector<MMI::LogicalDisplayInfo>::iterator& iter)
{
    iter->windowsInfo_.clear();
    for (auto& windowNode: windowNodes) {
        if (windowTypeSkipped_.find(windowNode->GetWindowProperty()->GetWindowType()) != windowTypeSkipped_.end()) {
            WLOGFI("window has been skipped. [id: %{public}d, type: %{public}d]", windowNode->GetWindowId(),
                   windowNode->GetWindowProperty()->GetWindowType());
            continue;
        }
        MMI::WindowInfo windowInfo = {
            .id = static_cast<int32_t>(windowNode->GetWindowId()),
            .pid = windowNode->GetCallingPid(),
            .uid = windowNode->GetCallingUid(),
            .topLeftX = windowNode->GetLayoutRect().posX_,
            .topLeftY = windowNode->GetLayoutRect().posY_,
            .width = static_cast<int32_t>(windowNode->GetLayoutRect().width_),
            .height = static_cast<int32_t>(windowNode->GetLayoutRect().height_),
            .displayId = windowNode->GetDisplayId(),
            .agentWindowId = static_cast<int32_t>(windowNode->GetWindowId()),
        };
        if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            const int32_t divTouchRegion = 20;
            if (windowInfo.width < windowInfo.height) {
                windowInfo.topLeftX -= divTouchRegion;
                windowInfo.width += (divTouchRegion + divTouchRegion);
            } else {
                windowInfo.topLeftY -= divTouchRegion;
                windowInfo.height += (divTouchRegion + divTouchRegion);
            }
        }
        iter->windowsInfo_.emplace_back(windowInfo);
    }
}
}
}