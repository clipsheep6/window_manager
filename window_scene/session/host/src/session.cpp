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

#include "session/host/include/session.h"

#include "ability_info.h"
#include "ability_start_setting.h"
#include "input_manager.h"
#include "ipc_skeleton.h"
#include "key_event.h"
#include "pointer_event.h"
#include <transaction/rs_interfaces.h>
#include <ui/rs_surface_node.h>
#include "../../proxy/include/window_info.h"

#include "anr_manager.h"
#include "session_helper.h"
#include "surface_capture_future.h"
#include "util.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "Session" };
std::atomic<int32_t> g_persistentId = INVALID_SESSION_ID;
std::set<int32_t> g_persistentIdSet;
constexpr float INNER_BORDER_VP = 5.0f;
constexpr float OUTSIDE_BORDER_VP = 4.0f;
constexpr float INNER_ANGLE_VP = 16.0f;
} // namespace

Session::Session(const SessionInfo& info) : sessionInfo_(info)
{
    property_ = new WindowSessionProperty();
    property_->SetWindowType(static_cast<WindowType>(info.windowType_));

    using type = std::underlying_type_t<MMI::WindowArea>;
    for (type area = static_cast<type>(MMI::WindowArea::FOCUS_ON_TOP);
        area <= static_cast<type>(MMI::WindowArea::FOCUS_ON_BOTTOM_RIGHT); ++area) {
        auto ret = windowAreas_.insert(
            std::pair<MMI::WindowArea, WSRectF>(static_cast<MMI::WindowArea>(area), WSRectF()));
        if (!ret.second) {
            WLOGFE("Failed to insert area:%{public}d", area);
        }
    }
}

void Session::SetEventHandler(const std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    handler_ = handler;
}

void Session::PostTask(Task&& task, int64_t delayTime)
{
    if (!handler_ || handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        return task();
    }
    handler_->PostTask(std::move(task), delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

int32_t Session::GetPersistentId() const
{
    return persistentId_;
}

std::shared_ptr<RSSurfaceNode> Session::GetSurfaceNode() const
{
    return surfaceNode_;
}

std::shared_ptr<RSSurfaceNode> Session::GetLeashWinSurfaceNode() const
{
    return leashWinSurfaceNode_;
}

std::shared_ptr<Media::PixelMap> Session::GetSnapshot() const
{
    return snapshot_;
}

void Session::SetSessionInfoAncoSceneState(int32_t ancoSceneState)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.ancoSceneState = ancoSceneState;
}

void Session::SetSessionInfoTime(const std::string& time)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.time = time;
}

void Session::SetSessionInfoAbilityInfo(const std::shared_ptr<AppExecFwk::AbilityInfo>& abilityInfo)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.abilityInfo = abilityInfo;
}

void Session::SetSessionInfoWant(const std::shared_ptr<AAFwk::Want>& want)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.want = want;
}

void Session::SetSessionInfoPersistentId(int32_t persistentId)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.persistentId_ = persistentId;
}

void Session::SetSessionInfoCallerPersistentId(int32_t callerPersistentId)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.callerPersistentId_ = callerPersistentId;
}

void Session::SetSessionInfoContinueState(ContinueState state)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.continueState = state;
}

void Session::SetSessionInfoLockedState(bool lockedState)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.lockedState = lockedState;
    NotifySessionInfoLockedStateChange(lockedState);
}

void Session::SetSessionInfoIsClearSession(bool isClearSession)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.isClearSession = isClearSession;
}

void Session::SetSessionInfoAffinity(std::string affinity)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.sessionAffinity = affinity;
}

void Session::GetCloseAbilityWantAndClean(AAFwk::Want& outWant)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    if (sessionInfo_.closeAbilityWant != nullptr) {
        outWant = *sessionInfo_.closeAbilityWant;
        sessionInfo_.closeAbilityWant = nullptr;
    }
}

void Session::SetSessionInfo(const SessionInfo& info)
{
    std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
    sessionInfo_.want = info.want;
    sessionInfo_.callerToken_ = info.callerToken_;
    sessionInfo_.requestCode = info.requestCode;
    sessionInfo_.callerPersistentId_ = info.callerPersistentId_;
    sessionInfo_.callingTokenId_ = info.callingTokenId_;
    sessionInfo_.uiAbilityId_ = info.uiAbilityId_;
    sessionInfo_.startSetting = info.startSetting;
}

void Session::SetScreenId(uint64_t screenId)
{
    sessionInfo_.screenId_ = screenId;
}

const SessionInfo& Session::GetSessionInfo() const
{
    return sessionInfo_;
}

bool Session::RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return RegisterListenerLocked(lifecycleListeners_, listener);
}

bool Session::UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return UnregisterListenerLocked(lifecycleListeners_, listener);
}

template<typename T>
bool Session::RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return false;
    }
    std::lock_guard<std::mutex> lock(lifecycleListenersMutex_);
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        WLOGFE("Listener already registered");
        return false;
    }
    holder.emplace_back(listener);
    return true;
}

template<typename T>
bool Session::UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return false;
    }
    std::lock_guard<std::mutex> lock(lifecycleListenersMutex_);
    holder.erase(std::remove_if(holder.begin(), holder.end(),
        [listener](std::shared_ptr<T> registeredListener) { return registeredListener == listener; }),
        holder.end());
    return true;
}

void Session::NotifyActivation()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnActivation();
        }
    }
}

void Session::NotifyConnect()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnConnect();
        }
    }
}

void Session::NotifyForeground()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnForeground();
        }
    }
}

void Session::NotifyBackground()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnBackground();
        }
    }
}

void Session::NotifyDisconnect()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnDisconnect();
        }
    }
}

void Session::NotifyExtensionDied()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnExtensionDied();
        }
    }
}

void Session::NotifyTransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
    const std::vector<int32_t>& uiExtensionIdLevelVec)
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnAccessibilityEvent(info, uiExtensionIdLevelVec);
        }
    }
}

float Session::GetAspectRatio() const
{
    return aspectRatio_;
}

WSError Session::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
    return WSError::WS_OK;
}

SessionState Session::GetSessionState() const
{
    return state_;
}

void Session::SetSessionState(SessionState state)
{
    if (state < SessionState::STATE_DISCONNECT || state > SessionState::STATE_END) {
        WLOGFD("Invalid session state: %{public}u", state);
        return;
    }
    state_ = state;
}

void Session::UpdateSessionState(SessionState state)
{
    state_ = state;
    NotifySessionStateChange(state);
}

void Session::UpdateSessionTouchable(bool touchable)
{
    GetSessionProperty()->SetTouchable(touchable);
    NotifySessionTouchableChange(touchable);
}

WSError Session::SetFocusable(bool isFocusable)
{
    WLOGFI("SetFocusable id: %{public}d, focusable: %{public}d", GetPersistentId(), isFocusable);
    GetSessionProperty()->SetFocusable(isFocusable);
    if (isFocused_ && !GetFocusable()) {
        NotifyRequestFocusStatusNotifyManager(false);
    }
    return WSError::WS_OK;
}

bool Session::GetFocusable() const
{
    auto property = GetSessionProperty();
    if (property) {
        return property->GetFocusable();
    }
    WLOGFD("property is null");
    return true;
}

void Session::SetNeedNotify(bool needNotify)
{
    needNotify_ = needNotify;
}

bool Session::NeedNotify() const
{
    return needNotify_;
}

WSError Session::SetTouchable(bool touchable)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionTouchable(touchable);
    return WSError::WS_OK;
}

