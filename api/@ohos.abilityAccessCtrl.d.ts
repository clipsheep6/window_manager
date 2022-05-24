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

import { AsyncCallback } from "./basic";

/**
 * @syscap SystemCapability.Security.AccessToken
 */
 declare namespace abilityAccessCtrl {
    /**
     * Obtains the AtManager instance.
     * @return returns the instance of the AtManager.
     * @since 8
     */
    function createAtManager(): AtManager;
  
    /**
     * Provides methods for managing access_token.
     * @name AtManager
     */
    interface AtManager {
        /**
         * Checks whether a specified application has been granted the given permission.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be verified.
         * @return Returns permission verify result
         * @since 8
         */
        verifyAccessToken(tokenID: number, permissionName: string): Promise<GrantStatus>;

        /**
         * Grants a specified user_grant permission to the given application.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be granted.
         * @param permissionFlag Flag of permission state.
         * @permission ohos.permission.GRANT_SENSITIVE_PERMISSIONS.
         * @systemapi hid this for inner system use
         * @since 8
         */
        grantUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number): Promise<number>;
        grantUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number, callback: AsyncCallback<number>): void;

        /**
         * Revokes a specified user_grant permission to the given application.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be revoked.
         * @param permissionFlag Flag of permission state.
         * @permission ohos.permission.REVOKE_SENSITIVE_PERMISSIONS.
         * @systemapi hid this for inner system use
         * @since 8
         */
        revokeUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number): Promise<number>;
        revokeUserGrantedPermission(tokenID: number, permissionName: string, permissionFlag: number, callback: AsyncCallback<number>): void;

        /**
         * Queries specified permission flag of the given application.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be granted.
         * @return Return permission flag.
         * @permission ohos.permission.GET_SENSITIVE_PERMISSIONS or ohos.permission.GRANT_SENSITIVE_PERMISSIONS or ohos.permission.REVOKE_SENSITIVE_PERMISSIONS.
         * @systemapi hid this for inner system use
         * @since 8
         */
        getPermissionFlags(tokenID: number, permissionName: string): Promise<number>;

        /**
         * Add access record of sensitive permission.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be added.
         * @param successCount Access count.
         * @param failCount Reject account.
         * @return NA.
         * @permission ohos.permission.PERMISSION_USED_STATS.
         * @systemapi hid this for inner system use
         * @since 9
         */
        addPermissionUsedRecord(tokenID: number, permissionName: string, successCount: number, failCount: number): Promise<void>;
        addPermissionUsedRecord(tokenID: number, permissionName: string, successCount: number, failCount: number, callback: AsyncCallback<void>): void;

        /**
         * Start using sensitive permission.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be started.
         * @return NA.
         * @permission ohos.permission.PERMISSION_USED_STATS.
         * @systemapi hid this for inner system use
         * @since 9
         */
        startUsingPermission(tokenID: number, permissionName: string): Promise<number>;
        startUsingPermission(tokenID: number, permissionName: string, callback: AsyncCallback<number>): void;

        /**
         * Stop using sensitive permission.
         * @param tokenID The tokenId of specified application.
         * @param permissionName The permission name to be stopped.
         * @return NA.
         * @permission ohos.permission.PERMISSION_USED_STATS.
         * @systemapi hid this for inner system use
         * @since 9
         */
        stopUsingPermission(tokenID: number, permissionName: string): Promise<number>;
        stopUsingPermission(tokenID: number, permissionName: string, callback: AsyncCallback<number>): void;

        /**
         * Queries the access records of sensitive permission.
         * @param request The request of permission used records.
         * @return Return the reponse of permission used records.
         * @permission ohos.permission.PERMISSION_USED_STATS.
         * @systemapi hid this for inner system use
         * @since 9
         */
         getPermissionUsedRecords(request: PermissionUsedRequest): Promise<PermissionUsedResponse>;
         getPermissionUsedRecords(request: PermissionUsedRequest, callback: AsyncCallback<PermissionUsedResponse>): void;
    }
  
    /**
     * GrantStatus.
     * @since 8
     */
    export enum GrantStatus {
        /**
         * access_token permission check fail
         */
        PERMISSION_DENIED = -1,
        /**
         * access_token permission check success
         */
        PERMISSION_GRANTED = 0,
    }

    /**
     * PermissionUsageFlag.
     * @since 9
     */
    enum PermissionUsageFlag {
        /**
         * permission used summary
         */
        FLAG_PERMISSION_USAGE_SUMMARY = 0,
        /**
         * permission used detail
         */
        FLAG_PERMISSION_USAGE_DETAIL = 1,
    }

    /**
     * Provides request of querying permission used records.
     * @since 9
     */
    interface PermissionUsedRequest {
        /**
         * The device id
         */
        deviceId: string;

        /**
         * The bundle name
         */
        bundleName: string;

        /**
         * The list of permision name
         */ 
        permNames: Array<string>;

        /**
         * The begin time
         */
        beginTimeMillis: number;

        /**
         * The end time
         */
        endTimeMillis: number;

        /**
         * The permission usage flag
         */
        flag: PermissionUsageFlag;
    }

    /**
     * Provides response of querying permission used records.
     * @since 9
     */
    interface PermissionUsedResponse {
        /**
         * The begin time 
         */
        beginTimeMillis: number;

        /**
         * The end time
         */
        endTimeMillis: number;

        /**
         * The list of permision used records of bundle
         */ 
        bundleRecords: Array<BundleUsedRecord>;
    }

    /**
     * BundleUsedRecord.
     * @since 9
     */
     interface BundleUsedRecord {
        /**
         * The device id 
         */
        deviceId: string;

        /**
         * The bundle name 
         */
        bundleName: string;

        /**
         * The list of permission used records
         */ 
        permissionRecords: Array<PermissionUsedRecord>;
    }

    /**
     * PermissionUsedRecord.
     * @since 9
     */
    interface PermissionUsedRecord {
        /**
        * The permission name 
        */
        permissionName: string;

        /**
         * The access counts
         */
        accessCount: number;

        /**
         * The reject counts
         */
        rejectCount: number;

        /**
         * The last access time
         */
        lastAccessTime: number;

        /**
         * The last reject time
         */
        lastRejectTime: number;

        /**
         * The last access duration
         */
        lastAccessDuration: number;

         /**
         * The list of access timestamp records
         */
        accessRecords: Array<UsedRecordDetail>;

        /**
         * The list of reject timestamp records
         */
        rejectRecords: Array<UsedRecordDetail>;
    }

    /**
     * UsedRecordDetail.
     * @since 9
     */
    interface UsedRecordDetail {
        /**
         * The status of foreground/background
         */
        isForeground: boolean;

        /**
         * The status of lockscreen
         */
        isLockScreen: boolean;

        /**
         * Access timestamp
         */
        accessTimestamp: number;

        /**
         * Access duration
         */
        accessDuration: number;
    }
}

export default abilityAccessCtrl;
