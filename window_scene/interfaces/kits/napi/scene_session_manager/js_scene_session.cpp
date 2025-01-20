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

#include "pixel_map_napi.h"
#include "session/host/include/ability_info_manager.h"
#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"
#include "common/include/session_permission.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSession" };
const std::string PENDING_SCENE_CB = "pendingSceneSessionActivation";
const std::string CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR = "changeSessionVisibilityWithStatusBar";
const std::string SESSION_STATE_CHANGE_CB = "sessionStateChange";
const std::string BUFFER_AVAILABLE_CHANGE_CB = "bufferAvailableChange";
const std::string SESSION_EVENT_CB = "sessionEvent";
const std::string SESSION_RECT_CHANGE_CB = "sessionRectChange";
const std::string WINDOW_MOVING_CB = "windowMoving";
const std::string SESSION_PIP_CONTROL_STATUS_CHANGE_CB = "sessionPiPControlStatusChange";
const std::string SESSION_AUTO_START_PIP_CB = "autoStartPiP";
const std::string CREATE_SUB_SESSION_CB = "createSpecificSession";
const std::string BIND_DIALOG_TARGET_CB = "bindDialogTarget";
const std::string RAISE_TO_TOP_CB = "raiseToTop";
const std::string RAISE_TO_TOP_POINT_DOWN_CB = "raiseToTopForPointDown";
const std::string CLICK_MODAL_WINDOW_OUTSIDE_CB = "clickModalWindowOutside";
const std::string BACK_PRESSED_CB = "backPressed";
const std::string SESSION_FOCUSABLE_CHANGE_CB = "sessionFocusableChange";
const std::string SESSION_TOUCHABLE_CHANGE_CB = "sessionTouchableChange";
const std::string SESSION_TOP_MOST_CHANGE_CB = "sessionTopmostChange";
const std::string SUB_MODAL_TYPE_CHANGE_CB = "subModalTypeChange";
const std::string MAIN_MODAL_TYPE_CHANGE_CB = "mainModalTypeChange";
const std::string THROW_SLIP_ANIMATION_STATE_CHANGE_CB = "throwSlipAnimationStateChange";
const std::string FULLSCREEN_WATERFALL_MODE_CHANGE_CB = "fullScreenWaterfallModeChange";
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
const std::string CUSTOM_ANIMATION_PLAYING_CB = "isCustomAnimationPlaying";
const std::string NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB = "needDefaultAnimationFlagChange";
const std::string SHOW_WHEN_LOCKED_CB = "sessionShowWhenLockedChange";
const std::string REQUESTED_ORIENTATION_CHANGE_CB = "sessionRequestedOrientationChange";
const std::string RAISE_ABOVE_TARGET_CB = "raiseAboveTarget";
const std::string FORCE_HIDE_CHANGE_CB = "sessionForceHideChange";
const std::string WINDOW_DRAG_HOT_AREA_CB = "windowDragHotArea";
const std::string TOUCH_OUTSIDE_CB = "touchOutside";
const std::string SESSIONINFO_LOCKEDSTATE_CHANGE_CB = "sessionInfoLockedStateChange";
const std::string PREPARE_CLOSE_PIP_SESSION = "prepareClosePiPSession";
const std::string LANDSCAPE_MULTI_WINDOW_CB = "landscapeMultiWindow";
const std::string CONTEXT_TRANSPARENT_CB = "contextTransparent";
const std::string ADJUST_KEYBOARD_LAYOUT_CB = "adjustKeyboardLayout";
const std::string LAYOUT_FULL_SCREEN_CB = "layoutFullScreenChange";
const std::string DEFAULT_DENSITY_ENABLED_CB = "defaultDensityEnabled";
const std::string TITLE_DOCK_HOVER_SHOW_CB = "titleAndDockHoverShowChange";
const std::string RESTORE_MAIN_WINDOW_CB = "restoreMainWindow";
const std::string NEXT_FRAME_LAYOUT_FINISH_CB = "nextFrameLayoutFinish";
const std::string PRIVACY_MODE_CHANGE_CB = "privacyModeChange";
const std::string MAIN_WINDOW_TOP_MOST_CHANGE_CB = "mainWindowTopmostChange";
const std::string SET_WINDOW_RECT_AUTO_SAVE_CB = "setWindowRectAutoSave";
const std::string UPDATE_APP_USE_CONTROL_CB = "updateAppUseControl";
const std::string SESSION_DISPLAY_ID_CHANGE_CB = "sessionDisplayIdChange";
const std::string SET_SUPPORT_WINDOW_MODES_CB = "setSupportWindowModes";
const std::string SESSION_LOCK_STATE_CHANGE_CB = "sessionLockStateChange";
const std::string UPDATE_SESSION_LABEL_AND_ICON_CB = "updateSessionLabelAndIcon";
const std::string KEYBOARD_STATE_CHANGE_CB = "keyboardStateChange";
const std::string KEYBOARD_VIEW_MODE_CHANGE_CB = "keyboardViewModeChange";
const std::string SET_WINDOW_CORNER_RADIUS_CB = "setWindowCornerRadius";

constexpr int ARG_COUNT_3 = 3;
constexpr int ARG_COUNT_4 = 4;
constexpr int ARG_INDEX_0 = 0;
constexpr int ARG_INDEX_1 = 1;
constexpr int ARG_INDEX_2 = 2;
constexpr int ARG_INDEX_3 = 3;

const std::map<std::string, ListenerFuncType> ListenerFuncMap {
    {PENDING_SCENE_CB,                      ListenerFuncType::PENDING_SCENE_CB},
    {CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR,
        ListenerFuncType::CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR},
    {SESSION_STATE_CHANGE_CB,               ListenerFuncType::SESSION_STATE_CHANGE_CB},
    {BUFFER_AVAILABLE_CHANGE_CB,            ListenerFuncType::BUFFER_AVAILABLE_CHANGE_CB},
    {SESSION_EVENT_CB,                      ListenerFuncType::SESSION_EVENT_CB},
    {SESSION_RECT_CHANGE_CB,                ListenerFuncType::SESSION_RECT_CHANGE_CB},
    {SESSION_PIP_CONTROL_STATUS_CHANGE_CB,  ListenerFuncType::SESSION_PIP_CONTROL_STATUS_CHANGE_CB},
    {SESSION_AUTO_START_PIP_CB,             ListenerFuncType::SESSION_AUTO_START_PIP_CB},
    {CREATE_SUB_SESSION_CB,                 ListenerFuncType::CREATE_SUB_SESSION_CB},
    {BIND_DIALOG_TARGET_CB,                 ListenerFuncType::BIND_DIALOG_TARGET_CB},
    {RAISE_TO_TOP_CB,                       ListenerFuncType::RAISE_TO_TOP_CB},
    {RAISE_TO_TOP_POINT_DOWN_CB,            ListenerFuncType::RAISE_TO_TOP_POINT_DOWN_CB},
    {CLICK_MODAL_WINDOW_OUTSIDE_CB,         ListenerFuncType::CLICK_MODAL_WINDOW_OUTSIDE_CB},
    {BACK_PRESSED_CB,                       ListenerFuncType::BACK_PRESSED_CB},
    {SESSION_FOCUSABLE_CHANGE_CB,           ListenerFuncType::SESSION_FOCUSABLE_CHANGE_CB},
    {SESSION_TOUCHABLE_CHANGE_CB,           ListenerFuncType::SESSION_TOUCHABLE_CHANGE_CB},
    {SESSION_TOP_MOST_CHANGE_CB,            ListenerFuncType::SESSION_TOP_MOST_CHANGE_CB},
    {SUB_MODAL_TYPE_CHANGE_CB,              ListenerFuncType::SUB_MODAL_TYPE_CHANGE_CB},
    {MAIN_MODAL_TYPE_CHANGE_CB,             ListenerFuncType::MAIN_MODAL_TYPE_CHANGE_CB},
    {THROW_SLIP_ANIMATION_STATE_CHANGE_CB,  ListenerFuncType::THROW_SLIP_ANIMATION_STATE_CHANGE_CB},
    {FULLSCREEN_WATERFALL_MODE_CHANGE_CB,   ListenerFuncType::FULLSCREEN_WATERFALL_MODE_CHANGE_CB},
    {CLICK_CB,                              ListenerFuncType::CLICK_CB},
    {TERMINATE_SESSION_CB,                  ListenerFuncType::TERMINATE_SESSION_CB},
    {TERMINATE_SESSION_CB_NEW,              ListenerFuncType::TERMINATE_SESSION_CB_NEW},
    {TERMINATE_SESSION_CB_TOTAL,            ListenerFuncType::TERMINATE_SESSION_CB_TOTAL},
    {SESSION_EXCEPTION_CB,                  ListenerFuncType::SESSION_EXCEPTION_CB},
    {UPDATE_SESSION_LABEL_CB,               ListenerFuncType::UPDATE_SESSION_LABEL_CB},
    {UPDATE_SESSION_ICON_CB,                ListenerFuncType::UPDATE_SESSION_ICON_CB},
    {SYSTEMBAR_PROPERTY_CHANGE_CB,          ListenerFuncType::SYSTEMBAR_PROPERTY_CHANGE_CB},
    {NEED_AVOID_CB,                         ListenerFuncType::NEED_AVOID_CB},
    {PENDING_SESSION_TO_FOREGROUND_CB,      ListenerFuncType::PENDING_SESSION_TO_FOREGROUND_CB},
    {PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB,
        ListenerFuncType::PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB},
    {CUSTOM_ANIMATION_PLAYING_CB,           ListenerFuncType::CUSTOM_ANIMATION_PLAYING_CB},
    {NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB, ListenerFuncType::NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB},
    {SHOW_WHEN_LOCKED_CB,                   ListenerFuncType::SHOW_WHEN_LOCKED_CB},
    {REQUESTED_ORIENTATION_CHANGE_CB,       ListenerFuncType::REQUESTED_ORIENTATION_CHANGE_CB},
    {RAISE_ABOVE_TARGET_CB,                 ListenerFuncType::RAISE_ABOVE_TARGET_CB},
    {FORCE_HIDE_CHANGE_CB,                  ListenerFuncType::FORCE_HIDE_CHANGE_CB},
    {WINDOW_DRAG_HOT_AREA_CB,               ListenerFuncType::WINDOW_DRAG_HOT_AREA_CB},
    {TOUCH_OUTSIDE_CB,                      ListenerFuncType::TOUCH_OUTSIDE_CB},
    {SESSIONINFO_LOCKEDSTATE_CHANGE_CB,     ListenerFuncType::SESSIONINFO_LOCKEDSTATE_CHANGE_CB},
    {PREPARE_CLOSE_PIP_SESSION,             ListenerFuncType::PREPARE_CLOSE_PIP_SESSION},
    {LANDSCAPE_MULTI_WINDOW_CB,             ListenerFuncType::LANDSCAPE_MULTI_WINDOW_CB},
    {CONTEXT_TRANSPARENT_CB,                ListenerFuncType::CONTEXT_TRANSPARENT_CB},
    {ADJUST_KEYBOARD_LAYOUT_CB,             ListenerFuncType::ADJUST_KEYBOARD_LAYOUT_CB},
    {LAYOUT_FULL_SCREEN_CB,                 ListenerFuncType::LAYOUT_FULL_SCREEN_CB},
    {DEFAULT_DENSITY_ENABLED_CB,            ListenerFuncType::DEFAULT_DENSITY_ENABLED_CB},
    {TITLE_DOCK_HOVER_SHOW_CB,              ListenerFuncType::TITLE_DOCK_HOVER_SHOW_CB},
    {NEXT_FRAME_LAYOUT_FINISH_CB,           ListenerFuncType::NEXT_FRAME_LAYOUT_FINISH_CB},
    {PRIVACY_MODE_CHANGE_CB,                ListenerFuncType::PRIVACY_MODE_CHANGE_CB},
    {RESTORE_MAIN_WINDOW_CB,                ListenerFuncType::RESTORE_MAIN_WINDOW_CB},
    {PRIVACY_MODE_CHANGE_CB,           ListenerFuncType::PRIVACY_MODE_CHANGE_CB},
    {MAIN_WINDOW_TOP_MOST_CHANGE_CB,        ListenerFuncType::MAIN_WINDOW_TOP_MOST_CHANGE_CB},
    {SET_WINDOW_RECT_AUTO_SAVE_CB,          ListenerFuncType::SET_WINDOW_RECT_AUTO_SAVE_CB},
    {UPDATE_APP_USE_CONTROL_CB,             ListenerFuncType::UPDATE_APP_USE_CONTROL_CB},
    {SESSION_DISPLAY_ID_CHANGE_CB,          ListenerFuncType::SESSION_DISPLAY_ID_CHANGE_CB},
    {SET_SUPPORT_WINDOW_MODES_CB,           ListenerFuncType::SET_SUPPORT_WINDOW_MODES_CB},
    {WINDOW_MOVING_CB,                      ListenerFuncType::WINDOW_MOVING_CB},
    {SESSION_LOCK_STATE_CHANGE_CB,          ListenerFuncType::SESSION_LOCK_STATE_CHANGE_CB},
    {UPDATE_SESSION_LABEL_AND_ICON_CB,      ListenerFuncType::UPDATE_SESSION_LABEL_AND_ICON_CB},
    {KEYBOARD_STATE_CHANGE_CB,              ListenerFuncType::KEYBOARD_STATE_CHANGE_CB},
    {KEYBOARD_VIEW_MODE_CHANGE_CB,         ListenerFuncType::KEYBOARD_VIEW_MODE_CHANGE_CB},
    {SET_WINDOW_CORNER_RADIUS_CB,           ListenerFuncType::SET_WINDOW_CORNER_RADIUS_CB},
};

const std::vector<std::string> g_syncGlobalPositionPermission {
    "Recent",
};
} // namespace

std::map<int32_t, napi_ref> JsSceneSession::jsSceneSessionMap_;

napi_value CreateJsPiPControlStatusObject(napi_env env, PiPControlStatusInfo controlStatusInfo)
{
    TLOGI(WmsLogTag::WMS_PIP, "in");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "objValue is nullptr");
        return NapiGetUndefined(env);
    }
    uint32_t controlType = static_cast<uint32_t>(controlStatusInfo.controlType);
    int32_t status = static_cast<int32_t>(controlStatusInfo.status);
    napi_set_named_property(env, objValue, "controlType", CreateJsValue(env, controlType));
    napi_set_named_property(env, objValue, "status", CreateJsValue(env, status));
    return objValue;
}

napi_value CreateJsPiPControlEnableObject(napi_env env, PiPControlEnableInfo controlEnableInfo)
{
    TLOGI(WmsLogTag::WMS_PIP, "in");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "objValue is nullptr");
        return NapiGetUndefined(env);
    }
    uint32_t controlType = static_cast<uint32_t>(controlEnableInfo.controlType);
    int32_t enabled = static_cast<int32_t>(controlEnableInfo.enabled);
    napi_set_named_property(env, objValue, "controlType", CreateJsValue(env, controlType));
    napi_set_named_property(env, objValue, "enabled", CreateJsValue(env, enabled));
    return objValue;
}

