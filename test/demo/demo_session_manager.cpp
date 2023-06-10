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
#include "session_manager_service_proxy.h"
#include <unistd.h>
#include <iostream>
#include <memory>
#include "display_manager.h"
#include "screen_manager.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    SessionManager sessionManager;
    sessionManager.Init();
    sleep(1);

    auto remoteObject = sessionManager.GetRemoteObject();
    if (!remoteObject) {
        std::cout << "remoteObject is nullptr." << std::endl;
    }

    auto proxy = std::make_unique<SessionManagerServiceProxy>(remoteObject);

    if (!proxy) {
        std::cout << "proxy is nullptr." << std::endl;
    } else {
        int ret = proxy->GetValueById(1);
        std::cout << "ret value: " << ret << std::endl;
    }

    /*sptr<IScreenSessionManager> screenSessionManagerProxy = sessionManager.GetScreenSessionManagerProxy();

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

    std::cout << "    ----------------------    " << std::endl;*/
    std::cout << "    ------------display.cpp start ------------    " << std::endl;
    auto& dm = OHOS::Rosen::DisplayManager::GetInstance();
    auto displays = dm.GetDefaultDisplay();
    std::cout << "ID:" << displays->GetId() << std::endl;
    std::cout << "Width:" << displays->GetWidth() << std::endl;
    std::cout << "Height:" << displays->GetHeight() << std::endl;
    std::cout << "RefreshRate:" << displays->GetRefreshRate() << std::endl;
    std::cout << "ScreenId:" << displays->GetScreenId() << std::endl;
    std::cout << "Rotation:" <<  static_cast<int>(displays->GetRotation()) << std::endl;
    std::cout << "Orientation:" <<  static_cast<int>(displays->GetOrientation()) << std::endl;
    std::cout << "VirtualPixelRatio:" << displays->GetVirtualPixelRatio() << std::endl;
    std::cout << "Dpi:" << displays->GetDpi() << std::endl;

    std::cout << "    ------------display.cpp end ------------    " << std::endl;

    std::cout << "    ------------screen.cpp start ------------    " << std::endl;

    auto& sm = OHOS::Rosen::ScreenManager::GetInstance();
    auto screens = sm.GetScreenById(0);
    std::cout << "ID:" << screens->GetId() << std::endl;
    std::cout << "Width:" << screens->GetWidth() << std::endl;
    std::cout << "Height:" << screens->GetHeight() << std::endl;
    std::cout << "VirtualWidth:" << screens->GetVirtualWidth() << std::endl;
    std::cout << "VirtualHeight:" << screens->GetVirtualHeight() << std::endl;
    std::cout << "VirtualPixelRatio:" << screens->GetVirtualPixelRatio() << std::endl;
    std::cout << "Rotation:" <<  static_cast<int>(screens->GetRotation()) << std::endl;
    std::cout << "Orientation:" <<  static_cast<int>(screens->GetOrientation()) << std::endl;
    std::cout << "IsReal:" <<  static_cast<int>(screens->IsReal()) << std::endl;
    std::cout << "    ------------screen.cpp end ------------    " << std::endl;

    return 0;
}