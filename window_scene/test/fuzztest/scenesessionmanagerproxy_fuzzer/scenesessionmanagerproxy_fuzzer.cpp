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

#include "scenesessionmanagerproxy_fuzzer.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "session_manager.h"
#include "zidl/scene_session_manager_proxy.h"
#include "zidl/mock_session_manager_service_interface.h"
#include "window_manager_hilog.h"
#include "data_source.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerProxyFuzzTest"};
}

std::pair<sptr<ISceneSessionManager>, sptr<IRemoteObject>> GetProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("Failed to get system ability mgr");
        return {nullptr, nullptr};
    }

    sptr<IRemoteObject> remoteObject1 = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject1) {
        WLOGFE("Remote object1 is nullptr");
        return {nullptr, nullptr};
    }
    auto mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject1);
    if (!mockSessionManagerServiceProxy_) {
        WLOGFW("Get mock session manager service proxy failed, nullptr");
        return {nullptr, nullptr};
    }

    sptr<IRemoteObject> remoteObject2 = mockSessionManagerServiceProxy_->GetSessionManagerService();
    if (!remoteObject2) {
        WLOGFE("Remote object2 is nullptr");
        return {nullptr, nullptr};
    }
    auto sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(remoteObject2);
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
        return {nullptr, nullptr};
    }

    sptr<IRemoteObject> remoteObject3 = sessionManagerServiceProxy_->GetSceneSessionManager();
    if (!remoteObject3) {
        WLOGFE("Get scene session manager proxy failed, scene session manager service is nullptr");
        return {nullptr, nullptr};
    }
    auto sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject3);
    if (!sceneSessionManagerProxy_) {
        WLOGFE("Get scene session manager proxy failed, nullptr");
        return {nullptr, nullptr};
    }

    return {sceneSessionManagerProxy_, remoteObject3};
}

void IPCFuzzTest(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);
    uint32_t code = source.GetObject<uint32_t>();
    int flags = source.GetObject<int>();
    int waitTime = source.GetObject<int>();

    MessageParcel sendData;
    MessageParcel reply;
    MessageOption option(flags, waitTime);
    auto rawSize = source.size_ - source.pos_;
    auto buf = source.GetRaw(rawSize);
    if (buf) {
        sendData.WriteBuffer(buf, rawSize);
    }
    remoteObject->SendRequest(code, sendData, reply, option);
}

void IPCSpecificInterfaceFuzzTest(sptr<IRemoteObject> proxy, MessageParcel& sendData, MessageParcel& reply,
    MessageOption& option)
{
#define SEND_REQUEST(msg) \
    proxy->SendRequest(static_cast<uint32_t>(ISceneSessionManager::msg), sendData, reply, option)

    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UPDATE_PROPERTY);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_INFO);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_LABEL);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_ICON);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_IS_VALID_SESSION_IDS);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_CHANGE_LISTENER);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_CHANGE_LISTENER);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_CHECK_WINDOW_ID);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_WINDOW_INFO);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_LISTENER);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_MISSION_INFOS);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_MISSION_INFO_BY_ID);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_DUMP_SESSION_ALL);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_DUMP_SESSION_WITH_ID);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_TERMINATE_SESSION_NEW);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_DUMP_INFO);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_SNAPSHOT);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_BIND_DIALOG_TARGET);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_NOTIFY_DUMP_INFO_RESULT);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_GRAVITY);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_CLEAR_SESSION);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_CLEAR_ALL_SESSIONS);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_LOCK_SESSION);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UNLOCK_SESSION);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_REGISTER_COLLABORATOR);
    SEND_REQUEST(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_COLLABORATOR);

#undef SEND_REQUEST
}

void IPCInterfaceFuzzTest(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);
    int flags = source.GetObject<int>();
    int waitTime = source.GetObject<int>();

    MessageParcel sendData;
    MessageParcel reply;
    MessageOption option(flags, waitTime);
    sendData.WriteInterfaceToken(proxy->GetDescriptor());
    auto rawSize = source.size_ - source.pos_;
    auto buf = source.GetRaw(rawSize);
    if (buf) {
        sendData.WriteBuffer(buf, rawSize);
    }
    IPCSpecificInterfaceFuzzTest(remoteObject, sendData, reply, option);
}

void ProxyInterfaceFuzzTestPart1(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    sptr<ISessionStage> sessionStage = nullptr;
    sptr<IWindowEventChannel> eventChannel = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
    sptr<IRemoteObject> token = nullptr;
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    int32_t persistentId = 0;
    sptr<ISession> session = nullptr;

    proxy->CreateAndConnectSpecificSession(sessionStage, eventChannel, surfaceNode,
        property, persistentId, session, token);

    persistentId = source.GetObject<int32_t>();
    proxy->DestroyAndDisconnectSpecificSession(persistentId);

    WSPropertyChangeAction action = source.GetObject<WSPropertyChangeAction>();
    proxy->UpdateProperty(property, action);

    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    WindowManagerAgentType type = source.GetObject<WindowManagerAgentType>();
    proxy->RegisterWindowManagerAgent(type, windowManagerAgent);
    proxy->UnregisterWindowManagerAgent(type, windowManagerAgent);

    FocusChangeInfo focusInfo;
    proxy->GetFocusWindowInfo(focusInfo);

    std::string label = source.GetString();
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    proxy->SetSessionLabel(token, label);
    proxy->SetSessionIcon(token, pixelMap);

    std::vector<int32_t> sessionIds{
        source.GetObject<int32_t>(), source.GetObject<int32_t>(), source.GetObject<int32_t>()};
    std::vector<bool> results;
    proxy->IsValidSessionIds(sessionIds, results);
}

