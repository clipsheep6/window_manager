/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_test_utils.h"
#include <ability_context.h>
#include "wm_common_inner.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowTestUtils"};
}

Rect WindowTestUtils::displayRect_        = {0, 0, 0, 0};
Rect WindowTestUtils::statusBarRect_      = {0, 0, 0, 0};
Rect WindowTestUtils::naviBarRect_        = {0, 0, 0, 0};
Rect WindowTestUtils::customAppRect_      = {0, 0, 0, 0};
Rect WindowTestUtils::limitDisplayRect_   = {0, 0, 0, 0};
SplitRects WindowTestUtils::splitRects_   = {
    .primaryRect   = {0, 0, 0, 0},
    .secondaryRect = {0, 0, 0, 0},
    .dividerRect   = {0, 0, 0, 0},
};
Rect WindowTestUtils::singleTileRect_     = {0, 0, 0, 0};
std::vector<Rect> WindowTestUtils::doubleTileRects_ = std::vector<Rect>(2);
std::vector<Rect> WindowTestUtils::tripleTileRects_ = std::vector<Rect>(3);

bool WindowTestUtils::isVerticalDisplay_ = false;

sptr<Window> WindowTestUtils::CreateTestWindow(const TestWindowInfo& info)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowRect(info.rect);
    option->SetWindowType(info.type);
    option->SetWindowMode(info.mode);
    option->SetFocusable(info.focusable_);
    if (info.parentName != "") {
        option->SetParentName(info.parentName);
    }
    if (info.needAvoid) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    }
    if (info.parentLimit) {
        option->AddWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    } else {
        option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    }
    sptr<Window> window = Window::Create(info.name, option);
    return window;
}

sptr<Window> WindowTestUtils::CreateStatusBarWindow()
{
    TestWindowInfo info = {
        .name = "statusBar",
        .rect = statusBarRect_,
        .type = WindowType::WINDOW_TYPE_STATUS_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    return CreateTestWindow(info);
}

sptr<Window> WindowTestUtils::CreateNavigationBarWindow()
{
    TestWindowInfo info = {
        .name = "naviBar",
        .rect = naviBarRect_,
        .type = WindowType::WINDOW_TYPE_NAVIGATION_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    return CreateTestWindow(info);
}

sptr<WindowScene> WindowTestUtils::CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, abilityContext, listener);
    return scene;
}

Rect WindowTestUtils::GetDefaultFoatingRect(const sptr<Window>& window)
{
    limitDisplayRect_ = displayRect_;
    UpdateSplitRects(window);
    constexpr uint32_t half = 2;
    constexpr float ratio = 0.75;  // 0.75: default height/width ratio

    // calculate default H and w
    uint32_t defaultW = static_cast<uint32_t>(displayRect_.width_ * ratio);
    uint32_t defaultH = static_cast<uint32_t>(displayRect_.height_ * ratio);

    // calculate default x and y
    Rect resRect = {0, 0, defaultW, defaultH};
    if (defaultW <= limitDisplayRect_.width_ && defaultH <= limitDisplayRect_.height_) {
        int32_t centerPosX = limitDisplayRect_.posX_ + static_cast<int32_t>(limitDisplayRect_.width_ / half);
        resRect.posX_ = centerPosX - static_cast<int32_t>(defaultW / half);

        int32_t centerPosY = limitDisplayRect_.posY_ + static_cast<int32_t>(limitDisplayRect_.height_ / half);
        resRect.posY_ = centerPosY - static_cast<int32_t>(defaultH / half);
    }

    return resRect;
}

Rect WindowTestUtils::GetLimitedDecoRect(const Rect& rect, float virtualPixelRatio)
{
    constexpr uint32_t windowTitleBarHeight = 48;
    constexpr uint32_t windowFrameWidth = 4;
    uint32_t winFrameH = static_cast<uint32_t>((windowTitleBarHeight + windowFrameWidth) * virtualPixelRatio);
    uint32_t winFrameW = static_cast<uint32_t>((windowFrameWidth + windowFrameWidth) * virtualPixelRatio);
    uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);

    bool vertical = displayRect_.width_ < displayRect_.height_;
    uint32_t minFloatingW = vertical ? minVerticalFloatingW : minVerticalFloatingH;
    uint32_t minFloatingH = vertical ? minVerticalFloatingH : minVerticalFloatingW;
    Rect resRect = {
        rect.posX_,
        rect.posY_,
        std::max(minFloatingW, rect.width_ + winFrameW),
        std::max(minFloatingH, rect.height_ + winFrameH),
    };
    return resRect;
}

