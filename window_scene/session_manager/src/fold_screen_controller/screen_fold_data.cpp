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

#include "fold_screen_controller/screen_fold_data.h"

#include "typec_port_info.h"

namespace OHOS::Rosen {
void ScreenFoldData::SetInvalid()
{
    currentScreenFoldStatus_ = ScreenFoldData::INVALID_VALUE;
}

bool ScreenFoldData::GetTypeCThermalWithUtil()
{
    return TypeCPortInfo::GetTypeCThermal(typeCThermal_);
}

void ScreenFoldData::SetFocusedPkgName(const std::string& packageName)
{
    focusedPackageName_ = packageName;
}
} // namespace OHOS::Rosen