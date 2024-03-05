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

#include "session/host/include/extension_session_info.h"

namespace OHOS {
namespace Rosen {
void ExtensionSessionInfo::SetParentId(int32_t parentId)
{
    parentId_ = parentId;
}

void ExtensionSessionInfo::SetPersistentId(int32_t persistentId)
{
    persistentId_ = persistentId;
}

void ExtensionSessionInfo::SetVisibility(bool isVisible)
{
    isVisible_ = isVisible;
}

void ExtensionSessionInfo::SetWaterMark(bool isEnable)
{
    isWaterMarkEnable_ = isEnable;
}

int32_t ExtensionSessionInfo::GetParentId() const
{
    return parentId_;
}

int32_t ExtensionSessionInfo::GetPersistenId() const
{
    return persistentId_;
}

bool ExtensionSessionInfo::GetVisibility() const
{
    return isVisible_;
}

bool ExtensionSessionInfo::GetWaterMark() const;
{
    return isWaterMarkEnable_;
}

} // namespace Rosen
} // namespace OHOS