static napi_value CreatePipTemplateInfo(napi_env env, const sptr<SceneSession>& session)
{
    TLOGI(WmsLogTag::WMS_PIP, "in");
    napi_value pipTemplateInfoValue = nullptr;
    napi_create_object(env, &pipTemplateInfoValue);
    napi_set_named_property(env, pipTemplateInfoValue, "pipTemplateType",
        CreateJsValue(env, session->GetPiPTemplateInfo().pipTemplateType));
    napi_set_named_property(env, pipTemplateInfoValue, "priority",
        CreateJsValue(env, session->GetPiPTemplateInfo().priority));
    napi_value controlArrayValue = nullptr;
    std::vector<std::uint32_t> controlGroups = session->GetPiPTemplateInfo().controlGroup;
    napi_create_array_with_length(env, controlGroups.size(), &controlArrayValue);
    auto index = 0;
    for (const auto& controlGroup : controlGroups) {
        napi_set_element(env, controlArrayValue, index++, CreateJsValue(env, controlGroup));
    }
    napi_set_named_property(env, pipTemplateInfoValue, "controlGroup", controlArrayValue);
    napi_value controlStatusArrayValue = nullptr;
    std::vector<PiPControlStatusInfo> controlStatusInfoList = session->GetPiPTemplateInfo().pipControlStatusInfoList;
    napi_create_array_with_length(env, controlStatusInfoList.size(), &controlStatusArrayValue);
    auto controlStatusIndex = 0;
    for (const auto& controlStatus : controlStatusInfoList) {
        napi_set_element(env, controlStatusArrayValue, controlStatusIndex++,
            CreateJsPiPControlStatusObject(env, controlStatus));
    }
    napi_set_named_property(env, pipTemplateInfoValue, "pipControlStatusInfoList", controlStatusArrayValue);
    napi_value controlEnableArrayValue = nullptr;
    std::vector<PiPControlEnableInfo> controlEnableInfoList = session->GetPiPTemplateInfo().pipControlEnableInfoList;
    napi_create_array_with_length(env, controlEnableInfoList.size(), &controlEnableArrayValue);
    auto controlEnableIndex = 0;
    for (const auto& controlEnableInfo : controlEnableInfoList) {
        napi_set_element(env, controlEnableArrayValue, controlEnableIndex++,
            CreateJsPiPControlEnableObject(env, controlEnableInfo));
    }
    napi_set_named_property(env, pipTemplateInfoValue, "pipControlEnableInfoList", controlEnableArrayValue);
    return pipTemplateInfoValue;
}

static void SetWindowSizeAndPosition(napi_env env, napi_value objValue, const sptr<SceneSession>& session)
{
    auto abilityInfo = session->GetSessionInfo().abilityInfo;
    if (!abilityInfo) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "abilityInfo is nullptr");
        return;
    }
    uint32_t value = 0;
    for (const auto& item : abilityInfo->metadata) {
        if (item.name == "ohos.ability.window.width") {
            if (GetIntValueFromString(item.value, value) == WSError::WS_OK) {
                TLOGI(WmsLogTag::WMS_LAYOUT_PC, "ohos.ability.window.width=%{public}d", value);
                napi_set_named_property(env, objValue, "windowWidth", CreateJsValue(env, value));
            }
        } else if (item.name == "ohos.ability.window.height") {
            if (GetIntValueFromString(item.value, value) == WSError::WS_OK) {
                TLOGI(WmsLogTag::WMS_LAYOUT_PC, "ohos.ability.window.height=%{public}d", value);
                napi_set_named_property(env, objValue, "windowHeight", CreateJsValue(env, value));
            }
        } else if (item.name == "ohos.ability.window.left") {
            if (item.value.size() > 0) {
                TLOGI(WmsLogTag::WMS_LAYOUT_PC, "ohos.ability.window.left=%{public}s", item.value.c_str());
                napi_set_named_property(env, objValue, "windowLeft", CreateJsValue(env, item.value));
            }
        } else if (item.name == "ohos.ability.window.top") {
            if (item.value.size() > 0) {
                TLOGI(WmsLogTag::WMS_LAYOUT_PC, "ohos.ability.window.top=%{public}s", item.value.c_str());
                napi_set_named_property(env, objValue, "windowTop", CreateJsValue(env, item.value));
            }
        }
    }
}

napi_value JsSceneSession::Create(napi_env env, const sptr<SceneSession>& session)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || session == nullptr) {
        WLOGFE("Object or session is null!");
        return NapiGetUndefined(env);
    }

    sptr<JsSceneSession> jsSceneSession = sptr<JsSceneSession>::MakeSptr(env, session);
    jsSceneSession->IncStrongRef(nullptr);
    napi_wrap(env, objValue, jsSceneSession.GetRefPtr(), JsSceneSession::Finalizer, nullptr, nullptr);
    napi_set_named_property(env, objValue, "persistentId",
        CreateJsValue(env, static_cast<int32_t>(session->GetPersistentId())));
    napi_set_named_property(env, objValue, "parentId",
        CreateJsValue(env, static_cast<int32_t>(session->GetParentPersistentId())));
    napi_set_named_property(env, objValue, "type",
        CreateJsValue(env, static_cast<uint32_t>(GetApiType(session->GetWindowType()))));
    napi_set_named_property(env, objValue, "isAppType", CreateJsValue(env, session->IsFloatingWindowAppType()));
    napi_set_named_property(env, objValue, "pipTemplateInfo", CreatePipTemplateInfo(env, session));
    napi_set_named_property(env, objValue, "keyboardGravity",
        CreateJsValue(env, static_cast<int32_t>(session->GetKeyboardGravity())));
    napi_set_named_property(env, objValue, "isTopmost",
        CreateJsValue(env, static_cast<int32_t>(session->IsTopmost())));
    napi_set_named_property(env, objValue, "isMainWindowTopmost",
        CreateJsValue(env, static_cast<int32_t>(session->IsMainWindowTopmost())));
    napi_set_named_property(env, objValue, "subWindowModalType",
        CreateJsValue(env, static_cast<int32_t>(session->GetSubWindowModalType())));
    napi_set_named_property(env, objValue, "appInstanceKey",
        CreateJsValue(env, session->GetSessionInfo().appInstanceKey_));
    napi_set_named_property(env, objValue, "isSystemKeyboard",
        CreateJsValue(env, static_cast<int32_t>(session->IsSystemKeyboard())));
    SetWindowSizeAndPosition(env, objValue, session);
    sptr<WindowSessionProperty> sessionProperty = session->GetSessionProperty();
    if (sessionProperty != nullptr) {
        napi_set_named_property(env, objValue, "screenId",
            CreateJsValue(env, static_cast<int32_t>(sessionProperty->GetDisplayId())));
    } else {
        napi_set_named_property(env, objValue, "screenId",
            CreateJsValue(env, static_cast<int32_t>(SCREEN_ID_INVALID)));
        TLOGE(WmsLogTag::WMS_LIFE, "sessionProperty is nullptr!");
    }
    const char* moduleName = "JsSceneSession";
    BindNativeMethod(env, objValue, moduleName);
    BindNativeMethodForKeyboard(env, objValue, moduleName);
    BindNativeMethodForCompatiblePcMode(env, objValue, moduleName);
    BindNativeMethodForSCBSystemSession(env, objValue, moduleName);
    BindNativeMethodForFocus(env, objValue, moduleName);
    BindNativeMethodForWaterfall(env, objValue, moduleName);
    napi_ref jsRef = nullptr;
    napi_status status = napi_create_reference(env, objValue, 1, &jsRef);
    if (status != napi_ok) {
        WLOGFE("get ref failed");
    }
    jsSceneSessionMap_[session->GetPersistentId()] = jsRef;
    BindNativeFunction(env, objValue, "updateSizeChangeReason", moduleName, JsSceneSession::UpdateSizeChangeReason);
    return objValue;
}

void JsSceneSession::BindNativeMethod(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "activateDragBySystem", moduleName, JsSceneSession::ActivateDragBySystem);
    BindNativeFunction(env, objValue, "on", moduleName, JsSceneSession::RegisterCallback);
    BindNativeFunction(env, objValue, "updateNativeVisibility", moduleName, JsSceneSession::UpdateNativeVisibility);
    BindNativeFunction(env, objValue, "setShowRecent", moduleName, JsSceneSession::SetShowRecent);
    BindNativeFunction(env, objValue, "setZOrder", moduleName, JsSceneSession::SetZOrder);
    BindNativeFunction(env, objValue, "setTouchable", moduleName, JsSceneSession::SetTouchable);
    BindNativeFunction(env, objValue, "setSystemActive", moduleName, JsSceneSession::SetSystemActive);
    BindNativeFunction(env, objValue, "setPrivacyMode", moduleName, JsSceneSession::SetPrivacyMode);
    BindNativeFunction(env, objValue, "setSystemSceneOcclusionAlpha",
        moduleName, JsSceneSession::SetSystemSceneOcclusionAlpha);
    BindNativeFunction(env, objValue, "resetOcclusionAlpha", moduleName, JsSceneSession::ResetOcclusionAlpha);
    BindNativeFunction(env, objValue, "setSystemSceneForceUIFirst",
        moduleName, JsSceneSession::SetSystemSceneForceUIFirst);
    BindNativeFunction(env, objValue, "markSystemSceneUIFirst",
        moduleName, JsSceneSession::MarkSystemSceneUIFirst);
    BindNativeFunction(env, objValue, "setFloatingScale", moduleName, JsSceneSession::SetFloatingScale);
    BindNativeFunction(env, objValue, "setIsMidScene", moduleName, JsSceneSession::SetIsMidScene);
    BindNativeFunction(env, objValue, "setScale", moduleName, JsSceneSession::SetScale);
    BindNativeFunction(env, objValue, "setWindowLastSafeRect", moduleName, JsSceneSession::SetWindowLastSafeRect);
    BindNativeFunction(env, objValue, "setMovable", moduleName, JsSceneSession::SetMovable);
    BindNativeFunction(env, objValue, "setSplitButtonVisible", moduleName, JsSceneSession::SetSplitButtonVisible);
    BindNativeFunction(env, objValue, "setOffset", moduleName, JsSceneSession::SetOffset);
    BindNativeFunction(env, objValue, "setExitSplitOnBackground", moduleName,
        JsSceneSession::SetExitSplitOnBackground);
    BindNativeFunction(env, objValue, "setWaterMarkFlag", moduleName, JsSceneSession::SetWaterMarkFlag);
    BindNativeFunction(env, objValue, "setPipActionEvent", moduleName, JsSceneSession::SetPipActionEvent);
    BindNativeFunction(env, objValue, "setPiPControlEvent", moduleName, JsSceneSession::SetPiPControlEvent);
    BindNativeFunction(env, objValue, "notifyPipOcclusionChange", moduleName, JsSceneSession::NotifyPipOcclusionChange);
    BindNativeFunction(env, objValue, "notifyDisplayStatusBarTemporarily", moduleName,
        JsSceneSession::NotifyDisplayStatusBarTemporarily);
    BindNativeFunction(env, objValue, "setTemporarilyShowWhenLocked", moduleName,
        JsSceneSession::SetTemporarilyShowWhenLocked);
    BindNativeFunction(env, objValue, "setSkipDraw", moduleName, JsSceneSession::SetSkipDraw);
    BindNativeFunction(env, objValue, "setSkipSelfWhenShowOnVirtualScreen", moduleName,
        JsSceneSession::SetSkipSelfWhenShowOnVirtualScreen);
    BindNativeFunction(env, objValue, "setCompatibleModeInPc", moduleName,
        JsSceneSession::SetCompatibleModeInPc);
    BindNativeFunction(env, objValue, "setUniqueDensityDpiFromSCB", moduleName,
        JsSceneSession::SetUniqueDensityDpiFromSCB);
    BindNativeFunction(env, objValue, "setBlank", moduleName, JsSceneSession::SetBlank);
    BindNativeFunction(env, objValue, "removeBlank", moduleName, JsSceneSession::RemoveBlank);
    BindNativeFunction(env, objValue, "addSnapshot", moduleName, JsSceneSession::AddSnapshot);
    BindNativeFunction(env, objValue, "removeSnapshot", moduleName, JsSceneSession::RemoveSnapshot);
    BindNativeFunction(env, objValue, "setBufferAvailableCallbackEnable", moduleName,
        JsSceneSession::SetBufferAvailableCallbackEnable);
    BindNativeFunction(env, objValue, "syncDefaultRequestedOrientation", moduleName,
        JsSceneSession::SyncDefaultRequestedOrientation);
    BindNativeFunction(env, objValue, "setIsPcAppInPad", moduleName,
        JsSceneSession::SetIsPcAppInPad);
    BindNativeFunction(env, objValue, "setCompatibleModeEnableInPad", moduleName,
        JsSceneSession::SetCompatibleModeEnableInPad);
    BindNativeFunction(env, objValue, "setStartingWindowExitAnimationFlag", moduleName,
        JsSceneSession::SetStartingWindowExitAnimationFlag);
    BindNativeFunction(env, objValue, "setWindowEnableDragBySystem", moduleName,
        JsSceneSession::SetWindowEnableDragBySystem);
    BindNativeFunction(env, objValue, "setNeedSyncSessionRect", moduleName,
        JsSceneSession::SetNeedSyncSessionRect);
    BindNativeFunction(env, objValue, "setIsPendingToBackgroundState", moduleName,
        JsSceneSession::SetIsPendingToBackgroundState);
    BindNativeFunction(env, objValue, "setIsActivatedAfterScreenLocked", moduleName,
        JsSceneSession::SetIsActivatedAfterScreenLocked);
    BindNativeFunction(env, objValue, "setFrameGravity", moduleName,
        JsSceneSession::SetFrameGravity);
    BindNativeFunction(env, objValue, "setUseStartingWindowAboveLocked", moduleName,
        JsSceneSession::SetUseStartingWindowAboveLocked);
    BindNativeFunction(env, objValue, "saveSnapshotSync", moduleName,
        JsSceneSession::SaveSnapshotSync);
    BindNativeFunction(env, objValue, "setBehindWindowFilterEnabled", moduleName,
        JsSceneSession::SetBehindWindowFilterEnabled);
    BindNativeFunction(env, objValue, "setFreezeImmediately", moduleName,
        JsSceneSession::SetFreezeImmediately);
    BindNativeFunction(env, objValue, "sendContainerModalEvent", moduleName, JsSceneSession::SendContainerModalEvent);
}

void JsSceneSession::BindNativeMethodForKeyboard(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "setSCBKeepKeyboard", moduleName, JsSceneSession::SetSCBKeepKeyboard);
    BindNativeFunction(env, objValue, "requestHideKeyboard", moduleName, JsSceneSession::RequestHideKeyboard);
    BindNativeFunction(env, objValue, "openKeyboardSyncTransaction", moduleName,
        JsSceneSession::OpenKeyboardSyncTransaction);
    BindNativeFunction(env, objValue, "closeKeyboardSyncTransaction", moduleName,
        JsSceneSession::CloseKeyboardSyncTransaction);
    BindNativeFunction(env, objValue, "notifyTargetScreenWidthAndHeight", moduleName,
        JsSceneSession::NotifyTargetScreenWidthAndHeight);
}

void JsSceneSession::BindNativeMethodForCompatiblePcMode(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "setAppSupportPhoneInPc", moduleName,
        JsSceneSession::SetAppSupportPhoneInPc);
    BindNativeFunction(env, objValue, "setCompatibleWindowSizeInPc", moduleName,
        JsSceneSession::SetCompatibleWindowSizeInPc);
    BindNativeFunction(env, objValue, "compatibleFullScreenRecover", moduleName,
        JsSceneSession::CompatibleFullScreenRecover);
    BindNativeFunction(env, objValue, "compatibleFullScreenMinimize", moduleName,
        JsSceneSession::CompatibleFullScreenMinimize);
    BindNativeFunction(env, objValue, "compatibleFullScreenClose", moduleName,
        JsSceneSession::CompatibleFullScreenClose);
}

void JsSceneSession::BindNativeMethodForSCBSystemSession(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "syncScenePanelGlobalPosition", moduleName,
        JsSceneSession::SyncScenePanelGlobalPosition);
    BindNativeFunction(env, objValue, "unSyncScenePanelGlobalPosition", moduleName,
        JsSceneSession::UnSyncScenePanelGlobalPosition);
}

void JsSceneSession::BindNativeMethodForFocus(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "setFocusable", moduleName, JsSceneSession::SetFocusable);
    BindNativeFunction(env, objValue, "setFocusableOnShow", moduleName, JsSceneSession::SetFocusableOnShow);
    BindNativeFunction(env, objValue, "setSystemFocusable", moduleName, JsSceneSession::SetSystemFocusable);
    BindNativeFunction(env, objValue, "setSystemSceneBlockingFocus", moduleName,
        JsSceneSession::SetSystemSceneBlockingFocus);
}

