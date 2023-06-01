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
#include "zidl/screen_session_manager_proxy.h"
#include "iconsumer_surface.h"
#include "display_manager.h"
#include "screen_manager.h"
#include <surface.h>

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

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
    }

    std::cout << "    ----------------------    " << std::endl;

    DisplayManager& dm = DisplayManager::GetInstance();
    ScreenManager& sm = ScreenManager::GetInstance();  

    auto display = dm.GetDefaultDisplay();
    if (display) {
        std::cout << "Name: " << display->GetName() << std::endl;
        std::cout << "Id: " << display->GetId() << std::endl;
        std::cout << "screenId: " << display->GetScreenId() << std::endl;
        std::cout << "Width: " << display->GetWidth() << std::endl;
        std::cout << "Height: " << display->GetHeight() << std::endl;
    }

    cout << endl;
    VirtualScreenOption option;
    option.name_ = "myScreenId";
    option.width_ = 720;
    option.height_ = 1280;
    option.density_ = 1.5f;
    option.surface_ = nullptr;
    option.flags_ = true;
    option.isForShot_ = true;

    ScreenId screenId = sm.CreateVirtualScreen(option);
    cout << "ScreenId: " << (int)screenId << endl;

    sptr<IBufferProducer> bufferProducer = nullptr;
    auto surface = IConsumerSurface::Create();
    if (surface == nullptr) {
        cout << "CreateSurfaceAsConsumer fail" << endl;
    } else {
        bufferProducer = surface->GetProducer();
    }

    DMError ret;
    cout << endl;
    ret = sm.SetVirtualScreenSurface(screenId, surface);
    if (ret == DMError::DM_OK) {
        cout << "SetVirtualScreenSurface succeed!" << endl;
    } else {
        cout << "SetVirtualScreenSurface failed!" << endl;
    }

    cout << endl;
    ScreenId screenGroupId;
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(screenId);
    ret = sm.MakeMirror(1, mirrorIds, screenGroupId);
    if (ret == DMError::DM_OK) {
        cout << "MakeMirror succeed!" << endl;
    } else {
        cout << "MakeMirror failed!" << endl;
    }

    auto pixelmap = dm.GetScreenshot(0);
    cout <<"0: ";
    if (pixelmap == nullptr) {
        cout << "pixelmap == nullptr!" << endl;
    } else {
        cout << "ok!" << endl;
    }

    
    sleep(2);

    cout << endl;
    sptr<ScreenGroup> screenGroup = sm.GetScreenGroup(screenGroupId);
    if (screenGroup == nullptr) {
        cout << "GetScreenGroupInfoById screenGroup is nullptr!" << endl;
    } else {
        cout << "GetScreenGroupInfoById screenGroup ok!" << endl;
    }
    std::vector<ScreenId> ids = screenGroup->GetChildIds();
    // void
    sm.RemoveVirtualScreenFromGroup(ids);


    cout << endl;
    ret = sm.DestroyVirtualScreen(screenId);
    if (ret == DMError::DM_OK) {
        cout << "DestroyVirtualScreen succeed!" << endl;
    } else {
        cout << "DestroyVirtualScreen failed!" << endl;
    }

    return 0;
}