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

#ifndef OHOS_ROSEN_AVOID_AREA_CONTROLLER_H
#define OHOS_ROSEN_AVOID_AREA_CONTROLLER_H

#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include <refbase.h>

#include "window_node.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
enum class AvoidControlType : uint32_t {
    AVOID_NODE_ADD,
    AVOID_NODE_UPDATE,
    AVOID_NODE_REMOVE,
    AVOID_NODE_UNKNOWN,
};

class AvoidAreaController : public RefBase {
public:
    AvoidAreaController(std::map<uint32_t, sptr<WindowNode>>& nodeMap, uint32_t& focusedWindow) : 
        windowNodeMap_(nodeMap), focusedWindow_(focusedWindow) {};
    ~AvoidAreaController() = default;

    void UpdateAvoidAreaListener(sptr<WindowNode>& windowNode, bool haveAvoidAreaListener);
    void ProcessWindowChange(const sptr<WindowNode>& windowNode, AvoidControlType avoidType);
    AvoidArea GetAvoidAreaByType(const sptr<WindowNode>& node, AvoidAreaType avoidAreaType) const;

private:
    void AddOrRemoveOverlayWindowIfNeed(const sptr<WindowNode>& overlayNode, bool isAdding);
    void AddOrRemoveKeyboard(const sptr<WindowNode>& keyboardNode, bool isAdding);
    void UpdateOverlayWindowIfNeed(const sptr<WindowNode>& node);
    sptr<AvoidArea> CalcOverlayRect(const sptr<WindowNode>& node,
        const sptr<WindowNode>& overlayNode, bool isRemoving) const;
    sptr<AvoidArea> BuildAvoidArea(const uint32_t width, const uint32_t height, const Rect& overlayRect) const;

    std::map<uint32_t, sptr<WindowNode>>& windowNodeMap_;
    uint32_t& focusedWindow_;
    std::unordered_map<uint32_t, sptr<WindowNode>> overlayWindowMap_;
    std::set<sptr<WindowNode>> haveListenerNodes_;
};
}
}
#endif // OHOS_ROSEN_AVOID_AREA_CONTROLLER_H
