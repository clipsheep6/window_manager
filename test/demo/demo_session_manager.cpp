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

#include "session_manager.h"
#include <unistd.h>
#include <iostream>
#include <memory>
#include "session_manager_service_proxy.h"
#include "display_manager.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    // SessionManager& sessionManager = SessionManager::GetInstance();
    /*
    sessionManager.Init();
    usleep(10 * 1000);
    sptr<IScreenSessionManager> screenSessionManagerProxy = sessionManager.GetScreenSessionManagerProxy();

    if (!screenSessionManagerProxy) {
        std::cout << "Get screenSessionManagerProxy: nullptr" << std::endl;
        return 0;
    }

    sptr<OHOS::Rosen::DisplayInfo> displayInfo = screenSessionManagerProxy->GetDefaultDisplayInfo();
    if (!displayInfo) {
        std::cout << "Get default display info failed." << std::endl;
        return 0;
    }

    std::cout << "Name: " << displayInfo->GetName() << std::endl;
    std::cout << "Id: " << displayInfo->GetDisplayId() << std::endl;
    std::cout << "Width: " << displayInfo->GetWidth() << std::endl;
    std::cout << "Height: " << displayInfo->GetHeight() << std::endl;

    std::cout << "    ----------------------    " << std::endl;
*/
/*    DisplayManager& dm = DisplayManager::GetInstance();

    auto display = dm.GetDefaultDisplay();
    if (display) {
        std::cout << "Name: " << display->GetName() << std::endl;
        std::cout << "Id: " << display->GetId() << std::endl;
        std::cout << "Width: " << display->GetWidth() << std::endl;
        std::cout << "Height: " << display->GetHeight() << std::endl;
    }
*/

 auto& dms = OHOS::Rosen::DisplayManager::GetInstance();
    auto displays = dms.GetDefaultDisplay();
    std::cout << "ID:" << displays->GetId() << std::endl;
    std::cout << "Width:" << displays->GetWidth() << std::endl;
    std::cout << "Height:" << displays->GetHeight() << std::endl;
    std::cout << "RefreshRate:" << displays->GetRefreshRate() << std::endl;
    std::cout << "ScreenId:" << displays->GetScreenId() << std::endl;
    std::cout << "Rotation:" <<  static_cast<int>(displays->GetRotation()) << std::endl;
    std::cout << "Orientation:" <<  static_cast<int>(displays->GetOrientation()) << std::endl;
    std::cout << "VirtualPixelRatio:" << displays->GetVirtualPixelRatio() << std::endl;
    std::cout << "Dpi:" << displays->GetDpi() << std::endl;
  
    // while (displays.empty()) {
    //     printf("displays is empty, retry to get displays\n");
    //     displays = displays.GetId();
    //     usleep(30 * 1000);
    // }

    return 0;
}