bool Session::GetTouchable() const
{
    return GetSessionProperty()->GetTouchable();
}

WSError Session::SetVisible(bool isVisible)
{
    isRSVisible_ = isVisible;
    return WSError::WS_OK;
}

bool Session::GetVisible() const
{
    return isRSVisible_;
}

WSError Session::SetDrawingContentState(bool isRSDrawing)
{
    isRSDrawing_ = isRSDrawing;
    return WSError::WS_OK;
}

bool Session::GetDrawingContentState() const
{
    return isRSDrawing_;
}

int32_t Session::GetWindowId() const
{
    return GetPersistentId();
}

void Session::SetCallingPid(int32_t id)
{
    callingPid_ = id;
}

void Session::SetCallingUid(int32_t id)
{
    callingUid_ = id;
}

int32_t Session::GetCallingPid() const
{
    return callingPid_;
}

int32_t Session::GetCallingUid() const
{
    return callingUid_;
}

void Session::SetAbilityToken(sptr<IRemoteObject> token)
{
    abilityToken_ = token;
}

sptr<IRemoteObject> Session::GetAbilityToken() const
{
    return abilityToken_;
}

WSError Session::SetBrightness(float brightness)
{
    auto property = GetSessionProperty();
    if (!property) {
        return WSError::WS_ERROR_NULLPTR;
    }
    property->SetBrightness(brightness);
    return WSError::WS_OK;
}

float Session::GetBrightness() const
{
    auto property = GetSessionProperty();
    if (!property) {
        return UNDEFINED_BRIGHTNESS;
    }
    return property->GetBrightness();
}

