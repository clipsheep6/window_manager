#ifndef OHOS_PICTURE_IN_PICTURE_MANAGER_H
#define OHOS_PICTURE_IN_PICTURE_MANAGER_H

#include <refbase.h>
#include "picture_in_picture_controller.h"
#include "pip_info.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class PictureInPictureManager {
public:
    PictureInPictureManager();
    ~PictureInPictureManager();
    static void PutPipControllerInfo(int32_t windowId, wptr<PictureInPictureController> pipController);
    static void RemovePipControllerInfo(int32_t windowId);

    static void SetPipWindowState(PipWindowState pipWindowState);
    static void GetPipWindowState();

    static bool IsCurrentPipControllerExist();
    static bool IsCurrentPipController(wptr<PictureInPictureController> pipController);
    static void SetCurrentPipController(sptr<PictureInPictureController> pipController);
    static void RemoveCurrentPipController();
    static void RemoveCurrentPipControllerSafety();

    static bool IsCurrentWindow(int32_t windowId);
    static sptr<Window> GetCurrentWindow();

    static void DoRestore();
    static void DoClose();
    static void DoStartMove();
    static void DoScale();
    static void DoActionEvent();
private:
    sptr<PictureInPictureController> curPipController_;
    sptr<PictureInPictureController> nextPipController_;
    std::map<int32_t, sptr<PictureInPictureController>> windowToControllerMap_;
    std::mutex pipWindowStateMutex_;
    PipWindowState pipWindowState_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_PICTURE_IN_PICTURE_MANAGER_H