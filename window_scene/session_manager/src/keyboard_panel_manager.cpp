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

#include "session_manager/include/keyboard_panel_manager.h"

#include "input_method_controller.h"
#include "window_manager_hilog.h"
#include "bundle_mgr_client.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "KeyboardPanelManager" };
std::recursive_mutex g_instanceMutex;
} // namespace
constexpr const char *SMART_MENU_METADATA_NAME = "ohos.extension.smart_menu";
using namespace OHOS::AppExecFwk;
using namespace OHOS::MiscServices;

KeyboardPanelManager::KeyboardPanelManager()
{
    WLOGFD("Create KeyboardPanelManager");
}

KeyboardPanelManager& KeyboardPanelManager::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static KeyboardPanelManager* instance = nullptr;
    if (instance == nullptr) {
        instance = new KeyboardPanelManager();
    }
    return *instance;
}

bool KeyboardPanelManager::CheckSystemCmdChannel()
{
    if (privateCommandListener_ != nullptr) {
        return true;
    }
    WLOGFD("privateCommandListener is nullptr, need ConnectSystemCmd");
    privateCommandListener_ = new SCBOnSystemCmdListenerImpl();
    if (inputMethodController_ == nullptr) {
        inputMethodController_ = InputMethodController::GetInstance();
    }
    auto ret = inputMethodController_->ConnectSystemCmd(privateCommandListener_);
    if (ret != ErrorCode::NO_ERROR) {
        WLOGFE("connect system cmd failed, errorCode = %{public}d", ret);
        return false;
    }
    return true;
}

int32_t KeyboardPanelManager::SendKeyboardPrivateCommand(const std::unordered_map<std::string,
                                                                              KeyboardPrivateDataValue> &privateCommand)
{
    WLOGFD("Create SendKeyboardPrivateCommand");
    if (CheckSystemCmdChannel()) {
        auto ret = inputMethodController_->SendPrivateCommand(privateCommand);
        if (ret != ErrorCode::NO_ERROR) {
            WLOGFE("send keyboard privateCommand failed, errorCode = %{public}d", ret);
            return ret;
        }
        return ret;
    }
    return ErrorCode::NO_ERROR;
}

void KeyboardPanelManager::SetKeyboardPanelIsPanelShowsListener(const NotifyReceiveKeyboardPanelStatusFunc& func)
{
    if (CheckSystemCmdChannel()) {
        privateCommandListener_->SetKeyboardPanelStatusListener(func);
    }
}

void KeyboardPanelManager::SetKeyboardPrivateCommandListener(const NotifyReceiveKeyboardPrivateCommandFunc& func)
{
    if (CheckSystemCmdChannel()) {
        privateCommandListener_->SetPrivateCommandListener(func);
    }
}

std::string KeyboardPanelManager::GetSmartMenuCfg()
{
    if (inputMethodController_ == nullptr) {
        inputMethodController_ = InputMethodController::GetInstance();
    }
    if (inputMethodController_ == nullptr) {
        return "";
    }
    std::shared_ptr<Property> prop = nullptr;
    int32_t ret = inputMethodController_->GetDefaultInputMethod(prop);
    if (ret != 0 || prop == nullptr) {
        WLOGFE("GetDefaultInputMethod failed");
        return "";
    }
    BundleMgrClient client;
    BundleInfo bundleInfo;
    if (!client.GetBundleInfo(prop->name, BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo)) {
        WLOGFE("GetBundleInfo failed");
        return "";
    }
    ExtensionAbilityInfo extInfo;
    auto extensionInfos = bundleInfo.extensionInfos;
    for (size_t i = 0; i < extensionInfos.size(); i++) {
        auto metadata = extensionInfos[i].metadata;
        for (size_t j = 0; j < metadata.size(); j++) {
            if (metadata[j].name == SMART_MENU_METADATA_NAME) {
                extInfo = extensionInfos[i];
                break;
            }
        }
    }
    std::vector<std::string> profiles;
    if (!client.GetResConfigFile(extInfo, SMART_MENU_METADATA_NAME, profiles) || profiles.empty()) {
        WLOGFE("GetResConfigFile failed");
        return "";
    }
    return profiles[0];
}
} // namespace