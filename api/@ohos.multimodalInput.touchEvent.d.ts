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
import { InputEvent } from './@ohos.multimodalInput.inputEvent'

/**
* TouchEvent
*
* @since 9
* @syscap SystemCapability.MultimodalInput.Input.Core
* @import import {PointerEvent, Pointer, PointerAction} from '@ohos.multimodalInput.ToucEvent';
* @permission N/A
*/

export declare enum Action {
  // 触摸取消
  CANCEL = 0,

  // 触摸按下
  DOWN = 1,

  // 触摸移动
  MOVE = 2,

  // 触摸抬起
  UP = 3,
}

export declare enum ToolType {
  // 手指
  FINGER = 0,

  // 笔
  PEN = 1,

  RUBBER = 2,

  BRUSH = 3,

  PENCIL = 4,

  AIRBRUSH = 5,

  MOUSE = 6,

  LENS = 7,
}

export declare enum SourceType {
  // 触摸屏
  TOUCH_SCREEN = 0,

  // 手写笔
  PEN = 1,

  // 触摸板
  TOUCH_PAD = 2,
}

export declare interface Touch {
  // 指针标识，整型 >= 0 有效
  id: number;

  // 按下时刻的时间戳
  pressedTime: number;

  // 触摸位置所属的屏幕x坐标
  screenX: number;

  // 触摸位置所属的屏幕y坐标
  screenY: number;

  // 触摸位置在窗口中的x坐标
  windowX: number;

  // 触摸位置在窗口中的y坐标
  windowY: number;

  // 压力值 [0.0, 1.0], 0.0表示不支持
  pressure: number;

  // 按下接触区域的宽度
  width: number;

  // 按下接触区域的高度
  height: number;

  // 相对YZ平面的角度,值的范围为-90到90（含），其中正值是向右倾斜。
  tiltX: number;

  // 相对XZ平面的角度,值的范围为-90到90（含），其中正值是向下倾斜。
  tiltY: number;

  // 工具区域的中心点X
  toolX: number;

  // 工具区域的中心点Y
  toolY: number;

  // 工具区域宽度
  toolWidth: number;

  // 工具区域高度
  toolHeight: number;

  // 输入设备上的x坐标
  rawX: number;

  //输入设备上的y坐标
  rawY: number;

  // 工具类型
  toolType: ToolType;
}


export declare interface TouchEvent extends InputEvent {
  // 触摸动作
  action: Action;

  // 当前触摸
  touch: Touch;

  // 所有触摸
  touches: Touch[];

  // 触摸来源的设备类型
  sourceType: SourceType;
}