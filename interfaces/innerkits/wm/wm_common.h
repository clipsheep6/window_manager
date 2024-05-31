/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WM_COMMON_H
#define OHOS_ROSEN_WM_COMMON_H

#include "wm_common_enum.h"
#include <parcel.h>
#include <sstream>
#include <float.h>
#include <string>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;

/**
 * @brief Flag of uiextension window.
 */
union ExtensionWindowFlags {
    uint32_t bitData;
    struct {
        // Each flag should be false default, true when active
        bool hideNonSecureWindowsFlag : 1;
        bool waterMarkFlag : 1;
        bool privacyModeFlag : 1;
    };
    ExtensionWindowFlags() : bitData(0) {}
    ExtensionWindowFlags(uint32_t bits) : bitData(bits) {}
    ~ExtensionWindowFlags() {}
    void SetAllActive()
    {
        hideNonSecureWindowsFlag = true;
        waterMarkFlag = true;
        privacyModeFlag = true;
    }
};

/**
 * @struct PointInfo.
 *
 * @brief point Info.
 */
struct PointInfo {
    int32_t x;
    int32_t y;
};

/**
 * @struct MainWindowInfo.
 *
 * @brief topN main window info.
 */
struct MainWindowInfo : public Parcelable {
    virtual bool Marshalling(Parcel &parcel) const override
    {
        if (!parcel.WriteInt32(pid_)) {
            return false;
        }
        if (!parcel.WriteString(bundleName_)) {
            return false;
        }

        if (!parcel.WriteInt32(persistentId_)) {
            return false;
        }
        
        if (!parcel.WriteInt32(bundleType_)) {
            return false;
        }
        return true;
    }

    static MainWindowInfo* Unmarshalling(Parcel& parcel)
    {
        MainWindowInfo* mainWindowInfo = new MainWindowInfo;
        mainWindowInfo->pid_ = parcel.ReadInt32();
        mainWindowInfo->bundleName_ = parcel.ReadString();
        mainWindowInfo->persistentId_ = parcel.ReadInt32();
        mainWindowInfo->bundleType_ = parcel.ReadInt32();
        return mainWindowInfo;
    }

    int32_t pid_ = 0;
    std::string bundleName_ = "";
    int32_t persistentId_ = 0;
    int32_t bundleType_ = 0;
};

namespace {
    constexpr uint32_t SYSTEM_COLOR_WHITE = 0xE5FFFFFF;
    constexpr uint32_t SYSTEM_COLOR_BLACK = 0x66000000;
    constexpr uint32_t INVALID_WINDOW_ID = 0;
    constexpr float UNDEFINED_BRIGHTNESS = -1.0f;
    constexpr float MINIMUM_BRIGHTNESS = 0.0f;
    constexpr float MAXIMUM_BRIGHTNESS = 1.0f;
    constexpr int32_t INVALID_PID = -1;
    constexpr int32_t INVALID_UID = -1;
    constexpr int32_t INVALID_USER_ID = -1;
    constexpr int32_t SYSTEM_USERID = 0;
    constexpr int32_t BASE_USER_RANGE = 200000;
    constexpr int32_t DEFAULT_SCREEN_ID = 0;
}

/**
 * @class Transform
 *
 * @brief parameter of transform and rotate.
 */
class Transform {
    static bool NearZero(float val);
public:
    Transform() : pivotX_(0.5f), pivotY_(0.5f), scaleX_(1.f), scaleY_(1.f), scaleZ_(1.f), rotationX_(0.f),
                  rotationY_(0.f), rotationZ_(0.f), translateX_(0.f), translateY_(0.f), translateZ_(0.f) {}
    ~Transform() {}

    bool operator==(const Transform& right) const
    {
        return NearZero(pivotX_ - right.pivotX_) && NearZero(pivotY_ - right.pivotY_) &&
            NearZero(scaleX_ - right.scaleX_) && NearZero(scaleY_ - right.scaleY_) &&
            NearZero(scaleZ_ - right.scaleZ_) && NearZero(rotationX_ - right.rotationX_) &&
            NearZero(rotationY_ - right.rotationY_) && NearZero(rotationZ_ - right.rotationZ_) &&
            NearZero(translateX_ - right.translateX_) && NearZero(translateY_ - right.translateY_) &&
            NearZero(translateZ_ - right.translateZ_);
    }

    bool operator!=(const Transform& right) const
    {
        return !(*this == right);
    }