bool Session::IsSessionValid() const
{
    if (sessionInfo_.isSystem_) {
        WLOGFD("session is system, id: %{public}d, name: %{public}s, state: %{public}u",
            GetPersistentId(), sessionInfo_.bundleName_.c_str(), state_);
        return false;
    }
    bool res = state_ > SessionState::STATE_DISCONNECT && state_ < SessionState::STATE_END;
    if (!res) {
        WLOGFI("session is already destroyed or not created! id: %{public}d state: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

bool Session::IsActive() const
{
    return isActive_;
}

bool Session::IsSystemSession() const
{
    return sessionInfo_.isSystem_;
}

bool Session::IsTerminated() const
{
    return (GetSessionState() == SessionState::STATE_DISCONNECT || isTerminating);
}

bool Session::IsSessionForeground() const
{
    return state_ == SessionState::STATE_FOREGROUND || state_ == SessionState::STATE_ACTIVE;
}

WSError Session::SetPointerStyle(MMI::WindowArea area)
{
    WLOGFI("Information to be set: pid:%{public}d, windowId:%{public}d, MMI::WindowArea:%{public}s",
        callingPid_, persistentId_, DumpPointerWindowArea(area));
    MMI::InputManager::GetInstance()->SetWindowPointerStyle(area, callingPid_, persistentId_);
    return WSError::WS_OK;
}

WSRectF Session::UpdateTopBottomArea(const WSRectF& rect, MMI::WindowArea area)
{
    const float innerBorder = INNER_BORDER_VP * vpr_;
    const float outsideBorder = OUTSIDE_BORDER_VP * vpr_;
    const float innerAngle = INNER_ANGLE_VP * vpr_;
    const float horizontalBorderLength = outsideBorder + innerAngle;
    const float verticalBorderLength = outsideBorder + innerBorder;
    const size_t innerAngleCount = 2;
    WSRectF tbRect;
    tbRect.posX_ = rect.posX_ + horizontalBorderLength;
    tbRect.width_ = rect.width_ - horizontalBorderLength * innerAngleCount;
    tbRect.height_ = verticalBorderLength;
    if (area == MMI::WindowArea::FOCUS_ON_TOP) {
        tbRect.posY_ = rect.posY_;
    } else if (area == MMI::WindowArea::FOCUS_ON_BOTTOM) {
        tbRect.posY_ = rect.posY_ + rect.height_ - verticalBorderLength;
    } else {
        return WSRectF();
    }
    return tbRect;
}

WSRectF Session::UpdateLeftRightArea(const WSRectF& rect, MMI::WindowArea area)
{
    const float innerBorder = INNER_BORDER_VP * vpr_;
    const float outsideBorder = OUTSIDE_BORDER_VP * vpr_;
    const float innerAngle = INNER_ANGLE_VP * vpr_;
    const float verticalBorderLength = outsideBorder + innerAngle;
    const float horizontalBorderLength = outsideBorder + innerBorder;
    const size_t innerAngleCount = 2;
    WSRectF lrRect;
    lrRect.posY_ = rect.posY_ + verticalBorderLength;
    lrRect.width_ = horizontalBorderLength;
    lrRect.height_ = rect.height_ - verticalBorderLength * innerAngleCount;
    if (area == MMI::WindowArea::FOCUS_ON_LEFT) {
        lrRect.posX_ = rect.posX_;
    } else if (area == MMI::WindowArea::FOCUS_ON_RIGHT) {
        lrRect.posX_ = rect.posX_ + rect.width_ - horizontalBorderLength;
    } else {
        return WSRectF();
    }
    return lrRect;
}

WSRectF Session::UpdateInnerAngleArea(const WSRectF& rect, MMI::WindowArea area)
{
    const float outsideBorder = OUTSIDE_BORDER_VP * vpr_;
    const float innerAngle = INNER_ANGLE_VP * vpr_;
    WSRectF iaRect;
    iaRect.width_ = outsideBorder + innerAngle;
    iaRect.height_ = outsideBorder + innerAngle;
    if (area == MMI::WindowArea::FOCUS_ON_TOP_LEFT) {
        iaRect.posX_ = rect.posX_;
        iaRect.posY_ = rect.posY_;
    } else if (area == MMI::WindowArea::FOCUS_ON_TOP_RIGHT) {
        iaRect.posX_ = rect.posX_ + rect.width_ - iaRect.width_;
        iaRect.posY_ = rect.posY_;
    } else if (area == MMI::WindowArea::FOCUS_ON_BOTTOM_LEFT) {
        iaRect.posX_ = rect.posX_;
        iaRect.posY_ = rect.posY_ + rect.height_ - iaRect.height_;
    } else if (area == MMI::WindowArea::FOCUS_ON_BOTTOM_RIGHT) {
        iaRect.posX_ = rect.posX_ + rect.width_ - iaRect.width_;
        iaRect.posY_ = rect.posY_ + rect.height_ - iaRect.height_;
    } else {
        return WSRectF();
    }
    return iaRect;
}

WSRectF Session::UpdateHotRect(const WSRect& rect)
{
    WSRectF newRect;
    const float outsideBorder = OUTSIDE_BORDER_VP * vpr_;
    const size_t outsideBorderCount = 2;
    newRect.posX_ = rect.posX_ - outsideBorder;
    newRect.posY_ = rect.posY_ - outsideBorder;
    newRect.width_ = rect.width_ + outsideBorder * outsideBorderCount;
    newRect.height_ = rect.height_ + outsideBorder * outsideBorderCount;
    return newRect;
}

void Session::UpdatePointerArea(const WSRect& rect)
{
    if (IsSystemSession()) {
        return;
    }
    if (!(GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
         GetWindowMode()== WindowMode::WINDOW_MODE_FLOATING)) {
        return;
    }
    if (preRect_ == rect) {
        WLOGFD("The window area does not change");
        return;
    }
    WSRectF hotRect = UpdateHotRect(rect);
    for (const auto &[area, _] : windowAreas_) {
        if (area == MMI::WindowArea::FOCUS_ON_TOP || area == MMI::WindowArea::FOCUS_ON_BOTTOM) {
            windowAreas_[area] = UpdateTopBottomArea(hotRect, area);
        } else if (area == MMI::WindowArea::FOCUS_ON_RIGHT || area == MMI::WindowArea::FOCUS_ON_LEFT) {
            windowAreas_[area] = UpdateLeftRightArea(hotRect, area);
        } else if (area == MMI::WindowArea::FOCUS_ON_TOP_LEFT || area == MMI::WindowArea::FOCUS_ON_TOP_RIGHT ||
            area == MMI::WindowArea::FOCUS_ON_BOTTOM_LEFT || area == MMI::WindowArea::FOCUS_ON_BOTTOM_RIGHT) {
            windowAreas_[area] = UpdateInnerAngleArea(hotRect, area);
        }
    }
    preRect_ = rect;
}

WSError Session::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    WLOGFD("session update rect: id: %{public}d, rect[%{public}d, %{public}d, %{public}u, %{public}u], "\
        "reason:%{public}u", GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    if (!IsSessionValid()) {
        winRect_ = rect;
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    winRect_ = rect;
    if (sessionStage_ != nullptr) {
        sessionStage_->UpdateRect(rect, reason, rsTransaction);
    } else {
        WLOGFE("sessionStage_ is nullptr");
    }
    UpdatePointerArea(winRect_);
    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        NotifyCallingSessionUpdateRect();
    }
    return WSError::WS_OK;
}

WSError Session::UpdateDensity()
{
    WLOGFI("session update density: id: %{public}d.", GetPersistentId());
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (sessionStage_ != nullptr) {
        sessionStage_->UpdateDensity();
    } else {
        WLOGFE("Session::UpdateDensity sessionStage_ is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    return WSError::WS_OK;
}

void Session::NotifyCallingSessionUpdateRect()
{
    if (notifyCallingSessionUpdateRectFunc_) {
        WLOGFI("Notify calling window that input method update rect");
        notifyCallingSessionUpdateRectFunc_(persistentId_);
    }
}

WSError Session::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token, int32_t pid, int32_t uid)
{
    WLOGFI("[WMSCom] Connect session, id: %{public}d, state: %{public}u, isTerminating: %{public}d", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()), isTerminating);
    if (GetSessionState() != SessionState::STATE_DISCONNECT && !isTerminating) {
        WLOGFE("state is not disconnect!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (sessionStage == nullptr || eventChannel == nullptr) {
        WLOGFE("session stage or eventChannel is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_ = sessionStage;
    windowEventChannel_ = eventChannel;
    surfaceNode_ = surfaceNode;
    abilityToken_ = token;
    systemConfig = systemConfig_;
    if (property_ && property_->GetIsNeedUpdateWindowMode()) {
        property->SetIsNeedUpdateWindowMode(true);
        property->SetWindowMode(property_->GetWindowMode());
    }
    SetSessionProperty(property);
    if (property) {
        property->SetPersistentId(GetPersistentId());
    }
    callingPid_ = pid;
    callingUid_ = uid;

    UpdateSessionState(SessionState::STATE_CONNECT);
    // once update rect before connect, update again when connect
    if (WindowHelper::IsUIExtensionWindow(GetWindowType())) {
        UpdateRect(winRect_, SizeChangeReason::UNDEFINED);
    } else {
        NotifyClientToUpdateRect();
    }
    NotifyConnect();
    callingBundleName_ = DelayedSingleton<ANRManager>::GetInstance()->GetBundleName(callingPid_, callingUid_);
    DelayedSingleton<ANRManager>::GetInstance()->SetApplicationInfo(persistentId_, callingPid_, callingBundleName_);
    return WSError::WS_OK;
}

WSError Session::Foreground(sptr<WindowSessionProperty> property)
{
    HandleDialogForeground();
    SessionState state = GetSessionState();
    WLOGFI("[WMSCom] Foreground session, id: %{public}d, state: %{public}" PRIu32"", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state != SessionState::STATE_CONNECT && state != SessionState::STATE_BACKGROUND &&
        state != SessionState::STATE_INACTIVE) {
        WLOGFE("[WMSCom] Foreground state invalid! state:%{public}u", state);
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    UpdateSessionState(SessionState::STATE_FOREGROUND);
    if (!isActive_) {
        SetActive(true);
    }

    if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG && GetParentSession() &&
        !GetParentSession()->IsSessionForeground()) {
        WLOGFD("[WMSDialog] parent is not foreground");
        SetSessionState(SessionState::STATE_BACKGROUND);
    }

    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        NotifyCallingSessionForeground();
    }
    return WSError::WS_OK;
}

void Session::NotifyCallingSessionForeground()
{
    if (notifyCallingSessionForegroundFunc_) {
        WLOGFI("[WMSInput] Notify calling window that input method shown");
        notifyCallingSessionForegroundFunc_(persistentId_);
    }
}

void Session::HandleDialogBackground()
{
    const auto& type = GetWindowType();
    if (type < WindowType::APP_MAIN_WINDOW_BASE || type >= WindowType::APP_MAIN_WINDOW_END) {
        WLOGFD("[WMSDialog] Current session is not main window, id: %{public}d, type: %{public}d",
            GetPersistentId(), type);
        return;
    }

    std::vector<sptr<Session>> dialogVec;
    {
        std::unique_lock<std::mutex> lock(dialogVecMutex_);
        dialogVec = dialogVec_;
    }
    for (const auto& dialog : dialogVec) {
        if (dialog == nullptr) {
            continue;
        }
        WLOGFI("[WMSDialog] Background dialog, id: %{public}d, dialogId: %{public}d",
            GetPersistentId(), dialog->GetPersistentId());
        dialog->SetSessionState(SessionState::STATE_BACKGROUND);
    }
}

void Session::HandleDialogForeground()
{
    const auto& type = GetWindowType();
    if (type < WindowType::APP_MAIN_WINDOW_BASE || type >= WindowType::APP_MAIN_WINDOW_END) {
        WLOGFD("[WMSDialog] Current session is not main window, id: %{public}d, type: %{public}d",
            GetPersistentId(), type);
        return;
    }

    std::vector<sptr<Session>> dialogVec;
    {
        std::unique_lock<std::mutex> lock(dialogVecMutex_);
        dialogVec = dialogVec_;
    }
    for (const auto& dialog : dialogVec) {
        if (dialog == nullptr) {
            continue;
        }
        WLOGFI("[WMSDialog] Foreground dialog, id: %{public}d, dialogId: %{public}d",
            GetPersistentId(), dialog->GetPersistentId());
        dialog->SetSessionState(SessionState::STATE_ACTIVE);
    }
}

WSError Session::Background()
{
    HandleDialogBackground();
    SessionState state = GetSessionState();
    WLOGFI("[WMSCom] Background session, id: %{public}d, state: %{public}" PRIu32"", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state == SessionState::STATE_ACTIVE && GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        UpdateSessionState(SessionState::STATE_INACTIVE);
        state = SessionState::STATE_INACTIVE;
        isActive_ = false;
    }
    if (state != SessionState::STATE_INACTIVE) {
        WLOGFE("[WMSCom] Background state invalid! state:%{public}u", state);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionState(SessionState::STATE_BACKGROUND);
    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        NotifyCallingSessionBackground();
    }
    DelayedSingleton<ANRManager>::GetInstance()->OnBackground(persistentId_);
    return WSError::WS_OK;
}

void Session::NotifyCallingSessionBackground()
{
    if (notifyCallingSessionBackgroundFunc_) {
        WLOGFI("[WMSInput] Notify calling window that input method hide");
        notifyCallingSessionBackgroundFunc_();
    }
}

WSError Session::Disconnect()
{
    auto state = GetSessionState();
    WLOGFI("[WMSCom] Disconnect session, id: %{public}d, state: %{public}u", GetPersistentId(), state);
    UpdateSessionState(SessionState::STATE_BACKGROUND);
    UpdateSessionState(SessionState::STATE_DISCONNECT);
    NotifyDisconnect();
    DelayedSingleton<ANRManager>::GetInstance()->OnSessionLost(persistentId_);
    return WSError::WS_OK;
}

WSError Session::Show(sptr<WindowSessionProperty> property)
{
    WLOGFD("Show session, id: %{public}d", GetPersistentId());
    return WSError::WS_OK;
}

WSError Session::Hide()
{
    WLOGFD("Hide session, id: %{public}d", GetPersistentId());
    return WSError::WS_OK;
}

WSError Session::SetActive(bool active)
{
    SessionState state = GetSessionState();
    WLOGFI("[WMSCom] isActive: %{public}d, id: %{public}d, state: %{public}" PRIu32"",
        active, GetPersistentId(), static_cast<uint32_t>(state));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (active == isActive_) {
        WLOGFD("[WMSCom] Session active do not change: [%{public}d]", active);
        return WSError::WS_DO_NOTHING;
    }
    if (active && GetSessionState() == SessionState::STATE_FOREGROUND) {
        sessionStage_->SetActive(true);
        UpdateSessionState(SessionState::STATE_ACTIVE);
        isActive_ = active;
    }
    if (!active && GetSessionState() == SessionState::STATE_ACTIVE) {
        sessionStage_->SetActive(false);
        UpdateSessionState(SessionState::STATE_INACTIVE);
        isActive_ = active;
    }
    return WSError::WS_OK;
}

void Session::NotifyForegroundInteractiveStatus(bool interactive)
{
    if (!IsSessionValid() || !sessionStage_) {
        return;
    }
    WLOGFI("NotifyForegroundInteractiveStatus %{public}d", interactive);
    sessionStage_->NotifyForegroundInteractiveStatus(interactive);
}

void Session::SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func)
{
    pendingSessionActivationFunc_ = func;
}

void Session::SetBackPressedListenser(const NotifyBackPressedFunc& func)
{
    backPressedFunc_ = func;
}

void Session::SetTerminateSessionListener(const NotifyTerminateSessionFunc& func)
{
    terminateSessionFunc_ = func;
}

WSError Session::TerminateSessionNew(const sptr<AAFwk::SessionInfo> abilitySessionInfo, bool needStartCaller)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (isTerminating) {
        WLOGFE("TerminateSessionNew isTerminating, return!");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    isTerminating = true;
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
    {
        std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
        sessionInfo_.closeAbilityWant = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
        sessionInfo_.resultCode = abilitySessionInfo->resultCode;
    }
    if (terminateSessionFuncNew_) {
        terminateSessionFuncNew_(info, needStartCaller);
    }
    return WSError::WS_OK;
}

void Session::SetTerminateSessionListenerNew(const NotifyTerminateSessionFuncNew& func)
{
    terminateSessionFuncNew_ = func;
}

WSError Session::TerminateSessionTotal(const sptr<AAFwk::SessionInfo> abilitySessionInfo, TerminateType terminateType)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (isTerminating) {
        WLOGFE("TerminateSessionTotal isTerminating, return!");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    isTerminating = true;
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
    {
        std::lock_guard<std::recursive_mutex> lock(sessionInfoMutex_);
        sessionInfo_.closeAbilityWant = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
        sessionInfo_.resultCode = abilitySessionInfo->resultCode;
    }
    if (terminateSessionFuncTotal_) {
        terminateSessionFuncTotal_(info, terminateType);
    }
    return WSError::WS_OK;
}

void Session::SetTerminateSessionListenerTotal(const NotifyTerminateSessionFuncTotal& func)
{
    terminateSessionFuncTotal_ = func;
}

WSError Session::SetSessionLabel(const std::string &label)
{
    WLOGFI("run Session::SetSessionLabel");
    if (updateSessionLabelFunc_) {
        updateSessionLabelFunc_(label);
    }
    return WSError::WS_OK;
}

void Session::SetUpdateSessionLabelListener(const NofitySessionLabelUpdatedFunc &func)
{
    updateSessionLabelFunc_ = func;
}

WSError Session::SetSessionIcon(const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFD("run Session::SetSessionIcon, id: %{public}d", GetPersistentId());
    if (scenePersistence_ == nullptr) {
        WLOGFE("scenePersistence_ is nullptr.");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    scenePersistence_->SaveUpdatedIcon(icon);
    std::string updatedIconPath = scenePersistence_->GetUpdatedIconPath();
    if (updateSessionIconFunc_) {
        updateSessionIconFunc_(updatedIconPath);
    }
    return WSError::WS_OK;
}

void Session::SetUpdateSessionIconListener(const NofitySessionIconUpdatedFunc &func)
{
    updateSessionIconFunc_ = func;
}

WSError Session::Clear()
{
    WLOGFI("Clear, id: %{public}d", GetPersistentId());
    if (isTerminating) {
        WLOGFE("Clear isTerminating, return!");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    isTerminating = true;
    SessionInfo info = GetSessionInfo();
    if (terminateSessionFuncNew_) {
        terminateSessionFuncNew_(info, false);
    }
    return WSError::WS_OK;
}

void Session::SetSessionExceptionListener(const NotifySessionExceptionFunc& func)
{
    if (func == nullptr) {
        WLOGFE("func is nullptr");
        return;
    }
    std::shared_ptr<NotifySessionExceptionFunc> funcSptr = std::make_shared<NotifySessionExceptionFunc>(func);
    if (std::find(sessionExceptionFuncs_.begin(), sessionExceptionFuncs_.end(), funcSptr) !=
        sessionExceptionFuncs_.end()) {
        WLOGFW("func already regitered");
        return;
    }
    sessionExceptionFuncs_.emplace_back(funcSptr);
}

void Session::SetSessionSnapshotListener(const NotifySessionSnapshotFunc& func)
{
    if (func == nullptr) {
        WLOGFE("func is nullptr");
        return;
    }
    notifySessionSnapshotFunc_ = func;
}

void Session::SetPendingSessionToForegroundListener(const NotifyPendingSessionToForegroundFunc& func)
{
    pendingSessionToForegroundFunc_ = func;
}

WSError Session::PendingSessionToForeground()
{
    WLOGFI("id: %{public}d", GetPersistentId());
    SessionInfo info = GetSessionInfo();
    if (pendingSessionActivationFunc_) {
        pendingSessionActivationFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetPendingSessionToBackgroundForDelegatorListener(
    const NotifyPendingSessionToBackgroundForDelegatorFunc& func)
{
    pendingSessionToBackgroundForDelegatorFunc_ = func;
}

WSError Session::PendingSessionToBackgroundForDelegator()
{
    WLOGFD("run PendingSessionToBackgroundForDelegator");
    SessionInfo info = GetSessionInfo();
    if (pendingSessionToBackgroundForDelegatorFunc_) {
        pendingSessionToBackgroundForDelegatorFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetNotifyCallingSessionUpdateRectFunc(const NotifyCallingSessionUpdateRectFunc& func)
{
    notifyCallingSessionUpdateRectFunc_ = func;
}

void Session::SetNotifyCallingSessionForegroundFunc(const NotifyCallingSessionForegroundFunc& func)
{
    notifyCallingSessionForegroundFunc_ = func;
}

void Session::SetNotifyCallingSessionBackgroundFunc(const NotifyCallingSessionBackgroundFunc& func)
{
    notifyCallingSessionBackgroundFunc_ = func;
}

void Session::SetRaiseToAppTopForPointDownFunc(const NotifyRaiseToTopForPointDownFunc& func)
{
    raiseToTopForPointDownFunc_ = func;
}

void Session::NotifyTouchDialogTarget()
{
    if (!sessionStage_) {
        return;
    }
    sessionStage_->NotifyTouchDialogTarget();
}

void Session::NotifyScreenshot()
{
    if (!sessionStage_) {
        return;
    }
    sessionStage_->NotifyScreenshot();
}

WSError Session::NotifyCloseExistPipWindow()
{
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyCloseExistPipWindow();
}

WSError Session::NotifyDestroy()
{
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyDestroy();
}

void Session::SetParentSession(const sptr<Session>& session)
{
    if (session == nullptr) {
        WLOGFW("Session is nullptr");
        return;
    }
    parentSession_ = session;
    WLOGFD("[WMSSystem][WMSSub] Set parent success, parentId: %{public}d, id: %{public}d",
        session->GetPersistentId(), GetPersistentId());
}

sptr<Session> Session::GetParentSession() const
{
    return parentSession_;
}

void Session::BindDialogToParentSession(const sptr<Session>& session)
{
    std::unique_lock<std::mutex> lock(dialogVecMutex_);
    auto iter = std::find(dialogVec_.begin(), dialogVec_.end(), session);
    if (iter != dialogVec_.end()) {
        WLOGFW("[WMSDialog] Dialog is existed in parentVec, id: %{public}d, parentId: %{public}d",
            session->GetPersistentId(), GetPersistentId());
        return;
    }
    dialogVec_.push_back(session);
    WLOGFD("[WMSDialog] Bind dialog success, id: %{public}d, parentId: %{public}d",
        session->GetPersistentId(), GetPersistentId());
}

void Session::RemoveDialogToParentSession(const sptr<Session>& session)
{
    std::unique_lock<std::mutex> lock(dialogVecMutex_);
    auto iter = std::find(dialogVec_.begin(), dialogVec_.end(), session);
    if (iter != dialogVec_.end()) {
        WLOGFD("[WMSDialog] Remove dialog success, id: %{public}d, parentId: %{public}d",
            session->GetPersistentId(), GetPersistentId());
        dialogVec_.erase(iter);
    }
    WLOGFW("[WMSDialog] Remove dialog failed, id: %{public}d, parentId: %{public}d",
        session->GetPersistentId(), GetPersistentId());
}

std::vector<sptr<Session>> Session::GetDialogVector() const
{
    std::unique_lock<std::mutex> lock(dialogVecMutex_);
    return dialogVec_;
}

void Session::ClearDialogVector()
{
    std::unique_lock<std::mutex> lock(dialogVecMutex_);
    dialogVec_.clear();
    WLOGFD("[WMSDialog] parentId: %{public}d", GetPersistentId());
    return;
}

bool Session::CheckDialogOnForeground()
{
    std::unique_lock<std::mutex> lock(dialogVecMutex_);
    if (dialogVec_.empty()) {
        WLOGFD("[WMSDialog] Dialog is empty, id: %{public}d", GetPersistentId());
        return false;
    }
    for (auto iter = dialogVec_.rbegin(); iter != dialogVec_.rend(); iter++) {
        auto dialogSession = *iter;
        if (dialogSession && (dialogSession->GetSessionState() == SessionState::STATE_ACTIVE ||
            dialogSession->GetSessionState() == SessionState::STATE_FOREGROUND)) {
            dialogSession->NotifyTouchDialogTarget();
            WLOGFD("[WMSDialog] Notify touch dialog window, id: %{public}d", GetPersistentId());
            return true;
        }
    }
    return false;
}

bool Session::CheckPointerEventDispatch(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const
{
    auto windowType = GetWindowType();
    bool isSystemWindow = GetSessionInfo().isSystem_;
    auto sessionState = GetSessionState();
    int32_t action = pointerEvent->GetPointerAction();
    if (!isSystemWindow && WindowHelper::IsMainWindow(windowType) &&
        sessionState != SessionState::STATE_FOREGROUND &&
        sessionState != SessionState::STATE_ACTIVE &&
        action != MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW) {
        WLOGFW("Current Session Info: [persistentId: %{public}d, isSystemWindow: %{public}d,"
            "state: %{public}d, action:%{public}d]", GetPersistentId(), isSystemWindow, state_, action);
        return false;
    }
    return true;
}

bool Session::CheckKeyEventDispatch(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    if (GetWindowType() != WindowType::WINDOW_TYPE_DIALOG) {
        return true;
    }

    auto currentRect = winRect_;
    if (!isRSVisible_ || currentRect.width_ == 0 || currentRect.height_ == 0) {
        WLOGE("Error size: [width: %{public}d, height: %{public}d], isRSVisible_: %{public}d,"
            " persistentId: %{public}d",
            currentRect.width_, currentRect.height_, isRSVisible_, GetPersistentId());
        return false;
    }

    auto parentSession = GetParentSession();
    if (parentSession == nullptr) {
        WLOGFW("Dialog parent is null");
        return false;
    }
    auto parentSessionState = parentSession->GetSessionState();
    if ((parentSessionState != SessionState::STATE_FOREGROUND &&
        parentSessionState != SessionState::STATE_ACTIVE) ||
        (state_ != SessionState::STATE_FOREGROUND &&
        state_ != SessionState::STATE_ACTIVE)) {
        WLOGFE("[WMSDialog] Dialog's parent info : [persistentId: %{publicd}d, state:%{public}d];"
            "Dialog info:[persistentId: %{publicd}d, state:%{public}d]",
            parentSession->GetPersistentId(), parentSessionState, GetPersistentId(), state_);
        return false;
    }
    return true;
}

bool Session::IsTopDialog() const
{
    int32_t currentPersistentId = GetPersistentId();
    auto parentSession = GetParentSession();
    if (parentSession == nullptr) {
        WLOGFW("[WMSDialog] Dialog's Parent is NULL. id: %{public}d", currentPersistentId);
        return false;
    }
    std::unique_lock<std::mutex> lock(parentSession->dialogVecMutex_);
    if (parentSession->dialogVec_.size() <= 1) {
        return true;
    }
    auto parentDialogVec = parentSession->dialogVec_;
    for (auto iter = parentDialogVec.rbegin(); iter != parentDialogVec.rend(); iter++) {
        auto dialogSession = *iter;
        if (dialogSession && (dialogSession->GetSessionState() == SessionState::STATE_ACTIVE ||
            dialogSession->GetSessionState() == SessionState::STATE_FOREGROUND)) {
            return dialogSession->GetPersistentId() == currentPersistentId;
        }
    }
    return false;
}

const char* Session::DumpPointerWindowArea(MMI::WindowArea area) const
{
    const std::map<MMI::WindowArea, const char*> areaMap = {
        { MMI::WindowArea::FOCUS_ON_INNER, "FOCUS_ON_INNER" },
        { MMI::WindowArea::FOCUS_ON_TOP, "FOCUS_ON_TOP" },
        { MMI::WindowArea::FOCUS_ON_BOTTOM, "FOCUS_ON_BOTTOM" },
        { MMI::WindowArea::FOCUS_ON_LEFT, "FOCUS_ON_LEFT" },
        { MMI::WindowArea::FOCUS_ON_RIGHT, "FOCUS_ON_RIGHT" },
        { MMI::WindowArea::FOCUS_ON_BOTTOM_LEFT, "FOCUS_ON_BOTTOM_LEFT" },
        { MMI::WindowArea::FOCUS_ON_BOTTOM_RIGHT, "FOCUS_ON_BOTTOM_RIGHT" },
        { MMI::WindowArea::FOCUS_ON_TOP_LEFT, "FOCUS_ON_TOP_LEFT" },
        { MMI::WindowArea::FOCUS_ON_TOP_RIGHT, "FOCUS_ON_TOP_RIGHT" },
        { MMI::WindowArea::EXIT, "EXIT" }
    };
    auto iter = areaMap.find(area);
    if (iter == areaMap.end()) {
        return "UNKNOW";
    }
    return iter->second;
}

WSError Session::RaiseToAppTopForPointDown()
{
    if (raiseToTopForPointDownFunc_) {
        raiseToTopForPointDownFunc_();
        WLOGFD("RaiseToAppTopForPointDown, id: %{public}d", GetPersistentId());
    }
    return WSError::WS_OK;
}

void Session::HandlePointDownDialog(int32_t pointAction)
{
    if (!(pointAction == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        pointAction == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN)) {
        WLOGFD("Point main window, action is not down, id: %{public}d", GetPersistentId());
        return;
    }
    for (auto dialog : dialogVec_) {
        if (dialog && (dialog->GetSessionState() == SessionState::STATE_FOREGROUND ||
            dialog->GetSessionState() == SessionState::STATE_ACTIVE)) {
            dialog->RaiseToAppTopForPointDown();
            dialog->PresentFoucusIfNeed(pointAction);
            WLOGFD("[WMSDialog] Point main window, raise to top and dialog need focus, "
                "id: %{public}d, dialogId: %{public}d", GetPersistentId(), dialog->GetPersistentId());
        }
    }
}

WSError Session::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("Session TransferPointEvent, id: %{public}d", GetPersistentId());
    if (!IsSystemSession() && !IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (pointerEvent == nullptr) {
        WLOGFE("PointerEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto pointerAction = pointerEvent->GetPointerAction();
    if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (CheckDialogOnForeground()) {
            HandlePointDownDialog(pointerAction);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
        if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
            WLOGFD("[WMSDialog] Its main window has dialog on foreground, id: %{public}d", GetPersistentId());
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
            parentSession_->HandlePointDownDialog(pointerAction);
            if (!IsTopDialog()) {
                WLOGFI("[WMSDialog] There is at least one active dialog upon this dialog, id: %{public}d",
                    GetPersistentId());
                return WSError::WS_ERROR_INVALID_PERMISSION;
            }
        }
    }
    if (DelayedSingleton<ANRManager>::GetInstance()->IsANRTriggered(persistentId_)) {
        WLOGFW("InputTracking id:%{public}d, The pointerEvent does not report normally,"
            "bundleName:%{public}s not reponse, pid:%{public}d, persistentId:%{public}d",
            pointerEvent->GetId(), callingBundleName_.c_str(), callingPid_, persistentId_);
        return WSError::WS_DO_NOTHING;
    }
    PresentFoucusIfNeed(pointerAction);
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }

    if (WSError ret = windowEventChannel_->TransferPointerEvent(pointerEvent); ret != WSError::WS_OK) {
        WLOGFE("InputTracking id:%{public}d, TransferPointer failed, ret:%{public}d ",
            pointerEvent->GetId(), ret);
        return ret;
    }
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_MOVE ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_MOVE) {
        WLOGFD("Session TransferPointEvent, eventId:%{public}d, action:%{public}s, persistentId:%{public}d, "
            "bundleName:%{public}s, pid:%{public}d", pointerEvent->GetId(), pointerEvent->DumpPointerAction(),
            persistentId_, callingBundleName_.c_str(), callingPid_);
    } else {
        WLOGFI("Session TransferPointEvent, eventId:%{public}d, action:%{public}s, persistentId:%{public}d, "
            "bundleName:%{public}s, pid:%{public}d", pointerEvent->GetId(), pointerEvent->DumpPointerAction(),
            persistentId_, callingBundleName_.c_str(), callingPid_);
    }
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW) {
        WLOGFD("Action:%{public}s, eventId:%{public}d, report without timer",
            pointerEvent->DumpPointerAction(), pointerEvent->GetId());
    } else {
        DelayedSingleton<ANRManager>::GetInstance()->AddTimer(pointerEvent->GetId(), persistentId_);
    }
    return WSError::WS_OK;
}

WSError Session::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (!IsSystemSession() && !IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (CheckDialogOnForeground()) {
            WLOGFD("[WMSDialog] Has dialog on foreground, not transfer pointer event");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
        if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
            WLOGFD("[WMSDialog] Its main window has dialog on foreground, not transfer pointer event");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        if (keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_BACK) {
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (parentSession_ && parentSession_->CheckDialogOnForeground() &&
            !IsTopDialog()) {
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    }

    if (!CheckKeyEventDispatch(keyEvent)) {
        WLOGFW("Do not dispatch the key event.");
        return WSError::WS_DO_NOTHING;
    }

    WLOGFD("Session TransferKeyEvent eventId:%{public}d persistentId:%{public}d bundleName:%{public}s pid:%{public}d",
        keyEvent->GetId(), persistentId_, callingBundleName_.c_str(), callingPid_);
    if (DelayedSingleton<ANRManager>::GetInstance()->IsANRTriggered(persistentId_)) {
        WLOGFD("The keyEvent does not report normally, "
            "bundleName:%{public}s not response, pid:%{public}d, persistentId:%{public}d",
            callingBundleName_.c_str(), callingPid_, persistentId_);
        return WSError::WS_DO_NOTHING;
    }
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    WLOGD("TransferKeyEvent, id: %{public}d", persistentId_);
    if (WSError ret = windowEventChannel_->TransferKeyEvent(keyEvent); ret != WSError::WS_OK) {
        WLOGFE("TransferKeyEvent failed, ret:%{public}d", ret);
        return ret;
    }
    DelayedSingleton<ANRManager>::GetInstance()->AddTimer(keyEvent->GetId(), persistentId_);
    return WSError::WS_OK;
}

WSError Session::TransferBackPressedEventForConsumed(bool& isConsumed)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferBackpressedEventForConsumed(isConsumed);
}

WSError Session::TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferKeyEventForConsumed(keyEvent, isConsumed);
}

WSError Session::TransferFocusActiveEvent(bool isFocusActive)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferFocusActiveEvent(isFocusActive);
}

WSError Session::TransferFocusStateEvent(bool focusState)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferFocusState(focusState);
}

WSError Session::UpdateConfiguration()
{
    if (!sessionStage_) {
        WLOGFE("session stage is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_->NotifyConfigurationUpdated();
    return WSError::WS_OK;
}

std::shared_ptr<Media::PixelMap> Session::Snapshot() const
{
    if (!surfaceNode_ || !surfaceNode_->IsBufferAvailable()) {
        return nullptr;
    }
    auto callback = std::make_shared<SurfaceCaptureFuture>();
    bool ret = RSInterfaces::GetInstance().TakeSurfaceCapture(surfaceNode_, callback, snapshotScale_, snapshotScale_);
    if (!ret) {
        WLOGFE("TakeSurfaceCapture failed");
        return nullptr;
    }
    auto pixelMap = callback->GetResult(2000); // wait for <= 2000ms
    if (pixelMap != nullptr) {
        WLOGFD("Save pixelMap WxH = %{public}dx%{public}d", pixelMap->GetWidth(), pixelMap->GetHeight());
        if (notifySessionSnapshotFunc_) {
            notifySessionSnapshotFunc_(persistentId_);
        }
    } else {
        WLOGFE("Failed to get pixelMap, return nullptr");
    }
    return pixelMap;
}

void Session::SetSessionStateChangeListenser(const NotifySessionStateChangeFunc& func)
{
    sessionStateChangeFunc_ = func;
    NotifySessionStateChange(state_);
    WLOGFD("SetSessionStateChangeListenser, id: %{public}d", GetPersistentId());
}

void Session::UnregisterSessionChangeListeners()
{
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (session == nullptr) {
            WLOGFE("session is null");
            return;
        }
        session->sessionStateChangeFunc_ = nullptr;
        session->sessionFocusableChangeFunc_ = nullptr;
        session->sessionTouchableChangeFunc_ = nullptr;
        session->clickFunc_ = nullptr;
        WLOGFD("UnregisterSessionChangeListenser, id: %{public}d", session->GetPersistentId());
    });
}

void Session::SetSessionStateChangeNotifyManagerListener(const NotifySessionStateChangeNotifyManagerFunc& func)
{
    sessionStateChangeNotifyManagerFunc_ = func;
    NotifySessionStateChange(state_);
}

void Session::SetRequestFocusStatusNotifyManagerListener(const NotifyRequestFocusStatusNotifyManagerFunc& func)
{
    requestFocusStatusNotifyManagerFunc_ = func;
}

void Session::SetNotifyUIRequestFocusFunc(const NotifyUIRequestFocusFunc& func)
{
    requestFocusFunc_ = func;
}

void Session::SetNotifyUILostFocusFunc(const NotifyUILostFocusFunc& func)
{
    lostFocusFunc_ = func;
}

void Session::SetGetStateFromManagerListener(const GetStateFromManagerFunc& func)
{
    getStateFromManagerFunc_ = func;
}

void Session::NotifySessionStateChange(const SessionState& state)
{
    PostTask([weakThis = wptr(this), state]() {
        auto session = weakThis.promote();
        if (session == nullptr) {
            WLOGFE("session is null");
            return;
        }
        WLOGD("Session::NotifySessionStateChange: session info: [state: %{public}u, persistent: %{public}d]",
            static_cast<uint32_t>(state), session->GetPersistentId());
        if (session->sessionStateChangeFunc_) {
            session->sessionStateChangeFunc_(state);
        }

        if (session->sessionStateChangeNotifyManagerFunc_) {
            session->sessionStateChangeNotifyManagerFunc_(session->GetPersistentId(), state);
        }
    });
}

void Session::SetSessionFocusableChangeListener(const NotifySessionFocusableChangeFunc& func)
{
    sessionFocusableChangeFunc_ = func;
    sessionFocusableChangeFunc_(GetFocusable());
}

void Session::SetSessionTouchableChangeListener(const NotifySessionTouchableChangeFunc& func)
{
    sessionTouchableChangeFunc_ = func;
    sessionTouchableChangeFunc_(GetTouchable());
}

void Session::SetClickListener(const NotifyClickFunc& func)
{
    clickFunc_ = func;
}

void Session::NotifySessionFocusableChange(bool isFocusable)
{
    WLOGFI("Notify session focusable change, id: %{public}d, focusable: %{public}u", GetPersistentId(), isFocusable);
    if (sessionFocusableChangeFunc_) {
        sessionFocusableChangeFunc_(isFocusable);
    }
}

void Session::NotifySessionTouchableChange(bool touchable)
{
    WLOGFD("Notify session touchable change: %{public}u", touchable);
    if (sessionTouchableChangeFunc_) {
        sessionTouchableChangeFunc_(touchable);
    }
}

void Session::NotifyClick()
{
    WLOGFD("Notify click");
    if (clickFunc_) {
        clickFunc_();
    }
}

void Session::NotifyRequestFocusStatusNotifyManager(bool isFocused)
{
    WLOGFD("NotifyRequestFocusStatusNotifyManager id: %{public}d, focused: %{public}d", GetPersistentId(), isFocused);
    if (requestFocusStatusNotifyManagerFunc_) {
        requestFocusStatusNotifyManagerFunc_(GetPersistentId(), isFocused);
    }
}

bool Session::GetStateFromManager(const ManagerState key)
{
    if (getStateFromManagerFunc_) {
        return getStateFromManagerFunc_(key);
    }
    switch (key)
    {
    case ManagerState::MANAGER_STATE_SCREEN_LOCKED:
        return false;
        break;
    default:
        return false;
    }
}

void Session::NotifyUIRequestFocus()
{
    WLOGFD("NotifyUIRequestFocus id: %{public}d", GetPersistentId());
    if (requestFocusFunc_) {
        requestFocusFunc_();
    }
}

void Session::NotifyUILostFocus()
{
    WLOGFD("NotifyUILostFocus id: %{public}d", GetPersistentId());
    if (lostFocusFunc_) {
        lostFocusFunc_();
    }
}

void Session::PresentFoucusIfNeed(int32_t pointerAction)
{
    WLOGFD("OnClick down, id: %{public}d", GetPersistentId());
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        if (!isFocused_ && GetFocusable()) {
            NotifyRequestFocusStatusNotifyManager(true);
        }
        if (!sessionInfo_.isSystem_ || (!isFocused_ && GetFocusable())) {
            NotifyClick();
        }
    }
}

WSError Session::UpdateFocus(bool isFocused)
{
    WLOGFD("Session update focus id: %{public}d", GetPersistentId());
    if (isFocused_ == isFocused) {
        WLOGFD("Session focus do not change: [%{public}d]", isFocused);
        return WSError::WS_DO_NOTHING;
    }
    isFocused_ = isFocused;
    // notify scb arkui focus
    if (isFocused) {
        if (sessionInfo_.isSystem_) {
            NotifyUIRequestFocus();
        }
    } else {
         NotifyUILostFocus();
    }
    return WSError::WS_OK;
}

WSError Session::NotifyFocusStatus(bool isFocused)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionStage_->UpdateFocus(isFocused);

    return WSError::WS_OK;
}

WSError Session::UpdateWindowMode(WindowMode mode)
{
    WLOGFD("Session update window mode, id: %{public}d, mode: %{public}d", GetPersistentId(),
        static_cast<int32_t>(mode));
    if (sessionInfo_.isSystem_) {
        WLOGFD("session is system, id: %{public}d, name: %{public}s, state: %{public}u",
            GetPersistentId(), sessionInfo_.bundleName_.c_str(), state_);
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    if (property_ == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return WSError::WS_ERROR_NULLPTR;
    }

    if (state_ == SessionState::STATE_END) {
        WLOGFI("session is already destroyed or property is nullptr! id: %{public}d state: %{public}u",
            GetPersistentId(), state_);
        return WSError::WS_ERROR_INVALID_SESSION;
    } else if (state_ == SessionState::STATE_DISCONNECT) {
        property_->SetWindowMode(mode);
        property_->SetIsNeedUpdateWindowMode(true);
    } else {
        property_->SetWindowMode(mode);
        if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
        }
        return sessionStage_->UpdateWindowMode(mode);
    }
    return WSError::WS_OK;
}

WSError Session::SetSessionProperty(const sptr<WindowSessionProperty>& property)
{
    std::unique_lock<std::shared_mutex> lock(propertyMutex_);
    property_ = property;
    return WSError::WS_OK;
}

sptr<WindowSessionProperty> Session::GetSessionProperty() const
{
    std::shared_lock<std::shared_mutex> lock(propertyMutex_);
    return property_;
}

void Session::SetSessionRect(const WSRect& rect)
{
    if (winRect_ == rect) {
        WLOGFW("id: %{public}d skip same rect", persistentId_);
        return;
    }
    winRect_ = rect;
    isDirty_ = true;
}

WSRect Session::GetSessionRect() const
{
    return winRect_;
}

void Session::SetSessionRequestRect(const WSRect& rect)
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return;
    }
    property->SetRequestRect(SessionHelper::TransferToRect(rect));
    WLOGFD("is: %{public}d, rect: [%{public}d, %{public}d, %{public}u, %{public}u]", persistentId_,
        rect.posX_, rect.posY_, rect.width_, rect.height_);
}

WSRect Session::GetSessionRequestRect() const
{
    WSRect rect;
    auto property = GetSessionProperty();
    if (property == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return rect;
    }
    rect = SessionHelper::TransferToWSRect(property->GetRequestRect());
    WLOGFD("id: %{public}d, rect: %{public}s", persistentId_, rect.ToString().c_str());
    return rect;
}

WindowType Session::GetWindowType() const
{
    auto property = GetSessionProperty();
    if (property) {
        return property->GetWindowType();
    }
    return WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
}

void Session::SetSystemConfig(const SystemSessionConfig& systemConfig)
{
    systemConfig_ = systemConfig;
}

void Session::SetSnapshotScale(const float snapshotScale)
{
    snapshotScale_ = snapshotScale;
}

WSError Session::ProcessBackEvent()
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        WLOGFI("[WMSDialog] this is dialog, id: %{public}d", GetPersistentId());
        return WSError::WS_OK;
    }
    return sessionStage_->HandleBackEvent();
}

WSError Session::MarkProcessed(int32_t eventId)
{
    int32_t persistentId = GetPersistentId();
    WLOGFI("InputTracking persistentId:%{public}d, eventId:%{public}d", persistentId, eventId);
    DelayedSingleton<ANRManager>::GetInstance()->MarkProcessed(eventId, persistentId);
    return WSError::WS_OK;
}

void Session::GeneratePersistentId(bool isExtension, int32_t persistentId)
{
    if (persistentId != INVALID_SESSION_ID  && !g_persistentIdSet.count(g_persistentId)) {
        g_persistentIdSet.insert(persistentId);
        persistentId_ = persistentId;
        return;
    }

    if (g_persistentId == INVALID_SESSION_ID) {
        g_persistentId++; // init non system session id from 2
    }

    g_persistentId++;
    while (g_persistentIdSet.count(g_persistentId)) {
        g_persistentId++;
    }
    persistentId_ = isExtension ? static_cast<uint32_t>(
        g_persistentId.load()) | 0x40000000 : static_cast<uint32_t>(g_persistentId.load()) & 0x3fffffff;
    g_persistentIdSet.insert(g_persistentId);
    WLOGFI("GeneratePersistentId, persistentId: %{public}d, persistentId_: %{public}d", persistentId, persistentId_);
}

sptr<ScenePersistence> Session::GetScenePersistence() const
{
    return scenePersistence_;
}

void Session::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info)
{
    if (!sessionStage_) {
        WLOGFE("session stage is nullptr");
        return;
    }
    sessionStage_->NotifyOccupiedAreaChangeInfo(info);
}

