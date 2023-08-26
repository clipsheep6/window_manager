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

#include "js_scene_utils.h"
#include "js_scene_session.h"

#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSession" };
const std::string PENDING_SCENE_CB = "pendingSceneSessionActivation";
const std::string SESSION_STATE_CHANGE_CB = "sessionStateChange";
const std::string SESSION_EVENT_CB = "sessionEvent";
const std::string SESSION_RECT_CHANGE_CB = "sessionRectChange";
const std::string CREATE_SPECIFIC_SCENE_CB = "createSpecificSession";
const std::string BIND_DIALOG_TARGET_CB = "bindDialogTarget";
const std::string RAISE_TO_TOP_CB = "raiseToTop";
const std::string BACK_PRESSED_CB = "backPressed";
const std::string SESSION_FOCUSABLE_CHANGE_CB = "sessionFocusableChange";
const std::string SESSION_TOUCHABLE_CHANGE_CB = "sessionTouchableChange";
const std::string CLICK_CB = "click";
const std::string TERMINATE_SESSION_CB = "terminateSession";
const std::string TERMINATE_SESSION_CB_NEW = "terminateSessionNew";
const std::string TERMINATE_SESSION_CB_TOTAL = "terminateSessionTotal";
const std::string UPDATE_SESSION_LABEL_CB = "updateSessionLabel";
const std::string UPDATE_SESSION_ICON_CB = "updateSessionIcon";
const std::string SESSION_EXCEPTION_CB = "sessionException";
const std::string SYSTEMBAR_PROPERTY_CHANGE_CB = "systemBarPropertyChange";
const std::string NEED_AVOID_CB = "needAvoid";
const std::string PENDING_SESSION_TO_FOREGROUND_CB = "pendingSessionToForeground";
const std::string PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB = "pendingSessionToBackgroundForDelegator";
const std::string NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB = "needDefaultAnimationFlagChange";
const std::string CUSTOM_ANIMATION_PLAYING_CB = "isCustomAnimationPlaying";
const std::string SHOW_WHEN_LOCKED_CB = "sessionShowWhenLockedChange";
const std::string REQUESTED_ORIENTATION_CHANGE_CB = "sessionRequestedOrientationChange";
const std::string RAISE_ABOVE_TARGET_CB = "raiseAboveTarget";
} // namespace

NativeValue* JsSceneSession::Create(NativeEngine& engine, const sptr<SceneSession>& session)
{
    WLOGI("[NAPI]Create");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr || session == nullptr) {
        WLOGFE("[NAPI]Object or session is null!");
        return engine.CreateUndefined();
    }

    std::unique_ptr<JsSceneSession> jsSceneSession = std::make_unique<JsSceneSession>(engine, session);
    object->SetNativePointer(jsSceneSession.release(), JsSceneSession::Finalizer, nullptr);
    object->SetProperty("persistentId", CreateJsValue(engine, static_cast<int32_t>(session->GetPersistentId())));
    object->SetProperty("parentId", CreateJsValue(engine, static_cast<int32_t>(session->GetParentPersistentId())));
    object->SetProperty("type", CreateJsValue(engine, static_cast<uint32_t>(GetApiType(session->GetWindowType()))));
    object->SetProperty("isAppType", CreateJsValue(engine, session->IsFloatingWindowAppType()));

    const char* moduleName = "JsSceneSession";
    BindNativeFunction(engine, *object, "on", moduleName, JsSceneSession::RegisterCallback);
    BindNativeFunction(engine, *object, "updateNativeVisibility", moduleName, JsSceneSession::UpdateNativeVisibility);
    BindNativeFunction(engine, *object, "setShowRecent", moduleName, JsSceneSession::SetShowRecent);
    BindNativeFunction(engine, *object, "setZOrder", moduleName, JsSceneSession::SetZOrder);
    BindNativeFunction(engine, *object, "setPrivacyMode", moduleName, JsSceneSession::SetPrivacyMode);

    return objValue;
}

JsSessionType JsSceneSession::GetApiType(WindowType type)
{
    auto iter = WINDOW_TO_JS_SESSION_TYPE_MAP.find(type);
    if (iter == WINDOW_TO_JS_SESSION_TYPE_MAP.end()) {
        WLOGFE("[NAPI]window type: %{public}u cannot map to api type!", type);
        return JsSessionType::TYPE_UNDEFINED;
    } else {
        return iter->second;
    }
}

