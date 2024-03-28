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

#include "scene_session_dirty_manager.h"

#include <cinttypes>
#include <memory>
#include <sstream>

#include "input_manager.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_helper.h"
#include "wm_common_inner.h"

namespace OHOS::Rosen {
namespace {
constexpr float DIRECTION0 = 0 ;
constexpr float DIRECTION90 = 90 ;
constexpr float DIRECTION180 = 180 ;
constexpr float DIRECTION270 = 270 ;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionDirtyManager"};
constexpr unsigned int POINTER_CHANGE_AREA_COUNT = 8;
constexpr int POINTER_CHANGE_AREA_SEXTEEN = 16;
constexpr int POINTER_CHANGE_AREA_DEFAULT = 0;
constexpr int POINTER_CHANGE_AREA_FIVE = 5;
constexpr int UPDATE_TASK_DURATION = 10;
const std::string UPDATE_WINDOW_INFO_TASK = "UpdateWindowInfoTask";
} //namespace

static bool operator==(const MMI::Rect left, const MMI::Rect right)
{
    return ((left.x == right.x) && (left.y == right.y) && (left.width == right.width) && (left.height == right.height));
}

static MMI::Direction ConvertDegreeToMMIRotation(float degree, MMI::DisplayMode displayMode)
{
    MMI::Direction rotation = MMI::DIRECTION0;
    if (NearEqual(degree, DIRECTION0)) {
        rotation = MMI::DIRECTION0;
    }
    if (NearEqual(degree, DIRECTION90)) {
        rotation = MMI::DIRECTION90;
    }
    if (NearEqual(degree, DIRECTION180)) {
        rotation = MMI::DIRECTION180;
    }
    if (NearEqual(degree, DIRECTION270)) {
        rotation = MMI::DIRECTION270;
    }
    if (displayMode == MMI::DisplayMode::FULL) {
        switch (rotation) {
            case MMI::DIRECTION0:
                rotation = MMI::DIRECTION90;
                break;
            case MMI::DIRECTION90:
                rotation = MMI::DIRECTION180;
                break;
            case MMI::DIRECTION180:
                rotation = MMI::DIRECTION270;
                break;
            case MMI::DIRECTION270:
                rotation = MMI::DIRECTION0;
                break;
            default:
                rotation = MMI::DIRECTION0;
                break;
        }
    }
    return rotation;
}

void SceneSessionDirtyManager::CalNotRotateTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform) const
{
    if (sceneSession == nullptr || sceneSession->GetSessionProperty() == nullptr) {
        WLOGFE("SceneSession or SessionProperty is nullptr");
        return;
    }
    auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
    auto displayMode = Rosen::ScreenSessionManagerClient::GetInstance().GetFoldDisplayMode();
    std::map<ScreenId, ScreenProperty> screensProperties =
        Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    if (screensProperties.find(displayId) == screensProperties.end()) {
        return;
    }
    auto screenProperty = screensProperties[displayId];
    auto screenSession = Rosen::ScreenSessionManagerClient::GetInstance().GetScreenSessionById(displayId);
    MMI::Direction displayRotation = ConvertDegreeToMMIRotation(screenProperty.GetRotation(),
        static_cast<MMI::DisplayMode>(displayMode));
    float width = screenProperty.GetBounds().rect_.GetWidth();
    float height = screenProperty.GetBounds().rect_.GetHeight();
    Vector2f scale(sceneSession->GetScaleX(), sceneSession->GetScaleY());
    Vector2f offset(sceneSession->GetSessionRect().posX_, sceneSession->GetSessionRect().posY_);
    Vector2f translate = offset;
    float rotate = 0.0f;
    switch (displayRotation) {
        case MMI::DIRECTION0: {
            break;
        }
        case MMI::DIRECTION270: {
            translate.x_ = offset.y_;
            translate.y_ = height - offset.x_;
            rotate = -M_PI_2;
            break;
        }
        case MMI::DIRECTION180:
            translate.x_ = width - offset.x_;
            translate.y_ = height - offset.y_;
            rotate = M_PI;
            break;
        case MMI::DIRECTION90: {
            translate.x_ = width - offset.y_;
            translate.y_ = offset.x_;
            rotate = M_PI_2;
            break;
        }
        default:
            break;
    }
    tranform = tranform.Translate(translate).Rotate(rotate).Scale(scale, sceneSession->GetPivotX(),
        sceneSession->GetPivotY());
    tranform = tranform.Inverse();
}