    float pivotX_;
    float pivotY_;
    float scaleX_;
    float scaleY_;
    float scaleZ_;
    float rotationX_;
    float rotationY_;
    float rotationZ_;
    float translateX_;
    float translateY_;
    float translateZ_;

    static const Transform& Identity()
    {
        static Transform I;
        return I;
    }

    bool Marshalling(Parcel& parcel) const
    {
        return parcel.WriteFloat(pivotX_) && parcel.WriteFloat(pivotY_) &&
               parcel.WriteFloat(scaleX_) && parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_) &&
               parcel.WriteFloat(rotationX_) && parcel.WriteFloat(rotationY_) && parcel.WriteFloat(rotationZ_) &&
               parcel.WriteFloat(translateX_) && parcel.WriteFloat(translateY_) && parcel.WriteFloat(translateZ_);
    }

    void Unmarshalling(Parcel& parcel)
    {
        pivotX_ = parcel.ReadFloat();
        pivotY_ = parcel.ReadFloat();
        scaleX_ = parcel.ReadFloat();
        scaleY_ = parcel.ReadFloat();
        scaleZ_ = parcel.ReadFloat();
        rotationX_ = parcel.ReadFloat();
        rotationY_ = parcel.ReadFloat();
        rotationZ_ = parcel.ReadFloat();
        translateX_ = parcel.ReadFloat();
        translateY_ = parcel.ReadFloat();
        translateZ_ = parcel.ReadFloat();
    }
};

/**
 * @struct SystemBarProperty
 *
 * @brief Property of system bar
 */
struct SystemBarProperty {
    bool enable_;
    uint32_t backgroundColor_;
    uint32_t contentColor_;
    bool enableAnimation_;
    SystemBarSettingFlag settingFlag_;
    SystemBarProperty() : enable_(true), backgroundColor_(SYSTEM_COLOR_BLACK), contentColor_(SYSTEM_COLOR_WHITE),
                          enableAnimation_(false), settingFlag_(SystemBarSettingFlag::DEFAULT_SETTING) {}
    SystemBarProperty(bool enable, uint32_t background, uint32_t content)
        : enable_(enable), backgroundColor_(background), contentColor_(content), enableAnimation_(false),
          settingFlag_(SystemBarSettingFlag::DEFAULT_SETTING) {}
    SystemBarProperty(bool enable, uint32_t background, uint32_t content, bool enableAnimation)
        : enable_(enable), backgroundColor_(background), contentColor_(content), enableAnimation_(enableAnimation),
          settingFlag_(SystemBarSettingFlag::DEFAULT_SETTING) {}
    SystemBarProperty(bool enable, uint32_t background, uint32_t content,
                      bool enableAnimation, SystemBarSettingFlag settingFlag)
        : enable_(enable), backgroundColor_(background), contentColor_(content), enableAnimation_(enableAnimation),
          settingFlag_(settingFlag) {}
    bool operator == (const SystemBarProperty& a) const
    {
        return (enable_ == a.enable_ && backgroundColor_ == a.backgroundColor_ && contentColor_ == a.contentColor_ &&
            enableAnimation_ == a.enableAnimation_);
    }
};

/**
 * @struct Rect
 *
 * @brief Window Rect
 */
struct Rect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;

    bool operator==(const Rect& a) const
    {
        return (posX_ == a.posX_ && posY_ == a.posY_ && width_ == a.width_ && height_ == a.height_);
    }

    bool operator!=(const Rect& a) const { return !this->operator==(a); }

    bool IsUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const Rect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }

    std::string ToString() const;
};

/**
 * @struct KeyboardPanelInfo
 *
 * @brief Info of keyboard panel
 */
struct KeyboardPanelInfo : public Parcelable {
    Rect rect_ = {0, 0, 0, 0};
    WindowGravity gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    bool isShowing_ = false;

    bool Marshalling(Parcel& parcel) const
    {
        return parcel.WriteInt32(rect_.posX_) && parcel.WriteInt32(rect_.posY_) && parcel.WriteUint32(rect_.width_) &&
            parcel.WriteUint32(rect_.height_) && parcel.WriteUint32(static_cast<uint32_t>(gravity_)) &&
            parcel.WriteBool(isShowing_);
    }

