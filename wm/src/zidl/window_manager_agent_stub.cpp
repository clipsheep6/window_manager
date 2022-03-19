/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "zidl/window_manager_agent_stub.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerAgentStub"};
}

int WindowManagerAgentStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    WLOGFI("WindowManagerAgentStub::OnRemoteRequest code is %{public}d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    switch (code) {
        case TRANS_ID_UPDATE_FOCUS_STATUS: {
            uint32_t windowId = data.ReadUint32();
            sptr<IRemoteObject> abilityToken = data.ReadRemoteObject();
            WindowType windowType = static_cast<WindowType>(data.ReadUint32());
            DisplayId displayId = data.ReadUint64();
            bool focused = data.ReadBool();
            UpdateFocusStatus(windowId, abilityToken, windowType, displayId, focused);
            break;
        }
        case TRANS_ID_UPDATE_FOCUS: {
            sptr<FocusChangeInfo> info = data.ReadParcelable<FocusChangeInfo>();
            if (!info) {
                WLOGFE("info is null.");
                break;
            }
            info->abilityToken_ = data.ReadRemoteObject();
            bool focused = data.ReadBool();
            UpdateFocusChangeInfo(info, focused);
            break;
        }
        case TRANS_ID_UPDATE_SYSTEM_BAR_PROPS: {
            DisplayId displayId = data.ReadUint64();
            SystemBarRegionTints tints;
            uint32_t size = data.ReadUint32();
            if (size > data.GetReadableBytes() || size > tints.max_size()) {
                WLOGFE("fail to get SystemBarRegionTints size");
                break;
            }
            tints.resize(size);
            if (tints.size() < size) {
                WLOGFE("fail to resize SystemBarRegionTints");
                break;
            }
            for (uint32_t i = 0; i < size; i++) {
                WindowType type = static_cast<WindowType>(data.ReadUint32());
                SystemBarProperty prop = { data.ReadBool(), data.ReadUint32(), data.ReadUint32() };
                Rect region = { data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32() };
                SystemBarRegionTint tint(type, prop, region);
                tints.emplace_back(tint);
            }
            UpdateSystemBarRegionTints(displayId, tints);
            break;
        }
        case TRANS_ID_UPDATE_WINDOW_STATUS: {
            sptr<AccessibilityWindowInfo> windowInfo = data.ReadParcelable<AccessibilityWindowInfo>();
            if (!windowInfo) {
                WLOGFE("windowInfo is null.");
                break;
            }
            WindowUpdateType type = static_cast<WindowUpdateType>(data.ReadUint32());
            NotifyAccessibilityWindowInfo(windowInfo, type);
            break;
        }
        case TRANS_ID_UPDATE_WINDOW_VISIBILITY: {
            uint32_t size = data.ReadUint32();
            std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
            if (size > data.GetReadableBytes() || size > windowVisibilityInfos.max_size()) {
                WLOGFE("fail to get windowVisibilityInfos size.");
                break;
            }
            windowVisibilityInfos.resize(size);
            if (windowVisibilityInfos.size() < size) {
                WLOGFE("fail to resize windowVisibilityInfos.");
                break;
            }
            for (uint32_t i = 0; i < size; ++i) {
                sptr<WindowVisibilityInfo> info = data.ReadParcelable<WindowVisibilityInfo>();
                if (!info) {
                    WLOGFE("info is null.");
                    break;
                }
                windowVisibilityInfos.emplace_back(info);
            }
            UpdateWindowVisibilityInfo(windowVisibilityInfos);
            break;
        }
        default:
            break;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
