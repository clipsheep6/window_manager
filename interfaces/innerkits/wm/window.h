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

#ifndef OHOS_ROSEN_WINDOW_H
#define OHOS_ROSEN_WINDOW_H

#include <i_input_event_consumer.h>
#include <iremote_object.h>
#include <input_event.h>
#include <refbase.h>
#include <ui/rs_surface_node.h>
#include <key_event.h>
#include <pointer_event.h>
#include <axis_event.h>
#include "foundation/appexecfwk/standard/kits/appkit/native/ability_runtime/context/context.h"
#include "wm_common.h"
#include "window_option.h"
#include "window_life_cycle_interface.h"

class NativeValue;
class NativeEngine;
namespace OHOS::AppExecFwk {
    class Configuration;
}

namespace OHOS::AbilityRuntime {
    class AbilityContext;
}

namespace OHOS {
namespace Rosen {
class IWindowChangeListener : virtual public RefBase {
public:
    virtual void OnSizeChange(Rect rect) = 0;
};

class IAvoidAreaChangedListener : virtual public RefBase {
public:
    virtual void OnAvoidAreaChanged(const std::vector<Rect> avoidAreas) = 0;
};

class Window : public RefBase {
public:
    static sptr<Window> Create(const std::string& windowName,
        sptr<WindowOption>& option, const std::shared_ptr<AbilityRuntime::Context>& context = nullptr);
    static sptr<Window> Find(const std::string& windowName);

    virtual std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const = 0;

    virtual Rect GetRect() const = 0;
    virtual WindowType GetType() const = 0;
    virtual WindowMode GetMode() const = 0;
    virtual const std::string& GetWindowName() const = 0;
    virtual uint32_t GetWindowId() = 0;
    virtual uint32_t GetWindowFlags() = 0;
    virtual SystemBarProperty GetSystemBarPropertyByType(WindowType type) = 0;

    virtual WMError SetWindowType(WindowType type) = 0;
    virtual WMError SetWindowMode(WindowMode mode) = 0;
    virtual WMError AddWindowFlag(WindowFlag flag) = 0;
    virtual WMError RemoveWindowFlag(WindowFlag flag) = 0;
    virtual WMError SetWindowFlags(uint32_t flags) = 0;
    virtual WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) = 0;
    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) = 0;

    virtual WMError Destroy() = 0;
    virtual WMError Show() = 0;
    virtual WMError Hide() = 0;

    virtual WMError MoveTo(int32_t x, int32_t y) = 0;
    virtual WMError Resize(uint32_t width, uint32_t height) = 0;

    virtual WMError RequestFocus() const = 0;
    virtual void AddInputEventListener(std::shared_ptr<MMI::IInputEventConsumer>& inputEventListener) = 0; // for api 7
    virtual void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) = 0;
    virtual void ConsumePointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) = 0;
    virtual void RequestFrame() = 0;
    virtual void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration) = 0;

    virtual void RegisterLifeCycleListener(sptr<IWindowLifeCycle>& listener) = 0;
    virtual void RegisterWindowChangeListener(sptr<IWindowChangeListener>& listener) = 0;
    virtual void RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener) = 0;
    virtual void UnregisterAvoidAreaChangeListener() = 0;
    virtual WMError SetUIContent(std::shared_ptr<AbilityRuntime::AbilityContext> context,
        std::string& contentInfo, NativeEngine* engine, NativeValue* storage, bool isdistributed = false) = 0;
    virtual WMError SetUIContent(const std::string& contentInfo, NativeEngine* engine,
        NativeValue* storage, bool isdistributed = false) = 0;
    virtual const std::string& GetContentInfo() = 0;
};
}
}
#endif // OHOS_ROSEN_WINDOW_H
