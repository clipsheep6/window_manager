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

#include "window_inner_manager.h"

#include "ui_service_mgr_client.h"
#include "window_manager_hilog.h"

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInnerManager"};
    const std::string IMAGE_PLACE_HOLDER_PNG_PATH = "/etc/window/resources/bg_place_holder.png";
    const int32_t IMAGE_WIDTH = 512;
    const int32_t IMAGE_HEIGHT = 512;
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowInnerManager)

WindowInnerManager::WindowInnerManager() : eventHandler_(nullptr), eventLoop_(nullptr),
    state_(InnerWMRunningState::STATE_NOT_START)
{
}

WindowInnerManager::~WindowInnerManager()
{
    Stop();
}

bool WindowInnerManager::Init()
{
    eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    if (eventLoop_ == nullptr) {
        return false;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        return false;
    }
    WLOGFI("init window inner manager service success.");
    return true;
}

void WindowInnerManager::Start()
{
    if (state_ == InnerWMRunningState::STATE_RUNNING) {
        WLOGFI("window inner manager service has already started.");
    }
    if (!Init()) {
        WLOGFI("failed to init window inner manager service.");
        return;
    }
    state_ = InnerWMRunningState::STATE_RUNNING;
    eventLoop_->Run();
    WLOGFI("window inner manager service start success.");
}

void WindowInnerManager::Stop()
{
    WLOGFI("stop window inner manager service.");
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_.reset();
    }
    if (eventHandler_ != nullptr) {
        eventHandler_.reset();
    }
    state_ = InnerWMRunningState::STATE_NOT_START;
}

void WindowInnerManager::HandleCreateWindow(std::string name, WindowType type, Rect rect)
{
    auto dialogCallback = [this](int32_t id, const std::string& event, const std::string& params) {
        if (params == "EVENT_CANCLE_CODE") {
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        }
    };
    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(name, dividerParams_, type,
        rect.posX_, rect.posY_, rect.width_, rect.height_, dialogCallback, &dialogId_);
    WLOGFI("create inner window id: %{public}d succes", dialogId_);
    return;
}

std::unique_ptr<OHOS::Media::PixelMap> WindowInnerManager::DecodeImageToPixelMap(const std::string &imagePath)
{
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/png";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    // CHKPP(imageSource);
    std::set<std::string> formats;
    ret = imageSource->GetSupportedFormats(formats);
    WLOGFD("get supported format ret:%{public}u", ret);

    OHOS::Media::DecodeOptions decodeOpts;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    if (pixelMap == nullptr) {
        WLOGFE("pixelMap is nullptr");
    }
    return pixelMap;
}

void WindowInnerManager::DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas)
{
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap = DecodeImageToPixelMap(IMAGE_PLACE_HOLDER_PNG_PATH);
    OHOS::Rosen::Drawing::Pen pen;
    pen.SetAntiAlias(true);
    pen.SetColor(OHOS::Rosen::Drawing::Color::COLOR_BLUE);
    OHOS::Rosen::Drawing::scalar penWidth = 1;
    pen.SetWidth(penWidth);
    canvas.AttachPen(pen);
    canvas.DrawBitmap(*pixelmap, 0, 0);
}

void WindowInnerManager::DoDraw(uint8_t *addr, uint32_t width, uint32_t height)
{
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format { OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::ALPHATYPE_OPAQUYE };
    bitmap.Build(width, height, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_TRANSPARENT);
    DrawPixelmap(canvas);
    static constexpr uint32_t stride = 4;
    uint32_t addrSize = width * height * stride;
    errno_t ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
    if (ret != EOK) {
        return;
    }
}
void WindowInnerManager::DestroyPlaceHolderWindow()
{
    if (placeHolderwindow_ != nullptr) {
        placeHolderwindow_->Destroy();
        placeHolderwindow_ = nullptr;
    }
}

