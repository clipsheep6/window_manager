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
import abilityManager from '../@ohos.application.abilityManager';

/**
 * The class of an ability running information.
 *
 * @since 8
 * @sysCap AAFwk
 * @devices phone, tablet, tv, wearable, car
 * @permission N/A
 */
export interface AbilityRunningInfo {
    /**
    * @default ability element name
    * @since 8
    * @SysCap SystemCapability.Appexecfwk
    */
    ability: ElementName;

    /**
    * @default process id
    * @since 8
    * @SysCap SystemCapability.Appexecfwk
    */
    pid: number;

    /**
    * @default user id
    * @since 8
    * @SysCap SystemCapability.Appexecfwk
    */
    uid: number;
 
    /**
    * @default the name of the process
    * @since 8
    * @SysCap SystemCapability.Appexecfwk
    */
    processName: string;

    /**
    * @default ability start time
    * @since 8
    * @SysCap SystemCapability.Appexecfwk
    */
    startTime: number;

    /**
    * @default Enumerates state of the ability state info
    * @since 8
    * @SysCap SystemCapability.Appexecfwk
    */
    abilityState: abilityManager.AbilityState;
}