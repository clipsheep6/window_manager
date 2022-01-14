/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: snapshot virtual display
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

#include <string>

#include "snapshot_utils.h"
#include "image_reader.h"
#include "image_reader_handler_impl.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Media;

#define FAKE_VIRTUAL_DISPLAY_SURFACE

// ABGR
namespace {
const int SLEEP_US = 10 * 1000; // 10ms
const int MAX_WAIT_COUNT = 1000;
}

int main(int argc, char *argv[])
{
    CmdArgments cmdArgments;
    cmdArgments.fileName = "/data/snapshot_virtual_display_1.png";

    if (SnapShotUtils::ProcessArgs(argc, argv, cmdArgments) <= 0) {
        return 0;
    }

    ImageReader imgReader;
    sptr<ImageReaderHandlerImpl> imgReaderHandler = new ImageReaderHandlerImpl();
    if (!imgReader.Init()) {
        printf("ImgReader init failed!\n");
    }
    imgReader.SetHandler(imgReaderHandler);

    auto &screenManager = ScreenManager::GetInstance();

    auto display = DisplayManager::GetInstance().GetDefaultDisplay();

    VirtualScreenOption option = {
        .name_ = "virtualDisplay",
        .width_ = display->GetWidth(),
        .height_ = display->GetHeight(),
        .density_ = 2.0,
        .surface_ = imgReader.GetSurface(),
        .flags_ = 0
    };
    ScreenId mainId = static_cast<ScreenId>(DisplayManager::GetInstance().GetDefaultDisplayId());

    #if 1
    ScreenId virtualscreenId2 = screenManager.CreateVirtualScreen(option);
    screenManager.AddMirror(mainId, virtualscreenId2);
    printf("add mirror id = %llu\n", virtualscreenId2 >> 32);

    for (int i = 0; i < 10; i++) {
        ScreenId virtualscreenId = screenManager.CreateVirtualScreen(option);
        screenManager.AddMirror(mainId, virtualscreenId);
        screenManager.DestroyVirtualScreen(virtualscreenId);
        printf("test mirror %d, id = %llu\n", i, virtualscreenId >> 32);
    }
    for (int i = 0; i < 10; i++) {
        ScreenId virtualscreenId = screenManager.CreateVirtualScreen(option);
        screenManager.DestroyVirtualScreen(virtualscreenId);
        printf("test mirror %d, id = %llu\n", i, virtualscreenId >> 32);
    }
    #endif

    ScreenId virtualscreenId = screenManager.CreateVirtualScreen(option);
    screenManager.AddMirror(mainId, virtualscreenId);
    printf("virtualscreenId: %" PRIu64 "\n", virtualscreenId >> 32);

    int wait_count = 0;
    while (!imgReaderHandler->IsImageOk()) {
        if (wait_count >= MAX_WAIT_COUNT) {
            break; // timeout
        }
        wait_count++;
        usleep(SLEEP_US);
    }

    bool ret = false;
    if (wait_count < MAX_WAIT_COUNT) {
        printf("enter pixelmap\n");
        auto pixelMap = imgReaderHandler->GetPixelMap();
        ret = SnapShotUtils::WriteToPngWithPixelMap(cmdArgments.fileName, *pixelMap);
    }
    if (ret) {
        printf("snapshot %" PRIu64 ", write to %s as png\n", cmdArgments.displayId, cmdArgments.fileName.c_str());
    } else {
        printf("snapshot %" PRIu64 " write to %s failed!\n", cmdArgments.displayId, cmdArgments.fileName.c_str());
    }

    imgReader.DeInit();


    screenManager.DestroyVirtualScreen(virtualscreenId);
    printf("DestroyVirtualDisplay %" PRIu64 "\n", virtualscreenId >> 32);

    #if 1
    screenManager.DestroyVirtualScreen(virtualscreenId2);
    printf("destroy id = %llu\n", virtualscreenId2 >> 32);
    #endif

    return 0;
}