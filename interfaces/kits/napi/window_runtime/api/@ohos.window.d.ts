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

/**
 * Window manager.
 * @devices tv, phone, tablet, wearable.
*/
declare namespace windowmanager {
  enum  WindowType {
    APP_WINDOW_BASE = 1,
    APP_MAIN_WINDOW_BASE = APP_WINDOW_BASE,
    WINDOW_TYPE_APP_MAIN_WINDOW = APP_MAIN_WINDOW_BASE,
    APP_MAIN_WINDOW_END = WINDOW_TYPE_APP_MAIN_WINDOW, // equals last window type

    APP_SUB_WINDOW_BASE = 1000,
    WINDOW_TYPE_MEDIA = APP_SUB_WINDOW_BASE,
    WINDOW_TYPE_APP_SUB_WINDOW,
    APP_SUB_WINDOW_END = WINDOW_TYPE_APP_SUB_WINDOW, // equals last window type
    APP_WINDOW_END = APP_SUB_WINDOW_END,

    SYSTEM_WINDOW_BASE = 2000,
    BELOW_APP_SYSTEM_WINDOW_BASE = SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_WALLPAPER = SYSTEM_WINDOW_BASE,
    BELOW_APP_SYSTEM_WINDOW_END = WINDOW_TYPE_WALLPAPER, // equals last window type

    ABOVE_APP_SYSTEM_WINDOW_BASE = 2100,
    WINDOW_TYPE_APP_LAUNCHING = ABOVE_APP_SYSTEM_WINDOW_BASE,
    WINDOW_TYPE_DOCK_SLICE,
    WINDOW_TYPE_INCOMING_CALL,
    WINDOW_TYPE_SEARCHING_BAR,
    WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
    WINDOW_TYPE_INPUT_METHOD_FLOAT,
    WINDOW_TYPE_FLOAT,
    WINDOW_TYPE_TOAST,
    WINDOW_TYPE_STATUS_BAR,
    WINDOW_TYPE_PANEL,
    WINDOW_TYPE_KEYGUARD,
    WINDOW_TYPE_VOLUME_OVERLAY,
    WINDOW_TYPE_NAVIGATION_BAR,
    WINDOW_TYPE_DRAGGING_EFFECT,
    WINDOW_TYPE_POINTER,
    ABOVE_APP_SYSTEM_WINDOW_END = WINDOW_TYPE_POINTER, // equals last window type
    SYSTEM_WINDOW_END = ABOVE_APP_SYSTEM_WINDOW_END,
  }

  enum WindowMode {
    /**
     * FullSCREEN.
     */
    WINDOW_MODE_FULLSCREEN,

    /**
     * SPLIT.
     */
    WINDOW_MODE_SPLIT,

    /**
     * FREEFORM.
     */
    WINDOW_MODE_FLOATING,

    /**
     * PIP.
     */
    WINDOW_MODE_PIP
  }

  interface WindowManager {
    /**
     * Create a sub window with a specific id and type.
     * @param id Indicates window id.
     * @param type Indicates window type.
     * @permission ohos.permission.SYSTEM_FLOAT_WINDOW
     * @since 7
     */
    create(id: string, type: WindowType, callback: AsyncCallback<Window>): void;
    /**
     * Create a sub window with a specific id and type.
     * @param id Indicates window id.
     * @param type Indicates window type.
     * @permission ohos.permission.SYSTEM_FLOAT_WINDOW
     * @since 7
     */
    create(id: string, type: WindowType): Promise<Window>;
    /**
     * Find the sub window by id.
     * @param id Indicates window id.
     * @since 7
     */
    find(id: string, callback: AsyncCallback<Window>): void;
    /**
     * Find the sub window by id.
     * @param id Indicates window id.
     * @since 7
     */
    find(id: string): Promise<Window>;

  }
  /**
   * Properties of status bar and navigation bar, it couldn't update automatically
   * @devices tv, phone, tablet, wearable, car
   * @since 6
   */
  interface SystemBarProperties {
    /**
     * the color of the status bar.
     * @since 6
     */
    statusBarColor?: string;

    /**
     * the light icon of the status bar.
     * @since 7
     */
    isStatusBarLightIcon?: boolean;

    /**
     * the color of the navigation bar.
     * @since 6
     */
    navigationBarColor?: string;

    /**
     * the light icon of the navigation bar.
     * @since 7
     */
    isNavigationBarLightIcon?: boolean;
  }
  /**
   * system bar tint of region
   * @devices tv, phone, tablet, wearable, car
   * @SystemAPI
   * @since 8
   */
  interface SystemBarRegionTint {
    /**
     * system bar type
     */
    type: WindowType;
    /**
     * the visibility of system bar
     */
    isEnable?: boolean;
    /**
     * the region of system bar
     */
    region?: Rect;
    /**
     * the background color of the system bar.
     */
    backgroundColor?: string;
    /**
     * the content color of the system bar.
     */
    contentColor?: string

  }

  /**
   * system bar tint state for systemui
   * @devices tv, phone, tablet, wearable, car
   * @SystemAPI
   * @since 8
   */
  interface SystemBarTintState {
    /**
     * id of display
     */
    displayId: number;
    /**
     * region tint of systembar
     */
    regionTint: Array<SystemBarRegionTint>;
  }
  /**
   * Rectangle
   * @devices tv, phone, tablet, wearable, car
   * @since 7
   */
  interface Rect {
    left: number;
    top: number;
    width: number;
    height: number;
  }

  /**
   * window size
   * @devices tv, phone, tablet, wearable, car
   * @since 7
   */
  interface Size {
    /**
     * the width of the window.
     */
    width: number;

