/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at,
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software,
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_LITE_H
#define FOUNDATION_DM_DISPLAY_MANAGER_LITE_H

#include <vector>
#include <mutex>

#include "display_lite.h"
#include "dm_common.h"
#include "wm_single_instance.h"
#include "display_change_info.h"

namespace OHOS::Rosen {
class DisplayManagerLite {
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManagerLite);
friend class DMSDeathRecipientLite;
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

    class IFoldStatusListener : public virtual RefBase {
    public:
        /**
         * @brief Notify listeners when screen fold status changed.
         *
         * @param foldStatus Screen foldStatus.
         */
        virtual void OnFoldStatusChanged([[maybe_unused]]FoldStatus foldStatus) {}
    };

    /**
     * @brief Register a display listener.
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
     * @brief Register a listener for the event of screen fold status changed.
     *
     * @param listener IFoldStatusListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterFoldStatusListener(sptr<IFoldStatusListener> listener);

    /**
     * @brief Unregister an existed listener for the event of screen fold status changed.
     *
     * @param listener IFoldStatusListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterFoldStatusListener(sptr<IFoldStatusListener> listener);

    /**
     * @brief Get the default display object.
     *
     * @return Default display object.
     */
    sptr<DisplayLite> GetDefaultDisplay();

    /**
     * @brief Check whether the device is foldable.
     *
     * @return true means the device is foldable.
     */
    bool IsFoldable();

    /**
     * @brief Get the current fold status of the foldable device.
     *
     * @return fold status of device.
     */
    FoldStatus GetFoldStatus();

    /**
     * @brief Get the display mode of the foldable device.
     *
     * @return display mode of the foldable device.
     */
    FoldDisplayMode GetFoldDisplayMode();
private:
    DisplayManagerLite();
    ~DisplayManagerLite();
    void OnRemoteDied();

    class Impl;
    std::recursive_mutex mutex_;
    bool destroyed_ = false;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_LITE_H