void JsSceneSession::BindNativeMethodForWaterfall(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "maskSupportEnterWaterfallMode", moduleName,
        JsSceneSession::MaskSupportEnterWaterfallMode);
    BindNativeFunction(env, objValue, "updateFullScreenWaterfallMode", moduleName,
        JsSceneSession::UpdateFullScreenWaterfallMode);
}

JsSceneSession::JsSceneSession(napi_env env, const sptr<SceneSession>& session)
    : env_(env), weakSession_(session), persistentId_(session->GetPersistentId()),
      taskScheduler_(std::make_shared<MainThreadScheduler>(env))
{
    session->RegisterClearCallbackMapCallback([weakThis = wptr(this)](bool needRemove) {
        if (!needRemove) {
            TLOGND(WmsLogTag::WMS_LIFE, "clearCallbackFunc needRemove is false");
            return;
        }
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "clearCallbackFunc jsSceneSession is null");
            return;
        }
        jsSceneSession->ClearCbMap();
    });

    TLOGI(WmsLogTag::WMS_LIFE, "created, id:%{public}d", persistentId_);
}

JsSceneSession::~JsSceneSession()
{
    TLOGI(WmsLogTag::WMS_LIFE, "destroyed, id:%{public}d", persistentId_);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGI(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->UnregisterSessionChangeListeners();
    SceneSessionManager::GetInstance().UnregisterSpecificSessionCreateListener(session->GetPersistentId());
}

void JsSceneSession::ProcessPendingSceneSessionActivationRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetPendingSessionActivationEventListener([weakThis = wptr(this), where](SessionInfo& info) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->PendingSessionActivation(info);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessWindowDragHotAreaRegister()
{
    WLOGFI("[NAPI]");
    NotifyWindowDragHotAreaFunc func = [weakThis = wptr(this)](
        DisplayId displayId, uint32_t type, SizeChangeReason reason) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGE(WmsLogTag::WMS_LIFE, "ProcessWindowDragHotAreaRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnWindowDragHotArea(displayId, type, reason);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetWindowDragHotAreaListener(func);
}

void JsSceneSession::OnWindowDragHotArea(DisplayId displayId, uint32_t type, SizeChangeReason reason)
{
    WLOGFI("[NAPI]");

    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    WSRect rect = session->GetSessionTargetRectByDisplayId(displayId);
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_,
                 displayId, type, reason, rect, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(WINDOW_DRAG_HOT_AREA_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsCallBack is nullptr");
            return;
        }
        napi_value jsHotAreaDisplayId = CreateJsValue(env, static_cast<int64_t>(displayId));
        if (jsHotAreaDisplayId == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsHotAreaDisplayId is nullptr");
            return;
        }
        napi_value jsHotAreaType = CreateJsValue(env, type);
        if (jsHotAreaType == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsHotAreaType is nullptr");
            return;
        }
        napi_value jsHotAreaReason = CreateJsValue(env, reason);
        if (jsHotAreaReason == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsHotAreaReason is nullptr");
            return;
        }
        napi_value jsHotAreaRect = CreateJsSessionRect(env, rect);
        if (jsHotAreaRect == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsHotAreaRect is nullptr");
            return;
        }
        napi_value argv[] = {jsHotAreaDisplayId, jsHotAreaType, jsHotAreaReason, jsHotAreaRect};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnWindowDragHotArea");
}

void JsSceneSession::ProcessSessionInfoLockedStateChangeRegister()
{
    NotifySessionInfoLockedStateChangeFunc func = [weakThis = wptr(this)](bool lockedState) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessSessionInfoLockedStateChangeRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSessionInfoLockedStateChange(lockedState);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionInfoLockedStateChangeListener(func);
    WLOGFD("success");
}

void JsSceneSession::ProcessLandscapeMultiWindowRegister()
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "session is nullptr");
        return;
    }
    session->RegisterSetLandscapeMultiWindowFunc([weakThis = wptr(this)](bool isLandscapeMultiWindow) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->SetLandscapeMultiWindow(isLandscapeMultiWindow);
    });
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "success");
}

void JsSceneSession::SetLandscapeMultiWindow(bool isLandscapeMultiWindow)
{
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "isLandscapeMultiWindow: %{public}u",
        isLandscapeMultiWindow);

    auto task = [weakThis = wptr(this), persistentId = persistentId_, isLandscapeMultiWindow, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "SetLandscapeMultiWindow jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(LANDSCAPE_MULTI_WINDOW_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionLandscapeMultiWindowObj = CreateJsValue(env, isLandscapeMultiWindow);
        napi_value argv[] = {jsSessionLandscapeMultiWindowObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task,
        "SetLandscapeMultiWindow, isLandscapeMultiWindow:" + std::to_string(isLandscapeMultiWindow));
}

void JsSceneSession::ProcessAdjustKeyboardLayoutRegister()
{
    NotifyKeyboardLayoutAdjustFunc func = [weakThis = wptr(this)](const KeyboardLayoutParams& params) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessAdjustKeyboardLayoutRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnAdjustKeyboardLayout(params);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr");
        return;
    }
    session->SetAdjustKeyboardLayoutCallback(func);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "success");
}

void JsSceneSession::ProcessLayoutFullScreenChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    auto layoutFullScreenChangeCallback = [weakThis = wptr(this), where](bool isLayoutFullScreen) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnLayoutFullScreenChange(isLayoutFullScreen);
    };
    session->RegisterLayoutFullScreenChangeCallback(layoutFullScreenChangeCallback);
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "success");
}

void JsSceneSession::OnLayoutFullScreenChange(bool isLayoutFullScreen)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isLayoutFullScreen, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession id: %{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(LAYOUT_FULL_SCREEN_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value paramsObj = CreateJsValue(env, isLayoutFullScreen);
        napi_value argv[] = {paramsObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, where);
}

void JsSceneSession::ProcessDefaultDensityEnabledRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterDefaultDensityEnabledCallback([weakThis = wptr(this)](bool isDefaultDensityEnabled) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessDefaultDensityEnabledRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnDefaultDensityEnabled(isDefaultDensityEnabled);
    });
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "success");
}

void JsSceneSession::OnDefaultDensityEnabled(bool isDefaultDensityEnabled)
{
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isDefaultDensityEnabled, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnDefaultDensityEnabled jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(DEFAULT_DENSITY_ENABLED_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "OnDefaultDensityEnabled jsCallBack is nullptr");
            return;
        }
        napi_value paramsObj = CreateJsValue(env, isDefaultDensityEnabled);
        napi_value argv[] = {paramsObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnDefaultDensityEnabled");
}

void JsSceneSession::ProcessTitleAndDockHoverShowChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const funcName = __func__;
    session->SetTitleAndDockHoverShowChangeCallback([weakThis = wptr(this), funcName](
        bool isTitleHoverShown, bool isDockHoverShown) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession is null", funcName);
            return;
        }
        jsSceneSession->OnTitleAndDockHoverShowChange(isTitleHoverShown, isDockHoverShown);
    });
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "Register success, persistent id %{public}d", persistentId_);
}

void JsSceneSession::OnTitleAndDockHoverShowChange(bool isTitleHoverShown, bool isDockHoverShown)
{
    const char* const funcName = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isTitleHoverShown, isDockHoverShown,
        env = env_, funcName] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                funcName, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(TITLE_DOCK_HOVER_SHOW_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsCallBack is nullptr", funcName);
            return;
        }
        napi_value jsObjTitle = CreateJsValue(env, isTitleHoverShown);
        napi_value jsObjDock = CreateJsValue(env, isDockHoverShown);
        napi_value argv[] = {jsObjTitle, jsObjDock};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, funcName);
}

void JsSceneSession::ProcessRestoreMainWindowRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const funcName = __func__;
    session->SetRestoreMainWindowCallback([weakThis = wptr(this), funcName] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession is null", funcName);
            return;
        }
        jsSceneSession->RestoreMainWindow();
    });
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "success");
}

void JsSceneSession::RestoreMainWindow()
{
    const char* const funcName = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_, funcName] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                funcName, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(RESTORE_MAIN_WINDOW_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsCallBack is nullptr", funcName);
            return;
        }
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, {}, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, funcName);
}

void JsSceneSession::OnAdjustKeyboardLayout(const KeyboardLayoutParams& params)
{
    auto task = [weakThis = wptr(this), persistentId = persistentId_, params, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnAdjustKeyboardLayout jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(ADJUST_KEYBOARD_LAYOUT_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "OnAdjustKeyboardLayout jsCallBack is nullptr");
            return;
        }
        napi_value keyboardLayoutParamsObj = CreateJsKeyboardLayoutParams(env, params);
        if (keyboardLayoutParamsObj == nullptr) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "OnAdjustKeyboardLayout this keyboard layout params obj is nullptr");
        }
        napi_value argv[] = {keyboardLayoutParamsObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnAdjustKeyboardLayout");
}

void JsSceneSession::OnSessionInfoLockedStateChange(bool lockedState)
{
    WLOGFI("state: %{public}u", lockedState);

    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, lockedState, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSIONINFO_LOCKEDSTATE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionInfoLockedStateObj = CreateJsValue(env, lockedState);
        napi_value argv[] = {jsSessionInfoLockedStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionInfoLockedStateChange: state " + std::to_string(lockedState));
}

void JsSceneSession::ClearCbMap()
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: jsSceneSession is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s: persistent id %{public}d", where, jsSceneSession->persistentId_);
        {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession clear jsCbMap");
            std::unique_lock<std::shared_mutex> lock(jsSceneSession->jsCbMapMutex_);
            jsSceneSession->jsCbMap_.clear();
        }
        // delete native reference
        if (auto iter = jsSceneSessionMap_.find(jsSceneSession->persistentId_); iter != jsSceneSessionMap_.end()) {
            napi_delete_reference(jsSceneSession->env_, iter->second);
            jsSceneSessionMap_.erase(iter);
        } else {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: delete ref failed, %{public}d",
                   where, jsSceneSession->persistentId_);
        }
    };
    taskScheduler_->PostMainThreadTask(task, "ClearCbMap PID:" + std::to_string(persistentId_));
}

void JsSceneSession::ProcessSessionDefaultAnimationFlagChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterDefaultAnimationFlagChangeCallback([weakThis = wptr(this)](bool isNeedDefaultAnimationFlag) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessSessionDefaultAnimationFlagChangeRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnDefaultAnimationFlagChange(isNeedDefaultAnimationFlag);
    });
}

void JsSceneSession::OnDefaultAnimationFlagChange(bool isNeedDefaultAnimationFlag)
{
    WLOGFI("flag: %{public}u", isNeedDefaultAnimationFlag);

    auto task = [weakThis = wptr(this), persistentId = persistentId_, isNeedDefaultAnimationFlag, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnDefaultAnimationFlagChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionDefaultAnimationFlagObj = CreateJsValue(env, isNeedDefaultAnimationFlag);
        napi_value argv[] = {jsSessionDefaultAnimationFlagObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string info = "OnDefaultAnimationFlagChange, flag:" + std::to_string(isNeedDefaultAnimationFlag);
    taskScheduler_->PostMainThreadTask(task, info);
}

void JsSceneSession::ProcessChangeSessionVisibilityWithStatusBarRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetChangeSessionVisibilityWithStatusBarEventListener(
        [weakThis = wptr(this), where = __func__](const SessionInfo& info, bool visible) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->ChangeSessionVisibilityWithStatusBar(info, visible);
    });
    WLOGFD("success");
}

void JsSceneSession::ProcessSessionStateChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionStateChangeListenser([weakThis = wptr(this), where = __func__](const SessionState& state) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSessionStateChange(state);
    });
    WLOGFD("success");
}

void JsSceneSession::ProcessBufferAvailableChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetBufferAvailableChangeListener([weakThis = wptr(this), where = __func__](const bool isAvailable) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnBufferAvailableChange(isAvailable);
    });
    WLOGFD("success");
}

void JsSceneSession::ProcessCreateSubSessionRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    SceneSessionManager::GetInstance().RegisterCreateSubSessionListener(session->GetPersistentId(),
        [weakThis = wptr(this)](const sptr<SceneSession>& sceneSession) {
            auto jsSceneSession = weakThis.promote();
            if (!jsSceneSession) {
                TLOGNE(WmsLogTag::WMS_LIFE, "ProcessCreateSubSessionRegister jsSceneSession is null");
                return;
            }
            jsSceneSession->OnCreateSubSession(sceneSession);
        });
    WLOGFD("success, id: %{public}d", session->GetPersistentId());
}

void JsSceneSession::ProcessBindDialogTargetRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    SceneSessionManager::GetInstance().RegisterBindDialogTargetListener(session,
        [weakThis = wptr(this)](const sptr<SceneSession>& sceneSession) {
            auto jsSceneSession = weakThis.promote();
            if (!jsSceneSession) {
                TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
                return;
            }
            jsSceneSession->OnBindDialogTarget(sceneSession);
        });
    TLOGD(WmsLogTag::WMS_DIALOG, "success");
}

void JsSceneSession::ProcessSessionRectChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionRectChangeCallback([weakThis = wptr(this)](const WSRect& rect,
        SizeChangeReason reason, DisplayId displayId = DISPLAY_ID_INVALID,
        const RectAnimationConfig& rectAnimationConfig = {}) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessSessionRectChangeRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSessionRectChange(rect, reason, displayId, rectAnimationConfig);
    });
    WLOGFD("success");
}

void JsSceneSession::ProcessSessionDisplayIdChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionDisplayIdChangeCallback([weakThis = wptr(this), where = __func__](uint64_t displayId) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSessionDisplayIdChange(displayId);
    });
}

void JsSceneSession::ProcessWindowMovingRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetWindowMovingCallback([weakThis = wptr(this)](DisplayId displayId, int32_t pointerX, int32_t pointerY) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_DECOR, "ProcessWindowMovingRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnWindowMoving(displayId, pointerX, pointerY);
    });
    TLOGI(WmsLogTag::WMS_DECOR, "success");
}

void JsSceneSession::ProcessSessionPiPControlStatusChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionPiPControlStatusChangeCallback([weakThis = wptr(this), where = __func__](
        WsPiPControlType controlType, WsPiPControlStatus status) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSessionPiPControlStatusChange(controlType, status);
    });
    TLOGI(WmsLogTag::WMS_PIP, "success");
}

void JsSceneSession::ProcessAutoStartPiPStatusChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetAutoStartPiPStatusChangeCallback([weakThis = wptr(this)](bool isAutoStart, uint32_t priority) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_PIP, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnAutoStartPiPStatusChange(isAutoStart, priority);
    });
    TLOGI(WmsLogTag::WMS_PIP, "success");
}

/** @note @window.hierarchy */
void JsSceneSession::ProcessRaiseToTopRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterRaiseToTopCallback([weakThis = wptr(this)] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnRaiseToTop();
    });
    TLOGD(WmsLogTag::WMS_HIERARCHY, "success");
}

/** @note @window.hierarchy */
void JsSceneSession::ProcessRaiseToTopForPointDownRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetRaiseToAppTopForPointDownFunc([weakThis = wptr(this)]() {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessRaiseToTopForPointDownRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnRaiseToTopForPointDown();
    });
    WLOGFD("success");
}

void JsSceneSession::ProcessClickModalWindowOutsideRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetClickModalWindowOutsideListener([where, weakThis = wptr(this)] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnClickModalWindowOutside();
    });
    TLOGD(WmsLogTag::WMS_LAYOUT, "success");
}

/** @note @window.hierarchy */
void JsSceneSession::ProcessRaiseAboveTargetRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterRaiseAboveTargetCallback([weakThis = wptr(this)](int32_t subWindowId) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnRaiseAboveTarget(subWindowId);
    });
    TLOGD(WmsLogTag::WMS_HIERARCHY, "success");
}

void JsSceneSession::ProcessSessionEventRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is null, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterSessionEventCallback([weakThis = wptr(this)](uint32_t eventId, const SessionEventParam& param) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "ProcessSessionEventRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSessionEvent(eventId, param);
    });
    TLOGD(WmsLogTag::WMS_LAYOUT, "success");
}

