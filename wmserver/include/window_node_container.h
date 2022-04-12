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
using WindowNodeOperationFunc = std::function<bool(sptr<WindowNode>)>; // return true indicates to stop traverse
using SysBarNodeMap = std::unordered_map<WindowType, sptr<WindowNode>>;
using SysBarTintMap = std::unordered_map<WindowType, SystemBarRegionTint>;
class WindowNodeContainer : public RefBase {
public:
    WindowNodeContainer(DisplayId displayId, uint32_t width, uint32_t height);
    ~WindowNodeContainer();
    WMError AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    WMError RemoveWindowNode(sptr<WindowNode>& node);
    WMError UpdateWindowNode(sptr<WindowNode>& node, WindowUpdateReason reason);
    WMError DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds);
    const std::vector<uint32_t>& Destroy();
    void AssignZOrder();
    WMError SetFocusWindow(uint32_t windowId);
    uint32_t GetFocusWindow() const;
    WMError SetActiveWindow(uint32_t windowId, bool byRemoved);
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    uint32_t GetActiveWindow() const;
    std::vector<Rect> GetAvoidAreaByType(AvoidAreaType avoidAreaType, DisplayId displayId);
    WMError MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node);
    void TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const;
    uint64_t GetScreenId(DisplayId displayId) const;
    Rect GetDisplayRect(DisplayId displayId) const;
    Rect GetDisplayLimitRect(DisplayId displayId) const;
    std::unordered_map<WindowType, SystemBarProperty> GetExpectImmersiveProperty() const;
    void NotifyAccessibilityWindowInfo(const sptr<WindowNode>& windowId, WindowUpdateType type) const;
    // void UpdateDisplayRect(uint32_t width, uint32_t height, DisplayId displayId);

    void OnAvoidAreaChange(const std::vector<Rect>& avoidAreas, DisplayId displayId);
    bool isVerticalDisplay(DisplayId displayId) const;
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    sptr<WindowNode> GetNextFocusableWindow(uint32_t windowId) const;
    sptr<WindowNode> GetNextActiveWindow(uint32_t windowId) const;
    void MinimizeAllAppWindows(DisplayId displayId);
    void ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason);
    void NotifySystemBarTints(std::vector<DisplayId> displayIdVec);
    void NotifySystemBarDismiss(sptr<WindowNode>& node);
    WMError MinimizeAppNodeExceptOptions(bool fromUser, const std::vector<uint32_t> &exceptionalIds = {},
                                         const std::vector<WindowMode> &exceptionalModes = {});
    WMError EnterSplitWindowMode(sptr<WindowNode>& node);
    WMError ExitSplitWindowMode(sptr<WindowNode>& node);
    WMError SwitchLayoutPolicy(WindowLayoutMode mode, DisplayId displayId, bool reorder = false);
    void RaiseSplitRelatedWindowToTop(sptr<WindowNode>& node);
    void MoveWindowNode(sptr<WindowNodeContainer>& container);
    float GetVirtualPixelRatio(DisplayId displayId) const;
    void TraverseWindowTree(const WindowNodeOperationFunc& func, bool isFromTopToBottom = true) const;
    void UpdateSizeChangeReason(sptr<WindowNode>& node, WindowSizeChangeReason reason);
    void GetWindowList(std::vector<sptr<WindowInfo>>& windowList) const;
    void ProcessDisplayCreate(DisplayId displayId);

private:
    void TraverseWindowNode(sptr<WindowNode>& root, std::vector<sptr<WindowNode>>& windowNodes) const;
    sptr<WindowNode> FindRootAndGetRootType(WindowType type, WindowRootNodeType& rootType) const;
    RootNodeVectorPtr FindRootOfDisplay(DisplayId displayId, WindowRootNodeType type);
    sptr<WindowNode> FindWindowNodeById(uint32_t id) const;
    void UpdateFocusStatus(uint32_t id, bool focused) const;
    void UpdateActiveStatus(uint32_t id, bool isActive) const;
    void UpdateBrightness(uint32_t id, bool byRemoved);
    void UpdateWindowTree(sptr<WindowNode>& node);
    bool UpdateRSTree(sptr<WindowNode>& node, bool isAdd);

    void SendSplitScreenEvent(sptr<WindowNode>& node);
    sptr<WindowNode> FindSplitPairNode(sptr<WindowNode>& node) const;
    WMError UpdateWindowPairInfo(sptr<WindowNode>& triggerNode, sptr<WindowNode>& pairNode);
    void NotifyIfSystemBarTintChanged(DisplayId displayId);
    void NotifyIfSystemBarRegionChanged(DisplayId displayId);
    void TraverseAndUpdateWindowState(WindowState state, int32_t topPriority);
    void UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state);
    bool IsTopAppWindow(uint32_t windowId) const;
    sptr<WindowNode> FindDividerNode() const;
    void RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes);
    void MinimizeWindowFromAbility(const sptr<WindowNode>& node, bool fromUser);
    void ResetLayoutPolicy();
    bool IsAboveSystemBarNode(sptr<WindowNode> node) const;
    bool IsFullImmersiveNode(sptr<WindowNode> node) const;
    bool IsSplitImmersiveNode(sptr<WindowNode> node) const;
    bool TraverseFromTopToBottom(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const;
    bool TraverseFromBottomToTop(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const;

    // cannot determine in case of a window covered by union of several windows or with transparent value
    void UpdateWindowVisibilityInfos(std::vector<sptr<WindowVisibilityInfo>>& infos);
    void RaiseOrderedWindowToTop(std::vector<uint32_t> orderedIds, std::vector<sptr<WindowNode>>& windowNodes);
    static bool ReadIsWindowAnimationEnabledProperty();
    void DumpScreenWindowTree();
    void RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const;
    void RaiseShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node) const;


    void InitSysBarMapForDisplay(DisplayId displayId);
    void InitWindowNodeMapForDisplay(DisplayId displayId);
    WMError AddWindowNodeOnTree(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);

    const int32_t WINDOW_TYPE_RAISED_INPUT_METHOD = 115;
    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    uint32_t zOrder_ { 0 };
    uint32_t focusedWindow_ { INVALID_WINDOW_ID };
    uint32_t activeWindow_ = INVALID_WINDOW_ID;

    sptr<AvoidAreaController> avoidController_;
    sptr<WindowZorderPolicy> zorderPolicy_ = new WindowZorderPolicy();
    std::unordered_map<WindowLayoutMode, sptr<WindowLayoutPolicy>> layoutPolicys_;
    WindowLayoutMode layoutMode_ = WindowLayoutMode::CASCADE;
    sptr<WindowLayoutPolicy> layoutPolicy_;

    std::vector<Rect> currentCoveredArea_;
    std::map<DisplayId, SysBarNodeMap> sysBarNodeMaps_;
    std::map<DisplayId, SysBarTintMap> sysBarTintMaps_;

    struct WindowPairInfo {
        sptr<WindowNode> pairNode_;
        float splitRatio_;
    };
    std::unordered_map<uint32_t, WindowPairInfo> pairedWindowMap_;
    std::vector<uint32_t> removedIds_;
    sptr<WindowNode> belowAppWindowNode_ = new WindowNode();
    sptr<WindowNode> appWindowNode_ = new WindowNode();
    sptr<WindowNode> aboveAppWindowNode_ = new WindowNode();
    std::map<DisplayId, Rect> displayRectMap_;
    std::unique_ptr<WindowNodeMaps> windowNodeMaps_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