void ProxyInterfaceFuzzTestPart2(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    sptr<ISessionChangeListener> sessionChangeListener = nullptr;
    proxy->RegisterSessionListener(sessionChangeListener);
    proxy->UnregisterSessionListener();

    sptr<IRemoteObject> token = nullptr;
    proxy->PendingSessionToForeground(token);
    proxy->PendingSessionToBackgroundForDelegator(token);
    proxy->GetFocusSessionToken(token);

    int32_t pid;
    int32_t windowId = source.GetObject<int32_t>();
    proxy->CheckWindowId(windowId, pid);

    bool enable = source.GetObject<bool>();
    proxy->SetGestureNavigaionEnabled(enable);

    std::vector<sptr<AccessibilityWindowInfo>> windowInfos;
    proxy->GetAccessibilityWindowInfo(windowInfos);

    sptr<ISessionListener> sessionListener = nullptr;
    proxy->RegisterSessionListener(sessionListener);
    proxy->UnRegisterSessionListener(sessionListener);

    std::string deviceId = source.GetString();
    int32_t numMax = source.GetObject<int32_t>();
    std::vector<SessionInfoBean> sessionInfos;
    proxy->GetSessionInfos(deviceId, numMax, sessionInfos);

    deviceId = source.GetString();
    int32_t persistentId = source.GetObject<int32_t>();
    SessionInfoBean sessionInfo;
    proxy->GetSessionInfo(deviceId, persistentId, sessionInfo);

    std::vector<std::string> infos;
    persistentId = source.GetObject<int32_t>();
    proxy->DumpSessionAll(infos);
    proxy->DumpSessionWithId(persistentId, infos);
}

void ProxyInterfaceFuzzTestPart3(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    bool needStartCaller = source.GetObject<bool>();
    proxy->TerminateSessionNew(abilitySessionInfo, needStartCaller);

    bool haveListener = source.GetObject<bool>();
    int32_t persistentId = source.GetObject<int32_t>();
    proxy->UpdateAvoidAreaListener(persistentId, haveListener);

    std::string result;
    std::vector<std::string> params{source.GetString(), source.GetString(), source.GetString()};
    proxy->GetSessionDumpInfo(params, result);

    std::string deviceId = source.GetString();
    persistentId = source.GetObject<int32_t>();
    bool isLowResolution = source.GetObject<bool>();
    std::shared_ptr<SessionSnapshot> snapshot = std::make_shared<SessionSnapshot>();
    proxy->GetSessionSnapshot(deviceId, persistentId, *snapshot, isLowResolution);

    sptr<IRemoteObject> token = nullptr;
    uint64_t persistentId2 = source.GetObject<uint64_t>();
    proxy->BindDialogTarget(persistentId2, token);

    std::vector<std::string> info{source.GetString(), source.GetString(), source.GetString()};
    proxy->NotifyDumpInfoResult(info);

    ContinueState continueState = source.GetObject<ContinueState>();
    proxy->SetSessionContinueState(token, continueState);
}

void ProxyInterfaceFuzzTestPart4(const uint8_t* data, size_t size)
{
    auto [proxy, remoteObject] = GetProxy();
    if (!proxy || !remoteObject) {
        return;
    }

    DataSource source(data, size);

    int32_t persistentId = source.GetObject<int32_t>();
    SessionGravity gravity = source.GetObject<SessionGravity>();
    uint32_t percent = source.GetObject<uint32_t>();
    proxy->SetSessionGravity(persistentId, gravity, percent);

    persistentId = source.GetObject<int32_t>();
    proxy->ClearSession(persistentId);
    proxy->ClearAllSessions();

    int32_t sessionId = source.GetObject<int32_t>();
    proxy->LockSession(sessionId);
    proxy->UnlockSession(sessionId);

    std::vector<std::int32_t> sessionIds{
        source.GetObject<int32_t>(), source.GetObject<int32_t>(), source.GetObject<int32_t>()};
    int32_t topSessionId = source.GetObject<int32_t>();
    std::vector<int32_t> result;
    proxy->MoveSessionsToForeground(sessionIds, topSessionId);
    proxy->MoveSessionsToBackground(sessionIds, result);

    int32_t type = source.GetObject<int32_t>();
    sptr<AAFwk::IAbilityManagerCollaborator> impl = nullptr;
    proxy->RegisterIAbilityManagerCollaborator(type, impl);
    proxy->UnregisterIAbilityManagerCollaborator(type);
}

void ProxyInterfaceFuzzTest(const uint8_t* data, size_t size)
{
    ProxyInterfaceFuzzTestPart1(data, size);
    ProxyInterfaceFuzzTestPart2(data, size);
    ProxyInterfaceFuzzTestPart3(data, size);
    ProxyInterfaceFuzzTestPart4(data, size);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::IPCFuzzTest(data, size);
    OHOS::Rosen::IPCInterfaceFuzzTest(data, size);
    OHOS::Rosen::ProxyInterfaceFuzzTest(data, size);
    return 0;
}