WindowMode Session::GetWindowMode()
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        WLOGFW("null property.");
        return WindowMode::WINDOW_MODE_UNDEFINED;
    }
    return property->GetWindowMode();
}

WSError Session::UpdateMaximizeMode(bool isMaximize)
{
    WLOGFD("Session update maximize mode, isMaximize: %{public}d", isMaximize);
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    MaximizeMode mode = MaximizeMode::MODE_RECOVER;
    if (isMaximize) {
        mode = MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    } else if (GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
        mode = MaximizeMode::MODE_FULL_FILL;
    }
    return sessionStage_->UpdateMaximizeMode(mode);
}

void Session::SetZOrder(uint32_t zOrder)
{
    zOrder_ = zOrder;
}

uint32_t Session::GetZOrder() const
{
    return zOrder_;
}

void Session::SetUINodeId(uint32_t uiNodeId)
{
    uiNodeId_ = uiNodeId;
}

uint32_t Session::GetUINodeId() const
{
    return uiNodeId_;
}

void Session::SetShowRecent(bool showRecent)
{
    showRecent_ = showRecent;
}

bool Session::GetShowRecent() const
{
    return showRecent_;
}

void Session::SetBufferAvailable(bool bufferAvailable)
{
    bufferAvailable_ = bufferAvailable;
}

