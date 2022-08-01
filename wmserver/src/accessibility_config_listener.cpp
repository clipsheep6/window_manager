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

#include "accessibility_config_listener.h"
#include "window_manager_hilog.h"

#include "window_node_container.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AccessibilityConfigListener"};
}
using namespace OHOS::AccessibilityConfig;

std::shared_ptr<AccessibilityConfigListener>& AccessibilityConfigListener::GetSharedInstance() 
{
    static std::shared_ptr<AccessibilityConfigListener> instance{ new AccessibilityConfigListener() };
    return instance;
}

void AccessibilityConfigListener::SubscribeObserver()
{
    static bool subscribeDone = false;
    if (!subscribeDone) {
        WLOGFI("zhirong SubscribeObserver start");
        auto& accessibilityConfig = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
        if (!accessibilityConfig.InitializeContext()) {
            WLOGE("zhirong InitializeContext failed");
            return;
        }
        accessibilityConfig.SubscribeConfigObserver(CONFIG_ANIMATION_OFF, GetSharedInstance());
        // bool animationOff;
        // accessibilityConfig.GetAnimationOffState(animationOff);
        WLOGFI("zhirong SubscribeObserver done");
        subscribeDone = true;
    }
}

void AccessibilityConfigListener::OnConfigChanged(const CONFIG_ID id, const ConfigValue &value)
{
    WLOGFI("zhirong OnConfigChanged");
    switch (id) {
        case CONFIG_ANIMATION_OFF:
            WLOGFI("zhirong animationOff changed to %{public}d", int(value.animationOff));
            //WindowNodeContainer::GetAnimationConfigRef().enable_ = value.animationOff;
            break;      
        default:
            break;
    }
}
}