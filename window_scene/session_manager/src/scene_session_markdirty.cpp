#include "scene_session_markdirty.h"
#include <memory>
#include <sstream>
#include <cinttypes>

#include "session_manager/include/scene_session_manager.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
//#include "scene_input_manager.h"
#include "session/host/include/scene_session.h"
#include "input_manager.h"

namespace OHOS::Rosen{

namespace {
constexpr float DIRECTION0 = 0 ;
constexpr float DIRECTION90 = 90 ;
constexpr float DIRECTION180 = 180 ;
constexpr float DIRECTION270 = 270 ;

static MMI::Direction ConvertDegreeToMMIRotation(float degree){
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
    return MMI::DIRECTION0;
  }
}


constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionDirty"};


struct windowinfochangetype{
    int32_t UNKNOW = 0;
    int32_t ADD = 0;
    int32_t DELETE = 0;
    int32_t CHANGE = 0;
    int32_t Num = 0;
};


std::map<int, windowinfochangetype> recordlogislose3442344;
static void printinputwindow(std::string str, const MMI::WindowInfo& e, std::map<int, windowinfochangetype>& recordlogislose344){
    if(recordlogislose344.find(e.id) != recordlogislose344.end() ){
        recordlogislose344[e.id].Num = recordlogislose344[e.id].Num + 1;
        switch(int(e.action)){
            case 0: recordlogislose344[e.id].UNKNOW++; break;


            case 1: recordlogislose344[e.id].ADD++; break;


            case 2: recordlogislose344[e.id].DELETE++; break;


            case 3: recordlogislose344[e.id].CHANGE++; break;

            default: break;
        }
    }


    else{
        windowinfochangetype wt;
        wt.ADD = 0;
        wt.CHANGE = 0;
        wt.DELETE = 0;
        wt.Num = 0;
        wt.UNKNOW = 0;
        switch(int(e.action)){
            case 0 : wt.UNKNOW = 1; break;


            case 1 : wt.ADD = 1; break;


            case 2: wt.DELETE = 1; break;


            case 3:  wt.CHANGE = 1; break;


            default : WLOGFE("ywttest: rcordlogisloseGetIncrementWindowInfoList id = %{public}d ERRRRRRR", e.id); break;
        }


        recordlogislose344.emplace(e.id, wt);
        //
    }
    for(const auto& e: recordlogislose344){
        WLOGFE("ywttest:recordlogisloseGetIncrementWindowInfoList%{public}s id = %{public}d UNKNOW = %{public}d ADD = %{public}d DELETE = %{public}d CHANGE=%{public}d  num=%{public}d", 
        str.c_str(), e.first, e.second.UNKNOW, e.second.ADD, e.second.DELETE, e.second.CHANGE, e.second.Num);
    }
}

void PrintLogGetFullWindowInfoList(const std::vector<MMI::WindowInfo>& windowInfoList){

    WLOGFI("testywt:GetFullWindowInfoList Start WindowInfoList_.size = %{public}d", int(windowInfoList.size()));
    for(const auto& e: windowInfoList){
        auto sessionleft = SceneSessionManager::GetInstance().GetSceneSession(e.id);
        WLOGFI("tesywt:GetFullWindowInfoList windowInfoList id = %{public}d area.x = %{public}d  area.y = %{public}d  area.w = %{public}d area.h = %{public}d  agentWindowId = %{public}d flags = %{pulic}d GetZOrder = %{public}d",
        e.id , e.area.x, e.area.y, e.area.width, e.area.height, e.agentWindowId,  e.flags, int(sessionleft->GetZOrder()));
        WLOGFI("testywt:GetFullWindowInfoList End");
    }
}

void PrintLogGetIncrementWindowInfoList(const std::map<uint64_t, std::vector<MMI::WindowInfo>>& screen2windowInfo){

    static std::map<int, windowinfochangetype> recordlogisloseIncrementWindo;
    WLOGFI("testywt:GetIncrementWindowInfoList Start1");
    for(const auto& windowinfolist : screen2windowInfo){
        WLOGFI("testywt:GetIncrementWindowInfoList screen id = %{public}d windowinfolist = %{public}d", int(windowinfolist.first), int(windowinfolist.second.size()));
        for(const auto& e: windowinfolist.second){
            WLOGFI("testywt:GetIncrementWindowInfoList  windowInfoList id = %{public}d action = %{public}d area.x = %{public}d area.y = %{public}d area.w = %{publid}d area.h = %{public}d agentWindowId = %{public}d  flags = %{public}d",
            e.id, int(e.action), e.area.x, e.area.y, e.area.width, e.area.height, e.agentWindowId, e.flags);

            printinputwindow("test2", e, recordlogisloseIncrementWindo);
        }
    }
    WLOGFI("testywt:GetIncrementWindowInfoList End");
}


void SceneSessionDirty::Clear()
{
    screen2windowInfo_.clear();
    isScreenSessionChange = false;
}

void SceneSessionDirty::Init()
{
    windowType2Action = {
        {WindowUpdateType::WINDOW_UPDATE_ADDED, WindowAction::WINDOW_ADD},
        {WindowUpdateType::WINDOW_UPDATE_REMOVED, WindowAction::WINDOW_DELETE},
        {WindowUpdateType::WINDOW_UPDATE_FOCUSED, WindowAction::WINDOW_CHANGE},
        {WindowUpdateType::WINDOW_UPDATE_BOUNDS, WindowAction::WINDOW_CHANGE},
        {WindowUpdateType::WINDOW_UPDATE_ACTIVE, WindowAction::WINDOW_ADD},
        {WindowUpdateType::WINDOW_UPDATE_PROPERTY, WindowAction::WINDOW_CHANGE},
    };
    RegisterScreenInfoChangeListener();
}

static void CalTramform(const sptr<SceneSession> sceneSession, Matrix3f& tranform){
    if(sceneSession == nullptr){
        WLOGFE("jing-test sceneSession is nullptr");
        return;
    }

    Vector2f scale(sceneSession->GetFloatingScale(), sceneSession->GetFloatingScale());

    WSRect windowRect = sceneSession->GetSessionRect();
    Vector2f translate(windowRect.posX_, windowRect.posY_);
    tranform = Matrix3f::IDENTITY;

    tranform = tranform.Translate(translate);

    tranform = tranform.Scale(scale);

    tranform = tranform.Inverse();
}



static MMI::WindowInfo PrepareWindowInfo(sptr<SceneSession> sceneSession, int action){



    if(sceneSession == nullptr){
        WLOGFE("jing-test sceneSession is nullptr");
        return {};
    }

    Matrix3f tranform;
    WSRect windowRect = sceneSession->GetSessionRect();
    auto pid = sceneSession->GetCallingPid();
    auto uid = sceneSession->GetCallingUid();
    auto windowId = sceneSession->GetWindowId();
    auto displayId = sceneSession->GetSessionProperty()->GetDisplayId();
    CalTramform(sceneSession, tranform);
    std::vector<float> transformData(tranform.GetData(), tranform.GetData() + 9 );

    auto agentWindowId = sceneSession->GetWindowId();
    auto zOrder = sceneSession->GetZOrder();
    const std::vector<int32_t> pointerChangeAreas{16, 5, 16, 5, 16, 5, 16, 5};
    const std::vector<WSRect>& hotAreas = sceneSession->GetResponseRegion();
    std::vector<MMI::Rect> mmiHotAreas;
    for(auto area : hotAreas){
        MMI::Rect rect;
        rect.x = area.posX_;
        rect.y = area.posY_;
        rect.width = area.width_;
        rect.height = area.height_;
        mmiHotAreas.emplace_back(rect);
   }

   MMI::Rect rect{0, 0, windowRect.width_, windowRect.height_};

   rect.x = windowRect.posX_;
   rect.y = windowRect.posY_;

   if(mmiHotAreas.empty()){
    mmiHotAreas.emplace_back(rect);
   }

   MMI::WindowInfo windowInfo = {
    .id = windowId,
    .pid = pid,
    .uid = uid,
    .area = rect,
    .defaultHotAreas = mmiHotAreas,
    .pointerHotAreas = mmiHotAreas,
    .agentWindowId = agentWindowId,
    .flags = (!sceneSession->GetTouchableEx()),

    .displayId = displayId,
    .pointerChangeAreas = pointerChangeAreas,
    .action = static_cast<MMI::WINDOW_UPDATE_ACTION>(action),
    .transform = transformData,
    .zOrder = zOrder
   };
   return windowInfo;
}

std::vector<MMI::WindowInfo> SceneSessionDirty::FullSceneSessionInfoUpdate()
{
    std::vector<MMI::WindowInfo> tWindowInfoList;
    const auto& sceneSessionMap = Rosen::SceneSessionManager::GetInstance().GetSceneSessionMap();
    WLOGFI("testywt: SceneSessionDirty FullSceneSessionInfoUpdate sceneSessionMap.size = %{public}d", int(sceneSessionMap.size()));
    for(const auto& sceneSessionValuePair : sceneSessionMap){
        const auto& sceneSessionValue = sceneSessionValuePair.second;
        WLOGFI("testywt:SceneSessionDirty windowName = %{public}s bundleName = %{public}s windowId = %{public}d", 
        sceneSessionValue->GetWindowName().c_str(), sceneSessionValue->GetSessionInfo().bundleName_.c_str(), sceneSessionValue->GetWindowId());
        if(IsWindowBackGround(sceneSessionValue)){
            continue;
        }



        const auto windowinfo = GetWindowInfo(sceneSessionValue, WindowAction::UNKNOWN);
        tWindowInfoList.emplace_back(windowinfo);
    }
    return tWindowInfoList;
}

bool SceneSessionDirty::IsWindowBackGround(const sptr<SceneSession>& sceneSession) const
{
    //WLOGFI("testywt : SceneSessionDirty IsWindowBackGround GetSessionState = %{public}d, IsSessionVisible = %{public}d", 
    //int(sceneSession->GetSessionState(), int(Rosen::SceneSessionManager::GetInstance().IsSessionVisible(sptr(sceneSession))));
    
    auto bundlename = sceneSession->GetSessionInfo().bundleName_;
    if(bundlename.find("SCBScenePanel") != std::string::npos){
        return false;
    }
    if(!Rosen::SceneSessionManager::GetInstance().IsSessionVisible(sceneSession)){
        return true;
    }
    return false;
}

void SceneSessionDirty::NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession, const WindowUpdateType& type){
    MMI::WindowInfo windowinfo;
    WindowAction action = GetSceneSessionAction(type);
    if(action == WindowAction::UNKNOWN){
        WLOGFW("windowAction UNKNOW WindowUpdateType type = %{public}d", int(type));
        return;
    }
    auto wid = sceneSession->GetWindowId();
    WLOGFI("testywt:NotifyWindowInfoChange3 = %{public}d type = %{public}d", wid, int(type));
    if(action == WindowAction::WINDOW_DELETE){
        windowinfo.action = static_cast<MMI::WINDOW_UPDATE_ACTION>(WindowAction::WINDOW_DELETE);
        windowinfo.id = wid;
        windowinfo.displayId = sceneSession->GetSessionProperty()->GetDisplayId();
        windowinfo.pid = sceneSession->GetCallingPid();
        windowinfo.uid = sceneSession->GetCallingUid();
    }
    else
    {
        if(action == WindowAction::WINDOW_CHANGE && IsWindowBackGround(sceneSession))
        {
            return;
        }
        windowinfo = GetWindowInfo(sceneSession, action);
    }
    PushWindowInfoList(windowinfo.displayId,  windowinfo);
    printinputwindow("test1", windowinfo, recordlogislose3442344);
}




