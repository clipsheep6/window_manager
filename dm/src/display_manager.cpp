/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "display_manager.h"

#include <cinttypes>
#include <transaction/rs_interfaces.h>

#include "display_manager_adapter.h"
#include "display_manager_agent.h"
#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManager)

DisplayManager::DisplayManager()
{
}

DisplayManager::~DisplayManager()
{
}

DisplayId DisplayManager::GetDefaultDisplayId()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDefaultDisplayId();
}

const sptr<Display> DisplayManager::GetDisplayById(DisplayId displayId)
{
    sptr<Display> display = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("DisplayManager::GetDisplayById failed!\n");
        return nullptr;
    }
    return display;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALD) {
        WLOGFE("displayId invalid!");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> screenShot =
        SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId);
    if (screenShot == nullptr) {
        WLOGFE("DisplayManager::GetScreenshot failed!");
        return nullptr;
    }

    return screenShot;
}

bool DisplayManager::CheckRectValid(const Media::Rect &rect, int32_t oriHeight, int32_t oriWidth) const
{
    if (!((rect.left >= 0) and (rect.left < oriWidth) and (rect.top >= 0) and (rect.top < oriHeight))) {
        WLOGFE("CheckRectValid rect left top invalid!");
        return false;
    }

    if (!((rect.width > 0) and (rect.width <= (oriWidth - rect.left)) and
        (rect.height > 0) and (rect.height <= (oriHeight - rect.top)))) {
        if (!((rect.width == 0) and (rect.height == 0))) {
            WLOGFE("CheckRectValid rect height width invalid!");
            return false;
        }
    }
    return true;
}

bool DisplayManager::CheckSizeValid(const Media::Size &size, int32_t oriHeight, int32_t oriWidth)
{
    if (!((size.width > 0) and (size.height > 0))) {
        if (!((size.width == 0) and (size.height == 0))) {
            WLOGFE("CheckSizeValid width height invalid!");
            return false;
        }
    }

    if ((size.width > MAX_RESOLUTION_SIZE_SCREENSHOT) or (size.height > MAX_RESOLUTION_SIZE_SCREENSHOT)) {
        WLOGFE("CheckSizeValid width and height too big!");
        return false;
    }
    return true;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId, const Media::Rect &rect,
                                                               const Media::Size &size, int rotation)
{
    if (displayId == DISPLAY_ID_INVALD) {
        WLOGFE("displayId invalid!");
        return nullptr;
    }
    
    std::shared_ptr<Media::PixelMap> screenShot =
        SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId);
    if (screenShot == nullptr) {
        WLOGFE("DisplayManager::GetScreenshot failed!");
        return nullptr;
    }

    // check parameters
    int32_t oriHeight = screenShot->GetHeight();
    int32_t oriWidth = screenShot->GetWidth();
    if (!CheckRectValid(rect, oriHeight, oriWidth)) {
        WLOGFE("rect invalid! left %{public}d, top %{public}d, w %{public}d, h %{public}d",
            rect.left, rect.top, rect.width, rect.height);
        return nullptr;
    }
    if (!CheckSizeValid(size, oriHeight, oriWidth)) {
        WLOGFE("size invalid! w %{public}d, h %{public}d", rect.width, rect.height);
        return nullptr;
    }
    
    // create crop dest pixelmap
    Media::InitializationOptions opt;
    opt.size.width = size.width;
    opt.size.height = size.height;
    opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
    opt.editable = false;
    auto pixelMap = Media::PixelMap::Create(*screenShot, rect, opt);
    if (pixelMap == nullptr) {
        WLOGFE("Media::PixelMap::Create failed!");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> dstScreenshot(pixelMap.release());

    return dstScreenshot;
}

