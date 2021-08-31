/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

export interface DeviceResponse {
  /**
   * Brand.
   * @since 3
   */
  brand: string;

  /**
   * Manufacturer.
   * @since 3
   */
  manufacturer: string;

  /**
   * Model.
   * @since 3
   */
  model: string;

  /**
   * Product number.
   * @since 3
   */
  product: string;

  /**
   * System language.
   * @since 4
   */
  language: string;

  /**
   * System region.
   * @since 4
   */
  region: string;

  /**
   * Window width.
   * @since 3
   */
  windowWidth: number;

  /**
   * Window Height.
   * @since 3
   */
  windowHeight: number;

  /**
   * Screen density.
   * @since 4
   */
  screenDensity: number;

  /**
   * Screen shape. The options are as follows:
   * rect: Rectangle screen.
   * cicle: Circle screen.
   * @since 4
   */
  screenShape: "rect" | "circle";

    /**
   * API version.
   * @devices tv, phone, tablet, wearable, liteWearable, smartVision
   * @since 4
   */
  apiVersion: number;

  /**
   * Device type. The options are as follows:
   * phone: smartphone
   * tablet: tablet
   * tv: smart TV
   * wearable: wearable
   * liteWearable: lite wearable
   * ar: AR
   * vr: virtual reality
   * earphones: headset
   * pc: personal computer
   * speaker: speaker
   * smartVision: smart visual device
   * linkIoT: connection module
   * @devices tv, phone, tablet, wearable, liteWearable, smartVision
   * @since 4
   */
  deviceType: string;
}

/**
 * @Syscap SysCap.ACE.UIEngine
 */
export default class Device {
  /**
   * Obtains the device information.
   * @param options
   */
  static getInfo(options?: {
    /**
     * Called when the device information is obtained.
     * @since 3
     */
    success?: (data: DeviceResponse) => void;

    /**
     * Called when the device information fails to be obtained.
     * @since 3
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     * @since 3
     */
    complete?: () => void;
  }): void;
}