Rect WindowTestUtils::CalcLimitedRect(const Rect& rect, float virtualPixelRatio)
{
    uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);

    bool vertical = displayRect_.width_ < displayRect_.height_;
    uint32_t minFloatingW = vertical ? minVerticalFloatingW : minVerticalFloatingH;
    uint32_t minFloatingH = vertical ? minVerticalFloatingH : minVerticalFloatingW;
    Rect resRect = {
        rect.posX_,
        rect.posY_,
        std::max(minFloatingW, rect.width_),
        std::max(minFloatingH, rect.height_),
    };
    return resRect;
}

Rect WindowTestUtils::GetFloatingLimitedRect(const Rect& rect, float virtualPixelRatio)
{
    uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);
    bool vertical = displayRect_.width_ < displayRect_.height_;

    uint32_t minFloatingW = vertical ? minVerticalFloatingW : minVerticalFloatingH;
    uint32_t minFloatingH = vertical ? minVerticalFloatingH : minVerticalFloatingW;
    Rect resRect = {
        rect.posX_,
        rect.posY_,
        std::max(minFloatingW, rect.width_),
        std::max(minFloatingH, rect.height_),
    };
    return resRect;
}

void WindowTestUtils::InitByDisplayRect(const Rect& displayRect)
{
    const float barRatio = 0.07;
    const float appRation = 0.4;
    const float spaceRation = 0.125;
    displayRect_ = displayRect;
    limitDisplayRect_ = displayRect;
    if (displayRect_.width_ < displayRect_.height_) {
        isVerticalDisplay_ = true;
    }
    statusBarRect_ = {0, 0, displayRect_.width_, displayRect_.height_ * barRatio};
    naviBarRect_ = {0, displayRect_.height_ * (1 - barRatio), displayRect_.width_, displayRect_.height_ * barRatio};
    customAppRect_ = {
        displayRect_.width_ * spaceRation,
        displayRect_.height_ * spaceRation,
        displayRect_.width_ * appRation,
        displayRect_.height_ * appRation
    };
}

