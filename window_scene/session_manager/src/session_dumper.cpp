/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "session_dumper.h"

#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <map>
#include <sstream>

#include "display_manager_service_inner.h"
#include "string_ex.h"
#include "unique_fd.h"
#include "display_group_info.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "Dumper"};
    constexpr int WINDOW_NAME_MAX_LENGTH = 20;
    const std::string ARG_DUMP_HELP = "-h";
    const std::string ARG_DUMP_ALL = "-a";
    const std::string ARG_DUMP_WINDOW = "-w";
}

WMError SessionDumper::Dump(int fd, const std::vector<std::u16string>& args)
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
        WMError errCode = ### DumpWindowInfo(params, dumpInfo);
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

WMError SessionDumper::DumpScreenGroupWindowInfo(ScreenId screenGroupId, std::string& dumpInfo)
{
    std::vector<std::string> params;
    params.push_back(ARG_DUMP_ALL);
    SessionDumpInfo dumpInfo;    
    auto remoteObject = SessionManager::GetInstance().GetRemoteObject();
    if (!remoteObject) {
        std::cout << "remoteObject is nullptr." << std::endl;
    }
    auto proxy = std::make_unique<MockSessionManagerServiceProxy>(remoteObject);
    if (!proxy) {
        std::cout << "proxy is nullptr." << std::endl;
        return WMError::WM_ERROR_NULLPTR;
    }

    SessionDumpInfo sessionDumpInfo;
    int ret = proxy->GetSessionDumpInfo(params, sessionDumpInfo);
    if (!ret) {
        std::cout << "GetSessionDumpInfo is error." << std::endl;
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    std::ostringstream oss;
    oss << "-------------------------------------ScreenGroup " << screenGroupId << "-------------------------------------" << std::endl;
    oss << "WindowName           DisplayId Pid     WinId Type Mode Flag ZOrd Orientation [ x    y    w    h    ]" << std::endl;
    for (auto& sData : sessionDumpInfo.sessionDataList_) {
        int32_t zOrder = sData.zOrder_;
        if (zOrder < 0) {
            zOrder = 0;
        } else if (zOrder == 0) {
            oss << "---------------------------------------------------------------------------------------" << std::endl;
        }
        WSRect rect = sData.sessionRect_;
        const std::string& windowName = sData.sessionName_.size() <= WINDOW_NAME_MAX_LENGTH ?
            sData.sessionName_ : sData.sessionName_.substr(0, WINDOW_NAME_MAX_LENGTH);
        // std::setw is used to set the output width and different width values are set to keep the format aligned.
        oss << std::left << std::setw(21) << windowName
            << std::left << std::setw(10) << sData.screenId_;
            << std::left << std::setw(8) << sData.callingPid_;
            << std::left << std::setw(6) << sData.persistId_;
            << std::left << std::setw(5) << static_cast<uint32_t>(sData.sessionType_);
            << std::left << std::setw(5) << static_cast<uint32_t>(sData.sessionMode_);
            << std::left << std::setw(5) << sData.sessionFlags_;
            << std::left << std::setw(5) << --zOrder
            << std::left << std::setw(12) << static_cast<uint32_t>(sData.orientation_)
            << "[ "
            << std::left << std::setw(5) << rect.posX_
            << std::left << std::setw(5) << rect.posY_
            << std::left << std::setw(5) << rect.width_
            << std::left << std::setw(5) << rect.height_
            << "]"
            << std::endl;
    }
    oss << "Focus window: " << sessionDumpInfo.focusSession_ << std::endl;
    oss << "total window num: " << sessionDumpInfo.sessionDataList_ << std::endl;
    dumpInfo.append(oss.str());
    return WMError::WM_OK;
}

WMError SessionDumper::DumpAllWindowInfo(std::string& dumpInfo)
{
    WMError ret = DumpScreenGroupWindowInfo(0, dumpInfo);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    return WMError::WM_OK;
}

WMError SessionDumper::DumpSpecifiedWindowInfo(uint32_t windowId, const std::vector<std::string>& params,
    std::string& dumpInfo)
{
    std::vector<std::string> params;
    params.push_back("-w 10");

    auto remoteObject = SessionManager::GetInstance().GetRemoteObject();
    if (!remoteObject) {
        std::cout << "remoteObject is nullptr." << std::endl;
    }
    auto proxy = std::make_unique<MockSessionManagerServiceProxy>(remoteObject);
    if (!proxy) {
        std::cout << "proxy is nullptr." << std::endl;
        return WMError::WM_ERROR_NULLPTR;
    }

    SessionData sData;
    int ret = proxy->GetSpecifiedSessionDumpInfo(params, sData);
    if (!ret) {
        std::cout << "GetSessionDumpInfo is error." << std::endl;
        return WMError::WM_ERROR_INVALID_PARAM;
    }



    SessionDumpInfo sessionDumpInfo;
    Rect rect = sessionDumpInfo.;

    auto node = windowRoot_->GetWindowNode(windowId);
    std::string isShown_ = node->startingWindowShown_ ? "true" : "false";
    std::string isVisible = node->isVisible_ ? "true" : "false";
    std::string Focusable = node->GetWindowProperty()->GetFocusable() ? "true" : "false";
    std::string DecoStatus = node->GetWindowProperty()->GetDecoStatus() ? "true" : "false";
    bool PrivacyMode = node->GetWindowProperty()->GetSystemPrivacyMode() ||
        node->GetWindowProperty()->GetPrivacyMode();
    std::string isPrivacyMode = PrivacyMode ? "true" : "false";
    std::ostringstream oss;
    oss << "WindowName: " << node->GetWindowName()  << std::endl;
    oss << "DisplayId: " << node->GetDisplayId() << std::endl;
    oss << "WinId: " << node->GetWindowId() << std::endl;
    oss << "Pid: " << node->GetCallingPid() << std::endl;
    oss << "Type: " << static_cast<uint32_t>(node->GetWindowType()) << std::endl;
    oss << "Mode: " << static_cast<uint32_t>(node->GetWindowMode()) << std::endl;
    oss << "Flag: " << node->GetWindowFlags() << std::endl;
    oss << "Orientation: " << static_cast<uint32_t>(node->GetRequestedOrientation()) << std::endl;
    oss << "IsStartingWindow: " << isShown_ << std::endl;
    oss << "FirstFrameCallbackCalled: " << node->firstFrameAvailable_ << std::endl;
    oss << "IsVisible: " << isVisible << std::endl;
    oss << "Focusable: "  << Focusable << std::endl;
    oss << "DecoStatus: "  << DecoStatus << std::endl;
    oss << "isPrivacyMode: "  << isPrivacyMode << std::endl;
    oss << "WindowRect: " << "[ "
        << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_
        << " ]" << std::endl;
    oss << "TouchHotAreas: ";
    std::vector<Rect> touchHotAreas;
    node->GetTouchHotAreas(touchHotAreas);
    int index = 0;
    for (const auto& area : touchHotAreas) {
        oss << "[ " << area.posX_ << ", " << area.posY_ << ", " << area.width_ << ", " << area.height_ << " ]";
        index++;
        if (index < static_cast<int32_t>(touchHotAreas.size())) {
            oss <<", ";
        }
    }
    oss << std::endl;
    dumpInfo.append(oss.str());
    if (node->GetWindowToken() != nullptr) {
        std::vector<std::string> resetParams;
        resetParams.assign(params.begin() + 2, params.end()); // 2: params num
        if (resetParams.empty()) {
            WLOGI("do not dump ui info");
            return WMError::WM_OK;
        }
        dumpInfoFuture_.ResetLock({});
        node->GetWindowToken()->DumpInfo(resetParams);
        auto infos = dumpInfoFuture_.GetResult(2000); // 2000: wait for 2000ms
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    }
    return WMError::WM_OK;
}

static bool IsValidDigitString(const std::string& windowIdStr)
{
    if (windowIdStr.empty()) {
        return false;
    }
    for (char ch : windowIdStr) {
        if ((ch >= '0' && ch <= '9')) {
            continue;
        }
        WLOGFE("invalid window id");
        return false;
    }
    return true;
}

WMError SessionDumper::DumpWindowInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (args.size() == 1 && args[0] == ARG_DUMP_ALL) { // 1: params num
        return DumpAllWindowInfo(dumpInfo);
    } else if (args.size() >= 2 && args[0] == ARG_DUMP_WINDOW && IsValidDigitString(args[1])) { // 2: params num
        uint32_t windowId = std::stoul(args[1]);
        return DumpSpecifiedWindowInfo(windowId, args, dumpInfo);
    } else {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
}

void SessionDumper::ShowIllegalArgsInfo(std::string& dumpInfo, WMError errCode)
{
    switch (errCode) {
        case WMError::WM_ERROR_INVALID_PARAM:
            dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
            break;
        case WMError::WM_ERROR_NULLPTR:
            dumpInfo.append("The window is invalid, you can enter '-a' to get valid window id.");
            break;
        default:
            break;
    }
}

void SessionDumper::ShowHelpInfo(std::string& dumpInfo)
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

void SessionDumper::ShowAceDumpHelp(std::string& dumpInfo)
{
}
}
}
