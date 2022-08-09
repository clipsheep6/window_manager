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
#include "window_fuzzer.h"
#include <securec.h>
#include "window.h"
#include "window_manager.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}
class WindowLifeCycle : public IWindowLifeCycle {
public:
    virtual void AfterForeground() override
    {
    }
    virtual void AfterBackground() override
    {
    }
    virtual void AfterFocused() override
    {
    }
    virtual void AfterUnfocused() override
    {
    }
    virtual void AfterActive() override
    {
    }
    virtual void AfterInactive() override
    {
    }
};

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

class FocusChangedListener : public IFocusChangedListener {
public:
    virtual void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }

    virtual void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override
    {
    }
};

class SystemBarChangedListener : public ISystemBarChangedListener {
public:
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
    }
};

class VisibilityChangedListener : public IVisibilityChangedListener {
public:
    virtual void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
    }
};

class WindowUpdateListener : public IWindowUpdateListener {
public:
    virtual void OnWindowUpdate(const sptr<AccessibilityWindowInfo>& windowInfo, WindowUpdateType type) override
    {
    }
};

    class WindowChangeListener : public IWindowChangeListener {
        virtual void OnSizeChange(Rect rect, WindowSizeChangeReason reason) override {

        }
        virtual void OnModeChange(WindowMode mode) override {

        }
    };

    class AvoidAreaChangedListener : public IAvoidAreaChangedListener {
        virtual void OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type) override {

        }
    };

    class WindowDragListener : public IWindowDragListener {
        virtual void OnDrag(int32_t x, int32_t y, DragEvent event) override {

        }
    };

    class DisplayMoveListener : public IDisplayMoveListener {
        virtual void OnDisplayMove(DisplayId from, DisplayId to) override {

        }
    };


    class OccupiedAreaChangeListener : public IOccupiedAreaChangeListener {
        virtual void OnSizeChange(const sptr<OccupiedAreaChangeInfo> &info) override {

        }
    };

    class TouchOutsideListener : public ITouchOutsideListener {
        virtual void OnTouchOutside() const override {

        }
    };

    class AceAbilityHandler : public IAceAbilityHandler {
        virtual void SetBackgroundColor(uint32_t color) override {

        }
        virtual uint32_t GetBackgroundColor() override {

        }
    };

