/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "lru.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
bool LRUCache::get(uint32_t key)
{
    if(cacheMap.find(key) == cacheMap.end()) {
        return false;
    }
    cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
    return true;
}

uint32_t LRUCache::put(uint32_t key)
{
    uint32_t lastKey = -1;
    if (!get(key)) {
        if (cacheList.size() >= capacity_) {
            lastKey = cacheList.back();
            cacheMap.erase(lastKey);
            cacheList.pop_back();
        }
        cacheList.push_front(key);
        cacheMap[key] = cacheList.begin();
    }
    return lastKey;
}
} // namespace OHOS::Rosen
