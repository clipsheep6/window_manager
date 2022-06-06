/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
* InputEvent
*
* @since 9
* @syscap SystemCapability.MultimodalInput.Input.Core
* @import import InputEvent from '@ohos.multimodalInput.inputEvent';
*/

export declare interface InputEvent {
  /**
   * Unique event ID generated by the server
   */
  id: number;

  /**
   * ID of the device that reports the input event
   */
  deviceId: number;

  /**
   * Occurrence time of the input event
   */
  actionTime: number;

  /**
   * ID of the target screen
   */
  screenId: number;

  /**
   * ID of the target window
   */
  windowId: number;
}