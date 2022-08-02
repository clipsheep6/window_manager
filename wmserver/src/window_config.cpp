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

#include "window_config.h"
#include <wm_common_inner.h>
#include "window_manager_hilog.h"
#include "window_manager_config.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowConfig"};
}

bool WindowConfig::isInited_(false);
uint32_t WindowConfig::maxFloatingWidth_(WINDOW_CONFIG_INVALID_VALUE);
uint32_t WindowConfig::maxFloatingHeight_(WINDOW_CONFIG_INVALID_VALUE);
uint32_t WindowConfig::minFloatingWidth_(WINDOW_CONFIG_INVALID_VALUE);
uint32_t WindowConfig::minFloatingHeight_(WINDOW_CONFIG_INVALID_VALUE);
int WindowConfig::floatingBottomPosY_(WINDOW_CONFIG_INVALID_VALUE);
uint32_t WindowConfig::defaultWindowMode_(WINDOW_CONFIG_INVALID_VALUE);
Rect WindowConfig::defaultFloatingWindow_;

uint32_t WindowConfig::getMaxFloatingWidth()
{
    checkInitConfig();
    return maxFloatingWidth_ == WINDOW_CONFIG_INVALID_VALUE ? MAX_FLOATING_SIZE : maxFloatingWidth_;
}

uint32_t WindowConfig::getMaxFloatingHeight()
{
    checkInitConfig();
    return maxFloatingHeight_ == WINDOW_CONFIG_INVALID_VALUE ? MAX_FLOATING_SIZE : maxFloatingHeight_;
}

uint32_t WindowConfig::getMinFloatingWidth()
{
    checkInitConfig();
    return minFloatingWidth_ == WINDOW_CONFIG_INVALID_VALUE ? MIN_VERTICAL_FLOATING_WIDTH : minFloatingWidth_;
}

uint32_t WindowConfig::getMinFloatingHeight()
{
    checkInitConfig();
    return minFloatingHeight_ == WINDOW_CONFIG_INVALID_VALUE ? MIN_VERTICAL_FLOATING_HEIGHT : minFloatingHeight_;
}

int WindowConfig::getFloatingBottomPosY()
{
    checkInitConfig();
    return floatingBottomPosY_;
}

uint32_t WindowConfig::getDefaultWindowMode()
{
    checkInitConfig();
    return defaultWindowMode_;
}

void WindowConfig::checkInitConfig()
{
    if (isInited_) {
        return;
    }

    isInited_ = true;
    initDefaultFloatingWindow();
    maxFloatingWidth_ = static_cast<uint32_t>(getValue("maxFloatingWidth"));
    maxFloatingHeight_ = static_cast<uint32_t>(getValue("maxFloatingHeight"));
    minFloatingWidth_ = static_cast<uint32_t>(getValue("minFloatingWidth"));
    minFloatingHeight_ = static_cast<uint32_t>(getValue("minFloatingHeight"));
    defaultWindowMode_ = static_cast<uint32_t>(getValue("defaultWindowMode"));
    floatingBottomPosY_ = getValue("floatingBottomPosY");
    WLOGFI("[WmConfig] checkInitConfig %{public}u, %{public}u, %{public}u, %{public}u, %{public}u, %{public}d",
        maxFloatingWidth_, maxFloatingHeight_, minFloatingWidth_, minFloatingHeight_, defaultWindowMode_, floatingBottomPosY_);
}

int WindowConfig::getValue(std::string name)
{
    const auto& numbersConfig = WindowManagerConfig::GetConfig();
    auto values = numbersConfig.find(name);
    if (values == numbersConfig.end()) {
        WLOGFI("[WmConfig] WindowConfig::getValue %{public}s null.", name.c_str());
        return WINDOW_CONFIG_INVALID_VALUE;
    }
    if (values->second.size() == 1) {
        return values->second[0];
    }
    WLOGFE("[WmConfig] WindowConfig::getValue %{public}s size wrong.", name.c_str());
    return WINDOW_CONFIG_INVALID_VALUE;
}

void WindowConfig::initDefaultFloatingWindow()
{
    defaultFloatingWindow_.posX_ = 0;
    defaultFloatingWindow_.posY_ = 0;
    defaultFloatingWindow_.width_ = 0;
    defaultFloatingWindow_.height_ = 0;
    const auto& numbersConfig = WindowManagerConfig::GetConfig();
    auto values = numbersConfig.find("defaultFloatingWindow");
    if (values == numbersConfig.end()) {
        WLOGFI("[WmConfig] WindowConfig::initDefaultFloatingWindow null.");
        return;
    }
    if (values->second.size() != WINDOW_LOCATION_NUM) {
        WLOGFE("[WmConfig] WindowConfig::initDefaultFloatingWindowsize wrong.");
        return;
    }
    defaultFloatingWindow_.posX_ = values->second[first];
    defaultFloatingWindow_.posY_ = values->second[second];
    defaultFloatingWindow_.width_ = values->second[third];
    defaultFloatingWindow_.height_ = values->second[fourth];
    WLOGFI("[WmConfig] initDefaultFloatingWindow [%{public}d, %{public}d, %{public}d, %{public}d]",
        defaultFloatingWindow_.posX_, defaultFloatingWindow_.posY_,
        defaultFloatingWindow_.width_, defaultFloatingWindow_.height_);
}

Rect WindowConfig::getDefaultFloatingWindow()
{
    checkInitConfig();
    return defaultFloatingWindow_;
}
} // namespace Rosen
} // namespace OHOS