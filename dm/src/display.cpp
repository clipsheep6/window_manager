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

#include "display.h"
#include "display_info.h"
#include "display_manager_adapter.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "Display"};
    constexpr int32_t LARGE_SCREEN_WIDTH = 2560;
    constexpr int32_t PHONE_SCREEN_WIDTH = 2376;
}
class Display::Impl : public RefBase {
public:
    Impl(const std::string& name, sptr<DisplayInfo> info)
    {
        name_= name;
        displayInfo_ = info;
    }
    ~Impl() = default;
    DEFINE_VAR_FUNC_GET_SET(std::string, Name, name);
    DEFINE_VAR_FUNC_GET_SET(sptr<DisplayInfo>, DisplayInfo, displayInfo);
};

Display::Display(const std::string& name, sptr<DisplayInfo> info)
    : pImpl_(new Impl(name, info))
{
}

Display::~Display()
{
}

DisplayId Display::GetId() const
{
    return pImpl_->GetDisplayInfo()->GetDisplayId();
}

int32_t Display::GetWidth() const
{
    UpdateDisplayInfo();
    return pImpl_->GetDisplayInfo()->GetWidth();
}

int32_t Display::GetHeight() const
{
    UpdateDisplayInfo();
    return pImpl_->GetDisplayInfo()->GetHeight();
}

uint32_t Display::GetFreshRate() const
{
    UpdateDisplayInfo();
    return pImpl_->GetDisplayInfo()->GetFreshRate();
}

ScreenId Display::GetScreenId() const
{
    UpdateDisplayInfo();
    return pImpl_->GetDisplayInfo()->GetScreenId();
}

Rotation Display::GetRotation() const
{
    UpdateDisplayInfo();
    return pImpl_->GetDisplayInfo()->GetRotation();
}

Orientation Display::GetOrientation() const
{
    UpdateDisplayInfo();
    return pImpl_->GetDisplayInfo()->GetOrientation();
}

void Display::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const
{
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is invalid");
        return;
    }
    pImpl_->SetDisplayInfo(displayInfo);
}

void Display::UpdateDisplayInfo() const
{
    auto displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(GetId());
    UpdateDisplayInfo(displayInfo);
}

float Display::GetVirtualPixelRatio() const
{
    // Should get from DMS
    if ((pImpl_->GetDisplayInfo()->GetWidth() >= PHONE_SCREEN_WIDTH)
        || (pImpl_->GetDisplayInfo()->GetHeight() >= PHONE_SCREEN_WIDTH)) {
        if ((pImpl_->GetDisplayInfo()->GetWidth() == LARGE_SCREEN_WIDTH)
            || (pImpl_->GetDisplayInfo()->GetHeight() == LARGE_SCREEN_WIDTH)) {
            return 2.0f;
        }
        return 3.0f;
    } else {
        return 2.0f;
    }
}
} // namespace OHOS::Rosen