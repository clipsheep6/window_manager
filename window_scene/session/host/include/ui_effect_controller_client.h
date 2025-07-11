/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_UI_EFFECT_CONTROLLER_CLIENT_H
#define OHOS_ROSEN_UI_EFFECT_CONTROLLER_CLIENT_H
#include <functional>

#include "ui_effect_controller_client_stub.h"

namespace OHOS::Rosen {
class UIEffectControllerClient : public UIEffectControllerClientStub {
public:
    UIEffectControllerClient();
    WMError SetParams(const sptr<UIEffectParams>& params) override;
    void SetId(int32_t id) { id_ = id; }
    int32_t GetId() const { return id_; }

private:
    // unique id of ui effect controller
    int32_t id_;
    sptr<UIEffectParams> params_;
};
}
#endif