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


#include <accesstoken_kit.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <bundle_mgr_proxy.h>
#include <bundle_mgr_interface.h>
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include <tokenid_kit.h>
#include "common/include/permission.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "Permission"};
}

bool Permission::IsSystemServiceCalling(bool needPrintLog)
{
    Security::AccessToken::NativeTokenInfo tokenInfo;
    Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(IPCSkeleton::GetCallingTokenID(), tokenInfo);
    if (tokenInfo.apl == Security::AccessToken::ATokenAplEnum::APL_SYSTEM_CORE ||
        tokenInfo.apl == Security::AccessToken::ATokenAplEnum::APL_SYSTEM_BASIC) {
        return true;
    }
    if (needPrintLog) {
        WLOGFE("Is not system service calling, native apl: %{public}d", tokenInfo.apl);
    }
    return false;
}

bool Permission::IsSystemCalling()
{
    if (IsSystemServiceCalling(false)) {
        return true;
    }
    uint64_t accessTokenIDEx = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApp = Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(accessTokenIDEx);
    return isSystemApp;
}

bool Permission::IsStartedByInputMethod()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("Failed to get system ability mgr.");
        return false;
    }
    sptr<IRemoteObject> remoteObject
        = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get display manager service.");
        return false;
    }
    auto bundleManagerServiceProxy_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if ((!bundleManagerServiceProxy_) || (!bundleManagerServiceProxy_->AsObject())) {
        WLOGFE("Failed to get system display manager services");
        return false;
    }

    int uid = IPCSkeleton::GetCallingUid();
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    std::string bundleName;
    bundleManagerServiceProxy_->GetNameForUid(uid, bundleName);
    AppExecFwk::BundleInfo bundleInfo;
    // 200000 use uid to caculate userId
    int userId = uid / 200000;
    bool result = bundleManagerServiceProxy_->GetBundleInfo(bundleName,
        AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, userId);
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    if (!result) {
        WLOGFE("failed to query extension ability info");
        return false;
    }

    auto extensionInfo = std::find_if(bundleInfo.extensionInfos.begin(), bundleInfo.extensionInfos.end(),
        [](AppExecFwk::ExtensionAbilityInfo extensionInfo) {
            return (extensionInfo.type == AppExecFwk::ExtensionAbilityType::INPUTMETHOD);
        });
    if (extensionInfo != bundleInfo.extensionInfos.end()) {
        return true;
    } else {
        return false;
    }
}
} // namespace Rosen
} // namespace OHOS