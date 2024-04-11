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

#include "session_manager.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"
#include "session_manager_lite.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManager" };
}

class SessionManagerServiceRecoverListener : public IRemoteStub<ISessionManagerServiceRecoverListener> {
public:
    explicit SessionManagerServiceRecoverListener() = default;

    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_CLIENT, "InterfaceToken check failed");
        return -1;
        }
        auto msgId = static_cast<SessionManagerServiceRecoverMessage>(code);
        switch (msgId) {
            case SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER: {
                auto sessionManagerService = data.ReadRemoteObject();
                OnSessionManagerServiceRecover(sessionManagerService);
                break;
            }
            case SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED: {
                int32_t userId = data.ReadInt32();
                int32_t screenId = data.ReadInt32();
                bool isConnected = data.ReadBool();
                OnWMSConnectionChanged(userId, screenId, isConnected);
                break;
            }
            default:
                TLOGW(WmsLogTag::WMS_CLIENT, "unknown transaction code %{public}d", code);
                return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
        return 0;
    }

    void OnSessionManagerServiceRecover(const sptr<IRemoteObject>& sessionManagerService) override
    {
        SessionManager::GetInstance().Clear();
        SessionManager::GetInstance().ClearSessionManagerProxy();

        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        SessionManager::GetInstance().RecoverSessionManagerService(sms);
    }

    void OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected) override
    {
        SessionManager::GetInstance().OnWMSConnectionChanged(userId, screenId, isConnected);
    }
};

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

SessionManager::~SessionManager()
{
    TLOGI(WmsLogTag::WMS_CLIENT, "SessionManager destory!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
    if (mockSessionManagerServiceProxy_ != nullptr) {
        mockSessionManagerServiceProxy_->UnregisterSMSRecoverListener();
        mockSessionManagerServiceProxy_ = nullptr;
    }
}

void SessionManager::OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected)
{
    WLOGFD("OnWMSConnectionChanged");
    isWMSConnected_ = isConnected;
    currentUserId_ = userId;
    currentScreenId_ = screenId;
    if (wmsConnectionChangedFunc_ != nullptr) {
        WLOGFI("WMS connection changed with userId=%{public}d, screenId=%{public}d, isConnected=%{public}d",
            userId, screenId, isConnected);
        wmsConnectionChangedFunc_(userId, screenId, isConnected);
    }
}

void SessionManager::ClearSessionManagerProxy()
{
    TLOGI(WmsLogTag::WMS_CLIENT, "ClearSessionManagerProxy enter!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (destroyed_) {
        TLOGE(WmsLogTag::WMS_CLIENT, "Already destroyed");
        return;
    }
    if (sessionManagerServiceProxy_ != nullptr) {
        int refCount = sessionManagerServiceProxy_->GetSptrRefCount();
        TLOGI(WmsLogTag::WMS_CLIENT, "sessionManagerServiceProxy_ GetSptrRefCount : %{public}d", refCount);
        sessionManagerServiceProxy_ = nullptr;
    }
    sceneSessionManagerProxy_ = nullptr;
}

__attribute__((no_sanitize("cfi")))
sptr<ISceneSessionManager> SessionManager::GetSceneSessionManagerProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    InitSessionManagerServiceProxy();
    InitSceneSessionManagerProxy();
    return sceneSessionManagerProxy_;
}

void SessionManager::InitSessionManagerServiceProxy()
{
    if (sessionManagerServiceProxy_) {
        return;
    }

    if (InitMockSMSProxy() != WMError::WM_OK) {
        TLOGW(WmsLogTag::WMS_CLIENT, "Init mock session manager service proxy failed");
        return;
    }

    RegisterSMSRecoverListener();

    sessionManagerServiceProxy_ = SessionManagerLite::GetInstance().GetSessionManagerServiceProxy();
    if (!sessionManagerServiceProxy_) {
        TLOGE(WmsLogTag::WMS_CLIENT, "sessionManagerServiceProxy_ is nullptr");
    }
}

