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

#ifndef OHOS_SESSION_MANAGER_SCENE_INPUT_MANAGER_H
#define OHOS_SESSION_MANAGER_SCENE_INPUT_MANAGER_H

#include <string>
#include <vector>

#include "wm_single_instance.h"
#include "common/include/task_scheduler.h"

#include "../../../arkui/ace_engine/interfaces/inner_api/ace/ui_content.h"

#include "common/rs_vector4.h"
#include "session/host/include/scene_session.h"
#include "wm_common.h"
#include "common/rs_matrix3.h"

namespace OHOS{
namespace Rosen{
class SceneSessionDirty;
   
constexpr  int32_t DEFAULT_INVAL_VALUE = -1;

class SceneInputManager : public std::enable_shared_from_this<SceneInputManager>
{
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneInputManager)
public:
    void NotifyMMIDisplayInfo();
    void NotifyWindowInfoChange(const sptr<SceneSession>& scenenSession, const WindowUpdateType& type);
    void NotifyWindowIncoChangeFormSession(const sptr<SceneSession>& sceneSession);
protected:
    SceneInputManager();
    virtual ~SceneInputManager() = default;

private:
    void Init();
    
    void NotifyFullInfo();
    void NotifyChangeInfo();
    std::shared_ptr<SceneSessionDirty>  sceneSessionDirty_;
    int32_t currentUserId_ = DEFAULT_INVAL_VALUE;
};


}//Rosen
}//OHOS
#endif //OHOS_SESSION_MANAGER_SCENE_INPUT_MANAGER_H