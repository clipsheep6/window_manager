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

#ifndef OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H
#define OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H

#include <map>

#include "interfaces/include/ws_common.h"
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "root_scene.h"
#include "session/host/include/scene_session.h"
#include "ability_info.h"

namespace OHOS::Rosen {
class JsSceneSessionManager final {
public:
    explicit JsSceneSessionManager(NativeEngine& engine);
    ~JsSceneSessionManager() = default;

    static NativeValue* Init(NativeEngine* engine, NativeValue* exportObj);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    static NativeValue* GetRootSceneSession(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSession(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionActivation(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionBackground(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionDestruction(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionByCall(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* StartAbilityBySpecified(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetWindowSceneConfig(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* ProcessBackEvent(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* UpdateFocus(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SwitchUser(NativeEngine* engin, NativeCallbackInfo* info);
    static NativeValue* GetSessionSnapshotFilePath(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* InitWithRenderServiceAdded(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetAllAbilityInfos(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* PrepareTerminate(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* PerfRequestEx(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* UpdateWindowMode(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetRootSceneSession(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSession(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionActivation(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionBackground(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionDestruction(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionByCall(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnStartAbilityBySpecified(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetWindowSceneConfig(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnProcessBackEvent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnUpdateFocus(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSwitchUser(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetSessionSnapshotFilePath(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnInitWithRenderServiceAdded(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetAllAbilityInfos(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* CreateAbilityInfos(NativeEngine& engine, const std::vector<AppExecFwk::AbilityInfo>& abilityInfos);
    NativeValue* CreateAbilityItemInfo(NativeEngine &engine, const AppExecFwk::AbilityInfo& abilityInfo);
    NativeValue* CreateWindowModes(NativeEngine& engine, const std::vector<AppExecFwk::SupportWindowMode>& windowModes);
    NativeValue* CreateWindowSize(NativeEngine& engine, const AppExecFwk::AbilityInfo& abilityInfo);
    NativeValue* OnPrepareTerminate(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnPerfRequestEx(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnUpdateWindowMode(NativeEngine& engine, NativeCallbackInfo& info);

    void OnGestureNavigationEnabledUpdate(bool enable);
    void OnCreateSpecificSession(const sptr<SceneSession>& sceneSession);
    void OnOutsideDownEvent(int32_t x, int32_t y);
    void ProcessCreateSpecificSessionRegister();
    void ProcessGestureNavigationEnabledChangeListener();
    void ProcessOutsideDownEvent();
    bool IsCallbackRegistered(const std::string& type, NativeValue* jsListenerObject);
    void RegisterDumpRootSceneElementInfoListener();

    NativeEngine& engine_;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    using Func = void(JsSceneSessionManager::*)();
    std::map<std::string, Func> listenerFunc_;

    sptr<RootScene> rootScene_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H
