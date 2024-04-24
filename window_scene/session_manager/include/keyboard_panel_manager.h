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

#ifndef OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_PANEL_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_PANEL_MANAGER_H

#include "ime_system_channel.h"
#include "scb_system_cmd_listener_impl.h"
#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class KeyboardPanelManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(KeyboardPanelManager)
public:
    KeyboardPanelManager();
    ~KeyboardPanelManager() = default;
    void SetKeyboardPrivateCommandListener(const NotifyReceiveKeyboardPrivateCommandFunc& func);
    void SetKeyboardPanelIsPanelShowsListener(const NotifyReceiveIsPanelShowFunc& func);
    int32_t SendKeyboardPrivateCommand(const std::unordered_map<std::string,
        KeyboardPrivateDataValue>& privateCommand);
    std::string GetSmartMenuCfg();
private:
    bool CheckSystemCmdChannel();
    sptr<SCBOnSystemCmdListenerImpl> privateCommandListener_;
    sptr<OHOS::MiscServices::ImeSystemCmdChannel> imeSystemCmdChannel_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_KEYBOARD_PANEL_MANAGER_H