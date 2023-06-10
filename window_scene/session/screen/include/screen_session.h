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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H

#include <vector>

#include <refbase.h>
#include <screen_manager/screen_types.h>
#include <ui/rs_display_node.h>

#include "screen_property.h"
#include "dm_common.h"
#include "display_info.h"
#include "event_handler.h"
#include "screen.h"
#include "screen_info.h"

namespace OHOS::Rosen {
class IScreenChangeListener : public RefBase {
public:
    IScreenChangeListener() = default;
    virtual ~IScreenChangeListener() = default;

    virtual void OnConnect() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnPropertyChange(const ScreenProperty& newProperty) = 0;
};

enum class ScreenState : int32_t {
    INIT,
    CONNECTION,
    DISCONNECTION,
};

class ScreenSession : public RefBase {
public:
    explicit ScreenSession(ScreenId screenId, const ScreenProperty& property);
    ~ScreenSession() = default;

    void RegisterScreenChangeListener(IScreenChangeListener* screenChangeListener);
    void UnregisterScreenChangeListener(IScreenChangeListener* screenChangeListener);

    void FillScreenInfo(sptr<ScreenInfo> info) const;

    sptr<DisplayInfo> ConvertToDisplayInfo();
    sptr<ScreenInfo> ConvertToScreenInfo() const;
    sptr<SupportedScreenModes> GetActiveScreenMode() const;
    ScreenSourceMode GetSourceMode() const;
    

    ScreenId GetScreenId();
    ScreenProperty GetScreenProperty() const;
    std::shared_ptr<RSDisplayNode> GetDisplayNode() const;

    ScreenId GetId() const;
    ScreenState GetScreenState() const;
    DMError SetScreenActiveMode(uint32_t modeId);
    std::vector<sptr<SupportedScreenModes>> GetScreenModes() const;

    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts);
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut);
    DMError SetScreenColorGamut(int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap);
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform();

    void Connect();
    void Disconnect();

    int32_t activeIdx_ { 0 };
    std::vector<sptr<SupportedScreenModes>> modes_ = {};

private:
    ScreenId screenId_;
    ScreenProperty property_;
    std::string name_ { "UNKNOW" };
    int32_t activeModeIdx_ { 0 };
    
    std::shared_ptr<RSDisplayNode> displayNode_;
    ScreenState screenState_ { ScreenState::INIT };
    std::vector<IScreenChangeListener*> screenChangeListenerList_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCREEN_SESSION_H
