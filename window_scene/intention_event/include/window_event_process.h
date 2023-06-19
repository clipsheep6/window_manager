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

#ifndef OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_PROCESS_H
#define OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_PROCESS_H

#include "singleton.h"

#include "base/geometry/ng/point_t.h"
#include "base/geometry/ng/rect_t.h"

namespace OHOS::Rosen {
class WindowEventProcess {
    DECLARE_DELAYED_SINGLETON(WindowEventProcess);
public:
    DISALLOW_COPY_AND_MOVE(WindowEventProcess);
    bool IsInWindowHotRect(const Ace::NG::RectF& windowSceneRect,
        const Ace::NG::PointF& parentLocalPoint, int32_t souceType);

private:
    bool GreatOrEqual(double left, double right);
    bool LessOrEqual(double left, double right);
};

#define WindowEventHandler DelayedSingleton<Rosen::WindowEventProcess>::GetInstance()
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_WINDOW_EVENT_PROCESS_H
