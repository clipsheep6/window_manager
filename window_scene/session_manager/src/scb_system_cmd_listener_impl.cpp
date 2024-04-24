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

#include "session_manager/include/scb_system_cmd_listener_impl.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SCBOnSystemCmdListenerImpl" };
} // namespace

SCBOnSystemCmdListenerImpl::SCBOnSystemCmdListenerImpl()
{
    WLOGFD("Create SCBOnSystemCmdListenerImpl");
}

void SCBOnSystemCmdListenerImpl::SetPrivateCommandListener(const NotifyReceiveKeyboardPrivateCommandFunc& func)
{
    std::unique_lock<std::shared_mutex> lock(privateCommandListenerLock_);
    receivePrivateCommandFunc_ = func;
}

void SCBOnSystemCmdListenerImpl::SetKeyboardPanelStatusListener(const NotifyReceiveIsPanelShowFunc& func)
{
    std::unique_lock<std::shared_mutex> lock(keyboardPanelShowListenerLock_);
    receiveKeyboardPanelShowFunc_ = func;
}

int32_t SCBOnSystemCmdListenerImpl::ReceivePrivateCommand(const std::unordered_map<std::string,
    MiscServices::PrivateDataValue>& privateCommand)
{
    std::shared_lock<std::shared_mutex> lock(privateCommandListenerLock_);
    if (receivePrivateCommandFunc_) {
        receivePrivateCommandFunc_(privateCommand);
    }
    return MiscServices::ErrorCode::NO_ERROR;
}

void SCBOnSystemCmdListenerImpl::NotifyIsShowSysPanel(bool shouldSysPanelShow)
{
    std::shared_lock<std::shared_mutex> lock(keyboardPanelShowListenerLock_);
    if (receiveKeyboardPanelShowFunc_) {
        receiveKeyboardPanelShowFunc_(shouldSysPanelShow);
    }
}
} // namespace OHOS::Rosen