JsSceneSession::JsSceneSession(NativeEngine& engine, const sptr<SceneSession>& session)
    : engine_(engine), weakSession_(session)
{
    listenerFunc_ = {
        { PENDING_SCENE_CB,               &JsSceneSession::ProcessPendingSceneSessionActivationRegister },
        { SESSION_STATE_CHANGE_CB,        &JsSceneSession::ProcessSessionStateChangeRegister },
        { SESSION_EVENT_CB,               &JsSceneSession::ProcessSessionEventRegister },
        { SESSION_RECT_CHANGE_CB,         &JsSceneSession::ProcessSessionRectChangeRegister },
        { CREATE_SPECIFIC_SCENE_CB,       &JsSceneSession::ProcessCreateSpecificSessionRegister },
        { BIND_DIALOG_TARGET_CB,       &JsSceneSession::ProcessBindDialogTargetRegister },
        { RAISE_TO_TOP_CB,                &JsSceneSession::ProcessRaiseToTopRegister },
        { BACK_PRESSED_CB,                &JsSceneSession::ProcessBackPressedRegister },
        { SESSION_FOCUSABLE_CHANGE_CB,    &JsSceneSession::ProcessSessionFocusableChangeRegister },
        { SESSION_TOUCHABLE_CHANGE_CB,    &JsSceneSession::ProcessSessionTouchableChangeRegister },
        { CLICK_CB,                       &JsSceneSession::ProcessClickRegister },
        { TERMINATE_SESSION_CB,           &JsSceneSession::ProcessTerminateSessionRegister },
        { TERMINATE_SESSION_CB_NEW,       &JsSceneSession::ProcessTerminateSessionRegisterNew },
        { TERMINATE_SESSION_CB_TOTAL,           &JsSceneSession::ProcessTerminateSessionRegisterTotal },
        { SESSION_EXCEPTION_CB,           &JsSceneSession::ProcessSessionExceptionRegister },
        { UPDATE_SESSION_LABEL_CB,        &JsSceneSession::ProcessUpdateSessionLabelRegister },
        { UPDATE_SESSION_ICON_CB,         &JsSceneSession::ProcessUpdateSessionIconRegister },
        { SYSTEMBAR_PROPERTY_CHANGE_CB,   &JsSceneSession::ProcessSystemBarPropertyChangeRegister },
        { NEED_AVOID_CB,          &JsSceneSession::ProcessNeedAvoidRegister },
        { PENDING_SESSION_TO_FOREGROUND_CB,           &JsSceneSession::ProcessPendingSessionToForegroundRegister },
        { PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB,
            &JsSceneSession::ProcessPendingSessionToBackgroundForDelegatorRegister },
        { NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB, &JsSceneSession::ProcessSessionDefaultAnimationFlagChangeRegister },
        { CUSTOM_ANIMATION_PLAYING_CB,                  &JsSceneSession::ProcessIsCustomAnimationPlaying },
        { SHOW_WHEN_LOCKED_CB,            &JsSceneSession::ProcessShowWhenLockedRegister },
        { REQUESTED_ORIENTATION_CHANGE_CB,            &JsSceneSession::ProcessRequestedOrientationChange },
        { RAISE_ABOVE_TARGET_CB,          &JsSceneSession::ProcessRaiseAboveTargetRegister }
    };

    sptr<SceneSession::SessionChangeCallback> sessionchangeCallback = new (std::nothrow)
        SceneSession::SessionChangeCallback();
    if (sessionchangeCallback != nullptr) {
        if (session != nullptr) {
            session->RegisterSessionChangeCallback(sessionchangeCallback);
        }
        sessionchangeCallback_ = sessionchangeCallback;
        WLOGFD("RegisterSessionChangeCallback success");
    }
}

JsSceneSession::~JsSceneSession()
{
    WLOGD("~JsSceneSession");
}

void JsSceneSession::ProcessSessionDefaultAnimationFlagChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onWindowAnimationFlagChange_ = std::bind(
        &JsSceneSession::OnDefaultAnimationFlagChange, this, std::placeholders::_1);
    WLOGFD("ProcessSessionDefaultAnimationFlagChangeRegister success");
}

