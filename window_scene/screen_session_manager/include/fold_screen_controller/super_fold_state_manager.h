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

#ifndef OHOS_ROSEN_SUPER_FOLD_STATE_MANAGER_H
#define OHOS_ROSEN_SUPER_FOLD_STATE_MANAGER_H
  
#include <mutex>
#include <refbase.h>
#include <functional>
#include <map>
#include <vector>
#include <memory>
#include <utility>

#include "dm_common.h"
#include "wm_single_instance.h"

namespace OHOS {

namespace Rosen {

class SuperFoldStateManager final {
    WM_DECLARE_SINGLE_INSTANCE_BASE(SuperFoldStateManager)
public:
    SuperFoldStateManager();
    ~SuperFoldStateManager();

    void initStateManagerMap(SuperFoldStatus curState,
    SuperFoldStatusChangeEvents event,
    SuperFoldStatus nextState,
    std::function<void ()> action);

    void transferState(SuperFoldStatus nextState);

    void HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents events);

    SuperFoldStatus GetCurrentStatus();
private:
    SuperFoldStatus curState_;

    struct Transition {
        SuperFoldStatus nextState;
        std::function<void ()> action;
    };

    using transEvent = std::pair<SuperFoldStatus, SuperFoldStatusChangeEvents>;
    std::map<transEvent, Transition> stateManagerMap_;

    static void DoFoldToHalfFold();

    static void DoHalfFoldToFold();

    static void DoHalFoldToExpand();

    static void DoExpandToHalfFold();

    static void DoHalfFoldToKeyboard();

    static void DoKeyboardToHalfFold();

    static void DoHalfFoldToSoftKeyboard();

    static void DoSoftKeyboardToHalfFold();

    static void DoSoftKeyboardToKeyboard();
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SUPER_FOLD_STATE_MANAGER_H