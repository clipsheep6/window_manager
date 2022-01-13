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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H
#define FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H

#include <map>
#include <pixel_map.h>
#include <surface.h>
#include <transaction/rs_interfaces.h>

#include "abstract_display.h"
#include "abstract_screen_controller.h"
#include "transaction/rs_interfaces.h"
#include "virtual_display_info.h"

namespace OHOS::Rosen {
class AbstractDisplayController : public RefBase {
public:
    AbstractDisplayController(std::recursive_mutex& mutex);
    ~AbstractDisplayController();

    void Init(sptr<AbstractScreenController> abstractScreenController);
    ScreenId GetDefaultScreenId();
    RSScreenModeInfo GetScreenActiveMode(ScreenId id);
    ScreenId CreateVirtualScreen(const VirtualDisplayInfo &virtualDisplayInfo, sptr<Surface> surface);
    bool DestroyVirtualScreen(ScreenId screenId);
    sptr<Media::PixelMap> GetScreenSnapshot(ScreenId screenId);

    std::map<DisplayId, sptr<AbstractDisplay>> abstractDisplayMap_;
private:
    void OnAbstractScreenConnected(sptr<AbstractScreen> absScreen);
    void OnAbstractScreenDisconnected(sptr<AbstractScreen> absScreen);
    void OnAbstractScreenChanged(sptr<AbstractScreen> absScreen);
    void CreateAndBindDisplayLocked(sptr<AbstractScreen> absScreen);

    std::recursive_mutex& mutex_;
    volatile DisplayId displayCount_;
    sptr<AbstractScreenController> abstractScreenController_;
    sptr<AbstractScreenController::AbstractScreenCallback> abstractScreenCallback_;
    OHOS::Rosen::RSInterfaces *rsInterface_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H