void SceneSessionDirty::NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event)
{
    //WLOGFI("testywt: NotifyScreenChanged = %{public}lu  event = %{public}d", screenInfo->GetScreenId(), int(event));
    isScreenSessionChange = true;
}

void SceneSessionDirty::GetFullWindowInfoList(std::vector<MMI::WindowInfo>& windowInfoList)
{
    windowInfoList = FullSceneSessionInfoUpdate();
    //PrintLogGetFullWindowInfoList(windowInfoList);
    Clear();
}


void SceneSessionDirty::GetIncrementWindowInfoList(std::map<uint64_t, std::vector<MMI::WindowInfo>>& screen2windowInfo)
{
    screen2windowInfo = screen2windowInfo_;
    //PrintLogGetIncrementWindowInfoList(screen2windowInfo);
    Clear();
}

SceneSessionDirty::WindowAction SceneSessionDirty::GetSceneSessionAction(const WindowUpdateType& type)
{
    auto iter = windowType2Action.find(type);
    if(iter != windowType2Action.end()){
        return windowType2Action[type];
    }
    WLOGFW("WindowAction UNKNOW WindowUpdateType type = %{public}d", int(type));
    return WindowAction::UNKNOWN;
}

bool SceneSessionDirty::IsScreenChange()
{
    auto ret = isScreenSessionChange;
    if(ret){
        SetScreenChange(false);
    }
    return ret;
}

