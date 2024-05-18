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

#include "screen_session_publish.h"

#include <common_event_manager.h>
#include <common_event_support.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
const std::string CAST_BUNDLE_NAME = "com.huawei.hmos.huaweicast";
const std::string CAST_PLUG_IN_FLAG_DATA = "1";
const std::string CAST_PLUG_OUT_FLAG_DATA = "2";
constexpr int32_t PUBLISH_SUCCESS = 0;
constexpr int32_t PUBLISH_FAILURE = -1;
constexpr int32_t TRANS_CODE_CAST_PLUG_IN = 1;
constexpr int32_t TRANS_CODE_CAST_PLUG_OUT = 2;

std::map<std::string, sptr<EventFwk::Want>> ScreenSessionPublish::cesWantMap_ = {
    {EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_LOW, new (std::nothrow) EventFwk::Want()},
};

ScreenSessionPublish &ScreenSessionPublish::GetInstance()
{
    static ScreenSessionPublish screenSessionPublish;
    return screenSessionPublish;
}

void ScreenSessionPublish::InitPublishEvents()
{
    if (publishInfo_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "ScreenSessionPublish has been initialized");
        return;
    }
    publishInfo_ = new (std::nothrow) EventFwk::CommonEventPublishInfo();
    if (publishInfo_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "publishInfo new failed");
        return;
    }
    publishInfo_->SetOrdered(false);
    for (auto item : cesWantMap_) {
        item.second->SetAction(item.first);
    }
}

int32_t ScreenSessionPublish::PublishEvents(EventFwk::CommonEventData &eventData, std::string bundleName)
{
    if (publishInfo_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "publishInfo is nullptr");
        return PUBLISH_FAILURE;
    }
    if (bundleName != "") {
        publishInfo_->SetBundleName(bundleName);
    }
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(eventData, *publishInfo_, nullptr);
    if (!result) {
        TLOGE(WmsLogTag::DMS, "PublishCommonEvent failed");
        return PUBLISH_FAILURE;
    }
    TLOGI(WmsLogTag::DMS, "PublishCommonEvent succeed");
    return PUBLISH_SUCCESS;
}

void ScreenSessionPublish::PublishCastPluggedEvent(bool isEnable)
{
    TLOGI(WmsLogTag::DMS, "Start to publish cast plugged event");
    EventFwk::CommonEventData eventData;
    if (isEnable) {
        eventData.SetCode(TRANS_CODE_CAST_PLUG_IN);
        eventData.SetData(CAST_PLUG_IN_FLAG_DATA);
    } else {
        eventData.SetCode(TRANS_CODE_CAST_PLUG_OUT);
        eventData.SetData(CAST_PLUG_OUT_FLAG_DATA);
    }
    eventData.SetWant(*cesWantMap_[EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_LOW]);
    int32_t ret = PublishEvents(eventData, CAST_BUNDLE_NAME);
    if (ret != PUBLISH_SUCCESS) {
        TLOGE(WmsLogTag::DMS, "PublishCastPluggedEvent failed");
        return;
    }
    TLOGI(WmsLogTag::DMS, "End of publish cast plugged event");
}

void ScreenSessionPublish::PublishCastPlugInEvent()
{
    TLOGI(WmsLogTag::DMS, "Start to publish cast plug in event");
    PublishCastPluggedEvent(true);
}

void ScreenSessionPublish::PublishCastPlugOutEvent()
{
    TLOGI(WmsLogTag::DMS, "Start to publish cast plug out event");
    PublishCastPluggedEvent(false);
}
} // namespace OHOS::Rosen