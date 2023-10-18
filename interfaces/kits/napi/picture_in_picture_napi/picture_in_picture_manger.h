#ifndef OHOS_PICTURE_IN_PICTURE_MANAGER_H
#define OHOS_PICTURE_IN_PICTURE_MANAGER_H

#include "refbase.h"
#include "picture_in_picture_control.h"

namespace OHOS {
namespace Rosen {
class PictureInPictureManager {
public:
    PictureInPictureManager();
    ~PictureInPictureManager();
    static void SetPipController(sptr<PictureInPictureControl> pipController);
    static void SetAutoPipController(sptr<PictureInPictureControl> autoPipController);
    static void AutoPullPipWindow(wptr<PictureInPictureControl> nextPipController);
    static void IsControllerNow();
    static void IsAutoPullController(wptr<PictureInPictureControl> pipController);
    static void IsCurrentController(wptr<PictureInPictureControl> pipController);
    static void IsMainWindowControlPip(int32_t mainWindowId);
    static sptr<Window> GetCurrentPipWidow();
    static void DeattachCurrentController();
    static void AttachCurrentController();
    static void DoActionEvent();
    static void DoRestore();
    static void DoClose();
    static void DoStartMove();
    static void DoScale();
private:
    static sptr<pictureInPictureControl> showingPipController_;
    static sptr<pictureInPictureControl> autoPullPipController_;
};
} // namespace Rosen
} // namespace OHOS
#endif //OHOS_PICTURE_IN_PICTURE_MANAGER_H