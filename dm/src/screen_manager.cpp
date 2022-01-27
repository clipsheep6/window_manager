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

#include "screen_manager.h"
#include "window_manager_hilog.h"
#include "display_manager_adapter.h"


#include <map>
#include <vector>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "ScreenManager"};
}
class ScreenManager::Impl : public RefBase {
friend class ScreenManager;
private:
    Impl() = default;
    ~Impl() = default;

    std::map<ScreenId, sptr<Screen>> monitorMap_;
};
WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManager)

ScreenManager::ScreenManager()
{
    pImpl_ = new Impl();
}

ScreenManager::~ScreenManager()
{
}

sptr<Screen> ScreenManager::GetScreenById(ScreenId id)
{
    return nullptr;
}

std::vector<const sptr<Screen>> ScreenManager::GetAllScreens()
{
    std::vector<const sptr<Screen>> res;
    return res;
}

void ScreenManager::RegisterScreenChangeListener(sptr<IScreenChangeListener> listener)
{
}

sptr<ScreenGroup> ScreenManager::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    return nullptr;
}

sptr<ScreenGroup> ScreenManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId)
{
    return nullptr;
}

sptr<ScreenGroup> ScreenManager::AddMirror(ScreenId mainScreenId, ScreenId mirrorScreenId)
{
    DMError result = SingletonContainer::Get<DisplayManagerAdapter>().AddMirror(mainScreenId, mirrorScreenId);
    if (result == DMError::DM_OK) {
        WLOGFI("AddMirror::Successful");
    }
    return nullptr;
}

ScreenId ScreenManager::CreateVirtualScreen(VirtualScreenOption option)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().CreateVirtualScreen(option);
}

DMError ScreenManager::DestroyVirtualScreen(ScreenId screenId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().DestroyVirtualScreen(screenId);
}

DMError ScreenManager::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().SetScreenActiveMode(screenId, modeId);
}

std::vector<RSScreenModeInfo> ScreenManager::GetScreenSupportedModes(ScreenId screenId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetScreenSupportedModes(screenId);
}
} // namespace OHOS::Rosen