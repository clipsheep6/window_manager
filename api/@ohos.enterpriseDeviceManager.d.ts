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

import { AsyncCallback, Callback } from "./basic";
import { DeviceSettingsManager } from "./enterpriseDeviceManager/DeviceSettingsManager";
import Want from "./@ohos.application.want";

/**
 * enterprise device manager.
 * @name enterpriseDeviceManager
 * @since 9
 * @syscap SystemCapability.Customization.EnterpriseDeviceManager
 */
declare namespace enterpriseDeviceManager {

  /**
   * @name EnterpriseInfo
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   */
  export interface EnterpriseInfo {
    name: string;
    description: string;
  }

  /**
   * @name AdminType
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   */
  export enum AdminType {
    ADMIN_TYPE_NORMAL = 0x00,
    ADMIN_TYPE_SUPER = 0x01
  }

  /**
   * Enables the given ability as a administrator of the device.
   * 
   * Only apps with the ohos.permission.MANAGE_ADMIN permission or the shell uid can call this method.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param admin Indicates the administrator ability information.
   * @param enterpriseInfo Indicates the enterprise information of the calling application.
   * @param type Indicates the type of administrator to set.
   * @param userId Indicates the user ID or do not pass user ID.
   * @return {@code true} if enables administrator success.
   * @permission ohos.permission.MANAGE_ADMIN
   */
  function enableAdmin(admin: Want, enterpriseInfo: EnterpriseInfo, type: AdminType, userId?: number, callback: AsyncCallback<boolean>): void;
  function enableAdmin(admin: Want, enterpriseInfo: EnterpriseInfo, type: AdminType, userId?: number): Promise<boolean>;

  /**
   * Disables a current normal administrator ability. 
   * 
   * Only apps with the ohos.permission.MANAGE_ADMIN permission or the shell uid can call this method.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param admin Indicates the administrator ability information.
   * @param userId Indicates the user ID or do not pass user ID.
   * @return {@code true} if disables administrator success.
   * @permission ohos.permission.MANAGE_ADMIN
   */
  function disableAdmin(admin: Want, userId?: number, callback: AsyncCallback<boolean>): void;
  function disableAdmin(admin: Want, userId?: number): Promise<boolean>;

  /**
   * Disables a current super administrator ability. 
   * 
   * Only the administrator app or apps with the shell uid can call this method.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param bundleName Indicates the administrator bundle information.
   * @return {@code true} if disables super administrator success.
   */
  function disableSuperAdmin(bundleName: String, callback: AsyncCallback<boolean>): void;
  function disableSuperAdmin(bundleName: String): Promise<boolean>;

  /**
   * Get whether the ability is enabled as device administrator.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param admin Indicates the administrator ability information.
   * @param userId Indicates the user ID or do not pass user ID.
   * @return {@code true} if the administrator is enabled.
   */
  function isAdminEnabled(admin: Want, userId?: number, callback: AsyncCallback<boolean>): void;
  function isAdminEnabled(admin: Want, userId?: number): Promise<boolean>;

  /**
   * Get information of the administrator's enterprise.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param admin Indicates the administrator ability information.
   * @return Returns the enterprise information of the administrator.
   */
  function getEnterpriseInfo(admin: Want, callback: AsyncCallback<EnterpriseInfo>): void;
  function getEnterpriseInfo(admin: Want): Promise<EnterpriseInfo>;

  /**
   * Set the information of the administrator's enterprise.
   * 
   * Only the administrator app can call this method.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param admin Indicates the administrator ability information.
   * @param enterpriseInfo Indicates the enterprise information of the calling application.
   * @return {@code true} if sets enterprise information success.
   */
  function setEnterpriseInfo(admin: Want, enterpriseInfo: EnterpriseInfo, callback: AsyncCallback<boolean>): void;
  function setEnterpriseInfo(admin: Want, enterpriseInfo: EnterpriseInfo): Promise<boolean>;

  /**
   * Get whether the ability is enabled as super device administrator.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @param bundleName Indicates the administrator bundle information.
   * @return {@code true} if the administrator is enabled as super device administrator.
   */
  function isSuperAdmin(bundleName: String, callback: AsyncCallback<boolean>): void;
  function isSuperAdmin(bundleName: String): Promise<boolean>;

  /**
   * Obtains the interface used to set device settings policy.
   *
   * @since 9
   * @syscap SystemCapability.Customization.EnterpriseDeviceManager
   * @return Returns the DeviceSettingsManager interface.
   */
  function getDeviceSettingsManager(callback: AsyncCallback<DeviceSettingsManager>): void;
  function getDeviceSettingsManager(): Promise<DeviceSettingsManager>;

}

export default enterpriseDeviceManager;