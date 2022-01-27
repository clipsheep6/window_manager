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

#ifndef OHOS_JS_WINDOW_LISTENER_H
#define OHOS_JS_WINDOW_LISTENER_H

#include <map>
#include <mutex>
#include <unordered_set>
#include "event_handler.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"
#include "window_manager.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class JsWindowListener : public IWindowChangeListener,
                         public ISystemBarChangedListener,
                         public IAvoidAreaChangedListener {
public:
    JsWindowListener(NativeEngine* engine, std::shared_ptr<OHOS::AppExecFwk::EventHandler> &handler)
        : engine_(engine), mainHandler_(handler) {}
    virtual ~JsWindowListener() = default;
    void AddCallback(NativeValue* jsListenerObject);
    void RemoveAllCallback();
    void RemoveCallback(NativeValue* jsListenerObject);
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override;
    void OnSizeChange(Rect rect) override;
    void OnAvoidAreaChanged(std::vector<Rect> avoidAreas) override;

private:
    void CallJsMethod(const char* methodName, NativeValue* const* argv = nullptr, size_t argc = 0);
    NativeEngine* engine_ = nullptr;
    std::mutex mtx_;
    std::vector<std::unique_ptr<NativeReference>> jsCallBack_;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif /* OHOS_JS_WINDOW_LISTENER_H */