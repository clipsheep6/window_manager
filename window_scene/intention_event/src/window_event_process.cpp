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
#include "window_event_process.h"

#include "frameworks/core/event/ace_events.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
// constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventProcess" };
constexpr double epsilon = -0.001f;
constexpr float MOUSE_RECT_HOT = 4;
constexpr float TOUCH_RECT_HOT = 20;
} // namespace

WindowEventProcess::WindowEventProcess() {}
WindowEventProcess::~WindowEventProcess() {}

bool WindowEventProcess::IsInWindowHotRect(const Ace::NG::RectF& windowSceneRect,
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

bool WindowEventProcess::LessOrEqual(double left, double right)
{
    constexpr double epsilon = 0.001f;
    return (left - right) < epsilon;
}

bool WindowEventProcess::GreatOrEqual(double left, double right)
{
    return (left - right) > epsilon;
}
}
}