/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_PIP_INFO_H
#define OHOS_PIP_INFO_H

namespace OHOS {
namespace Rosen {
enum class PipWindowState : uint32_t {
    STATE_UNDEFINED,
    STATE_STARTING,
    STATE_STARTED,
    STATE_STOPPING,
    STATE_STOPPED,
}

enum class PictureInPictureTemplateType : int32_t {
    VIDEO_PLAY = 0,
    VIDEO_CALL = 1,
    VIDEO_MEETING = 2,
};

enum class PictureInPictureState : int32_t {
    WILL_START = 1,
    STARTED = 2,
    WILL_STOP = 3,
    STOPPED = 4,
    RESTORE = 5,
    ERROR = 6,
};
} // namespace Rosen
} // namespace OHOS
#endif //OHOS_PIP_INFO_H