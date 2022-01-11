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
#include "window_layout_policy.h"
#include "window_node.h"
#include "window_zorder_policy.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
using UpdateFocusStatusFunc = std::function<void (uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, int32_t displayId, bool focused)>;
using UpdateSystemBarPropsFunc = std::function<void (uint32_t displayId, const SystemBarProps& props)>;

class WindowNodeContainer : public RefBase {
public:
    WindowNodeContainer(uint64_t screenId, uint32_t width, uint32_t height,
        UpdateFocusStatusFunc focusCallback, UpdateSystemBarPropsFunc sysBarCallback);
    ~WindowNodeContainer();
    WMError AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    WMError RemoveWindowNode(sptr<WindowNode>& node);
    WMError UpdateWindowNode(sptr<WindowNode>& node);
    WMError DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds);
    const std::vector<uint32_t>& Destroy();
    void AssignZOrder();
    WMError SetFocusWindow(uint32_t windowId);
    uint32_t GetFocusWindow() const;
    WMError MinimizeOtherFullScreenAbility(); // adapt to api7
    void TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes);
    uint64_t GetScreenId() const;
    Rect GetDisplayRect() const;
    sptr<WindowNode> GetTopImmersiveNode() const;
    void NotifySystemBarIfChanged();

private:
    void AssignZOrder(sptr<WindowNode>& node);
    void TraverseWindowNode(sptr<WindowNode>& root, std::vector<sptr<WindowNode>>& windowNodes);
    sptr<WindowNode> FindRoot(WindowType type) const;
    void UpdateFocusWindow();
    sptr<WindowNode> FindWindowNodeById(uint32_t id) const;
    void UpdateFocusStatus(uint32_t id, bool focused) const;
    void UpdateWindowTree(sptr<WindowNode>& node);
    bool UpdateRSTree(sptr<WindowNode>& node, bool isAdd);
    sptr<WindowZorderPolicy> zorderPolicy_ = new WindowZorderPolicy();
    sptr<WindowNode> belowAppWindowNode_ = new WindowNode();
    sptr<WindowNode> appWindowNode_ = new WindowNode();
    sptr<WindowNode> aboveAppWindowNode_ = new WindowNode();
    sptr<WindowLayoutPolicy> layoutPolicy_ =
        new WindowLayoutPolicy(belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    std::shared_ptr<RSDisplayNode> displayNode_;
    std::vector<uint32_t> removedIds_;
    std::unordered_map<WindowType, sptr<WindowNode>> sysBarNodeMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     nullptr },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, nullptr },
    };
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };
    uint32_t zOrder_ { 0 };
    uint32_t focusedWindow_ { 0 };
    Rect displayRect_;
    uint64_t screenId_ = 0;
    UpdateFocusStatusFunc focusStatusCallBack_;
    UpdateSystemBarPropsFunc systemBarChangedCallBack_;
    void DumpScreenWindowTree();
};
}
}
#endif // OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
