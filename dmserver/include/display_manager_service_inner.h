/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_INNER_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_INNER_H

#include <vector>

#include <pixel_map.h>
#include <system_ability.h>

#include "abstract_display.h"
#include "display_change_listener.h"
#include "wm_single_instance.h"
#include "singleton_delegator.h"

namespace OHOS::Rosen {
class DisplayManagerServiceInner {
WM_DECLARE_SINGLE_INSTANCE(DisplayManagerServiceInner);

public:
    std::vector<const sptr<AbstractDisplay>> GetAllDisplays();
    DisplayId GetDefaultDisplayId();
    const sptr<AbstractDisplay> GetDefaultDisplay();
    const sptr<AbstractDisplay> GetDisplayById(DisplayId displayId);
    std::vector<DisplayId> GetAllDisplayIds();
    ScreenId GetRSScreenId(DisplayId displayId) const;
    const sptr<ScreenInfo> GetScreenInfoByDisplayId(DisplayId displayId) const;
    const sptr<SupportedScreenModes> GetScreenModesByDisplayId(DisplayId displayId);
    std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId);
    void UpdateRSTree(DisplayId displayId, std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd);
    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H