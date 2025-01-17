/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "screen_setting_helper.h"

#include "window_manager_hilog.h"
#include "setting_provider.h"
#include "system_ability_definition.h"
#include <parameters.h>

namespace OHOS {
namespace Rosen {
sptr<SettingObserver> ScreenSettingHelper::dpiObserver_;
sptr<SettingObserver> ScreenSettingHelper::castObserver_;
sptr<SettingObserver> ScreenSettingHelper::rotationObserver_;
sptr<SettingObserver> ScreenSettingHelper::halfScreenObserver_;
sptr<SettingObserver> ScreenSettingHelper::screenSkipProtectedWindowObserver_;
constexpr int32_t PARAM_NUM_TEN = 10;
constexpr uint32_t EXPECT_SCREEN_MODE_SIZE = 2;
constexpr uint32_t EXPECT_RELATIVE_POSITION_SIZE = 3;
constexpr uint32_t DATA_SIZE_INVALID = 0xffffffff;
const std::string SCREEN_SHAPE = system::GetParameter("const.window.screen_shape", "0:0");
const std::string SCREEN_SHARE_PROTECT_TABLE = "USER_SETTINGDATA_SECURE_";

void ScreenSettingHelper::RegisterSettingDpiObserver(SettingObserver::UpdateFunc func)
{
    if (dpiObserver_) {
        TLOGD(WmsLogTag::DMS, "setting dpi observer is registered");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    dpiObserver_ = provider.CreateObserver(SETTING_DPI_KEY, func);
    if (dpiObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
    ErrCode ret = provider.RegisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        dpiObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingDpiObserver()
{
    if (dpiObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "dpiObserver_ is nullptr");
        return;
    }
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.UnregisterObserver(dpiObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
    }
    dpiObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingDpi(uint32_t& dpi, const std::string& key)
{
    return GetSettingValue(dpi, key);
}

bool ScreenSettingHelper::GetSettingValue(uint32_t& value, const std::string& key)
{
    SettingProvider& settingData = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    int32_t getValue;
    ErrCode ret = settingData.GetIntValue(key, getValue);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    value = static_cast<uint32_t>(getValue);
    return true;
}

bool ScreenSettingHelper::GetSettingValue(const std::string& key, std::string& value)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    std::string getValue = "";
    ErrCode ret = provider.GetStringValue(key, getValue);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    value = getValue;
    return true;
}

bool ScreenSettingHelper::SetSettingValue(const std::string& key, uint32_t value)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.PutIntValue(key, value, false);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
    return true;
}

bool ScreenSettingHelper::SetSettingValue(const std::string& key, const std::string& value)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.PutStringValue(key, value, false);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
    return true;
}

bool ScreenSettingHelper::SetSettingDefaultDpi(uint32_t& dpi, const std::string& key)
{
    SettingProvider& provider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = provider.PutIntValue(key, dpi, false);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "put int value failed, ret=%{public}d", ret);
        return false;
    }
    return true;
}

