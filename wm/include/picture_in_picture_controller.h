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

#define PIP_WINDOW_NAME "pip_window"

#include <event_handler.h>
#include <refbase.h>
#include "picture_in_picture_option.h"
#include "window.h"
#include "wm_common.h"
#include "picture_in_picture_interface.h"

namespace OHOS {
namespace Rosen {
class PictureInPictureController : virtual public RefBase {
public:
    constexpr static int32_t DEFAULT_WIDTH = 800;
    constexpr static int32_t DEFAULT_HEIGHT = 600;
    constexpr static int32_t DEFAULT_TIME_DELAY = 400;
    PictureInPictureController(sptr<PipOption> pipOption, uint32_t mainWindowId);
    ~PictureInPictureController();
    WMError StartPictureInPicture();
    WMError StopPictureInPicture(bool needAnim);
    sptr<Window> GetPipWindow();
    uint32_t GetMainWindowId();
    void SetPipWindow(sptr<Window> window);
    void SetAutoStartEnabled(bool enable);
    void IsAutoStartEnabled(bool& enable) const;
    void UpdateContentSize(uint32_t width, uint32_t height);
    void StartMove();
    void DoScale();
    void DoActionEvent(std::string& actionName);
    void RestorePictureInPictureWindow();
    void SetPictureInPictureLifecycle(sptr<IPiPLifeCycle> listener);
    void SetPictureInPictureActionObserver(sptr<IPiPActionObserver> listener);
    sptr<IPiPLifeCycle> GetPictureInPictureLifecycle() const;
    sptr<IPiPActionObserver> GetPictureInPictureActionObserver() const;

    class PipMainWindowLifeCycleImpl : public Rosen::IWindowLifeCycle {
    public:
        PipMainWindowLifeCycleImpl() {};
        ~PipMainWindowLifeCycleImpl() {};
        void AfterBackground() override;
        void BackgroundFailed(int32_t type) override;
    };
private:
    WMError CreatePictureInPictureWindow();
    WMError ShowPictureInPictureWindow();
    wptr<PictureInPictureController> weakRef_ = nullptr;
    sptr<PipOption> pipOption_;
    sptr<IPiPLifeCycle> pipLifeCycleListener_;
    sptr<IPiPActionObserver> pipActionObserver_;
    sptr<Window> window_;
    uint32_t mainWindowId_;
    Rect windowRect_ = {0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT};
    bool isAutoStartEnabled_ = false;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_PICTURE_IN_PICTURE_CONTROLLER_H
