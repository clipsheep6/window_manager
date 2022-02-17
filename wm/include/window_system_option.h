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
#ifndef OHOS_ROSEN_WINDOW_SYSTEM_OPTION_H
#define OHOS_ROSEN_WINDOW_SYSTEM_OPTION_H
#include <map>
#include "window_property.h"
#include "wm_common.h"
namespace OHOS {
namespace Rosen {
using Func_t = void(*)(sptr<WindowProperty> property);
class WindowSystemOption {
public:
    static void SetDefaultOption(sptr<WindowProperty> property);
private:
    static void SetStatusBar(sptr<WindowProperty> property);
    static void SetNavigationBar(sptr<WindowProperty> property);
    static void SetSystemAlarmWindow(sptr<WindowProperty> property);
    static void SetDraggingEffect(sptr<WindowProperty> property);
    static void SetVolumeOverlay(sptr<WindowProperty> property);
    static void SetInputMethod(sptr<WindowProperty> property);
    static void SetTypePointer(sptr<WindowProperty> property);
    static constexpr float SYSTEM_ALARM_WINDOW_WIDTH_RATIO = 0.8;
    static constexpr float SYSTEM_ALARM_WINDOW_HEIGHT_RATIO = 0.3;
    static const std::map<WindowType, Func_t> sysFuncMap_;
};
}
}


#endif