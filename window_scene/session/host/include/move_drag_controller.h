/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H

#include <refbase.h>

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "wm_common.h"

namespace OHOS::Rosen {

using NotifyVsyncHandleFunc = std::function<void(void)>;

class MoveDragController : public RefBase {
public:
    struct MoveProperty
    {
        bool isStartMove_;
        int32_t startPointerId_;
        int32_t startPointerDisplayX_;
        int32_t startPointerDisplayY_;
        int32_t startWindowDisplayX_;
        int32_t startWindowDisplayY_;
        WSRect targetRect_;
    };

    MoveDragController();
    ~MoveDragController() = default;

    void SetVsyncHandleListenser(const NotifyVsyncHandleFunc& func);
    void SetStartMoveFlag(bool flag);
    bool GetStartMoveFlag();
    void SetTargetRect(const WSRect& rect);
    MoveProperty GetMoveProperty() const;
    void StartMove(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void EndMove();
    void RequestVsync(void);

private:
    std::shared_ptr<VsyncCallback> vsyncCallback_ = std::make_shared<VsyncCallback>(VsyncCallback());
    NotifyVsyncHandleFunc vsyncHandleFunc_;
    MoveProperty moveProperty_ = { false, -1, -1, -1, -1, -1, { 0, 0, 0, 0 } };

    void RemoveVsync();
    void OnReceiveVsync(int64_t timeStamp);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MOVE_DRAG_CONTROLLER_H