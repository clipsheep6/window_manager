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
#ifndef OHOS_ROSEN_WINDOW_ROOT_H
#define OHOS_ROSEN_WINDOW_ROOT_H

#include <refbase.h>
#include <iremote_object.h>

#include "agent_death_recipient.h"
#include "display_manager_service_inner.h"
#include "window_node_container.h"
#include "zidl/window_manager_agent_interface.h"

namespace OHOS {
namespace Rosen {
enum class Event : uint32_t {
    REMOTE_DIED,
};

class WindowRoot : public RefBase {
using Callback = std::function<void (Event event, uint32_t windowId)>;

public:
    WindowRoot(std::recursive_mutex& mutex, Callback callback) : mutex_(mutex), callback_(callback) {}
    ~WindowRoot() = default;

    sptr<WindowNodeContainer> GetOrCreateWindowNodeContainer(DisplayId displayId);
    void NotifyDisplayRemoved(DisplayId displayId);
    sptr<WindowNode> GetWindowNode(uint32_t windowId) const;

    WMError SaveWindow(const sptr<WindowNode>& node);
    WMError AddWindowNode(uint32_t parentId, sptr<WindowNode>& node);
    WMError RemoveWindowNode(uint32_t windowId);
    WMError DestroyWindow(uint32_t windowId);
    WMError UpdateWindowNode(uint32_t windowId);
    bool isVerticalDisplay(sptr<WindowNode>& node) const;

    WMError RequestFocus(uint32_t windowId);
    WMError MinimizeOtherFullScreenAbility(sptr<WindowNode>& node);
    std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType);
    WMError MinimizeAllAppNodeAbility(sptr<WindowNode>& node);
    WMError HandleSplitWindowModeChange(sptr<WindowNode>& node, bool isChangeToSplit);
    std::shared_ptr<RSSurfaceNode> GetSurfaceNodeByAbilityToken(const sptr<IRemoteObject>& abilityToken) const;

    void NotifyWindowStateChange(WindowState state, WindowStateChangeReason reason);
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node);

private:
    void OnRemoteDied(const sptr<IRemoteObject>& remoteObject);
    WMError DestroyWindowInner(sptr<WindowNode>& node);
    bool CheckDisplayInfo(const sptr<AbstractDisplay>& display);

    std::recursive_mutex& mutex_;
    std::map<int32_t, sptr<WindowNodeContainer>> windowNodeContainerMap_;
    std::map<uint32_t, sptr<WindowNode>> windowNodeMap_;
    std::map<sptr<IRemoteObject>, uint32_t> windowIdMap_;

    std::map<WindowManagerAgentType, std::vector<sptr<IWindowManagerAgent>>> windowManagerAgents_;

    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(std::bind(&WindowRoot::OnRemoteDied,
        this, std::placeholders::_1));
    Callback callback_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_ROOT_H