void SceneSessionDirtyManager::CalTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    tranform = Matrix3f::IDENTITY;
    bool isRotate = sceneSession->GetSessionInfo().isRotable_;
    auto displayMode = Rosen::ScreenSessionManagerClient::GetInstance().GetFoldDisplayMode();
    if (isRotate || !sceneSession->GetSessionInfo().isSystem_ ||
        static_cast<MMI::DisplayMode>(displayMode) == MMI::DisplayMode::FULL) {
        Vector2f scale(sceneSession->GetScaleX(), sceneSession->GetScaleY());
        WSRect windowRect = sceneSession->GetSessionRect();
        Vector2f translate(windowRect.posX_, windowRect.posY_);
        tranform = tranform.Translate(translate);
        tranform = tranform.Scale(scale, sceneSession->GetPivotX(), sceneSession->GetPivotY());
        tranform = tranform.Inverse();
        return;
    }
    CalNotRotateTramform(sceneSession, tranform);
}


void SceneSessionDirtyManager::UpdateDefaultHotAreas(sptr<SceneSession> sceneSession,
    std::vector<MMI::Rect>& touchHotAreas,
    std::vector<MMI::Rect>& pointerHotAreas) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    WSRect windowRect = sceneSession->GetSessionRect();
    uint32_t touchOffset = 0;
    uint32_t pointerOffset = 0;
    if ((sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) ||
        (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_PIP)) {
        float vpr = 1.5f; // 1.5: default vp
        if (sceneSession->GetSessionProperty() != nullptr) {
            auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
            auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(displayId);
            if (screenSession != nullptr) {
                vpr = screenSession->GetScreenProperty().GetDensity();
            }
        }
        WLOGFD("[WMSEvent] UpdateDefaultHotAreas, vpr: %{public}f", vpr);
        touchOffset = static_cast<uint32_t>(HOTZONE_TOUCH * vpr);
        pointerOffset = static_cast<uint32_t>(HOTZONE_POINTER * vpr);
    }

    MMI::Rect touchRect = {
        .x = -touchOffset,
        .y = -touchOffset,
        .width = windowRect.width_ + touchOffset * 2,  // 2 : double touchOffset
        .height = windowRect.height_ + touchOffset * 2 // 2 : double touchOffset
    };

    MMI::Rect pointerRect = {
        .x = -pointerOffset,
        .y = -pointerOffset,
        .width = windowRect.width_ + pointerOffset * 2,  // 2 : double pointerOffset
        .height = windowRect.height_ + pointerOffset * 2 // 2 : double pointerOffset
    };

    touchHotAreas.emplace_back(touchRect);
    pointerHotAreas.emplace_back(pointerRect);
}

