/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "scenesessionmgrlstubreg_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <parcel.h>
#include <securec.h>

#include <iremote_stub.h>
#include "message_option.h"
#include "message_parcel.h"
#include "marshalling_helper.h"
#include "scene_session_manager_lite.h"
#include "scene_session_manager_lite_stub.h"
#include "scene_session_manager_lite_interface.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    
    MessageParcel parcel;
    MessageParcel reply;
    MessageOption option;

    parcel.WriteInterfaceToken(SceneSessionManagerLiteStub::GetDescriptor());
    parcel.WriteBuffer(data, size);

    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::
            SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_SESSION_LISTENER),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::
            SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::
            SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::
            SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::
            SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::
            SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT),
        parcel, reply, option);
    parcel.RewindRead(0);
    SceneSessionManagerLite::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManagerLite::SceneSessionManagerLiteMessage::TRANS_ID_CHECK_WINDOW_ID),
        parcel, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}