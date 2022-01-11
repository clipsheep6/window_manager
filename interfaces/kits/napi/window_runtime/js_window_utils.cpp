/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "js_window_utils.h"
#include <iomanip>
#include <regex>
#include <sstream>
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindowUtils"};
    constexpr int RGB_LENGTH = 7;
}

static NativeValue* GetRectAndConvertToJsValue(NativeEngine& engine, const Rect& rect)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert rect to jsObject");
        return nullptr;
    }
    object->SetProperty("left", CreateJsValue(engine, rect.posX_));
    object->SetProperty("top", CreateJsValue(engine, rect.posY_));
    object->SetProperty("width", CreateJsValue(engine, rect.width_));
    object->SetProperty("height", CreateJsValue(engine, rect.height_));
    return objValue;
}

NativeValue* CreateJsWindowPropertiesObject(NativeEngine& engine, sptr<Window>& window)
{
    WLOGFI("JsWindow::CreateJsWindowPropertiesObject is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert windowProperties to jsObject");
        return nullptr;
    }

    Rect rect = window->GetRect();
    NativeValue* rectObj = GetRectAndConvertToJsValue(engine, rect);
    if (rectObj == nullptr) {
        WLOGFE("GetRect failed!");
    }
    object->SetProperty("windowRect", rectObj);
    object->SetProperty("type", CreateJsValue(engine, window->GetType()));
    return objValue;
}

static std::string GetHexColor(uint32_t color)
{
    std::stringstream ioss;
    std::string temp;
    ioss << std::setiosflags(std::ios::uppercase) << std::hex << color;
    ioss >> temp;
    return temp;
}

NativeValue* CreateJsSystemBarRegionTintObject(NativeEngine& engine,
    const SystemBarProperty& prop, WindowType type)
{
    WLOGFI("JsWindow::CreateJsSystemBarRegionTintObject is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert windowProperties to jsObject");
        return nullptr;
    }
    object->SetProperty("type", CreateJsValue(engine, static_cast<uint32_t>(type)));
    object->SetProperty("isEnable", CreateJsValue(engine, prop.enable_));
    std::string bkgColor = GetHexColor(prop.backgroundColor_);
    object->SetProperty("backgroundColor", CreateJsValue(engine, bkgColor));
    std::string contentColor = GetHexColor(prop.contentColor_);
    object->SetProperty("contentColor", CreateJsValue(engine, contentColor));
    return objValue;
}

bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
                        NativeEngine& engine, NativeCallbackInfo& info, sptr<Window>& window)
{
    NativeArray* nativeArray = ConvertNativeValueTo<NativeArray>(info.argv[0]);
    if (nativeArray == nullptr) {
        WLOGFE("Failed to convert parameter to SystemBarArray");
        return false;
    }
    uint32_t size = nativeArray->GetLength();
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    statusProperty.enable_ = false;
    navProperty.enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    for (uint32_t i = 0; i < size; i++) {
        std::string name;
        if (!ConvertFromJsValue(engine, nativeArray->GetElement(i), name)) {
            WLOGFE("Failed to convert parameter to SystemBarName");
            return false;
        }
        if (name.compare("status") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
        } else if (name.compare("navigation") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
        }
    }
    return true;
}

static uint32_t GetBkgColorFromJs(NativeEngine& engine,  NativeObject* jsObject,
    const char* name, SystemBarProperty& property)
{
    NativeValue* jsColor = jsObject->GetProperty(name);
    if (jsColor->TypeOf() != NATIVE_UNDEFINED) {
        std::string colorStr;
        if (!ConvertFromJsValue(engine, jsColor, colorStr)) {
            WLOGFE("Failed to convert parameter to color");
            return property.backgroundColor_;
        }
        std::regex pattern("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
        if (!std::regex_match(colorStr, pattern)) {
            WLOGFE("invalid color input");
            return property.backgroundColor_;
        }
        if (colorStr.length() == RGB_LENGTH) {
            colorStr += "FF"; // RGB + A
        }
        std::string color = colorStr.substr(1);
        std::stringstream ss;
        uint32_t hexColor;
        ss << std::hex << color;
        ss >> hexColor;
        WLOGFI("Final Color is %{public}x", hexColor);
        return hexColor;
    }
    return property.backgroundColor_;
}

bool SetSystemBarPropertiesFromJs(NativeEngine& engine, NativeObject* jsObject,
    std::map<WindowType, SystemBarProperty>& properties, sptr<Window>& window)
{
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColor_ = GetBkgColorFromJs(engine,
        jsObject, "statusBarColor", properties[WindowType::WINDOW_TYPE_STATUS_BAR]);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColor_ = GetBkgColorFromJs(engine,
        jsObject, "navigationBarColor", properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR]);
    NativeValue* jsStatusIcon = jsObject->GetProperty("isStatusBarLightIcon");
    if (jsStatusIcon->TypeOf() != NATIVE_UNDEFINED) {
        bool isStatusBarLightIcon;
        if (!ConvertFromJsValue(engine, jsStatusIcon, isStatusBarLightIcon)) {
            WLOGFE("Failed to convert parameter to isStatusBarLightIcon");
            return false;
        }
        if (isStatusBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
    }

    NativeValue* jsNavigationIcon = jsObject->GetProperty("isNavigationBarLightIcon");
    if (jsNavigationIcon->TypeOf() != NATIVE_UNDEFINED) {
        bool isNavigationBarLightIcon;
        if (!ConvertFromJsValue(engine, jsNavigationIcon, isNavigationBarLightIcon)) {
            WLOGFE("Failed to convert parameter to isNavigationBarLightIcon");
            return false;
        }
        if (isNavigationBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS