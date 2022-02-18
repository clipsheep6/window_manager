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

#ifndef FOUNDATION_DM_SCREEN_H
#define FOUNDATION_DM_SCREEN_H

#include <string>
#include <vector>

#include <surface.h>
#include <screen_manager/screen_types.h>

#include "dm_common.h"

namespace OHOS::Rosen {
class ScreenInfo;

struct Point {
    int32_t posX_;
    int32_t posY_;
    Point() {};
    Point(int32_t posX, int32_t posY) : posX_(posX), posY_(posY) {};
};

struct SupportedScreenModes : public RefBase {
    uint32_t width_;
    uint32_t height_;
    uint32_t freshRate_;
};

struct VirtualScreenOption {
    const std::string name_;
    uint32_t width_;
    uint32_t height_;
    float density_;
    sptr<Surface> surface_;
    int32_t flags_;
    bool isForShot_ {true};
};

struct ExpandOption {
    uint32_t screenId_;
    uint32_t startX_;
    uint32_t startY_;
};

class Screen : public RefBase {
public:
    Screen(const ScreenInfo* info);
    ~Screen();
    bool IsGroup() const;
    ScreenId GetId() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetVirtualWidth() const;
    uint32_t GetVirtualHeight() const;
    float GetVirtualPixelRatio() const;
    Rotation GetRotation();
    Orientation GetOrientation();
    ScreenId GetParentId() const;
    uint32_t GetModeId() const;
    std::vector<sptr<SupportedScreenModes>> GetSupportedModes() const;
    bool SetScreenActiveMode(uint32_t modeId);
    bool SetOrientation(Orientation orientation);

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts) const;
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut) const;
    DMError SetScreenColorGamut(int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap) const;
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform();

private:
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_H