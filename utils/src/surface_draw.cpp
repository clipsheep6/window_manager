/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "surface_draw.h"

#include <algorithm>
#include <surface.h>
#include <ui/rs_surface_extractor.h>
#include "window_manager_hilog.h"
#include "include/core/SkCanvas.h"
#include "include/codec/SkCodec.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkPixmap.h"

#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SurfaceDraw"};
    const std::string IMAGE_PLACE_HOLDER_PNG_PATH = "/etc/window/resources/bg_place_holder.png";
    const int32_t IMAGE_WIDTH = 512;
    const int32_t IMAGE_HEIGHT = 512;
} // namespace

void SurfaceDraw::Init()
{
#ifdef ACE_ENABLE_GL
    WLOGFI("Draw divider on gpu");
    if (!rc_) {
        rc_ = std::make_unique<RenderContext>();
        rc_->InitializeEglContext();
        WLOGFI("Draw Init Success");
    }
#endif
}

std::shared_ptr<RSSurface> SurfaceDraw::PrepareDraw(std::shared_ptr<RSSurfaceNode> surfaceNode,
    std::unique_ptr<RSSurfaceFrame>& frame, SkCanvas*& canvas, uint32_t width, uint32_t height)
{
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        WLOGFE("RSSurface is nullptr");
        return nullptr;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(rc_.get());
#endif
    frame = rsSurface->RequestFrame(width, height);
    if (frame == nullptr) {
        WLOGFE("DrawBackgroundColor frameptr is nullptr");
        return nullptr;
    }
    canvas = frame->GetCanvas();
    if (canvas == nullptr) {
        WLOGFE("DrawBackgroundColor canvas is nullptr");
        return nullptr;
    }
    return rsSurface;
}

void SurfaceDraw::DrawBackgroundColor(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect, uint32_t bkgColor)
{
    WLOGFI("DrawBackgroundColor winRect, [%{public}d, %{public}d, %{public}d, %{public}d], bkgColor:%{public}x",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_, bkgColor);
    auto width = winRect.width_;
    auto height = winRect.height_;
    std::unique_ptr<RSSurfaceFrame> frame;
    SkCanvas* canvas;
    auto rsSurface = PrepareDraw(surfaceNode, frame, canvas, width, height);
    if (rsSurface == nullptr) {
        return;
    }
    SkPaint bkgPaint;
    bkgPaint.setColor(bkgColor);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, width, height), bkgPaint);
    rsSurface->FlushFrame(frame);
}

// just for test
static bool g_isLoaded = false;

bool SurfaceDraw::DecodeImageFile(const char* filename, SkBitmap& bitmap)
{
    if (g_isLoaded) {
        return true;
    }
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
    g_isLoaded = true;
    return SkCodec::kSuccess == codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
}

void SurfaceDraw::DrawBitmap(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect,
    SkBitmap& bitmap, uint32_t bkgColor)
{
    if (bitmap.isNull() || bitmap.width() == 0 || bitmap.height() == 0) {
        WLOGFE("bitmap is empty or width/height is 0");
        return;
    }
    WLOGFI("DrawBitmap winRect, x : %{public}d, y : %{public}d, width: %{public}d, height: %{public}d",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    auto width = static_cast<int32_t>(winRect.width_);
    auto height = static_cast<int32_t>(winRect.height_);
    std::unique_ptr<RSSurfaceFrame> frame;
    SkCanvas* canvas;
    auto rsSurface = PrepareDraw(surfaceNode, frame, canvas, width, height);
    if (rsSurface == nullptr) {
        return;
    }
    SkPaint bkgPaint;
    bkgPaint.setColor(bkgColor);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, width, height), bkgPaint);
    SkPaint paint;
    SkRect rect;
    int realHeight = std::min(height, bitmap.height()); // need to scale
    int realWidth = std::min(width, bitmap.width());
    int pointX = (width - realWidth) / 2;
    int pointY = (height - realHeight) / 2;
    rect.setXYWH(pointX, pointY, realWidth, realHeight);
    sk_sp<SkImage> image = SkImage::MakeFromBitmap(bitmap);
    if (!image) {
        WLOGFE("image is nullptr");
        return;
    }
    canvas->drawImageRect(image.get(), rect, &paint);
    rsSurface->FlushFrame(frame);
}

