/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_CONFIG_H
#define OHOS_ROSEN_WINDOW_CONFIG_H

#include <climits>
#include <string>
#include <refbase.h>
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
const int WINDOW_CONFIG_INVALID_VALUE = INT_MAX;
const int WINDOW_LOCATION_NUM = 4;
enum ArrIndex : uint32_t {
    first, 
    second, 
    third, 
    fourth,
};

class WindowConfig : public RefBase {
public:
    WindowConfig() = delete;
    ~WindowConfig() = default;

    static uint32_t getMaxFloatingWidth();
    static uint32_t getMaxFloatingHeight();
    static uint32_t getMinFloatingWidth();
    static uint32_t getMinFloatingHeight();
    static uint32_t getDefaultWindowMode();
    static int getFloatingBottomPosY();
    static Rect getDefaultFloatingWindow();

private:
    static int getValue(std::string name);
    static void checkInitConfig();
    static void initDefaultFloatingWindow();

private:
    static bool isInited_;
    static uint32_t maxFloatingWidth_;
    static uint32_t maxFloatingHeight_;
    static uint32_t minFloatingWidth_;
    static uint32_t minFloatingHeight_;
    static uint32_t defaultWindowMode_;
    static int floatingBottomPosY_;
    static Rect defaultFloatingWindow_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_CONFIG_H