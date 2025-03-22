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

#ifndef OHOS_ROSEN_FUTURE_CALLBACK_H
#define OHOS_ROSEN_FUTURE_CALLBACK_H

#include <future.h>
#include "interfaces/include/ws_common.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class FutureCallback : public RefBase {
public:
    WSError OnUpdateSessionRect(
        const Rect& rect, WindowSizeChangeReason reason, int32_t persistenId);
    Rect GetResizeAsyncResult(long timeOut); // unit: ms
    Rect GetMoveToAsyncResult(long timeOut); // unit: ms
    void ResetResizeLock();
    void ResetMoveToLock();

    // oriention
    OrientationInfo GetTargetOrientationResult(long timeOut); // unit: ms
    void ResetGetTargetRotationLock();
    WSError OnUpdateTargetOrientationInfo(OrientationInfo& info);

    // rotation change
    RotationChangeResult GetRotationResult(long timeout);
    void ResetRotationResultLock();
    void OnUpdateRotationResult(const RotationChangeResult rotationChangeResult);

private:
    RunnableFuture<Rect> resizeFuture_{};
    RunnableFuture<Rect> moveToFuture_{};
    RunnableFuture<OrientationInfo> getTargetRotationFuture_{};
    RunnableFuture<RotationChangeResult> getRotationResultFuture_{};
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_FUTURE_CALLBACK_H