bool Session::GetBufferAvailable() const
{
    return bufferAvailable_;
}

void Session::SetNeedSnapshot(bool needSnapshot)
{
    needSnapshot_ = needSnapshot;
}

void Session::SetFloatingScale(float floatingScale)
{
    floatingScale_ = floatingScale;
}

float Session::GetFloatingScale() const
{
    return floatingScale_;
}

void Session::SetSCBKeepKeyboard(bool scbKeepKeyboardFlag)
{
    scbKeepKeyboardFlag_ = scbKeepKeyboardFlag;
}

bool Session::GetSCBKeepKeyboardFlag() const
{
    return scbKeepKeyboardFlag_;
}

WSError Session::TransferSearchElementInfo(int32_t elementId, int32_t mode, int32_t baseParent,
    std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
}

WSError Session::TransferSearchElementInfosByText(int32_t elementId, const std::string& text, int32_t baseParent,
    std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
}

WSError Session::TransferFindFocusedElementInfo(int32_t elementId, int32_t focusType, int32_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
}

WSError Session::TransferFocusMoveSearch(int32_t elementId, int32_t direction, int32_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
}

WSError Session::TransferExecuteAction(int32_t elementId, const std::map<std::string, std::string>& actionArguments,
    int32_t action, int32_t baseParent)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
}

void Session::SetSessionInfoLockedStateChangeListener(const NotifySessionInfoLockedStateChangeFunc& func)
{
    sessionInfoLockedStateChangeFunc_ = func;
}

void Session::NotifySessionInfoLockedStateChange(bool lockedState)
{
    WLOGFD("Notify sessioninfo lockedstate change: %{public}u", lockedState);
    if (sessionInfoLockedStateChangeFunc_) {
        sessionInfoLockedStateChangeFunc_(lockedState);
    }
}

WSError Session::UpdateTitleInTargetPos(bool isShow, int32_t height)
{
    WLOGFD("Session update title in target position, id: %{public}d, isShow: %{public}d, height: %{public}d",
        GetPersistentId(), isShow, height);
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    return sessionStage_->UpdateTitleInTargetPos(isShow, height);
}
} // namespace OHOS::Rosen
