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

#ifndef OHOS_ROSEN_WINDOW_PERSISTENT_STORAGE_H
#define OHOS_ROSEN_WINDOW_PERSISTENT_STORAGE_H

#include "preferences.h"
#include "preferences_helper.h"

#include "window_manager_hilog.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
using PersistentPerference = NativePreferences::Preferences;
class PersistentStorage {
public:
    PersistentStorage() = default;
    ~PersistentStorage() = default;

    template <typename T>
    static void Insert(const std::string& key, const T& value, PersistentStorageType storageType)
    {
        auto pref = GetPreference(storageType);
        if (!pref) {
            WLOG_E("[PersistentStorage] Preferences is nullptr");
            return;
        }
        switch (storageType) {
            case PersistentStorageType::ASPECT_RATIO: {
                pref->PutFloat(key, value);
                WLOG_D("[PersistentStorage] Insert aspect ratio, key %{public}s, value %{public}f",
                    key.c_str(), static_cast<float>(value));
                break;
            }
            case PersistentStorageType::MAXIMIZE_STATE: {
                pref->PutInt(key, value);
                WLOG_D("[PersistentStorage] Insert Maximize state, key %{public}s, value %{public}d",
                    key.c_str(), static_cast<int>(value));
                break;
            }
            default:
                WLOG_W("[PersistentStorage] Unknown storage type!");
        }
        pref->Flush();
    }

    template <typename T>
    static void Get(const std::string& key, T& value, PersistentStorageType storageType)
    {
        auto pref = GetPreference(storageType);
        if (!pref) {
            WLOG_E("[PersistentStorage] Preferences is nullptr");
            return;
        }
        switch (storageType) {
            case PersistentStorageType::ASPECT_RATIO: {
                value = pref->GetFloat(key);
                WLOG_D("[PersistentStorage] Get aspect ratio, key: %{public}s, value:%{public}f",
                    key.c_str(), static_cast<float>(value));
                break;
            }
            case PersistentStorageType::MAXIMIZE_STATE: {
                value = pref->GetInt(key);
                WLOG_D("[PersistentStorage] Get Maximize state, key: %{public}s, value:%{public}d",
                    key.c_str(), static_cast<int>(value));
                break;
            }
            default:
                WLOG_W("[PersistentStorage] Unknown storage type!");
        }
    }

    static bool HasKey(const std::string& key, PersistentStorageType storageType);
    static void Delete(const std::string& key, PersistentStorageType storageType);

private:
    static std::shared_ptr<PersistentPerference> GetPreference(PersistentStorageType storageType);
    static std::map<PersistentStorageType, std::string> storagePath_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_PERSISTENT_STORAGE_H