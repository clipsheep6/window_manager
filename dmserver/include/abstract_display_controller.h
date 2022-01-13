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

#include "abstract_display.h"
#include "single_instance.h"
#include "transaction/rs_interfaces.h"
#include "virtual_display_info.h"

namespace OHOS::Rosen {
class AbstractDisplayController {
DECLARE_SINGLE_INSTANCE_BASE(AbstractDisplayController);
public:
    std::map<int32_t, sptr<AbstractDisplay>> abstractDisplayMap_;

    ScreenId GetDefaultScreenId();
    RSScreenModeInfo GetScreenActiveMode(ScreenId id);
    ScreenId CreateVirtualScreen(const VirtualDisplayInfo &virtualDisplayInfo, sptr<Surface> surface);
    bool DestroyVirtualScreen(ScreenId screenId);
    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(DisplayId displayId, ScreenId screenId);

private:
    AbstractDisplayController();
    ~AbstractDisplayController();
    void parepareRSScreenManger();

    OHOS::Rosen::RSInterfaces *rsInterface_;

    class ScreenshotCallback : public SurfaceCaptureCallback {
    public:
        ScreenshotCallback() = default;
        ~ScreenshotCallback() {};
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
        {
            if (flag_ == false) {
                flag_ = true;
                pixelMap_ = pixelmap;
            }
        }
        bool IsPixelMapOk()
        {
            return flag_;
        }
        std::shared_ptr<Media::PixelMap> GetPixelMap()
        {
            return pixelMap_;
        }
    private:
        bool flag_ = false;
        std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    };
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H