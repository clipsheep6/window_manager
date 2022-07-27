/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from './basic';

/**
 * Interface of display manager.
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @devices tv, phone, tablet, wearable
 */
declare namespace display {

  /**
   * Obtain the default display.
   * @devices tv, phone, tablet, wearable
   */
  function getDefaultDisplay(callback: AsyncCallback<Display>): void;

  /**
   * Obtain the default display.
   * @devices tv, phone, tablet, wearable
   */
  function getDefaultDisplay(): Promise<Display>;

  /**
   * Obtain the default display.
   * @devices tv, phone, tablet, wearable
   */
  function getDefaultDisplaySync(): Display;

  /**
   * Obtain all displays.
   * @devices tv, phone, tablet, wearable
   * @param callback
   */
  function getAllDisplay(callback: AsyncCallback<Array<Display>>): void;

  /**
   * Obtain all displays.
   * @devices tv, phone, tablet, wearable
   */
  function getAllDisplay() : Promise<Array<Display>>;

  /**
   * Register the callback for display changes.
   * @param type: type of callback
   * @devices tv, phone, tablet, wearable, car
   */
  function on(type: 'add' | 'remove' | 'change', callback: Callback<number>): void;

  /**
   * Unregister the callback for display changes.
   * @param type: type of callback
   * #devices tv, phone, tablet, wearable, car
   */
  function off(type: 'add' | 'remove' | 'change', callback?: Callback<number>): void;

  /**
   * Enumerates the display states.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @devices tv, phone, tablet, wearable
   */
  enum DisplayState {
    /**
     * Unknown.
     */
    STATE_UNKNOWN = 0,
    /**
     * Screen off.
     */
    STATE_OFF,
    /**
     * Screen on.
     */
    STATE_ON,
    /**
     * Doze, but it will update for some important system messages.
     */
    STATE_DOZE,
    /**
     * Doze and not update.
     */
    STATE_DOZE_SUSPEND,
    /**
     * VR node.
     */
    STATE_VR,
    /**
     * Screen on and not update.
     */
    STATE_ON_SUSPEND,
  }

  /**
   * Defines properties of the display. They cannot be updated automatically.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @devices tv, phone, tablet, wearable
   */
  interface Display {
    /**
     * Display ID.
     */
    id: number;

    /**
     * Display name.
     */
    name: string;

    /**
     * The display is alive.
     */
    alive: boolean;

    /**
     * The state of display.
     */
    state: DisplayState;

    /**
     * Refresh rate, in Hz.
     */
    refreshRate: number;

    /**
     * Rotation degrees of the display.
     */
    rotation: number;

    /**
     * Display width, in pixels.
     */
    width: number;

    /**
     * Display height, in pixels.
     */
    height: number;

    /**
     * Display resolution.
     */
    densityDPI: number;

    /**
     * Display density, in pixels. The value for a low-resolution display is 1.0.
     */
    densityPixels: number;

    /**
     * Text scale density of the display.
     */
    scaledDensity: number;

    /**
     * DPI on the x-axis.
     */
    xDPI: number;

    /**
     * DPI on the y-axis.
     */
    yDPI: number;

    /**
     * Obtain the HDR info;
     * @devices tv, phone, tablet, wearable
     */
    getHdrInfo(callback: AsyncCallback<HdrInfo>): void;

    /**
     * Obtain the HDR info.
     * @devices tv, phone, tablet, wearable
     */
    getHdrInfo(): Promise<HdrInfo>;
 
    /**
     * Obtain the cutout info of the display.
     * @devices tv, phone, tablet, wearable
     */
    getCutoutInfo(callback: AsyncCallback<void>): void;

    /**
     * Obtain the cutout info of the display.
     * @devices tv, phone, tablet, wearable
     */
    getCutoutInfo(): Promise<CutoutInfo>;
  }

  /**
   * Screen HDR info.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  interface HdrInfo {
    readonly maxLum: number;
    readonly minLum: number;
    readonly maxAverageLum: number;
    readonly supportedHdrFormats: Array<HdrFormat>;
  }

  /**
   * Screen HDR formats.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  enum HdrFormat {
    NOT_SUPPORT_HDR = 0,
    DOLBY_VISION,
    HDR10,
    HDR10_PLUS,
    HLG,
    HDR_VIVID,
  }

  /**
   * cutout information of the screen.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  interface CutoutInfo {
    BoundingRects: Array<Rect>;
    waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
  }

  interface WaterfallDisplayAreaRects {
    left: Rect;
    right: Rect;
    top: Rect;
    bottom: Rect;
  }

  interface Rect {
    left: number;
    top: number;
    width: number;
    height: number;
  }
}

export default display;
