/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "window_listener.h"
#include "cj_lambda.h"
#include "event_handler.h"
#include "event_runner.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
CjWindowListener::CjWindowListener(int64_t callbackObject)
{
    weakRef_ = wptr<CjWindowListener>(this);
    auto func = reinterpret_cast<void(*)(void*)>(callbackObject);
    cjCallBack_ = CJLambda::Create(func);
}

CjWindowListener::~CjWindowListener()
{
    TLOGI(WmsLogTag::WMS_DIALOG, "~CjWindowListener");
}

void CjWindowListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void CjWindowListener::CallCjMethod(const char* methodName, void* argv, size_t argc)
{
    TLOGD(WmsLogTag::WMS_DIALOG, "[WindowListener]CallCjMethod methodName = %{public}s", methodName);
    if (cjCallBack_ == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowListener]env_ nullptr or jsCallBack_ is nullptr");
        return;
    }
    cjCallBack_(argv);
}

void CjWindowListener::OnSystemBarPropertyChange(DisplayId displayId,
    const SystemBarRegionTints& tints)
{
    return;
}

void CjWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    return;
}

void CjWindowListener::OnModeChange(WindowMode mode, bool hasDeco)
{
    return;
}

void CjWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type)
{
    return;
}

void CjWindowListener::AfterForeground()
{
    return;
}

void CjWindowListener::AfterBackground()
{
    return;
}

void CjWindowListener::AfterFocused()
{
    return;
}

void CjWindowListener::AfterUnfocused()
{
    return;
}

void CjWindowListener::AfterResumed()
{
    return;
}

void CjWindowListener::AfterPaused()
{
    return;
}

void CjWindowListener::AfterDestroyed()
{
    return;
}

void CjWindowListener::OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    if (info == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowListener] this changeInfo is nullptr");
        return;
    }
    TLOGI(WmsLogTag::WMS_DIALOG, "[WindowListener]OccupiedAreaChangeInfo, type: %{public}u, " \
        "input rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
        static_cast<uint32_t>(info->type_),
        info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowListener] this listener is nullptr");
        return;
    }
    void* argv = &(info->rect_.height_);
    size_t argc = 1;
    thisListener->CallCjMethod(KEYBOARD_HEIGHT_CHANGE_CB.c_str(), argv, argc);
    return;
}

void CjWindowListener::OnTouchOutside() const
{
    return;
}

void CjWindowListener::OnScreenshot()
{
    return;
}

void CjWindowListener::OnDialogTargetTouch() const
{
    return;
}

void CjWindowListener::OnDialogDeathRecipient() const
{
    return;
}

void CjWindowListener::OnGestureNavigationEnabledUpdate(bool enable)
{
    return;
}

void CjWindowListener::OnWaterMarkFlagUpdate(bool showWaterMark)
{
    return;
}

void CjWindowListener::OnWindowVisibilityChangedCallback(const bool isVisible)
{
    return;
}

void CjWindowListener::OnWindowStatusChange(WindowStatus status)
{
    return;
}

void CjWindowListener::OnWindowTitleButtonRectChanged(const TitleButtonRect& titleButtonRect)
{
    return;
}
}
}
