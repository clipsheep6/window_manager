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

#ifndef FOUNDATION_DM_SCREEN_MANAGER_LITE_H
#define FOUNDATION_DM_SCREEN_MANAGER_LITE_H

#include <cinttypes>
#include <refbase.h>
#include "screen_lite.h"
#include "dm_common.h"
#include "screen_group_lite.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class ScreenManagerLite : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(ScreenManagerLite);
friend class DMSDeathRecipientLite;
public:
    class IScreenListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when a new screen is connected.
         */
        virtual void OnConnect(ScreenId) = 0;

        /**
         * @brief Notify when a screen is disconnected.
         */
        virtual void OnDisconnect(ScreenId) = 0;

        /**
         * @brief Notify when state of the screen is changed.
         */
        virtual void OnChange(ScreenId) = 0;
    };

    class IScreenGroupListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when state of the screenGroup is changed.
         */
        virtual void OnChange(const std::vector<ScreenId>&, ScreenGroupChangeEvent) = 0;
    };

    class IVirtualScreenGroupListener : public virtual RefBase {
    public:
        struct ChangeInfo {
            ScreenGroupChangeEvent event;
            std::string trigger;
            std::vector<ScreenId> ids;
        };
        /**
         * @brief Notify when an event related to screen mirror occurs.
         *
         * @param info Change info of screen mirror.
         */
        virtual void OnMirrorChange([[maybe_unused]]const ChangeInfo& info) {}
    };

    /**
     * @brief Get the screen object by screen id.
     *
     * @param screenId ScreenLite id.
     * @return ScreenLite object.
     */
    sptr<ScreenLite> GetScreenById(ScreenId screenId);

    /**
     * @brief Get the screen group object by groupId.
     *
     * @param groupId ScreenLite group id.
     * @return ScreenGroupLite object.
     */
    sptr<ScreenGroupLite> GetScreenGroup(ScreenId groupId);

    /**
     * @brief Get all screens object.
     *
     * @param screens All screen objects.
     * @return DM_OK means get success, others means get failed.
     */
    DMError GetAllScreens(std::vector<sptr<ScreenLite>>& screens);

    /**
     * @brief Set the screen power state on the specified screen.
     *
     * @param screenId ScreenLite id.
     * @param state ScreenLite power state.
     * @param reason Reason for power state change.
     * @return True means set success, false means set failed.
     */
    bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason);

    /**
     * @brief Set the screen power states for all screens.
     *
     * @param state ScreenLite power state.
     * @param reason Reason for power state change.
     * @return True means set success, false means set failed.
     */
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason);

    /**
     * @brief Get screen power state.
     *
     * @param screenId ScreenLite id.
     * @return Power state of screen.
     */
    ScreenPowerState GetScreenPower(ScreenId screenId);

    /**
     * @brief Set screen rotation lock status.
     *
     * @param isLocked True means forbide to rotate screen, false means the opposite.
     * @return DM_OK means set success, others means set failed.
     */
    DMError SetScreenRotationLocked(bool isLocked);

    /**
     * @brief Get screen rotation lock status.
     *
     * @param isLocked Query the rotation lock status.
     * @return DM_OK means query success, others means query failed.
     */
    DMError IsScreenRotationLocked(bool& isLocked);

    /**
     * @brief Register screen listener.
     *
     * @param listener IScreenListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterScreenListener(sptr<IScreenListener> listener);

    /**
     * @brief Unregister screen listener.
     *
     * @param listener IScreenListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterScreenListener(sptr<IScreenListener> listener);

    /**
     * @brief Register screen group listener.
     *
     * @param listener IScreenGroupListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterScreenGroupListener(sptr<IScreenGroupListener> listener);

    /**
     * @brief Unregister screen group listener.
     *
     * @param listener IScreenGroupListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener);

    /**
     * @brief Register virtual screen group listener.
     *
     * @param listener IVirtualScreenGroupListener.
     * @return DM_OK means register success, others means register failed.
     */
    DMError RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);

    /**
     * @brief Unregister virtual screen group listener.
     *
     * @param listener IVirtualScreenGroupListener.
     * @return DM_OK means unregister success, others means unregister failed.
     */
    DMError UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
private:
    ScreenManagerLite();
    ~ScreenManagerLite();
    void OnRemoteDied();

    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_SCREEN_MANAGER_LITE_H