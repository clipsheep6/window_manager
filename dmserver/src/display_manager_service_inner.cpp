/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "display_manager_service_inner.h"

#include <cinttypes>
#include <iservice_registry.h>

#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerServiceInner"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerServiceInner)

DisplayId DisplayManagerServiceInner::GetDefaultDisplayId() const
{
    auto defaultDisplayInfo = DisplayManagerService::GetInstance().GetDefaultDisplayInfo();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("GetDefaultDisplayId, defaultDisplayInfo is nullptr.");
        return DISPLAY_ID_INVALID;
    }
    return defaultDisplayInfo->GetDisplayId();
}

sptr<DisplayInfo> DisplayManagerServiceInner::GetDisplayById(DisplayId displayId) const
{
    sptr<DisplayInfo> display = DisplayManagerService::GetInstance().GetDisplayInfoById(displayId);
    if (display == nullptr) {
        WLOGFE("GetDisplayById can not find corresponding display!\n");
    }
    return display;
}

sptr<DisplayInfo> DisplayManagerServiceInner::GetDefaultDisplay() const
{
    return DisplayManagerService::GetInstance().GetDefaultDisplayInfo();
}

std::vector<DisplayId> DisplayManagerServiceInner::GetAllDisplayIds() const
{
    return DisplayManagerService::GetInstance().GetAllDisplayIds();
}

std::vector<sptr<DisplayInfo>> DisplayManagerServiceInner::GetAllDisplays() const
{
    std::vector<sptr<DisplayInfo>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        sptr<DisplayInfo> display = DisplayManagerService::GetInstance().GetDisplayInfoById(displayId);
        if (display != nullptr) {
            res.emplace_back(display);
        } else {
            WLOGFE("GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}

void DisplayManagerServiceInner::UpdateRSTree(DisplayId displayId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
    bool isAdd, bool isMultiDisplay, DisplayId parentDisplayId)
{
    DisplayManagerService::GetInstance().UpdateRSTree(displayId, surfaceNode, isAdd, isMultiDisplay, parentDisplayId);
}

ScreenId DisplayManagerServiceInner::GetRSScreenId(DisplayId displayId) const
{
    return DisplayManagerService::GetInstance().GetRSScreenId(displayId);
}

sptr<ScreenInfo> DisplayManagerServiceInner::GetScreenInfoByDisplayId(DisplayId displayId) const
{
    auto displayInfo = DisplayManagerService::GetInstance().GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("can not get display.");
        return nullptr;
    }
    return DisplayManagerService::GetInstance().GetScreenInfoById(displayInfo->GetScreenId());
}

ScreenId DisplayManagerServiceInner::GetScreenGroupIdByDisplayId(DisplayId displayId) const
{
    auto displayInfo = DisplayManagerService::GetInstance().GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("can not get display.");
        return INVALID_SCREEN_ID;
    }
    return DisplayManagerService::GetInstance().GetScreenGroupIdByScreenId(displayInfo->GetScreenId());
}

sptr<SupportedScreenModes> DisplayManagerServiceInner::GetScreenModesByDisplayId(DisplayId displayId) const
{
    const sptr<ScreenInfo> screenInfo = GetScreenInfoByDisplayId(displayId);
    if (screenInfo == nullptr) {
        WLOGFE("can not get display.");
        return nullptr;
    }
    auto modes = screenInfo->GetModes();
    auto id = screenInfo->GetModeId();
    if (id >= modes.size()) {
        WLOGFE("can not get screenMode.");
        return nullptr;
    }
    return modes[id];
}

std::shared_ptr<Media::PixelMap> DisplayManagerServiceInner::GetDisplaySnapshot(DisplayId displayId) const
{
    return DisplayManagerService::GetInstance().GetDisplaySnapshot(displayId);
}

void DisplayManagerServiceInner::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    DisplayManagerService::GetInstance().RegisterDisplayChangeListener(listener);
}

bool DisplayManagerServiceInner::SetOrientationFromWindow(DisplayId displayId, Orientation orientation)
{
    auto displayInfo = GetDisplayById(displayId);
    if (displayInfo == nullptr) {
        return false;
    }
    return DisplayManagerService::GetInstance().
        SetOrientationFromWindow(displayInfo->GetScreenId(), orientation);
}
} // namespace OHOS::Rosen