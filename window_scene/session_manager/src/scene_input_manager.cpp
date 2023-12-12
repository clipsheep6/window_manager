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

#include "scene_input_manager.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "root_scene.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"
#include "scene_session_markdirty.h"
#include <cstdint>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include "input_manager.h"
namespace OHOS {
namespace Rosen{

namespace{
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW,
"SceneInputManager"};
const std::string SCENE_INPUT_MANAGER_THREAD = "SceneInputManager";
std::recursive_mutex g_instanceMutex;

constexpr float DIRECTION0 = 0;
constexpr float DIRECTION90 = 90;
constexpr float DIRECTION180 = 180;
constexpr float DIRECTION270 = 270;

MMI::Direction ConvertDegreeToMMIRotation(float degree, MMI::DisplayMode displayMode){
    MMI::Direction out = MMI::DIRECTION0;
    if(NearEqual(degree, DIRECTION0)){
        return MMI::DIRECTION0;
    }
    if(NearEqual(degree, DIRECTION90)){
        return MMI::DIRECTION90;
    }
    if(NearEqual(degree, DIRECTION180)){
        return MMI::DIRECTION180;
    }
    if(NearEqual(degree, DIRECTION270)){
        return MMI::DIRECTION270;
    }
    if(displayMode == MMI::DisplayMode::FULL){
        switch(out){
            case MMI::DIRECTION0: out = MMI::DIRECTION90; break;
            case MMI::DIRECTION90: out = MMI::DIRECTION180; break;
            case MMI::DIRECTION180: out = MMI::DIRECTION270; break;
            case MMI::DIRECTION270: out = MMI::DIRECTION0; break;
        }
    }
    return out;
}








}//namespace

SceneInputManager& SceneInputManager::GetInstance(){
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static SceneInputManager *instance = nullptr;
    if(instance == nullptr){
        instance = new SceneInputManager();
        instance->Init();
    }
    return *instance;
}

SceneInputManager::SceneInputManager(){
    
}

std::string DumpRect(const std::vector<MMI::Rect>& rects)
{
    std::string rectStr = "";
    for(auto rect : rects){
        rectStr = rectStr + " hot : [ " + std::to_string(rect.x) +" , " + std::to_string(rect.y) + 
        " , " + std::to_string(rect.width) + " , " + std::to_string(rect.height) + "]"; 
    }
    return rectStr;
}

std::string DumpWindowInfo(const MMI::WindowInfo& info)
{
    std::string infoStr = "windowInfo:";
    infoStr = infoStr + "windowId: " + std::to_string(info.id) + " pid : " + std::to_string(info.pid) +
    " uid: " + std::to_string(info.uid) + " area: [ " + std::to_string(info.area.x) + " , " + std::to_string(info.area.y) + 
    " , " + std::to_string(info.area.width) + " , " + std::to_string(info.area.height) + "] agentWindowId:" + std::to_string(info.agentWindowId) + " flags:" + std::to_string(info.flags)  +" displayId: " + std::to_string(info.displayId) +
    " action: " + std::to_string(static_cast<int>(info.action)) + " zOrder: " + std::to_string(info.zOrder);

    infoStr = infoStr + DumpRect(info.defaultHotAreas);
    return infoStr;
}


void SceneInputManager::Init(){
    sceneSessionDirty_ = std::make_shared<SceneSessionDirty>();
    if (sceneSessionDirty_) {
        sceneSessionDirty_->Init();
    }
}

std::string DumpDisplayInfo(const MMI::DisplayInfo& info)
{
    std::string infoStr =  "DisplayInfo: ";
    infoStr = infoStr + " id: " + std::to_string(info.id) + " x: " + std::to_string(info.x) +
    "y: " + std::to_string(info.y) + " width: " + std::to_string(info.width) + 
    "height: " + std::to_string(info.height) + " dpi: " + std::to_string(info.dpi) + " name:" + info.name +
    " uniq: " + info.uniq + " displayMode: " + std::to_string(static_cast<int>(info.displayMode)) +
    " direction : " + std::to_string( static_cast<int>(info.direction));
    return infoStr;
}

void SceneInputManager::NotifyFullInfo(){
    std::unordered_map<ScreenId, ScreenProperty> screensProperties;
    ScreenSessionManagerClient::GetInstance().GetAllScreensProperties(screensProperties);
  //
    auto displayMode = Rosen::ScreenSessionManagerClient::GetInstance().GetFoldDisplayMode();
    auto constructDisplayInfos = 
    [&](std::vector<MMI::DisplayInfo>& displayInfos){
    for(const auto& iter: screensProperties){
        auto screenId = iter.first;
        auto screenProperty = iter.second;
        MMI::DisplayInfo displayInfo = {
            .id = screenId,
            .x = screenProperty.GetOffsetX(),
            .y = screenProperty.GetOffsetY(),
            //
            .width = screenProperty.GetBounds().rect_.GetWidth(),
            .height = screenProperty.GetBounds().rect_.GetHeight(),



            .dpi = screenProperty.GetDensity() *  DOT_PER_INCH,
            .name = "display" + std::to_string(screenId),
            .uniq = "default" + std::to_string(screenId),
            .displayMode = static_cast<MMI::DisplayMode>(displayMode),
            .direction = 
                    ConvertDegreeToMMIRotation( screenProperty.GetRotation(), static_cast<MMI::DisplayMode>(displayMode)  ) };
        displayInfos.emplace_back(displayInfo);

        }
    };
    std::vector<MMI::DisplayInfo> displayInfos;
    constructDisplayInfos(displayInfos);

    std::vector<MMI::WindowInfo> windowsInfo;

    int firstWidth = 0; 
    int firstHeight = 0;
    if(!displayInfos.empty()){
        firstWidth = displayInfos[0].width;
        firstHeight = displayInfos[0].height;
    }
    std::vector<MMI::WindowInfo> windowInfoList;
    if (sceneSessionDirty_) {
        sceneSessionDirty_->GetFullWindowInfoList(windowInfoList);
    }

    int32_t focusId = Rosen::SceneSessionManager::GetInstance().GetFocusedSession();
    MMI::DisplayGroupInfo displayGroupInfo = {
        .width = firstWidth,
        .height = firstHeight,
        .focusWindowId = focusId,
        .windowsInfo = windowInfoList,
        .displaysInfo = displayInfos,
        };
        for(auto& displayInfo : displayGroupInfo.displaysInfo){
            WLOG_E("lkf ------——%s", DumpDisplayInfo(displayInfo).c_str());
        }
        for(auto& windowInfo : displayGroupInfo.windowsInfo){
            WLOG_E("lkf------%s", DumpWindowInfo(windowInfo).c_str());
        }

        MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);

