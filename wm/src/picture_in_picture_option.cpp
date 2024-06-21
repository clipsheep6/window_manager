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

void PipOption::SetPiPControlStatus(PiPControlType controlType, PiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlStatus %{public}u : %{public}u", controlType, status);
    PiPControlStatusInfo newPipControlStatusInfo;
    newPipControlStatusInfo.controlType = controlType;
    newPipControlStatusInfo.status = status;
    for (auto& controlStatusInfo : pipControlStatusInfoList_) {
        TLOGI(WmsLogTag::WMS_PIP, "controlStatus %{public}u : %{public}u",
            controlStatusInfo.controlType, controlStatusInfo.status);
        if (controlType == controlStatusInfo.controlType) {
            controlStatusInfo = newPipControlStatusInfo;
            return;
        }
    }
    pipControlStatusInfoList_.push_back(newPipControlStatusInfo);
}

void PipOption::SetPiPControlEnable(PiPControlType controlType, bool isEnable)
{
    TLOGI(WmsLogTag::WMS_PIP, "SetControlEnable %{public}u : %{public}u", controlType, isEnable);
    PiPControlEnableInfo newPipControlEnableInfo;
    newPipControlEnableInfo.controlType = controlType;
    newPipControlEnableInfo.isEnable = isEnable;
    for (auto& controlEnableInfo : pipControlEnableInfoList_) {
        TLOGI(WmsLogTag::WMS_PIP, "controlEnable %{public}u : %{public}u",
            controlEnableInfo.controlType, controlEnableInfo.isEnable);
        if (controlType == controlEnableInfo.controlType) {
            controlEnableInfo = newPipControlEnableInfo;
            return;
        }
    }
    pipControlEnableInfoList_.push_back(newPipControlEnableInfo);
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

void PipOption::SetNodeControllerRef(napi_ref ref)
{
    customNodeController_ = ref;
}

napi_ref PipOption::GetNodeControllerRef() const
{
    return customNodeController_;
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

std::vector<PiPControlStatusInfo> PipOption::GetControlStatus()
{
    return pipControlStatusInfoList_;
}

std::vector<PiPControlEnableInfo> PipOption::GetControlEnable()
{
    return pipControlEnableInfoList_;
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