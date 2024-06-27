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

#include "publish/scene_event_publish.h"
#include <sstream>
#include <want.h>
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "window_manager_hilog.h"
#include "matching_skills.h"
#include "common_event_manager.h"

namespace OHOS::Rosen {
static const std::string TIME_OUT("timeout");
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };

void SceneEventPublish::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << data.GetData() << std::endl;
    s = oss.str();
    valueReady_ = true;
    cv_.notify_all();
}

std::string SceneEventPublish::GetDebugDumpInfo(std::chrono::milliseconds const &time)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (cv_.wait_for(lock, time, [&] { return valueReady_; })) {
        return s;
    }
    return TIME_OUT; // 超时返回
}

WSError SceneEventPublish::Publish(std::string cmd)
{
    valueReady_ = false;
    static const std::string scbDebugEventListenerName = "com.ohos.sceneboard.debug.event.listener";
    AAFwk::Want want;
    want.SetAction(scbDebugEventListenerName);

    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    commonEventData.SetCode(0);
    commonEventData.SetData(cmd);

    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetSticky(false);
    publishInfo.SetOrdered(false);

    // publish the common event
    bool ret = EventFwk::CommonEventManager::PublishCommonEvent(commonEventData, publishInfo, nullptr);
    if (!ret) {
        TLOGE(WmsLogTag::WMS_FOCUS, "publish debug event to scene error.");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    return WSError::WS_OK;
}

void SceneEventPublish::Subscribe(std::shared_ptr<SceneEventPublish>& scbSubscriber)
{
    static const std::string scbDebugEventResponseName = "com.ohos.sceneboard.debug.event.response";

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(scbDebugEventResponseName);

    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);

    if (scbSubscriber == nullptr) {
        scbSubscriber = std::make_shared<SceneEventPublish>(subscribeInfo);
    }

    EventFwk::CommonEventManager::SubscribeCommonEvent(scbSubscriber);
}

void SceneEventPublish::UnSubscribe(std::shared_ptr<SceneEventPublish>& scbSubscriber)
{
    if (scbSubscriber) {
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(scbSubscriber);
    }
}

std::string SceneEventPublish::JoinCommands(const std::vector<std::string>& params, int size)
{
    std::string cmd;
    for (int i = 1; i < size; i++) { // 从1开始，0为-b
        cmd += params[i];
        cmd += ' ';
    }
    return cmd;
}

} // namespace OHOS::Rosen