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

#include "screen.h"

#include "display_manager_adapter.h"
#include "screen_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "Screen"};
}
class Screen::Impl : public RefBase {
public:
    Impl(sptr<ScreenInfo> info)
    {
        screenInfo_ = info;
    }
    ~Impl() = default;
    DEFINE_VAR_FUNC_GET_SET_WITH_LOCK(sptr<ScreenInfo>, ScreenInfo, screenInfo);
};

Screen::Screen(sptr<ScreenInfo> info)
    : pImpl_(new Impl(info))
{
}

Screen::~Screen()
{
}

bool Screen::IsGroup() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetIsScreenGroup();
}

const std::string Screen::GetName() const
{
    return pImpl_->GetScreenInfo()->GetName();
}

ScreenId Screen::GetId() const
{
    return pImpl_->GetScreenInfo()->GetScreenId();
}

uint32_t Screen::GetWidth() const
{
    auto modeId = GetModeId();
    auto modes = GetSupportedModes();
    if (modeId < 0 || modeId >= modes.size()) {
        return 0;
    }
    return modes[modeId]->width_;
}

uint32_t Screen::GetHeight() const
{
    UpdateScreenInfo();
    auto modeId = GetModeId();
    auto modes = GetSupportedModes();
    if (modeId < 0 || modeId >= modes.size()) {
        return 0;
    }
    return modes[modeId]->height_;
}

uint32_t Screen::GetVirtualWidth() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetVirtualWidth();
}

uint32_t Screen::GetVirtualHeight() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetVirtualHeight();
}

float Screen::GetVirtualPixelRatio() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetVirtualPixelRatio();
}

Rotation Screen::GetRotation() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetRotation();
}

Orientation Screen::GetOrientation() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetOrientation();
}

bool Screen::IsReal() const
{
    return pImpl_->GetScreenInfo()->GetType() == ScreenType::REAL;
}

bool Screen::SetOrientation(Orientation orientation) const
{
    WLOGFD("set orientation %{public}u", orientation);
    return SingletonContainer::Get<ScreenManagerAdapter>().SetOrientation(GetId(), orientation);
}

DMError Screen::GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts) const
{
    return SingletonContainer::Get<ScreenManagerAdapter>().GetScreenSupportedColorGamuts(GetId(), colorGamuts);
}

DMError Screen::GetScreenColorGamut(ScreenColorGamut& colorGamut) const
{
    return SingletonContainer::Get<ScreenManagerAdapter>().GetScreenColorGamut(GetId(), colorGamut);
}

DMError Screen::SetScreenColorGamut(int32_t colorGamutIdx)
{
    return SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorGamut(GetId(), colorGamutIdx);
}

DMError Screen::GetScreenGamutMap(ScreenGamutMap& gamutMap) const
{
    return SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGamutMap(GetId(), gamutMap);
}

DMError Screen::SetScreenGamutMap(ScreenGamutMap gamutMap)
{
    return SingletonContainer::Get<ScreenManagerAdapter>().SetScreenGamutMap(GetId(), gamutMap);
}

DMError Screen::SetScreenColorTransform()
{
    return SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorTransform(GetId());
}

ScreenId Screen::GetParentId() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetParentId();
}

uint32_t Screen::GetModeId() const
{
    UpdateScreenInfo();
    return pImpl_->GetScreenInfo()->GetModeId();
}

std::vector<sptr<SupportedScreenModes>> Screen::GetSupportedModes() const
{
    return pImpl_->GetScreenInfo()->GetModes();
}

bool Screen::SetScreenActiveMode(uint32_t modeId)
{
    ScreenId screenId = GetId();
    if (modeId >= GetSupportedModes().size()) {
        return false;
    }
    return SingletonContainer::Get<ScreenManagerAdapter>().SetScreenActiveMode(screenId, modeId);
}

void Screen::UpdateScreenInfo(sptr<ScreenInfo> info) const
{
    if (info == nullptr) {
        WLOGFE("ScreenInfo is invalid");
        return;
    }
    pImpl_->SetScreenInfo(info);
}

void Screen::UpdateScreenInfo() const
{
    auto screenInfo = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenInfo(GetId());
    UpdateScreenInfo(screenInfo);
}

bool Screen::SetDensityDpi(uint32_t dpi) const
{
    if (dpi > DOT_PER_INCH_MAXIMUM_VALUE || dpi < DOT_PER_INCH_MINIMIM_VALUE) {
        WLOGE("Invalid input dpi value, the valid input range for DPI values is %{public}u ~ %{public}u",
            DOT_PER_INCH_MINIMIM_VALUE, DOT_PER_INCH_MAXIMUM_VALUE);
        return false;
    }
    // Calculate display density, Density = Dpi / 160.
    float density = static_cast<float>(dpi) / 160; // 160 is the coefficient between density and dpi.
    return SingletonContainer::Get<ScreenManagerAdapter>().SetVirtualPixelRatio(GetId(), density);
}
} // namespace OHOS::Rosen