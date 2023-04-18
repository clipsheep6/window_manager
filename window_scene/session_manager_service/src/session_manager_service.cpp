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

#include "session_manager_service.h"

#include <system_ability_definition.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    // constexpr HiviewDFX::HiLogLabel LEVEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(SessionManagerService)

SessionManagerService::SessionManagerService()
{
}

int SessionManagerService::GetValueById(int id)
{
    return id + 1;
}

IRemoteObject* SessionManagerService::GetRemoteObject()
{
    return  dynamic_cast<IRemoteObject*>(this);
}

} // namesapce OHOS::Rosen