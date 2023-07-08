/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "mock_session_manager_service.h"

#include <system_ability_definition.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerService" };

constexpr int WINDOW_NAME_MAX_LENGTH = 20;
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
}

WM_IMPLEMENT_SINGLE_INSTANCE(MockSessionManagerService)

void MockSessionManagerService::SMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& object)
{
    auto sessionManagerService = object.promote();
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is null");
        return;
    }
    WLOGFI("SessionManagerService died, restart foundation now!");
    exit(0);
}

MockSessionManagerService::MockSessionManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true)
{
}

bool MockSessionManagerService::RegisterMockSessionManagerService()
{
    bool res = SystemAbility::MakeAndRegisterAbility(this);
    if (!res) {
        WLOGFE("register failed");
    }
    if (!Publish(this)) {
        WLOGFE("Publish failed");
    }
    WLOGFI("Publish mock session manager service success");
    return true;
}

void MockSessionManagerService::OnStart()
{
    WLOGFD("OnStart begin");
}

WMError MockSessionManagerService::Dump(int fd, const std::vector<std::string>& args)
{
    WLOGI("Dump begin fd: %{public}d", fd);
    if (fd < 0) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    (void) signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE crash
    UniqueFd ufd = UniqueFd(fd); // auto close
    fd = ufd.Get();
    std::vector<std::string> params;
    for (auto& arg : args) {
        params.emplace_back(Str16ToStr8(arg));
    }

    std::string dumpInfo;
    if (params.empty()) {
        ShowHelpInfo(dumpInfo);
    } else if (params.size() == 1 && params[0] == ARG_DUMP_HELP) { // 1: params num
        ShowHelpInfo(dumpInfo);
    } else {
        WMError errCode = DumpWindowInfo(params, dumpInfo);
        if (errCode != WMError::WM_OK) {
            ShowIllegalArgsInfo(dumpInfo, errCode);
        }
    }
    int ret = dprintf(fd, "%s\n", dumpInfo.c_str());
    if (ret < 0) {
        WLOGFE("dprintf error");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    WLOGI("Dump end");
    return WMError::WM_OK;
}

bool MockSessionManagerService::SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService)
{
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is nullptr");
        return false;
    }
    sessionManagerService_ = sessionManagerService;

    smsDeathRecipient_ = new SMSDeathRecipient();
    if (sessionManagerService_->IsProxyObject() && !sessionManagerService_->AddDeathRecipient(smsDeathRecipient_)) {
        WLOGFE("Failed to add death recipient");
        return false;
    }

    RegisterMockSessionManagerService();
    WLOGFI("sessionManagerService set success!");
    return true;
}

sptr<IRemoteObject> MockSessionManagerService::GetSessionManagerService()
{
    if (!sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return nullptr;
    }
    WLOGFD("Get session manager service success");
    return sessionManagerService_;
}

WMError MockSessionManagerService::DumpWindowInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (args.size() == 1 && args[0] == ARG_DUMP_ALL) { // 1: params num
        return DumpAllWindowInfo(dumpInfo);
    } else if (args.size() >= 2 && args[0] == ARG_DUMP_WINDOW && IsValidDigitString(args[1])) { // 2: params num
        uint32_t windowId = std::stoul(args[1]);
    }
    return WMError::WM_ERROR_INVALID_PARAM;
}

WMError MockSessionManagerService::DumpAllWindowInfo(std::string& dumpInfo)
{
     if (!sessionManagerService_) {
         WLOGFE("sessionManagerService is nullptr");
         return nullptr;
     }
     sptr<IRemoteObject> remoteObject = sessionManagerService_->GetSceneSessionManager();
     if (!remoteObject) {
         WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
         return;
     }
     sptr<ISceneSessionManager> sceneSessionManagerProxy = iface_cast<ISceneSessionManager>(remoteObject);
     if (!sceneSessionManagerProxy) {
         WLOGFW("Get scene session manager proxy failed, nullptr");
     }
     std::vector<std::string> params;
     params.push_back(ARG_DUMP_ALL);
     WMError ret = sceneSessionManagerProxy->GetSessionDumpInfo(params, dumpInfo);
     if (ret != WMError::WM_OK) {
         WLOGFD("sessionManagerService set success!");
         return ret;
     }
    return WMError::WM_OK;
}

void WindowDumper::ShowHelpInfo(std::string& dumpInfo)
{
    dumpInfo.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all window information in the system\n")
        .append(" -w {window id} [ArkUI Option]  ")
        .append("|dump specified window information\n")
        .append(" ------------------------------------[ArkUI Option]------------------------------------ \n");
    ShowAceDumpHelp(dumpInfo);
}

void WindowDumper::ShowAceDumpHelp(std::string& dumpInfo)
{
}

} // namespace Rosen
} // namespace OHOS
