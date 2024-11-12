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

#ifndef PC_FOLD_SCREEN_CONTROLLER_H
#define PC_FOLD_SCREEN_CONTROLLER_H

#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <tuple>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "dm_common.h"
#include "interfaces/include/ws_common.h"
#include "wm_single_instance.h"


namespace OHOS::Rosen {

class SceneSession;
using RectRecordsVector =
    std::vector<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, WSRect>>;

enum class ScreenFoldStatus : uint8_t {
    UNKNOWN = 0,
    COVER_CLOSE = 1,
    HALF_FOLDED = 2,
    HALF_FOLDED_PHYSICAL_KEYBOARD = 3,
    FLATTENED = 4,
};

enum class ScreenSide : uint8_t {
    EXPAND = 0,
    FOLD_B = 1,
    FOLD_C = 2,
};

class PcFoldScreenManager : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(PcFoldScreenManager);
public:
    explicit PcFoldScreenManager();
    void UpdateFoldScreenStatus(DisplayId displayId, ScreenFoldStatus status,
        const WSRect& defaultDisplayRect, const WSRect& virtualDisplayRect, const WSRect& foldCreaseRect);
    bool IsHalfFolded(DisplayId displayId);

    std::tuple<WSRect, WSRect, WSRect> GetDisplayRects();

    // animation parameters
    RSAnimationTimingProtocol GetMovingTimingProtocol();
    RSAnimationTimingCurve GetMovingTimingCurve();
    RSAnimationTimingProtocol GetThrowSlipTimingProtocol();
    RSAnimationTimingCurve GetThrowSlipTimingCurve();

    ScreenSide CalculateScreenSide(const WSRect& rect);

    void ResetArrangeRule();
    void ResetArrangeRule(ScreenSide side);
    void ResetArrangeRule(const WSRect& rect);

    void ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);

    bool NeedDoThrowSlip(ScreenSide startSide, WSRectF velocity);
    bool ThrowSlipToOppositeSide(ScreenSide startSide, WSRect& rect,
        int32_t topAvoidHeight, int32_t botAvoidHeight, int32_t titleHeight);
    void MappingRectInScreenSide(ScreenSide side, WSRect& rect,
        int32_t topAvoidHeight, int32_t botAvoidHeight);
    void MappingRectInScreenSideWithArrangeRule(ScreenSide side, WSRect& rect,
        int32_t topAvoidHeight, int32_t botAvoidHeight, int32_t titleHeight);

    void ApplyInitArrangeRule(WSRect& rect, WSRect& lastArrangedRect,
        const WSRect& limitRect, int32_t titleHeight);
    void ApplyArrangeRule(WSRect& rect, WSRect& lastArrangedRect,
        const WSRect& limitRect, int32_t titleHeight);
private:
    void SetDisplayInfo(DisplayId displayId, ScreenFoldStatus status);
    void SetDisplayRects(
        const WSRect& defaultDisplayRect, const WSRect& virtualDisplayRect, const WSRect& foldCreaseRect);
    float GetVpr();

    /*
     * fold screen property
     * if need, use map for multi fold screen
     */
    std::shared_mutex displayInfoMutex_; // protect display infos
    DisplayId displayId_;
    float vpr_ { 1.5f }; // display vp ratio
    ScreenFoldStatus screenFoldStatus_ { ScreenFoldStatus::UNKNOWN };
    std::shared_mutex rectsMutex_; // protect rects
    WSRect defaultDisplayRect_;
    WSRect virtualDisplayRect_;
    WSRect foldCreaseRect_;

    /*
     * arranged rect
     * x,y: last arranged position
     * w,h: offset for next arranged position
     */
    std::mutex arrangedRectsMutex_;
    WSRect defaultArrangedRect_;
    WSRect virtualArrangedRect_;
};

class PcFoldScreenController : public RefBase {
public:
    explicit PcFoldScreenController(wptr<SceneSession> weak);
    bool IsHalfFolded(DisplayId displayId);
    void RecordStartMoveRect(const WSRect& rect, bool isStartFullScreen);
    void RecordMoveRects(const WSRect& rect);
    bool ThrowSlip(DisplayId displayId, WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);
    bool IsStartFullScreen() const;
    void ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight);

    // animation parameters
    RSAnimationTimingProtocol GetMovingTimingProtocol();
    RSAnimationTimingCurve GetMovingTimingCurve();
    RSAnimationTimingProtocol GetThrowSlipTimingProtocol();
    RSAnimationTimingCurve GetThrowSlipTimingCurve();
private:
    int32_t GetPersistentId() const;
    int32_t GetTitleHeight() const;
    WSRectF CalculateMovingVelocity();
    void RemoveMoveRects();

    wptr<SceneSession> weakSceneSession_ = nullptr;

    // use queue to calculate velocity
    WSRect startMoveRect_;
    bool isStartFullScreen_ { false };
    RectRecordsVector movingRectRecords_;
};
} // namespace OHOS::Rosen

#endif // PC_FOLD_SCREEN_CONTROLLER_H