void JsSceneSession::OnDefaultAnimationFlagChange(bool isNeedDefaultAnimationFlag)
{
    WLOGFI("[NAPI]OnDefaultAnimationFlagChange, flag: %{public}u", isNeedDefaultAnimationFlag);
    auto iter = jsCbMap_.find(NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [isNeedDefaultAnimationFlag, jsCallBack, eng = &engine_](
            NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionDefaultAnimationFlagObj = CreateJsValue(engine, isNeedDefaultAnimationFlag);
            NativeValue* argv[] = { jsSessionDefaultAnimationFlagObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnDefaultAnimationFlagChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::ProcessPendingSceneSessionActivationRegister()
{
    NotifyPendingSessionActivationFunc func = [this](SessionInfo& info) {
        this->PendingSessionActivation(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetPendingSessionActivationEventListener(func);
    WLOGFD("ProcessPendingSceneSessionActivationRegister success");
}

void JsSceneSession::ProcessSessionStateChangeRegister()
{
    NotifySessionStateChangeFunc func = [this](const SessionState& state) {
        this->OnSessionStateChange(state);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionStateChangeListenser(func);
    WLOGFD("ProcessSessionStateChangeRegister success");
}

void JsSceneSession::ProcessCreateSpecificSessionRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onCreateSpecificSession_ = std::bind(&JsSceneSession::OnCreateSpecificSession,
        this, std::placeholders::_1);
    WLOGFD("ProcessCreateSpecificSessionRegister success");
}

void JsSceneSession::ProcessBindDialogTargetRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onBindDialogTarget_ = std::bind(&JsSceneSession::OnBindDialogTarget,
        this, std::placeholders::_1);
    WLOGFD("ProcessBindDialogTargetRegister success");
}

void JsSceneSession::ProcessSessionRectChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRectChange_ = std::bind(&JsSceneSession::OnSessionRectChange,
        this, std::placeholders::_1, std::placeholders::_2);

    auto session = weakSession_.promote();
    if (session && session->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        OnSessionRectChange(session->GetSessionRequestRect());
    }
    WLOGFD("ProcessSessionRectChangeRegister success");
}

void JsSceneSession::ProcessRaiseToTopRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRaiseToTop_ = std::bind(&JsSceneSession::OnRaiseToTop, this);
    WLOGFD("ProcessRaiseToTopRegister success");
}

void JsSceneSession::ProcessRaiseAboveTargetRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRaiseAboveTarget_ = std::bind(&JsSceneSession::OnRaiseAboveTarget,
        this, std::placeholders::_1);
    WLOGFD("ProcessRaiseToTopRegister success");
}

void JsSceneSession::ProcessSessionEventRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnSessionEvent_ = std::bind(&JsSceneSession::OnSessionEvent, this, std::placeholders::_1);
    WLOGFD("ProcessSessionEventRegister success");
}

void JsSceneSession::ProcessTerminateSessionRegister()
{
    WLOGFD("begin to run ProcessTerminateSessionRegister");
    NotifyTerminateSessionFunc func = [this](const SessionInfo& info) {
        this->TerminateSession(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListener(func);
    WLOGFD("ProcessTerminateSessionRegister success");
}

void JsSceneSession::ProcessTerminateSessionRegisterNew()
{
    WLOGFD("begin to run ProcessTerminateSessionRegisterNew");
    NotifyTerminateSessionFuncNew func = [this](const SessionInfo& info, bool needStartCaller) {
        this->TerminateSessionNew(info, needStartCaller);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListenerNew(func);
    WLOGFD("ProcessTerminateSessionRegisterNew success");
}

void JsSceneSession::ProcessTerminateSessionRegisterTotal()
{
    WLOGFD("begin to run ProcessTerminateSessionRegisterTotal");
    NotifyTerminateSessionFuncTotal func = [this](const SessionInfo& info, TerminateType terminateType) {
        this->TerminateSessionTotal(info, terminateType);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListenerTotal(func);
    WLOGFD("ProcessTerminateSessionRegisterTotal success");
}

void JsSceneSession::ProcessPendingSessionToForegroundRegister()
{
    WLOGFD("begin to run ProcessPendingSessionToForegroundRegister");
    NotifyPendingSessionToForegroundFunc func = [this](const SessionInfo& info) {
        this->PendingSessionToForeground(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetPendingSessionToForegroundListener(func);
    WLOGFD("ProcessPendingSessionToForegroundRegister success");
}

void JsSceneSession::ProcessPendingSessionToBackgroundForDelegatorRegister()
{
    WLOGFD("begin to run ProcessPendingSessionToBackgroundForDelegatorRegister");
    auto weak = weak_from_this();
    NotifyPendingSessionToBackgroundForDelegatorFunc func = [this](const SessionInfo& info) {
        this->PendingSessionToBackgroundForDelegator(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetPendingSessionToBackgroundForDelegatorListener(func);
    WLOGFD("ProcessPendingSessionToBackgroundForDelegatorRegister success");
}

void JsSceneSession::ProcessSessionFocusableChangeRegister()
{
    NotifySessionFocusableChangeFunc func = [this](bool isFocusable) {
        this->OnSessionFocusableChange(isFocusable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionFocusableChangeListener(func);
    WLOGFD("ProcessSessionFocusableChangeRegister success");
}

void JsSceneSession::ProcessSessionTouchableChangeRegister()
{
    NotifySessionTouchableChangeFunc func = [this](bool touchable) {
        this->OnSessionTouchableChange(touchable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionTouchableChangeListener(func);
    WLOGFD("ProcessSessionTouchableChangeRegister success");
}

void JsSceneSession::ProcessSessionExceptionRegister()
{
    WLOGFD("begin to run ProcessSessionExceptionRegister");
    NotifySessionExceptionFunc func = [this](const SessionInfo& info) {
        this->OnSessionException(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionExceptionListener(func);
    WLOGFD("ProcessSessionExceptionRegister success");
}

void JsSceneSession::ProcessClickRegister()
{
    NotifyClickFunc func = [this]() {
        this->OnClick();
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetClickListener(func);
    WLOGFD("ProcessClickChangeRegister success");
}

void JsSceneSession::OnSessionEvent(uint32_t eventId)
{
    WLOGFI("[NAPI]OnSessionEvent, eventId: %{public}d", eventId);
    auto iter = jsCbMap_.find(SESSION_EVENT_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [eventId, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, eventId);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionEvent", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::ProcessBackPressedRegister()
{
    NotifyBackPressedFunc func = [this](bool needMoveToBackground) {
        this->OnBackPressed(needMoveToBackground);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetBackPressedListenser(func);
}

void JsSceneSession::ProcessSystemBarPropertyChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnSystemBarPropertyChange_ = std::bind(
        &JsSceneSession::OnSystemBarPropertyChange, this, std::placeholders::_1);
    WLOGFD("ProcessSystemBarPropertyChangeRegister success");
}

void JsSceneSession::ProcessNeedAvoidRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnNeedAvoid_ = std::bind(
        &JsSceneSession::OnNeedAvoid, this, std::placeholders::_1);
    WLOGFD("ProcessNeedAvoidRegister success");
}

void JsSceneSession::ProcessIsCustomAnimationPlaying()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onIsCustomAnimationPlaying_ = std::bind(
        &JsSceneSession::OnIsCustomAnimationPlaying, this, std::placeholders::_1);
    WLOGFD("ProcessIsCustomAnimationPlaying success");
}

void JsSceneSession::ProcessShowWhenLockedRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnShowWhenLocked_ = std::bind(
        &JsSceneSession::OnShowWhenLocked, this, std::placeholders::_1);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sessionchangeCallback->OnShowWhenLocked_(session->IsShowWhenLocked());
    WLOGFD("ProcessShowWhenLockedRegister success");
}

void JsSceneSession::ProcessRequestedOrientationChange()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnRequestedOrientationChange_ = std::bind(
        &JsSceneSession::OnReuqestedOrientationChange, this, std::placeholders::_1);
    WLOGFD("ProcessRequestedOrientationChange success");
}

void JsSceneSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSession>(static_cast<JsSceneSession*>(data));
}

NativeValue* JsSceneSession::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RegisterCallback");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

NativeValue* JsSceneSession::UpdateNativeVisibility(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]UpdateNativeVisibility");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnUpdateNativeVisibility(*engine, *info) : nullptr;
}

NativeValue* JsSceneSession::SetPrivacyMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]SetPrivacyMode");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnSetPrivacyMode(*engine, *info) : nullptr;
}

NativeValue* JsSceneSession::SetShowRecent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]SetShowRecent");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnSetShowRecent(*engine, *info) : nullptr;
}

NativeValue* JsSceneSession::SetZOrder(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]SetZOrder");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnSetZOrder(*engine, *info) : nullptr;
}

bool JsSceneSession::IsCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        return false;
    }

    for (auto iter = jsCbMap_.begin(); iter != jsCbMap_.end(); ++iter) {
        if (jsListenerObject->StrictEquals(iter->second->Get())) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

bool JsSceneSession::IsCallbackTypeSupported(const std::string& type)
{
    if (listenerFunc_.find(type) != listenerFunc_.end()) {
        return true;
    }
    return false;
}

NativeValue* JsSceneSession::OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (value == nullptr || !value->IsCallable()) {
        WLOGFE("[NAPI]Invalid argument");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    if (!IsCallbackTypeSupported(cbType)) {
        WLOGFE("[NAPI]callback type is not supported, type = %{public}s", cbType.c_str());
        return engine.CreateUndefined();
    }
    if (IsCallbackRegistered(cbType, value)) {
        WLOGFE("[NAPI]callback is registered, type = %{public}s", cbType.c_str());
        return engine.CreateUndefined();
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_[cbType] = callbackRef;
    (this->*listenerFunc_[cbType])();
    WLOGFI("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return engine.CreateUndefined();
}

NativeValue* JsSceneSession::OnUpdateNativeVisibility(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    bool visible = false;
    if (!ConvertFromJsValue(engine, info.argv[0], visible)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    session->UpdateNativeVisibility(visible);
    SceneSessionManager::GetInstance().UpdatePrivateStateAndNotify(session->GetPersistentId());
    WLOGFI("[NAPI]OnUpdateNativeVisibility end");
    return engine.CreateUndefined();
}

NativeValue* JsSceneSession::OnSetPrivacyMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    bool isPrivacy = false;
    if (!ConvertFromJsValue(engine, info.argv[0], isPrivacy)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    session->SetPrivacyMode(isPrivacy);
    SceneSessionManager::GetInstance().UpdatePrivateStateAndNotify(session->GetPersistentId());
    WLOGFI("[NAPI]OnSetPrivacyMode end");
    return engine.CreateUndefined();
}

void JsSceneSession::OnCreateSpecificSession(const sptr<SceneSession>& sceneSession)
{
    WLOGFI("OnCreateSpecificSession");
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    WLOGFI("[NAPI]OnCreateSpecificSession");
    auto iter = jsCbMap_.find(CREATE_SPECIFIC_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    wptr<SceneSession> weakSession(sceneSession);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [this, weakSession, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto specificSession = weakSession.promote();
            if (specificSession == nullptr) {
                WLOGFE("[NAPI]root session or target session or engine is nullptr");
                return;
            }
            NativeValue* jsSceneSessionObj = Create(*eng, specificSession);
            if (jsSceneSessionObj == nullptr || !jsCallBack) {
                WLOGFE("[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
                return;
            }
            WLOGFI("CreateJsSceneSessionObject success");
            NativeValue* argv[] = { jsSceneSessionObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnCreateSpecificSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnBindDialogTarget(const sptr<SceneSession>& sceneSession)
{
    WLOGFI("OnBindDialogTarget");
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    WLOGFI("[NAPI]OnBindDialogTarget");
    auto iter = jsCbMap_.find(BIND_DIALOG_TARGET_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    wptr<SceneSession> weakSession(sceneSession);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [this, weakSession, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto specificSession = weakSession.promote();
            if (specificSession == nullptr) {
                WLOGFE("[NAPI]root session or target session or engine is nullptr");
                return;
            }
            NativeValue* jsSceneSessionObj = Create(*eng, specificSession);
            if (jsSceneSessionObj == nullptr || !jsCallBack) {
                WLOGFE("[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
                return;
            }
            WLOGFI("CreateJsSceneSessionObject success");
            NativeValue* argv[] = { jsSceneSessionObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnBindDialogTarget", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionStateChange(const SessionState& state)
{
    WLOGFI("[NAPI]OnSessionStateChange, state: %{public}u", static_cast<uint32_t>(state));
    auto iter = jsCbMap_.find(SESSION_STATE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [state, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionStateObj = CreateJsValue(engine, state);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionStateChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionRectChange(const WSRect& rect, const SizeChangeReason& reason)
{
    if (rect.IsEmpty()) {
        WLOGFD("Rect is empty, there is no need to notify");
        return;
    }
    WLOGFI("[NAPI]OnSessionRectChange");
    auto iter = jsCbMap_.find(SESSION_RECT_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [rect, reason, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionStateObj = CreateJsSessionRect(engine, rect);
            NativeValue* sizeChangeReason = CreateJsValue(engine, static_cast<int32_t>(reason));
            NativeValue* argv[] = { jsSessionStateObj, sizeChangeReason };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionRectChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnRaiseToTop()
{
    WLOGFI("[NAPI]OnRaiseToTop");
    auto iter = jsCbMap_.find(RAISE_TO_TOP_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* argv[] = {};
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, 0);
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnRaiseToTop", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnRaiseAboveTarget(int32_t subWindowId)
{
    WLOGFI("[NAPI]OnRaiseAboveTarget");
    auto iter = jsCbMap_.find(RAISE_ABOVE_TARGET_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, eng = &engine_, subWindowId](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSceneSessionObj = CreateJsValue(engine, subWindowId);
            if (jsSceneSessionObj == nullptr) {
                WLOGFE("[NAPI]jsSceneSessionObj is nullptr");
                return;
            }
            NativeValue* argv[] = {
                [0]=CreateJsError(engine, 0),
                [1]=jsSceneSessionObj
            };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnRaiseAboveTarget", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}


void JsSceneSession::OnSessionFocusableChange(bool isFocusable)
{
    WLOGFI("[NAPI]OnSessionFocusableChange, state: %{public}u", isFocusable);
    auto iter = jsCbMap_.find(SESSION_FOCUSABLE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [isFocusable, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionFocusableObj = CreateJsValue(engine, isFocusable);
            NativeValue* argv[] = { jsSessionFocusableObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionFocusableChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionTouchableChange(bool touchable)
{
    WLOGFI("[NAPI]OnSessionTouchableChange, state: %{public}u", touchable);
    auto iter = jsCbMap_.find(SESSION_TOUCHABLE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [touchable, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionTouchableObj = CreateJsValue(engine, touchable);
            NativeValue* argv[] = { jsSessionTouchableObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionTouchableChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnClick()
{
    WLOGFI("[NAPI]OnClick");
    auto iter = jsCbMap_.find(CLICK_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* argv[] = { };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, 0);
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnClick", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionActivation(SessionInfo& info)
{
    WLOGI("[NAPI]pending session activation: bundleName %{public}s, moduleName %{public}s, abilityName %{public}s, \
        appIndex %{public}d, reuse %{public}d", info.bundleName_.c_str(), info.moduleName_.c_str(),
        info.abilityName_.c_str(), info.appIndex_, info.reuse);
    if (info.persistentId_ == 0) {
        sptr<SceneSession> sceneSession = nullptr;
        if (info.reuse) {
            sceneSession = SceneSessionManager::GetInstance().GetSceneSessionByName(
                info.bundleName_, info.moduleName_, info.abilityName_, info.appIndex_);
        }
        if (sceneSession == nullptr) {
            WLOGFI("GetSceneSessionByName return nullptr, RequestSceneSession");
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                WLOGFE("RequestSceneSession return nullptr");
                return;
            }
        } else {
            sceneSession->GetSessionInfo().want = info.want;
            sceneSession->GetSessionInfo().callerToken_ = info.callerToken_;
            sceneSession->GetSessionInfo().requestCode = info.requestCode;
            sceneSession->GetSessionInfo().callerPersistentId_ = info.callerPersistentId_;
            sceneSession->GetSessionInfo().callingTokenId_ = info.callingTokenId_;
            sceneSession->GetSessionInfo().uiAbilityId_ = info.uiAbilityId_;
            sceneSession->GetSessionInfo().startSetting = info.startSetting;
        }
        info.persistentId_ = sceneSession->GetPersistentId();
        sceneSession->GetSessionInfo().persistentId_ = sceneSession->GetPersistentId();
    } else {
        auto sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            WLOGFE("GetSceneSession return nullptr");
            return;
        }
        sceneSession->GetSessionInfo().want = info.want;
        sceneSession->GetSessionInfo().callerToken_ = info.callerToken_;
        sceneSession->GetSessionInfo().requestCode = info.requestCode;
        sceneSession->GetSessionInfo().callerPersistentId_ = info.callerPersistentId_;
        sceneSession->GetSessionInfo().callingTokenId_ = info.callingTokenId_;
        sceneSession->GetSessionInfo().uiAbilityId_ = info.uiAbilityId_;
        sceneSession->GetSessionInfo().startSetting = info.startSetting;
    }

    PendingSessionActivationInner(info);
}

void JsSceneSession::PendingSessionActivationInner(SessionInfo& info)
{
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            if (sessionInfo == nullptr) {
                WLOGFE("[NAPI]sessionInfo is nullptr");
                return;
            }
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, *sessionInfo);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::PendingSessionActivation", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnBackPressed(bool needMoveToBackground)
{
    WLOGFI("[NAPI]OnBackPressed needMoveToBackground %{public}d", needMoveToBackground);
    auto iter = jsCbMap_.find(BACK_PRESSED_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [needMoveToBackground, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsNeedMoveToBackgroundObj = CreateJsValue(engine, needMoveToBackground);
            NativeValue* argv[] = { jsNeedMoveToBackgroundObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnBackPressed", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSession(const SessionInfo& info)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, abilityName = %{public}s, persistentId = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), info.persistentId_);
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            if (sessionInfo == nullptr) {
                WLOGFE("[NAPI]sessionInfo is nullptr");
                return;
            }
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, *sessionInfo);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::TerminateSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSessionNew(const SessionInfo& info, bool needStartCaller)
{
    WLOGFI("[NAPI]run TerminateSessionNew, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB_NEW);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [needStartCaller, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsNeedStartCaller = CreateJsValue(engine, needStartCaller);
            if (jsNeedStartCaller == nullptr) {
                WLOGFE("[NAPI]this target jsNeedStartCaller is nullptr");
                return;
            }
            NativeValue* argv[] = { jsNeedStartCaller };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::TerminateSessionNew", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSessionTotal(const SessionInfo& info, TerminateType terminateType)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, id = %{public}s, terminateType = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), static_cast<int32_t>(terminateType));
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB_TOTAL);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [terminateType, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsTerminateType = CreateJsValue(engine, static_cast<int32_t>(terminateType));
            if (jsTerminateType == nullptr) {
                WLOGFE("[NAPI]this target jsTerminateType is nullptr");
                return;
            }
            NativeValue* argv[] = { jsTerminateType  };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::terminateSessionTotal", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::UpdateSessionLabel(const std::string &label)
{
    WLOGFI("[NAPI]run UpdateSessionLabel");
    auto iter = jsCbMap_.find(UPDATE_SESSION_LABEL_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [label, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsLabel = CreateJsValue(engine, label);
            if (jsLabel == nullptr) {
                WLOGFE("[NAPI]this target jsLabel is nullptr");
                return;
            }
            NativeValue* argv[] = { jsLabel  };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::UpdateSessionLabel", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::ProcessUpdateSessionLabelRegister()
{
    WLOGFD("begin to run ProcessUpdateSessionLabelRegister");
    NofitySessionLabelUpdatedFunc func = [this](const std::string& label) {
        this->UpdateSessionLabel(label);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetUpdateSessionLabelListener(func);
    WLOGFD("ProcessUpdateSessionLabelRegister success");
}

void JsSceneSession::ProcessUpdateSessionIconRegister()
{
    WLOGFD("begin to run ProcessUpdateSessionIconRegister");
    NofitySessionIconUpdatedFunc func = [this](const std::string& iconPath) {
        this->UpdateSessionIcon(iconPath);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetUpdateSessionIconListener(func);
    WLOGFD("ProcessUpdateSessionIconRegister success");
}

void JsSceneSession::UpdateSessionIcon(const std::string &iconPath)
{
    WLOGFI("[NAPI]run UpdateSessionIcon");
    auto iter = jsCbMap_.find(UPDATE_SESSION_ICON_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [iconPath, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsIconPath = CreateJsValue(engine, iconPath);
            if (jsIconPath == nullptr) {
                WLOGFE("[NAPI]this target jsIconPath is nullptr");
                return;
            }
            NativeValue* argv[] = { jsIconPath  };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::UpdateSessionIcon", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionException(const SessionInfo& info)
{
    WLOGFI("[NAPI]run OnSessionException, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(SESSION_EXCEPTION_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            if (sessionInfo == nullptr) {
                WLOGFE("[NAPI]sessionInfo is nullptr");
                return;
            }
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, *sessionInfo);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::TerminateSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionToForeground(const SessionInfo& info)
{
    WLOGFI("[NAPI]run PendingSessionToForeground, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(PENDING_SESSION_TO_FOREGROUND_CB);
    if (iter == jsCbMap_.end()) {
        WLOGFE("[NAPI]fail to find pending session to foreground callback");
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            if (sessionInfo == nullptr) {
                WLOGFE("[NAPI]sessionInfo is nullptr");
                return;
            }
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, *sessionInfo);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::PendingSessionToForeground", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionToBackgroundForDelegator(const SessionInfo& info)
{
    WLOGFI("[NAPI]run PendingSessionToBackgroundForDelegator, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB);
    if (iter == jsCbMap_.end()) {
        WLOGFE("[NAPI]fail to find pending session to background for delegator callback");
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            if (sessionInfo == nullptr) {
                WLOGFE("[NAPI]sessionInfo is nullptr");
                return;
            }
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, *sessionInfo);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::PendingSessionToBackgroundForDelegator", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSystemBarPropertyChange(const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    WLOGFI("[NAPI]OnSystemBarPropertyChange");
    auto iter = jsCbMap_.find(SYSTEMBAR_PROPERTY_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, propertyMap, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsSystemBarPropertyArrayObject(engine, propertyMap);
            if (jsSessionStateObj == nullptr) {
                WLOGFE("[NAPI]jsSessionStateObj is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSystemBarPropertyChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnNeedAvoid(bool status)
{
    WLOGFI("[NAPI]OnNeedAvoid %{public}d", status);
    auto iter = jsCbMap_.find(NEED_AVOID_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, needAvoid = status, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, needAvoid);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnNeedAvoid", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnIsCustomAnimationPlaying(bool status)
{
    WLOGFI("[NAPI]OnIsCustomAnimationPlaying %{public}d", status);
    auto iter = jsCbMap_.find(CUSTOM_ANIMATION_PLAYING_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, isPlaying = status, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, isPlaying);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnIsCustomAnimationPlaying", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFI("[NAPI]OnShowWhenLocked %{public}d", showWhenLocked);
    auto iter = jsCbMap_.find(SHOW_WHEN_LOCKED_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, flag = showWhenLocked, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, flag);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnShowWhenLocked", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnReuqestedOrientationChange(uint32_t orientation)
{
    WLOGFI("[NAPI]OnReuqestedOrientationChange %{public}u", orientation);
    auto iter = jsCbMap_.find(REQUESTED_ORIENTATION_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    if (WINDOW_ORIENTATION_TO_JS_SESSION_MAP.count(static_cast<Orientation>(orientation)) == 0) {
        WLOGFE("[NAPI]failed %{public}u since no this type", orientation);
        return;
    }
    uint32_t value = static_cast<uint32_t>(WINDOW_ORIENTATION_TO_JS_SESSION_MAP.at(
        static_cast<Orientation>(orientation)));
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, rotation = value, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionRotationObj = CreateJsValue(engine, rotation);
            NativeValue* argv[] = { jsSessionRotationObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
            WLOGFI("[NAPI]change rotation success %{public}u", rotation);
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnReuqestedOrientationChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

NativeValue* JsSceneSession::OnSetShowRecent(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is null");
        return engine.CreateUndefined();
    }
    session->SetShowRecent(true);
    return engine.CreateUndefined();
}

NativeValue* JsSceneSession::OnSetZOrder(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc != 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    uint32_t zOrder;
    if (!ConvertFromJsValue(engine, info.argv[0], zOrder)) {
        WLOGFE("[NAPI]Failed to convert parameter to uint32_t");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is null");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
            "session is null"));
        return engine.CreateUndefined();
    }
    session->SetZOrder(zOrder);
    return engine.CreateUndefined();
}

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return weakSession_.promote();
}
} // namespace OHOS::Rosen
