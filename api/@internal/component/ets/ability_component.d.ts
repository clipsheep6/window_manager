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

/**
 * Provides an interface for the ability component.
 * @since 9
 */
interface AbilityComponentInterface {
  /**
   * Construct the ability component.
   * Called when the ability component is used.
   * @since 9
   */
  (want: Want): AbilityComponentAttribute;
}

/**
 * Defines the attribute functions of ability component.
 * @since 9
 */
declare class AbilityComponentAttribute extends CommonMethod<AbilityComponentAttribute> {
  /**
   * Called when the component connected with ability.
   * @since 9
   */
  onConnect(callback: () => void): AbilityComponentAttribute;
  /**
   * Called when the component onDisconnected with ability.
   * @since 9
   */
   onDisconnect(callback: () => void): AbilityComponentAttribute;
}

declare const AbilityComponent: AbilityComponentInterface;
declare const AbilityComponentInstance: AbilityComponentAttribute;
