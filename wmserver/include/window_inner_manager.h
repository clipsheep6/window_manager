/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_INNER_MANAGER_H
#define OHOS_ROSEN_WINDOW_INNER_MANAGER_H

#include <refbase.h>
#include "event_handler.h"
#include "event_runner.h"

#include <ui/rs_surface_node.h>
#include "draw/canvas.h"
#include "nocopyable.h"
#include "pixel_map.h"

#include "wm_common.h"
#include "wm_single_instance.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class TouchOutsideListener : public ITouchOutsideListener {
    virtual void OnTouchOutside();
};
class InputListener : public IInputEventListener {
    virtual void OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent);
    virtual void OnPointerInputEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent);
};
class LifeCycle : public IWindowLifeCycle {
    virtual void AfterForeground() {};
    virtual void AfterBackground() {};
    virtual void AfterFocused() {};
    virtual void AfterInactive();
    virtual void AfterUnfocused() {};
};
class WindowInnerManager : public RefBase {
friend class TouchOutsideListener;
friend class InputListener;
friend class LifeCycle;
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowInnerManager);
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
public:
    void Start();
    void Stop();
    void CreateWindow(std::string name, WindowType type, Rect rect);
    void DestroyWindow();
    void CreatePlaceHolderWindow(WindowMode mode, DisplayId displayId);
    void DestroyPlaceHolderWindow();
public:
    enum class InnerWMRunningState {
        STATE_NOT_START,
        STATE_RUNNING,
    };
    ~WindowInnerManager();

private:
    WindowInnerManager();
    bool Init();
    void HandleCreateWindow(std::string name, WindowType type, Rect rect);
    void HandleCreatePlaceHolderWindow(WindowMode mode, DisplayId displayId);
    void HandleDestroyWindow();

    sptr<OHOS::Surface> GetLayer();
    sptr<OHOS::SurfaceBuffer> GetSurfaceBuffer(sptr<OHOS::Surface> layer) const;
    void DoDraw(uint8_t *addr, uint32_t width, uint32_t height);
    void DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas);
    std::unique_ptr<OHOS::Media::PixelMap> DecodeImageToPixelMap(const std::string &imagePath);

private:
    int32_t dialogId_ = -1;
    std::shared_ptr<EventHandler> eventHandler_;
    std::shared_ptr<EventRunner> eventLoop_;
    InnerWMRunningState state_;
    std::string dividerParams_ = "";
    const std::string INNER_WM_THREAD_NAME = "inner_window_manager";
    sptr<OHOS::Rosen::Window> placeHolderwindow_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_INNER_MANAGER_H