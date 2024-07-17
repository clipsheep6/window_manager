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

#include "scene_session_manager_interface.h"

#include "permission.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

WMError ISceneSessionManager::MinimizeAllAppWindows(DisplayId displayId)
{
    if (!Permission::IsLocalSystemCalling() && !Permission::IsLocalStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_LIFE, "Not system app, no right, displayId %{public}" PRIu64, displayId);
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    TLOGE(WmsLogTag::WMS_LIFE, "Not support minimize, displayId %{public}" PRIu64, displayId);
    return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
}

WMError ISceneSessionManager::ToggleShownStateForAllAppWindows()
{
    if (!Permission::IsLocalSystemCalling() && !Permission::IsLocalStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_LIFE, "Not system app, no right");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    TLOGE(WmsLogTag::WMS_LIFE, "Not support call toggleShownState");
    return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
}
}
