/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "window_impl.h"

#include "window_manager_hilog.h"
#include "window_helper.h"
#include "window_option.h"
#include "viewport_config.h"
#include "singleton_container.h"
#include "vsync_station.h"
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImpl"};
}
std::map<std::string, std::pair<uint32_t, sptr<Window>>> WindowImpl::windowMap_;
std::map<uint32_t, std::vector<sptr<WindowImpl>>> WindowImpl::subWindowMap_;
static int constructorCnt = 0;
static int deConstructorCnt = 0;
WindowImpl::WindowImpl(const sptr<WindowOption>& option)
{
    // property_ = new (std::nothrow) WindowProperty();
    // property_->SetWindowName(option->GetWindowName());
    // property_->SetRequestRect(option->GetWindowRect());
    // property_->SetWindowType(option->GetWindowType());
    // property_->SetWindowMode(option->GetWindowMode());
    // property_->SetFullScreen(option->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    // property_->SetFocusable(option->GetFocusable());
    // property_->SetTouchable(option->GetTouchable());
    // property_->SetDisplayId(option->GetDisplayId());
    // property_->SetCallingWindow(option->GetCallingWindow());
    // property_->SetWindowFlags(option->GetWindowFlags());
    // property_->SetHitOffset(option->GetHitOffset());
    // property_->SetRequestedOrientation(option->GetRequestedOrientation());
    // property_->SetTurnScreenOn(option->IsTurnScreenOn());
    // property_->SetKeepScreenOn(option->IsKeepScreenOn());
    // property_->SetBrightness(option->GetBrightness());
    // auto& sysBarPropMap = option->GetSystemBarProperty();
    // for (auto it : sysBarPropMap) {
    //     property_->SetSystemBarProperty(it.first, it.second);
    // }
    // name_ = option->GetWindowName();

    surfaceNode_ = CreateSurfaceNode("preview_surface", option->GetWindowType());

    WLOGFI("WindowImpl constructorCnt: %{public}d name: %{public}s",
        ++constructorCnt, property_->GetWindowName().c_str());
}

WindowImpl::~WindowImpl()
{
    WLOGFI("windowName: %{public}s, windowId: %{public}d, deConstructorCnt: %{public}d",
        GetWindowName().c_str(), GetWindowId(), ++deConstructorCnt);
    Destroy();
}

RSSurfaceNode::SharedPtr WindowImpl::CreateSurfaceNode(std::string name, WindowType type)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = name;
    RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
    switch (type) {
        case WindowType::WINDOW_TYPE_BOOT_ANIMATION:
        case WindowType::WINDOW_TYPE_POINTER:
            rsSurfaceNodeType = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
            break;
        case WindowType::WINDOW_TYPE_APP_MAIN_WINDOW:
            rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
            break;
        default:
            rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
            break;
    }
    return RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType);
}

sptr<Window> WindowImpl::Find(const std::string& name)
{
    return nullptr;
}

const std::shared_ptr<AbilityRuntime::Context> WindowImpl::GetContext() const
{
    return nullptr;
}

sptr<Window> WindowImpl::GetTopWindowWithId(uint32_t mainWinId)
{
    return nullptr;
}

sptr<Window> WindowImpl::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    return nullptr;
    // if (windowMap_.empty()) {
    //     WLOGFE("Please create mainWindow First!");
    //     return nullptr;
    // }
    // uint32_t mainWinId = INVALID_WINDOW_ID;
    // WLOGFI("GetTopWindowfinal MainWinId:%{public}u!", mainWinId);
    // if (mainWinId == INVALID_WINDOW_ID) {
    //     WLOGFE("Cannot find topWindow!");
    //     return nullptr;
    // }
    // return GetTopWindowWithId(mainWinId);
}

std::vector<sptr<Window>> WindowImpl::GetSubWindow(uint32_t parentId)
{
    return std::vector<sptr<Window>>();
    // if (subWindowMap_.find(parentId) == subWindowMap_.end()) {
    //     WLOGFE("Cannot parentWindow with id: %{public}u!", parentId);
    //     return std::vector<sptr<Window>>();
    // }
    // return std::vector<sptr<Window>>(subWindowMap_[parentId].begin(), subWindowMap_[parentId].end());
}

void WindowImpl::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    for (const auto& winPair : windowMap_) {
        auto window = winPair.second.second;
        window->UpdateConfiguration(configuration);
    }
}

std::shared_ptr<RSSurfaceNode> WindowImpl::GetSurfaceNode() const
{
    return surfaceNode_;
}

Rect WindowImpl::GetRect() const
{
    return property_->GetWindowRect();
}

Rect WindowImpl::GetRequestRect() const
{
    return property_->GetRequestRect();
}

WindowType WindowImpl::GetType() const
{
    return property_->GetWindowType();
}

