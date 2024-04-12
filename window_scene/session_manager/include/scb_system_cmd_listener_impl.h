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

#include "wm_common.h"
#include "input_method_controller.h"

namespace OHOS::Rosen {
using NotifyReceiveKeyboardPrivateCommandFunc = std::function<void(const std::unordered_map<std::string,
    KeyboardPrivateDataValue>& privateCommand)>;
using NotifyReceiveKeyboardPanelStatusFunc = std::function<void(const bool isShowKeyboardPanel)>;
class SCBOnSystemCmdListenerImpl : public OHOS::MiscServices::OnSystemCmdListener {
public:
    SCBOnSystemCmdListenerImpl();
    ~SCBOnSystemCmdListenerImpl() override = default;
    int32_t ReceivePrivateCommand(const std::unordered_map<std::string,
                                                           MiscServices::PrivateDataValue> &privateCommand) override;
    void OnNotifyIsShowSysPanel(bool isShow) override;
    void SetPrivateCommandListener(const NotifyReceiveKeyboardPrivateCommandFunc& func);
    void SetKeyboardPanelStatusListener(const NotifyReceiveKeyboardPanelStatusFunc& func);
private:
    std::mutex setPrivateCommandListenerLock;
    NotifyReceiveKeyboardPrivateCommandFunc receivePrivateCommandFunc_;
    std::mutex setKeyboardPanelShowListenerLock;
    NotifyReceiveKeyboardPanelStatusFunc receiveKeyboardPanelShowFunc_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCB_SYSTEM_CMD_LISTENER_IMPL_H
