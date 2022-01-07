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

#ifndef OHOS_ROSEN_WINDOW_NODE_H
#define OHOS_ROSEN_WINDOW_NODE_H

#include <ipc_skeleton.h>
#include <refbase.h>
#include <ui/rs_surface_node.h>
#include "window_interface.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class WindowNode : public RefBase {
public:
    WindowNode(const sptr<WindowProperty>& property, const sptr<IWindow>& window,
        std::shared_ptr<RSSurfaceNode> surfaceNode)
        : surfaceNode_(surfaceNode), property_(property), windowToken_(window)
    {
        callingPid_ = IPCSkeleton::GetCallingPid();
        callingUid_ = IPCSkeleton::GetCallingUid();
    }
    WindowNode() : property_(new WindowProperty())
    {
        callingPid_ = IPCSkeleton::GetCallingPid();
        callingUid_ = IPCSkeleton::GetCallingUid();
    }
    ~WindowNode() = default;

    void SetDisplayId(int32_t displayId);
    void SetLayoutRect(const Rect& rect);
    void SetWindowProperty(const sptr<WindowProperty>& property);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& property);

    const sptr<IWindow>& GetWindowToken() const;
    uint32_t GetWindowId() const;
    int32_t GetDisplayId() const;
    const Rect& GetLayoutRect() const;
    WindowType GetWindowType() const;
    WindowMode GetWindowMode() const;
    uint32_t GetWindowFlags() const;
    const sptr<WindowProperty>& GetWindowProperty() const;
    int32_t GetCallingPid() const;
    int32_t GetCallingUid() const;
    const std::unordered_map<WindowType, SystemBarProperty>& GetSystemBarProperty() const;

    sptr<WindowNode> parent_;
    std::vector<sptr<WindowNode>> children_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    sptr<IRemoteObject> abilityToken_ = nullptr;
    int32_t priority_ { 0 };
    bool requestedVisibility_ { false };
    bool currentVisibility_ { false };

private:
    sptr<WindowProperty> property_;
    sptr<IWindow> windowToken_;
    Rect layoutRect_ { 0, 0, 0, 0 };
    int32_t callingPid_;
    int32_t callingUid_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_NODE_H
