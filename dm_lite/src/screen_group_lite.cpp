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

#include "screen_group_lite.h"

#include <new>

#include "class_var_definition.h"
#include "display_manager_adapter_lite.h"
#include "screen_group_info.h"
#include "screen_info.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenGroupLite"};
}
class ScreenGroupLite::Impl : public RefBase {
public:
    explicit Impl(sptr<ScreenGroupInfo> info)
    {
        ScreenGroupInfo_ = info;
    }
    ~Impl() = default;

    DEFINE_VAR_FUNC_GET_SET_WITH_LOCK(sptr<ScreenGroupInfo>, ScreenGroupInfo, ScreenGroupInfo);
};

ScreenGroupLite::ScreenGroupLite(sptr<ScreenGroupInfo> info)
    : ScreenLite(info), pImpl_(new Impl(info))
{
}

void ScreenGroupLite::UpdateScreenGroupInfo(sptr<ScreenGroupInfo> info) const
{
    if (info == nullptr) {
        WLOGFE("ScreenGroupInfo is nullptr.");
        return;
    }
    ScreenLite::UpdateScreenInfo(info);
    pImpl_->SetScreenGroupInfo(info);
}

void ScreenGroupLite::UpdateScreenGroupInfo() const
{
    auto screenInfo = SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenGroupInfoById(GetId());
    UpdateScreenGroupInfo(screenInfo);
}

ScreenGroupLite::~ScreenGroupLite()
{
}

ScreenCombination ScreenGroupLite::GetCombination() const
{
    UpdateScreenGroupInfo();
    return pImpl_->GetScreenGroupInfo()->GetCombination();
}

std::vector<ScreenId> ScreenGroupLite::GetChildIds() const
{
    UpdateScreenGroupInfo();
    return pImpl_->GetScreenGroupInfo()->GetChildren();
}

std::vector<Point> ScreenGroupLite::GetChildPositions() const
{
    UpdateScreenGroupInfo();
    return pImpl_->GetScreenGroupInfo()->GetPosition();
}
} // namespace OHOS::Rosen