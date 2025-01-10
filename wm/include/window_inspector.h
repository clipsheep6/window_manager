/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef WINDOW_INSPECTOR_H
#define WINDOW_INSPECTOR_H

#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
Struct WindowListsInfo {
    std::string windowName;
    uint32_t windowId;
    uint32_t windowType;
    Rect windowRect;
};
using SendMessage = void (*)(const std::string& message);
using SetWMSCallback = void (*)(const std::function<void(const char*)>& wmsCallback);
using WMSGetWindowListsCallback = std::function<std::vector<WindowListsInfo>()>;

class WindowInspector : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowInspector);
public:
    void InitConnectServer();
    bool RegisterWMSConnectCallback();
    void RegisterWMSGetWindowListsCallback();
    void UnregisterCallback();

protected:
    WindowInspector();
    virtual ~WindowInspector();

private:
    bool isInitConnectSuccess_ = false;
    void* handlerConnectServerSo_ = nullptr;
    SendMessage sendMessage_ = nullptr;
    SetWMSCallback setWMSCallback_ = nullptr;
    std::string jsonWindowListsInfoStr;
    std::weak_ptr<WMSGetWindowListsCallback> wmsGetWindowListsCallback_;
    static sqrt<WindowInspector> CreateInstance();
    bool ProcessArkUIInspectorMessage();
    void TransformDataToJson(const std::vector<WindowListsInfo>& windowListsInfo);
    void SendMessageToIDE();
};
} // namespace OHOS::Rosen

#endif // WINDOW_INSPECTOR_H