WindowMode WindowImpl::GetMode() const
{
    return property_->GetWindowMode();
}

float WindowImpl::GetAlpha() const
{
    return property_->GetAlpha();
}

WindowState WindowImpl::GetWindowState() const
{
    return state_;
}

WMError WindowImpl::SetFocusable(bool isFocusable)
{
    return WMError::WM_OK;
}

bool WindowImpl::GetFocusable() const
{
    return property_->GetFocusable();
}

WMError WindowImpl::SetTouchable(bool isTouchable)
{
    return WMError::WM_OK;
}

bool WindowImpl::GetTouchable() const
{
    return property_->GetTouchable();
}

const std::string& WindowImpl::GetWindowName() const
{
    return name_;
}

uint32_t WindowImpl::GetWindowId() const
{
    return property_->GetWindowId();
}

uint32_t WindowImpl::GetWindowFlags() const
{
    return property_->GetWindowFlags();
}

uint32_t WindowImpl::GetRequestModeSupportInfo() const
{
    return property_->GetRequestModeSupportInfo();
}

bool WindowImpl::IsMainHandlerAvailable() const
{
    return true;
}

SystemBarProperty WindowImpl::GetSystemBarPropertyByType(WindowType type) const
{
    auto curProperties = property_->GetSystemBarProperty();
    return curProperties[type];
}

WMError WindowImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowType(WindowType type)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowMode(WindowMode mode)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetAlpha(float alpha)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetTransform(const Transform& trans)
{
    return WMError::WM_OK;
}

const Transform& WindowImpl::GetTransform() const
{
    return property_->GetTransform();
}

WMError WindowImpl::AddWindowFlag(WindowFlag flag)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RemoveWindowFlag(WindowFlag flag)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowFlags(uint32_t flags)
{
    return WMError::WM_OK;
}

void WindowImpl::OnNewWant(const AAFwk::Want& want)
{
    return;
}

WMError WindowImpl::SetUIContent(const std::string& contentInfo,
    NativeEngine* engine, NativeValue* storage, bool isdistributed, AppExecFwk::Ability* ability)
{
    // WLOGFD("SetUIContent: %{public}s", contentInfo.c_str());
    // if (uiContent_) {
    //     uiContent_->Destroy();
    // }
    // std::unique_ptr<Ace::UIContent> uiContent;
    // if (ability != nullptr) {
    //     uiContent = Ace::UIContent::Create(ability);
    // } else {
    //     uiContent = Ace::UIContent::Create(context_.get(), engine);
    // }
    // if (uiContent == nullptr) {
    //     WLOGFE("fail to SetUIContent id: %{public}u", property_->GetWindowId());
    //     return WMError::WM_ERROR_NULLPTR;
    // }
    // if (isdistributed) {
    //     uiContent->Restore(this, contentInfo, storage);
    // } else {
    //     uiContent->Initialize(this, contentInfo, storage);
    // }
    // uiContent_ = std::move(uiContent);
    // UpdateViewportConfig();
    return WMError::WM_OK;
}

Ace::UIContent* WindowImpl::GetUIContent() const
{
    return uiContent_.get();
}

std::string WindowImpl::GetContentInfo()
{
    return "";
}

bool WindowImpl::IsSupportWideGamut()
{
    return true;
}

void WindowImpl::SetColorSpace(ColorSpace colorSpace)
{
    return;
}

ColorSpace WindowImpl::GetColorSpace()
{
    return ColorSpace::COLOR_SPACE_DEFAULT;
}

std::shared_ptr<Media::PixelMap> WindowImpl::Snapshot()
{
    return nullptr;
}

void WindowImpl::DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    return;
}

WMError WindowImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetLayoutFullScreen(bool status)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetFullScreen(bool status)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Create(uint32_t parentId, const std::shared_ptr<AbilityRuntime::Context>& context)
{
    WLOGFI("[Client] Window [name:%{public}s] Create", name_.c_str());
    // check window name, same window names are forbidden

    // if (surfaceNode_) {
    //     surfaceNode_->SetWindowId(windowId);
    // }
    
    return WMError::WM_OK;
}

WMError WindowImpl::BindDialogTarget(sptr<IRemoteObject> targetToken)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Destroy()
{
    // if (uiContent_) {
    //     uiContent_->Destroy();
    // }
    return WMError::WM_OK;
}

WMError WindowImpl::UpdateSurfaceNodeAfterCustomAnimation(bool isAdd)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Show(uint32_t reason, bool withAnimation)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    return WMError::WM_OK;
}

WMError WindowImpl::MoveTo(int32_t x, int32_t y)
{
    return WMError::WM_OK;
}

