/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "display_lite.h"

#include <cstdint>
#include <new>
#include <refbase.h>

#include "class_var_definition.h"
#include "display_info.h"
#include "display_manager_adapter_lite.h"
#include "dm_common.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayLite"};
}
class DisplayLite::Impl : public RefBase {
public:

    Impl(const std::string& name, sptr<DisplayInfo> info)
    {
        name_= name;
        displayInfo_ = info;
    }
    ~Impl() = default;

    DEFINE_VAR_FUNC_GET_SET(std::string, Name, name);
    DEFINE_VAR_FUNC_GET_SET_WITH_LOCK(sptr<DisplayInfo>, DisplayInfo, displayInfo);
};

DisplayLite::DisplayLite(const std::string& name, sptr<DisplayInfo> info)
    : pImpl_(new Impl(name, info))
{
}

DisplayLite::~DisplayLite()
{
}

DisplayId DisplayLite::GetId() const
{
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return DisplayId(0);
    }
    return pImpl_->GetDisplayInfo()->GetDisplayId();
}

std::string DisplayLite::GetName() const
{
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return std::string();
    }
    return pImpl_->GetDisplayInfo()->GetName();
}

int32_t DisplayLite::GetWidth() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetWidth();
}

int32_t DisplayLite::GetHeight() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetHeight();
}

int32_t DisplayLite::GetPhysicalWidth() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetPhysicalWidth();
}

int32_t DisplayLite::GetPhysicalHeight() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetPhysicalHeight();
}

uint32_t DisplayLite::GetRefreshRate() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetRefreshRate();
}

ScreenId DisplayLite::GetScreenId() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return SCREEN_ID_INVALID;
    }
    return pImpl_->GetDisplayInfo()->GetScreenId();
}

Rotation DisplayLite::GetRotation() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return Rotation::ROTATION_0;
    }
    return pImpl_->GetDisplayInfo()->GetRotation();
}

Orientation DisplayLite::GetOrientation() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return Orientation::UNSPECIFIED;
    }
    return pImpl_->GetDisplayInfo()->GetOrientation();
}

void DisplayLite::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const
{
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is invalid");
        return;
    }
    if (pImpl_ == nullptr) {
        WLOGFE("pImpl_ is nullptr");
        return;
    }
    pImpl_->SetDisplayInfo(displayInfo);
}

void DisplayLite::UpdateDisplayInfo() const
{
    auto displayInfo = SingletonContainer::Get<DisplayManagerAdapterLite>().GetDisplayInfo(GetId());
    UpdateDisplayInfo(displayInfo);
}

float DisplayLite::GetVirtualPixelRatio() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return 0;
    }
    return pImpl_->GetDisplayInfo()->GetVirtualPixelRatio();
}

int DisplayLite::GetDpi() const
{
    return static_cast<int>(GetVirtualPixelRatio() * DOT_PER_INCH);
}

sptr<DisplayInfo> DisplayLite::GetDisplayInfo() const
{
    UpdateDisplayInfo();
    if (pImpl_ == nullptr || pImpl_->GetDisplayInfo() == nullptr) {
        WLOGFE("pImpl_ or pImpl_->GetDisplayInfo is nullptr");
        return nullptr;
    }
    return pImpl_->GetDisplayInfo();
}
} // namespace OHOS::Rosen