    static KeyboardPanelInfo* Unmarshalling(Parcel& parcel)
    {
        KeyboardPanelInfo* keyboardPanelInfo = new(std::nothrow)KeyboardPanelInfo;
        if (keyboardPanelInfo == nullptr) {
            return nullptr;
        }
        if (!(parcel.ReadInt32(keyboardPanelInfo->rect_.posX_) && parcel.ReadInt32(keyboardPanelInfo->rect_.posY_) &&
              parcel.ReadUint32(keyboardPanelInfo->rect_.width_) &&
              parcel.ReadUint32(keyboardPanelInfo->rect_.height_))) {
            delete keyboardPanelInfo;
            return nullptr;
        }
        keyboardPanelInfo->gravity_ = static_cast<WindowGravity>(parcel.ReadUint32());
        keyboardPanelInfo->isShowing_ = parcel.ReadBool();

        return keyboardPanelInfo;
    }
};

/**
 * @class AvoidArea
 *
 * @brief Area needed to avoid.
 */
class AvoidArea : public Parcelable {
public:
    Rect topRect_ { 0, 0, 0, 0 };
    Rect leftRect_ { 0, 0, 0, 0 };
    Rect rightRect_ { 0, 0, 0, 0 };
    Rect bottomRect_ { 0, 0, 0, 0 };

    bool operator==(const AvoidArea& a) const
    {
        return (leftRect_ == a.leftRect_ && topRect_ == a.topRect_ &&
            rightRect_ == a.rightRect_ && bottomRect_ == a.bottomRect_);
    }

    bool operator!=(const AvoidArea& a) const { return !this->operator==(a); }

    bool isEmptyAvoidArea() const
    {
        return topRect_.IsUninitializedRect() && leftRect_.IsUninitializedRect() &&
            rightRect_.IsUninitializedRect() && bottomRect_.IsUninitializedRect();
    }

    static bool WriteParcel(Parcel& parcel, const Rect& rect);

    static bool ReadParcel(Parcel& parcel, Rect& rect);

    virtual bool Marshalling(Parcel& parcel) const override
    {
        return (WriteParcel(parcel, leftRect_) && WriteParcel(parcel, topRect_) &&
            WriteParcel(parcel, rightRect_) && WriteParcel(parcel, bottomRect_));
    }

    static AvoidArea* Unmarshalling(Parcel& parcel)
    {
        AvoidArea *avoidArea = new(std::nothrow) AvoidArea();
        if (avoidArea == nullptr) {
            return nullptr;
        }
        if (ReadParcel(parcel, avoidArea->leftRect_) && ReadParcel(parcel, avoidArea->topRect_) &&
            ReadParcel(parcel, avoidArea->rightRect_) && ReadParcel(parcel, avoidArea->bottomRect_)) {
            return avoidArea;
        }
        delete avoidArea;
        return nullptr;
    }
};

struct PiPTemplateInfo {
    uint32_t pipTemplateType;
    uint32_t priority;
    std::vector<uint32_t> controlGroup;
};

using OnCallback = std::function<void(int64_t, int64_t)>;

/**
 * @struct VsyncCallback
 *
 * @brief Vsync callback
 */
struct VsyncCallback {
    OnCallback onCallback;
};

struct WindowLimits {
    uint32_t maxWidth_;
    uint32_t maxHeight_;
    uint32_t minWidth_;
    uint32_t minHeight_;
    float maxRatio_;
    float minRatio_;
    WindowLimits() : maxWidth_(UINT32_MAX), maxHeight_(UINT32_MAX), minWidth_(0), minHeight_(0), maxRatio_(FLT_MAX),
        minRatio_(0.0f) {}
    WindowLimits(uint32_t maxWidth, uint32_t maxHeight, uint32_t minWidth, uint32_t minHeight, float maxRatio,
        float minRatio) : maxWidth_(maxWidth), maxHeight_(maxHeight), minWidth_(minWidth), minHeight_(minHeight),
        maxRatio_(maxRatio), minRatio_(minRatio) {}

    bool IsEmpty() const
    {
        return (maxWidth_ == 0 || minWidth_ == 0 || maxHeight_ == 0 || minHeight_ == 0);
    }
};

/**
 * @struct TitleButtonRect
 *
 * @brief An area of title buttons relative to the upper right corner of the window.
 */
struct TitleButtonRect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;

    bool operator==(const TitleButtonRect& a) const
    {
        return (posX_ == a.posX_ && posY_ == a.posY_ && width_ == a.width_ && height_ == a.height_);
    }

    bool operator!=(const TitleButtonRect& a) const { return !this->operator==(a); }

    bool IsUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const TitleButtonRect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }
};