void SceneSessionDirtyManager::UpdateHotAreas(sptr<SceneSession> sceneSession, std::vector<MMI::Rect>& touchHotAreas,
    std::vector<MMI::Rect>& pointerHotAreas) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }

    WSRect windowRect = sceneSession->GetSessionRect();
    const std::vector<Rect>& hotAreas = sceneSession->GetTouchHotAreas();
    for (auto area : hotAreas) {
        MMI::Rect rect;
        rect.x = area.posX_;
        rect.y = area.posY_;
        rect.width = area.width_;
        rect.height = area.height_;
        auto iter = std::find_if(touchHotAreas.begin(), touchHotAreas.end(),
            [&rect](const MMI::Rect& var) { return rect == var; });
        if (iter != touchHotAreas.end()) {
            continue;
        }
        touchHotAreas.emplace_back(rect);
        pointerHotAreas.emplace_back(rect);
        if (touchHotAreas.size() == static_cast<int>(MMI::WindowInfo::MAX_HOTAREA_COUNT)) {
            auto sessionid = sceneSession->GetWindowId();
            WLOGFE("id = %{public}d hotAreas size > %{public}d", sessionid, static_cast<int>(hotAreas.size()));
            break;
        }
    }

    if (touchHotAreas.empty()) {
        return UpdateDefaultHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    }
}

std::map<int32_t, sptr<SceneSession>> SceneSessionDirtyManager::GetDialogSessionMap(
    const std::map<int32_t, sptr<SceneSession>>& sessionMap) const
{
    std::map<int32_t, sptr<SceneSession>> dialogMap;

    for (const auto& elem: sessionMap) {
        const auto& session = elem.second;
        if (session == nullptr) {
            continue;
        }
        bool isModalSubWindow = false;
        auto property = session->GetSessionProperty();
        if (property != nullptr) {
            bool isSubWindow = WindowHelper::IsSubWindow(property->GetWindowType());
            bool isModal = property->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL);
            isModalSubWindow = isSubWindow && isModal;
        }
        if (isModalSubWindow || session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            const auto& parentSession = session->GetParentSession();
            if (parentSession == nullptr) {
                continue;
            }
            dialogMap[parentSession->GetPersistentId()] = session;
            WLOGFI("Add dialog session, id: %{public}d, parentId: %{public}d",
                session->GetPersistentId(), parentSession->GetPersistentId());
        }
    }
    return dialogMap;
}

bool SceneSessionDirtyManager::IsFilterSession(const sptr<SceneSession>& sceneSession) const
{
    if (sceneSession == nullptr) {
        return true;
    }

    if (sceneSession->IsSystemInput()) {
        return false;
    } else if (sceneSession->IsSystemSession() && sceneSession->IsVisible() && sceneSession->IsSystemActive()) {
        return false;
    }
    if (!Rosen::SceneSessionManager::GetInstance().IsSessionVisible(sceneSession)) {
        return true;
    }
    return false;
}

void SceneSessionDirtyManager::NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession,
    const WindowUpdateType& type, const bool startMoving)
{
    if (sceneSession == nullptr) {
        WLOGFW("sceneSession is null");
        return;
    }

    if (type == WindowUpdateType::WINDOW_UPDATE_ADDED || type == WindowUpdateType::WINDOW_UPDATE_REMOVED||
        type == WindowUpdateType::WINDOW_UPDATE_ACTIVE) {
            WLOGFI("[EventDispatch] NotifyWindowInfoChange wid = %{public}d, WindowUpdateType = %{public}d",
                sceneSession->GetWindowId(), static_cast<int>(type));
    }
    sessionDirty_.store(true);
    if (!hasPostTask_.load()) {
        hasPostTask_.store(true);
        auto task = [this]() {
            hasPostTask_.store(false);
            if (!sessionDirty_.load() || flushWindowInfoCallback_ == nullptr) {
                return;
            }
            flushWindowInfoCallback_();
        };
        SceneSessionManager::GetInstance().PostFlushWindowInfoTask(task,
            UPDATE_WINDOW_INFO_TASK, UPDATE_TASK_DURATION);
    }
}