WMError SessionManager::InitMockSMSProxy()
{
    if (mockSessionManagerServiceProxy_) {
        return WMError::WM_OK;
    }
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        TLOGE(WmsLogTag::WMS_CLIENT, "Failed to get system ability mgr.");
        return WMError::WM_ERROR_NULLPTR;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject) {
        TLOGI(WmsLogTag::WMS_CLIENT, "Remote object is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
    if (!mockSessionManagerServiceProxy_) {
        TLOGW(WmsLogTag::WMS_CLIENT, "Get mock session manager service proxy failed, nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (GetUserIdByUid(getuid()) != SYSTEM_USERID) {
        return WMError::WM_OK;
    }
    if (!foundationDeath_) {
        foundationDeath_ = new (std::nothrow) FoundationDeathRecipient();
        if (!foundationDeath_) {
            TLOGE(WmsLogTag::WMS_CLIENT, "Failed to create death Recipient ptr FoundationDeathRecipient");
            mockSessionManagerServiceProxy_ = nullptr;
            return WMError::WM_ERROR_NO_MEM;
        }
    }
    if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(foundationDeath_)) {
        TLOGE(WmsLogTag::WMS_CLIENT, "Failed to add death recipient");
        mockSessionManagerServiceProxy_ = nullptr;
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return WMError::WM_OK;
}

__attribute__((no_sanitize("cfi")))
void SessionManager::InitSceneSessionManagerProxy()
{
    if (sceneSessionManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        TLOGE(WmsLogTag::WMS_CLIENT, "sessionManagerServiceProxy_ is nullptr");
        return;
    }

    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetSceneSessionManager();
    if (!remoteObject) {
        TLOGW(WmsLogTag::WMS_CLIENT, "Get scene session manager proxy failed, scene session manager service is null");
        return;
    }
    sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject);
    if (sceneSessionManagerProxy_) {
        ssmDeath_ = new (std::nothrow) SSMDeathRecipient();
        if (!ssmDeath_) {
            TLOGE(WmsLogTag::WMS_CLIENT, "Failed to create death Recipient ptr WMSDeathRecipient");
            return;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(ssmDeath_)) {
            TLOGE(WmsLogTag::WMS_CLIENT, "Failed to add death recipient");
            return;
        }
    }
    if (!sceneSessionManagerProxy_) {
        TLOGW(WmsLogTag::WMS_CLIENT, "Get scene session manager proxy failed, nullptr");
    }
}

void SessionManager::RegisterSMSRecoverListener()
{
    if (!mockSessionManagerServiceProxy_) {
        WLOGFE("[WMSRecover] mockSessionManagerServiceProxy_ is null");
        return;
    }
    if (!isRecoverListenerRegistered_) {
        isRecoverListenerRegistered_ = true;
        WLOGFI("[WMSRecover] Register recover listener");
        smsRecoverListener_ = new SessionManagerServiceRecoverListener();
        mockSessionManagerServiceProxy_->RegisterSMSRecoverListener(smsRecoverListener_);
    }
}

void SessionManager::RegisterWindowManagerRecoverCallbackFunc(const WindowManagerRecoverCallbackFunc& callbackFunc)
{
    std::lock_guard<std::recursive_mutex> lock(recoverMutex_);
    windowManagerRecoverFunc_ = callbackFunc;
}

void SessionManager::RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService)
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        sessionManagerServiceProxy_ = sessionManagerService;
    }
    
    {
        std::lock_guard<std::recursive_mutex> lock(recoverMutex_);
        WLOGFI("[WMSRecover] Run recover");
        if (windowManagerRecoverFunc_ != nullptr) {
            WLOGFD("[WMSRecover] windowManagerRecover");
            windowManagerRecoverFunc_();
        }
    }
}

void SessionManager::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((sceneSessionManagerProxy_ != nullptr) && (sceneSessionManagerProxy_->AsObject() != nullptr)) {
        sceneSessionManagerProxy_->AsObject()->RemoveDeathRecipient(ssmDeath_);
    }
}

WMError SessionManager::RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc)
{
    TLOGI(WmsLogTag::WMS_CLIENT, "RegisterWMSConnectionChangedListener in");
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (InitMockSMSProxy() != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_CLIENT, "Init mock session manager service failed");
            return WMError::WM_ERROR_NULLPTR;
        }
        RegisterSMSRecoverListener();
    }
    wmsConnectionChangedFunc_ = callbackFunc;
    if (isWMSConnected_) {
        OnWMSConnectionChanged(currentUserId_, currentScreenId_, true);
    }
    return WMError::WM_OK;
}

void SessionManager::OnFoundationRemoteDied()
{
    TLOGI(WmsLogTag::WMS_CLIENT, "OnFoundationRemoteDied enter");
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        mockSessionManagerServiceProxy_ = nullptr;
        isRecoverListenerRegistered_ = false;
        isWMSConnected_ = false;
    }
    SessionManager::GetInstance().ClearSessionManagerProxy();
}

void FoundationDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    TLOGI(WmsLogTag::WMS_CLIENT, "Foundation died");
    SessionManager::GetInstance().OnFoundationRemoteDied();
}

void SSMDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        TLOGE(WmsLogTag::WMS_CLIENT, "SSMDeathRecipient wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        TLOGE(WmsLogTag::WMS_CLIENT, "SSMDeathRecipient object is null");
        return;
    }
    TLOGI(WmsLogTag::WMS_CLIENT, "ssm OnRemoteDied");
    SessionManager::GetInstance().Clear();
    SessionManager::GetInstance().ClearSessionManagerProxy();
}
} // namespace OHOS::Rosen
