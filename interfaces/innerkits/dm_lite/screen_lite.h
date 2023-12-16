/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_SCREEN_LITE_H
#define FOUNDATION_DM_SCREEN_LITE_H

#include <cstdint>
#include <string>
#include <vector>

#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class ScreenInfo;

class ScreenLite : public RefBase {
friend class ScreenManagerLite;
public:
    ~ScreenLite();
    ScreenLite(const ScreenLite&) = delete;
    ScreenLite(ScreenLite&&) = delete;
    ScreenLite& operator=(const ScreenLite&) = delete;
    ScreenLite& operator=(ScreenLite&&) = delete;
    bool IsGroup() const;
    std::string GetName() const;
    
    /**
     * @brief Get screen id.
     *
     * @return ScreenLite id.
     */
    ScreenId GetId() const;

    /**
     * @brief Get width of the screen.
     *
     * @return Width of the screen.
     */
    uint32_t GetWidth() const;
    
    /**
     * @brief Get height of the screen.
     *
     * @return Height of the screen.
     */
    uint32_t GetHeight() const;

    /**
     * @brief Get virtual width of the screen.
     *
     * @return Virtual width of the screen.
     */
    uint32_t GetVirtualWidth() const;

    /**
     * @brief Get virtual height of the screen.
     *
     * @return Virtual height of the screen.
     */
    uint32_t GetVirtualHeight() const;

    /**
     * @brief Get virtual pixel ratio of the screen.
     *
     * @return Virtual pixel ratio of the screen.
     */
    float GetVirtualPixelRatio() const;

    /**
     * @brief Get the Rotation of the screen.
     *
     * @return The Rotation of the screen.
     */
    Rotation GetRotation() const;

    /**
     * @brief Get the orientation of the screen.
     *
     * @return Orientation of the screen.
     */
    Orientation GetOrientation() const;

    /**
     * @brief Is a real screen.
     *
     * @return True means screen is real, false means the opposite.
     */
    bool IsReal() const;

    /**
     * @brief Get screen parent id.
     *
     * @return ScreenLite parent id.
     */
    ScreenId GetParentId() const;

    /**
     * @brief Get screen mode id.
     *
     * @return ScreenLite mode id.
     */
    uint32_t GetModeId() const;

    /**
     * @brief Get supported modes of the screen.
     *
     * @return Supported modes of the screen.
     */
    std::vector<sptr<SupportedScreenModes>> GetSupportedModes() const;

    /**
     * @brief Set screen active mode.
     *
     * @param moddId Mode id.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetScreenActiveMode(uint32_t modeId);

    /**
     * @brief Set orientation for the screen.
     *
     * @param orientation Orientation for the screen.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetOrientation(Orientation orientation) const;

    /**
     * @brief Set the density dpi of the screen.
     *
     * @param dpi Density dpi of the screen.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetDensityDpi(uint32_t dpi) const;

    /**
     * @brief Get the screen info.
     *
     * @return ScreenLite info.
     */
    sptr<ScreenInfo> GetScreenInfo() const;

protected:
    // No more methods or variables can be defined here.
    explicit ScreenLite(sptr<ScreenInfo> info);
    void UpdateScreenInfo() const;
    void UpdateScreenInfo(sptr<ScreenInfo> info) const;
private:
    // No more methods or variables can be defined here.
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_LITE_H