/*
 * Config of keyboard animation
 */
class KeyboardAnimationConfig : public Parcelable {
public:
    std::string curveType_ = "";
    std::vector<float> curveParams_ = {};
    uint32_t durationIn_ = 0;
    uint32_t durationOut_ = 0;

    virtual bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteString(curveType_)) {
            return false;
        }
        auto paramSize = curveParams_.size();
        if (paramSize == 4) { // 4: param size
            if (!parcel.WriteUint32(static_cast<uint32_t>(paramSize))) {
                return false;
            }
            for (auto& param : curveParams_) {
                if (!parcel.WriteFloat(param)) {
                    return false;
                }
            }
        } else {
            if (!parcel.WriteUint32(0)) {
                return false;
            }
        }
        if (!parcel.WriteUint32(durationIn_) || !parcel.WriteUint32(durationOut_)) {
            return false;
        }
        return true;
    }

    static KeyboardAnimationConfig* Unmarshalling(Parcel& parcel)
    {
        KeyboardAnimationConfig* config = new KeyboardAnimationConfig;
        config->curveType_ = parcel.ReadString();
        auto paramSize = parcel.ReadUint32();
        if (paramSize == 4) { // 4: param size
            for (uint32_t i = 0; i < paramSize; i++) {
                config->curveParams_.push_back(parcel.ReadFloat());
            }
        }
        config->durationIn_ = parcel.ReadUint32();
        config->durationOut_ = parcel.ReadUint32();
        return config;
    }
};

struct MaximizeLayoutOption {
    ShowType decor = ShowType::HIDE;
    ShowType dock = ShowType::HIDE;
};

/**
 * @class KeyboardLayoutParams
 *
 * @brief Keyboard need adjust layout
 */
class KeyboardLayoutParams : public Parcelable {
public:
    WindowGravity gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    Rect LandscapeKeyboardRect_ { 0, 0, 0, 0 };
    Rect PortraitKeyboardRect_ { 0, 0, 0, 0 };
    Rect LandscapePanelRect_ { 0, 0, 0, 0 };
    Rect PortraitPanelRect_ { 0, 0, 0, 0 };

    bool operator==(const KeyboardLayoutParams& params) const
    {
        return (gravity_ == params.gravity_ && LandscapeKeyboardRect_ == params.LandscapeKeyboardRect_ &&
            PortraitKeyboardRect_ == params.PortraitKeyboardRect_ &&
            LandscapePanelRect_ == params.LandscapePanelRect_ && PortraitPanelRect_ == params.PortraitPanelRect_);
    }

    bool operator!=(const KeyboardLayoutParams& params) const { return !this->operator==(params); }

    bool isEmpty() const
    {
        return LandscapeKeyboardRect_.IsUninitializedRect() && PortraitKeyboardRect_.IsUninitializedRect() &&
            LandscapePanelRect_.IsUninitializedRect() && PortraitPanelRect_.IsUninitializedRect();
    }

    static bool WriteParcel(Parcel& parcel, const Rect& rect);

    static bool ReadParcel(Parcel& parcel, Rect& rect);

    virtual bool Marshalling(Parcel& parcel) const override
    {
        return (parcel.WriteUint32(static_cast<uint32_t>(gravity_)) && WriteParcel(parcel, LandscapeKeyboardRect_) &&
            WriteParcel(parcel, PortraitKeyboardRect_) && WriteParcel(parcel, LandscapePanelRect_) &&
            WriteParcel(parcel, PortraitPanelRect_));
    }

    static KeyboardLayoutParams* Unmarshalling(Parcel& parcel)
    {
        KeyboardLayoutParams *params = new(std::nothrow) KeyboardLayoutParams();
        if (params == nullptr) {
            return nullptr;
        }
        params->gravity_ = static_cast<WindowGravity>(parcel.ReadUint32());
        if (ReadParcel(parcel, params->LandscapeKeyboardRect_) && ReadParcel(parcel, params->PortraitKeyboardRect_) &&
            ReadParcel(parcel, params->LandscapePanelRect_) && ReadParcel(parcel, params->PortraitPanelRect_)) {
            return params;
        }
        delete params;
        return nullptr;
    }
};
}
}

#include "wm_common.inl"

#endif // OHOS_ROSEN_WM_COMMON_H