    /**
     * the height of the window.
     */
    height: number;
  }

  /**
   * Properties of window, it couldn't update automatically
   * @devices tv, phone, tablet, wearable, car
   * @since 6
   */
  interface WindowProperties {
    /**
     * the position and size of the window
     * @since 7
     */
    windowRect: Rect;

    /**
     * window type
     * @since 7
     */
    type: WindowType;
  }

  interface Window {
    /**
     * hide sub window.
     * @systemapi Hide this for inner system use.
     * @since 7
     */
    hide (callback: AsyncCallback<void>): void;
    /**
      * hide sub window.
      * @systemapi Hide this for inner system use.
      * @since 7
      */
    hide(): Promise<void>;

    /**
      * show sub window.
      * @since 7
      */
    show(callback: AsyncCallback<void>): void;
    /**
      * show sub window.
      * @since 7
      */
    show(): Promise<void>;
    /**
     * Destroy the sub window.
     * @since 7
     */
    destroy(callback: AsyncCallback<void>): void;
    /**
      * Destroy the sub window.
      * @since 7
      */
    destroy(): Promise<void>;
    /**
     * Set the position of a window.
     * @param x Indicate the X-coordinate of the window.
     * @param y Indicate the Y-coordinate of the window.
     * @devices tv, phone, tablet, wearable, liteWearable.
    */
    moveTo(x:number, y:number): Promise<void>;
    /**
     * Set the size of a window .
     * @param width Indicates the width of the window.
     * @param height Indicates the height of the window.
     * @devices tv, phone, tablet, wearable, liteWearable.
     */
    resize(width:number, height:number): Promise<void>;
    /**
     * Set the type of a window.
     * @param windowType Indicate the type of a window.
     * @devices tv, phone, tablet, wearable, liteWearable.
     */
    setWindowType(windowType: WindowType): Promise<void>;
    /**
     * Set the type of a window.
     * @param windowMode Indicate the mode of a window.
     * @devices tv, phone, tablet, wearable, liteWearable.
     */
    setWindowMode(windowMode: WindowMode): Promise<void>;
    /**
     * get the properties of current window
     * @devices tv, phone, tablet, wearable, car
     * @since 6
     */
    getProperties(callback: AsyncCallback<WindowProperties>): void;
     /**
      * get the properties of current window
      * @devices tv, phone, tablet, wearable, car
      * @since 6
      */
    getProperties(): Promise<WindowProperties>;
    /**
     * set the flag of the window is shown full screen
     * @param isFullScreen the flag of the window is shown full screen
     * @devices tv, phone, tablet, wearable, car
     * @since 6
     */
    setFullScreen(isFullScreen: boolean, callback: AsyncCallback<void>): void;
    /**
     * set the flag of the window is shown full screen
     * @param isFullScreen the flag of the window is shown full screen
     * @devices tv, phone, tablet, wearable, car
     * @since 6
     */
    setFullScreen(isFullScreen: boolean): Promise<void>;
    /**
     * set the property of the window can layout in full screen
     * @param isLayoutFullScreen the window can layout in full screen
     * @devices tv, phone, tablet, wearable, car
     * @since 7
     */
    setLayoutFullScreen(isLayoutFullScreen: boolean, callback: AsyncCallback<void>): void;
    /**
     * set the property of the window can layout in full screen
     * @param isLayoutFullScreen the window can layout in full screen
     * @devices tv, phone, tablet, wearable, car
     * @since 7
     */
    setLayoutFullScreen(isLayoutFullScreen: boolean): Promise<void>;
    /**
     * set the system bar to have visible.
     * @param names the set of system bar
     * @devices tv, phone, tablet, wearable, car
     * @since 7
     */
    setSystemBarEnable(names: Array<'status'|'navigation'>, callback: AsyncCallback<void>): void;
    /**
     * set the system bar to have visible.
     * @param names the set of system bar
     * @devices tv, phone, tablet, wearable, car
     * @since 7
     */
    setSystemBarEnable(names: Array<'status'|'navigation'>): Promise<void>;

    /**
     * set the background color of statusbar
     * @param color the background color of statusbar
     * @devices tv, phone, tablet, wearable, car
     * @since 6
     */
    setSystemBarProperties(systemBarProperties: SystemBarProperties, callback: AsyncCallback<void>): void;
    /**
     * set the background color of statusbar
     * @param color the background color of statusbar
     * @devices tv, phone, tablet, wearable, car
     * @since 6
     */
    setSystemBarProperties(systemBarProperties: SystemBarProperties): Promise<void>;
    /**
     * register the callback of windowSizeChange
     * @param type: 'windowSizeChange'
     * @devices tv, phone, tablet, wearable, car
     * @since 7
     */
    on(type: 'windowSizeChange', callback: Callback<Size>): void;

    /**
     * unregister the callback of windowSizeChange
     * @param type: 'windowSizeChange'
     * @devices tv, phone, tablet, wearable, car
     * @since 7
     */
    off(type: 'windowSizeChange', callback?: Callback<Size>): void;

    /**
     * register the callback of systemBarTintChange
     * @param type: 'systemBarTintChange'
     * @devices tv, phone, tablet, wearable, car
     * @SystemAPI
     * @since 8
     */
    on(type: 'systemBarTintChange', callback: Callback<SystemBarTintState>): void;

    /**
     * unregister the callback of systemBarTintChange
     * @param type: 'systemBarTintChange'
     * @devices tv, phone, tablet, wearable, car
     * @SystemAPI
     * @since 8
     */
    off(type: 'systemBarTintChange', callback?: Callback<SystemBarTintState>): void;
  }
}

export default windowmanager;
