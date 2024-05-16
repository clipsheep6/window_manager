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

#include "window_display_change_adapter.h"
#include "window_manager_hilog.h"
#include "singleton_container.h"
#include "display_info.h"
#include "scene_board_judgement.h"

namespace OHOS {
namespace Rosen {

WindowDisplayChangeAdapter::WindowDisplayChangeAdapter(const sptr<IRemoteObject>& token,
                                                       const sptr<IDisplayInfoChangedListener>& listener)
    : token_(token), listener_(listener)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        TLOGI(WmsLogTag::DMS, "SceneBoard is not useable, register this to DM.");
        DisplayManager::GetInstance().RegisterDisplayListener(this);
    }
}

WindowDisplayChangeAdapter::~WindowDisplayChangeAdapter()
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        TLOGI(WmsLogTag::DMS, "SceneBoard is not useable, unregister this from DM.");
        DisplayManager::GetInstance().UnregisterDisplayListener(this);
    }
}

void WindowDisplayChangeAdapter::OnCreate(DisplayId displayId)
{
    TLOGI(WmsLogTag::DMS, "not need notify OnCreate");
}

void WindowDisplayChangeAdapter::OnDestroy(DisplayId displayId)
{
    TLOGI(WmsLogTag::DMS, "not need notify OnDestroy");
}

void WindowDisplayChangeAdapter::OnChanged(DisplayId displayId)
{
    if (displayInfoChangeListener_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "notify display info failed because of the Listener is nullptr");
        return;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayId(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "get display by displayId: %{public}" PRIu64 " failed.", displayId);
        return;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "get displayInfo by displayId: %{public}" PRIu64 " failed.", displayId);
        return;
    }
    auto density = displayInfo->GetVirtualPixelRatio();
    auto orientation = displayInfo->GetDisplayOrientation();
    if (displayId_ == displayId && orientation_ == orientation && density_ == density) {
        TLOGW(WmsLogTag::DMS, "not need notify display info, data is not changed.");
        return;
    }
    displayId_ = displayId;
    density_ = density;
    orientation_ = orientation;
    displayInfoChangeListener_->OnDisplayInfoChange(token_, displayId, density, orientation);
}

void WindowDisplayChangeAdapter::OnDisplayInfoChange(const sptr<IRemoteObject>& token,
    DisplayId displayId, float density, DisplayOrientation orientation)
{
    if (displayInfoChangeListener_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "notify display info failed because of the Listener is nullptr");
        return;
    }
    if (displayId_ == displayId && density_ == density && orientation_ == orientation) {
        TLOGW(WmsLogTag::DMS, "not need notify display info, data is not changed.");
        return;
    }
    displayId_ = displayId;
    density_ = density;
    orientation_ = orientation;
    displayInfoChangeListener_->OnDisplayInfoChange(token_, displayId, density, orientation);
}

const sptr<IDisplayInfoChangedListener> WindowsDisplayChangeAdapter::GetListener()) const
{
    return displayInfoChangeListener_;
}

} // namespace Rosen
} // namespace OHOS