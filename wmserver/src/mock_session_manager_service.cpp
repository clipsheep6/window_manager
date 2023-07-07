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
#include <iremote_broker.h>
#include <iremote_object.h>
#include <system_ability_definition.h>
#include <sstream>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MOCK_SMS"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(MockSessionManagerService)

MockSessionManagerService::MockSessionManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true)
{
}

bool MockSessionManagerService::RegisterMockSessionManagerService()
{
    WLOGFD("Register mock session manager service");
    bool res = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<MockSessionManagerService>());
    if (!res) {
        WLOGFE("register failed");
    }
    if (!Publish(this)) {
        WLOGFE("Publish failed");
    }
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
    RegisterMockSessionManagerService();
    WLOGFD("sessionManagerService set success!");
    return true;
}

sptr<IRemoteObject> MockSessionManagerService::GetSessionManagerService()
{
    if (sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return nullptr;
    }
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
     auto proxy = std::make_unique<MockSessionManagerServiceProxy>(sessionManagerService_);
     if (!proxy) {
         std::cout << "proxy is nullptr." << std::endl;
         return WMError::WM_ERROR_NULLPTR;
     }
     int ret = proxy->GetSessionDumpInfo(params, dumpInfo);
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
