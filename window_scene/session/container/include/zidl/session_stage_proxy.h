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

#ifndef OHOS_WINDOW_SCENE_SESSION_STAGE_RPOXY_H
#define OHOS_WINDOW_SCENE_SESSION_STAGE_RPOXY_H

#include <iremote_proxy.h>
#include <transaction/rs_transaction.h>

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "ws_common.h"

namespace OHOS::Rosen {
class SessionStageProxy : public IRemoteProxy<ISessionStage> {
public:
    explicit SessionStageProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISessionStage>(impl) {};

    ~SessionStageProxy() {};

    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const SceneAnimationConfig& config = { nullptr, ROTATE_ANIMATION_DURATION },
        const std::map<AvoidAreaType, AvoidArea>& avoidAreas = {}) override;
    void UpdateDensity() override;
    WSError UpdateOrientation() override;
    WSError UpdateSessionViewportConfig(const SessionViewportConfig& config) override;
    WSError HandleBackEvent() override;
    WSError MarkProcessed(int32_t eventId) override;
    WSError UpdateFocus(bool isFocused) override;
    WSError NotifyDestroy() override;
    WSError NotifyCloseExistPipWindow() override;
    WSError NotifyTransferComponentData(const AAFwk::WantParams& wantParams) override;
    WSErrorCode NotifyTransferComponentDataSync(const AAFwk::WantParams& wantParams,
                                                AAFwk::WantParams& reWantParams) override;
    void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
                                      const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    void NotifyScreenshot() override;
    void DumpSessionElementInfo(const std::vector<std::string>& params)  override;
    WSError NotifyTouchOutside() override;
    WSError NotifyWindowVisibility(bool isVisible) override;
    WSError UpdateWindowMode(WindowMode mode) override;
    void NotifyForegroundInteractiveStatus(bool interactive) override;
    WSError UpdateMaximizeMode(MaximizeMode mode) override;
    void NotifySessionForeground(uint32_t reason, bool withAnimation) override;
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits) override;
    WSError NotifyDensityFollowHost(bool isFollowHost, float densityValue) override;
    WSError UpdateTitleInTargetPos(bool isShow, int32_t height) override;
    void NotifyTransformChange(const Transform& transform) override;
    void NotifySingleHandTransformChange(const SingleHandTransform& singleHandTransform) override;
    WSError NotifyDialogStateChange(bool isForeground) override;
    WSError SetPipActionEvent(const std::string& action, int32_t status) override;
    WSError SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status) override;
    WSError NotifyPipWindowSizeChange(uint32_t width, uint32_t height, double scale) override;
    WSError UpdateDisplayId(uint64_t displayId) override;
    void NotifyDisplayMove(DisplayId from, DisplayId to) override;
    WSError SwitchFreeMultiWindow(bool enable) override;
    WSError GetUIContentRemoteObj(sptr<IRemoteObject>& uiContentRemoteObj) override;
    void NotifyKeyboardPanelInfoChange(const KeyboardPanelInfo& keyboardPanelInfo) override;
    WSError CompatibleFullScreenRecover() override;
    WSError CompatibleFullScreenMinimize() override;
    WSError CompatibleFullScreenClose() override;
    WSError PcAppInPadNormalClose() override;
    WSError NotifyCompatibleModeEnableInPad(bool enable) override;
    void SetUniqueVirtualPixelRatio(bool useUniqueDensity, float virtualPixelRatio) override;
    void NotifySessionFullScreen(bool fullScreen) override;

    // UIExtension
    WSError NotifyDumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override;
    WSError SendExtensionData(MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

    WSError SetDragActivated(bool dragActivated) override;
    WSError SetSplitButtonVisible(bool isVisible) override;
    WSError SetEnableDragBySystem(bool dragEnable) override;
    WSError SetFullScreenWaterfallMode(bool isWaterfallMode) override;
    WSError SetSupportEnterWaterfallMode(bool isSupportEnter) override;
    WSError SendContainerModalEvent(const std::string& eventName, const std::string& eventValue) override;
    WSError NotifyHighlightChange(bool isHighlight) override;
    void NotifyWindowCrossAxisChange(CrossAxisState state) override;
    WSError NotifyWindowAttachStateChange(bool isAttach) override;
    void NotifyKeyboardAnimationCompleted(const KeyboardPanelInfo& keyboardPanelInfo) override;

private:
    static inline BrokerDelegator<SessionStageProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_SESSION_STAGE_RPOXY_H