void WindowInnerManager::CreatePlaceHolderWindow(WindowMode mode, DisplayId displayId)
{
    eventHandler_->PostTask([this, mode, displayId]() {
        HandleCreatePlaceHolderWindow(mode, displayId);
    });
    return;
}

void TouchOutsideListener::OnTouchOutside()
{
    WindowInnerManager::GetInstance().DestroyPlaceHolderWindow();
}

void InputListener::OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    WindowInnerManager::GetInstance().DestroyPlaceHolderWindow();
}

void InputListener::OnPointerInputEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WindowInnerManager::GetInstance().DestroyPlaceHolderWindow();
}
void WindowInnerManager::HandleCreatePlaceHolderWindow(WindowMode mode, DisplayId displayId)
{
    if (placeHolderwindow_ != nullptr) {
        return;
    }
    sptr<WindowOption> opt = new (std::nothrow) WindowOption();
    if (opt == nullptr) {
        WLOGFE("Window option is nullptr.");
        return;
    }
    opt->SetWindowType(WindowType::WINDOW_TYPE_PLACE_HOLDER);
    opt->SetFocusable(false);
    opt->SetWindowMode(mode);
    placeHolderwindow_ = Window::Create("place_holder" + std::to_string(displayId), opt);
    if (placeHolderwindow_ == nullptr) {
        WLOGFE("Window is nullptr.");
        return;
    }
    sptr<ITouchOutsideListener> touchOutsideListener = new TouchOutsideListener();
    sptr<IInputEventListener> inputListener = new InputListener();
    placeHolderwindow_->RegisterTouchOutsideListener(touchOutsideListener);
    placeHolderwindow_->RegisterInputEventListener(inputListener);
    placeHolderwindow_->Show();

    sptr<OHOS::Surface> layer = GetLayer();
    if (layer == nullptr) {
        placeHolderwindow_->Destroy();
        placeHolderwindow_ = nullptr;
        return;
    }

    sptr<OHOS::SurfaceBuffer> buffer = GetSurfaceBuffer(layer);
    if (buffer == nullptr || buffer->GetVirAddr() == nullptr) {
        placeHolderwindow_->Destroy();
        placeHolderwindow_ = nullptr;
        return;
    }

    auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
    DoDraw(addr, buffer->GetWidth(), buffer->GetHeight());
    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    OHOS::SurfaceError ret = layer->FlushBuffer(buffer, -1, flushConfig);
    WLOGFD("draw pointer FlushBuffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
    placeHolderwindow_->Show();
    return;
}

sptr<OHOS::Surface> WindowInnerManager::GetLayer()
{
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode = placeHolderwindow_->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        placeHolderwindow_->Destroy();
        placeHolderwindow_ = nullptr;
        return nullptr;
    }
    return surfaceNode->GetSurface();
}

sptr<OHOS::SurfaceBuffer> WindowInnerManager::GetSurfaceBuffer(sptr<OHOS::Surface> layer) const
{
    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence = 0;
    OHOS::BufferRequestConfig config = {
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };

    OHOS::SurfaceError ret = layer->RequestBuffer(buffer, releaseFence, config);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        WLOGFE("request buffer ret:%{public}s", SurfaceErrorStr(ret).c_str());
        return nullptr;
    }
    return buffer;
}

void WindowInnerManager::HandleDestroyWindow()
{
    if (dialogId_ == -1) {
        return;
    }
    WLOGFI("destroy inner window id:: %{public}d.", dialogId_);
    Ace::UIServiceMgrClient::GetInstance()->CancelDialog(dialogId_);
    dialogId_ = -1;
    return;
}

void WindowInnerManager::CreateWindow(std::string name, WindowType type, Rect rect)
{
    if (dialogId_ != -1) {
        return;
    }
    eventHandler_->PostTask([this, name, type, rect]() {
        HandleCreateWindow(name, type, rect);
    });
    return;
}

void WindowInnerManager::DestroyWindow()
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