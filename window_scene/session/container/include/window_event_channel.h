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

#include <functional>
#include <list>
#include <map>

#include "accessibility_element_info.h"
#include "iremote_proxy.h"

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/container/include/zidl/window_event_channel_stub.h"

namespace OHOS::Rosen {
class WindowEventChannelListenerProxy : public IRemoteProxy<IWindowEventChannelListener> {
public:
    explicit WindowEventChannelListenerProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IWindowEventChannelListener>(impl) {}
    virtual ~WindowEventChannelListenerProxy() = default;

    void OnTransferKeyEventForConsumed(int32_t keyEventId, bool isPreImeEvent,
                                       bool isConsumed, WSError retCode) override;

private:
    static inline BrokerDelegator<WindowEventChannelListenerProxy> delegator_;
};

class WindowEventChannel : public WindowEventChannelStub {
public:
    explicit WindowEventChannel(sptr<ISessionStage> iSessionStage) : sessionStage_(iSessionStage)
    {
    }
    ~WindowEventChannel() = default;

    void SetIsUIExtension(bool isUIExtension);
    void SetUIExtensionUsage(UIExtensionUsage uiExtensionUsage);
    WSError TransferBackpressedEventForConsumed(bool& isConsumed) override;
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool isPreImeEvent = false) override;
    WSError TransferKeyEventForConsumedAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent,
        const sptr<IRemoteObject>& listener) override;
    WSError TransferFocusActiveEvent(bool isFocusActive) override;
    WSError TransferFocusState(bool focusState) override;
    WSError TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
        int32_t eventType, int64_t timeMs) override;
    WSError TransferAccessibilityChildTreeRegister(
        uint32_t windowId, int32_t treeId, int64_t accessibilityId) override;
    WSError TransferAccessibilityChildTreeUnregister() override;
    WSError TransferAccessibilityDumpChildInfo(
        const std::vector<std::string>& params, std::vector<std::string>& info) override;

private:
    void PrintKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event);
    void PrintPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event);
    void PrintInfoPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event);
    static void OnDispatchEventProcessed(int32_t eventId, int64_t actionTime);
    bool IsUIExtensionKeyEventBlocked(const std::shared_ptr<MMI::KeyEvent>& keyEvent);

private:
    sptr<ISessionStage> sessionStage_ = nullptr;
    bool isUIExtension_ { false };
    UIExtensionUsage uiExtensionUsage_ { UIExtensionUsage::EMBEDDED };
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_H