void JsSceneSession::ProcessTerminateSessionRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetTerminateSessionListener([weakThis = wptr(this), where](const SessionInfo& info) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->TerminateSession(info);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessTerminateSessionRegisterNew()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetTerminateSessionListenerNew([weakThis = wptr(this), where](
        const SessionInfo& info, bool needStartCaller, bool isFromBroker) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->TerminateSessionNew(info, needStartCaller, isFromBroker);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessTerminateSessionRegisterTotal()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetTerminateSessionListenerTotal([weakThis = wptr(this), where](
        const SessionInfo& info, TerminateType terminateType) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->TerminateSessionTotal(info, terminateType);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessPendingSessionToForegroundRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetPendingSessionToForegroundListener([weakThis = wptr(this), where](const SessionInfo& info) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->PendingSessionToForeground(info);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessPendingSessionToBackgroundForDelegatorRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetPendingSessionToBackgroundForDelegatorListener([weakThis = wptr(this), where](
        const SessionInfo& info, bool shouldBackToCaller) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->PendingSessionToBackgroundForDelegator(info, shouldBackToCaller);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessSessionExceptionRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetSessionExceptionListener([weakThis = wptr(this), where](const SessionInfo& info,
        bool needRemoveSession, bool startFail) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSessionException(info, needRemoveSession, startFail);
    }, true);
}

/** @note @window.hierarchy */
void JsSceneSession::ProcessSessionTopmostChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterSessionTopmostChangeCallback([weakThis = wptr(this)](bool topmost) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSessionTopmostChange(topmost);
    });
    TLOGD(WmsLogTag::WMS_HIERARCHY, "success");
}

/** @note @window.hierarchy */
void JsSceneSession::ProcessMainWindowTopmostChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetMainWindowTopmostChangeCallback([weakThis = wptr(this), where](bool isTopmost) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnMainWindowTopmostChange(isTopmost);
    });
    TLOGD(WmsLogTag::WMS_HIERARCHY, "register success");
}

void JsSceneSession::ProcessSubModalTypeChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "session is nullptr, persistentId:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterSubModalTypeChangeCallback([weakThis = wptr(this), where](SubWindowModalType subWindowModalType) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSubModalTypeChange(subWindowModalType);
    });
    TLOGD(WmsLogTag::WMS_HIERARCHY, "register success, persistentId:%{public}d", persistentId_);
}

void JsSceneSession::ProcessMainModalTypeChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, persistentId:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterMainModalTypeChangeCallback([weakThis = wptr(this), where](bool isModal) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnMainModalTypeChange(isModal);
    });
    TLOGD(WmsLogTag::WMS_HIERARCHY, "register success, persistentId:%{public}d", persistentId_);
}

void JsSceneSession::RegisterThrowSlipAnimationStateChangeCallback()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, persistent id: %{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterThrowSlipAnimationStateChangeCallback([weakThis = wptr(this), where](bool isAnimating) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnThrowSlipAnimationStateChange(isAnimating);
    });
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "register success, persistent id: %{public}d", persistentId_);
}

void JsSceneSession::RegisterFullScreenWaterfallModeChangeCallback()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, persistent id: %{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterFullScreenWaterfallModeChangeCallback([weakThis = wptr(this), where](bool isWaterfallMode) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnFullScreenWaterfallModeChange(isWaterfallMode);
    });
    TLOGD(WmsLogTag::WMS_LAYOUT, "register success, persistent id: %{public}d", persistentId_);
}

void JsSceneSession::ProcessSessionFocusableChangeRegister()
{
    NotifySessionFocusableChangeFunc func = [weakThis = wptr(this)](bool isFocusable) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessSessionFocusableChangeRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSessionFocusableChange(isFocusable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[WMSComm]session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionFocusableChangeListener(func);
    TLOGD(WmsLogTag::WMS_FOCUS, "success");
}

void JsSceneSession::ProcessSessionTouchableChangeRegister()
{
    NotifySessionTouchableChangeFunc func = [weakThis = wptr(this)](bool touchable) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessSessionTouchableChangeRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSessionTouchableChange(touchable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetSessionTouchableChangeListener(func);
    WLOGFD("success");
}

void JsSceneSession::ProcessClickRegister()
{
    NotifyClickFunc func = [weakThis = wptr(this)](bool requestFocus, bool isClick) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessClickRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnClick(requestFocus, isClick);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetClickListener(func);
    WLOGFD("success");
}

void JsSceneSession::ProcessContextTransparentRegister()
{
    NotifyContextTransparentFunc func = [weakThis = wptr(this)]() {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessContextTransparentRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnContextTransparent();
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetContextTransparentFunc(func);
    WLOGFD("success");
}

void JsSceneSession::OnSessionEvent(uint32_t eventId, const SessionEventParam& param)
{
    WLOGFI("eventId: %{public}d", eventId);

    auto task = [weakThis = wptr(this), persistentId = persistentId_, eventId, param, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSessionEvent jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_EVENT_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, eventId);
        napi_value jsSessionParamObj = CreateJsSessionEventParam(env, param);
        napi_value argv[] = {jsSessionStateObj, jsSessionParamObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    taskScheduler_->PostMainThreadTask(task, "OnSessionEvent, EventId:" + std::to_string(eventId));
}

void JsSceneSession::ProcessBackPressedRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetBackPressedListenser([weakThis = wptr(this), where](bool needMoveToBackground) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnBackPressed(needMoveToBackground);
    });
}

void JsSceneSession::ProcessSystemBarPropertyChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "session is null, win %{public}d", persistentId_);
        return;
    }
    session->RegisterSystemBarPropertyChangeCallback([weakThis = wptr(this)](
        const std::unordered_map<WindowType, SystemBarProperty>& propertyMap) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnSystemBarPropertyChange(propertyMap);
    });
}

void JsSceneSession::ProcessNeedAvoidRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "session is null, win %{public}d", persistentId_);
        return;
    }
    session->RegisterNeedAvoidCallback([weakThis = wptr(this)](bool status) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_IMMS, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnNeedAvoid(status);
    });
}

void JsSceneSession::ProcessIsCustomAnimationPlaying()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->RegisterIsCustomAnimationPlayingCallback([weakThis = wptr(this)](bool status) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnIsCustomAnimationPlaying(status);
    });
}

void JsSceneSession::ProcessShowWhenLockedRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterShowWhenLockedCallback([weakThis = wptr(this), where](bool showWhenLocked) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnShowWhenLocked(showWhenLocked);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessRequestedOrientationChange()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->RegisterRequestedOrientationChangeCallback([weakThis = wptr(this)](uint32_t orientation) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnReuqestedOrientationChange(orientation);
    });
}

void JsSceneSession::ProcessForceHideChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterForceHideChangeCallback([weakThis = wptr(this), where](bool hide) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnForceHideChange(hide);
    });
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::OnForceHideChange(bool hide)
{
    WLOGFI("hide: %{public}u", hide);

    auto task = [weakThis = wptr(this), persistentId = persistentId_, hide, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnForceHideChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(FORCE_HIDE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionForceHideObj = CreateJsValue(env, hide);
        napi_value argv[] = {jsSessionForceHideObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnForceHideChange, hide:" + std::to_string(hide));
}

void JsSceneSession::ProcessTouchOutsideRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->RegisterTouchOutsideCallback([weakThis = wptr(this)] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession is null");
            return;
        }
        jsSceneSession->OnTouchOutside();
    });
}

void JsSceneSession::OnTouchOutside()
{
    WLOGFI("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnTouchOutside jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(TOUCH_OUTSIDE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task);
}

void JsSceneSession::ProcessFrameLayoutFinishRegister()
{
    NotifyFrameLayoutFinishFunc func = [weakThis = wptr(this)]() {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessFrameLayoutFinishRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->NotifyFrameLayoutFinish();
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "session is nullptr");
        return;
    }
    session->SetFrameLayoutFinishListener(func);
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "success");
}

void JsSceneSession::NotifyFrameLayoutFinish()
{
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "session is nullptr");
        return;
    }
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_MULTI_WINDOW, "jsSceneSession id:%{public}d has been destroyed", persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(NEXT_FRAME_LAYOUT_FINISH_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_MULTI_WINDOW, "jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "NotifyFrameLayoutFinish");
}

void JsSceneSession::ProcessPrivacyModeChangeRegister()
{
    NotifyPrivacyModeChangeFunc func = [this](bool isPrivacyMode) {
        this->NotifyPrivacyModeChange(isPrivacyMode);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr");
        return;
    }
    session->SetPrivacyModeChangeNotifyFunc(func);
}

void JsSceneSession::NotifyPrivacyModeChange(bool isPrivacyMode)
{
    TLOGI(WmsLogTag::WMS_SCB, "isPrivacyMode:%{public}d, id:%{public}d", isPrivacyMode, persistentId_);
    auto task = [this, isPrivacyMode, env = env_]() {
        auto jsCallback = this->GetJSCallback(PRIVACY_MODE_CHANGE_CB);
        if (!jsCallback) {
            TLOGNE(WmsLogTag::WMS_SCB, "jsCallback is nullptr");
            return;
        }
        napi_value jsSessionPrivacyMode = CreateJsValue(env, isPrivacyMode);
        napi_value argv[] = { jsSessionPrivacyMode };
        napi_call_function(env, NapiGetUndefined(env), jsCallback->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "NotifyPrivacyModeChange");
}

void JsSceneSession::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGFI("[NAPI]");
    auto jsSceneSession = static_cast<JsSceneSession*>(data);
    if (jsSceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "JsSceneSession is nullptr");
        return;
    }
    jsSceneSession->DecStrongRef(nullptr);
}

napi_value JsSceneSession::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsSceneSession::UpdateNativeVisibility(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUpdateNativeVisibility(env, info) : nullptr;
}

napi_value JsSceneSession::SetPrivacyMode(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetPrivacyMode(env, info) : nullptr;
}