void ScreenSettingHelper::RegisterSettingCastObserver(SettingObserver::UpdateFunc func)
{
    if (castObserver_) {
        TLOGD(WmsLogTag::DMS, "setting cast observer is registered");
        return;
    }
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    castObserver_ = castProvider.CreateObserver(SETTING_CAST_KEY, func);
    if (castObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
    ErrCode ret = castProvider.RegisterObserver(castObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        castObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingCastObserver()
{
    if (castObserver_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "castObserver_ is nullptr");
        return;
    }
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = castProvider.UnregisterObserver(castObserver_);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
    }
    castObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingCast(bool& enable, const std::string& key)
{
    SettingProvider& castProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = castProvider.GetBoolValue(key, enable);
    if (ret != ERR_OK) {
        TLOGW(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    return true;
}

void ScreenSettingHelper::RegisterSettingRotationObserver(SettingObserver::UpdateFunc func)
{
    if (rotationObserver_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "setting rotation observer is registered");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    rotationObserver_ = settingProvider.CreateObserver(SETTING_ROTATION_KEY, func);
    if (rotationObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
    ErrCode ret = settingProvider.RegisterObserver(rotationObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        rotationObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingRotationObserver()
{
    if (rotationObserver_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "rotationObserver_ is nullptr");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.UnregisterObserver(rotationObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
    }
    rotationObserver_ = nullptr;
}

void ScreenSettingHelper::SetSettingRotation(int32_t rotation)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.PutIntValue(SETTING_ROTATION_KEY, rotation, true);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return;
    }
    TLOGE(WmsLogTag::DMS, "succeed, ret:%{public}d", ret);
}

bool ScreenSettingHelper::GetSettingRotation(int32_t& rotation, const std::string& key)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValue(key, rotation);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
    TLOGE(WmsLogTag::DMS, "current rotation:%{public}d", rotation);
    return true;
}

void ScreenSettingHelper::SetSettingRotationScreenId(int32_t screenId)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.PutIntValue(SETTING_ROTATION_SCREEN_ID_KEY, screenId, false);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return;
    }
    TLOGE(WmsLogTag::DMS, "succeed, ret:%{public}d", ret);
}

bool ScreenSettingHelper::GetSettingRotationScreenID(int32_t& screenId, const std::string& key)
{
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValue(key, screenId);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        return false;
    }
    TLOGE(WmsLogTag::DMS, "current rotation screen id:%{public}d", screenId);
    return true;
}

std::string ScreenSettingHelper::RemoveInvalidChar(const std::string& input)
{
    TLOGI(WmsLogTag::DMS, "input string: %{public}s", input.c_str());
    std::string resultString = "";
    for (char character : input) {
        if (std::isdigit(character) || character == ' ' || character == ',' || character == '.') {
            resultString += character;
        }
    }
    return resultString;
}

bool ScreenSettingHelper::SplitString(std::vector<std::string>& splitValues, const std::string& input,
    char delimiter)
{
    TLOGI(WmsLogTag::DMS, "input string: %{public}s", input.c_str());
    if (input.size() == 0) {
        TLOGE(WmsLogTag::DMS, "noting in input string");
        return false;
    }
    std::stringstream stream(input);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        splitValues.push_back(token);
    }
    if (splitValues.size() == 0) {
        TLOGE(WmsLogTag::DMS, "failed, noting split");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::IsNumber(const std::string& str)
{
    int32_t length = static_cast<int32_t>(str.size());
    int32_t start = 0;
    while (start < length && isspace(str[start])) {
        start++;
    }
    bool hasDot = false;
    bool hasDigit = false;
    if (start < length && str[start] == '-') {
        start++;
    }
    for (int32_t i = start; i < length; ++i) {
        if (str[i] == '.') {
            if (hasDot) {
                return false;
            }
            hasDot = true;
        } else if (isdigit(str[i])) {
            hasDigit = true;
        } else {
            return false;
        }
    }
    return hasDigit;
}

uint32_t ScreenSettingHelper::GetDataFromString(MultiScreenRecoverOption& option, const std::string& inputString)
{
    TLOGI(WmsLogTag::DMS, "begin, input string: %{public}s", inputString.c_str());
    std::vector<std::string> splitValues;
    char delimiter = ' ';
    SplitString(splitValues, inputString, delimiter);
    std::string value;
    uint32_t dataSize = splitValues.size();
    uint32_t index = 0;
    if (index < dataSize) {
        value = splitValues[index];
        if (!IsNumber(value)) {
            TLOGE(WmsLogTag::DMS, "not number");
            return DATA_SIZE_INVALID;
        } else {
            option.screenId_ = static_cast<ScreenId>(strtoll(value.c_str(), nullptr, PARAM_NUM_TEN));
        }
        index++;
    }
    if (index < dataSize) {
        value = splitValues[index];
        if (!IsNumber(value)) {
            TLOGE(WmsLogTag::DMS, "not number");
            return DATA_SIZE_INVALID;
        } else {
            option.first_ = static_cast<uint32_t>(strtoll(value.c_str(), nullptr, PARAM_NUM_TEN));
        }
        index++;
    }
    if (index < dataSize) {
        value = splitValues[index];
        if (!IsNumber(value)) {
            TLOGE(WmsLogTag::DMS, "not number");
            return DATA_SIZE_INVALID;
        } else {
            option.second_ = static_cast<uint32_t>(strtoll(value.c_str(), nullptr, PARAM_NUM_TEN));
        }
    }

    TLOGI(WmsLogTag::DMS, "number of split data: %{public}d", dataSize);
    return dataSize;
}

bool ScreenSettingHelper::GetSettingRecoveryResolutionString(std::vector<std::string>& resolutionString,
    const std::string& key)
{
    std::string value;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValueMultiUser(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    std::string validString = RemoveInvalidChar(value);
    bool isSplit = SplitString(resolutionString, validString);
    if (!isSplit) {
        TLOGE(WmsLogTag::DMS, "split failed");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingRecoveryResolutionSet(std::set<ScreenId>& restoredScreenId)
{
    std::vector<std::string> restoredScreenIdStrings;
    bool getString = GetSettingRecoveryResolutionString(restoredScreenIdStrings);
    if (!getString) {
        TLOGE(WmsLogTag::DMS, "get string failed");
        return false;
    }
    for (auto& screenIdString : restoredScreenIdStrings) {
        MultiScreenRecoverOption screenIdData;
        uint32_t dataSize = GetDataFromString(screenIdData, screenIdString);
        if (dataSize == DATA_SIZE_INVALID || dataSize == 0) {
            TLOGE(WmsLogTag::DMS, "get data failed");
            continue;
        }
        ScreenId screenId = screenIdData.screenId_;
        restoredScreenId.insert(screenId);
        TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64, screenId);
    }
    if (restoredScreenId.empty()) {
        TLOGE(WmsLogTag::DMS, "nothing found");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingScreenModeString(std::vector<std::string>& screenModeStrings,
    const std::string& key)
{
    std::string value;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValueMultiUser(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    std::string validString = RemoveInvalidChar(value);
    bool isSplit = SplitString(screenModeStrings, validString);
    if (!isSplit) {
        TLOGE(WmsLogTag::DMS, "split failed");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingScreenModeMap(std::map<ScreenId, uint32_t>& screenMode)
{
    std::vector<std::string> screenModeStrings;
    bool getString = GetSettingScreenModeString(screenModeStrings);
    if (!getString) {
        TLOGE(WmsLogTag::DMS, "get string failed");
        return false;
    }
    for (auto& screenModeString : screenModeStrings) {
        MultiScreenRecoverOption screenModeData;
        uint32_t dataSize = GetDataFromString(screenModeData, screenModeString);
        if (dataSize != EXPECT_SCREEN_MODE_SIZE) {
            TLOGE(WmsLogTag::DMS, "get data failed");
            continue;
        }
        ScreenId screenId = screenModeData.screenId_;
        uint32_t mode = screenModeData.first_;
        TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 ", mode: %{public}d",
            screenId, mode);
        screenMode[screenId] = mode;
    }
    if (screenMode.empty()) {
        TLOGE(WmsLogTag::DMS, "nothing found");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingRelativePositionString(std::vector<std::string>& relativePositionStrings,
    const std::string& key)
{
    std::string value;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetStringValueMultiUser(key, value);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    std::string validString = RemoveInvalidChar(value);
    bool isSplit = SplitString(relativePositionStrings, validString);
    if (!isSplit) {
        TLOGE(WmsLogTag::DMS, "split failed");
        return false;
    }
    return true;
}

bool ScreenSettingHelper::GetSettingRelativePositionMap
    (std::map<ScreenId, std::pair<uint32_t, uint32_t>>& relativePosition)
{
    std::vector<std::string> relativePositionStrings;
    bool getString = GetSettingRelativePositionString(relativePositionStrings);
    if (!getString) {
        TLOGE(WmsLogTag::DMS, "get string failed");
        return false;
    }
    for (auto& relativePositionString : relativePositionStrings) {
        MultiScreenRecoverOption relativePositionData;
        uint32_t dataSize = GetDataFromString(relativePositionData, relativePositionString);
        if (dataSize != EXPECT_RELATIVE_POSITION_SIZE) {
            TLOGE(WmsLogTag::DMS, "get data failed");
            continue;
        }
        ScreenId screenId = relativePositionData.screenId_;
        uint32_t startX = relativePositionData.first_;
        uint32_t startY = relativePositionData.second_;
        TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 ", startX: %{public}d, startY: %{public}d",
            screenId, startX, startY);
        relativePosition[screenId] = std::make_pair(startX, startY);
    }
    if (relativePosition.empty()) {
        TLOGE(WmsLogTag::DMS, "nothing found");
        return false;
    }
    return true;
}

ScreenShape ScreenSettingHelper::GetScreenShape(ScreenId screenId)
{
    std::istringstream iss(SCREEN_SHAPE);
    std::string id;
    std::string shape;
    while (std::getline(iss, id, ':')) {
        std::getline(iss, shape, ';');
        if (screenId == static_cast<ScreenId>(std::stoi(id))) {
            return static_cast<ScreenShape>(std::stoi(shape));
        }
    }
    TLOGI(WmsLogTag::DMS, "Can not find screen shape info. ccm:%{public}s", SCREEN_SHAPE.c_str());
    return ScreenShape::RECTANGLE;
}

void ScreenSettingHelper::RegisterSettingHalfScreenObserver(SettingObserver::UpdateFunc func)
{
    if (halfScreenObserver_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "setting halfScreen observer is registered");
        return;
    }
    SettingProvider& halfScreenProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    halfScreenObserver_ = halfScreenProvider.CreateObserver(SETTING_HALF_SCREEN_SWITCH_KEY, func);
    ErrCode ret = halfScreenProvider.RegisterObserver(halfScreenObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "halfScreen failed, ret:%{public}d", ret);
        halfScreenObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingHalfScreenObserver()
{
    if (halfScreenObserver_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "halfScreenObserver_ is nullptr");
        return;
    }
    SettingProvider& halfScreenProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = halfScreenProvider.UnregisterObserver(halfScreenObserver_);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
    }
    halfScreenObserver_ = nullptr;
}

bool ScreenSettingHelper::GetHalfScreenSwitchState(const std::string& key)
{
    SettingProvider& halfScreenProvider = SettingProvider::GetInstance(DEVICE_STANDBY_SERVICE_SYSTEM_ABILITY_ID);
    int dbValue = 0;
    ErrCode ret = halfScreenProvider.GetIntValue(key, dbValue);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "GetHalfScreenSwitchState failed, ret:%{public}d", ret);
        return false;
    }
    return !dbValue;
}

void ScreenSettingHelper::RegisterSettingscreenSkipProtectedWindowObserver(SettingObserver::UpdateFunc func)
{
    if (screenSkipProtectedWindowObserver_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "setting rotation observer is registered");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    screenSkipProtectedWindowObserver_ = settingProvider.CreateObserver(SETTING_SCREEN_SHARE_PROTECT_KEY, func);
    if (screenSkipProtectedWindowObserver_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "create observer failed");
    }
    ErrCode ret = settingProvider.RegisterObserverByTable(screenSkipProtectedWindowObserver_,
        SCREEN_SHARE_PROTECT_TABLE);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
        screenSkipProtectedWindowObserver_ = nullptr;
    }
}

void ScreenSettingHelper::UnregisterSettingscreenSkipProtectedWindowObserver()
{
    if (screenSkipProtectedWindowObserver_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "rotationObserver_ is nullptr");
        return;
    }
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.UnregisterObserverByTable(screenSkipProtectedWindowObserver_,
        SCREEN_SHARE_PROTECT_TABLE);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret:%{public}d", ret);
    }
    screenSkipProtectedWindowObserver_ = nullptr;
}

bool ScreenSettingHelper::GetSettingscreenSkipProtectedWindow(bool& enable, const std::string& key)
{
    int32_t value = 0;
    SettingProvider& settingProvider = SettingProvider::GetInstance(DISPLAY_MANAGER_SERVICE_SA_ID);
    ErrCode ret = settingProvider.GetIntValueMultiUserByTable(key, value, SCREEN_SHARE_PROTECT_TABLE);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "failed, ret=%{public}d", ret);
        return false;
    }
    enable = (value == 1);
    return true;
}
} // namespace Rosen
} // namespace OHOS