std::vector<MMI::WindowInfo> SceneSessionDirtyManager::GetFullWindowInfoList()
{
    std::vector<MMI::WindowInfo> windowInfoList;
    const auto sceneSessionMap = Rosen::SceneSessionManager::GetInstance().GetSceneSessionMap();
    // all input event should trans to dialog window if dialog exists
    const auto dialogMap = GetDialogSessionMap(sceneSessionMap);
    std::string windowIDLstLog;
    for (const auto& sceneSessionValuePair : sceneSessionMap) {
        const auto& sceneSessionValue = sceneSessionValuePair.second;
        if (sceneSessionValue == nullptr) {
            continue;
        }
        WLOGFD("[EventDispatch] FullSceneSessionInfoUpdate windowName = %{public}s bundleName = %{public}s"
            " windowId = %{public}d activeStatus = %{public}d", sceneSessionValue->GetWindowName().c_str(),
            sceneSessionValue->GetSessionInfo().bundleName_.c_str(), sceneSessionValue->GetWindowId(),
            sceneSessionValue->GetForegroundInteractiveStatus());
        auto windowInfo = GetWindowInfo(sceneSessionValue, WindowAction::WINDOW_ADD);
        auto iter = (sceneSessionValue->GetParentPersistentId() == INVALID_SESSION_ID) ?
            dialogMap.find(sceneSessionValue->GetPersistentId()) :
            dialogMap.find(sceneSessionValue->GetParentPersistentId());
        if (iter != dialogMap.end() && iter->second != nullptr) {
            windowInfo.agentWindowId = static_cast<int32_t>(iter->second->GetPersistentId());
            windowInfo.pid = static_cast<int32_t>(iter->second->GetCallingPid());
            WLOGFI("Change agentId, dialogId: %{public}d, parentId: %{public}d",
                iter->second->GetPersistentId(), sceneSessionValue->GetPersistentId());
        }
        windowInfoList.emplace_back(windowInfo);
        windowIDLstLog.append(std::to_string(windowInfo.id).append(", "));
    }
    TLOGI(WmsLogTag::WMS_EVENT, "windowIDList: size:%{public}d %{public}s ",
        static_cast<int>(windowInfoList.size()), windowIDLstLog.c_str());
    return windowInfoList;
}

void SceneSessionDirtyManager::UpdatePointerAreas(sptr<SceneSession> sceneSession,
    std::vector<int32_t>& pointerChangeAreas) const
{
    bool dragEnabled = sceneSession->GetSessionProperty()->GetDragEnabled();
    if (dragEnabled) {
        float vpr = 1.5f; // 1.5: default vp
        if (sceneSession->GetSessionProperty() != nullptr) {
            auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
            auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(displayId);
            if (screenSession != nullptr) {
                vpr = screenSession->GetScreenProperty().GetDensity();
            }
        }
        int32_t pointerArea_Five_PX = static_cast<int32_t>(POINTER_CHANGE_AREA_FIVE * vpr);
        int32_t pointerArea_Sexteen_PX = static_cast<int32_t>(POINTER_CHANGE_AREA_SEXTEEN * vpr);

        if (sceneSession->GetSessionInfo().isSetPointerAreas_) {
            pointerChangeAreas = {POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT,
                POINTER_CHANGE_AREA_DEFAULT, pointerArea_Five_PX, pointerArea_Sexteen_PX,
                pointerArea_Five_PX, pointerArea_Sexteen_PX, pointerArea_Five_PX};
            return;
        }
        auto limits = sceneSession->GetSessionProperty()->GetWindowLimits();
        if (limits.minWidth_ == limits.maxWidth_ && limits.minHeight_ != limits.maxHeight_) {
            pointerChangeAreas = {POINTER_CHANGE_AREA_DEFAULT, pointerArea_Five_PX,
                POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT,
                pointerArea_Five_PX, POINTER_CHANGE_AREA_DEFAULT,  POINTER_CHANGE_AREA_DEFAULT};
        } else if (limits.minWidth_ != limits.maxWidth_ && limits.minHeight_ == limits.maxHeight_) {
            pointerChangeAreas = {POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT,
                POINTER_CHANGE_AREA_DEFAULT, pointerArea_Five_PX, POINTER_CHANGE_AREA_DEFAULT,
                POINTER_CHANGE_AREA_DEFAULT, POINTER_CHANGE_AREA_DEFAULT, pointerArea_Five_PX};
        } else if (limits.minWidth_ != limits.maxWidth_ && limits.minHeight_ != limits.maxHeight_) {
            pointerChangeAreas = {pointerArea_Sexteen_PX, pointerArea_Five_PX,
                pointerArea_Sexteen_PX, pointerArea_Five_PX, pointerArea_Sexteen_PX,
                pointerArea_Five_PX, pointerArea_Sexteen_PX, pointerArea_Five_PX};
        }
    } else {
        WLOGFD("UpdatePointerAreas sceneSession is: %{public}d dragEnabled is false", sceneSession->GetPersistentId());
    }
}

