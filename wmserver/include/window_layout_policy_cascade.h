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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_CASCADE_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_CASCADE_H

#include <map>
#include <refbase.h>
#include <set>

#include "window_layout_policy.h"
#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowLayoutPolicyCascade : public WindowLayoutPolicy {
public:
    WindowLayoutPolicyCascade() = delete;
    WindowLayoutPolicyCascade(const std::map<DisplayId, Rect>& displayRectMap,
                              std::unique_ptr<WindowNodeMaps> windowNodeMaps);
    ~WindowLayoutPolicyCascade() = default;
    void Launch() override;
    void Clean() override;
    void Reset() override;
    void Reorder() override;
    void UpdateDisplayInfo() override;
    void AddWindowNode(sptr<WindowNode>& node) override;
    void UpdateWindowNode(sptr<WindowNode>& node, bool isAddWindow = false) override;
    void UpdateLayoutRect(sptr<WindowNode>& node) override;

private:
    void InitSplitRects(DisplayId displayId);
    void SetSplitRectByRatio(float ratio, DisplayId displayId);
    void SetSplitRect(const Rect& rect, DisplayId displayId);
    void UpdateSplitLimitRect(const Rect& limitRect, Rect& limitSplitRect);
    void LayoutWindowNode(sptr<WindowNode>& node) override;
    void LayoutWindowTree(DisplayId displayId) override;
    void InitLimitRects(DisplayId displayId);
    void LimitMoveBounds(Rect& rect, DisplayId displayId);
    void InitCascadeRect(DisplayId displayId);
    void SetCascadeRect(const sptr<WindowNode>& node);

    Rect GetRectByWindowMode(const WindowMode& mode) const;
    Rect GetLimitRect(const WindowMode mode, DisplayId displayId) const;
    Rect GetDisplayRect(const WindowMode mode, DisplayId displayId) const;
    Rect GetCurCascadeRect(const sptr<WindowNode>& node) const;
    Rect StepCascadeRect(Rect rect, DisplayId displayId) const;

    struct CascadeRects {
        Rect primaryRect;
        Rect secondaryRect;
        Rect primaryLimitRect;
        Rect secondaryLimitRect;
        Rect dividerRect;
        Rect firstCascadeRect;
    };
    std::map<DisplayId, CascadeRects> cascadeRectsMap_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_CASCADE_H