bool test(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < DATA_MIN_SIZE) {
            return false;
        }
        std::string name = "WindowFuzzTest";
        sptr<WindowOption> option = nullptr;
        sptr<Window> window = Window::Create(name, option);
        if (window == nullptr) {
            return false;
        }

        sptr<IWindowLifeCycle> lifeCycleListener = new WindowLifeCycle();
        sptr<IWindowChangeListener> windowChangeListener = new WindowChangeListener();
        sptr<IAvoidAreaChangedListener> avoidAreaChangedListener = new AvoidAreaChangedListener();
        sptr<IWindowDragListener> windowDragListener = new WindowDragListener();
        sptr<IDisplayMoveListener> displayMoveListener = new DisplayMoveListener();
        sptr<IOccupiedAreaChangeListener> occupiedAreaChangeListener = new OccupiedAreaChangeListener();
        sptr<ITouchOutsideListener> touchOutsideListener = new TouchOutsideListener();
        sptr<IAceAbilityHandler> handler = new AceAbilityHandler();
        window->RegisterLifeCycleListener(lifeCycleListener);
        window->RegisterWindowChangeListener(windowChangeListener);
        window->RegisterAvoidAreaChangeListener(avoidAreaChangedListener);
        window->RegisterDragListener(windowDragListener);
        window->RegisterDisplayMoveListener(displayMoveListener);
        window->RegisterOccupiedAreaChangeListener(occupiedAreaChangeListener);
        window->RegisterTouchOutsideListener(touchOutsideListener);

        size_t startPos = 0;
        bool focusable;
        startPos += GetObject<bool>(focusable, data + startPos, size - startPos);
        window->SetFocusable(focusable);
        bool touchable;
        startPos += GetObject<bool>(touchable, data + startPos, size - startPos);
        window->SetTouchable(touchable);
        WindowType type;
        startPos += GetObject<WindowType>(type, data + startPos, size - startPos);
        window->SetWindowType(type);
        WindowMode mode;
        startPos += GetObject<WindowMode>(mode, data + startPos, size - startPos);
        window->SetWindowMode(mode);
        WindowBlurLevel windowBlurLevel;
        startPos += GetObject<WindowBlurLevel>(windowBlurLevel, data + startPos, size - startPos);
        window->SetWindowBackgroundBlur(windowBlurLevel);
        float alpha;
        startPos += GetObject<float>(alpha, data + startPos, size - startPos);
        window->SetAlpha(alpha);
        const Transform transform;
        startPos += GetObject<const Transform>(transform, data + startPos, size - startPos);
        window->SetTransform(transform);
        WindowFlag flag;
        startPos += GetObject<WindowFlag>(flag, data + startPos, size - startPos);
        window->AddWindowFlag(flag);
        uint32_t flags;
        startPos += GetObject<uint32_t>(flags, data + startPos, size - startPos);
        window->SetWindowFlags(flags);
        SystemBarProperty statusBarProperty;
        startPos += GetObject<SystemBarProperty>(statusBarProperty, data + startPos, size - startPos);
        window->SetSystemBarProperty(type, statusBarProperty);
        bool layoutFullScreenStatus;
        startPos += GetObject<bool>(layoutFullScreenStatus, data + startPos, size - startPos);
        window->SetLayoutFullScreen(layoutFullScreenStatus);
        bool fullScreenStatus;
        startPos += GetObject<bool>(fullScreenStatus, data + startPos, size - startPos);
        window->SetFullScreen(fullScreenStatus);
        window->Show();
        int32_t x;
        startPos += GetObject<int32_t>(x, data + startPos, size - startPos);
        int32_t y;
        startPos += GetObject<int32_t>(y, data + startPos, size - startPos);
        window->MoveTo(x, y);
        uint32_t width;
        startPos += GetObject<uint32_t>(width, data + startPos, size - startPos);
        uint32_t height;
        startPos += GetObject<uint32_t>(height, data + startPos, size - startPos);
        window->Resize(width, height);
        bool keepScreenOn;
        startPos += GetObject<bool>(keepScreenOn, data + startPos, size - startPos);
        window->SetKeepScreenOn(keepScreenOn);
        bool turnScreenOn;
        startPos += GetObject<bool>(turnScreenOn, data + startPos, size - startPos);
        window->SetTurnScreenOn(turnScreenOn);
        char color[LEN + 1];
        color[LEN] = END_CHAR;
        for (int i = 0; i < LEN; ++i) {
            startPos += GetObject<char>(color[i], data + startPos, size - startPos);
        }
        std::string colorStr(color);
        window->SetBackgroundColor(colorStr);
        bool isTransparent;
        startPos += GetObject<bool>(isTransparent, data + startPos, size - startPos);
        window->SetTransparent(isTransparent);
        float brightness;
        startPos += GetObject<float>(brightness, data + startPos, size - startPos);
        window->SetBrightness(brightness);
        uint32_t windowId;
        startPos += GetObject<uint32_t>(windowId, data + startPos, size - startPos);
        window->SetCallingWindow(windowId);
        bool isPrivacyMode;
        startPos += GetObject<bool>(isPrivacyMode, data + startPos, size - startPos);
        window->SetCallingWindow(isPrivacyMode);


        window->UnregisterLifeCycleListener(lifeCycleListener);
        window->UnregisterWindowChangeListener(windowChangeListener);
        window->UnregisterAvoidAreaChangeListener(avoidAreaChangedListener);
        window->UnregisterDragListener(windowDragListener);
        window->UnregisterDisplayMoveListener(displayMoveListener);
        window->UnregisterOccupiedAreaChangeListener(occupiedAreaChangeListener);
        window->UnregisterTouchOutsideListener(touchOutsideListener);

        uint32_t modeSupportInfo;
        startPos += GetObject(modeSupportInfo, data + startPos, size - startPos);
        window->SetRequestModeSupportInfo(modeSupportInfo);
        bool needRemoveWindowInputChannel;
        startPos += GetObject(needRemoveWindowInputChannel, data + startPos, size - startPos);
        window->SetNeedRemoveWindowInputChannel(needRemoveWindowInputChannel);
        ColorSpace colorSpace;
        startPos += GetObject(colorSpace, data + startPos, size - startPos);
        window->SetColorSpace(colorSpace);

        window->Hide(0);
        window->Destroy();
        return true;
    }



bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    /*if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    std::string name = "WindowFuzzTest";
    sptr<WindowOption> option = nullptr;
    sptr<Window> window = Window::Create(name, option);
    if (window == nullptr) {
        return false;
    }
    window->Show(0);

    Orientation orientation = static_cast<Orientation>(data[0]);
    window->SetRequestedOrientation(static_cast<Orientation>(data[0]));
    if (window->GetRequestedOrientation() != orientation) {
        return false;
    }

    window->Hide(0);
    window->Destroy();*/
    test(data, size);
    return true;
}



} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

