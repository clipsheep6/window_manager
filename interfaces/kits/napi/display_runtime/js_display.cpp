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

#include "js_display.h"

#include <cinttypes>
#include <map>
#include "display.h"
#include "display_info.h"
#include "hdr_info.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplay"};
}

static thread_local std::map<DisplayId, std::shared_ptr<NativeReference>> g_JsDisplayMap;
std::recursive_mutex g_mutex;

JsDisplay::JsDisplay(const sptr<Display>& display) : display_(display)
{
}

JsDisplay::~JsDisplay()
{
    WLOGFI("JsDisplay::~JsDisplay is called");
}

void JsDisplay::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsDisplay::Finalizer is called");
    auto jsDisplay = std::unique_ptr<JsDisplay>(static_cast<JsDisplay*>(data));
    if (jsDisplay == nullptr) {
        WLOGFE("JsDisplay::Finalizer jsDisplay is null");
        return;
    }
    sptr<Display> display = jsDisplay->display_;
    if (display == nullptr) {
        WLOGFE("JsDisplay::Finalizer display is null");
        return;
    }
    DisplayId displayId = display->GetId();
    WLOGFI("JsDisplay::Finalizer displayId : %{public}" PRIu64"", displayId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsDisplayMap.find(displayId) != g_JsDisplayMap.end()) {
        WLOGFI("JsDisplay::Finalizer Display is destroyed: %{public}" PRIu64"", displayId);
        g_JsDisplayMap.erase(displayId);
    }
}

NativeValue* JsDisplay::GetHdrInfo(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("GetHdrInfo is called");
    JsDisplay* me = CheckParamsAndGetThis<JsDisplay>(engine, info);
    return (me != nullptr) ? me->OnGetHdrInfo(*engine, *info) : nullptr;
}

NativeValue* JsDisplay::OnGetHdrInfo(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnGetHdrInfo is called");
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            sptr<HdrInfo> hdrInfo = display_->GetHdrInfo();
            if (hdrInfo != nullptr) {
                task.Resolve(engine, CreateJsHdrInfoObject(engine, hdrInfo));
                WLOGFI("JsDisplay::OnGetHdrInfo success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplay::OnGetHdrInfo failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_ONE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplay::OnGetHdrInfo",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::shared_ptr<NativeReference> FindJsDisplayObject(DisplayId displayId)
{
    WLOGFI("[NAPI]Try to find display %{public}" PRIu64" in g_JsDisplayMap", displayId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsDisplayMap.find(displayId) == g_JsDisplayMap.end()) {
        WLOGFI("[NAPI]Can not find display %{public}" PRIu64" in g_JsDisplayMap", displayId);
        return nullptr;
    }
    return g_JsDisplayMap[displayId];
}

NativeValue* CreateJsDisplayObject(NativeEngine& engine, sptr<Display>& display)
{
    WLOGFI("JsDisplay::CreateJsDisplay is called");
    NativeValue* objValue = nullptr;
    std::shared_ptr<NativeReference> jsDisplayObj = FindJsDisplayObject(display->GetId());
    if (jsDisplayObj != nullptr && jsDisplayObj->Get() != nullptr) {
        WLOGFI("[NAPI]FindJsDisplayObject %{public}" PRIu64"", display->GetId());
        objValue = jsDisplayObj->Get();
    }
    if (objValue == nullptr) {
        objValue = engine.CreateObject();
    }
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    std::unique_ptr<JsDisplay> jsDisplay = std::make_unique<JsDisplay>(display);
    object->SetNativePointer(jsDisplay.release(), JsDisplay::Finalizer, nullptr);
    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        WLOGFE("Failed to GetDisplayInfo");
        return engine.CreateUndefined();
    }
    object->SetProperty("id", CreateJsValue(engine, static_cast<uint32_t>(info->GetDisplayId())));
    object->SetProperty("width", CreateJsValue(engine, info->GetWidth()));
    object->SetProperty("height", CreateJsValue(engine, info->GetHeight()));
    object->SetProperty("refreshRate", CreateJsValue(engine, info->GetRefreshRate()));
    object->SetProperty("name", engine.CreateUndefined());
    object->SetProperty("alive", engine.CreateUndefined());
    object->SetProperty("state", engine.CreateUndefined());
    object->SetProperty("rotation", CreateJsValue(engine, info->GetRotation()));
    object->SetProperty("densityDPI", CreateJsValue(engine, info->GetVirtualPixelRatio() * DOT_PER_INCH));
    object->SetProperty("densityPixels", engine.CreateUndefined());
    object->SetProperty("scaledDensity", engine.CreateUndefined());
    object->SetProperty("xDPI", engine.CreateUndefined());
    object->SetProperty("yDPI", engine.CreateUndefined());
    BindNativeFunction(engine, *object, "getHdrInfo", JsDisplay::GetHdrInfo);
    if (jsDisplayObj == nullptr || jsDisplayObj->Get() == nullptr) {
        std::shared_ptr<NativeReference> jsDisplayRef;
        jsDisplayRef.reset(engine.CreateReference(objValue, 1));
        DisplayId displayId = display->GetId();
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        g_JsDisplayMap[displayId] = jsDisplayRef;
    }
    return objValue;
}

NativeValue* CreateJsHdrInfoObject(NativeEngine& engine, sptr<HdrInfo> hdrInfo)
{
    WLOGFI("JsDisplay::CreateJsScreen is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* optionObject = ConvertNativeValueTo<NativeObject>(objValue);
    if (optionObject == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    float maxLum = hdrInfo->GetMaxLum();
    float minLum = hdrInfo->GetMinLum();
    float maxAverageLum = hdrInfo->GetMaxAverageLum();
    std::vector<ScreenHDRFormat> screenHdrFormats = hdrInfo->GetHdrFormats();

    NativeValue* arrayValue = engine.CreateArray(screenHdrFormats.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    uint32_t index = 0;
    for (auto& hdrFormat : screenHdrFormats) {
        array->SetElement(index++, CreateJsValue(engine, static_cast<uint32_t>(hdrFormat)));
    }

    optionObject->SetProperty("maxLum", CreateJsValue(engine, maxLum));
    optionObject->SetProperty("minLum", CreateJsValue(engine, minLum));
    optionObject->SetProperty("maxAverageLum", CreateJsValue(engine, maxAverageLum));
    optionObject->SetProperty("supportedHdrFormats", arrayValue);
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
