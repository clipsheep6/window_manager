/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_EVENT_CHANNEL_BASE_H
#define OHOS_ROSEN_WINDOW_EVENT_CHANNEL_BASE_H

#include "common/include/session_permission.h"
#include "key_event.h"

#include "session/host/include/session.h"
#include "session/host/include/system_session.h"
#include <ui/rs_surface_node.h>
#include "window_helper.h"

namespace OHOS {
namespace Rosen {
class TestWindowEventChannel : public IWindowEventChannel {
public:
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError TransferFocusActiveEvent(bool isFocusActive) override;
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
        bool isPreImeEvent = false) override;
    WSError TransferKeyEventForConsumedAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent,
        const sptr<IRemoteObject>& listener) override;
    WSError TransferFocusState(bool focusState) override;
    WSError TransferBackpressedEventForConsumed(bool& isConsumed) override;
    WSError TransferSearchElementInfo(int64_t elementId, int32_t mode, int64_t baseParent,
        std::list<Accessibility::AccessibilityElementInfo>& infos) override;
    WSError TransferSearchElementInfosByText(int64_t elementId, const std::string& text, int64_t baseParent,
        std::list<Accessibility::AccessibilityElementInfo>& infos) override;
    WSError TransferFindFocusedElementInfo(int64_t elementId, int32_t focusType, int64_t baseParent,
        Accessibility::AccessibilityElementInfo& info) override;
    WSError TransferFocusMoveSearch(int64_t elementId, int32_t direction, int64_t baseParent,
        Accessibility::AccessibilityElementInfo& info) override;
    WSError TransferExecuteAction(int64_t elementId, const std::map<std::string, std::string>& actionArguments,
        int32_t action, int64_t baseParent) override;
    WSError TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType, int32_t eventType,
        int64_t timeMs) override;

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

WSError TestWindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFocusActiveEvent(bool isFocusActive)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferKeyEventForConsumed(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed, bool isPreImeEvent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferKeyEventForConsumedAsync(const std::shared_ptr<MMI::KeyEvent>& keyEvent,
    bool isPreImeEvent, const sptr<IRemoteObject>& listener)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFocusState(bool foucsState)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferBackpressedEventForConsumed(bool& isConsumed)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferSearchElementInfo(int64_t elementId, int32_t mode, int64_t baseParent,
    std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferSearchElementInfosByText(int64_t elementId, const std::string& text,
    int64_t baseParent, std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFindFocusedElementInfo(int64_t elementId, int32_t focusType, int64_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferFocusMoveSearch(int64_t elementId, int32_t direction, int64_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferExecuteAction(int64_t elementId,
    const std::map<std::string, std::string>& actionArguments, int32_t action, int64_t baseParent)
{
    return WSError::WS_OK;
}

WSError TestWindowEventChannel::TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
    return WSError::WS_OK;
}
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_EVENT_CHANNEL_BASE_H