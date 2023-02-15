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

#ifndef OHOS_ROSEN_WINDOW_EXTENSION_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_EXTENSION_SESSION_MANAGER_H

#include <iremote_object.h>
#include <mutex>
#include <map>
#include <refbase.h>
#include <unistd.h>

#include "wm_single_instance.h"
#include "ws_common.h"
namespace OHOS::Rosen {
class ExtensionSession;

class ExtensionSessionManager {
WM_DECLARE_SINGLE_INSTANCE(ExtensionSessionManager)
public:
    sptr<ExtensionSession> RequestExtensionSession(const WindowSession::AbilityInfo& abilityInfo);
    WSError RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession);
    WSError RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession);
    WSError RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession);

private:

    std::recursive_mutex mutex_;
    int pid_ = getpid();
    std::atomic<uint32_t> sessionId_ = INVALID_SESSION_ID;
    std::map<uint32_t, std::pair<sptr<ExtensionSession>, sptr<IRemoteObject>>> abilityExtensionMap_;
};
}
#endif // OHOS_ROSEN_WINDOW_EXTENSION_SESSION_MANAGER_H