void SceneSessionDirty::SetScreenChange(bool value)
{
    std::lock_guard<std::mutex> lock(mutexlock);
    isScreenSessionChange = value;
}

void SceneSessionDirty::SetScreenChange(uint64_t id)
{
    SetScreenChange(true);
}

void SceneSessionDirty::PushWindowInfoList(uint64_t displayID, const MMI::WindowInfo& windowinfo)
{
    if(screen2windowInfo_.find(displayID) == screen2windowInfo_.end()){
        screen2windowInfo_.emplace(displayID, std::vector<MMI::WindowInfo>());
    }
    auto& twindowinlist = screen2windowInfo_[displayID];
    twindowinlist.emplace_back(windowinfo);
}



MMI::WindowInfo SceneSessionDirty::GetWindowInfo(const sptr<SceneSession>& sceneSession, const SceneSessionDirty::WindowAction& action) const
{
    
    return PrepareWindowInfo(sceneSession, static_cast<int32_t>(action));
}

void SceneSessionDirty::RegisterScreenInfoChangeListener()
{
    auto fun = [this](uint64_t a){
        //WLOGFI("testywt: RegisterScreenInfoChangeListener run funxxxx a = %{public}lu", a);
        this->SetScreenChange(a);
    };
    ScreenSessionManagerClient::GetInstance().RegisterScreenInfoChangeListener(fun);
    WLOGFI("testywt:RegisterScreenInfoChangeListener");
}

}//namespace OHOS::Rosen