napi_value JsSceneSession::SetSkipSelfWhenShowOnVirtualScreen(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSkipSelfWhenShowOnVirtualScreen(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneOcclusionAlpha(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneOcclusionAlpha(env, info) : nullptr;
}

napi_value JsSceneSession::ResetOcclusionAlpha(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnResetOcclusionAlpha(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneForceUIFirst(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneForceUIFirst(env, info) : nullptr;
}

napi_value JsSceneSession::MarkSystemSceneUIFirst(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnMarkSystemSceneUIFirst(env, info) : nullptr;
}

napi_value JsSceneSession::SetFocusable(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFocusable(env, info) : nullptr;
}

napi_value JsSceneSession::SetFocusableOnShow(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFocusableOnShow(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemFocusable(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemFocusable(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneBlockingFocus(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneBlockingFocus(env, info) : nullptr;
}

napi_value JsSceneSession::MaskSupportEnterWaterfallMode(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnMaskSupportEnterWaterfallMode(env, info) : nullptr;
}

napi_value JsSceneSession::UpdateFullScreenWaterfallMode(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "in");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUpdateFullScreenWaterfallMode(env, info) : nullptr;
}

napi_value JsSceneSession::UpdateSizeChangeReason(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUpdateSizeChangeReason(env, info) : nullptr;
}

napi_value JsSceneSession::OpenKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnOpenKeyboardSyncTransaction(env, info) : nullptr;
}

napi_value JsSceneSession::CloseKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnCloseKeyboardSyncTransaction(env, info) : nullptr;
}

napi_value JsSceneSession::NotifyTargetScreenWidthAndHeight(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnNotifyTargetScreenWidthAndHeight(env, info) : nullptr;
}

napi_value JsSceneSession::SetShowRecent(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetShowRecent(env, info) : nullptr;
}

/** @note @window.hierarchy */
napi_value JsSceneSession::SetZOrder(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
    WLOGFD("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetZOrder(env, info) : nullptr;
}

napi_value JsSceneSession::SetTouchable(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetTouchable(env, info): nullptr;
}

napi_value JsSceneSession::SetSystemActive(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemActive(env, info): nullptr;
}

napi_value JsSceneSession::SetFloatingScale(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFloatingScale(env, info) : nullptr;
}

napi_value JsSceneSession::SetIsMidScene(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetIsMidScene(env, info) : nullptr;
}

napi_value JsSceneSession::SetSCBKeepKeyboard(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSCBKeepKeyboard(env, info) : nullptr;
}

napi_value JsSceneSession::SetOffset(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
    WLOGFI("[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetOffset(env, info) : nullptr;
}

napi_value JsSceneSession::SetExitSplitOnBackground(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetExitSplitOnBackground(env, info) : nullptr;
}

napi_value JsSceneSession::SetWaterMarkFlag(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetWaterMarkFlag(env, info) : nullptr;
}

napi_value JsSceneSession::SetPipActionEvent(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetPipActionEvent(env, info) : nullptr;
}

napi_value JsSceneSession::SetPiPControlEvent(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetPiPControlEvent(env, info) : nullptr;
}

napi_value JsSceneSession::NotifyPipOcclusionChange(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PIP, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnNotifyPipOcclusionChange(env, info) : nullptr;
}

napi_value JsSceneSession::NotifyDisplayStatusBarTemporarily(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnNotifyDisplayStatusBarTemporarily(env, info) : nullptr;
}

napi_value JsSceneSession::SetTemporarilyShowWhenLocked(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetTemporarilyShowWhenLocked(env, info) : nullptr;
}

napi_value JsSceneSession::SetSkipDraw(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSkipDraw(env, info) : nullptr;
}

napi_value JsSceneSession::SetCompatibleModeInPc(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetCompatibleModeInPc(env, info) : nullptr;
}

napi_value JsSceneSession::SetCompatibleWindowSizeInPc(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetCompatibleWindowSizeInPc(env, info) : nullptr;
}

napi_value JsSceneSession::SetAppSupportPhoneInPc(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetAppSupportPhoneInPc(env, info) : nullptr;
}

napi_value JsSceneSession::SetUniqueDensityDpiFromSCB(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetUniqueDensityDpiFromSCB(env, info) : nullptr;
}

napi_value JsSceneSession::SetBlank(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetBlank(env, info) : nullptr;
}

napi_value JsSceneSession::RemoveBlank(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnRemoveBlank(env, info) : nullptr;
}

/*
 * AddSnapshot and RemoveSnapshot must be in pair
 * be in good control of the time RemoveSnapshot execute, do not rely on first frame callback
 */
napi_value JsSceneSession::AddSnapshot(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PATTERN, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnAddSnapshot(env, info) : nullptr;
}

napi_value JsSceneSession::RemoveSnapshot(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PATTERN, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnRemoveSnapshot(env, info) : nullptr;
}

napi_value JsSceneSession::SetBufferAvailableCallbackEnable(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetBufferAvailableCallbackEnable(env, info) : nullptr;
}

napi_value JsSceneSession::SyncDefaultRequestedOrientation(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSyncDefaultRequestedOrientation(env, info) : nullptr;
}

napi_value JsSceneSession::SetIsPcAppInPad(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetIsPcAppInPad(env, info) : nullptr;
}

napi_value JsSceneSession::SetCompatibleModeEnableInPad(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetCompatibleModeEnableInPad(env, info) : nullptr;
}

napi_value JsSceneSession::SetStartingWindowExitAnimationFlag(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetStartingWindowExitAnimationFlag(env, info) : nullptr;
}

napi_value JsSceneSession::CompatibleFullScreenRecover(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnCompatibleFullScreenRecover(env, info) : nullptr;
}

napi_value JsSceneSession::CompatibleFullScreenMinimize(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnCompatibleFullScreenMinimize(env, info) : nullptr;
}

napi_value JsSceneSession::CompatibleFullScreenClose(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnCompatibleFullScreenClose(env, info) : nullptr;
}

napi_value JsSceneSession::SyncScenePanelGlobalPosition(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSyncScenePanelGlobalPosition(env, info) : nullptr;
}

napi_value JsSceneSession::UnSyncScenePanelGlobalPosition(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUnSyncScenePanelGlobalPosition(env, info) : nullptr;
}

napi_value JsSceneSession::SetWindowEnableDragBySystem(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetWindowEnableDragBySystem(env, info) : nullptr;
}

napi_value JsSceneSession::ActivateDragBySystem(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnActivateDragBySystem(env, info) : nullptr;
}

napi_value JsSceneSession::SetNeedSyncSessionRect(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PIPELINE, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetNeedSyncSessionRect(env, info) : nullptr;
}

napi_value JsSceneSession::SetIsPendingToBackgroundState(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetIsPendingToBackgroundState(env, info) : nullptr;
}

napi_value JsSceneSession::SetIsActivatedAfterScreenLocked(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetIsActivatedAfterScreenLocked(env, info) : nullptr;
}

napi_value JsSceneSession::SetFrameGravity(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFrameGravity(env, info) : nullptr;
}

napi_value JsSceneSession::SetUseStartingWindowAboveLocked(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetUseStartingWindowAboveLocked(env, info) : nullptr;
}

napi_value JsSceneSession::SaveSnapshotSync(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSaveSnapshotSync(env, info) : nullptr;
}

napi_value JsSceneSession::SetFreezeImmediately(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "in");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFreezeImmediately(env, info) : nullptr;
}

napi_value JsSceneSession::SetBehindWindowFilterEnabled(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetBehindWindowFilterEnabled(env, info) : nullptr;
}

napi_value JsSceneSession::SendContainerModalEvent(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_EVENT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSendContainerModalEvent(env, info) : nullptr;
}

bool JsSceneSession::IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession::IsCallbackRegistered[%s]", type.c_str());
    std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        return false;
    }

    for (auto iter = jsCbMap_.begin(); iter != jsCbMap_.end(); ++iter) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter->second->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("%{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

napi_value JsSceneSession::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiGetUndefined(env);
    }
    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        WLOGFE("Invalid argument");
        return NapiGetUndefined(env);
    }
    auto iterFunctionType = ListenerFuncMap.find(cbType);
    if (iterFunctionType == ListenerFuncMap.end()) {
        WLOGFE("callback type is not supported, type=%{public}s", cbType.c_str());
        return NapiGetUndefined(env);
    }
    ListenerFuncType listenerFuncType = iterFunctionType->second;
    if (IsCallbackRegistered(env, cbType, value)) {
        WLOGFE("callback is registered, type=%{public}s", cbType.c_str());
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }

    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession set jsCbMap[%s]", cbType.c_str());
        std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
        jsCbMap_[cbType] = callbackRef;
    }
    ProcessRegisterCallback(listenerFuncType);
    WLOGFD("end, type=%{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessRegisterCallback(ListenerFuncType listenerFuncType)
{
    switch (static_cast<uint32_t>(listenerFuncType)) {
        case static_cast<uint32_t>(ListenerFuncType::PENDING_SCENE_CB):
            ProcessPendingSceneSessionActivationRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RESTORE_MAIN_WINDOW_CB):
            ProcessRestoreMainWindowRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR):
            ProcessChangeSessionVisibilityWithStatusBarRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_STATE_CHANGE_CB):
            ProcessSessionStateChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::BUFFER_AVAILABLE_CHANGE_CB):
            ProcessBufferAvailableChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_EVENT_CB):
            ProcessSessionEventRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_RECT_CHANGE_CB):
            ProcessSessionRectChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::WINDOW_MOVING_CB):
            ProcessWindowMovingRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_DISPLAY_ID_CHANGE_CB):
            ProcessSessionDisplayIdChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_PIP_CONTROL_STATUS_CHANGE_CB):
            ProcessSessionPiPControlStatusChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_AUTO_START_PIP_CB):
            ProcessAutoStartPiPStatusChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CREATE_SUB_SESSION_CB):
            ProcessCreateSubSessionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::BIND_DIALOG_TARGET_CB):
            ProcessBindDialogTargetRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RAISE_TO_TOP_CB):
            ProcessRaiseToTopRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RAISE_TO_TOP_POINT_DOWN_CB):
            ProcessRaiseToTopForPointDownRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CLICK_MODAL_WINDOW_OUTSIDE_CB):
            ProcessClickModalWindowOutsideRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::BACK_PRESSED_CB):
            ProcessBackPressedRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_FOCUSABLE_CHANGE_CB):
            ProcessSessionFocusableChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_TOUCHABLE_CHANGE_CB):
            ProcessSessionTouchableChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_TOP_MOST_CHANGE_CB):
            ProcessSessionTopmostChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::MAIN_WINDOW_TOP_MOST_CHANGE_CB):
            ProcessMainWindowTopmostChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SUB_MODAL_TYPE_CHANGE_CB):
            ProcessSubModalTypeChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::MAIN_MODAL_TYPE_CHANGE_CB):
            ProcessMainModalTypeChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::THROW_SLIP_ANIMATION_STATE_CHANGE_CB):
            RegisterThrowSlipAnimationStateChangeCallback();
            break;
        case static_cast<uint32_t>(ListenerFuncType::FULLSCREEN_WATERFALL_MODE_CHANGE_CB):
            RegisterFullScreenWaterfallModeChangeCallback();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CLICK_CB):
            ProcessClickRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TERMINATE_SESSION_CB):
            ProcessTerminateSessionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TERMINATE_SESSION_CB_NEW):
            ProcessTerminateSessionRegisterNew();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TERMINATE_SESSION_CB_TOTAL):
            ProcessTerminateSessionRegisterTotal();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_EXCEPTION_CB):
            ProcessSessionExceptionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::UPDATE_SESSION_LABEL_CB):
            ProcessUpdateSessionLabelRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::UPDATE_SESSION_ICON_CB):
            ProcessUpdateSessionIconRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SYSTEMBAR_PROPERTY_CHANGE_CB):
            ProcessSystemBarPropertyChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::NEED_AVOID_CB):
            ProcessNeedAvoidRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PENDING_SESSION_TO_FOREGROUND_CB):
            ProcessPendingSessionToForegroundRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB):
            ProcessPendingSessionToBackgroundForDelegatorRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CUSTOM_ANIMATION_PLAYING_CB):
            ProcessIsCustomAnimationPlaying();
            break;
        case static_cast<uint32_t>(ListenerFuncType::NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB):
            ProcessSessionDefaultAnimationFlagChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SHOW_WHEN_LOCKED_CB):
            ProcessShowWhenLockedRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::REQUESTED_ORIENTATION_CHANGE_CB):
            ProcessRequestedOrientationChange();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RAISE_ABOVE_TARGET_CB):
            ProcessRaiseAboveTargetRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::FORCE_HIDE_CHANGE_CB):
            ProcessForceHideChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::WINDOW_DRAG_HOT_AREA_CB):
            ProcessWindowDragHotAreaRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TOUCH_OUTSIDE_CB):
            ProcessTouchOutsideRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSIONINFO_LOCKEDSTATE_CHANGE_CB):
            ProcessSessionInfoLockedStateChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PREPARE_CLOSE_PIP_SESSION):
            ProcessPrepareClosePiPSessionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::LANDSCAPE_MULTI_WINDOW_CB):
            ProcessLandscapeMultiWindowRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CONTEXT_TRANSPARENT_CB):
            ProcessContextTransparentRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::ADJUST_KEYBOARD_LAYOUT_CB):
            ProcessAdjustKeyboardLayoutRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::LAYOUT_FULL_SCREEN_CB):
            ProcessLayoutFullScreenChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::DEFAULT_DENSITY_ENABLED_CB):
            ProcessDefaultDensityEnabledRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TITLE_DOCK_HOVER_SHOW_CB):
            ProcessTitleAndDockHoverShowChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::NEXT_FRAME_LAYOUT_FINISH_CB):
            ProcessFrameLayoutFinishRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PRIVACY_MODE_CHANGE_CB):
            ProcessPrivacyModeChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SET_WINDOW_RECT_AUTO_SAVE_CB):
            ProcessSetWindowRectAutoSaveRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::UPDATE_APP_USE_CONTROL_CB):
            RegisterUpdateAppUseControlCallback();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SET_SUPPORT_WINDOW_MODES_CB):
            ProcessSetSupportedWindowModesRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_LOCK_STATE_CHANGE_CB):
            ProcessSessionLockStateChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::UPDATE_SESSION_LABEL_AND_ICON_CB):
            ProcessUpdateSessionLabelAndIconRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::KEYBOARD_STATE_CHANGE_CB):
            ProcessKeyboardStateChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::KEYBOARD_VIEW_MODE_CHANGE_CB):
            ProcessKeyboardViewModeChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SET_WINDOW_CORNER_RADIUS_CB):
            ProcessSetWindowCornerRadiusRegister();
            break;
        default:
            break;
    }
}

