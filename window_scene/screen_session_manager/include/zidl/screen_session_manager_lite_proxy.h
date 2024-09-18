/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_PROXY_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_PROXY_H

#include "screen_session_manager_lite_interface.h"

#include "iremote_proxy.h"

namespace OHOS {
namespace Rosen {

class ScreenSessionManagerLiteProxy : public IRemoteProxy<IScreenSessionManagerLite> {
public:
    explicit ScreenSessionManagerLiteProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IScreenSessionManagerLite>(impl) {}

    ~ScreenSessionManagerLiteProxy() = default;

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    FoldDisplayMode GetFoldDisplayMode() override;
    void SetFoldDisplayMode(const FoldDisplayMode displayMode) override;
    bool IsFoldable() override;
    FoldStatus GetFoldStatus() override;
    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    virtual sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    /*
     * used by powermgr
     */
    virtual bool WakeUpBegin(PowerStateChangeReason reason) override;
    virtual bool WakeUpEnd() override;
    virtual bool SuspendBegin(PowerStateChangeReason reason) override;
    virtual bool SuspendEnd() override;
    virtual bool SetSpecifiedScreenPower(ScreenId, ScreenPowerState, PowerStateChangeReason) override;
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId) override;
    virtual bool SetDisplayState(DisplayState state) override;
    virtual DisplayState GetDisplayState(DisplayId displayId) override;
    virtual bool SetScreenBrightness(uint64_t screenId, uint32_t level) override;
    virtual uint32_t GetScreenBrightness(uint64_t screenId) override;
    virtual std::vector<DisplayId> GetAllDisplayIds() override;
private:
    static inline BrokerDelegator<ScreenSessionManagerLiteProxy> delegator_;
};

} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_PROXY_H