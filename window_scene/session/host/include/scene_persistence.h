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

#include <string>

#include <refbase.h>

#include "common/include/task_scheduler.h"
#include "session/host/include/ws_ffrt_helper.h"
#include "session/host/include/ws_snapshot_helper.h"

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class ScenePersistence : public RefBase {
public:
    ScenePersistence(const std::string& bundleName, int32_t persistentId, SnapshotStatus capacity = defaultCapacity);
    virtual ~ScenePersistence();

    static bool CreateSnapshotDir(const std::string& directory);
    static bool CreateUpdatedIconDir(const std::string& directory);

    void SetSnapshotCapacity(SnapshotStatus capacity);
    static void InitAstcEnabled();
    static bool IsAstcEnabled();
    void SetHasSnapshot(bool hasSnapshot, SnapshotStatus key = defaultStatus);
    void SetHasSnapshotTmp(bool hasSnapshot);
    bool HasSnapshot() const;
    bool HasSnapshot(SnapshotStatus key, bool specialType = false) const;
    void ClearSnapshot(SnapshotStatus key);
    bool IsSnapshotExisted(SnapshotStatus key = defaultStatus);
    std::string GetSnapshotFilePath(SnapshotStatus& key, bool useKey = false, bool specialType = false);
    std::pair<uint32_t, uint32_t> GetSnapshotSize(SnapshotStatus key = defaultStatus, bool specialType = false) const;
    void SetSnapshotSize(SnapshotStatus key, bool specialType, std::pair<uint32_t, uint32_t> size);
    std::shared_ptr<WSFFRTHelper> GetSnapshotFfrtHelper() const;

    void SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap,
        const std::function<void()> resetSnapshotCallback = []() {}, SnapshotStatus key = defaultStatus,
        DisplayOrientation rotate = DisplayOrientation::PORTRAIT, bool specialType = false);
    bool IsSavingSnapshot(SnapshotStatus key = defaultStatus, bool specialType = false);
    void SetIsSavingSnapshot(SnapshotStatus key, bool specialType, bool isSavingSnapshot);
    void ResetSnapshotCache();
    void RenameSnapshotFromOldPersistentId(const int32_t& oldPersistentId);
    void RenameSnapshotFromOldPersistentId(const int32_t& oldPersistentId, SnapshotStatus key);

    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::string GetUpdatedIconPath() const;
    std::shared_ptr<Media::PixelMap> GetLocalSnapshotPixelMap(const float oriScale, const float newScale,
        SnapshotStatus key = defaultStatus, bool specialType = false);
    DisplayOrientation rotate_[SCREEN_COUNT][ORIENTATION_COUNT] = {};

private:
    static std::string snapshotDirectory_;
    std::string bundleName_;
    int32_t persistentId_;
    SnapshotStatus capacity_;
    std::string snapshotPath_[SCREEN_COUNT][ORIENTATION_COUNT];
    std::string snapshotTmpPath_;
    std::pair<uint32_t, uint32_t> snapshotSize_[SCREEN_COUNT][ORIENTATION_COUNT];
    std::pair<uint32_t, uint32_t> snapshotTmpSize_;
    bool hasSnapshot_[SCREEN_COUNT][ORIENTATION_COUNT] = {};
    bool hasSnapshotTmp_;

    static std::string updatedIconDirectory_;
    std::string updatedIconPath_;
    static bool isAstcEnabled_;

    std::atomic<int> savingSnapshotSum_ { 0 };
    std::atomic<bool> isSavingSnapshot_[SCREEN_COUNT][ORIENTATION_COUNT] = {};
    std::atomic<bool> isSavingSnapshotTmp_ {false};

    static std::shared_ptr<WSFFRTHelper> snapshotFfrtHelper_;
    mutable std::mutex savingSnapshotMutex_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H
