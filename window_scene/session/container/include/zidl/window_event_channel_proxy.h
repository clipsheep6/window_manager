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

#ifndef OHOS_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_RPOXY_H
#define OHOS_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_RPOXY_H

#include <iremote_proxy.h>
#include <list>
#include <map>

#include "accessibility_element_info.h"
#include "interfaces/include/ws_common.h"
#include "window_event_channel_interface.h"

namespace OHOS::Rosen {
class WindowEventChannelProxy : public IRemoteProxy<IWindowEventChannel> {
public:
    explicit WindowEventChannelProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IWindowEventChannel>(impl) {};

    ~WindowEventChannelProxy() {};

    WSError TransferBackpressedEventForConsumed(bool& isConsumed) override;
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool isPreImeEvent = false) override;
    WSError TransferKeyEventForConsumedAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent,
        const sptr<IRemoteObject>& listener) override;
    WSError TransferFocusActiveEvent(bool isFocusActive) override;
    WSError TransferFocusState(bool focusState) override;
    WSError TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType, int32_t eventType,
        int64_t timeMs) override;
    WSError TransferAccessibilityChildTreeRegister(
        uint32_t windowId, int32_t treeId, int64_t accessibilityId) override;
    WSError TransferAccessibilityChildTreeUnregister() override;
    WSError TransferAccessibilityDumpChildInfo(
        const std::vector<std::string>& params, std::vector<std::string>& info) override;
        
private:
    static inline BrokerDelegator<WindowEventChannelProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_RPOXY_H

