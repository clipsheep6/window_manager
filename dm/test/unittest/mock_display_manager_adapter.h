/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_DISPLAY_MANAGER_ADAPTER_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_DISPLAY_MANAGER_ADAPTER_H
#include <gmock/gmock.h>

#include "display_manager_adapter.h"

namespace OHOS {
namespace Rosen {
class MockDisplayManagerAdapter : public DisplayManagerAdapter {
public:
    MOCK_METHOD0(GetDefaultDisplayId, DisplayId());
    MOCK_METHOD1(GetDisplayById, sptr<Display>(DisplayId displayId));
    MOCK_METHOD1(CreateVirtualScreen, ScreenId(VirtualScreenOption option));
    MOCK_METHOD1(DestroyVirtualScreen, DMError(ScreenId screenId));
    MOCK_METHOD1(GetDisplaySnapshot, std::shared_ptr<Media::PixelMap>(DisplayId displayId));
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD2(RegisterDisplayManagerAgent, bool(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type));
    MOCK_METHOD2(UnregisterDisplayManagerAgent, bool(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type));
    MOCK_METHOD1(WakeUpBegin, bool(PowerStateChangeReason reason));
    MOCK_METHOD0(WakeUpEnd, bool());
    MOCK_METHOD1(SuspendBegin, bool(PowerStateChangeReason reason));
    MOCK_METHOD0(SuspendEnd, bool());
    MOCK_METHOD2(SetScreenPowerForAll, bool(DisplayPowerState state, PowerStateChangeReason reason));
    MOCK_METHOD1(SetDisplayState, bool(DisplayState state));
    MOCK_METHOD1(GetDisplayState, DisplayState(uint64_t displayId));
    MOCK_METHOD2(SetScreenActiveMode, bool(ScreenId screenId, uint32_t modeId));
};
}
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_DISPLAY_MANAGER_ADAPTER_H