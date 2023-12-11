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

#include <hitrace_meter.h>
#include <image_packer.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScenePersistence" };
constexpr const char* UNDERLINE_SEPARATOR = "_";
constexpr const char* IMAGE_SUFFIX = ".png";
constexpr uint8_t IMAGE_QUALITY = 100;
const std::string SNAPSHOT_THREAD = "SnapshotThread";
} // namespace

std::string ScenePersistence::snapshotDirectory_;
std::string ScenePersistence::updatedIconDirectory_;
std::shared_ptr<TaskScheduler> ScenePersistence::snapshotScheduler_;

bool ScenePersistence::CreateSnapshotDir(const std::string& directory)
{
    snapshotDirectory_ = directory + "/SceneSnapShot/";
    if (mkdir(snapshotDirectory_.c_str(), S_IRWXU)) {
        WLOGFD("mkdir failed or the directory already exists");
        return false;
    }
    return true;
}

bool ScenePersistence::CreateUpdatedIconDir(const std::string& directory)
{
    updatedIconDirectory_ = directory + "/UpdatedIcon/";
    if (mkdir(updatedIconDirectory_.c_str(), S_IRWXU)) {
        WLOGFD("mkdir failed or the directory already exists");
        return false;
    }
    return true;
}

ScenePersistence::ScenePersistence(const std::string& bundleName, const int32_t& persistentId)
{
    uint32_t fileID = static_cast<uint32_t>(persistentId) & 0x3fffffff;
    snapshotPath_ = snapshotDirectory_ + bundleName + UNDERLINE_SEPARATOR + std::to_string(fileID) + IMAGE_SUFFIX;
    updatedIconPath_ = updatedIconDirectory_ + bundleName + IMAGE_SUFFIX;
    if (snapshotScheduler_ == nullptr) {
        snapshotScheduler_ = std::make_shared<TaskScheduler>(SNAPSHOT_THREAD);
    }
}

void ScenePersistence::SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    auto task = [weakThis = wptr(this), pixelMap]() {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr || pixelMap == nullptr ||
            scenePersistence->snapshotPath_.find('/') == std::string::npos) {
            WLOGFE("scenePersistence is%{public}s nullptr, pixelMap is%{public}s nullptr",
                scenePersistence == nullptr ? "" : " not", pixelMap == nullptr ? "" : " not");
            return;
        }

        WLOGFD("Save snapshot begin");
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ScenePersistence:SaveSnapshot");
        OHOS::Media::ImagePacker imagePacker;
        OHOS::Media::PackOption option;
        option.format = "image/png";
        option.quality = IMAGE_QUALITY;
        option.numberHint = 1;
        std::set<std::string> formats;
        if (imagePacker.GetSupportedFormats(formats)) {
            WLOGFE("Failed to get supported formats");
            return;
        }

        if (remove(scenePersistence->snapshotPath_.c_str())) {
            WLOGFE("Failed to delete old file");
        }
        scenePersistence->snapshotSize_ = { pixelMap->GetWidth(), pixelMap->GetHeight() };
        imagePacker.StartPacking(scenePersistence->snapshotPath_, option);
        imagePacker.AddImage(*pixelMap);
        int64_t packedSize = 0;
        imagePacker.FinalizePacking(packedSize);
        WLOGFD("Save snapshot end, packed size %{public}" PRIu64, packedSize);
    };
    snapshotScheduler_->PostAsyncTask(task);
}

std::string ScenePersistence::GetSnapshotFilePath()
{
    auto task = [weakThis = wptr(this)]() -> std::string {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr) {
            WLOGFE("scenePersistence is nullptr");
            return "";
        }
        return scenePersistence->snapshotPath_;
    };
    return snapshotScheduler_->PostSyncTask(task);
}

void ScenePersistence::SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap == nullptr || updatedIconPath_.find('/') == std::string::npos) {
        return;
    }

    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = "image/png";
    option.quality = IMAGE_QUALITY;
    option.numberHint = 1;
    std::set<std::string> formats;
    if (imagePacker.GetSupportedFormats(formats)) {
        WLOGFE("Failed to get supported formats");
        return;
    }

    if (remove(updatedIconPath_.c_str())) {
        WLOGFD("Failed to delete old file");
    }
    imagePacker.StartPacking(GetUpdatedIconPath(), option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
    WLOGFD("SaveUpdatedIcon finished");
}

std::string ScenePersistence::GetUpdatedIconPath() const
{
    return updatedIconPath_;
}

std::pair<uint32_t, uint32_t> ScenePersistence::GetSnapshotSize() const
{
    return snapshotSize_;
}

bool ScenePersistence::IsSnapshotExisted() const
{
    struct stat buf;
    if (stat(snapshotPath_.c_str(), &buf)) {
        WLOGFD("Snapshot file %{public}s does not exist", snapshotPath_.c_str());
        return false;
    }
    return S_ISREG(buf.st_mode);
}

std::shared_ptr<Media::PixelMap> ScenePersistence::GetLocalSnapshotPixelMap(const float& oriScale,
    const float& newScale) const
{
    if (!IsSnapshotExisted()) {
        WLOGE("local snapshot pic is not existed");
        return nullptr;
    }

    uint32_t errorCode = 0;
    Media::SourceOptions sourceOpts;
    sourceOpts.formatHint = "image/png";
    auto imageSource = Media::ImageSource::CreateImageSource(snapshotPath_, sourceOpts, errorCode);
    if (!imageSource) {
        WLOGE("create image source fail, errCode : %{public}d", errorCode);
        return nullptr;
    }

    Media::ImageInfo info;
    int32_t decoderWidth = 0;
    int32_t decoderHeight = 0;
    errorCode = imageSource->GetImageInfo(info);
    if (errorCode == Media::SUCCESS) {
        decoderWidth = info.size.width;
        decoderHeight = info.size.height;
    }
    Media::DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = Media::PixelFormat::RGBA_8888;
    if (oriScale != 0 && decoderWidth > 0 && decoderHeight > 0) {
        decodeOpts.desiredSize.width = static_cast<int>(decoderWidth * newScale / oriScale);
        decodeOpts.desiredSize.height = static_cast<int>(decoderHeight * newScale / oriScale);
    }
    return imageSource->CreatePixelMap(decodeOpts, errorCode);
}
} // namespace OHOS::Rosen
