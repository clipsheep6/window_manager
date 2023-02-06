/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include "vsync_station.h"
#include "frame_trace.h"
#include "transaction/rs_interfaces.h"
#include "window_manager_hilog.h"

using namespace FRAME_TRACE;
static const std::string UI_INTERVAL_NAME = "ui";
static struct TraceHandle* g_handleUI = nullptr;
static struct TidHandle* g_handleTid = nullptr;

#ifdef __aarch64__
#define FRAME_TRACE_SO_PATH "/system/lib64/libframe_trace_intf.z.so"
#else
#define FRAME_TRACE_SO_PATH "/system/lib/libframe_trace_intf.z.so"
#endif

static void VsyncStartTrace()
{
    if (access(FRAME_TRACE_SO_PATH, 0) != 0) {
        return;
    }
    if (FrameAwareTraceEnable(UI_INTERVAL_NAME)) {
        if (g_handleUI == nullptr) {
            g_handleUI = CreateTraceTag(UI_INTERVAL_NAME);
        }
        if (g_handleUI != nullptr) {
            if (JudgeUnequalFrameTrace(g_handleTid)) {
                UpdateFrameTraceTid(g_handleTid);
                EnableTraceForThread(g_handleUI);
            }
            StartFrameTrace(g_handleUI);
        }
    }
}

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "VsyncStation"};
    const std::string VSYNC_TIME_OUT_TASK = "vsync_time_out_task";
    constexpr int64_t VSYNC_TIME_OUT_MILLISECONDS = 600;
}
WM_IMPLEMENT_SINGLE_INSTANCE(VsyncStation)

void VsyncStation::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        vsyncCallbacks_.insert(vsyncCallback);

        if (!hasInitVsyncReceiver_ || !vsyncHandler_) {
            auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
            if (mainEventRunner != nullptr && isMainHandlerAvailable_) {
                WLOGFI("MainEventRunner is available");
                vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
            } else {
                WLOGFI("MainEventRunner is not available");
                if (!vsyncHandler_) {
                    vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(
                        AppExecFwk::EventRunner::Create(VSYNC_THREAD_ID));
                }
            }
            auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
            while (receiver_ == nullptr) {
                receiver_ = rsClient.CreateVSyncReceiver("WM_" + std::to_string(::getpid()), vsyncHandler_);
            }
            receiver_->Init();
            hasInitVsyncReceiver_ = true;
        }
        if (hasRequestedVsync_) {
            return;
        }
        hasRequestedVsync_ = true;
        if (vsyncHandler_) {
            vsyncHandler_->RemoveTask(VSYNC_TIME_OUT_TASK);
            vsyncHandler_->PostTask(vsyncTimeoutCallback_, VSYNC_TIME_OUT_TASK, VSYNC_TIME_OUT_MILLISECONDS);
        }
    }
    if (access(FRAME_TRACE_SO_PATH, 0) == 0) {
        if (FrameAwareTraceEnable(UI_INTERVAL_NAME)) {
            if (g_handleUI == nullptr) {
                g_handleUI = CreateTraceTag(UI_INTERVAL_NAME);
            }
            if (g_handleUI != nullptr) {
                StopFrameTrace(g_handleUI);
            }
        }
    }
    receiver_->RequestNextVSync(frameCallback_);
}

void VsyncStation::RemoveCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("[WM] Remove Vsync callback");
    vsyncCallbacks_.clear();
}

void VsyncStation::VsyncCallbackInner(int64_t timestamp)
{
    std::unordered_set<std::shared_ptr<VsyncCallback>> vsyncCallbacks;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        hasRequestedVsync_ = false;
        vsyncCallbacks = vsyncCallbacks_;
        vsyncCallbacks_.clear();
        vsyncHandler_->RemoveTask(VSYNC_TIME_OUT_TASK);
    }
    for (const auto& callback: vsyncCallbacks) {
        callback->onCallback(timestamp);
    }
}

void VsyncStation::OnVsync(int64_t timestamp, void* client)
{
    auto vsyncClient = static_cast<VsyncStation*>(client);
    if (vsyncClient) {
        vsyncClient->VsyncCallbackInner(timestamp);
        VsyncStartTrace();
    } else {
        WLOGFE("VsyncClient is null");
    }
}

void VsyncStation::OnVsyncTimeOut()
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("[WM] Vsync time out");
    hasRequestedVsync_ = false;
}
}
}
