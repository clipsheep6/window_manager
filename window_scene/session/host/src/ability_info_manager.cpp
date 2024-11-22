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

#include "session/host/include/ability_info_manager.h"
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

AbilityInfoManager& AbilityInfoManager::GetInstance()
{
    static AbilityInfoManager instance;
    return instance;
}

void AbilityInfoManager::Init(const sptr<AppExecFwk::IBundleMgr>& bundleMgr)
{
    if (bundleMgr == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "bundleMgr is nullptr");
        return;
    }
    bundleMgr_ = bundleMgr;
}

void AbilityInfoManager::SetCurrentUserId(int32_t userId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "set userId %{public}d", userId);
    userId_ = userId;
}

bool AbilityInfoManager::IsAnco(const std::string& bundleName, const std::string& abilityName,
    const std::string& moduleName)
{
    bool isAnco = false;
    auto iter = appInfoMap_.find(bundleName);
    if (iter == appInfoMap_.end()) {
        std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
        if (abilityInfo == nullptr || bundleMgr_ == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "abilityInfo or bundleMgr is nullptr!");
            return isAnco;
        }
        AAFwk::Want want;
        want.SetElementName("", bundleName, abilityName, moduleName);
        auto abilityInfoFlag = (AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
            AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
            AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA);
        TLOGI(WmsLogTag::WMS_LIFE, "bundleName: %{public}s, abilityName: %{public}s, moduleName: %{public}s, "
            "userId: %{public}d, abilityInfoFlag: %{public}d", bundleName.c_str(), abilityName.c_str(),
            moduleName.c_str(), userId_, abilityInfoFlag);
        bool ret = bundleMgr_->QueryAbilityInfo(want, abilityInfoFlag, userId_, *abilityInfo);
        if (!ret) {
            TLOGE(WmsLogTag::WMS_LIFE, "Get ability info from BMS failed!");
            return isAnco;
        }
        appInfoMap_[bundleName] = abilityInfo->applicationInfo;
        TLOGI(WmsLogTag::WMS_LIFE, "codePath: %{public}s", abilityInfo->applicationInfo.codePath.c_str());
        isAnco = abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE) ||
            abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE);
    } else {
        TLOGI(WmsLogTag::WMS_LIFE, "applicationInfo already in appInfoMap_, codePath: %{public}s",
            abilityInfo->applicationInfo.codePath.c_str());
        isAnco = iter->second.codePath == std::to_string(CollaboratorType::RESERVE_TYPE) ||
            iter->second.codePath == std::to_string(CollaboratorType::OTHERS_TYPE);
    }
    return isAnco;
}

void AbilityInfoManager::RefreshAppInfo(const std::string& bundleName)
{
    std::unique_lock<std::shared_mutex> lock(appInfoMutex_);
    appInfoMap_.erase(bundleName);
}
} // namespace OHOS::Rosen