#ifndef OHOS_ROSEN_ditry_SESSION_MANAGER_H
#define OHOS_ROSEN_ditry_SESSION_MANAGER_H


#include <map>
//#include "input_manager.h"

#include "session_manager/include/screen_session_manager.h"
#include "session/screen/include/screen_session.h"
#include "common/rs_vector4.h"
#include "session/host/include/root_scene_session.h"

namespace OHOS::MMI{
    struct WindowInfo;
}
namespace OHOS::Rosen{
class SceneSessionDirty{
private:
    enum WindowAction:uint32_t{
        UNKNOWN = 0,
        WINDOW_ADD,
        WINDOW_DELETE,
        WINDOW_CHANGE,
    };

using ScreenInfoChangeListener = std::function<void(int32_t)>;
public:
    SceneSessionDirty(){};
private:
    std::map<WindowUpdateType, WindowAction> windowType2Action;
    std::map<uint64_t, std::vector<MMI::WindowInfo>> screen2windowInfo_;
    bool isScreenSessionChange = true;
    std::vector<MMI::WindowInfo> windowInfoList_;
private:
    void Clear();
    std::vector<MMI::WindowInfo> FullSceneSessionInfoUpdate();
    bool IsWindowBackGround(const sptr<SceneSession>& sceneSession) const;
    MMI::WindowInfo GetWindowInfo(const sptr<SceneSession>& sceneSession, const WindowAction& action) const;
    void PushWindowInfoList(uint64_t displayId, const MMI::WindowInfo& windowinfo);
    WindowAction GetSceneSessionAction(const WindowUpdateType& type);
public:
    void RegisterScreenInfoChangeListener();
    void Init();
    bool IsScreenChange();
    void SetScreenChange(uint64_t id);
    void NotifyWindowInfoChange(const sptr<SceneSession>& sceneSession, const WindowUpdateType& type);
    void NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event);
    void GetFullWindowInfoList(std::vector<MMI::WindowInfo>& windowInfoList);
    void GetIncrementWindowInfoList(std::map<uint64_t, std::vector<MMI::WindowInfo>>& screen2windowInfo);
};

}//namespace OHOS::Rosen

#endif 