/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include <map>
#include <float.h>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;

/**
 * @struct PointInfo.
 *
 * @brief Point info.
 */
struct PointInfo {
    int32_t x;
    int32_t y;
};

/**
 * @class Transform
 *
 * @brief parameter of transform and rotate.
 */
class Transform {
public:
    Transform()
        : pivotX_(0.5f), pivotY_(0.5f), scaleX_(1.f), scaleY_(1.f), scaleZ_(1.f), rotationX_(0.f),
          rotationY_(0.f), rotationZ_(0.f), translateX_(0.f), translateY_(0.f), translateZ_(0.f)
    {}
    ~Transform() {}

    bool operator==(const Transform& right) const
    {
        return NearZero(scaleX_ - right.scaleX_) &&
            NearZero(scaleY_ - right.scaleY_) &&
            NearZero(scaleZ_ - right.scaleZ_) &&
            NearZero(pivotX_ - right.pivotX_) &&
            NearZero(pivotY_ - right.pivotY_) &&
            NearZero(translateX_ - right.translateX_) &&
            NearZero(translateY_ - right.translateY_) &&
            NearZero(translateZ_ - right.translateZ_) &&
            NearZero(rotationX_ - right.rotationX_) &&
            NearZero(rotationY_ - right.rotationY_) &&
            NearZero(rotationZ_ - right.rotationZ_);
    }

    bool operator!=(const Transform& right) const
    {
        return !(*this == right);
    }

    static const Transform& Identity()
    {
        static Transform I;
        return I;
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

    bool Marshalling(Parcel& parcel) const
    {
        return parcel.WriteFloat(pivotX_) && parcel.WriteFloat(pivotY_) &&
               parcel.WriteFloat(scaleX_) && parcel.WriteFloat(scaleY_) && parcel.WriteFloat(scaleZ_) &&
               parcel.WriteFloat(rotationX_) && parcel.WriteFloat(rotationY_) && parcel.WriteFloat(rotationZ_) &&
               parcel.WriteFloat(translateX_) && parcel.WriteFloat(translateY_) && parcel.WriteFloat(translateZ_);
    }

private:
    static inline bool NearZero(float val)
    {
        return -0.001f < val && val < 0.001f;
    }
};

/**
 * @struct Rect
 *
 * @brief Window Rect.
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

    bool operator!=(const Rect& a) const
    {
        return !this->operator==(a);
    }

    bool IsInsideOf(const Rect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }

    bool IsUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }
};

/**
 * @struct SystemBarProperty
 *
 * @brief Property of system bar.
 */
struct SystemBarProperty {
    bool enable_;
    uint32_t backgroundColor_;
    uint32_t contentColor_;
    bool enableAnimation_;
    SystemBarSettingFlag settingFlag_;
    SystemBarProperty() : enable_(true), backgroundColor_(SYSTEM_COLOR_BLACK), contentColor_(SYSTEM_COLOR_WHITE),
                          enableAnimation_(false), settingFlag_(SystemBarSettingFlag::DEFAULT_SETTING) {}
    SystemBarProperty(bool enable, uint32_t background, uint32_t content, bool enableAnimation)
        : enable_(enable), backgroundColor_(background), contentColor_(content), enableAnimation_(enableAnimation),
          settingFlag_(SystemBarSettingFlag::DEFAULT_SETTING) {}
    SystemBarProperty(bool enable, uint32_t background, uint32_t content)
        : enable_(enable), backgroundColor_(background), contentColor_(content), enableAnimation_(false),
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
 * @class AvoidArea
 *
 * @brief Area needed to avoid.
 */
class AvoidArea : virtual public RefBase {
public:
    Rect topRect_ { 0, 0, 0, 0 };
    Rect leftRect_ { 0, 0, 0, 0 };
    Rect rightRect_ { 0, 0, 0, 0 };
    Rect bottomRect_ { 0, 0, 0, 0 };

    bool operator==(const AvoidArea& a) const
    {
        return (topRect_ == a.topRect_ && leftRect_ == a.leftRect_ &&
            rightRect_ == a.rightRect_ && bottomRect_ == a.bottomRect_);
    }

    bool operator!=(const AvoidArea& a) const
    {
        return !this->operator==(a);
    }

    static inline bool ReadParcel(Parcel& parcel, Rect& rect)
    {
        return parcel.ReadInt32(rect.posX_) && parcel.ReadInt32(rect.posY_) &&
            parcel.ReadUint32(rect.width_) && parcel.ReadUint32(rect.height_);
    }

    static inline bool WriteParcel(Parcel& parcel, const Rect& rect)
    {
        return parcel.WriteInt32(rect.posX_) && parcel.WriteInt32(rect.posY_) &&
            parcel.WriteUint32(rect.width_) && parcel.WriteUint32(rect.height_);
    }

    bool isEmptyAvoidArea() const
    {
        return topRect_.IsUninitializedRect() && leftRect_.IsUninitializedRect() &&
            rightRect_.IsUninitializedRect() && bottomRect_.IsUninitializedRect();
    }
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
        return (maxHeight_ == 0 || minHeight_ == 0 || maxWidth_ == 0 || minWidth_ == 0);
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

    bool operator!=(const TitleButtonRect& a) const
    {
        return !this->operator==(a);
    }

    bool IsInsideOf(const TitleButtonRect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }

    bool IsUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
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

        const uint32_t curveParamSize = 4; // 4: param size
        auto paramSize = curveParams_.size();
        if (paramSize != curveParamSize) {
            if (!parcel.WriteUint32(0)) {
                return false;
            }
        } else {
            if (!parcel.WriteUint32(static_cast<uint32_t>(paramSize))) {
                return false;
            }
            for (auto& param : curveParams_) {
                if (!parcel.WriteFloat(param)) {
                    return false;
                }
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
        auto parameterSize = parcel.ReadUint32();
        if (parameterSize == 4) { // 4: param size
            for (uint32_t i = 0; i < parameterSize; i++) {
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

enum class BackupAndRestoreType: int32_t {
    NONE = 0,           // no backup and restore
    CONTINUATION = 1,   // distribute
    APP_RECOVERY = 2,   // app recovery
};
}
}
#endif // OHOS_ROSEN_WM_COMMON_H
