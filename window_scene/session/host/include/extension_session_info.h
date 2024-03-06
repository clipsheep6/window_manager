/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_INFO_H
#define OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_INFO_H

#include "refbase.h"

namespace OHOS::Rosen {
class ExtensionSessionInfo : public RefBase {
public:
    ExtensionSessionInfo() = default;
    ~ExtensionSessionInfo() = default;

    void SetParentId(int32_t parentId);
    void SetPersistentId(int32_t persistentId);
    void SetVisibility(bool isVisible);
    void SetWaterMark(bool isEnable);

    int32_t GetParentId() const;
    int32_t GetPersistenId() const;
    bool GetVisibility() const;
    bool GetWaterMark() const;

private:
    int32_t parentId_ = { -1 };
    int32_t persistentId_ = { -1 };
    bool isVisible_ = { false };
    bool isWaterMarkEnable_ = { false };
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_EXTENSION_SESSION_INFO_H
