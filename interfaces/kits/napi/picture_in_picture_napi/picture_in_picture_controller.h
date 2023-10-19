/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_PICTURE_IN_PICTURE_CONTROLLER_H
#define OHOS_PICTURE_IN_PICTURE_CONTROLLER_H

#include <event_handler.h>
#include <refbase.h>
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class PictureInPictureController : virtual public RefBase {
public:
    PictureInPictureController();
    ~PictureInPictureController();
    bool StartPictureInPicture();
    void SetAutoStartEnabled(bool enable);
    void IsAutoStartEnabled(bool& enable) const;
    bool StopPictureInPicture();
    void UpdateContentSize(uint32_t width, uint32_t height);
private:
    wptr<PictureInPictureController> weakRef_ = nullptr;

    sptr<Window> window_;
    int32_t mainWindowId_;
    Rect windowRect_ = {};
    bool isAutoStartEnabled_ = false;
    const float winCorner_ = 40.0f;

    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_PICTURE_IN_PICTURE_CONTROLLER_H
