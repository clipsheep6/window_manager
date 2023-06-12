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
#include "intention_event_manager.h"

#include <map>

#include "frameworks/core/event/ace_events.h"
#include "window_manager_hilog.h"
// #include "vsync_station.h"
#include "session_manager/include/scene_session_manager.h"

namespace OHOS {
namespace Rosen {
#define SCENE_SESSION_MANAGER SceneSessionManager::GetInstance()

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventManager" };
constexpr double epsilon = -0.001f;
constexpr float MOUSE_RECT_HOT = 4;
constexpr float TOUCH_RECT_HOT = 20;
} // namespace

IntentionEventManager::IntentionEventManager() {}
IntentionEventManager::~IntentionEventManager() {}

int32_t IntentionEventManager::EnableInputEventListener(Ace::UIContent* uiContent)
{
    if (uiContent == nullptr) {
        WLOGFE("uiContent is null");
        return -1;
    }
    auto listener = std::make_shared<IntentionEventManager::InputEventListener>(uiContent);
    auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainEventRunner) {
        WLOGFD("MainEventRunner is available");
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
    } else {
        // WLOGFD("MainEventRunner is not available");
        // eventHandler_ = AppExecFwk::EventHandler::Current();
        // if (!eventHandler_) {
        //     eventHandler_ =
        //         std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::Create(INPUT_AND_VSYNC_THREAD));
        // }
        // VsyncStation::GetInstance().SetIsMainHandlerAvailable(false);
        // VsyncStation::GetInstance().SetVsyncEventHandler(eventHandler_);
    }
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener, eventHandler_);
    return ERR_OK;
}

bool IntentionEventManager::IsInWindowSceneRegin(const Ace::NG::RectF& windowSceneRect,
    const Ace::NG::PointF& parentLocalPoint, int32_t souceType)
{
    float hotOffset = (souceType == static_cast<int32_t>(Ace::SourceType::MOUSE))
        ? MOUSE_RECT_HOT : TOUCH_RECT_HOT;

    float x = windowSceneRect.GetX();
    float y = windowSceneRect.GetY();
    float width = windowSceneRect.Width();
    float height = windowSceneRect.Height();
    float hotX = 0;
    float hotWidth = 0;
    if (x > hotOffset || x == hotOffset) {
        hotX = x - hotOffset;
        hotWidth = width + hotOffset * 2;
    } else {
        hotX = x - hotOffset;
        hotWidth = width + hotOffset + x;
    }

    float hotY = 0;
    float hotHeight = 0;
    if (y > hotOffset || y == hotOffset) {
        hotY = y - hotOffset;
        hotHeight = height + hotOffset * 2;
    } else {
        hotY = y - hotOffset;
        hotHeight = width + hotOffset + y;
    }
    return GreatOrEqual(parentLocalPoint.GetX(), hotX) &&
        LessOrEqual(parentLocalPoint.GetX(), hotX + hotWidth) &&
        GreatOrEqual(parentLocalPoint.GetY(), hotY) &&
        LessOrEqual(parentLocalPoint.GetY(), hotY + hotHeight);
}

bool IntentionEventManager::LessOrEqual(double left, double right)
{
    constexpr double epsilon = 0.001f;
    return (left - right) < epsilon;
}

bool IntentionEventManager::GreatOrEqual(double left, double right)
{
    return (left - right) > epsilon;
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        return;
    }
    uiContent_->ProcessPointerEvent(pointerEvent);
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    uint64_t focusedSessionId = SceneSessionManager::GetInstance().GetFocusedSession();
    if (focusedSessionId == INVALID_SESSION_ID) {
        WLOGFE("focusedSessionId is invalid");
        return;
    }
    auto focusedSceneSession = SceneSessionManager::GetInstance().GetSceneSession(focusedSessionId);
    if (focusedSceneSession == nullptr) {
        WLOGFE("focusedSceneSession is null");
        return;
    }
    focusedSceneSession->TransferKeyEvent(keyEvent);
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::AxisEvent> axisEvent) const
{
}
}
}