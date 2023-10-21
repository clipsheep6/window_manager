#include "picture_in_picture_manager.h"

#include <refbase.h>
#include "picture_in_picture_controller.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"


namespace OHOS {
namespace Rosen {
namespace {
	constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureManager"};
}

sptr<PictureInPictureController> PictureInPictureManager::curPipController_ = nullptr;
sptr<PictureInPictureController> PictureInPictureManager::activePipController_ = nullptr;
std::map<int32_t, sptr<PictureInPictureController>> PictureInPictureManager::windowToControllerMap_ = {};
std::mutex PictureInPictureManager::pipWindowStateMutex_;
PipWindowState PictureInPictureManager::pipWindowState_ = PipWindowState::STATE_UNDEFINED;

PictureInPictureManager::PictureInPictureManager()
{
}

PictureInPictureManager::~PictureInPictureManager()
{
}

void PictureInPictureManager::PutPipControllerInfo(int32_t windowId, sptr<PictureInPictureController> pipController)
{
    WLOGD("PutPipControllerInfo is called");
    PictureInPictureManager::windowToControllerMap_.insert(std::make_pair(windowId, pipController));
}

void PictureInPictureManager::RemovePipControllerInfo(int32_t windowId)
{
    WLOGD("RemovePipControllerInfo is called");
    PictureInPictureManager::windowToControllerMap_.erase(windowId);
}

void PictureInPictureManager::SetPipWindowState(PipWindowState pipWindowState)
{
    WLOGD("SetPipWindowState is called");
    std::lock_guard<std::mutex> lock(PictureInPictureManager::pipWindowStateMutex_);
    PictureInPictureManager::pipWindowState_ = pipWindowState;
}

PipWindowState PictureInPictureManager::GetPipWindowState()
{
    WLOGD("GetPipWindowState is called");
    return PictureInPictureManager::pipWindowState_;
}

bool PictureInPictureManager::IsCurrentPipControllerExist()
{
    WLOGD("IsCurrentPipControllerExist is called");
    return PictureInPictureManager::curPipController_ != nullptr;
}

bool PictureInPictureManager::IsCurrentPipController(wptr<PictureInPictureController> pipController)
{
    WLOGD("IsCurrentPipController is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return false;
    }
    return pipController.GetRefPtr() == PictureInPictureManager::curPipController_.GetRefPtr();
}

void PictureInPictureManager::SetCurrentPipController(sptr<PictureInPictureController> pipController)
{
    WLOGD("SetCurrentPipController is called");
    PictureInPictureManager::curPipController_ = pipController;
}

void PictureInPictureManager::RemoveCurrentPipController()
{
    WLOGD("RemoveCurrentPipController is called");
    PictureInPictureManager::curPipController_ = nullptr;
}

void PictureInPictureManager::RemoveCurrentPipControllerSafety()
{
    WLOGD("RemoveCurrentPipControllerSafety is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    PictureInPictureManager::curPipController_->SetWindow(nullptr);
    PictureInPictureManager::RemoveCurrentPipController();
}

bool PictureInPictureManager::IsCurrentWindow(int32_t windowId)
{
    WLOGD("IsCurrentWindow is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return false;
    }
    // TODO GetWindowId
    return PictureInPictureManager::curPipController_->GetWindowId() == windowId;
}

sptr<Window> PictureInPictureManager::GetCurrentWindow()
{
    WLOGD("GetCurrentWindow is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return nullptr;
    }
    // TODO GetWindow
    return PictureInPictureManager::curPipController_->GetWindow();
}

void PictureInPictureManager::DoRestore()
{
    WLOGD("DoRestore is called");
    // TODO: Add restore
}

void PictureInPictureManager::DoClose()
{
    WLOGD("DoClose is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    PictureInPictureManager::curPipController_->StopPictureInPicture(false);
}

void PictureInPictureManager::DoStartMove()
{
    WLOGD("DoStartMove is called");
    // TODO: Add startMove
}

void PictureInPictureManager::DoScale()
{
    WLOGD("DoScale is called");
    // TODO: Add scale
}

void PictureInPictureManager::DoActionEvent()
{
    WLOGD("DoActionEvent is called");
    // TODO: Add action event
}
}
}
