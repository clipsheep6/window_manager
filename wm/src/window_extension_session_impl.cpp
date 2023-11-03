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

#include "window_extension_session_impl.h"

#include <transaction/rs_transaction.h>
#include "window_manager_hilog.h"
#include "app_mgr_client.h"
#include "app_mgr_constants.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionSessionImpl"};
}

WindowExtensionSessionImpl::WindowExtensionSessionImpl(const sptr<WindowOption>& option) : WindowSessionImpl(option)
{
}

WindowExtensionSessionImpl::~WindowExtensionSessionImpl()
{
}

WMError WindowExtensionSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    if (!context || !iSession) {
        WLOGFE("context is nullptr: %{public}u or sessionToken is nullptr: %{public}u",
            context == nullptr, iSession == nullptr);
        return WMError::WM_ERROR_NULLPTR;
    }
    hostSession_ = iSession;
    context_ = context;
    Connect();
    state_ = WindowState::STATE_CREATED;
    return WMError::WM_OK;
}

WMError WindowExtensionSessionImpl::MoveTo(int32_t x, int32_t y)
{
    WLOGFD("Id:%{public}d MoveTo %{public}d %{public}d", property_->GetPersistentId(), x, y);
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& rect = property_->GetWindowRect();
    WSRect wsRect = { x, y, rect.width_, rect.height_ };
    WSError error = UpdateRect(wsRect, SizeChangeReason::MOVE);
    return static_cast<WMError>(error);
}

WMError WindowExtensionSessionImpl::Resize(uint32_t width, uint32_t height)
{
    WLOGFD("Id:%{public}d Resize %{public}u %{public}u", property_->GetPersistentId(), width, height);
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& rect = property_->GetWindowRect();
    WSRect wsRect = { rect.posX_, rect.posY_, width, height };
    WSError error = UpdateRect(wsRect, SizeChangeReason::RESIZE);
    return static_cast<WMError>(error);
}

WMError WindowExtensionSessionImpl::TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_REPEAT_OPERATION;
    }
    return static_cast<WMError>(hostSession_->TransferAbilityResult(resultCode, want));
}

WMError WindowExtensionSessionImpl::TransferExtensionData(const AAFwk::WantParams& wantParams)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_REPEAT_OPERATION;
    }
    return static_cast<WMError>(hostSession_->TransferExtensionData(wantParams));
}

void WindowExtensionSessionImpl::RegisterTransferComponentDataListener(const NotifyTransferComponentDataFunc& func)
{
    /* return RegisterTransferComponentDataForResultListener([](
            const AAFwk::WantParams& wantParams) {
            // test
            if (wantParams.HasParam("data")) {
                WLOGFI("%{public}s for test, input param data=%{public}s", __FUNCTION__, wantParams.GetStringParam("data").c_str());
            }
            
            AAFwk::Want wantDataTest;
            wantDataTest.SetParam("key2", std::string("val5"));
            wantDataTest.SetParam("key3", false);
            wantDataTest.SetParam("key4", 1005);
            return wantDataTest.GetParams();
            // test end
        }); */

    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return;
    }
    notifyTransferComponentDataFunc_ = std::move(func);
    hostSession_->NotifyRemoteReady();
}

WSError WindowExtensionSessionImpl::NotifyTransferComponentData(const AAFwk::WantParams& wantParams)
{
    if (notifyTransferComponentDataFunc_) {
        notifyTransferComponentDataFunc_(wantParams);
    }
    return WSError::WS_OK;
}

WSErrorCode WindowExtensionSessionImpl::NotifyTransferComponentDataSync(const AAFwk::WantParams& wantParams, AAFwk::WantParams& reWantParams)
{
    WLOGFI("%{public}s for test", __FUNCTION__);
    if (notifyTransferComponentDataForResultFunc_) {
        reWantParams = notifyTransferComponentDataForResultFunc_(wantParams);
        return WSErrorCode::WS_OK;
    }
    return WSErrorCode::WS_ERROR_NOT_REGISTER_SYNC_CALLBACK;
}

void WindowExtensionSessionImpl::RegisterTransferComponentDataForResultListener(const NotifyTransferComponentDataForResultFunc& func)
{
    WLOGFI("%{public}s for test", __FUNCTION__);
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return;
    }
    notifyTransferComponentDataForResultFunc_ = std::move(func);
    hostSession_->NotifyRemoteReadySync();
}

