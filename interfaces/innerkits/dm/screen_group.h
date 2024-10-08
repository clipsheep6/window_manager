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

#ifndef FOUNDATION_DM_SCREEN_GROUP_H
#define FOUNDATION_DM_SCREEN_GROUP_H

#include <refbase.h>
#include <vector>
#include "screen.h"

namespace OHOS::Rosen {
class ScreenGroupInfo;

class ScreenGroup : public Screen {
friend class ScreenManager;
public:
    ~ScreenGroup();
    ScreenGroup(const ScreenGroup&) = delete;
    ScreenGroup(ScreenGroup&&) = delete;
    ScreenGroup& operator=(const ScreenGroup&) = delete;
    ScreenGroup& operator=(ScreenGroup&&) = delete;

    /**
     * @brief Get the screenCombination type of the screen group.
     *
     * @return ScreenCombination type of the screen group.
     */
    ScreenCombination GetCombination() const;

    /**
     * @brief Get the child screen ids.
     *
     * @return Child screen ids.
     */
    std::vector<ScreenId> GetChildIds() const;

    /**
     * @brief Get the child screen positions.
     *
     * @return Child screen positions.
     */
    std::vector<Point> GetChildPositions() const;

private:
    // No more methods or variables can be defined here.
    explicit ScreenGroup(sptr<ScreenGroupInfo> info);
    void UpdateScreenGroupInfo(sptr<ScreenGroupInfo> info) const;
    void UpdateScreenGroupInfo() const;
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_GROUP_H