WMError WindowImpl::Resize(uint32_t width, uint32_t height)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetWindowGravity(WindowGravity gravity, uint32_t percent)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetKeepScreenOn(bool keepScreenOn)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsKeepScreenOn() const
{
    return property_->IsKeepScreenOn();
}

WMError WindowImpl::SetTurnScreenOn(bool turnScreenOn)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

WMError WindowImpl::SetBackgroundColor(const std::string& color)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetTransparent(bool isTransparent)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsTransparent() const
{
    return true;
}

WMError WindowImpl::SetBrightness(float brightness)
{
    return WMError::WM_OK;
}

float WindowImpl::GetBrightness() const
{
    return property_->GetBrightness();
}

WMError WindowImpl::SetCallingWindow(uint32_t windowId)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetPrivacyMode(bool isPrivacyMode)
{
    return WMError::WM_OK;
}

bool WindowImpl::IsPrivacyMode() const
{
    return property_->GetPrivacyMode();
}

void WindowImpl::SetSystemPrivacyMode(bool isSystemPrivacyMode)
{
    return;
}

WMError WindowImpl::SetSnapshotSkip(bool isSkip)
{
    return WMError::WM_OK;
}

WMError WindowImpl::DisableAppWindowDecor()
{
    return WMError::WM_OK;
}

bool WindowImpl::IsDecorEnable() const
{
    return property_->GetDecorEnable();
}

WMError WindowImpl::Maximize()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Minimize()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Recover()
{
    return WMError::WM_OK;
}

WMError WindowImpl::Close()
{
    return WMError::WM_OK;
}

void WindowImpl::StartMove()
{
    return;
}

WMError WindowImpl::RequestFocus() const
{
    return WMError::WM_OK;
}

bool WindowImpl::IsFocused() const
{
    return true;
}

void WindowImpl::SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer)
{
    return;
}

WMError WindowImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterDragListener(const sptr<IWindowDragListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterDragListener(const sptr<IWindowDragListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    return WMError::WM_OK;
}

void WindowImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    return;
}

WMError WindowImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterAnimationTransitionController(const sptr<IAnimationTransitionController>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowImpl::UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    return WMError::WM_OK;
}

void WindowImpl::RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    return;
}

void WindowImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    return;
}

void WindowImpl::SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler)
{
    return;
}

void WindowImpl::SetRequestModeSupportInfo(uint32_t modeSupportInfo)
{
    return;
}

void WindowImpl::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    std::shared_ptr<InputEventListener> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    if (inputEventConsumer != nullptr) {
        WLOGD("Transfer key event to inputEventConsumer");
        (void)inputEventConsumer->OnInputEvent(keyEvent);
    } else if (uiContent_ != nullptr) {
        WLOGD("Transfer key event to uiContent");
        uiContent_->ProcessKeyEvent(keyEvent);
    } else {
        WLOGFE("There is no key event consumer");
    }

    if (GetType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
        WLOGFI("DispatchKeyEvent: %{public}u", GetWindowId());
        keyEvent->MarkProcessed();
        return;
    }
}

void WindowImpl::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    // If windowRect transformed, transform event back to its origin position
    if (property_) {
        property_->UpdatePointerEvent(pointerEvent);
    }

    if (IsPointerEventConsumed()) {
        pointerEvent->MarkProcessed();
        return;
    }

    TransferPointerEvent(pointerEvent);
}


void WindowImpl::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
   VsyncStation::GetInstance().RequestVsync(vsyncCallback);
}

void WindowImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_ != nullptr) {
        WLOGFD("notify ace winId:%{public}u", GetWindowId());
        uiContent_->UpdateConfiguration(configuration);
    }
    if (subWindowMap_.count(GetWindowId()) == 0) {
        return;
    }
    for (auto& subWindow : subWindowMap_.at(GetWindowId())) {
        subWindow->UpdateConfiguration(configuration);
    }
}

void WindowImpl::NotifyTouchDialogTarget()
{
    return;
}

void WindowImpl::SetNeedRemoveWindowInputChannel(bool needRemoveWindowInputChannel)
{
    needRemoveWindowInputChannel_ = needRemoveWindowInputChannel;
}

bool WindowImpl::IsLayoutFullScreen() const
{
    return true;
}

bool WindowImpl::IsFullScreen() const
{
    return true;
}

void WindowImpl::SetRequestedOrientation(Orientation orientation)
{
    if (property_->GetRequestedOrientation() == orientation) {
        return;
    }
    property_->SetRequestedOrientation(orientation);
    if (state_ == WindowState::STATE_SHOWN) {
        UpdateProperty(PropertyChangeAction::ACTION_UPDATE_ORIENTATION);
    }
}

WMError WindowImpl::UpdateProperty(PropertyChangeAction action)
{
    return WMError::WM_OK;
}

