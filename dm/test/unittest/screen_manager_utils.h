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

#ifndef FRAMEWORKS_DM_TEST_UT_SCREEN_MANAGER_UTILS_H
#define FRAMEWORKS_DM_TEST_UT_SCREEN_MANAGER_UTILS_H

#include <securec.h>

#include "display.h"
#include "display_manager.h"
#include "dm_common.h"
#include "screen.h"
#include "screen_manager.h"
#include "window_manager_hilog.h"
#include "unique_fd.h"
#include "core/ui/rs_surface_node.h"
#include "core/ui/rs_display_node.h"

namespace OHOS {
namespace Rosen {
class ScreenManagerUtils {
public:
    bool CreateSurface();

    const std::string defaultName_ = "virtualScreen01";
    sptr<Surface> csurface_ = nullptr; // cosumer surface
    sptr<Surface> psurface_ = nullptr; // producer surface
    const float defaultDensity_ = 2.0;
    const int32_t defaultFlags_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // FRAMEWORKS_DM_TEST_UT_SCREEN_MANAGER_UTILS_H