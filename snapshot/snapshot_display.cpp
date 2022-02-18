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

#include <cstdio>
#include <unistd.h>
#include <refbase.h>
#include "screen.h"
#include "screen_manager.h"
#include "display_manager.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
    class MyScreenListener : public OHOS::Rosen::ScreenManager::IScreenListener {
        void OnConnect(ScreenId) {
        }
        void OnDisconnect(ScreenId) {
        }
        void OnChange(const std::vector<ScreenId>&, ScreenChangeEvent) {
        }
    };

    class MyDisplayListener : public OHOS::Rosen::DisplayManager::IDisplayListener {
    public:
        void OnCreate(DisplayId) {
        }
        void OnDestroy(DisplayId) {
        }
        void OnChange(DisplayId, DisplayChangeEvent) {
        }
    };
}

int main(int argc, char *argv[])
{
    //std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    DisplayManager::GetInstance().RegisterDisplayListener(new MyDisplayListener());
    ScreenManager::GetInstance().RegisterScreenListener(new MyScreenListener());
    auto screens = ScreenManager::GetInstance().GetAllScreens();
    Orientation orientation = static_cast<Orientation>(atoi(argv[1]));
    printf("orientation = %u\n", orientation);
    screens[0]->SetOrientation(orientation);
    printf("end orientation = %u\n", orientation);
    usleep(3E6);
    orientation = Orientation::UNSPECIFIED;
    printf("end orientation = %u\n", orientation); 
    screens[0]->SetOrientation(orientation);
    return -1;
}