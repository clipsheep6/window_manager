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

#include "common/include/window_session_property.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
void WindowSessionProperty::SetWindowName(const std::string& name)
{
    windowName_ = name;
}

void WindowSessionProperty::SetSessionInfo(const SessionInfo& info)
{
    sessionInfo_ = info;
}

void WindowSessionProperty::SetWindowRect(const struct Rect& rect)
{
    windowRect_ = rect;
}

void WindowSessionProperty::SetRequestRect(const Rect& requestRect)
{
    requestRect_ = requestRect;
}

void WindowSessionProperty::SetWindowType(WindowType type)
{
    type_ = type;
}

void WindowSessionProperty::SetFocusable(bool isFocusable)
{
    focusable_ = isFocusable;
}

void WindowSessionProperty::SetTouchable(bool isTouchable)
{
    touchable_ = isTouchable;
}

void WindowSessionProperty::SetDisplayId(DisplayId displayId)
{
    displayId_ = displayId;
}

const std::string& WindowSessionProperty::GetWindowName() const
{
    return windowName_;
}

const SessionInfo& WindowSessionProperty::GetSessionInfo() const
{
    return sessionInfo_;
}

Rect WindowSessionProperty::GetWindowRect() const
{
    return windowRect_;
}

Rect WindowSessionProperty::GetRequestRect() const
{
    return requestRect_;
}

WindowType WindowSessionProperty::GetWindowType() const
{
    return type_;
}

bool WindowSessionProperty::GetFocusable() const
{
    return focusable_;
}

bool WindowSessionProperty::GetTouchable() const
{
    return touchable_;
}

DisplayId WindowSessionProperty::GetDisplayId() const
{
    return displayId_;
}

void WindowSessionProperty::SetParentId(uint32_t parentId)
{
    parentId_ = parentId;
}

uint32_t WindowSessionProperty::GetParentId() const
{
    return parentId_;
}

void WindowSessionProperty::SetPersistentId(uint64_t persistentId)
{
    persistentId_ = persistentId;
}

uint64_t WindowSessionProperty::GetPersistentId() const
{
    return persistentId_;
}

void WindowSessionProperty::SetParentPersistentId(uint64_t persistentId)
{
    parentPersistentId_ = persistentId;
}

uint64_t WindowSessionProperty::GetParentPersistentId() const
{
    return parentPersistentId_;
}

void WindowSessionProperty::SetAccessTokenId(uint32_t accessTokenId)
{
    accessTokenId_ = accessTokenId;
}

uint32_t WindowSessionProperty::GetAccessTokenId() const
{
    return accessTokenId_;
}

bool WindowSessionProperty::Marshalling(Parcel& parcel) const
{
    return parcel.WriteString(windowName_) && parcel.WriteInt32(windowRect_.posX_) &&
        parcel.WriteInt32(windowRect_.posY_) && parcel.WriteUint32(windowRect_.width_) &&
        parcel.WriteUint32(windowRect_.height_) && parcel.WriteInt32(requestRect_.posX_) &&
        parcel.WriteInt32(requestRect_.posY_) && parcel.WriteUint32(requestRect_.width_) &&
        parcel.WriteUint32(requestRect_.height_) &&
        parcel.WriteUint32(static_cast<uint32_t>(type_)) &&
        parcel.WriteBool(focusable_) && parcel.WriteBool(touchable_) &&
        parcel.WriteUint64(displayId_) && parcel.WriteUint64(persistentId_) &&
        parcel.WriteString(sessionInfo_.bundleName_) && parcel.WriteString(sessionInfo_.moduleName_) &&
        parcel.WriteString(sessionInfo_.abilityName_) &&
        parcel.WriteUint64(parentPersistentId_);
}

WindowSessionProperty* WindowSessionProperty::Unmarshalling(Parcel& parcel)
{
    WindowSessionProperty* property = new(std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return nullptr;
    }
    property->SetWindowName(parcel.ReadString());
    Rect rect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetWindowRect(rect);
    Rect reqRect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetRequestRect(reqRect);
    property->SetWindowType(static_cast<WindowType>(parcel.ReadUint32()));
    property->SetFocusable(parcel.ReadBool());
    property->SetTouchable(parcel.ReadBool());
    property->SetDisplayId(parcel.ReadUint64());
    property->SetPersistentId(parcel.ReadUint64());
    SessionInfo info = { parcel.ReadString(), parcel.ReadString(), parcel.ReadString() };
    property->SetSessionInfo(info);
    property->SetParentPersistentId(parcel.ReadUint64());
    return property;
}
} // namespace Rosen
} // namespace OHOS
