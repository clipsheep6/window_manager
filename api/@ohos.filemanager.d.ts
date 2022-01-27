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

import { AsyncCallback } from './basic'

export default filemanager;

/**
 * @devices phone, tablet, tv, wearable
 */
declare namespace filemanager {
    export { listFile };
    export { getRoot };
    export { createFile };
    export { FileInfo };
    export { DevInfo };
}

/**
 * listFile.
 *
 * @note N/A
 * @sysCap SystemCapability.FileManagement.FileManagerService
 * @since 8
 * @permission N/A
 * @function listFile
 * @param {DevInfo} dev - dev name.
 * @param {string} type - type.
 * @param {string} path - path.
 * @param {Object} options - options
 * @param {number} [options.offset = 0] - offset.
 * @param {number} [options.count = 0] - count.
 * @param {AsyncCallback} [callback] - callback.
 * @returns {void | Promise<FileInfo[]>} no callback return Promise otherwise return void
 * @throws {TypedError} Parameter check failed
 */
declare function listFile(dev: DevInfo, type: string, path: string, options?:{offset?: number, count?: number}): Promise<FileInfo[]>;
declare function listFile(dev: DevInfo, type: string, path: string, options?:{offset?: number, count?: number}, AsyncCallback<FileInfo[]>): void;

/**
 * getRoot.
 *
 * @note N/A
 * @sysCap SystemCapability.FileManagement.FileManagerService
 * @since 8
 * @permission N/A
 * @function getRoot
 * @param {DevInfo} dev - dev name.
 * @param {AsyncCallback} [callback] - callback.
 * @returns {void | Promise<FileInfo>} no callback return Promise otherwise return void
 * @throws {TypedError} Parameter check failed
 */
declare function getRoot(dev: DevInfo): Promise<FileInfo[]>;
declare function getRoot(dev: DevInfo, callback: AsyncCallback<FileInfo[]>): void;

/**
 * createFile.
 *
 * @note N/A
 * @sysCap SystemCapability.FileManagement.FileManagerService
 * @since 8
 * @permission N/A
 * @function createFile
 * @param {DevInfo} dev - dev name.
 * @param {string} filename- file name.
 * @param {string} path - album uri.
 * @param {AsyncCallback} [callback] - callback.
 * @returns {void | Promise<string>} no callback return Promise otherwise return void
 * @throws {TypedError} Parameter check failed
 */
declare function createFile(dev: DevInfo, filename: string, path: string): Promise<string>;
declare function createFile(dev: DevInfo, filename: string, path: string, callback: AsyncCallback<string>): void;

/**
 * FileInfo
 * @note N/A
 * @sysCap SystemCapability.FileManagement.FileManagerService
 * @since 8
 * @permission N/A
 * @devices phone, tablet, tv, wearable
 */
declare interface FileInfo {
    /**
     * @type {string}
     * @readonly
     */
    name: string;
    /**
     * @type {string}
     * @readonly
     */
    path: string;
    /**
     * @type {string}
     * @readonly
     */
    type: string;
    /**
     * @type {string}
     * @readonly
     */
    size: number;
    /**
     * @type {string}
     * @readonly
     */
    addedTime: number;
    /**
     * @type {string}
     * @readonly
     */
    modifiedTime: number;
}

/**
 * DevInfo
 * @note N/A
 * @sysCap SystemCapability.FileManagement.FileManagerService
 * @since 8
 * @permission N/A
 * @devices phone, tablet, tv, wearable
 */
 declare interface DevInfo {
    /**
     * @type {string}
     */
     name: string;
 }
