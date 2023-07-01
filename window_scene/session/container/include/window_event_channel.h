/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_H
#define OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_H

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/container/include/zidl/window_event_channel_stub.h"

namespace OHOS::Rosen {
class WindowEventChannel : public WindowEventChannelStub {
public:
    explicit WindowEventChannel(sptr<ISessionStage> iSessionStage) : sessionStage_(iSessionStage) {}
    ~WindowEventChannel() = default;

    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed) override;
    WSError TransferFocusActiveEvent(bool isFocusActive) override;
    WSError TransferFocusWindowId(uint32_t windowId) override;

private:
    void PrintKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event);
    void PrintPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event);

    sptr<ISessionStage> sessionStage_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_H
