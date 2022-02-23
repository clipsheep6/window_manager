/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import { ElementName } from '../bundle/elementName';
import bundle from '../@ohos.bundle';

/**
 * The class of an extension running information.
 *
 * @since 9
 * @sysCap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 */
export interface ExtensionRunningInfo {
    /**
    * @default Indicates the extension of the extension info
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    extension: ElementName;

    /**
    * @default process id
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    pid: number;

    /**
    * @default user id
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    uid: number;

    /**
    * @default the name of the process
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    processName: string;

    /**
    * @default ability start time
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    startTime: number;

    /**
    * @default All package names under the current process
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    clientPackage: Array<String>;

    /**
    * @default Enumerates types of the entension info
    * @since 9
    * @sysCap SystemCapability.Ability.AbilityRuntime.Core
    */
    type: bundle.ExtensionAbilityType;
}