/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_FOCUS_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_FOCUS_CONTROLLER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>

#include "dm_common.h"
#include "wm_common.h"
#include "ws_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class FocusGroup : public RefBase {
public:
    explicit FocusGroup(DisplayId displayGroupId) : displayGroupId(displayGroupId) {}

    int32_t GetFocusedSessionId() const { return focusedSessionId; }
    int32_t GetLastFocusedSessionId() const { return lastFocusedSessionId; }
    int32_t GetLastFocusedAppSessionId() const { return lastFocusedAppSessionId; }
    bool GetNeedBlockNotifyFocusStatusUntilForeground() const { return needBlockNotifyFocusStatusUntilForeground; }
    bool GetNeedBlockNotifyUnfocusStatus() const { return needBlockNotifyUnfocusStatus; }
    DisplayId GetDisplayGroupId() const { return displayGroupId; }
    void SetFocusedSessionId(int32_t persistentId) { focusedSessionId = persistentId; }
    void SetLastFocusedSessionId(int32_t persistentId) { lastFocusedSessionId = persistentId; }
    void SetLastFocusedAppSessionId(int32_t persistentId) { lastFocusedAppSessionId = persistentId; }
    void SetNeedBlockNotifyFocusStatusUntilForeground(bool needBlock)
    {
        needBlockNotifyFocusStatusUntilForeground = needBlock;
    }
    void SetNeedBlockNotifyUnfocusStatus(bool needBlock) { needBlockNotifyUnfocusStatus = needBlock; }
    WSError UpdateFocusedSessionId(int32_t persistentId);
    WSError UpdateFocusedAppSessionId(int32_t persistentId);

private:
    int32_t focusedSessionId = INVALID_SESSION_ID;
    int32_t lastFocusedSessionId = INVALID_SESSION_ID;
    int32_t lastFocusedAppSessionId = INVALID_SESSION_ID;
    bool needBlockNotifyFocusStatusUntilForeground { false };
    bool needBlockNotifyUnfocusStatus { false };
    DisplayId displayGroupId = DISPLAY_ID_INVALID;
};

class WindowFocusController : public RefBase {
public:
    WindowFocusController() noexcept;
    ~WindowFocusController() = default;

    DisplayId GetDisplayGroupId(DisplayId displayId);
    WSError AddFocusGroup(DisplayId displayId);
    WSError RemoveFocusGroup(DisplayId displayId);
    int32_t GetFocusedSessionId(DisplayId displayId);
    sptr<FocusGroup> GetFocusGroup(DisplayId displayId = DEFAULT_DISPLAY_ID);
    std::vector<std::pair<DisplayId, int32_t>> GetAllFocusedSessionList();
    WSError UpdateFocusedSessionId(DisplayId displayId, int32_t persistentId);
    WSError UpdateFocusedAppSessionId(DisplayId displayId, int32_t persistentId);

private:
    std::unordered_map<DisplayId, sptr<FocusGroup>> focusGroupMap_;
    std::unordered_set<DisplayId> virtualScreenDisplayIdSet_;
    
    sptr<FocusGroup> GetFocusGroupInner(DisplayId displayId);
};
}
}
#endif // OHOS_ROSEN_WINDOW_FOCUS_CONTROLLER_H
