/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_H
#define FOUNDATION_DM_DISPLAY_MANAGER_H

#include <vector>
#include <mutex>
#include <pixel_map.h>

#include "display.h"
#include "dm_common.h"
#include "wm_single_instance.h"
#include "screenshot_info.h"

namespace OHOS::Rosen {
class DisplayManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManager);
friend class DMSDeathRecipient;
public:
    class IDisplayListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when a new display is created.
         */
        virtual void OnCreate(DisplayId) = 0;

        /**
         * @brief Notify when the display is destroyed.
         */
        virtual void OnDestroy(DisplayId) = 0;

        /**
         * @brief Notify when the state of a display changes
         */
        virtual void OnChange(DisplayId) = 0;
    };

    class IScreenshotListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when a screenshot event occurs.
         *
         * @param info Screenshot info.
         */
        virtual void OnScreenshot(const ScreenshotInfo info) {}
    };

    class IPrivateWindowListener : public virtual RefBase {
    public:
        /**
         * @brief Monitor whether the existence of privacy window has changed.
         *
         * @param hasPrivate True means the display has private window, false means the opposite.
         */
        virtual void OnPrivateWindow(bool hasPrivate) {};
    };

    /**
     * @brief Obtain all displays.
     *
     * @return All displays.
     */
    std::vector<sptr<Display>> GetAllDisplays();

    /**
     * @brief Obtain the id of the default display.
     *
     * @return Default display id.
     */
    DisplayId GetDefaultDisplayId();

    /**
     * @brief Get the default display object.
     *
     * @return Default display object.
     */
    sptr<Display> GetDefaultDisplay();

    /**
     * @brief Get the default display object by means of sync.
     *
     * @return Default display id.
     */
    sptr<Display> GetDefaultDisplaySync();

    /**
     * @brief Get the display object by id.
     *
     * @param displayId Id of the target display.
     * @return Default display object.
     */
    sptr<Display> GetDisplayById(DisplayId displayId);

    /**
     * @brief Get the display object by corresponding screenId.
     *
     * @param screenId The id of the target screen.
     * @return Display object.
     */
    sptr<Display> GetDisplayByScreen(ScreenId screenId);

    /**
     * @brief Get IDs of all displays.
     *
     * @return All display IDs.
     */
    std::vector<DisplayId> GetAllDisplayIds();

    /**
     * @brief Get whether the target display contains privacy windows.
     *
     * @param displayId Display id.
     * @param hasPrivateWindow True means display has private window, false means the opposite.
     * @return DM_OK means query privacy success, others means query failed.
     */
    DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow);

    /**
     * @brief Get screenshot of the target display.
     *
     * @param displayId Display id.
     * @param errorCode error code.
     * @return PixelMap object of screenshot.
     */
    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId, DmErrorCode* errorCode = nullptr);
    /**
     * @brief Get screenshot of the target display.
     *
     * @param displayId Display id.
     * @param rect Rect of screenshot.
     * @param size Size of screenshot.
     * @param rotation Parameter of rotation.
     * @param errorCode error code.
     * @return PixelMap object of screenshot.
     */
    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId, const Media::Rect &rect,
        const Media::Size &size, int rotation, DmErrorCode* errorCode = nullptr);

    /**
     * @brief Begin to wake up screen.
     *
     * @param reason Reason for power state change.
     * @return True means begin success, false means begin failed.
     */
    bool WakeUpBegin(PowerStateChangeReason reason);

    /**
     * @brief Wake up screen end.
     *
     * @return True means end success, false means end failed.
     */
    bool WakeUpEnd();

    /**
     * @brief Begin to suspend the screen.
     *
     * @param reason Reason for power state change.
     * @return True means begin success, false means begin failed.
     */
    bool SuspendBegin(PowerStateChangeReason reason);

    /**
     * @brief Suspend screen end.
     *
     * @return True means suspend screen end success.
     * @return False means suspend screen end failed.
     */
    bool SuspendEnd();

    /**
     * @brief Set the Display State object
     *
     * @param state State of display.
     * @param callback Callback for display state.
     * @return True means set success, false means set failed.
     */
    bool SetDisplayState(DisplayState state, DisplayStateCallback callback);

    /**
     * @brief Get the state of the target display.
     *
     * @param displayId Display id.
     * @return State of display.
     */
    DisplayState GetDisplayState(DisplayId displayId);

    /**
     * @brief Set the brightness level of the target screen.
     *
     * @param screenId Target screen.
     * @param level Brightness level.
     */
    bool SetScreenBrightness(uint64_t screenId, uint32_t level);

    /**
     * @brief Get the brightness level of the target screen.
     *
     * @param screenId Screen id.
     * @return Brightness value of screen.
     */
    uint32_t GetScreenBrightness(uint64_t screenId) const;

    /**
     * @brief Notify when events of certain types occur.
     *
     * @param event Enumerate value of DisplayEvent.
     */
    void NotifyDisplayEvent(DisplayEvent event);

    /**
     * @brief Freeze target displays.
     *
     * @param displayIds Display id needed to freeze.
     * @return True means freeze success, false means freeze failed.
     */
    bool Freeze(std::vector<DisplayId> displayIds);

    /**
     * @brief Unfreeze target displays.
     *
     * @param displayIds Display id needed to unfreeze.
     * @return True means unfreeze success, false means unfreeze failed.
     */
    bool Unfreeze(std::vector<DisplayId> displayIds);

    /**
     * @brief Resgister a display listener.
     *
     * @param listener IDisplayListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterDisplayListener(sptr<IDisplayListener> listener);

    /**
     * @brief Unregister an existed display listener.
     *
     * @param listener IDisplayListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterDisplayListener(sptr<IDisplayListener> listener);

    /**
     * @brief Register a listener for display power events.
     *
     * @param listener IDisplayPowerEventListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);

    /**
     * @brief Unregiste an existed listener for display power events.
     *
     * @param listener IDisplayPowerEventListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);

    /**
     * @brief Register a listener for screenshot event.
     *
     * @param listener IScreenshotListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterScreenshotListener(sptr<IScreenshotListener> listener);

    /**
     * @brief Unregister an existed listener for screenshot event.
     *
     * @param listener IScreenshotListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterScreenshotListener(sptr<IScreenshotListener> listener);

    /**
     * @brief Register a listener for the event of private window.
     *
     * @param listener IPrivateWindowListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterPrivateWindowListener(sptr<IPrivateWindowListener> listener);

    /**
     * @brief Unregister an existed listener for the event of private window.
     *
     * @param listener IPrivateWindowListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterPrivateWindowListener(sptr<IPrivateWindowListener> listener);

    /**
     * @brief Add a surface node to the target display.
     *
     * @param displayId Target display.
     * @param surfaceNode SurfaceNode object.
     * @return DM_OK means add success, others means add failed.
     */
    DMError AddSurfaceNodeToDisplay(DisplayId displayId, std::shared_ptr<class RSSurfaceNode>& surfaceNode);
    
    /**
     * @brief Remove a surface node from the target display.
     *
     * @param displayId Target display.
     * @param surfaceNode SurfaceNode object.
     * @return DM_OK means remove success, others means remove failed.
     */
    DMError RemoveSurfaceNodeFromDisplay(DisplayId displayId, std::shared_ptr<class RSSurfaceNode>& surfaceNode);

    constexpr static int32_t MAX_RESOLUTION_SIZE_SCREENSHOT = 3840; // max resolution, 4K

private:
    DisplayManager();
    ~DisplayManager();
    void OnRemoteDied();

    class Impl;
    std::recursive_mutex mutex_;
    bool destroyed_ = false;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_H