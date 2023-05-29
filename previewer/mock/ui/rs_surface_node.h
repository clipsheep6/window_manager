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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H

#include "ui/rs_node.h"

#include <string>
#include <stdint.h>

namespace OHOS {
namespace Rosen {

using NodeId = uint64_t;

struct RSSurfaceNodeConfig {
    std::string SurfaceNodeName = "SurfaceNode";
    void* additionalData = nullptr;
};

// types for RSSurfaceRenderNode
enum class RSSurfaceNodeType : uint8_t {
    DEFAULT,
    APP_WINDOW_NODE,          // surfacenode created as app main window
    ABILITY_COMPONENT_NODE,   // surfacenode created as ability component
    SELF_DRAWING_NODE,        // surfacenode created by arkui component (except ability component)
    STARTING_WINDOW_NODE,     // starting window, surfacenode created by wms
    LEASH_WINDOW_NODE,        // leashwindow
    SELF_DRAWING_WINDOW_NODE, // create by wms, such as pointer window and bootanimation
    EXTENSION_ABILITY_NODE,   // create by arkui to manage extension views
};

class RSSurfaceNode : public RSNode {
public:
    using SharedPtr = std::shared_ptr<RSSurfaceNode>;

    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, RSSurfaceNodeType type, bool isWindow = true);



protected:
    RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode) {};
    RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode, NodeId id) {};
    RSSurfaceNode(const RSSurfaceNode&) = delete;
    RSSurfaceNode(const RSSurfaceNode&&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&&) = delete;

};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
