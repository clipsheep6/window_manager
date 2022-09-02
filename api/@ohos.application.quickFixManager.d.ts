/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import { AsyncCallback } from "./basic";

/**
 * Interface of quickFixManager.
 *
 * @name quickFixManager
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.QuickFix
 * @systemapi Hide this for inner system use.
 */
declare namespace quickFixManager {
    /**
     * Apply quick fix files.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.QuickFix
     * @param hapModuleQuickFixFiles Quick fix files need to apply, this value should include file path and file name.
     * @systemapi Hide this for inner system use.
     * @return -
     * @permission ohos.permission.INSTALL_BUNDLE
     */
    function applyQuickFix(hapModuleQuickFixFiles: Array<string>, callback: AsyncCallback<void>): void;
    function applyQuickFix(hapModuleQuickFixFiles: Array<string>): Promise<void>;
}

export default quickFixManager;