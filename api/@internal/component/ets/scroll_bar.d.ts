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

import { CommonMethod } from "./common";
import { BarState } from "./enums";
import { Scroller } from "./scroll"

/**
 * Content scroll direction.
 * @since 8
 */
export declare enum ScrollBarDirection {
  /**
   * Vertical scrolling is supported.
   * @since 8
   */
  Vertical,

  /**
   * Horizontal scrolling is supported.
   * @since 8
   */
  Horizontal,
}

/**
 * Defines the option of ScrollBar.
 * @since 8
 */
export declare interface ScrollBarOption {
  /**
   * Sets the scroller of scroll bar.
   * @since 8
   */
  scroller: Scroller;

  /**
   * Sets the direction of scroll bar.
   * @since 8
   */
  direction?: ScrollBarDirection;

  /**
   * Sets the state of scroll bar.
   * @since 8
   */
  state?: BarState;
}

/**
 * Provides interfaces for scroll bar.
 * @since 8
 */
interface ScrollBar extends ScrollBarAttribute<ScrollBar> {
  /**
   * Called when a ScrollBar container is set.
   * @since 8
   */
  (value: ScrollBarOption): ScrollBar;
}

/**
 * @since 8
 */
declare class ScrollBarAttribute<T> extends CommonMethod<T> {}
export declare class ScrollBarExtend<T> extends ScrollBarAttribute<T> {}
export declare const ScrollBarInterface: ScrollBar;
