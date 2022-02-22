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
 * Type of progress bar
 * @since 7
 */
declare enum ProgressStyle {
  /**
   * Linear progress bar style.
   * @devices phone, tablet, car.
   * @since 7
   */
  Linear,

  /**
   * Ring progress bar.
   * @devices phone, tablet, car.
   * @since 8
   */
  Ring,

  /**
   * Eclipse progress bar.
   * @devices phone, tablet, car.
   * @since 7
   */
  Eclipse,

  /**
   * ScaleRing progress bar.
   * @devices phone, tablet, car.
   * @since 8
   */
  ScaleRing,

  /**
   * Capsule progress bar.
   * @devices phone, tablet, car.
   * @since 8
   */
  Capsule,
}

/**
 * Provides the progress bar interface.
 * @since 7
 */
interface ProgressInterface {
  /**
   * Called when the progress bar is set.
   * @since 7
   */
  (object: { value: number; total?: number; style?: ProgressStyle }): ProgressAttribute;
}

/**
 * @since 7
 */
declare class ProgressAttribute extends CommonMethod<ProgressAttribute> {
  /**
   * Called when the current progress value is set.
   * @since 7
   */
  value(value: number): ProgressAttribute;

  /**
   * Called when the progress bar foreground is set.
   * @since 7
   */
  color(value: ResourceColor): ProgressAttribute;

  /**
   * Called when the style of the circular progress bar is set.
   * @since 7
   */
  circularStyle(value: { strokeWidth?: Length; scaleCount?: number; scaleWidth?: Length }): ProgressAttribute;

  /**
   * Called when the style of the cricular progress bar is set.
   * @since 7
   * @deprecated since 7
   */
  cricularStyle(value: { strokeWidth?: Length; scaleCount?: number; scaleWidth?: Length }): ProgressAttribute;
}

declare const Progress: ProgressInterface;
declare const ProgressInstance: ProgressAttribute;
