#include "picture_in_picture_manager.h"
#include "refbase.h"
#include "picture_in_picture_control.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
	constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "pictureInpictureManager"};

}

sptr<pictureInPictureControl> PictureInPictureManager::showingPipController_ = nullptr;
sptr<pictureInPictureControl> PictureInPictureManager::autoPullPipController_ = nullptr;

PictureInPicture::PictureInPictureManager()
{
}

PictureInPicture::~PictureInPictureManager()
{
}

void PictureInPictureManager::SetPipController(sptr<PictureInPictureControl> pipController) {
	showingPipController_ = pipController;
	WLOGFD("SetPipControl");
}

void PictureInPictureManager::SetAutoPipController(sptr<PictureInPictureControl> autoPipController) {
	autoPullPipController_ = autoPipController;
	WLOGFD("SetAutoPipController");
}

void PictureInPictureManager::DoActionEvent(std::string actionName) {
	WLOGFD("DoActionEvent");
	if (showingPipController_ != nullptr) {
        showingPicshowingPipController_ -> DoActionEvient(actionName);
        WLOGFD("DoActionEvent done");
    }
}

void PictureInPictureManager::DoRestore() {
	WLOGFD("DoRestore");
	if (showingPipController_ != nullptr) {
        showingPicshowingPipController_ -> RestorePictureInPictureWindow();
        WLOGFD("DoRestore done");
    }
}

void PictureInPictureManager::DoClose() {
	WLOGFD("DoClose");
	if (showingPipController_ != nullptr) {
        showingPicshowingPipController_ -> StopPictureInPictureWindow();
        WLOGFD("DoClose done");
    }
}

void PictureInPictureManager::DoStartMove() {
	WLOGFD("DoStartMove");
	if (showingPipController_ != nullptr) {
        showingPicshowingPipController_ -> StartMove();
        WLOGFD("DoStartMove done");
    }
}

void PictureInPictureManager::DoScale() {
	WLOGFD("DoScale");
	if (showingPipController_ != nullptr) {
        showingPicshowingPipController_ -> ProcessScale();
        WLOGFD("DoScale done");
    }
}

void PictureInPictureManager::AutoPullPipWindow(wptr<PictureInPictureControl> nextPipController) {
	if (nextPipController.GetRefPtr() != autoPullPipControl_.GetRefPtr()) {
        return;
    }
    if (autoPullPipController_ == nullptr) {
        WLOGFD("autoPullPipController_ is null");
        return;
    }
    WLOGFD("AutoPullPipWindow now.");
    autoPullPipController_ -> StartPictureInPictureWindow();
}

bool PictureInPictureManager::IsCurrentController(wptr<PictureInPictureControl> pipController) {
	return pipController.GetRefPtr() == showingPipController_.GetRefPtr;
}

bool PictureInPictureManager::IsControllerNow() {
	return showingPipcontroller_ != nullptr;
}

bool PictureInPictureManager::IsAutoPullController(wptr<PictureInPictureControl> pipController) {
	return pipController.GetRefPtr() == autoPullPipController_.GetRefPtr;
}

bool PictureInPictureManager::IsMainWindowControlPip(int32_t mainWindowId) {
	return showingPipcontroller_->getPersistenId() == mainWindowId;
}

sptr<Window> PictureInPictureManger::GetCurrentPipWidow() {
    if (showingPipcontroller_ != nullptr) {
        return showingPipcontroller_->GetPipWindow();
    } else {
        reture nullptr;
    }
}

void PictureInPictureManager::AttachCurrentController(sptr<PictureInPictureControl> pipController) {
	showingPipcontroller_ = pipContorller;
}


void PictureInPictureManager::DeattachCurrentController() {
	if (showingPipcontroller_ != nullptr) {
        showingPipcontroller_->SetPipWindow(nullptr);
    }
    showingPipController_ = nullptr;
}

 



}
}
