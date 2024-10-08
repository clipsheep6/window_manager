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

#include <parcel.h>

#include <securec.h>

#include "marshalling_helper.h"
#include "window_manager.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

template<class T>
size_t GetObject(T& object, const uint8_t* data, size_t size)
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
    virtual void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
    }
};

class CameraFloatWindowChangedListener : public ICameraFloatWindowChangedListener {
public:
    void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) override
    {
    }
};

class WaterMarkFlagChangedListener : public IWaterMarkFlagChangedListener {
public:
    void OnWaterMarkFlagUpdate(bool showWaterMark) override
    {
    }
};

class GestureNavigationEnabledChangedListener : public IGestureNavigationEnabledChangedListener {
public:
    void OnGestureNavigationEnabledUpdate(bool enable) override
    {
    }
};

bool DoSomethingForWindowManagerImpl(WindowManager& windowManager, const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    size_t startPos = 0;
    uint32_t accessTokenId;
    bool isShowing;
    startPos += GetObject<uint32_t>(accessTokenId, data + startPos, size - startPos);
    startPos += GetObject<bool>(isShowing, data + startPos, size - startPos);
    windowManager.UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    bool enable;
    startPos += GetObject<bool>(enable, data + startPos, size - startPos);
    windowManager.SetGestureNavigationEnabled(enable);

    DisplayId displayId;
    SystemBarRegionTints tints;
    startPos += GetObject<DisplayId>(displayId, data + startPos, size - startPos);
    GetObject<SystemBarRegionTints>(tints, data + startPos, size - startPos);
    windowManager.UpdateSystemBarRegionTints(displayId, tints);

    return true;
}

void CheckAccessibilityWindowInfo(WindowManager& windowManager, const uint8_t* data, size_t size)
{
    Parcel accessibilityWindowInfosParcel;
    if (!accessibilityWindowInfosParcel.WriteBuffer(data, size)) {
        return;
    }
    std::vector<sptr<AccessibilityWindowInfo>> windowInfos;
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<AccessibilityWindowInfo>(
        accessibilityWindowInfosParcel, windowInfos)) {
        return;
    }
    windowManager.GetAccessibilityWindowInfo(windowInfos);
    WindowUpdateType type;
    GetObject<WindowUpdateType>(type, data, size);
    windowManager.NotifyAccessibilityWindowInfo(windowInfos, type);
}

void CheckVisibilityInfo(WindowManager& windowManager, const uint8_t* data, size_t size)
{
    Parcel windowVisibilityInfosParcel;
    if (!windowVisibilityInfosParcel.WriteBuffer(data, size)) {
        return;
    }
    std::vector<sptr<WindowVisibilityInfo>> visibilitynfos;
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<WindowVisibilityInfo>(windowVisibilityInfosParcel,
            visibilitynfos)) {
        return;
    }
    windowManager.GetVisibilityWindowInfo(visibilitynfos);
    windowManager.UpdateWindowVisibilityInfo(visibilitynfos);
    bool enable = false;
    windowManager.SetGestureNavigationEnabled(enable);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    WindowManager& windowManager = WindowManager::GetInstance();
    CheckAccessibilityWindowInfo(windowManager, data, size);
    CheckVisibilityInfo(windowManager, data, size);
    Parcel focusChangeInfoParcel;
    if (focusChangeInfoParcel.WriteBuffer(data, size)) {
        FocusChangeInfo::Unmarshalling(focusChangeInfoParcel);
    }
    Parcel parcel;
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo();
    focusChangeInfo->Marshalling(parcel);
    Parcel windowVisibilityInfoParcel;
    if (windowVisibilityInfoParcel.WriteBuffer(data, size)) {
        WindowVisibilityInfo::Unmarshalling(windowVisibilityInfoParcel);
    }
    sptr<WindowVisibilityInfo> windowVisibilityInfo = new WindowVisibilityInfo();
    windowVisibilityInfo->Marshalling(parcel);
    Parcel accessibilityWindowInfoParcel;
    if (accessibilityWindowInfoParcel.WriteBuffer(data, size)) {
        AccessibilityWindowInfo::Unmarshalling(accessibilityWindowInfoParcel);
    }
    sptr<AccessibilityWindowInfo> accessibilityWindowInfo = new AccessibilityWindowInfo();
    accessibilityWindowInfo->Marshalling(parcel);
    windowManager.MinimizeAllAppWindows(static_cast<DisplayId>(data[0]));
    sptr<IFocusChangedListener> focusChangedListener = new FocusChangedListener();
    windowManager.RegisterFocusChangedListener(focusChangedListener);
    sptr<ISystemBarChangedListener> systemBarChangedListener = new SystemBarChangedListener();
    windowManager.RegisterSystemBarChangedListener(systemBarChangedListener);
    sptr<IVisibilityChangedListener> visibilityChangedListener = new VisibilityChangedListener();
    windowManager.RegisterVisibilityChangedListener(visibilityChangedListener);
    sptr<IWindowUpdateListener> windowUpdateListener = new WindowUpdateListener();
    windowManager.RegisterWindowUpdateListener(windowUpdateListener);
    sptr<ICameraFloatWindowChangedListener> cameraFloatWindowChanagedListener = new CameraFloatWindowChangedListener();
    windowManager.RegisterCameraFloatWindowChangedListener(cameraFloatWindowChanagedListener);
    windowManager.SetWindowLayoutMode(static_cast<WindowLayoutMode>(data[0]));
    windowManager.UnregisterFocusChangedListener(focusChangedListener);
    windowManager.UnregisterSystemBarChangedListener(systemBarChangedListener);
    windowManager.UnregisterVisibilityChangedListener(visibilityChangedListener);
    windowManager.UnregisterWindowUpdateListener(windowUpdateListener);
    windowManager.UnregisterCameraFloatWindowChangedListener(cameraFloatWindowChanagedListener);
    sptr<IWaterMarkFlagChangedListener> waterMarkFlagChangedListener = new WaterMarkFlagChangedListener();
    windowManager.RegisterWaterMarkFlagChangedListener(waterMarkFlagChangedListener);
    windowManager.UnregisterWaterMarkFlagChangedListener(waterMarkFlagChangedListener);
    sptr<IGestureNavigationEnabledChangedListener> gestureListener = new GestureNavigationEnabledChangedListener();
    windowManager.RegisterGestureNavigationEnabledChangedListener(gestureListener);
    windowManager.UnregisterGestureNavigationEnabledChangedListener(gestureListener);
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr || size < OHOS::DATA_MIN_SIZE) {
        return 0;
    }
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