MMI::WindowInfo SceneSessionDirtyManager::GetWindowInfo(const sptr<SceneSession>& sceneSession,
    const SceneSessionDirtyManager::WindowAction& action) const
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return {};
    }

    if (sceneSession->GetSessionProperty() == nullptr) {
        WLOGFE("SceneSession` property is nullptr");
        return {};
    }
    
    Matrix3f tranform;
    WSRect windowRect = sceneSession->GetSessionRect();
    auto pid = sceneSession->GetCallingPid();
    auto uid = sceneSession->GetCallingUid();
    auto windowId = sceneSession->GetWindowId();
    auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
    CalTramform(sceneSession, tranform);
    const unsigned int len = 9;
    std::vector<float> transformData(tranform.GetData(), tranform.GetData() + len);

    auto agentWindowId = sceneSession->GetWindowId();
    auto zOrder = sceneSession->GetZOrder();
    std::vector<int32_t> pointerChangeAreas(POINTER_CHANGE_AREA_COUNT, 0);
    auto windowMode = sceneSession->GetSessionProperty()->GetWindowMode();
    auto maxMode = sceneSession->GetSessionProperty()->GetMaximizeMode();
    if ((windowMode == Rosen::WindowMode::WINDOW_MODE_FLOATING &&
        Rosen::WindowHelper::IsMainWindow(sceneSession->GetSessionProperty()->GetWindowType()) &&
        maxMode != Rosen::MaximizeMode::MODE_AVOID_SYSTEM_BAR) || (sceneSession->GetSessionInfo().isSetPointerAreas_)) {
            UpdatePointerAreas(sceneSession, pointerChangeAreas);
    }
    std::vector<MMI::Rect> touchHotAreas;
    std::vector<MMI::Rect> pointerHotAreas;
    UpdateHotAreas(sceneSession, touchHotAreas, pointerHotAreas);
    auto windowMask = sceneSession->GetSessionProperty()->GetWindowMask().GetRefPtr();
    MMI::WindowInfo windowInfo = {
        .id = windowId,
        .pid = sceneSession->IsStartMoving() ? static_cast<int32_t>(getpid()) : pid,
        .uid = uid,
        .area = { windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_ },
        .defaultHotAreas = touchHotAreas,
        .pointerHotAreas = pointerHotAreas,
        .agentWindowId = agentWindowId,
        .flags = (!sceneSession->GetSystemTouchable() || !sceneSession->GetForegroundInteractiveStatus()),
        .displayId = displayId,
        .action = static_cast<MMI::WINDOW_UPDATE_ACTION>(action),
        .pointerChangeAreas = pointerChangeAreas,
        .zOrder = zOrder,
        .transform = transformData,
        .windowMask = windowMask
    };
    return windowInfo;
}

void SceneSessionDirtyManager::RegisterFlushWindowInfoCallback(const FlushWindowInfoCallback &&callback)
{
    flushWindowInfoCallback_ = std::move(callback);
}

void SceneSessionDirtyManager::ResetSessionDirty()
{
    sessionDirty_.store(false);
}
} //namespace OHOS::Rosen
