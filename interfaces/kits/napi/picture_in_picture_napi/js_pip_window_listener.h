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

#ifndef OHOS_JS_PIP_WINDOW_LISTENER_H
#define OHOS_JS_PIP_WINDOW_LISTENER_H

#include <map>
#include <mutex>

#include "class_var_definition.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"
#include "window_manager.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class JsPiPWindowListener : public IPiPLifeCycle,
                         public IPiPActionObserver,
                         public IPiPControlObserver {
public:
    JsPiPWindowListener(napi_env env, std::shared_ptr<NativeReference> callback)
        : env_(env), jsCallBack_(callback), weakRef_(wptr<JsPiPWindowListener> (this)) {}
    ~JsPiPWindowListener();
    void OnPreparePictureInPictureStart() override;
    void OnPictureInPictureStart() override;
    void OnPreparePictureInPictureStop() override;
    void OnPictureInPictureStop() override;
    void OnPictureInPictureOperationError(int32_t errorCode) override;
    void OnRestoreUserInterface() override;
    void OnActionEvent(const std::string& actionEvent, int32_t statusCode) override;
    void OnControlEvent(PiPControlType controlType, PiPControlStatus statusCode) override;

private:
    void OnPipListenerCallback(PiPState state, int32_t errorCode);
    napi_env env_ = nullptr;
    std::shared_ptr<NativeReference> jsCallBack_ = nullptr;
    wptr<JsPiPWindowListener> weakRef_  = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif /* OHOS_JS_PIP_WINDOW_LISTENER_H */
