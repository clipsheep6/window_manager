/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "remote_animation.h"

#include <ability_manager_client.h>
#include <common/rs_rect.h>
#include <hitrace_meter.h>
#include "minimize_app.h"
#include "parameters.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "RemoteAnimation"};
}
bool RemoteAnimation::isRemoteAnimationEnable_ = true;

sptr<RSIWindowAnimationController> RemoteAnimation::windowAnimationController_ = nullptr;
std::weak_ptr<AppExecFwk::EventHandler> RemoteAnimation::wmsTaskHandler_;
wptr<WindowRoot> RemoteAnimation::windowRoot_;
bool RemoteAnimation::animationFirst_ = false;

std::map<TransitionReason, TransitionEvent> eventMap_ = {
    {TransitionReason::CLOSE, TransitionEvent::CLOSE},
    {TransitionReason::MINIMIZE, TransitionEvent::MINIMIZE},
    {TransitionReason::BACK, TransitionEvent::BACK}
};

void RemoteAnimation::SetAnimationFirst(bool animationFirst)
{
    animationFirst_ = animationFirst;
}

WMError RemoteAnimation::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller,
    const sptr<WindowRoot>& windowRoot)
{
    WLOGFI("RSWindowAnimation: set window animation controller!");
    if (!isRemoteAnimationEnable_) {
        WLOGE("RSWindowAnimation: failed to set window animation controller, remote animation is not enabled");
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (windowAnimationController_ != nullptr) {
        WLOGFI("RSWindowAnimation: maybe user switch!");
    }

    windowAnimationController_ = controller;
    windowRoot_ = windowRoot;
    return WMError::WM_OK;
}

void RemoteAnimation::SetMainTaskHandler(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    wmsTaskHandler_ = handler;
}

bool RemoteAnimation::CheckAnimationController()
{
    if (windowAnimationController_ == nullptr) {
        WLOGFI("RSWindowAnimation: windowAnimationController_ null!");
        return false;
    }
    return true;
}

bool RemoteAnimation::CheckTransition(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& dstNode)
{
    if (srcNode != nullptr && !srcNode->leashWinSurfaceNode_ && !srcNode->surfaceNode_) {
        WLOGFI("RSWindowAnimation: srcNode has no surface!");
        return false;
    }

    if (dstNode != nullptr && !dstNode->leashWinSurfaceNode_ && !dstNode->surfaceNode_) {
        WLOGFI("RSWindowAnimation: dstNode has no surface!");
        return false;
    }

    return CheckAnimationController();
}

void RemoteAnimation::OnRemoteDie(const sptr<IRemoteObject>& remoteObject)
{
    WLOGFI("RSWindowAnimation: OnRemoteDie!");
    if (windowAnimationController_ != nullptr && windowAnimationController_->AsObject() == remoteObject) {
        windowAnimationController_ = nullptr;
    }
}

TransitionEvent RemoteAnimation::GetTransitionEvent(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode)
{
    auto transitionReason = srcInfo->GetTransitionReason(); // src reason same as dst reason
    if (srcNode != nullptr && eventMap_.find(transitionReason) != eventMap_.end()) {
        WLOGFI("current window:%{public}u state: %{public}u", srcNode->GetWindowId(),
            static_cast<uint32_t>(srcNode->stateMachine_.GetCurrentState()));
        if (srcNode->stateMachine_.IsWindowNodeHiddenOrHiding()) {
            WLOGFE("srcNode is hiding or hidden id: %{public}d!", srcNode->GetWindowId());
            return TransitionEvent::UNKNOWN;
        }
        return eventMap_[transitionReason];
    }
    WLOGFI("Ability Transition");
    if (dstNode == nullptr) {
        if (dstInfo->GetAbilityToken() == nullptr) {
            WLOGFE("target window abilityToken is null");
        }
        return TransitionEvent::UNKNOWN;
    } else {
        WLOGFI("current window:%{public}u state: %{public}u", dstNode->GetWindowId(),
            static_cast<uint32_t>(dstNode->stateMachine_.GetCurrentState()));
        if (WindowHelper::IsMainWindow(dstInfo->GetWindowType())) {
            WLOGFE("chy ----%{public}s, %{public}d", __FUNCTION__, __LINE__);
            if (dstNode->stateMachine_.IsWindowNodeShownOrShowing()) {
                WLOGFE("dstNode is showing or shown id: %{public}d!", dstNode->GetWindowId());
                return TransitionEvent::UNKNOWN;
            }
            return TransitionEvent::APP_TRANSITION;
        } else if (dstInfo->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
            return TransitionEvent::HOME;
        }
    }
    return TransitionEvent::UNKNOWN;
}

WMError RemoteAnimation::NotifyAnimationStartApp(sptr<WindowTransitionInfo> srcInfo,
    const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode,
    sptr<RSWindowAnimationTarget>& dstTarget, sptr<RSWindowAnimationFinishedCallback>& finishedCallback)
{
    // start app from launcher
    if (srcNode != nullptr && srcNode->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
        WLOGFI("RSWindowAnimation: start app id:%{public}u from launcher!", dstNode->GetWindowId());
        windowAnimationController_->OnStartApp(StartingAppType::FROM_LAUNCHER, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }
    // start app from recent
    if (srcInfo != nullptr && srcInfo->GetIsRecent()) {
        WLOGFI("RSWindowAnimation: start app id:%{public}u from recent!", dstNode->GetWindowId());
        windowAnimationController_->OnStartApp(StartingAppType::FROM_RECENT, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }
    // start app from other
    WLOGFI("RSWindowAnimation: start app id:%{public}u from other!", dstNode->GetWindowId());
    windowAnimationController_->OnStartApp(StartingAppType::FROM_OTHER, dstTarget, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode,
    const sptr<WindowNode>& dstNode)
{
    if (!dstNode) {
        WLOGFI("id:%{public}d startingWindowShown_:%{public}d", dstNode->GetWindowId(), dstNode->startingWindowShown_);
    }
    if (!dstNode || !dstNode->startingWindowShown_) {
        WLOGFE("RSWindowAnimation: no startingWindow for dst window id:%{public}u!", dstNode->GetWindowId());
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    auto finishedCallback = CreateShowAnimationFinishedCallback(srcNode, dstNode);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    if (animationFirst_) {
        // Notify minimize before animation when animationFirst is enable.
        // Or notify minimize in animation finished callback.
        MinimizeApp::ExecuteMinimizeAll();
    }
    auto dstTarget = CreateWindowAnimationTarget(dstInfo, dstNode);
    if (dstTarget == nullptr) {
        WLOGFE("RSWindowAnimation: Failed to create dst target!");
        finishedCallback->OnAnimationFinished();
        return WMError::WM_ERROR_NO_MEM;
    }
    dstNode->isPlayAnimationShow_ = true;
    // Transition to next state and update task count will success when enable animationFirst_
    dstNode->stateMachine_.TransitionTo(WindowNodeState::SHOW_ANIMATION_PLAYING);
    dstNode->stateMachine_.UpdateAnimationTaskCount(true);
    bool needMinimizeSrcNode = MinimizeApp::IsNodeNeedMinimize(srcNode);
    // from app to app
    if (needMinimizeSrcNode && srcNode != nullptr) {
        auto srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
        // to avoid normal animation
        srcNode->isPlayAnimationHide_ = true;
        srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
        srcNode->stateMachine_.UpdateAnimationTaskCount(true);
        // update snapshot before animation
        AAFwk::AbilityManagerClient::GetInstance()->UpdateMissionSnapShot(srcNode->abilityToken_);
        auto winRoot = windowRoot_.promote();
        if (winRoot != nullptr) {
            winRoot->RemoveWindowNode(srcNode->GetWindowId(), true);
        }
        WLOGFI("RSWindowAnimation: app transition from id:%{public}u to id:%{public}u!",
            srcNode->GetWindowId(), dstNode->GetWindowId());
        windowAnimationController_->OnAppTransition(srcTarget, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }
    return NotifyAnimationStartApp(srcInfo, srcNode, dstNode, dstTarget, finishedCallback);
}

WMError RemoteAnimation::NotifyAnimationMinimize(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
    }
    WLOGFI("RSWindowAnimation: notify animation minimize Id:%{public}u!", srcNode->GetWindowId());
    srcNode->isPlayAnimationHide_ = true;
    srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    // update snapshot before animation
    AAFwk::AbilityManagerClient::GetInstance()->UpdateMissionSnapShot(srcNode->abilityToken_);
    auto winRoot = windowRoot_.promote();
    if (winRoot != nullptr) {
        winRoot->RemoveWindowNode(srcNode->GetWindowId(), true);
    }
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = CreateHideAnimationFinishedCallback(
        srcNode, TransitionEvent::MINIMIZE);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    srcNode->stateMachine_.UpdateAnimationTaskCount(true);
    windowAnimationController_->OnMinimizeWindow(srcTarget, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationClose(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
    TransitionEvent event)
{
    sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
    }
    WLOGFI("RSWindowAnimation: notify animation close id:%{public}u!", srcNode->GetWindowId());
    srcNode->isPlayAnimationHide_ = true;
    srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    // update snapshot before animation
    AAFwk::AbilityManagerClient::GetInstance()->UpdateMissionSnapShot(srcNode->abilityToken_);
    auto winRoot = windowRoot_.promote();
    if (winRoot != nullptr) {
        winRoot->RemoveWindowNode(srcNode->GetWindowId(), true);
    }
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = CreateHideAnimationFinishedCallback(srcNode, event);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    srcNode->stateMachine_.UpdateAnimationTaskCount(true);
    windowAnimationController_->OnCloseWindow(srcTarget, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationByHome()
{
    if (!CheckAnimationController()) {
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    auto needMinimizeAppNodes = MinimizeApp::GetNeedMinimizeAppNodes();
    WLOGFI("RSWindowAnimation: notify animation by home, need minimize size: %{public}u",
        static_cast<uint32_t>(needMinimizeAppNodes.size()));
    std::vector<sptr<RSWindowAnimationTarget>> animationTargets;
    for (auto& weakNode : needMinimizeAppNodes) {
        auto srcNode = weakNode.promote();
        sptr<WindowTransitionInfo> srcInfo = new(std::nothrow) WindowTransitionInfo();
        sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
        if (srcTarget == nullptr) {
            continue;
        }
        srcNode->isPlayAnimationHide_ = true;
        // update snapshot before animation
        srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
        srcNode->stateMachine_.UpdateAnimationTaskCount(true);
        AAFwk::AbilityManagerClient::GetInstance()->UpdateMissionSnapShot(srcNode->abilityToken_);
        auto winRoot = windowRoot_.promote();
        if (winRoot != nullptr) {
            winRoot->RemoveWindowNode(srcNode->GetWindowId(), true);
        }
        animationTargets.emplace_back(srcTarget);
    }
    std::function<void(void)> func;
    if (animationFirst_) {
        MinimizeApp::ExecuteMinimizeAll();
        func = [needMinimizeAppNodes]() {
            WLOGFI("NotifyAnimationByHome in animation callback in animationFirst");
            for (auto& weakNode : needMinimizeAppNodes) {
                auto srcNode = weakNode.promote();
                ProcessNodeStateTask(srcNode);
            }
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
        };
    } else {
        func = []() {
            WLOGFI("NotifyAnimationByHome in animation callback not animationFirst");
            MinimizeApp::ExecuteMinimizeAll();
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
        };
    }

    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        func);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    // need use OnMinimizeWindows with controller
    windowAnimationController_->OnMinimizeAllWindow(animationTargets, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationScreenUnlock(std::function<void(void)> callback)
{
    if (!CheckAnimationController()) {
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    WLOGFI("NotifyAnimationScreenUnlock");
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        callback);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    windowAnimationController_->OnScreenUnlock(finishedCallback);
    return WMError::WM_OK;
}

sptr<RSWindowAnimationTarget> RemoteAnimation::CreateWindowAnimationTarget(sptr<WindowTransitionInfo> info,
    const sptr<WindowNode>& windowNode)
{
    if (info == nullptr || windowNode == nullptr) {
        WLOGFW("Failed to create window animation target, info or window node is null!");
        return nullptr;
    }

    sptr<RSWindowAnimationTarget> windowAnimationTarget = new(std::nothrow) RSWindowAnimationTarget();
    if (windowAnimationTarget == nullptr) {
        WLOGFE("New RSWindowAnimationTarget failed");
        return nullptr;
    }
    windowAnimationTarget->bundleName_ = info->GetBundleName();
    windowAnimationTarget->abilityName_ = info->GetAbilityName();
    windowAnimationTarget->windowId_ = windowNode->GetWindowId();
    windowAnimationTarget->displayId_ = windowNode->GetDisplayId();
    if (WindowHelper::IsAppWindow(windowNode->GetWindowType())) {
        windowAnimationTarget->surfaceNode_ = windowNode->leashWinSurfaceNode_;
    } else {
        windowAnimationTarget->surfaceNode_ = windowNode->surfaceNode_;
    }

    if (windowAnimationTarget->surfaceNode_ == nullptr) {
        WLOGFE("Window surface node is null!");
        return nullptr;
    }

    auto& stagingProperties = windowAnimationTarget->surfaceNode_->GetStagingProperties();
    auto rect = windowNode->GetWindowRect();
    // 0, 1, 2, 3: convert bounds to RectF
    auto boundsRect = RectF(rect.posX_, rect.posY_, rect.width_, rect.height_);
    windowAnimationTarget->windowBounds_ = RRect(boundsRect, stagingProperties.GetCornerRadius());
    return windowAnimationTarget;
}

void RemoteAnimation::ProcessNodeStateTask(const sptr<WindowNode>& node)
{
    // when callback come, node maybe destroyed
    if (node == nullptr) {
        WLOGFI("node is nullptr!");
        return;
    }
    WLOGFI("ProcessNodeStateTask windowId: %{public}u, name:%{public}s state: %{public}u",
        node->GetWindowId(), node->GetWindowName().c_str(), static_cast<uint32_t>(node->stateMachine_.GetCurrentState()));
    node->stateMachine_.UpdateAnimationTaskCount(false);
    int taskCount = node->stateMachine_.GetAnimationCount();
    if (taskCount > 0) {
        WLOGFI("not last state task of window: %{public}d, %{public}d callback left not be executed!",
            node->GetWindowId(), taskCount);
        return;
    }
    StateTask task;
    if (!node->stateMachine_.GetFinalStateTask(task)) {
        if (node->stateMachine_.IsHideAnimationPlaying()) {
            // when no task, need remove from rs Tree
            wptr<WindowNode> weakNode = node;
            task = [weakRoot = windowRoot_, weakNode]() {
                auto winRoot = weakRoot.promote();
                auto winNode = weakNode.promote();
                if (winRoot == nullptr || winNode == nullptr) {
                    WLOGFE("windowRoot or winNode is nullptr");
                    return;
                }
                WLOGFI("no task valid and but after hide anmation playing execute task removing from rs tree!");
                winRoot->UpdateRsTree(winNode->GetWindowId(), false);
            };
        }
    } else {
        WLOGFD("GetFinalTask Success");
    }
    auto handler = wmsTaskHandler_.lock();
    if (handler != nullptr) {
        bool ret = handler->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostTask Failed");
            task();
        }
    }
    if (node->stateMachine_.IsShowAnimationPlaying()) {
        node->stateMachine_.TransitionTo(WindowNodeState::SHOW_ANIMATION_DONE);
    } else if (node->stateMachine_.IsHideAnimationPlaying()) {
        node->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_DONE);
    }
    WLOGFI("ProcessNodeStateTask windowId: %{public}u, name:%{public}s state: %{public}u",
        node->GetWindowId(), node->GetWindowName().c_str(), static_cast<uint32_t>(node->stateMachine_.GetCurrentState()));
}

sptr<RSWindowAnimationFinishedCallback> RemoteAnimation::CreateShowAnimationFinishedCallback(
    const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode)
{
    wptr<WindowNode> srcNodeWptr = srcNode;
    wptr<WindowNode> dstNodeWptr = dstNode;
    // need add timeout check
    std::function<void(void)> func;
    if (!animationFirst_) {
        WLOGFE("RSWindowAnimation: not animationFirst!");
        func = []() {
            WLOGFI("RSWindowAnimation: on finish transition with minimizeAll!");
            MinimizeApp::ExecuteMinimizeAll();
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
        };
    } else {
        func = [srcNodeWptr, dstNodeWptr]() {
            WLOGFI("RSWindowAnimation: on ShowAnimationFinishedCallback!");
            auto srcNodeSptr = srcNodeWptr.promote();
            auto dstNodeSptr = dstNodeWptr.promote();
            if (dstNodeSptr == nullptr) {
                WLOGFE("dstNode is nullptr!");
                return;
            }
            ProcessNodeStateTask(dstNodeSptr);
            // launcher not do this
            if (srcNodeSptr!= nullptr && WindowHelper::IsMainWindow(srcNodeSptr->GetWindowType())) {
                ProcessNodeStateTask(srcNodeSptr);
            }
            WLOGFI("current window:%{public}u state: %{public}u", dstNodeSptr->GetWindowId(),
                static_cast<uint32_t>(dstNodeSptr->stateMachine_.GetCurrentState()));
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
        };
    }
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        func);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return nullptr;
    }
    return finishedCallback;
}

static void ProcessAbility(const sptr<WindowNode>& srcNode, TransitionEvent event)
{
    switch (event) {
        case TransitionEvent::CLOSE: {
            WLOGFI("close windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            AAFwk::AbilityManagerClient::GetInstance()->CloseAbility(srcNode->abilityToken_);
            break;
        }
        case TransitionEvent::BACK: {
            WLOGFI("terminate windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            AAFwk::Want resultWant;
            AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(
                srcNode->abilityToken_, -1, &resultWant);
            break;
        }
        case TransitionEvent::MINIMIZE: {
            WLOGFI("minimize windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            WindowInnerManager::GetInstance().MinimizeAbility(srcNode, true);
            break;
        }
        default:
            break;
    }
}

sptr<RSWindowAnimationFinishedCallback> RemoteAnimation::CreateHideAnimationFinishedCallback(
    const sptr<WindowNode>& srcNode, TransitionEvent event)
{
    wptr<WindowNode> srcNodeWptr = srcNode;
    // need add timeout check
    std::function<void(void)> func;
    if (!animationFirst_) {
        func = [srcNodeWptr, event]() {
            auto weakNode = srcNodeWptr.promote();
            if (weakNode == nullptr || weakNode->abilityToken_ == nullptr) {
                WLOGFE("window node or ability token is nullptr");
                return;
            }
            ProcessAbility(weakNode, event);
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
        };
    } else {
        ProcessAbility(srcNode, event); // execute first when animationFirst
        func = [srcNodeWptr]() {
            WLOGFI("RSWindowAnimation: on HideAnimationFinishedCallback!");
            auto srcNodeSptr = srcNodeWptr.promote();
            ProcessNodeStateTask(srcNodeSptr);
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
        };
    }
    sptr<RSWindowAnimationFinishedCallback> callback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        func);
    if (callback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return nullptr;
    }
    return callback;
}
} // Rosen
} // OHOS
