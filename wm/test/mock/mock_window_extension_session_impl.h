/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef MOCK_WINDOW_EXTENSION_SESSION_IMPL_H
#define MOCK_WINDOW_EXTENSION_SESSION_IMPL_H

#include "mock_data_handler.h"
#include "window_extension_session_impl.h"

namespace OHOS {
namespace Rosen {
class MockWindowExtensionSessionImpl : public WindowExtensionSessionImpl {
public:
    MockWindowExtensionSessionImpl(const sptr<WindowOption>& option) : WindowExtensionSessionImpl(option) {}
    ~MockWindowExtensionSessionImpl() {}

    WMError HideNonSecureWindows(bool shouldHide)
    {
        extensionWindowFlags_.hideNonSecureWindowsFlag = shouldHide;
        return WMError::WM_OK;
    }

    WMError SetWaterMarkFlag(bool isEnable)
    {
        extensionWindowFlags_.waterMarkFlag = isEnable;
        return WMError::WM_OK;
    }

    bool IsPcWindow() const { return true; }

    bool IsPcOrPadFreeMultiWindowMode() const
    {
        return true;
    }

    WMError HidePrivacyContentForHost(bool needHide)
    {
        return WMError::WM_OK;
    }

    std::shared_ptr<IDataHandler> GetExtensionDataHandler() const
    {
        return mockHandler_;
    }

private:
    std::shared_ptr<IDataHandler> mockHandler_;
};
} // Rosen
} // OHOS
#endif