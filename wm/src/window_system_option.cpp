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
#include "window_system_option.h"
#include <cinttypes>
#include "display_manager.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowSystemOption"};
}

const std::map<WindowType, Func_t> WindowSystemOption::sysFuncMap_ = {
    std::make_pair(WindowType::WINDOW_TYPE_STATUS_BAR, &WindowSystemOption::SetStatusBar),
    std::make_pair(WindowType::WINDOW_TYPE_NAVIGATION_BAR, &WindowSystemOption::SetNavigationBar),
    std::make_pair(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, &WindowSystemOption::SetSystemAlarmWindow),
    std::make_pair(WindowType::WINDOW_TYPE_DRAGGING_EFFECT, &WindowSystemOption::SetDraggingEffect),
    std::make_pair(WindowType::WINDOW_TYPE_VOLUME_OVERLAY, &WindowSystemOption::SetVolumeOverlay),
    std::make_pair(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT, &WindowSystemOption::SetInputMethod),
    std::make_pair(WindowType::WINDOW_TYPE_POINTER, &WindowSystemOption::SetTypePointer)
};

void WindowSystemOption::SetStatusBar(sptr<WindowProperty> property)
{
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetFocusable(false);
    return;
}

void WindowSystemOption::SetNavigationBar(sptr<WindowProperty> property)
{
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetFocusable(false);
    return;
}

void WindowSystemOption::SetSystemAlarmWindow(sptr<WindowProperty> property)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property->GetDisplayId(),
            property->GetWindowId());
        return;
    }
    uint32_t width = static_cast<uint32_t>(display->GetWidth());
    uint32_t height = static_cast<uint32_t>(display->GetHeight());
    WLOGFI("width:%{public}u, height:%{public}u, displayId:%{public}" PRIu64"",
        width, height, property->GetDisplayId());
    uint32_t alarmWidth = static_cast<uint32_t>((static_cast<float>(width) *
        SYSTEM_ALARM_WINDOW_WIDTH_RATIO));
    uint32_t alarmHeight = static_cast<uint32_t>((static_cast<float>(height) *
        SYSTEM_ALARM_WINDOW_HEIGHT_RATIO));
    Rect rect;
    rect = {static_cast<int32_t>((width - alarmWidth) / 2), static_cast<int32_t>((height - alarmHeight) / 2),
            alarmWidth, alarmHeight };
    property->SetWindowRect(rect);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    return;
}

void WindowSystemOption::SetDraggingEffect(sptr<WindowProperty> property)
{
    property->SetWindowFlags(0);
    return;
}

void WindowSystemOption::SetVolumeOverlay(sptr<WindowProperty> property)
{
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    return;
}

void WindowSystemOption::SetInputMethod(sptr<WindowProperty> property)
{
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    return;
}

void WindowSystemOption::SetTypePointer(sptr<WindowProperty> property)
{
    property->SetFocusable(false);
    return;
}

void WindowSystemOption::SetDefaultOption(sptr<WindowProperty> property)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(property->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64", window id:%{public}u", property->GetDisplayId(),
            property->GetWindowId());
        return;
    }
    uint32_t width = static_cast<uint32_t>(display->GetWidth());
    uint32_t height = static_cast<uint32_t>(display->GetHeight());
    WLOGFI("width:%{public}u, height:%{public}u, displayId:%{public}" PRIu64"",
        width, height, property->GetDisplayId());

    auto it = sysFuncMap_.find(property->GetWindowType());
    if (it != sysFuncMap_.end()) {
        (it->second)(property);
    }
    return;
}
}
}