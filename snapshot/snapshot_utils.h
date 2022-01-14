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

#ifndef SNAPSHOT_UTILS_H
#define SNAPSHOT_UTILS_H

#include <cstdint>
#include <string>

#include "refbase.h"
#include "pixel_map.h"
#include "display_manager.h"
#include "screen_manager.h"
#include "dm_common.h"
#include "screen.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "core/ui/rs_display_node.h"
#include "core/ui/rs_surface_node.h"

namespace OHOS {
const int BITMAP_DEPTH = 8;
using  WriteToPngParam = struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bitDepth;
    const uint8_t *data;
};

using CmdArgments = struct {
    Rosen::DisplayId displayId = Rosen::DISPLAY_ID_INVALD;
    std::string fileName;
    Media::Rect cropRect = { -1, -1, -1, -1 };
    Media::Size dstSize = { -1, -1 };
};

class SnapShotUtils {
public:
    SnapShotUtils() = default;
    ~SnapShotUtils() = default;

    static void PrintUsage(const std::string &cmdLine);
    static bool CheckFileNameValid(const std::string &fileName);
    static bool WriteToPng(const std::string &fileName, const WriteToPngParam &param);
    static bool WriteToPngWithPixelMap(const std::string &fileName, Media::PixelMap &pixelMap);
    static void DrawSurface(SkRect surfaceGeometry, uint32_t color,
        SkRect shapeGeometry, std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode);
    static void DumpBuffer(const sptr<SurfaceBuffer> &buf);
    static std::shared_ptr<Rosen::RSSurfaceNode> CreateSurface();
    static int ProcessArgs(int argc, char * const argv[], CmdArgments& cmdArgments);
private:
};
}

#endif // SNAPSHOT_UTILS_H
