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

#include "session/host/include/scene_persistence.h"

#include "image_packer.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScenePersistence" };
constexpr const char *UNDERLINE_SEPARATOR = "_";
constexpr uint8_t IMAGE_QUALITY = 85;
}

std::string ScenePersistence::strPersistPath_;
std::string ScenePersistence::strPersistUpdatedIconPath_;

ScenePersistence::ScenePersistence(const SessionInfo& info, const int32_t& persistentId)
{
    uint32_t fileID = static_cast<uint32_t>(persistentId) & 0x3fffffff;
    strSnapshotFile_ =
        strPersistPath_ + info.bundleName_ + UNDERLINE_SEPARATOR + std::to_string(fileID);
    strUpdatedIconPath_ = strPersistPath_ + info.bundleName_;
        
}

void ScenePersistence::SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap == nullptr || strSnapshotFile_.find('/') == std::string::npos) {
        return;
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
    snapshotSize_ = { pixelMap->GetWidth(), pixelMap->GetHeight() };
    WLOGFD("save snapshot packedSize : %{public}" PRIu64 "", packedSize);
}

std::string ScenePersistence::GetSnapshotFilePath() const
{
    return strSnapshotFile_;
}

void ScenePersistence::SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap == nullptr || strUpdatedIconPath_.find('/') == std::string::npos) {
        return;
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

    imagePacker.StartPacking(GetUpdatedIconPath(), option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
    WLOGFD("SaveUpdatedIcon finished");
}

std::string ScenePersistence::GetUpdatedIconPath() const
{
    return strUpdatedIconPath_;
}

std::pair<uint32_t, uint32_t> ScenePersistence::GetSnapshotSize() const
{
    return snapshotSize_;
}

bool ScenePersistence::IsSnapshotExisted() const
{
    struct stat buf = {};
    if (stat(strSnapshotFile_.c_str(), &buf) != 0) {
        WLOGFD("snapshot file : %{public}s is not exist!", strSnapshotFile_.c_str());
        return false;
    }
    return S_ISREG(buf.st_mode);
}
} // namespace OHOS::Rosen
