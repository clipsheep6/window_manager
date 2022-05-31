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

#include "include/core/SkCanvas.h"
#include "include/codec/SkCodec.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"

#include "inner_window_manager.h"
#include "window_manager_hilog.h"
#include "ui_service_mgr_client.h"
#include "nlohmann/json.hpp"
#include "window.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InnerWindowManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(InnerWindowManager)

InnerWindowManager::InnerWindowManager() : eventHandler_(nullptr), eventLoop_(nullptr),
    state_(InnerWMRunningState::STATE_NOT_START)
{
}

InnerWindowManager::~InnerWindowManager()
{
    Stop();
}

bool InnerWindowManager::DecodeImageFile(const char* filename, SkBitmap& bitmap)
{
    sk_sp<SkData> data(SkData::MakeFromFileName(filename));
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(std::move(data));
    if (codec == nullptr) {
        return false;
    }
    SkColorType colorType = kN32_SkColorType;
    SkImageInfo info = codec->getInfo().makeColorType(colorType);
    if (!bitmap.tryAllocPixels(info)) {
        return false;
    }
    return SkCodec::kSuccess == codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
}

void InnerWindowManager::DrawSurface(const sptr<Window>& window)
{
    auto surfaceNode = window->GetSurfaceNode();
    auto winRect = window->GetRect();
    WLOGFI("SurfaceWindowDraw winRect, x : %{public}d, y : %{public}d, width: %{public}d, height: %{public}d",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);

    auto width = winRect.width_;
    auto height = winRect.height_;

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        WLOGFE("RSSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(renderContext_.get());
#endif
    if (!isDividerImageLoaded_) {
        isDividerImageLoaded_ = DecodeImageFile(splitIconPath_, dividerBitmap_);
    }
    if (!dividerBitmap_.isNull() && dividerBitmap_.width() != 0 && dividerBitmap_.height() != 0) {
        DrawBitmap(rsSurface, width, height);
    } else {
        DrawColor(rsSurface, width, height);
    }
}

void InnerWindowManager::DrawBitmap(std::shared_ptr<RSSurface>& rsSurface, uint32_t width, uint32_t height)
{
    std::unique_ptr<RSSurfaceFrame> frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawBitmap frameptr is nullptr");
        return;
    }
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawBitmap canvas is nullptr");
        return;
    }
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    SkMatrix matrix;
    SkRect rect;
    rect.set(0, 0, static_cast<int>(width), static_cast<int>(height));
    if (dividerBitmap_.width() != 0 && dividerBitmap_.height() != 0) {
        if (width > height) {
            // rotate when divider is horizontal
            matrix.setScale(static_cast<float>(height) / dividerBitmap_.width(),
                static_cast<float>(width) / dividerBitmap_.height());
            matrix.postRotate(-90.0f); // divider shader rotate -90.0
        } else {
            matrix.setScale(static_cast<float>(width) / dividerBitmap_.width(),
                static_cast<float>(height) / dividerBitmap_.height());
        }
    }
    paint.setShader(dividerBitmap_.makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat));
    if (paint.getShader() == nullptr) {
        WLOGFE("DrawBitmap shader is nullptr");
        return;
    }
    paint.setShader(paint.getShader()->makeWithLocalMatrix(matrix));
    canvas->drawRect(rect, paint);
    frame->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(frame);
}

void InnerWindowManager::DrawColor(std::shared_ptr<RSSurface>& rsSurface, uint32_t width, uint32_t height)
{
    std::unique_ptr<RSSurfaceFrame> frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawColor frameptr is nullptr");
        return;
    }
    auto canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawColor canvas is nullptr");
        return;
    }
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    const SkScalar skWidth = 20; // stroke width is 20.
    paint.setStrokeWidth(skWidth);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(DIVIDER_HANDLE_COLOR);

    SkPoint rectPts[] = { {0, 0}, {static_cast<int>(width), static_cast<int>(height)} };
    SkRect rect;
    rect.set(rectPts[0], rectPts[1]);

    canvas->drawRect(rect, paint);
    frame->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(frame);
    return;
}
bool InnerWindowManager::Init()
{
    eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    if (eventLoop_ == nullptr) {
        return false;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        return false;
    }
    WLOGFI("init inner window manager service success.");
    return true;
}

