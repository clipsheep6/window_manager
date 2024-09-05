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

#ifndef OHOS_WINDOW_SCENE_JS_RSS_SESSION_H
#define OHOS_WINDOW_SCENE_JS_RSS_SESSION_H

#include <map>
#include <list>
#include <mutex>
#include <memory>
#include <string>
#include <functional>

#include "refbase.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"
#include "nlohmann/json.hpp"
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
#include "res_sched_event_listener.h"
#endif

namespace OHOS::Rosen {
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
class RssEventListener : public OHOS::ResourceSchedule::ResSchedEventListener {
public:
    using OnRssEventCb = std::function<void(napi_env, napi_value, int32_t,
        std::unordered_map<std::string, std::string>)>;
    RssEventListener(napi_env env, napi_value callbackObj, OnRssEventCb callback);
    virtual ~RssEventListener() = default;
    void OnReceiveEvent(uint32_t eventType, uint32_t eventValue,
        std::unordered_map<std::string, std::string> extInfo) override;
private:
    static void ThreadSafeCallBack(napi_env env, napi_value js_cb, void *context, void *data);
    napi_threadsafe_function threadSafeFunction_ = nullptr;
    napi_env napiEnv_ = nullptr;
    OnRssEventCb eventCb_ = nullptr;
    std::shared_ptr<NativeReference> callbackRef_ = nullptr;
};

class RssSession {
public:
    static RssSession& GetInstance();

    struct RssSessionCbInfo {
        explicit RssSessionCbInfo(napi_env env)
            : nativeEnv(env) {}
        ~RssSessionCbInfo()
        {
            if (nativeEnv) {
                if (callback) {
                    napi_delete_reference(nativeEnv, callback);
                    callback = nullptr;
                }
                if (asyncWork) {
                    napi_delete_async_work(nativeEnv, asyncWork);
                    asyncWork = nullptr;
                }
            }
        }
        napi_ref callback = nullptr;
        napi_async_work asyncWork = nullptr;
        napi_deferred deferred = nullptr;
        napi_env nativeEnv = nullptr;
        std::unordered_map<std::string, std::string> extInfo;
    };

    using CallBackPair = std::pair<std::unique_ptr<NativeReference>, sptr<RssEventListener>>;
    // static function
    static napi_value RegisterRssData(napi_env env, napi_callback_info info);
    static napi_value UnregisterRssData(napi_env env, napi_callback_info info);

    static napi_value DealRssReply(napi_env env, const nlohmann::json& payload, const nlohmann::json& reply);

    void OnReceiveEvent(napi_env env, napi_value callbackObj, int32_t eventType,
        const std::unordered_map<std::string, std::string>& extInfo);
private:
    RssSession() = default;
    ~RssSession();
    napi_value RegisterRssDataCallback(napi_env env, napi_callback_info info);
    napi_value UnRegisterRssDataCallback(napi_env env, napi_callback_info info);
    bool CheckCallbackParam(napi_env env, napi_callback_info info, int32_t &eventType, napi_value *jsCallback);

    static void ParseMutex(const std::string& mutexStr, const nlohmann::json& payload, std::string& detailStr);
    static void CompleteCb(napi_env env, napi_status status, void* data);

    std::mutex jsCallbackMapLock_;
    std::map<std::string, std::list<CallBackPair>> jsCallBackMap_;
};
#endif
} // OHOS

#endif // OHOS_WINDOW_SCENE_JS_RSS_SESSION_H