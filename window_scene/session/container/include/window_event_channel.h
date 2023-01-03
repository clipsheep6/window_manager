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

#ifndef OHOS_WINDOW_EVENT_CHANNEL_H
#define OHOS_WINDOW_EVENT_CHANNEL_H

#include "zidl/window_event_channel_stub.h"

#include "scene_session_stage.h"
#include "window_scene_common.h"

namespace OHOS::Rosen {
class WindowEventChannel : public WindowEventChannelStub {
public:
    explicit WindowEventChannel(sptr<SceneSessionStage>& sessionStage) : sessionStage_(sessionStage) {}
    ~WindowEventChannel() = default;

    virtual WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    virtual WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;

private:
    sptr<SceneSessionStage> sessionStage_;
};
}
#endif // OHOS_WINDOW_EVENT_CHANNEL_H