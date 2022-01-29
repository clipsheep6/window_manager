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

#include "display_manager_agent_controller.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerAgentController"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerAgentController)

bool DisplayManagerAgentController::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    return dmAgentContainer_.RegisterAgent(displayManagerAgent, type);
}

bool DisplayManagerAgentController::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    return dmAgentContainer_.UnregisterAgent(displayManagerAgent, type);
}

bool DisplayManagerAgentController::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    if (agents.empty()) {
        return false;
    }
    WLOGFI("NotifyDisplayPowerEvent");
    for (auto& agent : agents) {
        agent->NotifyDisplayPowerEvent(event, status);
    }
    return true;
}

bool DisplayManagerAgentController::NotifyDisplayStateChanged(DisplayState state)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
    if (agents.empty()) {
        return false;
    }
    WLOGFI("NotifyDisplayStateChanged");
    for (auto& agent : agents) {
        agent->NotifyDisplayStateChanged(state);
    }
    return true;
}

void DisplayManagerAgentController::OnScreenConnect(sptr<ScreenInfo> screenInfo)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("OnScreenConnect");
    for (auto& agent : agents) {
        agent->OnScreenConnect(screenInfo);
    }
}

void DisplayManagerAgentController::OnScreenDisconnect(ScreenId screenId)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("OnScreenDisconnect");
    for (auto& agent : agents) {
        agent->OnScreenDisconnect(screenId);
    }
}

void DisplayManagerAgentController::OnScreenChange(
    const std::vector<const sptr<ScreenInfo>>& screenInfos, ScreenChangeEvent screenChangeEvent)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("OnScreenChange");
    for (auto& agent : agents) {
        agent->OnScreenChange(screenInfos, screenChangeEvent);
    }
}

void DisplayManagerAgentController::OnDisplayCreate(sptr<DisplayInfo> displayInfo)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("OnDisplayCreate");
    for (auto& agent : agents) {
        agent->OnDisplayCreate(displayInfo);
    }
}

void DisplayManagerAgentController::OnDisplayDestroy(DisplayId displayId)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("OnDisplayDestroy");
    for (auto& agent : agents) {
        agent->OnDisplayDestroy(displayId);
    }
}

void DisplayManagerAgentController::OnDisplayChange(sptr<DisplayInfo> displayInfo, DisplayChangeEvent screenChangeEvent)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("OnDisplayChange");
    for (auto& agent : agents) {
        agent->OnDisplayChange(displayInfo, screenChangeEvent);
    }
}
}
}