Orientation WindowImpl::GetRequestedOrientation()
{
    return property_->GetRequestedOrientation();
}

WMError WindowImpl::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    return WMError::WM_OK;
}
void WindowImpl::GetRequestedTouchHotAreas(std::vector<Rect>& rects) const
{
    property_->GetTouchHotAreas(rects);
}

WMError WindowImpl::SetAPPWindowLabel(const std::string& label)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowTitle(label);
    return WMError::WM_OK;
}

WMError WindowImpl::SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    if (icon == nullptr) {
        WLOGFE("window icon is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowIcon(icon);
    return WMError::WM_OK;
}

WMError WindowImpl::SetCornerRadius(float cornerRadius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowRadius(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowColor(std::string color)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowOffsetX(float offsetX)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetShadowOffsetY(float offsetY)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBlur(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBackdropBlur(float radius)
{
    return WMError::WM_OK;
}

WMError WindowImpl::SetBackdropBlurStyle(WindowBlurStyle blurStyle)
{
    return WMError::WM_OK;
}

WMError WindowImpl::NotifyMemoryLevel(int32_t level) const
{
    return WMError::WM_OK;
}

bool WindowImpl::IsAllowHaveSystemSubWindow()
{
    return true;
}

WmErrorCode WindowImpl::RaiseToAppTop()
{
    return WmErrorCode::WM_OK;
}

WMError WindowImpl::SetAspectRatio(float ratio)
{
    return WMError::WM_OK;
}

WMError WindowImpl::ResetAspectRatio()
{
    return WMError::WM_OK;
}

KeyboardAnimationConfig WindowImpl::GetKeyboardAnimationConfig()
{
    return keyboardAnimationConfig_;
}

void WindowImpl::SetNeedDefaultAnimation(bool needDefaultAnimation)
{
    return;
}

bool WindowImpl::IsPointerEventConsumed()
{
    return moveDragProperty_->startDragFlag_ || moveDragProperty_->startMoveFlag_;
}

void WindowImpl::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (windowSystemConfig_.isStretchable_ && GetMode() == WindowMode::WINDOW_MODE_FLOATING) {
        UpdatePointerEventForStretchableWindow(pointerEvent);
    }

    std::shared_ptr<InputEventListener> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    if (inputEventConsumer != nullptr) {
        WLOGFD("Transfer pointer event to inputEventConsumer");
        (void)inputEventConsumer->OnInputEvent(pointerEvent);
    } else if (uiContent_ != nullptr) {
        WLOGFD("Transfer pointer event to uiContent");
        (void)uiContent_->ProcessPointerEvent(pointerEvent);
    } else {
        WLOGFW("pointerEvent is not consumed, windowId: %{public}u", GetWindowId());
        pointerEvent->MarkProcessed();
    }
}


void WindowImpl::UpdatePointerEventForStretchableWindow(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        WLOGFW("Point item is invalid");
        return;
    }
    const Rect& originRect = property_->GetOriginRect();
    PointInfo originPos =
        WindowHelper::CalculateOriginPosition(originRect, GetRect(),
        { pointerItem.GetDisplayX(), pointerItem.GetDisplayY() });
    pointerItem.SetDisplayX(originPos.x);
    pointerItem.SetDisplayY(originPos.y);
    pointerItem.SetWindowX(originPos.x - originRect.posX_);
    pointerItem.SetWindowY(originPos.y - originRect.posY_);
    pointerEvent->UpdatePointerItem(pointerEvent->GetPointerId(), pointerItem);
}

void WindowImpl::UpdateViewportConfig()
{
    Ace::ViewportConfig config;
    config.SetSize(width_, height_);
    config.SetPosition(0, 0);
    config.SetDensity(density_);
    config.SetOrientation(orientation_);
    uiContent_->UpdateViewportConfig(config/*, WindowSizeChangeReason::UNDEFINED, nullptr*/);
}

void WindowImpl::SetOrientation(Orientation orientation)
{
    WLOGFD("SetOrientation : orientation=%{public}d", static_cast<int32_t>(orientation));
    if (orientation_ == static_cast<int32_t>(orientation)) {
        return;
    }
    orientation_ = static_cast<int32_t>(orientation);
    UpdateViewportConfig();
}

void WindowImpl::SetSize(int32_t width, int32_t height)
{
    WLOGFD("SetSize : width=%{public}d, height=%{public}d", width, height);
    if (width_ == width && height_ == height) {
        return;
    }
    width_ = width;
    height_ = height;
    UpdateViewportConfig();
}

void WindowImpl::SetDensity(float density)
{
    WLOGFD("SetDensity : density=%{public}f", density);
    if (abs(density_ - density) <= 0.000001) {
        return;
    }
    density_ = density;
    UpdateViewportConfig();
}

} // namespace Rosen
} // namespace OHOS
