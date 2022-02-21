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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H
#define FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H

#include <refbase.h>

#include "abstract_screen.h"
#include "display_info.h"

namespace OHOS::Rosen {
class AbstractDisplay : public RefBase {
public:
    constexpr static int32_t DEFAULT_WIDTH = 720;
    constexpr static int32_t DEFAULT_HIGHT = 1280;
    constexpr static float DEFAULT_VIRTUAL_PIXEL_RATIO = 1.0;
    constexpr static uint32_t DEFAULT_FRESH_RATE = 60;
    AbstractDisplay(const DisplayInfo& info);
    AbstractDisplay(DisplayId id, ScreenId screenId, int32_t width, int32_t height, uint32_t freshRate);
    ~AbstractDisplay() = default;
    static inline bool IsVertical(Rotation rotation)
    {
        return (rotation == Rotation::ROTATION_0 || rotation == Rotation::ROTATION_180);
    }
    DisplayId GetId() const;
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    uint32_t GetFreshRate() const;
    float GetVirtualPixelRatio() const;
    ScreenId GetAbstractScreenId() const;
    bool BindAbstractScreen(ScreenId dmsScreenId);
    bool BindAbstractScreen(sptr<AbstractScreen> abstractDisplay);
    const sptr<DisplayInfo> ConvertToDisplayInfo() const;

    void SetId(DisplayId displayId);
    void SetWidth(int32_t width);
    void SetHeight(int32_t height);
    void SetFreshRate(uint32_t freshRate);
    void SetVirtualPixelRatio(float virtualPixelRatio);
    bool RequestRotation(Rotation rotation);
    Rotation GetRotation();

private:
    DisplayId id_ { DISPLAY_ID_INVALD };
    ScreenId screenId_ { SCREEN_ID_INVALID };
    int32_t width_ { 0 };
    int32_t height_ { 0 };
    uint32_t freshRate_ { 0 };
    float virtualPixelRatio_ { 1.0 };
    Rotation rotation_ { Rotation::ROTATION_0 };
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_H