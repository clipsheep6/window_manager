/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_SCREEN_MANAGER_H
#define FOUNDATION_DM_SCREEN_MANAGER_H

#include <refbase.h>
#include "screen.h"
#include "dm_common.h"
#include "screen_group.h"
#include "wm_single_instance.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class ScreenManager : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenManager);
public:
    class IScreenListener : public virtual RefBase {
    public:
        virtual void OnConnect(ScreenId) = 0;
        virtual void OnDisconnect(ScreenId) = 0;
        virtual void OnChange(const std::vector<ScreenId>&, ScreenChangeEvent) = 0;
    };

    sptr<Screen> GetScreenById(ScreenId screenId);
    sptr<ScreenGroup> GetScreenGroupById(ScreenId screenId);
    std::vector<sptr<Screen>> GetAllScreens();

    bool RegisterScreenListener(sptr<IScreenListener> listener);
    bool UnregisterScreenListener(sptr<IScreenListener> listener);
    ScreenId MakeExpand(const std::vector<ExpandOption>& options);
    ScreenId MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId);
    ScreenId CreateVirtualScreen(VirtualScreenOption option);
    DMError DestroyVirtualScreen(ScreenId screenId);

private:
    ScreenManager();
    ~ScreenManager();

    class ScreenManagerListener;
    sptr<ScreenManagerListener> screenManagerListener_;
    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_MANAGER_H