napi_value JsSceneSession::OnUpdateNativeVisibility(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiGetUndefined(env);
    }
    bool visible = false;
    if (!ConvertFromJsValue(env, argv[0], visible)) {
        WLOGFE("Failed to convert parameter to bool");
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->UpdateNativeVisibility(visible);
    WLOGFI("end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetPrivacyMode(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isPrivacy = false;
    if (!ConvertFromJsValue(env, argv[0], isPrivacy)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetPrivacyMode(isPrivacy);
    SceneSessionManager::GetInstance().UpdatePrivateStateAndNotify(session->GetPersistentId());
    WLOGFI("end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemSceneOcclusionAlpha(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double alpha = 0.f;
    if (!ConvertFromJsValue(env, argv[0], alpha)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSystemSceneOcclusionAlpha(alpha);
    WLOGFI("end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnResetOcclusionAlpha(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->ResetOcclusionAlpha();
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemSceneForceUIFirst(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        TLOGE(WmsLogTag::DEFAULT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool forceUIFirst = false;
    if (!ConvertFromJsValue(env, argv[0], forceUIFirst)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to convert parameter to forceUIFirst");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSystemSceneForceUIFirst(forceUIFirst);
    TLOGD(WmsLogTag::DEFAULT, "end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnMarkSystemSceneUIFirst(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        TLOGE(WmsLogTag::DEFAULT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isForced = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], isForced)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to convert parameter to isForced");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isUIFirstEnabled = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], isUIFirstEnabled)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to convert parameter to isUIFirstEnabled");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->MarkSystemSceneUIFirst(isForced, isUIFirstEnabled);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetFocusable(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isFocusable = false;
    if (!ConvertFromJsValue(env, argv[0], isFocusable)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetFocusable(isFocusable);
    TLOGD(WmsLogTag::WMS_FOCUS, "end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetFocusableOnShow(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        TLOGE(WmsLogTag::WMS_FOCUS, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isFocusableOnShow = true;
    if (!ConvertFromJsValue(env, argv[0], isFocusableOnShow)) {
        TLOGE(WmsLogTag::WMS_FOCUS, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetFocusableOnShow(isFocusableOnShow);
    TLOGD(WmsLogTag::WMS_FOCUS, "end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemFocusable(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        TLOGE(WmsLogTag::WMS_FOCUS, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool systemFocusable = false;
    if (!ConvertFromJsValue(env, argv[0], systemFocusable)) {
        TLOGE(WmsLogTag::WMS_FOCUS, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSystemFocusable(systemFocusable);
    TLOGD(WmsLogTag::WMS_FOCUS, "end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemSceneBlockingFocus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool blocking = false;
    if (!ConvertFromJsValue(env, argv[0], blocking)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSystemSceneBlockingFocus(blocking);
    WLOGFI("end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnMaskSupportEnterWaterfallMode(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->MaskSupportEnterWaterfallMode();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnUpdateFullScreenWaterfallMode(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARG_INDEX_1) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isWaterfallMode = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], isWaterfallMode)) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "Failed to convert parameter to isWaterfallMode");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->UpdateFullScreenWaterfallMode(isWaterfallMode);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnUpdateSizeChangeReason(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    if (!ConvertFromJsValue(env, argv[0], reason)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->UpdateSizeChangeReason(reason);
    WLOGFI("reason: %{public}u end", reason);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnOpenKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->OpenKeyboardSyncTransaction();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnCloseKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 3) { // 3: params num
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    napi_value nativeObj = argv[ARG_INDEX_0];
    if (nativeObj == nullptr || !ConvertRectInfoFromJs(env, nativeObj, keyboardPanelRect)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to convert parameter to keyboardPanelRect");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isKeyboardShow = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], isKeyboardShow)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isRotating = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], isRotating)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnNotifyTargetScreenWidthAndHeight(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARG_COUNT_3) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isScreenAngleMismatch = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], isScreenAngleMismatch)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    uint32_t screenWidth = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], screenWidth)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to convert parameter to uint32_t");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    uint32_t screenHeight = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], screenHeight)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to convert parameter to uint32_t");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->NotifyTargetScreenWidthAndHeight(isScreenAngleMismatch, screenWidth, screenHeight);
    return NapiGetUndefined(env);
}

void JsSceneSession::OnCreateSubSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }

    TLOGI(WmsLogTag::WMS_LIFE, "id: %{public}d, parentId: %{public}d",
        sceneSession->GetPersistentId(), sceneSession->GetParentPersistentId());
    wptr<SceneSession> weakSession(sceneSession);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, weakSession, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnCreateSubSession jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(CREATE_SUB_SESSION_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        auto specificSession = weakSession.promote();
        if (specificSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "root session or target session or env is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = Create(env, specificSession);
        if (jsSceneSessionObj == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSessionObj or jsCallBack is nullptr");
            return;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "CreateJsSceneSessionObject success, id: %{public}d, parentId: %{public}d",
            specificSession->GetPersistentId(), specificSession->GetParentPersistentId());
        napi_value argv[] = {jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string info = "OnCreateSpecificSession PID:" + std::to_string(sceneSession->GetPersistentId());
    taskScheduler_->PostMainThreadTask(task, info);
}

void JsSceneSession::OnBindDialogTarget(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFI("sceneSession is nullptr");
        return;
    }

    TLOGI(WmsLogTag::WMS_DIALOG, "id: %{public}d", sceneSession->GetPersistentId());

    wptr<SceneSession> weakSession(sceneSession);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, weakSession, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnBindDialogTarget jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(BIND_DIALOG_TARGET_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        auto specificSession = weakSession.promote();
        if (specificSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "root session or target session or env is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = Create(env, specificSession);
        if (jsSceneSessionObj == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSessionObj or jsCallBack is nullptr");
            return;
        }
        TLOGNI(WmsLogTag::WMS_DIALOG, "CreateJsObject success, id: %{public}d", specificSession->GetPersistentId());
        napi_value argv[] = {jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnBindDialogTarget, PID:" +
        std::to_string(sceneSession->GetPersistentId()));
}

void JsSceneSession::OnSessionStateChange(const SessionState& state)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGW(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }

    TLOGI(WmsLogTag::WMS_LIFE, "id: %{public}d, state: %{public}d", session->GetPersistentId(), state);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, state, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSessionStateChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_STATE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, state);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionStateChange, state:" + std::to_string(static_cast<int>(state)));
}

void JsSceneSession::OnBufferAvailableChange(const bool isBufferAvailable)
{
    WLOGFD("state: %{public}u", isBufferAvailable);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isBufferAvailable, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnBufferAvailableChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(BUFFER_AVAILABLE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsBufferAvailableObj = CreateJsValue(env, isBufferAvailable);
        napi_value argv[] = { jsBufferAvailableObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnBufferAvailableChange");
}

/** @note @window.layout */
void JsSceneSession::OnSessionRectChange(const WSRect& rect, SizeChangeReason reason, DisplayId displayId,
    const RectAnimationConfig& rectAnimationConfig)
{
    if (!IsMoveToOrDragMove(reason) && reason != SizeChangeReason::PIP_RESTORE && rect.IsEmpty()) {
        WLOGFD("Rect is empty, there is no need to notify");
        return;
    }
    WLOGFD("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, rect, displayId, reason,
        rectAnimationConfig, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "OnSessionRectChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_RECT_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionRect = CreateJsSessionRect(env, rect);
        napi_value jsSizeChangeReason = CreateJsValue(env, static_cast<int32_t>(reason));
        napi_value jsDisplayId = CreateJsValue(env, static_cast<int32_t>(displayId));
        napi_value jsAnimationConfigObj = CreateJsRectAnimationConfig(env, rectAnimationConfig);
        napi_value argv[] = { jsSessionRect, jsSizeChangeReason, jsDisplayId, jsAnimationConfigObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string rectInfo = "OnSessionRectChange [" + std::to_string(rect.posX_) + "," + std::to_string(rect.posY_)
        + "], [" + std::to_string(rect.width_) + ", " + std::to_string(rect.height_);
    taskScheduler_->PostMainThreadTask(task, rectInfo);
}

void JsSceneSession::OnWindowMoving(DisplayId displayId, int32_t pointerX, int32_t pointerY)
{
    auto task = [weakThis = wptr(this), persistentId = persistentId_, displayId, pointerX, pointerY, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_DECOR, "jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(WINDOW_MOVING_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_DECOR, "jsCallBack is nullptr");
            return;
        }
        napi_value jsDisplayId = CreateJsValue(env, static_cast<int64_t>(displayId));
        napi_value jsPointerX = CreateJsValue(env, pointerX);
        napi_value jsPointerY = CreateJsValue(env, pointerY);
        napi_value argv[] = {jsDisplayId, jsPointerX, jsPointerY};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnWindowMoving");
}

void JsSceneSession::OnSessionDisplayIdChange(uint64_t displayId)
{
    auto task = [weakThis = wptr(this), displayId, persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "OnSessionDisplayIdChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_DISPLAY_ID_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionDisplayIdObj = CreateJsValue(env, static_cast<int32_t>(displayId));
        napi_value argv[] = { jsSessionDisplayIdObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::OnSessionPiPControlStatusChange(WsPiPControlType controlType, WsPiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, status:%{public}d", controlType, status);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, controlType, status, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSessionPiPControlStatusChange jsSceneSession id:%{public}d has been"
                " destroyed", persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_PIP_CONTROL_STATUS_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_PIP, "jsCallBack is nullptr");
            return;
        }
        napi_value controlTypeValue = CreateJsValue(env, controlType);
        napi_value controlStatusValue = CreateJsValue(env, status);
        napi_value argv[] = {controlTypeValue, controlStatusValue};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::OnAutoStartPiPStatusChange(bool isAutoStart, uint32_t priority)
{
    TLOGI(WmsLogTag::WMS_PIP, "isAutoStart:%{public}u priority:%{public}u", isAutoStart, priority);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isAutoStart, priority, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_PIP, "jsSceneSession id:%{public}d has been destroyed", persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_AUTO_START_PIP_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_PIP, "jsCallBack is nullptr");
            return;
        }
        napi_value isAutoStartValue = CreateJsValue(env, isAutoStart);
        napi_value priorityValue = CreateJsValue(env, priority);
        napi_value argv[] = {isAutoStartValue, priorityValue};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

/** @note @window.hierarchy */
void JsSceneSession::OnRaiseToTop()
{
    WLOGFI("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGE(WmsLogTag::WMS_HIERARCHY, "OnRaiseToTop jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(RAISE_TO_TOP_CB);
        if (!jsCallBack) {
            WLOGFE("jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseToTop");
}

/** @note @window.hierarchy */
void JsSceneSession::OnRaiseToTopForPointDown()
{
    WLOGFI("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "OnRaiseToTopForPointDown jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(RAISE_TO_TOP_POINT_DOWN_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseToTopForPointDown");
}

void JsSceneSession::OnClickModalWindowOutside()
{
    TLOGD(WmsLogTag::WMS_PC, "[NAPI]");
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(CLICK_MODAL_WINDOW_OUTSIDE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_PC, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::OnRaiseAboveTarget(int32_t subWindowId)
{
    WLOGFI("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_, subWindowId] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnRaiseAboveTarget jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(RAISE_ABOVE_TARGET_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = CreateJsValue(env, subWindowId);
        if (jsSceneSessionObj == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSessionObj is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsError(env, 0), jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseAboveTarget: " + std::to_string(subWindowId));
}

void JsSceneSession::OnSessionFocusableChange(bool isFocusable)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "state: %{public}u", isFocusable);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isFocusable, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSessionFocusableChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_FOCUSABLE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionFocusableObj = CreateJsValue(env, isFocusable);
        napi_value argv[] = {jsSessionFocusableObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionFocusableChange, state:" + std::to_string(isFocusable));
}

void JsSceneSession::OnSessionTouchableChange(bool touchable)
{
    WLOGFI("state: %{public}u", touchable);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, touchable, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSessionTouchableChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_TOUCHABLE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionTouchableObj = CreateJsValue(env, touchable);
        napi_value argv[] = {jsSessionTouchableObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionTouchableChange: state " + std::to_string(touchable));
}

/** @note @window.hierarchy */
void JsSceneSession::OnSessionTopmostChange(bool topmost)
{
    TLOGI(WmsLogTag::WMS_HIERARCHY, "State: %{public}u", topmost);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, topmost, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "OnSessionTopmostChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_TOP_MOST_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionTouchableObj = CreateJsValue(env, topmost);
        napi_value argv[] = {jsSessionTouchableObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionTopmostChange: state " + std::to_string(topmost));
}

/** @note @window.hierarchy */
void JsSceneSession::OnMainWindowTopmostChange(bool isTopmost)
{
    TLOGD(WmsLogTag::WMS_HIERARCHY, "isTopmost: %{public}u", isTopmost);
    const char* const where = __func__;
    taskScheduler_->PostMainThreadTask([weakThis = wptr(this), persistentId = persistentId_,
        isTopmost, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(MAIN_WINDOW_TOP_MOST_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "jsCallBack is nullptr");
            return;
        }
        napi_value jsMainWindowTopmostObj = CreateJsValue(env, isTopmost);
        napi_value argv[] = { jsMainWindowTopmostObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    }, "OnMainWindowTopmostChange: " + std::to_string(isTopmost));
}

void JsSceneSession::OnSubModalTypeChange(SubWindowModalType subWindowModalType)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, subWindowModalType, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SUB_MODAL_TYPE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value jsSessionModalTypeObj = CreateJsValue(env, subWindowModalType);
        napi_value argv[] = {jsSessionModalTypeObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task,
        "OnSubModalTypeChange: " + std::to_string(static_cast<uint32_t>(subWindowModalType)));
}

void JsSceneSession::OnMainModalTypeChange(bool isModal)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isModal, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(MAIN_MODAL_TYPE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value jsMainSessionModalType = CreateJsValue(env, isModal);
        napi_value argv[] = {jsMainSessionModalType};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnMainModalTypeChange: " + std::to_string(isModal));
}

void JsSceneSession::OnThrowSlipAnimationStateChange(bool isAnimating)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isAnimating, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsSceneSession id: %{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(THROW_SLIP_ANIMATION_STATE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value jsIsAnimating = CreateJsValue(env, isAnimating);
        napi_value argv[] = { jsIsAnimating };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::OnFullScreenWaterfallModeChange(bool isWaterfallMode)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isWaterfallMode, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s jsSceneSession id: %{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(FULLSCREEN_WATERFALL_MODE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value jsIsWaterfallMode = CreateJsValue(env, isWaterfallMode);
        napi_value argv[] = { jsIsWaterfallMode };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::OnClick(bool requestFocus, bool isClick)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "id: %{public}d, requestFocus: %{public}u, isClick: %{public}u",
        persistentId_, requestFocus, isClick);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, requestFocus, isClick, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnClick jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(CLICK_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsRequestFocusObj = CreateJsValue(env, requestFocus);
        napi_value jsIsClickObj = CreateJsValue(env, isClick);
        napi_value argv[] = {jsRequestFocusObj, jsIsClickObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnClick: requestFocus" + std::to_string(requestFocus));
}

void JsSceneSession::OnContextTransparent()
{
    WLOGFD("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnContextTransparent jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(CONTEXT_TRANSPARENT_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnContextTransparent");
}

void JsSceneSession::ChangeSessionVisibilityWithStatusBar(const SessionInfo& info, bool visible)
{
    WLOGI("bundleName %{public}s, moduleName %{public}s, "
        "abilityName %{public}s, appIndex %{public}d, reuse %{public}d, visible %{public}d",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str(),
        info.appIndex_, info.reuse, visible);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [weakThis = wptr(this), sessionInfo, visible]() {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ChangeSessionVisibilityWithStatusBar jsSceneSession is null");
            return;
        }
        jsSceneSession->ChangeSessionVisibilityWithStatusBarInner(sessionInfo, visible);
    };
    taskScheduler_->PostMainThreadTask(task, "ChangeSessionVisibilityWithStatusBar, visible:" +
        std::to_string(visible));
}

void JsSceneSession::ChangeSessionVisibilityWithStatusBarInner(std::shared_ptr<SessionInfo> sessionInfo, bool visible)
{
    std::shared_ptr<NativeReference> jsCallBack = GetJSCallback(CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR);
    if (!jsCallBack) {
        WLOGFE("jsCallBack is nullptr");
        return;
    }
    if (sessionInfo == nullptr) {
        WLOGFE("sessionInfo is nullptr");
        return;
    }
    napi_value jsSessionInfo = CreateJsSessionInfo(env_, *sessionInfo);
    if (jsSessionInfo == nullptr) {
        WLOGFE("target session info is nullptr");
        return;
    }
    napi_value visibleNapiV = CreateJsValue(env_, visible);
    napi_value argv[] = {jsSessionInfo, visibleNapiV};
    napi_call_function(env_, NapiGetUndefined(env_),
        jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
}

sptr<SceneSession> JsSceneSession::GenSceneSession(SessionInfo& info)
{
    sptr<SceneSession> sceneSession = nullptr;
    if (info.persistentId_ == 0) {
        auto result = SceneSessionManager::GetInstance().CheckIfReuseSession(info);
        if (result == BrokerStates::BROKER_NOT_START) {
            TLOGE(WmsLogTag::WMS_LIFE, "BrokerStates not started");
            return nullptr;
        }
        if (info.reuse || info.isAtomicService_) {
            TLOGI(WmsLogTag::WMS_LIFE, "session need to be reusesd.");
            if (SceneSessionManager::GetInstance().CheckCollaboratorType(info.collaboratorType_)) {
                sceneSession = SceneSessionManager::GetInstance().FindSessionByAffinity(info.sessionAffinity);
            } else {
                SessionIdentityInfo identityInfo = { info.bundleName_, info.moduleName_, info.abilityName_,
                    info.appIndex_, info.appInstanceKey_, info.windowType_, info.isAtomicService_ };
                sceneSession = SceneSessionManager::GetInstance().GetSceneSessionByIdentityInfo(identityInfo);
            }
        }
        if (sceneSession == nullptr) {
            TLOGI(WmsLogTag::WMS_LIFE, "SceneSession not exist, request a new one.");
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LIFE, "RequestSceneSession return nullptr");
                return nullptr;
            }
        } else {
            sceneSession->SetSessionInfo(info);
        }
        info.persistentId_ = sceneSession->GetPersistentId();
        sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
    } else {
        sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "GetSceneSession return nullptr");
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LIFE, "retry RequestSceneSession return nullptr");
                return nullptr;
            }
            info.persistentId_ = sceneSession->GetPersistentId();
            sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
        } else {
            sceneSession->SetSessionInfo(info);
        }
    }
    return sceneSession;
}

void JsSceneSession::PendingSessionActivation(SessionInfo& info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName %{public}s, moduleName %{public}s, abilityName %{public}s, "
        "appIndex %{public}d, reuse %{public}d, specifiedId %{public}d",
        info.bundleName_.c_str(), info.moduleName_.c_str(),
        info.abilityName_.c_str(), info.appIndex_, info.reuse, info.specifiedId);
    auto sceneSession = GenSceneSession(info);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "GenSceneSession failed");
        return;
    }

    if (info.want != nullptr) {
        auto focusedOnShow = info.want->GetBoolParam(AAFwk::Want::PARAM_RESV_WINDOW_FOCUSED, true);
        sceneSession->SetFocusedOnShow(focusedOnShow);
    } else {
        sceneSession->SetFocusedOnShow(true);
    }

    auto callerSession = SceneSessionManager::GetInstance().GetSceneSession(info.callerPersistentId_);
    if (callerSession != nullptr) {
        info.isCalledRightlyByCallerId_ = (info.callerToken_ == callerSession->GetAbilityToken()) &&
            SessionPermission::VerifyPermissionByBundleName(info.bundleName_,
                                                            "ohos.permission.CALLED_TRANSITION_ON_LOCK_SCREEN",
                                                            SceneSessionManager::GetInstance().GetCurrentUserId());
        TLOGI(WmsLogTag::WMS_SCB,
            "isCalledRightlyByCallerId result is: %{public}d", info.isCalledRightlyByCallerId_);
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [weakThis = wptr(this), sessionInfo] {
        auto jsSceneSession = weakThis.promote();
        if (jsSceneSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "PendingSessionActivation JsSceneSession is null");
            return;
        }
        jsSceneSession->PendingSessionActivationInner(sessionInfo);
    };
    sceneSession->PostLifeCycleTask(task, "PendingSessionActivation", LifeCycleTaskType::START);
    if (info.fullScreenStart_) {
        sceneSession->NotifySessionFullScreen(true);
    }
}

void JsSceneSession::PendingSessionActivationInner(std::shared_ptr<SessionInfo> sessionInfo)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, weakSession = weakSession_,
        sessionInfo, env = env_, where] {
        auto session = weakSession.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "session is nullptr");
            return;
        }
        bool isFromAncoAndToAnco = session->IsAnco() && AbilityInfoManager::GetInstance().IsAnco(
            sessionInfo->bundleName_, sessionInfo->abilityName_, sessionInfo->moduleName_);
        if (session->DisallowActivationFromPendingBackground(sessionInfo->isPcOrPadEnableActivation_,
            sessionInfo->isFoundationCall_, sessionInfo->canStartAbilityFromBackground_, isFromAncoAndToAnco)) {
            return;
        }
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "PendingSessionActivationInner jsSceneSession "
                "id:%{public}d has been destroyed", persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(PENDING_SCENE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s task success, "
            "id:%{public}d, specifiedId:%{public}d",
            where, sessionInfo->persistentId_, sessionInfo->specifiedId);
        napi_call_function(env, NapiGetUndefined(env),
            jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionActivationInner");
}

void JsSceneSession::OnBackPressed(bool needMoveToBackground)
{
    WLOGFI("%{public}d", needMoveToBackground);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, needMoveToBackground, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnBackPressed jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(BACK_PRESSED_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsNeedMoveToBackgroundObj = CreateJsValue(env, needMoveToBackground);
        napi_value argv[] = {jsNeedMoveToBackgroundObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnBackPressed:" + std::to_string(needMoveToBackground));
}

void JsSceneSession::TerminateSession(const SessionInfo& info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "bundle:%{public}s, ability:%{public}s, id:%{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), info.persistentId_);

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, sessionInfo, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "TerminateSession jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(TERMINATE_SESSION_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSession name:" + info.abilityName_);
}

void JsSceneSession::TerminateSessionNew(const SessionInfo& info, bool needStartCaller, bool isFromBroker)
{
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName=%{public}s, abilityName=%{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    bool needRemoveSession = false;
    if (!needStartCaller && !isFromBroker) {
        needRemoveSession = true;
    }
    auto task = [weakThis = wptr(this), persistentId = persistentId_, needStartCaller, needRemoveSession, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "TerminateSessionNew jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(TERMINATE_SESSION_CB_NEW);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsNeedStartCaller = CreateJsValue(env, needStartCaller);
        if (jsNeedStartCaller == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsNeedStartCaller is nullptr");
            return;
        }
        napi_value jsNeedRemoveSession = CreateJsValue(env, needRemoveSession);
        if (jsNeedRemoveSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsNeedRemoveSession is nullptr");
            return;
        }
        napi_value argv[] = {jsNeedStartCaller, jsNeedRemoveSession};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSessionNew, name:" + info.abilityName_);
}

void JsSceneSession::TerminateSessionTotal(const SessionInfo& info, TerminateType terminateType)
{
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName=%{public}s, abilityName=%{public}s, terminateType=%{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), static_cast<int32_t>(terminateType));

    auto task = [weakThis = wptr(this), persistentId = persistentId_, terminateType, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "TerminateSessionTotal jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(TERMINATE_SESSION_CB_TOTAL);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsTerminateType = CreateJsValue(env, static_cast<int32_t>(terminateType));
        if (jsTerminateType == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsTerminateType is nullptr");
            return;
        }
        napi_value argv[] = {jsTerminateType};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSessionTotal:name:" + info.abilityName_);
}

void JsSceneSession::UpdateSessionLabel(const std::string& label)
{
    WLOGFI("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, label, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "UpdateSessionLabel jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(UPDATE_SESSION_LABEL_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsLabel = CreateJsValue(env, label);
        if (jsLabel == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "this target jsLabel is nullptr");
            return;
        }
        napi_value argv[] = {jsLabel};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "UpdateSessionLabel");
}

void JsSceneSession::ProcessUpdateSessionLabelRegister()
{
    WLOGFD("in");
    NofitySessionLabelUpdatedFunc func = [weakThis = wptr(this)](const std::string& label) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessUpdateSessionLabelRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->UpdateSessionLabel(label);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetUpdateSessionLabelListener(func);
    WLOGFD("success");
}

void JsSceneSession::ProcessUpdateSessionIconRegister()
{
    WLOGFD("in");
    NofitySessionIconUpdatedFunc func = [weakThis = wptr(this)](const std::string& iconPath) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "ProcessUpdateSessionIconRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->UpdateSessionIcon(iconPath);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetUpdateSessionIconListener(func);
    WLOGFD("success");
}

void JsSceneSession::UpdateSessionIcon(const std::string& iconPath)
{
    WLOGFI("[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, iconPath, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "UpdateSessionIcon jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(UPDATE_SESSION_ICON_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsIconPath = CreateJsValue(env, iconPath);
        if (jsIconPath == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "this target jsIconPath is nullptr");
            return;
        }
        napi_value argv[] = {jsIconPath};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "UpdateSessionIcon");
}

void JsSceneSession::OnSessionException(const SessionInfo& info, bool needRemoveSession, bool startFail)
{
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName=%{public}s, abilityName=%{public}s, startFail=%{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), startFail);

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [weakThis = wptr(this), persistentId = persistentId_,
        sessionInfo, needRemoveSession, startFail, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSessionException jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_EXCEPTION_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        napi_value jsNeedRemoveSession = CreateJsValue(env, needRemoveSession);
        napi_value jsStartFail = CreateJsValue(env, startFail);
        if (jsSessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo, jsNeedRemoveSession, jsStartFail};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionException, name" + info.bundleName_);
}

void JsSceneSession::PendingSessionToForeground(const SessionInfo& info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName=%{public}s, abilityName=%{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, sessionInfo, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "PendingSessionToForeground jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(PENDING_SESSION_TO_FOREGROUND_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionToForeground:" + info.bundleName_);
}

void JsSceneSession::PendingSessionToBackgroundForDelegator(const SessionInfo& info, bool shouldBackToCaller)
{
    TLOGI(WmsLogTag::WMS_LIFE,
        "bundleName=%{public}s, abilityName=%{public}s, shouldBackToCaller=%{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), shouldBackToCaller);

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, sessionInfo, env = env_, shouldBackToCaller] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsSceneSession id:%{public}d has been destroyed", persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        napi_value jsShouldBackToCaller = CreateJsValue(env, shouldBackToCaller);
        if (jsSessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo, jsShouldBackToCaller};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionToBackgroundForDelegator, name:" + info.bundleName_);
}

void JsSceneSession::OnSystemBarPropertyChange(const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    TLOGD(WmsLogTag::WMS_IMMS, "[NAPI]");
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, propertyMap, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnSystemBarPropertyChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SYSTEMBAR_PROPERTY_CHANGE_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsArrayObject = CreateJsSystemBarPropertyArrayObject(env, propertyMap);
        if (jsArrayObject == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s jsArrayObject is nullptr", where);
            return;
        }
        napi_value argv[] = {jsArrayObject};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSystemBarPropertyChange");
}

void JsSceneSession::OnNeedAvoid(bool status)
{
    TLOGD(WmsLogTag::WMS_IMMS, "%{public}d", status);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, needAvoid = status, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnNeedAvoid jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(NEED_AVOID_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, needAvoid);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnNeedAvoid:" + std::to_string(status));
}

void JsSceneSession::OnIsCustomAnimationPlaying(bool status)
{
    WLOGFI("%{public}d", status);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isPlaying = status, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnIsCustomAnimationPlaying jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(CUSTOM_ANIMATION_PLAYING_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, isPlaying);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnIsCustomAnimationPlaying:" + std::to_string(status));
}

void JsSceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFI("%{public}d", showWhenLocked);
    auto task = [weakThis = wptr(this), persistentId = persistentId_, flag = showWhenLocked, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnShowWhenLocked jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SHOW_WHEN_LOCKED_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, flag);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnShowWhenLocked:" +std::to_string(showWhenLocked));
}

void JsSceneSession::OnReuqestedOrientationChange(uint32_t orientation)
{
    WLOGFI("%{public}u", orientation);

    auto task = [weakThis = wptr(this), persistentId = persistentId_, rotation = orientation, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnReuqestedOrientationChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(REQUESTED_ORIENTATION_CHANGE_CB);
        if (jsCallBack == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionRotationObj = CreateJsValue(env, rotation);
        napi_value argv[] = {jsSessionRotationObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        WLOGFI("change rotation success %{public}u", rotation);
    };
    taskScheduler_->PostMainThreadTask(task, "OnReuqestedOrientationChange:orientation" +std::to_string(orientation));
}

napi_value JsSceneSession::OnSetShowRecent(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is null, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    bool showRecent = true;
    if (argc == ARGC_ONE && GetType(env, argv[0]) == napi_boolean) {
        if (!ConvertFromJsValue(env, argv[0], showRecent)) {
            WLOGFE("Failed to convert parameter to bool");
            return NapiGetUndefined(env);
        }
    }
    session->SetShowRecent(showRecent);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetZOrder(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    uint32_t zOrder;
    if (!ConvertFromJsValue(env, argv[0], zOrder)) {
        WLOGFE("Failed to convert parameter to uint32_t");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFW("session is null, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetZOrder(zOrder);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetFloatingScale(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t floatingScale = 1.0;
    if (!ConvertFromJsValue(env, argv[0], floatingScale)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetFloatingScale(static_cast<float_t>(floatingScale));
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetIsMidScene(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isMidScene = false;
    if (!ConvertFromJsValue(env, argv[0], isMidScene)) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "Failed to convert parameter to isMidScene");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetIsMidScene(isMidScene);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSCBKeepKeyboard(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool scbKeepKeyboardFlag = false;
    if (!ConvertFromJsValue(env, argv[0], scbKeepKeyboardFlag)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSCBKeepKeyboard(scbKeepKeyboardFlag);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetOffset(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 2) {
        WLOGFE("Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    double offsetX = 0.0f;
    if (!ConvertFromJsValue(env, argv[0], offsetX)) {
        WLOGFE("Failed to convert parameter to double");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    double offsetY = 0.0f;
    if (!ConvertFromJsValue(env, argv[1], offsetY)) {
        WLOGFE("Failed to convert parameter to double");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetOffset(static_cast<float>(offsetX), static_cast<float>(offsetY));
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetExitSplitOnBackground(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isExitSplitOnBackground = false;
    if (!ConvertFromJsValue(env, argv[0], isExitSplitOnBackground)) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetExitSplitOnBackground(isExitSplitOnBackground);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetWaterMarkFlag(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isWaterMarkAdded = false;
    if (!ConvertFromJsValue(env, argv[0], isWaterMarkAdded)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    uint32_t currFlag = session->GetSessionProperty()->GetWindowFlags();
    if (isWaterMarkAdded) {
        currFlag = currFlag | static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    } else {
        currFlag = currFlag & ~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK));
    }
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetSystemCalling(true);
    property->SetWindowFlags(currFlag);
    SceneSessionManager::GetInstance().SetWindowFlags(session, property);
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessPrepareClosePiPSessionRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterProcessPrepareClosePiPCallback([weakThis = wptr(this)] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnPrepareClosePiPSession jsSceneSession is null");
            return;
        }
        jsSceneSession->OnPrepareClosePiPSession();
    });
    TLOGD(WmsLogTag::WMS_PIP, "success");
}

void JsSceneSession::OnPrepareClosePiPSession()
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]");
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "OnPrepareClosePiPSession jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(PREPARE_CLOSE_PIP_SESSION);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_PIP, "jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnPrepareClosePiPSession");
}

napi_value JsSceneSession::OnSetSystemActive(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("argc is invalid : %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool scbSystemActive = false;
    if (!ConvertFromJsValue(env, argv[0], scbSystemActive)){
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is null, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSystemActive(scbSystemActive);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetTouchable(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input Parameter is missing or invalid" ));
        return NapiGetUndefined(env);
    }

    bool touchable = false;
    if (!ConvertFromJsValue(env, argv[0], touchable)) {
        WLOGFE("Failed to convert parameter to touchable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is null, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }

    session->SetSystemTouchable(touchable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::SetScale(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
    WLOGFI("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetScale(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetScale(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARG_COUNT_4) { // ARG_COUNT_4: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t scaleX = 1.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], scaleX)) {
        WLOGFE("Failed to convert parameter to scaleX");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t scaleY = 1.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], scaleY)) {
        WLOGFE("Failed to convert parameter to scaleY");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t pivotX = 0.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], pivotX)) {
        WLOGFE("Failed to convert parameter to pivotX");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t pivotY = 0.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_3], pivotY)) {
        WLOGFE("Failed to convert parameter to pivotY");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetScale(static_cast<float_t>(scaleX), static_cast<float_t>(scaleY), static_cast<float_t>(pivotX),
        static_cast<float_t>(pivotY));
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::SetWindowLastSafeRect(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetWindowLastSafeRect(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetWindowLastSafeRect(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARG_COUNT_4) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t left = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], left)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to left");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t top = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], top)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to top");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t width = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], width)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to width");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t height = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_3], height)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to height");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    WSRect lastRect = { left, top, width, height };
    session->SetSessionRequestRect(lastRect);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::SetMovable(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetMovable(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetMovable(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input Parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool movable = true;
    if (!ConvertFromJsValue(env, argv[0], movable)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to movable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetMovable(movable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::SetSplitButtonVisible(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSplitButtonVisible(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetSplitButtonVisible(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isVisible = true;
    if (!ConvertFromJsValue(env, argv[0], isVisible)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSplitButtonVisible(isVisible);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::RequestHideKeyboard(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnRequestHideKeyboard(env, info) : nullptr;
}

napi_value JsSceneSession::OnRequestHideKeyboard(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->RequestHideKeyboard();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetPipActionEvent(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_PIP, "Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string action;
    if (!ConvertFromJsValue(env, argv[0], action)) {
        TLOGE(WmsLogTag::WMS_PIP, "Failed to convert parameter to string");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t status = -1;
    if (argc > 1) {
        if (!ConvertFromJsValue(env, argv[1], status)) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to convert parameter to int");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                          "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetPipActionEvent(action, status);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetPiPControlEvent(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_PIP, "Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    if (!ConvertFromJsValue(env, argv[0], controlType)) {
        TLOGE(WmsLogTag::WMS_PIP, "Failed to convert parameter to int");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto status = WsPiPControlStatus::PLAY;
    if (argc > 1) {
        if (!ConvertFromJsValue(env, argv[1], status)) {
            TLOGE(WmsLogTag::WMS_PIP, "Failed to convert parameter to int");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetPiPControlEvent(controlType, status);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnNotifyPipOcclusionChange(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_PIP, "Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool occluded = false;
    if (!ConvertFromJsValue(env, argv[0], occluded)) {
        TLOGE(WmsLogTag::WMS_PIP, "Failed to convert parameter, keep default: false");
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    TLOGI(WmsLogTag::WMS_PIP, "persistId:%{public}d, occluded:%{public}d", persistentId_, occluded);
    // Maybe expand with session visibility&state change
    SceneSessionManager::GetInstance().HandleKeepScreenOn(session, !occluded);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnNotifyDisplayStatusBarTemporarily(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "session is nullptr, win %{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    bool isTempDisplay = false;
    if (argc == ARGC_ONE && GetType(env, argv[0]) == napi_boolean) {
        if (!ConvertFromJsValue(env, argv[0], isTempDisplay)) {
            TLOGE(WmsLogTag::WMS_IMMS, "failed to convert parameter to bool");
            return NapiGetUndefined(env);
        }
    }
    session->SetIsDisplayStatusBarTemporarily(isTempDisplay);

    TLOGI(WmsLogTag::WMS_IMMS, "win [%{public}u %{public}s] isTempDisplay %{public}u",
        session->GetPersistentId(), session->GetWindowName().c_str(), isTempDisplay);
    return NapiGetUndefined(env);
}

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return weakSession_.promote();
}

napi_value JsSceneSession::OnSetTemporarilyShowWhenLocked(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_SCB, "argc is invalid : %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isTemporarilyShowWhenLocked = false;
    if (!ConvertFromJsValue(env, argv[0], isTemporarilyShowWhenLocked)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session_ is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetTemporarilyShowWhenLocked(isTemporarilyShowWhenLocked);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSkipDraw(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_SCB, "argc is invalid : %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool skip = false;
    if (!ConvertFromJsValue(env, argv[0], skip)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session_ is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetSkipDraw(skip);
    return NapiGetUndefined(env);
}

std::shared_ptr<NativeReference> JsSceneSession::GetJSCallback(const std::string& functionName)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession::GetJSCallback[%s]", functionName.c_str());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
    auto iter = jsCbMap_.find(functionName);
    if (iter == jsCbMap_.end()) {
        TLOGE(WmsLogTag::DEFAULT, "%{public}s callback not found!", functionName.c_str());
    } else {
        jsCallBack = iter->second;
    }
    return jsCallBack;
}

napi_value JsSceneSession::OnSetSkipSelfWhenShowOnVirtualScreen(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isSkip = false;
    if (!ConvertFromJsValue(env, argv[0], isSkip)) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetSkipSelfWhenShowOnVirtualScreen(isSkip);
    WLOGFI("end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetCompatibleModeInPc(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < ARGC_TWO) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isSupportDragInPcCompatibleMode = false;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], isSupportDragInPcCompatibleMode)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to isSupportDragInPcCompatibleMode");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetCompatibleModeInPc(enable, isSupportDragInPcCompatibleMode);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetAppSupportPhoneInPc(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isSupportPhone = false;
    if (!ConvertFromJsValue(env, argv[0], isSupportPhone)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to isSupportPhone");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetAppSupportPhoneInPc(isSupportPhone);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetCompatibleWindowSizeInPc(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_FOUR) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t portraitWidth = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], portraitWidth)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to portraitWidth");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t portraitHeight = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], portraitHeight)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to portraitHeight");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t landscapeWidth = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], landscapeWidth)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to landscapeWidth");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t landscapeHeight = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_3], landscapeHeight)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to landscapeHeight");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetCompatibleWindowSizeInPc(portraitWidth, portraitHeight, landscapeWidth, landscapeHeight);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetCompatibleModeEnableInPad(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], enable)) {
        TLOGE(WmsLogTag::WMS_COMPAT, "Failed to convert parameter to portraitWidth");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetCompatibleModeEnableInPad(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetUniqueDensityDpiFromSCB(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    bool paramValidFlag = true;
    bool useUnique = false;
    uint32_t densityDpi = 0;
    std::string errMsg = "";
    if (argc < ARGC_TWO) {
        TLOGE(WmsLogTag::WMS_SCB, "Params not match %{public}zu", argc);
        errMsg = "Invalid args count, need two arg!";
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(env, argv[0], useUnique)) {
            TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to useUnique");
            errMsg = "Failed to convert parameter to useUnique";
            paramValidFlag = false;
        }
        if (paramValidFlag && !ConvertFromJsValue(env, argv[1], densityDpi)) {
            TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to densityDpi");
            errMsg = "Failed to convert parameter to densityDpi";
            paramValidFlag = false;
        }
    }
    if (!paramValidFlag) {
        TLOGE(WmsLogTag::WMS_SCB, "paramValidFlag error");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), errMsg));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (!session) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetUniqueDensityDpi(useUnique, densityDpi);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetBlank(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isAddBlank = false;
    if (!ConvertFromJsValue(env, argv[0], isAddBlank)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to isAddBlank");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetBlank(isAddBlank);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnRemoveBlank(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->NotifyRemoveBlank();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnAddSnapshot(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->NotifyAddSnapshot();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnRemoveSnapshot(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->NotifyRemoveSnapshot();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetBufferAvailableCallbackEnable(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetBufferAvailableCallbackEnable(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSyncDefaultRequestedOrientation(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    uint32_t defaultRequestedOrientation = 0;
    if (!ConvertFromJsValue(env, argv[0], defaultRequestedOrientation)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to defaultRequestedOrientation");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    auto windowOrientation = static_cast<Orientation>(defaultRequestedOrientation);
    if (windowOrientation < Orientation::BEGIN || windowOrientation > Orientation::END) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Orientation %{public}u invalid, id:%{public}d",
            defaultRequestedOrientation, persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetDefaultRequestedOrientation(windowOrientation);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetIsPcAppInPad(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetIsPcAppInPad(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetStartingWindowExitAnimationFlag(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = true;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is null, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetStartingWindowExitAnimationFlag(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnCompatibleFullScreenRecover(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->CompatibleFullScreenRecover();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnCompatibleFullScreenMinimize(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->CompatibleFullScreenMinimize();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnCompatibleFullScreenClose(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_COMPAT, "[NAPI]");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_COMPAT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->CompatibleFullScreenClose();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetWindowEnableDragBySystem(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enableDrag = true;
    if (!ConvertFromJsValue(env, argv[0], enableDrag)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to bool");
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetWindowEnableDragBySystem(enableDrag);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnActivateDragBySystem(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool activateDrag = true;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], activateDrag)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to bool");
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->ActivateDragBySystem(activateDrag);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSyncScenePanelGlobalPosition(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    std::string reason;
    if (!ConvertFromJsValue(env, argv[0], reason)) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "Failed to convert parameter to sync reason");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto it = std::find_if(g_syncGlobalPositionPermission.begin(), g_syncGlobalPositionPermission.end(),
        [reason](const std::string& permission) { return permission.find(reason) != std::string::npos; });
    if (it == g_syncGlobalPositionPermission.end()) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "called reason:%{public}s is not permitted", reason.c_str());
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_NO_PERMISSION),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    TLOGI(WmsLogTag::WMS_PIPELINE, "called reason:%{public}s", reason.c_str());
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SyncScenePanelGlobalPosition(true);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnUnSyncScenePanelGlobalPosition(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    std::string reason;
    if (!ConvertFromJsValue(env, argv[0], reason)) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "Failed to convert parameter to un sync reason");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto it = std::find_if(g_syncGlobalPositionPermission.begin(), g_syncGlobalPositionPermission.end(),
        [reason](const std::string& permission) { return permission.find(reason) != std::string::npos; });
    if (it == g_syncGlobalPositionPermission.end()) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "called reason:%{public}s is not permitted", reason.c_str());
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_NO_PERMISSION),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    TLOGI(WmsLogTag::WMS_PIPELINE, "called reason:%{public}s", reason.c_str());
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SyncScenePanelGlobalPosition(false);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetIsPendingToBackgroundState(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is null, id: %{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    bool isPendingToBackgroundState = false;
    if (GetType(env, argv[0]) == napi_boolean) {
        if (!ConvertFromJsValue(env, argv[0], isPendingToBackgroundState)) {
            TLOGE(WmsLogTag::WMS_LIFE, "Failed to convert parameter to isPendingToBackgroundState");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
        TLOGD(WmsLogTag::WMS_LIFE, "isPendingToBackgroundState: %{public}u", isPendingToBackgroundState);
    }
    session->SetIsPendingToBackgroundState(isPendingToBackgroundState);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetIsActivatedAfterScreenLocked(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    bool isActivatedAfterScreenLocked = false;
    if (!ConvertFromJsValue(env, argv[0], isActivatedAfterScreenLocked)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Failed to convert parameter to isActivatedAfterScreenLocked");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    TLOGD(WmsLogTag::WMS_LIFE, "isActivatedAfterScreenLocked: %{public}u", isActivatedAfterScreenLocked);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is null, id: %{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetIsActivatedAfterScreenLocked(isActivatedAfterScreenLocked);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetNeedSyncSessionRect(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool needSync = true;
    if (!ConvertFromJsValue(env, argv[0], needSync)) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "Failed to convert parameter to needSync");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    TLOGI(WmsLogTag::WMS_PIPELINE, "needSync:%{public}u, id:%{public}d", needSync, persistentId_);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetNeedSyncSessionRect(needSync);
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessSetWindowRectAutoSaveRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetWindowRectAutoSaveCallback([weakThis = wptr(this), where](bool enabled) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSetWindowRectAutoSave(enabled);
    });
    TLOGI(WmsLogTag::WMS_MAIN, "success");
}

void JsSceneSession::OnSetWindowRectAutoSave(bool enabled)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, enabled, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SET_WINDOW_RECT_AUTO_SAVE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: jsCallBack is nullptr", where);
            return;
        }
        napi_value jsEnabled = CreateJsValue(env, enabled);
        napi_value argv[] = {jsEnabled};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::ProcessSetSupportedWindowModesRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterSupportWindowModesCallback([weakThis = wptr(this), where](
        std::vector<AppExecFwk::SupportWindowMode>&& supportedWindowModes) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s: jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSetSupportedWindowModes(std::move(supportedWindowModes));
    });
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "success");
}

void JsSceneSession::OnSetSupportedWindowModes(std::vector<AppExecFwk::SupportWindowMode>&& supportedWindowModes)
{
    const char* const where = __func__;
    taskScheduler_->PostMainThreadTask([weakThis = wptr(this), persistentId = persistentId_,
        supportedWindowModes = std::move(supportedWindowModes), env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s: jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SET_SUPPORT_WINDOW_MODES_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s: jsCallBack is nullptr", where);
            return;
        }
        napi_value jsSupportWindowModes = CreateSupportWindowModes(env, supportedWindowModes);
        napi_value argv[] = { jsSupportWindowModes };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    }, __func__);
}

void JsSceneSession::RegisterUpdateAppUseControlCallback()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->RegisterUpdateAppUseControlCallback(
        [weakThis = wptr(this), where](ControlAppType type, bool isNeedControl) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnUpdateAppUseControl(type, isNeedControl);
    });
    TLOGI(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::OnUpdateAppUseControl(ControlAppType type, bool isNeedControl)
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, type, isNeedControl, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(UPDATE_APP_USE_CONTROL_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: jsCallBack is nullptr", where);
            return;
        }
        napi_value jsTypeArgv = CreateJsValue(env, static_cast<uint8_t>(type));
        napi_value jsIsNeedControlArgv = CreateJsValue(env, isNeedControl);
        napi_value argv[] = { jsTypeArgv, jsIsNeedControlArgv };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

napi_value JsSceneSession::OnSetFrameGravity(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    Gravity gravity = Gravity::TOP_LEFT;
    if (!ConvertFromJsValue(env, argv[0], gravity)) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "Failed to convert parameter to gravity");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetFrameGravity(gravity);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetUseStartingWindowAboveLocked(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_PATTERN, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool useStartingWindowAboveLocked = false;
    if (!ConvertFromJsValue(env, argv[0], useStartingWindowAboveLocked)) {
        TLOGE(WmsLogTag::WMS_PATTERN, "Failed to convert parameter to useStartingWindowAboveLocked");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetUseStartingWindowAboveLocked(useStartingWindowAboveLocked);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSaveSnapshotSync(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SaveSnapshot(false);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetBehindWindowFilterEnabled(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool enabled = true;
    if (!ConvertFromJsValue(env, argv[0], enabled)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to convert parameter to enabled");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    session->SetBehindWindowFilterEnabled(enabled);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetFreezeImmediately(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_THREE) {
        TLOGE(WmsLogTag::WMS_PATTERN, "Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double scaleValue;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], scaleValue)) {
        TLOGE(WmsLogTag::WMS_PATTERN, "Failed to convert parameter to scaleValue");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    float scaleParam = GreatOrEqual(scaleValue, 0.0f) && LessOrEqual(scaleValue, 1.0f) ?
        static_cast<float>(scaleValue) : 0.0f;
    bool isFreeze = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], isFreeze)) {
        TLOGE(WmsLogTag::WMS_PATTERN, "Failed to convert parameter to isFreeze");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double blurValue;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], blurValue)) {
        TLOGE(WmsLogTag::WMS_PATTERN, "Failed to convert parameter to blurValue");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    float blurParam = static_cast<float>(blurValue);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    std::shared_ptr<Media::PixelMap> pixelPtr = session->SetFreezeImmediately(scaleParam, isFreeze, blurParam);
    if (isFreeze) {
        if (pixelPtr == nullptr) {
            TLOGE(WmsLogTag::WMS_PATTERN, "Failed to create pixelPtr");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                "System is abnormal"));
            return NapiGetUndefined(env);
        }
        napi_value pixelMapObj = Media::PixelMapNapi::CreatePixelMap(env, pixelPtr);
        if (pixelMapObj == nullptr) {
            TLOGE(WmsLogTag::WMS_PATTERN, "Failed to create pixel map object");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                "System is abnormal"));
            return NapiGetUndefined(env);
        }
        return pixelMapObj;
    }
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessSessionLockStateChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->RegisterSessionLockStateChangeCallback([weakThis = wptr(this), where = __func__](bool isLockedState) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSessionLockStateChange(isLockedState);
    });
    TLOGI(WmsLogTag::WMS_MAIN, "success");
}

void JsSceneSession::OnSessionLockStateChange(bool isLockedState)
{
    auto task = [weakThis = wptr(this), persistentId = persistentId_, isLockedState, env = env_, where = __func__] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: jsSceneSession id:%{public}d has been destroyed.",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(SESSION_LOCK_STATE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: jsCallBack is nullptr", where);
            return;
        }
        napi_value jsSessionLockState = CreateJsValue(env, isLockedState);
        napi_value argv[] = { jsSessionLockState };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

napi_value JsSceneSession::OnSendContainerModalEvent(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        TLOGE(WmsLogTag::WMS_EVENT, "Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string eventName;
    if (!ConvertFromJsValue(env, argv[0], eventName)) {
        TLOGE(WmsLogTag::WMS_EVENT, "Failed to convert parameter to string");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string eventValue;
    if (!ConvertFromJsValue(env, argv[1], eventValue) || eventValue.empty()) {
        TLOGE(WmsLogTag::WMS_EVENT, "Failed to convert parameter to string");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
 
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "session is nullptr, id:%{public}d", persistentId_);
        return NapiGetUndefined(env);
    }
    if (!session->IsPcWindow()) {
        TLOGE(WmsLogTag::WMS_EVENT, "device type not support");
        return NapiGetUndefined(env);
    }
    session->SendContainerModalEvent(eventName, eventValue);
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessUpdateSessionLabelAndIconRegister()
{
    TLOGD(WmsLogTag::WMS_MAIN, "in");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetUpdateSessionLabelAndIconListener([weakThis = wptr(this), where](const std::string& label,
        const std::shared_ptr<Media::PixelMap>& icon) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->UpdateSessionLabelAndIcon(label, icon);
    });
    TLOGD(WmsLogTag::WMS_MAIN, "success");
}

void JsSceneSession::UpdateSessionLabelAndIcon(const std::string& label, const std::shared_ptr<Media::PixelMap>& icon)
{
    TLOGI(WmsLogTag::WMS_MAIN, "in");
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), persistentId = persistentId_, label, icon, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(UPDATE_SESSION_LABEL_AND_ICON_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s jsCallBack is nullptr", where);
            return;
        }
        napi_value jsLabel = CreateJsValue(env, label);
        if (jsLabel == nullptr) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s label is nullptr", where);
            return;
        }
        napi_value jsIcon = Media::PixelMapNapi::CreatePixelMap(env, icon);
        if (jsIcon == nullptr) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s icon is nullptr", where);
            return;
        }
        napi_value argv[] = {jsLabel, jsIcon};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::ProcessKeyboardStateChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetKeyboardStateChangeListener(
        [weakThis = wptr(this), where = __func__](SessionState state, KeyboardViewMode mode) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnKeyboardStateChange(state, mode);
    });
    TLOGD(WmsLogTag::WMS_KEYBOARD, "success");
}

void JsSceneSession::OnKeyboardStateChange(SessionState state, KeyboardViewMode mode)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGW(WmsLogTag::WMS_KEYBOARD, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }

    TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, state: %{public}d, KeyboardViewMode: %{public}u",
        session->GetPersistentId(), state, static_cast<uint32_t>(mode));
    auto task = [weakThis = wptr(this), persistentId = persistentId_, state, env = env_, mode] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "OnKeyboardStateChange jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(KEYBOARD_STATE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "jsCallBack is nullptr");
            return;
        }
        napi_value jsKeyboardStateObj = CreateJsValue(env, state);
        napi_value jsKeyboardViewModeObj = CreateJsValue(env, mode);
        napi_value argv[] = { jsKeyboardStateObj, jsKeyboardViewModeObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnKeyboardStateChange, state:" +
        std::to_string(static_cast<uint32_t>(state)));
}

void JsSceneSession::ProcessKeyboardViewModeChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    session->SetKeyboardViewModeChangeListener(
        [weakThis = wptr(this)](const KeyboardViewMode& mode) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "ProcessKeyboardViewModeChangeRegister jsSceneSession is null");
            return;
        }
        jsSceneSession->OnKeyboardViewModeChange(mode);
    });
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Register success. id: %{public}d", persistentId_);
}

void JsSceneSession::OnKeyboardViewModeChange(KeyboardViewMode mode)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Change KeyboardViewMode to %{public}u", static_cast<uint32_t>(mode));
    auto task = [weakThis = wptr(this), persistentId = persistentId_, env = env_, mode] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "jsSceneSession id:%{public}d has been destroyed",
                persistentId);
            return;
        }
        auto jsCallBack = jsSceneSession->GetJSCallback(KEYBOARD_VIEW_MODE_CHANGE_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "jsCallBack is nullptr");
            return;
        }
        napi_value jsKeyboardViewMode = CreateJsValue(env, static_cast<uint32_t>(mode));
        napi_value argv[] = {jsKeyboardViewMode};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::ProcessSetWindowCornerRadiusRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "session is nullptr, id:%{public}d", persistentId_);
        return;
    }
    const char* const where = __func__;
    session->SetWindowCornerRadiusCallback([weakThis = wptr(this), where](float cornerRadius) {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s: jsSceneSession is null", where);
            return;
        }
        jsSceneSession->OnSetWindowCornerRadius(cornerRadius);
    });
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "success");
}

void JsSceneSession::OnSetWindowCornerRadius(float cornerRadius)
{
    const char* const where = __func__;
    taskScheduler_->PostMainThreadTask([weakThis = wptr(this), persistentId = persistentId_,
        cornerRadius, env = env_, where] {
        auto jsSceneSession = weakThis.promote();
        if (!jsSceneSession || jsSceneSessionMap_.find(persistentId) == jsSceneSessionMap_.end()) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s: jsSceneSession id:%{public}d has been destroyed",
                where, persistentId);
            return;
        }
        TLOGND(WmsLogTag::WMS_ATTRIBUTE, "%{public}s: cornerRadius is %{public}f", where, cornerRadius);
        auto jsCallBack = jsSceneSession->GetJSCallback(SET_WINDOW_CORNER_RADIUS_CB);
        if (!jsCallBack) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s: jsCallBack is nullptr", where);
            return;
        }
        napi_value jsCornerRadius = CreateJsValue(env, cornerRadius);
        napi_value argv[] = { jsCornerRadius };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    }, __func__);
}
} // namespace OHOS::Rosen
