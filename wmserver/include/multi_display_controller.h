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

#ifndef OHOS_ROSEN_MULTI_DISPLAY_CONTROLLER_H
#define OHOS_ROSEN_MULTI_DISPLAY_CONTROLLER_H

#include <refbase.h>

#include "avoid_area_controller.h"
#include "display_info.h"
#include "display_manager_service_inner.h"
#include "wm_common.h"
#include "window_layout_policy.h"
#include "window_manager.h"
#include "window_node.h"

namespace OHOS {
namespace Rosen {
using SysBarNodeMap = std::unordered_map<WindowType, sptr<WindowNode>>;
using SysBarTintMap = std::unordered_map<WindowType, SystemBarRegionTint>;
class WindowNodeContainer;

class MultiDisplayController : public RefBase {
public:
    MultiDisplayController(const sptr<WindowNodeContainer>& windowNodeContainer,
        std::map<DisplayId, Rect>& displayRectMap, std::map<DisplayId, sptr<DisplayInfo>>& displayInfosMap)
        : windowNodeContainer_(windowNodeContainer), displayRectMap_(displayRectMap),
          displayInfosMap_(displayInfosMap) {}
    ~MultiDisplayController() = default;

    void InitWindowNodesAndSysBarMapForDisplay(DisplayId displayId);
    void UpdateWindowNodeMaps();
    void PreProcessWindowNode(const sptr<WindowNode>& node, WindowUpdateType type);
    void ProcessDisplayCreate(DisplayId displayId,
                              const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap);
    void ProcessDisplayDestroy(DisplayId displayId,
                               const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap,
                               std::vector<uint32_t>& windowIds);
    void ProcessDisplayChange(const sptr<DisplayInfo>& displayInfo, DisplayStateChangeType type);

    WindowNodeMaps windowNodeMaps_;
    std::map<DisplayId, SysBarNodeMap> sysBarNodeMaps_;
    std::map<DisplayId, SysBarTintMap> sysBarTintMaps_;

private:
    std::vector<sptr<WindowNode>>* FindNodeVectorOfRoot(DisplayId displayId, WindowRootNodeType type);
    void AddWindowNodeInRootNodeVector(sptr<WindowNode>& node, WindowRootNodeType rootType);
    void ProcessCrossDisplayNodesIfDisplayChange();
    void MoveCrossNodeToDefaultDisplay(const sptr<WindowNode>& node);
    void MoveWindowNodeToDefaultDisplay(DisplayId displayId, const sptr<WindowNode>& node);
    void UpdateWindowCrossDisplayState(const sptr<WindowNode>& node, const Rect& requestRect);
    void UpdateWindowDisplayIdIfNeeded(const sptr<WindowNode>& node,
                                       const std::vector<DisplayId>& oldShownDisplays,
                                       const std::vector<DisplayId>& newShownDisplays);
    void ProcessWindowNodesOnDestroiedDisplay(DisplayId displayId, std::vector<uint32_t>& windowIds);
    void ProcessDisplaySizeChange(const sptr<DisplayInfo>& displayInfo, DisplayStateChangeType type);

    sptr<WindowNodeContainer> windowNodeContainer_;
    std::map<DisplayId, Rect>& displayRectMap_;
    std::map<DisplayId, sptr<DisplayInfo>>& displayInfosMap_;
    DisplayId defaultDisplayId_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_MULTI_DISPLAY_CONTROLLER_H