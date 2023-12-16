/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_LITE_H
#define FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_LITE_H

#include <map>
#include <mutex>

#include "display_lite.h"
#include "screen_lite.h"
#include "screen_group_lite.h"
#include "dm_common.h"
#include "fold_screen_info.h"
#include "singleton_delegator.h"
#include "zidl/screen_session_manager_lite_interface.h"

#ifdef SCENE_BOARD_DISABLED
#include <surface.h>
#include "display_manager_interface.h"
#endif

namespace OHOS::Rosen {
class BaseAdapterLite {
public:
    virtual ~BaseAdapterLite();
    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual void Clear();
protected:
    bool InitDMSProxy();
    std::recursive_mutex mutex_;
#ifdef SCENE_BOARD_DISABLED
    sptr<IDisplayManager> displayManagerServiceProxy_ = nullptr;
#else
    sptr<IScreenSessionManagerLite> displayManagerServiceProxy_ = nullptr;
#endif
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    bool isProxyValid_ { false };
};

class DMSDeathRecipientLite : public IRemoteObject::DeathRecipient {
public:
    explicit DMSDeathRecipientLite(BaseAdapterLite& adapter);
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
private:
    BaseAdapterLite& adapter_;
};

class DisplayManagerAdapterLite : public BaseAdapterLite {
WM_DECLARE_SINGLE_INSTANCE(DisplayManagerAdapterLite);
public:
    virtual sptr<DisplayInfo> GetDefaultDisplayInfo();
    virtual sptr<DisplayInfo> GetDisplayInfoByScreenId(ScreenId screenId);
    virtual std::vector<DisplayId> GetAllDisplayIds();
    virtual DMError DisableDisplaySnapshot(bool disableOrNot);
    virtual DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow);

    virtual bool WakeUpBegin(PowerStateChangeReason reason);
    virtual bool WakeUpEnd();
    virtual bool SuspendBegin(PowerStateChangeReason reason);
    virtual bool SuspendEnd();

    virtual bool SetDisplayState(DisplayState state);
    virtual DisplayState GetDisplayState(DisplayId displayId);
    virtual void NotifyDisplayEvent(DisplayEvent event);
    virtual sptr<DisplayInfo> GetDisplayInfo(DisplayId displayId);

    virtual bool IsFoldable();

    virtual FoldStatus GetFoldStatus();

    virtual FoldDisplayMode GetFoldDisplayMode();

    virtual void SetFoldDisplayMode(const FoldDisplayMode);

    virtual sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion();
private:
    static inline SingletonDelegator<DisplayManagerAdapterLite> delegator;
};

class ScreenManagerAdapterLite : public BaseAdapterLite {
WM_DECLARE_SINGLE_INSTANCE(ScreenManagerAdapterLite);
public:
    virtual bool SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason);
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason);
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId);
    virtual DMError SetOrientation(ScreenId screenId, Orientation orientation);
    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId);
    virtual DMError GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos);
    virtual DMError SetScreenActiveMode(ScreenId screenId, uint32_t modeId);
    virtual sptr<ScreenInfo> GetScreenInfo(ScreenId screenId);
    virtual DMError SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio);
    virtual DMError SetScreenRotationLocked(bool isLocked);
    virtual DMError IsScreenRotationLocked(bool& isLocked);
private:
    static inline SingletonDelegator<ScreenManagerAdapterLite> delegator;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_LITE_H
