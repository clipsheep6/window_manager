/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "session/host/include/scene_persistence.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <unistd.h>

#include "constants.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScenePersistence" };
constexpr const char *UNDERLINE_SEPARATOR = "_";
constexpr int32_t IMAGE_QUALITY = 85;
constexpr mode_t MKDIR_MODE = 0740;
}

std::string ScenePersistence::strSCBPath;

ScenePersistence::ScenePersistence(const SessionInfo &info) : sessionInfo_(info)
{
    strPersistPath_ = strSCBPath + SNAPSHOT_DIR;
}

void ScenePersistence::SaveSnapshot(const std::shared_ptr<Media::PixelMap> &pixelMap)
{
    if (pixelMap == nullptr) {
        return;
    }
    if (!strPreSnapshotFile_.empty()) {
        RemovePixelMapFile(strPreSnapshotFile_);
        strPreSnapshotFile_.clear();
    }
    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = "image/jpeg";
    option.quality = IMAGE_QUALITY;
    option.numberHint = 1;

    std::set<std::string> formats;
    auto ret = imagePacker.GetSupportedFormats(formats);
    if (ret) {
        WLOGFE("get supported formats() error : %{public}u", ret);
        return;
    }
    imagePacker.StartPacking(GetSnapshotFilePath(), option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
    WLOGFE("save snapshot packedSize : %{public}" PRIu64 "", packedSize);
}

std::string ScenePersistence::GetSnapshotFilePath() const
{
    WLOGFD("get snapshot file path cur: %{public}s  pre : %{public}s", strCurSnapshotFile_.c_str(),
        strPreSnapshotFile_.c_str());
    return strPreSnapshotFile_.empty() ? strCurSnapshotFile_ : strPreSnapshotFile_;
}

bool ScenePersistence::IsDirExisted(const std::string &path)
{
    DIR *dp;
    if ((dp = opendir(path.c_str())) == NULL) {
        WLOGFE("%{public}s IsDirExist()  is not exist!", path.c_str());
        return false;
    }
    closedir(dp);
    return true;
}

bool ScenePersistence::IsSnapshotExisted()
{
    std::string strPath;
    strPreSnapshotFile_.empty() ? strPath = strCurSnapshotFile_ : strPath = strPreSnapshotFile_;
    struct stat buf = {};
    if (stat(strPath.c_str(), &buf) != 0) {
        WLOGFD("snapshot : %{public}s is not exist!", strPath.c_str());
        return false;
    }
    return S_ISREG(buf.st_mode);
}

void ScenePersistence::CreateSnapshotDir()
{
    if (!IsDirExisted(strPersistPath_)) {
        if (mkdir(strPersistPath_.c_str(), MKDIR_MODE) != 0) {
            WLOGFE("CreateSnapshotDir() errInfo : %{public}s", strerror(errno));
            return;
        }
    }
}

void ScenePersistence::SetPrePersistId(const uint8_t prePersistId)
{
    if (prePersistId != 0) {
        strPreSnapshotFile_ =
            strPersistPath_ + sessionInfo_.bundleName_ + UNDERLINE_SEPARATOR + std::to_string(sessionInfo_.prePersistentId_);
    }
}

void ScenePersistence::SetCurPersistId(const uint64_t persistId)
{
    strCurSnapshotFile_ =
        strPersistPath_ + sessionInfo_.bundleName_ + UNDERLINE_SEPARATOR + std::to_string(persistId & 0xff);
}

void ScenePersistence::RemovePixelMapFile(const std::string &inFilePath)
{
    if (access(inFilePath.c_str(), F_OK) == 0 && remove(inFilePath.c_str()) == 0) {
        WLOGFD("remove pixelmap file success");
    } else {
        WLOGFE("remove pixelmap file fail");
    }
}
}
