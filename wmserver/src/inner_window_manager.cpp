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

#include "inner_window_manager.h"
#include "window_manager_hilog.h"
#include "ui_service_mgr_client.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InnerWindowManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(InnerWindowManager)

InnerWindowManager::InnerWindowManager() : eventHandler_(nullptr), eventLoop_(nullptr),
    state_(InnerWMRunningState::STATE_NOT_START)
{
}

InnerWindowManager::~InnerWindowManager()
{
    Stop();
}

bool InnerWindowManager::Init()
{
    eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    if (eventLoop_ == nullptr) {
        return false;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        return false;
    }
    WLOGFI("init inner window manager service success.");
    return true;
}

void InnerWindowManager::Start()
{
    if (state_ == InnerWMRunningState::STATE_RUNNING) {
        WLOGFI("inner window manager service has already started.");
    }
    if (!Init()) {
        WLOGFI("failed to init inner window manager service.");
        return;
    }
    state_ = InnerWMRunningState::STATE_RUNNING;
    eventLoop_->Run();
    WLOGFI("inner window manager service start success.");
}

void InnerWindowManager::Stop()
{
    WLOGFI("stop inner window manager service.");
    eventLoop_->Stop();
    eventLoop_.reset();
    eventHandler_.reset();
    state_ = InnerWMRunningState::STATE_NOT_START;
}

void InnerWindowManager::HandleCreateWindow(std::string name, WindowType type, Rect rect)
{
    std::string params = "{\"shutdownButton\":\"Power off\"}";
    auto callback = [this](int32_t id, const std::string& event, const std::string& params) {
        if (params == "EVENT_INIT_CODE") {

        } else if (params == "EVENT_CANCLE_CODE") {
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        }
    };
    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        name, params, type, rect.posX_, rect.posY_, rect.width_, rect.height_, callback, &dialogId_
    );
    WLOGFI("create inner window id: %{public}d succes", dialogId_);
    return;
}

void InnerWindowManager::HandleDestroyWindow()
{
    WLOGFI("destroy inner window id:: %{public}d.", dialogId_);
    Ace::UIServiceMgrClient::GetInstance()->CancelDialog(dialogId_);
    dialogId_ = -1;
    return;
}

void InnerWindowManager::CreateWindow(std::string name, WindowType type, Rect rect)
{
    if (dialogId_ != -1) {
        return;
    }
    eventHandler_->PostTask([this, name, type, rect]() {
        HandleCreateWindow(name, type, rect);
    });
    return;
}

void InnerWindowManager::DestroyWindow()
{
    if (dialogId_ == -1) {
        WLOGFI("inner window has destroyed.");
        return;
    }
    eventHandler_->PostTask([this]() {
        HandleDestroyWindow();
    });
    return;
}
} // Rosen
} // OHOS