void WindowTestUtils::InitTileWindowRects(const sptr<Window>& window)
{
    constexpr uint32_t edgeInterval = 48;
    constexpr uint32_t midInterval = 24;
    constexpr float ratio = 0.75;  // 0.75: default height/width ratio
    constexpr float edgeRatio = 0.125;
    constexpr int half = 2;
    limitDisplayRect_ = displayRect_;
    UpdateSplitRects(window);
    int x = limitDisplayRect_.posX_ + (limitDisplayRect_.width_ * edgeRatio);
    int y = limitDisplayRect_.posY_ + (limitDisplayRect_.height_ * edgeRatio);
    uint32_t w = limitDisplayRect_.width_ * ratio;
    uint32_t h = limitDisplayRect_.height_ * ratio;
    singleTileRect_ = { x, y, w, h };
    WLOGFI("singleRect_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
    x = edgeInterval;
    w = (limitDisplayRect_.width_ - edgeInterval * half - midInterval) / half;
    // calc doubleRect
    doubleTileRects_[0] = {x, y, w, h};
    doubleTileRects_[1] = {x + w + midInterval, y, w, h};
    WLOGFI("doubleRects_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
    // calc tripleRect
    w = (limitDisplayRect_.width_ - edgeInterval * half - midInterval * half) / 3; // 3 is triple rects num
    tripleTileRects_[0] = {x, y, w, h};
    tripleTileRects_[1] = {x + w + midInterval, y, w, h};
    tripleTileRects_[2] = {x + w * half + midInterval * half, y, w, h}; // 2 is third index
    WLOGFI("tripleRects_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
}

bool WindowTestUtils::RectEqualTo(const sptr<Window>& window, const Rect& r)
{
    usleep(100000); // 100000us
    Rect l = window->GetRect();
    bool res = ((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
    if (!res) {
        WLOGFE("GetLayoutRect: %{public}d %{public}d %{public}d %{public}d, " \
            "Expect: %{public}d %{public}d %{public}d %{public}d", l.posX_, l.posY_, l.width_, l.height_,
            r.posX_, r.posY_, r.width_, r.height_);
    }
    return res;
}

bool WindowTestUtils::RectEqualToRect(const Rect& l, const Rect& r)
{
    bool res = ((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
    if (!res) {
        WLOGFE("GetLayoutRect: %{public}d %{public}d %{public}d %{public}d, " \
            "Expect: %{public}d %{public}d %{public}d %{public}d", l.posX_, l.posY_, l.width_, l.height_,
            r.posX_, r.posY_, r.width_, r.height_);
    }
    return res;
}

AvoidPosType WindowTestUtils::GetAvoidPosType(const Rect& rect)
{
    if (rect.width_ >=  rect.height_) {
        if (rect.posY_ == 0) {
            return AvoidPosType::AVOID_POS_TOP;
        } else {
            return AvoidPosType::AVOID_POS_BOTTOM;
        }
    } else {
        if (rect.posX_ == 0) {
            return AvoidPosType::AVOID_POS_LEFT;
        } else {
            return AvoidPosType::AVOID_POS_RIGHT;
        }
    }
    return AvoidPosType::AVOID_POS_UNKNOWN;
}

void WindowTestUtils::InitSplitRects()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!");
        return;
    }
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetFreshRate());

    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    displayRect_ = displayRect;
    limitDisplayRect_ = displayRect;

    float virtualPixelRatio = WindowTestUtils::GetVirtualPixelRatio(0);
    uint32_t dividerWidth = static_cast<uint32_t>(DIVIDER_WIDTH * virtualPixelRatio);

    if (displayRect_.width_ < displayRect_.height_) {
        isVerticalDisplay_ = true;
    }
    if (isVerticalDisplay_) {
        splitRects_.dividerRect = { 0,
                                    static_cast<uint32_t>((displayRect_.height_ - dividerWidth) * DEFAULT_SPLIT_RATIO),
                                    displayRect_.width_,
                                    dividerWidth, };
    } else {
        splitRects_.dividerRect = { static_cast<uint32_t>((displayRect_.width_ - dividerWidth) * DEFAULT_SPLIT_RATIO),
                                    0,
                                    dividerWidth,
                                    displayRect_.height_ };
    }
}

void WindowTestUtils::UpdateSplitRects(const sptr<Window>& window)
{
    std::unique_ptr<WindowTestUtils> testUtils = std::make_unique<WindowTestUtils>();
    auto res = window->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM, testUtils->avoidArea_);
    if (res != WMError::WM_OK) {
        WLOGFE("Get avoid type failed");
    }
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.leftRect);
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.topRect);
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.rightRect);
    testUtils->UpdateLimitDisplayRect(testUtils->avoidArea_.bottomRect);

    if (isVerticalDisplay_) {
        splitRects_.dividerRect.posY_ = limitDisplayRect_.posY_ +
            static_cast<uint32_t>((limitDisplayRect_.height_ - splitRects_.dividerRect.height_) * DEFAULT_SPLIT_RATIO);
        testUtils->UpdateLimitSplitRects(splitRects_.dividerRect.posY_);
    } else {
        splitRects_.dividerRect.posX_ = limitDisplayRect_.posX_ +
            static_cast<uint32_t>((limitDisplayRect_.width_ - splitRects_.dividerRect.width_) * DEFAULT_SPLIT_RATIO);
        testUtils->UpdateLimitSplitRects(splitRects_.dividerRect.posX_);
    }
}

