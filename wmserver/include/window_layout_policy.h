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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H

#include <map>
#include <refbase.h>
#include <set>

#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
struct LayoutDependRects {
    Rect fullRect_ = {0, 0, 0, 0};
    Rect priRect_  = {0, 0, 0, 0};
    Rect secRect_  = {0, 0, 0, 0};
    Rect limitFullRect_ = {0, 0, 0, 0};
    Rect limitPriRect_  = {0, 0, 0, 0};
    Rect limitSecRect_  = {0, 0, 0, 0};
};
class WindowLayoutPolicy : public RefBase {
public:
    WindowLayoutPolicy() = default;
    WindowLayoutPolicy(const sptr<WindowNode>& belowAppNode,
        const sptr<WindowNode>& appNode, const sptr<WindowNode>& aboveAppNode);
    ~WindowLayoutPolicy() = default;
    void UpdateDisplayInfo(const Rect& primaryRect, const Rect& secondaryRect, const Rect& displayRect);
    void AddWindowNode(sptr<WindowNode>& node);
    void RemoveWindowNode(sptr<WindowNode>& node);
    void UpdateWindowNode(sptr<WindowNode>& node);
    void UpdateLayoutRect(sptr<WindowNode>& node);

private:
    LayoutDependRects dependRects;
    sptr<WindowNode> belowAppWindowNode_ = new WindowNode();
    sptr<WindowNode> appWindowNode_ = new WindowNode();
    sptr<WindowNode> aboveAppWindowNode_ = new WindowNode();
    std::map<uint32_t, sptr<WindowNode>> avoidNodes_;
    const std::set<WindowType> avoidTypes_ {
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
    };
    void UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect);
    void RecordAvoidRect(const sptr<WindowNode>& node);
    void LayoutWindowTree();
    void LayoutWindowNode(sptr<WindowNode>& node);
    void UpdateDependLimitRects(const sptr<WindowNode>& node);
    void ResetDependLimitRects();
    Rect& GetLimitRect(const WindowMode& mode);
    Rect& GetDisplayRect(const WindowMode& mode);
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
