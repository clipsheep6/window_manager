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

 /**
  * @name Indicates the Metadata
  * @since 9
  * @syscap SystemCapability.BundleManager.BundleFramework
  * @permission NA
  *
  */
  export interface Metadata {
    /**
     * @default Indicates the metadata name
     * @since 9
     * @syscap SystemCapability.BundleManager.BundleFramework
     */
    name: string;

    /**
     * @default Indicates the metadata value
     * @since 9
     * @syscap SystemCapability.BundleManager.BundleFramework
     */
    value: string;

    /**
     * @default Indicates the metadata resource
     * @since 9
     * @syscap SystemCapability.BundleManager.BundleFramework
     */
    resource: string;
  }