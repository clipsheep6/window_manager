/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WM_INCLUDE_WM_HELPER_H
#define OHOS_WM_INCLUDE_WM_HELPER_H

#include <wm_common.h>
#include <wm_common_inner.h>

namespace OHOS {
namespace Rosen {
class WindowHelper {
public:
    static inline bool IsMainWindow(WindowType type)
    {
        return (type >= WindowType::APP_MAIN_WINDOW_BASE && type < WindowType::APP_MAIN_WINDOW_END);
    }

    static inline bool IsSubWindow(WindowType type)
    {
        return (type >= WindowType::APP_SUB_WINDOW_BASE && type < WindowType::APP_SUB_WINDOW_END);
    }

    static inline bool IsAppWindow(WindowType type)
    {
        return (IsMainWindow(type) || IsSubWindow(type));
    }

    static inline bool IsBelowSystemWindow(WindowType type)
    {
        return (type >= WindowType::BELOW_APP_SYSTEM_WINDOW_BASE && type < WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsAboveSystemWindow(WindowType type)
    {
        return (type >= WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE && type < WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsSystemWindow(WindowType type)
    {
        return (IsBelowSystemWindow(type) || IsAboveSystemWindow(type));
    }

    static inline bool IsMainFloatingWindow(WindowType type, WindowMode mode)
    {
        return ((IsMainWindow(type)) && (mode == WindowMode::WINDOW_MODE_FLOATING));
    }

    static inline bool IsAvoidAreaWindow(WindowType type)
    {
        return (type == WindowType::WINDOW_TYPE_STATUS_BAR || type == WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    }

    static inline bool IsSplitWindowMode(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    }

    static inline bool IsValidWindowMode(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_FULLSCREEN || mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
            mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY || mode == WindowMode::WINDOW_MODE_FLOATING ||
            mode == WindowMode::WINDOW_MODE_PIP;
    }

    static inline bool IsValidWindowBlurLevel(WindowBlurLevel level)
    {
        return (level >= WindowBlurLevel::WINDOW_BLUR_OFF && level <= WindowBlurLevel::WINDOW_BLUR_HIGH);
    }

    static inline bool IsEmptyRect(const Rect& r)
    {
        return (r.posX_ == 0 && r.posY_ == 0 && r.width_ == 0 && r.height_ == 0);
    }

    static Rect GetFixedWindowRectByLimitSize(const Rect& oriDstRect, const Rect& lastRect, bool isVertical,
        float virtualPixelRatio)
    {
        uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
        uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);
        Rect dstRect = oriDstRect;
        // fix minimum size
        if (isVertical) {
            dstRect.width_ = std::max(minVerticalFloatingW, oriDstRect.width_);
            dstRect.height_ = std::max(minVerticalFloatingH, oriDstRect.height_);
        } else {
            dstRect.width_ = std::max(minVerticalFloatingH, oriDstRect.width_);
            dstRect.height_ = std::max(minVerticalFloatingW, oriDstRect.height_);
        }

        // fix maximum size
        dstRect.width_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), dstRect.width_);
        dstRect.height_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), dstRect.height_);

        // limit position by fixed width or height
        if (oriDstRect.posX_ != lastRect.posX_) {
            dstRect.posX_ = oriDstRect.posX_ + static_cast<int32_t>(oriDstRect.width_) -
                static_cast<int32_t>(dstRect.width_);
        }
        if (oriDstRect.posY_ != lastRect.posY_) {
            dstRect.posY_ = oriDstRect.posY_ + static_cast<int32_t>(oriDstRect.height_) -
                static_cast<int32_t>(dstRect.height_);
        }
        return dstRect;
    }

    static Rect GetFixedWindowRectByLimitPosition(const Rect& oriDstRect, const Rect& lastRect,
        float virtualPixelRatio, const Rect& displayLimitRect)
    {
        Rect dstRect = oriDstRect;
        uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
        // minimum position x
        if (dstRect.posX_ < (displayLimitRect.posX_ + static_cast<int32_t>(windowTitleBarH - oriDstRect.width_))) {
            if (oriDstRect.width_ != lastRect.width_) {
                dstRect.width_ = lastRect.width_;
            }
            dstRect.posX_ = displayLimitRect.posX_ + static_cast<int32_t>(windowTitleBarH - dstRect.width_);
        }
        // maximum position x
        if (dstRect.posX_ > (displayLimitRect.posX_ + static_cast<int32_t>(displayLimitRect.width_ - windowTitleBarH))) {
            dstRect.posX_ = displayLimitRect.posX_ + static_cast<int32_t>(displayLimitRect.width_ - windowTitleBarH);
            if (oriDstRect.width_ != lastRect.width_) {
                dstRect.width_ = lastRect.width_;
            }
        }
        // minimum position y
        dstRect.posY_ = std::max(displayLimitRect.posY_, oriDstRect.posY_);
        // maximum position y
        if (dstRect.posY_ > (displayLimitRect.posY_ +
                             static_cast<int32_t>(displayLimitRect.height_ - windowTitleBarH))) {
            dstRect.posY_ = displayLimitRect.posY_ + static_cast<int32_t>(displayLimitRect.height_ - windowTitleBarH);
            if (oriDstRect.height_ != lastRect.height_) {
                dstRect.height_ = lastRect.height_;
            }
        }
        return dstRect;
    }

    static bool IsPointInTargetRect(int32_t pointPosX, int32_t pointPosY, const Rect& targetRect)
    {
        if ((pointPosX > targetRect.posX_) &&
            (pointPosX < (targetRect.posX_ + static_cast<int32_t>(targetRect.width_))) &&
            (pointPosY > targetRect.posY_) &&
            (pointPosY < (targetRect.posY_ + static_cast<int32_t>(targetRect.height_)))) {
            return true;
        }
        return false;
    }

    static inline bool IsSwitchCascadeReason(WindowUpdateReason reason)
    {
        return (reason >= WindowUpdateReason::NEED_SWITCH_CASCADE_BASE) &&
            (reason < WindowUpdateReason::NEED_SWITCH_CASCADE_END);
    }

    static AvoidPosType GetAvoidPosType(const Rect& rect, uint32_t displayWidth, uint32_t displayHeight)
    {
        if (rect.width_ ==  displayWidth) {
            if (rect.posY_ == 0) {
                return AvoidPosType::AVOID_POS_TOP;
            } else {
                return AvoidPosType::AVOID_POS_BOTTOM;
            }
        } else if (rect.height_ ==  displayHeight) {
            if (rect.posX_ == 0) {
                return AvoidPosType::AVOID_POS_LEFT;
            } else {
                return AvoidPosType::AVOID_POS_RIGHT;
            }
        }

        return AvoidPosType::AVOID_POS_UNKNOWN;
    }

    WindowHelper() = default;
    ~WindowHelper() = default;
};
} // namespace OHOS
} // namespace Rosen
#endif // OHOS_WM_INCLUDE_WM_HELPER_H