void InnerWindowManager::Start()
{
    if (state_ == InnerWMRunningState::STATE_RUNNING) {
        WLOGFI("inner window manager service has already started.");
    }
    if (!Init()) {
        WLOGFI("failed to init inner window manager service.");
        return;
    }
    state_ = InnerWMRunningState::STATE_RUNNING;
    eventLoop_->Run();
    WLOGFI("inner window manager service start success.");
}

void InnerWindowManager::Stop()
{
    WLOGFI("stop inner window manager service.");
    eventLoop_->Stop();
    eventLoop_.reset();
    eventHandler_.reset();
    state_ = InnerWMRunningState::STATE_NOT_START;
}

void InnerWindowManager::HandleCreateWindow(std::string name, WindowType type, Rect rect)
{
    std::string params = "{\"shutdownButton\":\"Power off\"}";
    auto callback = [this](int32_t id, const std::string& event, const std::string& params) {
        if (params == "EVENT_INIT_CODE") {

        } else if (params == "EVENT_CANCLE_CODE") {
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        }
    };
    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        name, params, type, rect.posX_, rect.posY_, rect.width_, rect.height_, callback, &dialogId_
    );
    WLOGFI("create inner window id: %{public}d succes", dialogId_);
    return;
}

void InnerWindowManager::HandleCreateInnerWindow(WindowMode mode, DisplayId displayId)
{
    sptr<WindowOption> placeHolderWindowOpt = new (std::nothrow) WindowOption();
    if (placeHolderWindowOpt == nullptr) {
        WLOGFE("Window option is nullptr.");
        return;
    }
    // placeHolderWindowOpt->SetWindowRect(rect);
    placeHolderWindowOpt->SetWindowType(WindowType::WINDOW_TYPE_PLACE_HOLDER);
    placeHolderWindowOpt->SetFocusable(false);
    placeHolderWindowOpt->SetWindowMode(mode);
    sptr<Window> window = Window::Create("place_holder" + std::to_string(displayId), placeHolderWindowOpt);
    if (window == nullptr) {
        WLOGFE("Window is nullptr.");
        return;
    }
    window->Show();
#ifdef ACE_ENABLE_GL
    if (renderContext_ == nullptr) {
        renderContext_ = std::make_unique<RenderContext>();
        renderContext_->InitializeEglContext();
    }
#endif
    DrawSurface(window);
    WLOGFI("create inner window succes");
    return;
}

void InnerWindowManager::HandleDestroyWindow()
{
    WLOGFI("destroy inner window id:: %{public}d.", dialogId_);
    Ace::UIServiceMgrClient::GetInstance()->CancelDialog(dialogId_);
    dialogId_ = -1;
    return;
}

void InnerWindowManager::CreateWindow(std::string name, WindowType type, Rect rect)
{
    if (dialogId_ != -1) {
        return;
    }
    eventHandler_->PostTask([this, name, type, rect]() {
        HandleCreateWindow(name, type, rect);
    });
    return;
}

void InnerWindowManager::CreateInnerWindow(WindowMode mode, DisplayId displayId)
{
    eventHandler_->PostTask([this, mode, displayId]() {
        HandleCreateInnerWindow(mode, displayId);
    });
    return;
}

void InnerWindowManager::DestroyWindow()
{
    if (dialogId_ == -1) {
        WLOGFI("inner window has destroyed.");
        return;
    }
    eventHandler_->PostTask([this]() {
        HandleDestroyWindow();
    });
    return;
}
} // Rosen
} // OHOS