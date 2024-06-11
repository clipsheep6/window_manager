/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "picture_in_picture_option.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
PipOption::PipOption()
{
}

void PipOption::SetContext(void* contextPtr)
{
    contextPtr_ = contextPtr;
}

void PipOption::SetNavigationId(const std::string& navigationId)
{
    navigationId_ = navigationId;
}

void PipOption::SetPipTemplate(uint32_t templateType)
{
    templateType_ = templateType;
}

std::vector<PiPControlStatus> PipOption::GetControlStatus()
{
    return pipControlStatus_;
}

void PipOption::SetControlStatus(std::string actionType, uint32_t status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlStatus %{public}s : %{public}u", actionType.c_str(), status);
    PiPControlStatus newPipControlStatus;
    newPipControlStatus.controlName = actionType;
    newPipControlStatus.status = status;
    for (auto&controlStatus : pipControlStatus_) {
        TLOGI(WmsLogTag::WMS_PIP, "controlStatus %{public}s : %{public}u",
              controlStatus.controlName.c_str(), controlStatus.status);
        if (actionType == controlStatus.controlName) {
            controlStatus = newPipControlStatus;
            return
        }
    }
    pipControlStatus_.push_back(newPipControlStatus);
}

void PipOption::SetContentSize(uint32_t width, uint32_t height)
{
    contentWidth_ = width;
    contentHeight_ = height;
}

void PipOption::SetControlGroup(std::vector<std::uint32_t> controlGroup)
{
    controlGroup_ = controlGroup;
}

void* PipOption::GetContext() const
{
    return contextPtr_;
}

std::string PipOption::GetNavigationId() const
{
    return navigationId_;
}

uint32_t PipOption::GetPipTemplate()
{
    return templateType_;
}

void PipOption::GetContentSize(uint32_t& width, uint32_t& height)
{
    width = contentWidth_;
    height = contentHeight_;
}

std::vector<std::uint32_t> PipOption::GetControlGroup()
{
    return controlGroup_;
}

void PipOption::SetXComponentController(std::shared_ptr<XComponentController> xComponentController)
{
    xComponentController_ = xComponentController;
}

std::shared_ptr<XComponentController> PipOption::GetXComponentController()
{
    return xComponentController_;
}
} // namespace Rosen
} // namespace OHOS