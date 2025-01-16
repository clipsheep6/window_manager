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

#include "session_manager/include/extension_session_manager.h"

#include <ability_manager_client.h>
#include <hitrace_meter.h>

#include "display_info.h"
#include "display_manager.h"
#include "singleton_container.h"

#include "session/host/include/extension_session.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSessionManager" };
const std::string EXTENSION_SESSION_MANAGER_THREAD = "OS_ExtensionSessionManager";
} // namespace

ExtensionSessionManager::ExtensionSessionManager()
{
    taskScheduler_ = std::make_shared<TaskScheduler>(EXTENSION_SESSION_MANAGER_THREAD);
}

WM_IMPLEMENT_SINGLE_INSTANCE(ExtensionSessionManager)

sptr<AAFwk::SessionInfo> ExtensionSessionManager::SetAbilitySessionInfo(const sptr<ExtensionSession>& extensionSession)
{
    const auto& sessionInfo = extensionSession->GetSessionInfo();
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    abilitySessionInfo->sessionToken = sptr<ISession>(extensionSession)->AsObject();
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->parentToken = sessionInfo.rootToken_;
    abilitySessionInfo->persistentId = extensionSession->GetPersistentId();
    abilitySessionInfo->realHostWindowId = sessionInfo.realParentId_;
    abilitySessionInfo->isAsyncModalBinding = sessionInfo.isAsyncModalBinding_;
    abilitySessionInfo->uiExtensionUsage = static_cast<AAFwk::UIExtensionUsage>(sessionInfo.uiExtensionUsage_);
    abilitySessionInfo->parentWindowType = sessionInfo.parentWindowType_;
    abilitySessionInfo->displayId = sessionInfo.config_.displayId_;
    abilitySessionInfo->density = sessionInfo.config_.density_;
    abilitySessionInfo->orientation = sessionInfo.config_.orientation_;
    abilitySessionInfo->isDensityFollowHost = sessionInfo.config_.isDensityFollowHost_;
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    }
    return abilitySessionInfo;
}

float ExtensionSessionManager::GetSystemDensity(uint64_t displayId)
{
    float vpr = 1.0f;
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "display is null");
        return vpr;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "displayInfo is null");
        return vpr;
    }
    return displayInfo->GetVirtualPixelRatio();
}

sptr<ExtensionSession> ExtensionSessionManager::RequestExtensionSession(const SessionInfo& sessionInfo)
{
    auto task = [this, newSessionInfo = sessionInfo]() mutable -> sptr<ExtensionSession> {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "RequestExtensionSession");
        if (!newSessionInfo.config_.isDensityFollowHost_) {
            newSessionInfo.config_.density_ = GetSystemDensity(newSessionInfo.config_.displayId_);
        }
        sptr<ExtensionSession> extensionSession = sptr<ExtensionSession>::MakeSptr(newSessionInfo);
        extensionSession->SetEventHandler(taskScheduler_->GetEventHandler(), nullptr);
        auto persistentId = extensionSession->GetPersistentId();
        if (persistentId == INVALID_SESSION_ID) {
            return nullptr;
        }
        TLOGNI(WmsLogTag::WMS_UIEXT,
            "persistentId: %{public}d, bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s, "
            "isDensityFollowHost_: %{public}d, density_: %{public}f",
            persistentId, newSessionInfo.bundleName_.c_str(), newSessionInfo.moduleName_.c_str(),
            newSessionInfo.abilityName_.c_str(), newSessionInfo.config_.isDensityFollowHost_,
            newSessionInfo.config_.density_);
        extensionSessionMap_.insert({ persistentId, extensionSession });
        return extensionSession;
    };

    return taskScheduler_->PostSyncTask(task, "RequestExtensionSession");
}

WSError ExtensionSessionManager::RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession,
    uint32_t hostWindowId, const std::function<void(WSError)>&& resultCallback)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession, hostWindowId, callback = std::move(resultCallback), where = __func__]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        TLOGNI(WmsLogTag::WMS_UIEXT, "Activate session with persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "esm:RequestExtensionSessionActivation");
        if (extensionSessionMap_.count(persistentId) == 0) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "%{public}s Session is invalid! persistentId:%{public}d",
                where, persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        extSessionInfo->hostWindowId = hostWindowId;
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIExtensionAbility(extSessionInfo,
            AAFwk::DEFAULT_INVAL_VALUE);
        if (callback) {
            auto ret = errorCode == ERR_OK ? WSError::WS_OK : WSError::WS_ERROR_START_UI_EXTENSION_ABILITY_FAILED;
            callback(ret);
            return ret;
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "RequestExtensionSessionActivation");
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession,
    const std::function<void(WSError)>&& resultCallback)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession, callback = std::move(resultCallback)]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Background session with persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "esm:RequestExtensionSessionBackground");
        extSession->SetActive(false);
        extSession->Background();
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("RequestExtensionSessionBackground Session is invalid! persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIExtensionAbility(extSessionInfo);
        if (callback) {
            auto ret = errorCode == ERR_OK ? WSError::WS_OK : WSError::WS_ERROR_MIN_UI_EXTENSION_ABILITY_FAILED;
            callback(ret);
            return ret;
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "RequestExtensionSessionBackground");
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession,
    const std::function<void(WSError)>&& resultCallback)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession, callback = std::move(resultCallback),
                 where = __func__]() NO_THREAD_SAFETY_ANALYSIS {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        TLOGNI(WmsLogTag::WMS_UIEXT, "Destroy session with persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "esm:RequestExtensionSessionDestruction");
        extSession->Disconnect();
        if (extensionSessionMap_.count(persistentId) == 0) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "%{public}s Session is invalid! persistentId:%{public}d",
                where, persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->TerminateUIExtensionAbility(extSessionInfo);
        extensionSessionMap_.erase(persistentId);
        if (callback) {
            auto ret = errorCode == ERR_OK ? WSError::WS_OK : WSError::WS_ERROR_TERMINATE_UI_EXTENSION_ABILITY_FAILED;
            callback(ret);
            return ret;
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "RequestExtensionSessionDestruction");
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionDestructionDone(const sptr<ExtensionSession>& extensionSession)
{
    const char* const where = __func__;
    auto task = [this, where, weakExtSession = wptr<ExtensionSession>(extensionSession)] {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "%{public}s session is nullptr", where);
            return;
        }
        auto persistentId = extSession->GetPersistentId();
        TLOGNI(WmsLogTag::WMS_UIEXT, "Destroy session done with persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "esm:%{public}s", where);
        if (extensionSessionMap_.count(persistentId) == 0) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "%{public}s session is invalid! persistentId: %{public}d",
                where, persistentId);
            return;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        AAFwk::AbilityManagerClient::GetInstance()->TerminateUIExtensionAbility(extSessionInfo);
        extensionSessionMap_.erase(persistentId);
    };
    taskScheduler_->PostAsyncTask(task, __func__);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
