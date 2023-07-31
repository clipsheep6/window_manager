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

#ifndef OHOS_WINDOW_SCENE_SESSION_STAGE_INTERFACE_H
#define OHOS_WINDOW_SCENE_SESSION_STAGE_INTERFACE_H

#include <iremote_broker.h>

#include "interfaces/include/ws_common.h"
#include "occupied_area_change_info.h"

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
class AxisEvent;
} // namespace MMI

namespace OHOS::Rosen {
class ISessionStage : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISessionStage");

    enum class SessionStageMessage : uint32_t {
        TRANS_ID_SET_ACTIVE,
        TRANS_ID_NOTIFY_SIZE_CHANGE,
        TRANS_ID_HANDLE_BACK_EVENT,
        TRANS_ID_NOTIFY_FOCUS_CHANGE,
        TRANS_ID_NOTIFY_DESTROY,
        TRANS_ID_NOTIFY_TOUCH_DIALOG_TARGET,
        TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA,
        TRANS_ID_NOTIFY_OCCUPIED_AREA_CHANGE_INFO,
        TRANS_ID_UPDATE_AVOID_AREA,
        TRANS_ID_NOTIFY_SCREEN_SHOT,
        TRANS_ID_DUMP_SESSSION_ELEMENT_INFO,
        TRANS_ID_NOTIFY_TOUCH_OUTSIDE,
    };

    virtual void NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) {}
    virtual void NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed) {}
    virtual void NotifyFocusActiveEvent(bool isFocusActive) {}
    virtual void NotifyFocusWindowIdEvent(int32_t windowId) {}
    virtual int32_t GetPersistentId() const
    {
        return -1;
    }
    virtual void NotifyFocusStateEvent(bool focusState) {}
    virtual WSError NotifyTransferComponentData(const AAFwk::WantParams& wantParams)
    {
        return WSError::WS_OK;
    }

    virtual WSError SetActive(bool active) = 0;
    virtual WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) = 0;
    virtual WSError HandleBackEvent() = 0;
    virtual WSError MarkProcessed(int32_t eventId) = 0;
    virtual WSError UpdateFocus(bool isFocused) = 0;
    virtual WSError NotifyDestroy() = 0;
    virtual void NotifyTouchDialogTarget() = 0;
    virtual void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info) = 0;
    virtual WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) = 0;
    virtual void NotifyScreenshot() = 0;
    virtual void DumpSessionElementInfo(const std::vector<std::string>& params) = 0;
    virtual WSError NotifyTouchOutside() = 0;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_SESSION_STAGE_INTERFACE_H
