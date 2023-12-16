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

#include "screen_lite.h"

#include <cstdint>
#include <new>
#include <refbase.h>

#include "class_var_definition.h"
#include "display_manager_adapter_lite.h"
#include "screen_info.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenLite"};
}
class ScreenLite::Impl : public RefBase {
public:
    explicit Impl(sptr<ScreenInfo> info)
    {
        screenInfo_ = info;
    }
    ~Impl() = default;
    DEFINE_VAR_FUNC_GET_SET_WITH_LOCK(sptr<ScreenInfo>, ScreenInfo, screenInfo);
};

ScreenLite::ScreenLite(sptr<ScreenInfo> info)
    : pImpl_(new Impl(info))
{
}

ScreenLite::~ScreenLite()
{
}

bool ScreenLite::IsGroup() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetIsScreenGroup();
}

std::string ScreenLite::GetName() const
{
    return pImpl_->GetScreenInfo()->GetName();
}

ScreenId ScreenLite::GetId() const
{
    return pImpl_->GetScreenInfo()->GetScreenId();
}

uint32_t ScreenLite::GetWidth() const
{
    auto modeId = GetModeId();
    auto modes = GetSupportedModes();
    if (modeId < 0 || modeId >= modes.size()) {
        return 0;
    }
    return modes[modeId]->width_;
}

uint32_t ScreenLite::GetHeight() const
{
    UpdateScreenInfo();
    auto modeId = GetModeId();
    auto modes = GetSupportedModes();
    if (modeId < 0 || modeId >= modes.size()) {
        return 0;
    }
    return modes[modeId]->height_;
}

uint32_t ScreenLite::GetVirtualWidth() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetVirtualWidth();
}

uint32_t ScreenLite::GetVirtualHeight() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetVirtualHeight();
}

float ScreenLite::GetVirtualPixelRatio() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetVirtualPixelRatio();
}

Rotation ScreenLite::GetRotation() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetRotation();
}

Orientation ScreenLite::GetOrientation() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetOrientation();
}

bool ScreenLite::IsReal() const
{
    return pImpl_->GetScreenInfo()->GetType() == ScreenType::REAL;
}

DMError ScreenLite::SetOrientation(Orientation orientation) const
{
    WLOGFD("Orientation %{public}u", orientation);
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetOrientation(GetId(), orientation);
}

ScreenId ScreenLite::GetParentId() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetParentId();
}

uint32_t ScreenLite::GetModeId() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetModeId();
}

std::vector<sptr<SupportedScreenModes>> ScreenLite::GetSupportedModes() const
{
    return pImpl_->GetScreenInfo()->GetModes();
}

DMError ScreenLite::SetScreenActiveMode(uint32_t modeId)
{
    ScreenId screenId = GetId();
    if (modeId >= GetSupportedModes().size()) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetScreenActiveMode(screenId, modeId);
}

void ScreenLite::UpdateScreenInfo(sptr<ScreenInfo> info) const
{
    if (info == nullptr) {
        WLOGFE("ScreenInfo is invalid");
        return;
    }
    pImpl_->SetScreenInfo(info);
}

void ScreenLite::UpdateScreenInfo() const
{
    auto screenInfo = SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenInfo(GetId());
    UpdateScreenInfo(screenInfo);
}

DMError ScreenLite::SetDensityDpi(uint32_t dpi) const
{
    if (dpi > DOT_PER_INCH_MAXIMUM_VALUE || dpi < DOT_PER_INCH_MINIMUM_VALUE) {
        WLOGE("Invalid input dpi value, valid input range for DPI is %{public}u ~ %{public}u",
            DOT_PER_INCH_MINIMUM_VALUE, DOT_PER_INCH_MAXIMUM_VALUE);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    // Calculate display density, Density = Dpi / 160.
    float density = static_cast<float>(dpi) / 160; // 160 is the coefficient between density and dpi.
    return SingletonContainer::Get<ScreenManagerAdapterLite>().SetVirtualPixelRatio(GetId(), density);
}

sptr<ScreenInfo> ScreenLite::GetScreenInfo() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo();
}
} // namespace OHOS::Rosen