WMError WindowExtensionSessionImpl::SetPrivacyMode(bool isPrivacyMode)
{
    WLOGFD("id : %{public}u, SetPrivacyMode, %{public}u", GetWindowId(), isPrivacyMode);
    if (surfaceNode_ == nullptr) {
        WLOGFE("surfaceNode_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    surfaceNode_->SetSecurityLayer(isPrivacyMode);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

void WindowExtensionSessionImpl::NotifyFocusStateEvent(bool focusState)
{
    if (uiContent_) {
        focusState ? uiContent_->Focus() : uiContent_->UnFocus();
    }
    focusState_ = focusState;
}

void WindowExtensionSessionImpl::NotifyFocusActiveEvent(bool isFocusActive)
{
    if (uiContent_) {
        uiContent_->SetIsFocusActive(isFocusActive);
    }
}

void WindowExtensionSessionImpl::NotifyBackpressedEvent(bool& isConsumed)
{
    if (uiContent_) {
        WLOGFD("Transfer backpressed event to uiContent");
        isConsumed = uiContent_->ProcessBackPressed();
    }
    WLOGFD("Backpressed event is not cosumed");
}

void WindowExtensionSessionImpl::NotifyConfigurationUpdated()
{
    auto appMgrClient = std::make_shared<AppExecFwk::AppMgrClient>();
    if (appMgrClient->ConnectAppMgrService() != AppExecFwk::AppMgrResultCode::RESULT_OK) {
        WLOGFE("Failed to connnect AppManagerService");
        return;
    }
    auto systemConfig = AppExecFwk::Configuration();
    appMgrClient->GetConfiguration(systemConfig);
    auto newConfig = std::make_shared<AppExecFwk::Configuration>(systemConfig);
    if (uiContent_) {
        uiContent_->UpdateConfiguration(newConfig);
    }
}

WMError WindowExtensionSessionImpl::NapiSetUIContent(const std::string& contentInfo,
    napi_env env, napi_value storage, bool isdistributed, AppExecFwk::Ability* ability)
{
    WLOGFD("WindowExtensionSessionImpl NapiSetUIContent: %{public}s state:%{public}u", contentInfo.c_str(), state_);
    if (uiContent_) {
        uiContent_->Destroy();
    }
    std::unique_ptr<Ace::UIContent> uiContent;
    if (ability != nullptr) {
        uiContent = Ace::UIContent::Create(ability);
    } else {
        uiContent = Ace::UIContent::Create(context_.get(), reinterpret_cast<NativeEngine*>(env));
    }
    if (uiContent == nullptr) {
        WLOGFE("fail to NapiSetUIContent id: %{public}d", GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->Initialize(this, contentInfo, storage, property_->GetParentId());
    // make uiContent available after Initialize/Restore
    uiContent_ = std::move(uiContent);

    if (focusState_ != std::nullopt) {
        focusState_.value() ? uiContent_->Focus() : uiContent_->UnFocus();
    }

    uint32_t version = 0;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        version = context_->GetApplicationInfo()->apiCompatibleVersion;
    }
    // 10 ArkUI new framework support after API10
    if (version < 10) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
        if (!isSystembarPropertiesSet_) {
            SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SystemBarProperty());
        }
    } else if (isIgnoreSafeAreaNeedNotify_) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
    }

    UpdateDecorEnable(true);
    if (state_ == WindowState::STATE_SHOWN) {
        // UIContent may be nullptr when show window, need to notify again when window is shown
        uiContent_->Foreground();
        UpdateTitleButtonVisibility();
    }
    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    WLOGFD("notify uiContent window size change end");
    return WMError::WM_OK;
}

WSError WindowExtensionSessionImpl::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    WLOGFI("WindowExtensionSessionImpl Update rect [%{public}d, %{public}d, reason: %{public}d]", rect.width_,
        rect.height_, static_cast<int>(reason));
    auto wmReason = static_cast<WindowSizeChangeReason>(reason);
    Rect wmRect = {rect.posX_, rect.posY_, rect.width_, rect.height_};
    property_->SetWindowRect(wmRect);
    NotifySizeChange(wmRect, wmReason);
    UpdateViewportConfig(wmRect, wmReason);
    return WSError::WS_OK;
}
} // namespace Rosen
} // namespace OHOS
