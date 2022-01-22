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

import { ApplicationInfo } from './applicationInfo';
import { Metadata } from './metadata'
import bundle from './../@ohos.bundle';

/**
 * @name Obtains extension information about a bundle
 * @since 9 preview
 * @SysCap SystemCapability.Appexecfwk
 * @permission NA
 * @devices phone, tablet, tv, wearable, car
 * @StageModelOnly
 */
export interface ExtensionAbilityInfo {
  /**
    * @default Indicates the name of the bundle
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly bundleName: string;

  /**
    * @default Indicates the name of the module
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly moduleName: string;

  /**
    * @default Indicates the name of the extension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly name: string;

  /**
    * @default Indicates the label id of the entension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly labelId: number;

  /**
    * @default Indicates the description id of the entension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly descriptionId: number;

  /**
    * @default Indicates the icon id of the entension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly iconId: number;

  /**
    * @default Indicates whether the entensionInfo can be visible or not
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly isVisible: boolean;

  /**
    * @default Enumerates types of the entension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly extensionAbilityType: bundle.ExtensionAbilityType;

  /**
    * @default The permissions that others need to use this extension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly permissions: Array<string>;

  /**
    * @default Obtains configuration information about an application
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly applicationInfo: ApplicationInfo;

  /**
    * @default Indicates the metadata of bundle
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly metadata: Array<Metadata>;

  /**
    * @default Indicates the src language to express extension info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly enabled: boolean;

  /**
    * @default Indicates the read permission extension ability info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly readPermission: string;

  /**
    * @default Indicates the write permission of extension ability info
    * @since 9 preview
    * @SysCap SystemCapability.Appexecfwk
    * @StageModelOnly
    */
  readonly writePermission: string;
}
