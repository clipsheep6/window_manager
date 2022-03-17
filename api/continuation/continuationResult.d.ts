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

/**
 * Indicates the description of transfer results for continuation.
 *
 * @since 8
 * @syscap SystemCapability.Ability.DistributedAbilityManager
 */
export interface ContinuationResult {
    /**
     * the information about the selected device id.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    id: string;

    /**
     * the information about the selected device type.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    type: string;

    /**
     * Indicates the name of the selected device.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    name: string;
}