void WindowTestUtils::UpdateLimitDisplayRect(Rect& avoidRect)
{
    if (((avoidRect.posX_ == 0) && (avoidRect.posY_ == 0) &&
        (avoidRect.width_ == 0) && (avoidRect.height_ == 0))) {
        return;
    }
    auto avoidPosType = GetAvoidPosType(avoidRect);
    int32_t offsetH = 0;
    int32_t offsetW = 0;
    switch (avoidPosType) {
        case AvoidPosType::AVOID_POS_TOP:
            offsetH = avoidRect.posY_ + avoidRect.height_ - limitDisplayRect_.posY_;
            limitDisplayRect_.posY_ += offsetH;
            limitDisplayRect_.height_ -= offsetH;
            break;
        case AvoidPosType::AVOID_POS_BOTTOM:
            offsetH = limitDisplayRect_.posY_ + limitDisplayRect_.height_ - avoidRect.posY_;
            limitDisplayRect_.height_ -= offsetH;
            break;
        case AvoidPosType::AVOID_POS_LEFT:
            offsetW = avoidRect.posX_ + avoidRect.width_ - limitDisplayRect_.posX_;
            limitDisplayRect_.posX_ += offsetW;
            limitDisplayRect_.width_ -= offsetW;
            break;
        case AvoidPosType::AVOID_POS_RIGHT:
            offsetW = limitDisplayRect_.posX_ + limitDisplayRect_.width_ - avoidRect.posX_;
            limitDisplayRect_.width_ -= offsetW;
            break;
        default:
            WLOGFE("invaild avoidPosType: %{public}d", avoidPosType);
    }
}

void WindowTestUtils::UpdateLimitSplitRects(int32_t divPos)
{
    std::unique_ptr<WindowTestUtils> testUtils = std::make_unique<WindowTestUtils>();
    if (isVerticalDisplay_) {
        splitRects_.dividerRect.posY_ = divPos;

        splitRects_.primaryRect.posX_ = displayRect_.posX_;
        splitRects_.primaryRect.posY_ = displayRect_.posY_;
        splitRects_.primaryRect.height_ = divPos;
        splitRects_.primaryRect.width_ = displayRect_.width_;

        splitRects_.secondaryRect.posX_ = displayRect_.posX_;
        splitRects_.secondaryRect.posY_ = splitRects_.dividerRect.posY_ + splitRects_.dividerRect.height_;
        splitRects_.secondaryRect.height_ = displayRect_.height_ - splitRects_.secondaryRect.posY_;
        splitRects_.secondaryRect.width_ = displayRect_.width_;
    } else {
        splitRects_.dividerRect.posX_ = divPos;

        splitRects_.primaryRect.posX_ = displayRect_.posX_;
        splitRects_.primaryRect.posY_ = displayRect_.posY_;
        splitRects_.primaryRect.width_ = divPos;
        splitRects_.primaryRect.height_ = displayRect_.height_;

        splitRects_.secondaryRect.posX_ = splitRects_.dividerRect.posX_ + splitRects_.dividerRect.width_;
        splitRects_.secondaryRect.posY_ = displayRect_.posY_;
        splitRects_.secondaryRect.width_ = displayRect_.width_ - splitRects_.secondaryRect.posX_;
        splitRects_.secondaryRect.height_ = displayRect_.height_;
    }

    testUtils->UpdateLimitSplitRect(splitRects_.primaryRect);
    testUtils->UpdateLimitSplitRect(splitRects_.secondaryRect);
}

void WindowTestUtils::UpdateLimitSplitRect(Rect& limitSplitRect)
{
    Rect curLimitRect = limitSplitRect;
    limitSplitRect.posX_ = std::max(limitDisplayRect_.posX_, curLimitRect.posX_);
    limitSplitRect.posY_ = std::max(limitDisplayRect_.posY_, curLimitRect.posY_);
    limitSplitRect.width_ = std::min(limitDisplayRect_.posX_ + limitDisplayRect_.width_,
                                     curLimitRect.posX_ + curLimitRect.width_) -
                                     limitSplitRect.posX_;
    limitSplitRect.height_ = std::min(limitDisplayRect_.posY_ + limitDisplayRect_.height_,
                                      curLimitRect.posY_ + curLimitRect.height_) -
                                      limitSplitRect.posY_;
}

float WindowTestUtils::GetVirtualPixelRatio(DisplayId displayId)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("GetVirtualPixel fail. Get display fail. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }

    float virtualPixelRatio = display->GetVirtualPixelRatio();
    if (virtualPixelRatio == 0.0) {
        WLOGFE("GetVirtualPixel fail. vpr is 0.0. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }

    WLOGFI("GetVirtualPixel success. displayId:%{public}" PRIu64", vpr:%{public}f", displayId, virtualPixelRatio);
    return virtualPixelRatio;
}
} // namespace ROSEN
} // namespace OHOS
