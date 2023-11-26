#include "window_drawing_content_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowDrawingContentInfo"};
}

bool WindowDrawingContentInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUint32(windowId_) && parcel.WriteInt32(pid_) &&
           parcel.WriteInt32(uid_) && parcel.WriteUint32(static_cast<uint32_t>(drawingContentState_)) &&
           parcel.WriteUint32(static_cast<uint32_t>(windowType_));
}

WindowDrawingContentInfo* WindowDrawingContentInfo::Unmarshalling(Parcel &parcel)
{
    auto windowDrawingContentInfo = new (std::nothrow) WindowDrawingContentInfo();
    if (windowDrawingContentInfo == nullptr) {
        WLOGFE("window visibility info is nullptr.");
        return nullptr;
    }

    uint32_t drawingContentState = 0;
    bool res = parcel.ReadUint32(windowDrawingContentInfo->windowId_) &&
        parcel.ReadInt32(windowDrawingContentInfo->pid_) && parcel.ReadInt32(windowDrawingContentInfo->uid_)
        && parcel.ReadUint32(drawingContentState);
    if (!res) {
        delete windowDrawingContentInfo;
        return nullptr;
    }
    windowDrawingContentInfo->drawingContentState_ = static_cast<WindowLayerState>(drawingContentState);
    windowDrawingContentInfo->windowType_ = static_cast<WindowType>(parcel.ReadUint32());
    return windowDrawingContentInfo;
}
} // namespace OHOS::Rosen