        //WLOGFI("lkf UpdateDisplayInfo end, with width %{public}d height:%{public}d, windows num:%{public}lu, display num:%{public}lu, focus id:%{public}d", 
        //firstWidth, firstHeight, windowsInfo.size(), displayInfos.size(), focusId);
} 


void SceneInputManager::NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession, const WindowUpdateType& type)
{
    if (sceneSessionDirty_) {
        sceneSessionDirty_->NotifyWindowInfoChange(sceneSession, type);
    }
}

void SceneInputManager::NotifyChangeInfo(){
    //std::unordered_map<ScreenId, std::vector<MMI::WindowInfo>> screenId2Windows;
    std::map<uint64_t, std::vector<MMI::WindowInfo>> screenId2Windows;
    if (sceneSessionDirty_) {
        sceneSessionDirty_->GetIncrementWindowInfoList(screenId2Windows);
    }


    
    
    
    
    
    
    
    
    
    
    
    
    
  
    std::unordered_map<ScreenId, ScreenProperty> screensProperties;
    Rosen::ScreenSessionManagerClient::GetInstance().GetAllScreensProperties(screensProperties);
    if(screensProperties.size() < 1 ){
        return;
    }
    //screenId2Windows[screensProperties.begin()->first] = windowInfoList;

    for(auto& iter : screenId2Windows){
        auto displayId = iter.first;
        auto& windowInfos = iter.second;
        for(auto& windowInfo : windowInfos){
            WLOG_E("lkfu----%s", DumpWindowInfo(windowInfo).c_str());
        }

        int32_t foucsId = Rosen::SceneSessionManager::GetInstance().GetFocusedSession();
        MMI::InputManager::GetInstance()->UpdateWindowInfo(displayId, windowInfos, foucsId);
       // WLOGFI("lkfu UpdateWindowInfo end, with display id:%{public}d, windows num :%{public}lu, focus id :%{public}d", static_cast<int>(displayId), windowInfos.size(), foucsId);
    }
}


void SceneInputManager::NotifyMMIDisplayInfo(){



    if(1){
        NotifyFullInfo();
        return;
    }
    NotifyChangeInfo();
}

}//Rose
}//OHOS