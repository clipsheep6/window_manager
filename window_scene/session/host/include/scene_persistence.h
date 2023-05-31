/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H

#include <mutex>
#include <string>
#include <vector>
#include <refbase.h>

#include "image_packer.h"
#include "interfaces/include/ws_common.h"

namespace OHOS::Rosen {
class ScenePersistence : public RefBase {
public:
    ScenePersistence(const SessionInfo &info);
    ~ScenePersistence() = default;

    bool IsSnapshotExisted();
    void CreateSnapshotDir();
    void SetCurPersistId(const uint64_t persistId);
    void SetPrePersistId(const uint8_t prePersistId);
    void RemovePixelMapFile(const std::string &inFilePath);
    void SaveSnapshot(const std::shared_ptr<Media::PixelMap> &pixelMap);
    std::string GetSnapshotFilePath() const;

    static std::string strSCBPath;

private:
    bool IsDirExisted(const std::string &path);

    const std::string SNAPSHOT_DIR = "/SceneSnapShot/";
    SessionInfo sessionInfo_;
    std::string strPersistPath_;
    std::string strCurSnapshotFile_;
    std::string strPreSnapshotFile_;
};
}
#endif
