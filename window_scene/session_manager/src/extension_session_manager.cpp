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

#include <mutex>

#include <ability_manager_client.h>
#include <hitrace_meter.h>
#include <iservice_registry.h>
#include <session_info.h>
#include <start_options.h>
#include <system_ability_definition.h>

#include "scene_board_judgement.h"
#include "session/host/include/extension_session.h"
#include "session_manager/include/session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSessionManager" };
const std::string EXTENSION_SESSION_MANAGER_THREAD = "OS_ExtensionSessionManager";
std::recursive_mutex g_instanceMutex;
} // namespace

class ExtensionLifecycleListener : public ILifecycleListener {
public:
    explicit ExtensionLifecycleListener(int32_t persistentId) : persistentId_(persistentId) {}
    ~ExtensionLifecycleListener() = default;

    void OnActivation() override {}
    void OnForeground() override {}
    void OnBackground() override {}
    void OnConnect() override {}
    void OnDisconnect() override {}

    void OnExtensionDied() override
    {
        ExtensionSessionManager::GetInstance().OnExtensionDied(persistentId_);
    }

    void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) override {}
private:
    int32_t persistentId_;
};

ExtensionSessionManager& ExtensionSessionManager::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static ExtensionSessionManager* instance = nullptr;
    if (instance == nullptr) {
        instance = new ExtensionSessionManager();
        instance->Init();
    }
    return *instance;
}

void ExtensionSessionManager::Init()
{
    taskScheduler_ = std::make_shared<TaskScheduler>(EXTENSION_SESSION_MANAGER_THREAD);
    InitWms();
}

void ExtensionSessionManager::InitWms()
{
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
            if (!InitSsmProxy()) {
                WLOGFE("InitSSMProxy failed!");
            }
        } else {
            if (!InitWmsProxy()) {
                WLOGFE("InitWMSProxy failed!");
            }
        }
}
bool ExtensionSessionManager::InitWmsProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
        if (!remoteObject) {
            WLOGFE("Failed to get window manager service.");
            return false;
        }

        windowManagerServiceProxy_ = iface_cast<IWindowManager>(remoteObject);
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system window manager services");
            return false;
        }

        wmsDeath_ = new ExtensionWmsDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}
bool ExtensionSessionManager::InitSsmProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        windowManagerServiceProxy_ = SessionManager::GetInstance().GetSceneSessionManagerProxy();
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system scene session manager services");
            return false;
        }

        wmsDeath_ = new (std::nothrow) ExtensionWmsDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        sptr<IRemoteObject> remoteObject = windowManagerServiceProxy_->AsObject();
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

void ExtensionWmsDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    WLOGI("wms OnRemoteDied");
    ExtensionSessionManager::GetInstance().ClearWindowAdapter();
}

void ExtensionSessionManager::ClearWindowAdapter()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((windowManagerServiceProxy_ != nullptr) && (windowManagerServiceProxy_->AsObject() != nullptr)) {
        windowManagerServiceProxy_->AsObject()->RemoveDeathRecipient(wmsDeath_);
    }
    isProxyValid_ = false;
}

void ExtensionSessionManager::OnExtensionDied(int32_t persistentId)
{
    auto iter = extensionSessionMap_.find(persistentId);
    if (iter != extensionSessionMap_.end()) {
        auto extensionSession = iter->second;
        auto property = extensionSession->GetSessionProperty();
        if (property != nullptr) {
            auto parentId = property->GetParentId();
            WLOGFI("OnExtensionDied parentId: %{public}d, persistentId: %{public}d,", parentId, persistentId);
            InitWms();
            windowManagerServiceProxy_->RemoveExtensionSessionInfo(parentId, persistentId);
        }
        UnregisterLifecycleListener(persistentId, extensionSession);
    }
}
void ExtensionSessionManager::RegisterLifecycleListener(int32_t persistentId,
    const sptr<ExtensionSession>& extensionSession)
{
    WLOGFI("RegisterLifecycleListener persistentId: %{public}d,", persistentId);
    std::shared_ptr<ILifecycleListener> lifecycleListener = std::make_shared<ExtensionLifecycleListener>(persistentId);
    extensionSession->RegisterLifecycleListener(lifecycleListener);
    lifecycleListenerMap_.insert({ persistentId, lifecycleListener});
}
void ExtensionSessionManager::UnregisterLifecycleListener(int32_t persistentId,
    const sptr<ExtensionSession>& extensionSession)
{
    WLOGFI("UnregisterLifecycleListener persistentId: %{public}d,", persistentId);
    auto iter = lifecycleListenerMap_.find(persistentId);
    if (iter != lifecycleListenerMap_.end()) {
        auto listener = iter->second;
        extensionSession->UnregisterLifecycleListener(listener);
        lifecycleListenerMap_.erase(persistentId);
    }
}

sptr<AAFwk::SessionInfo> ExtensionSessionManager::SetAbilitySessionInfo(const sptr<ExtensionSession>& extSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (!abilitySessionInfo) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = extSession->GetSessionInfo();
    sptr<ISession> iSession(extSession);
    abilitySessionInfo->sessionToken = iSession->AsObject();
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->parentToken = sessionInfo.rootToken_;
    abilitySessionInfo->persistentId = extSession->GetPersistentId();
    abilitySessionInfo->isAsyncModalBinding = sessionInfo.isAsyncModalBinding_;
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    }
    return abilitySessionInfo;
}

sptr<ExtensionSession> ExtensionSessionManager::RequestExtensionSession(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "RequestExtensionSession");
        sptr<ExtensionSession> extensionSession = new ExtensionSession(sessionInfo);
        auto persistentId = extensionSession->GetPersistentId();
        WLOGFI("persistentId: %{public}d, bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s",
            persistentId, sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(),
            sessionInfo.abilityName_.c_str());
        RegisterLifecycleListener(persistentId, extensionSession);
        extensionSessionMap_.insert({ persistentId, extensionSession });
        return extensionSession;
    };

    return taskScheduler_->PostSyncTask(task, "RequestExtensionSession");
}

WSError ExtensionSessionManager::RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession,
    uint32_t hostWindowId, const std::function<void(WSError)>&& resultCallback)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession, hostWindowId, callback = std::move(resultCallback)]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Activate session with persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "esm:RequestExtensionSessionActivation");
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("RequestExtensionSessionActivation Session is invalid! persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (extSessionInfo == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
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
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
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
    auto task = [this, weakExtSession, callback = std::move(resultCallback)]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Destroy session with persistentId: %{public}d", persistentId);
        UnregisterLifecycleListener(persistentId, extSession);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "esm:RequestExtensionSessionDestruction");
        extSession->Disconnect();
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("RequestExtensionSessionDestruction Session is invalid! persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
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
} // namespace OHOS::Rosen
