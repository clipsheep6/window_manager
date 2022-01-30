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

#ifndef OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
#define OHOS_ROSEN_WINDOW_NODE_CONTAINER_H

#include <ui/rs_display_node.h>
#include "avoid_area_controller.h"
#include "window_layout_policy.h"
#include "window_manager.h"
#include "window_node.h"
#include "window_zorder_policy.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
class WindowNodeContainer : public RefBase {
public:
    WindowNodeContainer(uint64_t screenId, uint32_t width, uint32_t height);
    ~WindowNodeContainer();
    WMError AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    WMError RemoveWindowNode(sptr<WindowNode>& node);
    WMError UpdateWindowNode(sptr<WindowNode>& node);
    WMError DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds);
    const std::vector<uint32_t>& Destroy();
    void AssignZOrder();
    WMError SetFocusWindow(uint32_t windowId);
    uint32_t GetFocusWindow() const;
    std::vector<Rect> GetAvoidAreaByType(AvoidAreaType avoidAreaType);
    WMError MinimizeOtherFullScreenAbility(); // adapt to api7
    void TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const;
    uint64_t GetScreenId() const;
    Rect GetDisplayRect() const;
    sptr<WindowNode> GetTopImmersiveNode() const;
    WMError HandleSplitWindowModeChange(sptr<WindowNode>& triggerNode, bool isChangeToSplit);

    void OnAvoidAreaChange(const std::vector<Rect>& avoidAreas);
    std::shared_ptr<RSDisplayNode> GetDisplayNode() const;
    void LayoutDividerWindow(sptr<WindowNode>& node);
    bool isVerticalDisplay() const;
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    sptr<WindowNode> GetNextFocusableWindow(uint32_t windowId) const;
    void MinimizeAllAppWindows();
    void NotifyWindowStateChange(WindowState state, WindowStateChangeReason reason);

private:
    void AssignZOrder(sptr<WindowNode>& node);
    void TraverseWindowNode(sptr<WindowNode>& root, std::vector<sptr<WindowNode>>& windowNodes) const;
    sptr<WindowNode> FindRoot(WindowType type) const;
    sptr<WindowNode> FindWindowNodeById(uint32_t id) const;
    void UpdateFocusStatus(uint32_t id, bool focused) const;
    void UpdateWindowTree(sptr<WindowNode>& node);
    bool UpdateRSTree(sptr<WindowNode>& node, bool isAdd);

    void SendSplitScreenEvent(WindowMode mode);
    sptr<WindowNode> FindSplitPairNode(sptr<WindowNode>& node) const;
    WMError HandleModeChangeToSplit(sptr<WindowNode>& triggerNode);
    WMError HandleModeChangeFromSplit(sptr<WindowNode>& triggerNode);
    WMError UpdateWindowPairInfo(sptr<WindowNode>& triggerNode, sptr<WindowNode>& pairNode);
    WMError SwitchLayoutPolicy(WindowLayoutMode mode);
    void NotifyIfSystemBarTintChanged();
    void NotifyIfSystemBarRegionChanged();
    void TraverseAndUpdateWindowState(WindowState state, int32_t topPriority);
    void UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state);
    bool IsTopAppWindow(uint32_t windowId) const;
    void RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes);

    sptr<AvoidAreaController> avoidController_;
    sptr<WindowZorderPolicy> zorderPolicy_ = new WindowZorderPolicy();
    sptr<WindowNode> belowAppWindowNode_ = new WindowNode();
    sptr<WindowNode> appWindowNode_ = new WindowNode();
    sptr<WindowNode> aboveAppWindowNode_ = new WindowNode();
    std::shared_ptr<RSDisplayNode> displayNode_;
    std::vector<uint32_t> removedIds_;
    uint64_t screenId_ = 0;
    Rect displayRect_;
    std::unordered_map<WindowType, sptr<WindowNode>> sysBarNodeMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     nullptr },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, nullptr },
    };
    std::unordered_map<WindowType, SystemBarRegionTint> sysBarTintMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarRegionTint() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarRegionTint() },
    };
    std::unordered_map<WindowLayoutMode, sptr<WindowLayoutPolicy>> layoutPolicys_;
    WindowLayoutMode layoutMode_ = WindowLayoutMode::CASCADE;
    sptr<WindowLayoutPolicy> layoutPolicy_;
    uint32_t zOrder_ { 0 };
    uint32_t focusedWindow_ { INVALID_WINDOW_ID };
    void DumpScreenWindowTree();

    struct WindowPairInfo {
        sptr<WindowNode> pairNode;
        float splitRatio;
    };
    std::unordered_map<uint32_t, WindowPairInfo> pairedWindowMap_;
    void RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const;
    const int32_t WINDOW_TYPE_RAISED_INPUT_METHOD = 115;
};
}
}
#endif // OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
