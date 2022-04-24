
/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
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
import { InputEvent } from "./@ohos.multimodalInput.inputEvent"
import { KeyCode } from "./@ohos.multimodalInput.keyCode"
/**
* KeyEvent
*
* @since 9
* @syscap SystemCapability.MultimodalInput.Input.Core
* @import import KeyEvent from '@ohos.multimodalInput.keyEvent';
* @permission N/A
*/

export enum Action {
  // 取消
  Cancel = 0,
  // 按钮按下
  Down = 1,
  // 按钮抬起
  Up = 2,
}

export interface Key {
  // 按键码
  code: KeyCode;
  // 按下时间
  pressedTime: number;
  // 按键所属设备
  deviceId: number;
}

export declare interface KeyEvent extends InputEvent {
  // 按键动作
  action: Action;

  // 本次发生变化的按键
  key: Key;

  // unideCode编码
  unicodeChar: number;

  // 当前处于按下状态的按键列表
  keys: Key[];

  // 当前ctrlKey是否处于按下状态
  readonly ctrlKey: boolean

  // 当前altKey是否处于按下状态
  readonly altKey: boolean

  // 当前shiftKey是否处于按下状态
  readonly shiftKey: boolean

  // 当前logoKey是否处于按下状态
  readonly logoKey: boolean

  // 当前fnKey是否处于按下状态
  readonly fnKey: boolean

  // 当前capsLock是否处于激活状态
  capsLock: boolean

  // 当前numLock是否处于激活状态
  numLock: boolean

  // 当前scrollLock是否处于激活状态
  scrollLock: boolean
}