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

#include "fold_screen_controller/fold_screen_policy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOGCORE, HILOG_DOMAIN_DISPLAY, "FoldScreenPolicy"};
} // namespace
FoldScreenPolicy::FoldScreenPolicy() = default;
FoldScreenPolicy::~FoldScreenPolicy() = default;

void FoldScreenPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode) {}

FoldDisplayMode FoldScreenPolicy::GetScreenPolicy()
{
    return lastDisplayMode_;
}

void FoldScreenPolicy::LockDisplayStatus(bool locked)
{
    lockDisplayStatus_ = locked;
}

FoldStatus FoldScreenPolicy::GetFoldStatus()
{
    return lastFoldStatus_;
}

void FoldScreenPolicy::SetFoldStatus(FoldStatus foldStatus)
{
    WLOGI("SetFoldStatus FoldStatus: %{public}d", foldStatus);
    currentFoldStatus_ = foldStatus;
    lastFoldStatus_ = foldStatus;
}

void FoldScreenPolicy::SendSensorResult(FoldStatus foldStatus) {};
ScreenId FoldScreenPolicy::GetCurrentScreenId() { return screenId_; }

sptr<FoldCreaseRegion> FoldScreenPolicy::GetCurrentFoldCreaseRegion()
{
    return currentFoldCreaseRegion_;
}

void FoldScreenPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    onBootAnimation_ = onBootAnimation;
}

void FoldScreenPolicy::UpdateForPhyScreenPropertyChange() {}

void FoldScreenPolicy::ClearState()
{
    currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    currentFoldStatus_ = FoldStatus::UNKNOWN;
}
} // namespace OHOS::Rosen