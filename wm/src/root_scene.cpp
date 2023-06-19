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

#include "root_scene.h"

#include <event_handler.h>
#include <input_manager.h>
#include <ui_content.h>
#include <viewport_config.h>

#include "ability_manager_client.h"

#include "anr_manager.h"
#include "vsync_station.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "RootScene" };
const std::string INPUT_AND_VSYNC_THREAD = "InputAndVsyncThread";

class InputEventListener : public MMI::IInputEventConsumer {
public:
    explicit InputEventListener(RootScene* rootScene): rootScene_(rootScene) {}
    virtual ~InputEventListener() = default;

    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override
    {
        rootScene_->ConsumePointerEvent(pointerEvent);
    }

    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override
    {
        rootScene_->ConsumeKeyEvent(keyEvent);
    }

    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override
    {
    }

private:
    RootScene* rootScene_;
};
} // namespace

RootScene::RootScene()
{
}

RootScene::~RootScene()
{
}

void RootScene::LoadContent(const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
    AbilityRuntime::Context* context)
{
    CALL_DEBUG_ENTER;
    if (context == nullptr) {
        WLOGFE("context is nullptr!");
        return;
    }
    uiContent_ = Ace::UIContent::Create(context, engine);
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is nullptr!");
        return;
    }

    uiContent_->Initialize(this, contentUrl, storage);
    uiContent_->Foreground();

    RegisterInputEventListener();
    ANRMgr->Init();
    ANRMgr->SetAnrCallback(([](int32_t pid) {
        WLOGFI("WLD << Receive anr notice pid:%{public}d enter", pid);

        if (int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->SendANRProcessID(pid); ret != 0) {
            // 2097177 --- CHECK_PERMISSION_FAILED  权限错误 
            WLOGFE("WLD SendANRProcessID failed, processPid:%{public}d, errcode:%{public}d", pid, ret);
        }
        WLOGFI("WLD << Receive anr notice pid:%{public}d leave", pid);
    }));
}

void RootScene::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is nullptr!");
        return;
    }
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    config.SetDensity(density_);
    uiContent_->UpdateViewportConfig(config, reason);
}

void RootScene::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent)
{
    if (uiContent_) {
        uiContent_->ProcessPointerEvent(inputEvent);
    }
}

void RootScene::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent)
{
    if (uiContent_) {
        uiContent_->ProcessKeyEvent(inputEvent);
    }
}

void RootScene::RegisterInputEventListener()
{
    auto listener = std::make_shared<InputEventListener>(this);
    auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainEventRunner) {
        WLOGFD("MainEventRunner is available");
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
    } else {
        WLOGFD("MainEventRunner is not available");
        eventHandler_ = AppExecFwk::EventHandler::Current();
        if (!eventHandler_) {
            eventHandler_ =
                std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(INPUT_AND_VSYNC_THREAD));
        }
        VsyncStation::GetInstance().SetIsMainHandlerAvailable(false);
        VsyncStation::GetInstance().SetVsyncEventHandler(eventHandler_);
    }
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener, eventHandler_);
}

void RootScene::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    VsyncStation::GetInstance().RequestVsync(vsyncCallback);
}
} // namespace Rosen
} // namespace OHOS
