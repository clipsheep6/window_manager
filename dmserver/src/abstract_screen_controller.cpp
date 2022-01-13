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

#include "abstract_screen_controller.h"

#include <cinttypes>
#include <screen_manager/rs_screen_mode_info.h>
#include <screen_manager/screen_types.h>
#include <surface.h>

#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractScreenController"};
}

AbstractScreenController::AbstractScreenController(std::recursive_mutex& mutex)
    : mutex_(mutex), rsInterface_(RSInterfaces::GetInstance())
{
}

AbstractScreenController::~AbstractScreenController()
{
}

void AbstractScreenController::Init()
{
    WLOGFD("screen controller init");
    dmsScreenCount_ = 0;
    rsInterface_.SetScreenChangeCallback(
        std::bind(&AbstractScreenController::OnRsScreenChange, this, std::placeholders::_1, std::placeholders::_2));
}

std::vector<ScreenId> AbstractScreenController::GetAllScreenIds()
{
    std::vector<ScreenId> tmp;
    return tmp;
}

sptr<AbstractScreen> AbstractScreenController::GetAbstractScreen(ScreenId dmsScreenId)
{
    auto iter = dmsScreenMap_.find(dmsScreenId);
    if (iter == dmsScreenMap_.end()) {
        WLOGE("didnot find screen:%{public}" PRIu64"", dmsScreenId);
        return nullptr;
    }
    sptr<AbstractScreen> screen = iter->second;
    return screen;
}

sptr<AbstractScreenGroup> AbstractScreenController::GetAbstractScreenGroup(ScreenId dmsScreenId)
{
    auto iter = dmsScreenGroupMap_.find(dmsScreenId);
    if (iter == dmsScreenGroupMap_.end()) {
        WLOGE("didnot find screen:%{public}" PRIu64"", dmsScreenId);
        return nullptr;
    }
    sptr<AbstractScreenGroup> screen = iter->second;
    return screen;
}

ScreenId AbstractScreenController::ConvertToRsScreenId(ScreenId dmsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return SCREEN_ID_INVALID;
}

ScreenId AbstractScreenController::ConvertToDmsScreenId(ScreenId rsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return SCREEN_ID_INVALID;
}

void AbstractScreenController::RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abstractScreenCallback_ = cb;
}

void AbstractScreenController::OnRsScreenChange(ScreenId rsScreenId, ScreenEvent screenEvent)
{
    WLOGFI("rs screen event. id:%{public}" PRIu64", event:%{public}ud", rsScreenId, static_cast<uint32_t>(screenEvent));
    ScreenId dmsScreenId = INVALID_SCREEN_ID;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (screenEvent == ScreenEvent::CONNECTED) {
        auto iter = rs2DmsScreenIdMap_.find(rsScreenId);
        if (iter == rs2DmsScreenIdMap_.end()) {
            WLOGFD("connect new screen. dmsId:%{public}" PRIu64"", dmsScreenId);
            dmsScreenId = dmsScreenCount_;
            sptr<AbstractScreen> absScreen = new AbstractScreen(dmsScreenId, rsScreenId);
            if (!FillAbstractScreen(absScreen, rsScreenId)) {
                return;
            }
            rs2DmsScreenIdMap_.insert(std::make_pair(rsScreenId, dmsScreenId));
            dms2RsScreenIdMap_.insert(std::make_pair(dmsScreenId, rsScreenId));
            dmsScreenMap_.insert(std::make_pair(dmsScreenId, absScreen));
            dmsScreenCount_++;
            sptr<AbstractScreenGroup> screenGroup = AddToGroupLocked(absScreen);
            if (screenGroup != nullptr && abstractScreenCallback_ != nullptr) {
                abstractScreenCallback_->onConnected_(absScreen);
            }
        } else {
            WLOGE("reconnect screen, screenId=%{public}" PRIu64"", rsScreenId);
        }
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        WLOGI("connect screen");
    } else {
        WLOGE("unknow message:%{public}ud", static_cast<uint8_t>(screenEvent));
    }
}

bool AbstractScreenController::FillAbstractScreen(sptr<AbstractScreen>& absScreen, ScreenId rsScreenId)
{
    std::vector<RSScreenModeInfo> allModes = rsInterface_.GetScreenSupportedModes(rsScreenId);
    if (allModes.size() == 0) {
        WLOGE("supported screen mode is 0, screenId=%{public}" PRIu64"", rsScreenId);
        return false;
    }
    for (RSScreenModeInfo rsScreenModeInfo : allModes) {
        sptr<AbstractScreenInfo> info = new AbstractScreenInfo();
        info->width_ = rsScreenModeInfo.GetScreenWidth();
        info->height_ = rsScreenModeInfo.GetScreenHeight();
        info->freshRate_ = rsScreenModeInfo.GetScreenFreshRate();
        absScreen->infos_.push_back(info);
        WLOGD("fill screen w/h:%{public}d/%{public}d", info->width_, info->height_);
    }
    int32_t activeModeId = rsInterface_.GetScreenActiveMode(rsScreenId).GetScreenModeId();
    WLOGD("fill screen activeModeId:%{public}d", activeModeId);
    if (activeModeId >= allModes.size()) {
        WLOGE("activeModeId exceed, screenId=%{public}" PRIu64", activeModeId:%{public}d/%{public}d",
            rsScreenId, activeModeId, allModes.size());
        return false;
    }
    absScreen->activeIdx_ = activeModeId;
    return true;
}

sptr<AbstractScreenGroup> AbstractScreenController::AddToGroupLocked(sptr<AbstractScreen> newScreen)
{
    if (defaultDmsScreenId_ == SCREEN_ID_INVALID) {
        WLOGE("connect the first screen");
        return AddAsFirstScreenLocked(newScreen);
    } else {
        AddAsSuccedentScreenLocked(newScreen);
        return nullptr;
    }
}

sptr<AbstractScreenGroup> AbstractScreenController::AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen)
{
    // TODO: Create default display
    ScreenId dmsGroupScreenId = dmsScreenCount_++;
    sptr<AbstractScreenGroup> sreenGroup = new AbstractScreenGroup(dmsGroupScreenId, SCREEN_ID_INVALID);
    Point point;
    sreenGroup->AddChild(ScreenCombination::SCREEN_ALONE, newScreen, point);
    newScreen->groupDmsId_ = dmsGroupScreenId;
    auto iter = dmsScreenGroupMap_.find(dmsGroupScreenId);
    if (iter != dmsScreenGroupMap_.end()) {
        WLOGE("group screen existed. id=%{public}" PRIu64"", dmsGroupScreenId);
        dmsScreenGroupMap_.erase(iter);
    }
    dmsScreenGroupMap_.insert(std::make_pair(dmsGroupScreenId, sreenGroup));
    dmsScreenMap_.insert(std::make_pair(dmsGroupScreenId, sreenGroup));
    WLOGI("connect new group screen. id=%{public}" PRIu64"/%{public}" PRIu64", combination:%{public}ud",
        newScreen->dmsId_, dmsGroupScreenId, newScreen->type_);
    return sreenGroup;
}

void AbstractScreenController::AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen)
{
    // TODO: Mirror to default screen
}
} // namespace OHOS::Rosen