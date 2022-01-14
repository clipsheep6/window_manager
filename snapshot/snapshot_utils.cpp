/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: snapshot utils
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

#include "snapshot_utils.h"

#include <cstdio>
#include <getopt.h>
#include <climits>
#include <cstdlib>

#include "render_context/render_context.h"
#include "ui/rs_surface_extractor.h"
#include "png.h"

#define ACE_ENABLE_GL

using namespace OHOS::Media;
using namespace OHOS::Rosen;

namespace detail {
RenderContext *g_renderContext = nullptr;

template<typename Duration>
using SysTime = std::chrono::time_point<std::chrono::system_clock, Duration>;
using SysMicroSeconds = SysTime<std::chrono::microseconds>;

std::time_t MicroSecondsSinceEpoch()
{
    SysMicroSeconds tmp = std::chrono::system_clock::now();
    return tmp.time_since_epoch().count();
}

RenderContext *GetRenderContext()
{
    if (g_renderContext != nullptr) {
        return g_renderContext;
    }

    g_renderContext = RenderContextFactory::GetInstance().CreateEngine();
    g_renderContext->InitializeEglContext();
    return g_renderContext;
}
} // namespace detail

namespace OHOS {
void SnapShotUtils::PrintUsage(const std::string &cmdLine)
{
    printf("usage: %s [-i displayId] [-f output_file]\n", cmdLine.c_str());
}

bool SnapShotUtils::CheckFileNameValid(const std::string &fileName)
{
    std::string fileDir = fileName;
    auto pos = fileDir.find_last_of("/");
    if (pos != std::string::npos) {
        fileDir.erase(pos + 1);
    } else {
        fileDir = ".";
    }
    char resolvedPath[PATH_MAX] = { 0 };
    char *realPath = realpath(fileDir.c_str(), resolvedPath);
    if (realPath == nullptr) {
        printf("fileName %s invalid, nullptr!\n", fileName.c_str());
        return false;
    }
    std::string realPathString = realPath;
    if (realPathString.find("/data") != 0) {
        printf("fileName %s invalid, %s must dump at dir: /data \n", fileName.c_str(), realPathString.c_str());
        return false;
    }
    return true;
}


bool SnapShotUtils::WriteToPng(const std::string &fileName, const WriteToPngParam &param)
{
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        printf("png_create_write_struct error, nullptr!\n");
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        printf("png_create_info_struct error, nullptr!\n");
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        printf("open file [%s] error, nullptr!\n", fileName.c_str());
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);

    // set png header
    png_set_IHDR(pngStruct, pngInfo,
        param.width, param.height,
        param.bitDepth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct); // set packing info
    png_write_info(pngStruct, pngInfo); // write to header

    for (uint32_t i = 0; i < param.height; i++) {
        png_write_row(pngStruct, param.data + (i * param.stride));
    }

    png_write_end(pngStruct, pngInfo);

    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    fclose(fp);
    return true;
}

bool SnapShotUtils::WriteToPngWithPixelMap(const std::string &fileName, PixelMap &pixelMap)
{
    WriteToPngParam param;
    param.width = pixelMap.GetWidth();
    param.height = pixelMap.GetHeight();
    param.data = pixelMap.GetPixels();
    param.stride = pixelMap.GetRowBytes();
    param.bitDepth = BITMAP_DEPTH;
    return SnapShotUtils::WriteToPng(fileName, param);
}

void SnapShotUtils::DrawSurface(
    SkRect surfaceGeometry, uint32_t color, SkRect shapeGeometry, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        return;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(detail::GetRenderContext());
#endif
    auto framePtr = rsSurface->RequestFrame(width, height);
    if (framePtr == nullptr) {
        printf(": framePtr is nullptr!\n");
        return;
    }
    auto canvas = framePtr->GetCanvas();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(20); // stroken width 20
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);

    canvas->drawRect(shapeGeometry, paint);
    framePtr->SetDamageRegion(0, 0, width, height);
    rsSurface->FlushFrame(framePtr);
}

void SnapShotUtils::DumpBuffer(const sptr<SurfaceBuffer> &buf)
{
    if (access("/data", F_OK) < 0) {
        printf("ImageReader::DumpBuffer(): Can't access data directory!\n");
        return;
    }

    std::string dumpFileName = "/data/snapshot_virtual_display_0.png";
    BufferHandle *bufferHandle =  buf->GetBufferHandle();
    if (bufferHandle == nullptr) {
        printf("bufferHandle nullptr!\n");
    } else {
        printf("bufferHandle width: %d\n", bufferHandle->width);
        printf("bufferHandle height: %d\n", bufferHandle->height);
        printf("bufferHandle stride: %d\n", bufferHandle->stride);
        printf("bufferHandle format: %d\n", bufferHandle->format);
    }

    WriteToPngParam param;
    param.width = buf->GetWidth();
    param.height = buf->GetHeight();
    param.data = (const uint8_t *)buf->GetVirAddr();
    param.stride = buf->GetBufferHandle()->stride;
    param.bitDepth = BITMAP_DEPTH;
    SnapShotUtils::WriteToPng(dumpFileName, param);
    printf("ImageReader::DumpBuffer(): dump %s succeed.", dumpFileName.c_str());
}

std::shared_ptr<RSSurfaceNode> SnapShotUtils::CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}

static bool ProcessDisplayId(DisplayId &displayId)
{
    if (displayId == DISPLAY_ID_INVALD) {
        displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    } else {
        bool validFlag = false;
        auto displayIds = DisplayManager::GetInstance().GetAllDisplayIds();
        for (auto id: displayIds) {
            if (displayId == id) {
                validFlag = true;
                break;
            }
        }
        if (!validFlag) {
            printf("displayId %" PRIu64 " invalid!\n", displayId);
            printf("supported displayIds:\n");
            for (auto id: displayIds) {
                printf("\t%" PRIu64 "\n", id);
            }
            return false;
        }
    }
    return true;
}

int SnapShotUtils::ProcessArgs(int argc, char * const argv[], CmdArgments &cmdArgments)
{
    int opt = 0;
    const struct option longOption[] = {
        { "id", required_argument, nullptr, 'i' },
        { "file", required_argument, nullptr, 'f' },
        { "help", required_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, 0 }
    };
    while ((opt = getopt_long(argc, argv, "i:f:h", longOption, nullptr)) != -1) {
        switch (opt) {
            case 'i': // display id
                cmdArgments.displayId = atoll(optarg);
                break;
            case 'f': // output file name
                cmdArgments.fileName = optarg;
                break;
            case 'h': // help
                SnapShotUtils::PrintUsage(argv[0]);
                return 0;
            default:
                SnapShotUtils::PrintUsage(argv[0]);
                return 0;
        }
    }

    if (!ProcessDisplayId(cmdArgments.displayId)) {
        return -1;
    }

    // check fileName
    if (SnapShotUtils::CheckFileNameValid(cmdArgments.fileName) == false) {
        printf("filename %s invalid!\n", cmdArgments.fileName.c_str());
        return -1;
    }
    return 1;
}
}