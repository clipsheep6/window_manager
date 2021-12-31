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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H

#include <map>
#include <mutex>

#include <system_ability.h>

#include "display_manager_stub.h"
#include "display_screen.h"
#include "single_instance.h"
#include "singleton_delegator.h"
#include "display_screen_manager.h"

namespace OHOS::Rosen {
class DisplayManagerService : public SystemAbility, public DisplayManagerStub {
DECLARE_SYSTEM_ABILITY(DisplayManagerService);

DECLARE_SINGLE_INSTANCE_BASE(DisplayManagerService);

public:
    void OnStart() override;
    void OnStop() override;
    const sptr<DisplayInfo>& GetDisplayInfo(const DisplayType type) override;

    DisplayId GetDefaultDisplayId() override;
    DisplayInfo GetDisplayInfoById(DisplayId displayId) override;

private:
    DisplayManagerService();
    ~DisplayManagerService() = default;
    bool Init();
    DisplayId GetDisplayIdFromScreenId(ScreenId screenId);
    ScreenId GetScreenIdFromDisplayId(DisplayId displayId);

    static inline SingletonDelegator<DisplayManagerService> delegator_;
    std::map<int32_t, sptr<DisplayScreen>> displayScreenMap_;
    sptr<DisplayScreenManager> displayScreenManager_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H