const sptr<Display> DisplayManager::GetDefaultDisplay()
{
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManager::GetAllDisplayIds()
{
    std::vector<DisplayId> res;
    res.push_back(GetDefaultDisplayId());
    // 获取所有displayIds
    return res;
}

std::vector<const sptr<Display>> DisplayManager::GetAllDisplays()
{
    std::vector<const sptr<Display>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<Display> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.push_back(display);
        } else {
            WLOGFE("DisplayManager::GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}

void DisplayManager::RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    powerEventListeners_.push_back(listener);
    if (powerEventListenerAgent_ == nullptr) {
        powerEventListenerAgent_ = new DisplayManagerAgent();
        SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(powerEventListenerAgent_,
            DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    }
    WLOGFI("RegisterDisplayPowerEventListener end");
}

void DisplayManager::UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = std::find(powerEventListeners_.begin(), powerEventListeners_.end(), listener);
    if (iter == powerEventListeners_.end()) {
        WLOGFE("could not find this listener");
        return;
    }
    powerEventListeners_.erase(iter);
    if (powerEventListeners_.empty() && powerEventListenerAgent_ != nullptr) {
        SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(powerEventListenerAgent_,
            DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
        powerEventListenerAgent_ = nullptr;
    }
    WLOGFI("UnregisterDisplayPowerEventListener end");
}

void DisplayManager::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    WLOGFI("NotifyDisplayPowerEvent event:%{public}u, status:%{public}u, size:%{public}zu", event, status,
        powerEventListeners_.size());
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& listener : powerEventListeners_) {
        listener->OnDisplayPowerEvent(event, status);
    }
}

void DisplayManager::NotifyDisplayStateChanged(DisplayState state)
{
    WLOGFI("state:%{public}u", state);
    std::lock_guard<std::mutex> lock(mutex_);
    if (displayStateCallback_) {
        displayStateCallback_(state);
        ClearDisplayStateCallback();
        return;
    }
    WLOGFW("callback_ target is not set!");
}

void DisplayManager::ClearDisplayStateCallback()
{
    displayStateCallback_ = nullptr;
    if (displayStateAgent_ != nullptr) {
        SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(displayStateAgent_,
            DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
        displayStateAgent_ = nullptr;
    }
}

bool DisplayManager::WakeUpBegin(PowerStateChangeReason reason)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpBegin(reason);
}

bool DisplayManager::WakeUpEnd()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpEnd();
}

bool DisplayManager::SuspendBegin(PowerStateChangeReason reason)
{
    // dms->wms notify other windows to hide
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
}

bool DisplayManager::SuspendEnd()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendEnd();
}

bool DisplayManager::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    // TODO: should get all screen ids
    ScreenId defaultId = GetDefaultDisplayId();
    if (defaultId == DISPLAY_ID_INVALD) {
        return false;
    }
    WLOGFI("state:%{public}u, reason:%{public}u, defaultId:%{public}" PRIu64".", state, reason, defaultId);
    ScreenPowerStatus status;
    switch (state) {
        case DisplayPowerState::POWER_ON: {
            status = ScreenPowerStatus::POWER_STATUS_ON;
            break;
        }
        case DisplayPowerState::POWER_OFF: {
            status = ScreenPowerStatus::POWER_STATUS_OFF;
            break;
        }
        default: {
            WLOGFW("SetScreenPowerStatus state not support");
            return false;
        }
    }
    RSInterfaces::GetInstance().SetScreenPowerStatus(defaultId, status);
    return SingletonContainer::Get<DisplayManagerAdapter>().SetScreenPowerForAll(state, reason);
}

DisplayPowerState DisplayManager::GetScreenPower(uint64_t screenId)
{
    DisplayPowerState res = static_cast<DisplayPowerState>(RSInterfaces::GetInstance().GetScreenPowerStatus(screenId));
    WLOGFI("GetScreenPower:%{public}u, defaultId:%{public}" PRIu64".", res, screenId);
    return res;
}

bool DisplayManager::SetDisplayState(DisplayState state, DisplayStateCallback callback)
{
    WLOGFI("state:%{public}u", state);
    std::lock_guard<std::mutex> lock(mutex_);
    if (displayStateCallback_ != nullptr) {
        WLOGFI("previous callback not called, can't reset");
        return false;
    }
    displayStateCallback_ = callback;
    if (displayStateAgent_ == nullptr) {
        displayStateAgent_ = new DisplayManagerAgent();
        SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(displayStateAgent_,
            DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
    }

    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SetDisplayState(state);
    if (!ret) {
        ClearDisplayStateCallback();
    }
    return ret;
}

DisplayState DisplayManager::GetDisplayState(uint64_t displayId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayState(displayId);
}

bool DisplayManager::SetScreenBrightness(uint64_t screenId, uint32_t level)
{
    WLOGFI("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    RSInterfaces::GetInstance().SetScreenBacklight(screenId, level);
    return true;
}

uint32_t DisplayManager::GetScreenBrightness(uint64_t screenId) const
{
    uint32_t level = RSInterfaces::GetInstance().GetScreenBacklight(screenId);
    WLOGFI("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    return level;
}

void DisplayManager::NotifyDisplayEvent(DisplayEvent event)
{
    // Unlock event dms->wms restore other hidden windows
    SingletonContainer::Get<DisplayManagerAdapter>().NotifyDisplayEvent(event);
}
} // namespace OHOS::Rosen