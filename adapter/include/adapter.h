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

#ifndef OHOS_ROSEN_WINDOW_ADAPTER_H
#define OHOS_ROSEN_WINDOW_ADAPTER_H
#define _NEW_RENDERSERVER_
#include <string.h>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <functional>
#include <refbase.h>
#include <map>
#include "foundation/windowmanager/interfaces/innerkits/wm/window_option.h"
#include "wm_common.h"
#include "window.h"
#include "foundation/graphic/standard/interfaces/innerkits/wmclient/window_option.h"
#include "window_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class Adapter {
public:
    Adapter();
    ~Adapter();

    static WMError Init();
    static bool CreateWestonWindow(sptr<OHOS::Rosen::WindowOption>& option);
    static void DestroyWestonWindow();
    static bool GetMainWindowRect(OHOS::Rosen::Rect &rect);
    static OHOS::Rosen::WMError Show();
    static OHOS::Rosen::WMError Hide();
    static OHOS::Rosen::WMError MoveTo(int32_t x, int32_t y);
    static OHOS::Rosen::WMError Resize(uint32_t width, uint32_t height);

private:
    static bool WindowOptionConvertToWeston(sptr<OHOS::Rosen::WindowOption>& option,
        sptr<OHOS::WindowOption>& config);
    static bool GetSurfaceBuffer(sptr<OHOS::Surface> &surface,
        sptr<OHOS::SurfaceBuffer> &buffer,
        OHOS::BufferRequestConfig &rconfig);
    static void WriteDataToSurface();
    static void ColorDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BlackDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void FlushDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void set(void *vaddr, char ch, size_t len);
};
}
}
#endif // OHOS_ROSEN_WINDOW_ADAPTER_H
