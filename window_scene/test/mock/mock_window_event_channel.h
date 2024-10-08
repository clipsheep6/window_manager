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

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/container/include/window_event_channel.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class WindowEventChannelMocker : public WindowEventChannel {
public:
    WindowEventChannelMocker(sptr<ISessionStage> sessionStage) : WindowEventChannel(sessionStage) {};
    ~WindowEventChannelMocker() {};
    MOCK_METHOD1(TransferKeyEvent, WSError(const std::shared_ptr<MMI::KeyEvent>& keyEvent));
    MOCK_METHOD1(TransferPointerEvent, WSError(const std::shared_ptr<MMI::PointerEvent>& pointerEvent));
    MOCK_METHOD3(TransferKeyEventForConsumedAsync, WSError(const std::shared_ptr<MMI::KeyEvent>& keyEvent,
        bool isPreImeEvent, const sptr<IRemoteObject>& listener));
#ifdef ACCESSIBILITY_MOCK
    MOCK_METHOD5(TransferAccessibilityHoverEvent, WSError(float pointX, float pointY, int32_t sourceType,
        int32_t eventType, int64_t timeMs));
    MOCK_METHOD3(TransferAccessibilityChildTreeRegister, WSError(
        uint32_t windowId, int32_t treeId, int64_t accessibilityId));
    MOCK_METHOD0(TransferAccessibilityChildTreeUnregister, WSError());
    MOCK_METHOD2(TransferAccessibilityDumpChildInfo, WSError(
        const std::vector<std::string>& params, std::vector<std::string>& info));
#endif
};
} // namespace Rosen
} // namespace OHOS
