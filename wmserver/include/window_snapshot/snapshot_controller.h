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

#ifndef OHOS_ROSEN_SNAPSHOT_CONTROLLER_H
#define OHOS_ROSEN_SNAPSHOT_CONTROLLER_H

#include <snapshot.h>
#include <transaction/rs_interfaces.h>

#include "event_handler.h"
#include "future.h"
#include "snapshot_stub.h"
#include "wm_common_inner.h"
#include "window_root.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class SnapshotController : public SnapshotStub {
public:
    SnapshotController(sptr<WindowRoot>& root, std::shared_ptr<AppExecFwk::EventHandler>& handler) : windowRoot_(root),
        handler_(handler) {};
    SnapshotController() : windowRoot_(nullptr), handler_(nullptr) {};
    ~SnapshotController() = default;
    void Init(sptr<WindowRoot>& root);
    int32_t GetSnapshot(const sptr<IRemoteObject> &token, AAFwk::Snapshot& snapshot) override;

private:
    void RecordGetSnapshotEvent(int64_t costTime);

private:
    sptr<WindowRoot> windowRoot_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    GetSnapshotTimeConfig getSnapshotTimeConfig_ = { 0, 0, 0, 0, 0, 0 };
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SNAPSHOT_CONTROLLER_H
