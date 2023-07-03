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
#ifndef OHOS_ROSEN_ACCESSIBILITY_WINDOW_INFO_H
#define OHOS_ROSEN_ACCESSIBILITY_WINDOW_INFO_H

#include <refbase.h>
#include <iremote_object.h>

#include "wm_common.h"

namespace OHOS {
namespace Rosen {
/**
 * @class AccessibilityWindowInfo
 *
 * @brief Window info used for Accessibility.
 */
class AccessibilityWindowInfo : public Parcelable {
public:
    /**
     * @brief Default construct of AccessibilityWindowInfo.
     */
    AccessibilityWindowInfo() = default;
    /**
     * @brief Default deconstruct of AccessibilityWindowInfo.
     */
    ~AccessibilityWindowInfo() = default;

    /**
     * @brief Marshalling AccessibilityWindowInfo.
     *
     * @param parcel Package of AccessibilityWindowInfo.
     * @return True means marshall success, false means marshall failed.
     */
    virtual bool Marshalling(Parcel& parcel) const;

    /**
     * @brief Unmarshalling AccessibilityWindowInfo.
     *
     * @param parcel Package of AccessibilityWindowInfo.
     * @return AccessibilityWindowInfo object.
     */
    static AccessibilityWindowInfo* Unmarshalling(Parcel& parcel);

    int32_t wid_;
    Rect windowRect_;
    bool focused_ { false };
    bool isDecorEnable_ { false };
    DisplayId displayId_;
    uint32_t layer_;
    WindowMode mode_;
    WindowType type_;
};
}
}
#endif // OHOS_ROSEN_ACCESSIBILITY_WINDOW_INFO_H