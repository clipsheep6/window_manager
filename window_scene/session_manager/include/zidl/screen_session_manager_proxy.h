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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_PROXY_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_PROXY_H

#include "screen_session_manager_interface.h"

#include "iremote_proxy.h"

namespace OHOS {
namespace Rosen {

class ScreenSessionManagerProxy : public IRemoteProxy<IScreenSessionManager> {
public:
    explicit ScreenSessionManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy(impl) {}

    ~ScreenSessionManagerProxy() = default;

    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() override;

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;

    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) override;

    virtual DMError DestroyVirtualScreen(ScreenId screenId) override;

    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface) override;

    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
        ScreenId& screenGroupId) override;

    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;

    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override;

    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId, DmErrorCode* errorCode) override;

    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;

private:
    static inline BrokerDelegator<ScreenSessionManagerProxy> delegator_;
};

} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_PROXY_H