static SkAlphaType AlphaTypeToSkAlphaType(const sptr<Media::PixelMap>& pixmap)
{
    switch (pixmap->GetAlphaType()) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return SkAlphaType::kUnknown_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return SkAlphaType::kOpaque_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return SkAlphaType::kPremul_SkAlphaType;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return SkAlphaType::kUnpremul_SkAlphaType;
        default:
            return SkAlphaType::kUnknown_SkAlphaType;
    }
}

static SkColorType PixelFormatToSkColorType(const sptr<Media::PixelMap>& pixmap)
{
    switch (pixmap->GetPixelFormat()) {
        case Media::PixelFormat::RGB_565:
            return SkColorType::kRGB_565_SkColorType;
        case Media::PixelFormat::RGBA_8888:
            return SkColorType::kRGBA_8888_SkColorType;
        case Media::PixelFormat::BGRA_8888:
            return SkColorType::kBGRA_8888_SkColorType;
        case Media::PixelFormat::ALPHA_8:
            return SkColorType::kAlpha_8_SkColorType;
        case Media::PixelFormat::RGBA_F16:
            return SkColorType::kRGBA_F16_SkColorType;
        case Media::PixelFormat::UNKNOWN:
        case Media::PixelFormat::ARGB_8888:
        case Media::PixelFormat::RGB_888:
        case Media::PixelFormat::NV21:
        case Media::PixelFormat::NV12:
        case Media::PixelFormat::CMYK:
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}

static SkImageInfo MakeSkImageInfoFromPixelMap(sptr<Media::PixelMap>& pixmap)
{
    SkColorType colorType = PixelFormatToSkColorType(pixmap);
    SkAlphaType alphaType = AlphaTypeToSkAlphaType(pixmap);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), colorType, alphaType, colorSpace);
}

void SurfaceDraw::DrawSkImage(std::shared_ptr<RSSurfaceNode> surfaceNode, Rect winRect,
    sptr<Media::PixelMap> pixelMap, uint32_t bkgColor)
{
    // Get canvas
    WLOGFI("DrawSkImage winRect, [%{public}d, %{public}d, %{public}d, %{public}d], bkgColor:%{public}x",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_, bkgColor);
    auto width = static_cast<int32_t>(winRect.width_);
    auto height = static_cast<int32_t>(winRect.height_);
    std::unique_ptr<RSSurfaceFrame> frame;
    SkCanvas* canvas;
    auto rsSurface = PrepareDraw(surfaceNode, frame, canvas, width, height);
    if (rsSurface == nullptr) {
        return;
    }
    SkPaint bkgPaint;
    bkgPaint.setColor(bkgColor);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, width, height), bkgPaint);

    // Create SkPixmap from PixelMap
    auto imageInfo = MakeSkImageInfoFromPixelMap(pixelMap);
    SkPixmap imagePixmap(imageInfo, reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowBytes());

    // Create SkImage from SkPixmap
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
    if (!skImage) {
        WLOGFE("sk image is null");
        return;
    }

    SkPaint paint;
    SkRect rect;
    int realHeight = std::min(height, skImage->height()); // need to scale
    int realWidth = std::min(width, skImage->width());
    int pointX = (width - realWidth) / 2; // 2 is mid point
    int pointY = (height - realHeight) / 2; // 2 is mid point
    rect.setXYWH(pointX, pointY, realWidth, realHeight);

    canvas->drawImageRect(skImage.get(), rect, &paint);
    if (!rsSurface->FlushFrame(frame)) {
        WLOGFE("fail to flush frame");
        return;
    }
    return;
}

sptr<OHOS::Surface> SurfaceDraw::GetLayer(sptr<OHOS::Rosen::Window> window)
{
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        return nullptr;
    }
    return surfaceNode != nullptr ? surfaceNode->GetSurface() : nullptr;
}

sptr<OHOS::SurfaceBuffer> SurfaceDraw::GetSurfaceBuffer(sptr<OHOS::Surface> layer) const
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

std::unique_ptr<OHOS::Media::PixelMap> SurfaceDraw::DecodeImageToPixelMap(const std::string &imagePath)
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

void SurfaceDraw::DrawPixelmap(OHOS::Rosen::Drawing::Canvas &canvas)
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

void SurfaceDraw::DoDraw(uint8_t *addr, uint32_t width, uint32_t height)
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
} // Rosen
} // OHOS