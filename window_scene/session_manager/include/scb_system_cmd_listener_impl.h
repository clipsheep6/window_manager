/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCB_SYSTEM_CMD_LISTENER_IMPL_H
#define OHOS_ROSEN_WINDOW_SCENE_SCB_SYSTEM_CMD_LISTENER_IMPL_H

#include <shared_mutex>

#include "wm_common.h"
#include "ime_system_channel.h"

namespace OHOS::Rosen {
using NotifyReceiveKeyboardPrivateCommandFunc = std::function<void(const std::unordered_map<std::string,
    KeyboardPrivateDataValue>& privateCommand)>;
using NotifyReceiveIsPanelShowFunc = std::function<void(bool isShowKeyboardPanel)>;
class SCBOnSystemCmdListenerImpl : public OHOS::MiscServices::OnSystemCmdListener {
public:
    SCBOnSystemCmdListenerImpl();
    ~SCBOnSystemCmdListenerImpl() override = default;
    int32_t ReceivePrivateCommand(const std::unordered_map<std::string,
                                                           MiscServices::PrivateDataValue> &privateCommand) override;
    void NotifyIsShowSysPanel(bool shouldSysPanelShow) override;
    void SetPrivateCommandListener(const NotifyReceiveKeyboardPrivateCommandFunc& func);
    void SetKeyboardPanelStatusListener(const NotifyReceiveIsPanelShowFunc& func);
private:
    std::shared_mutex privateCommandListenerLock_;
    NotifyReceiveKeyboardPrivateCommandFunc receivePrivateCommandFunc_;
    std::shared_mutex keyboardPanelShowListenerLock_;
    NotifyReceiveIsPanelShowFunc receiveKeyboardPanelShowFunc_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCB_SYSTEM_CMD_LISTENER_IMPL_H
