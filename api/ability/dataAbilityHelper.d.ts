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

import { AsyncCallback } from '../basic';
import { ResultSet } from '../data/rdb/resultSet';
import { DataAbilityOperation } from './dataAbilityOperation';
import { DataAbilityResult } from './dataAbilityResult';
import dataAbility from '../@ohos.data.dataAbility';
import rdb from '../@ohos.data.rdb';

/**
 * DataAbilityHelper
 * @devices phone, tablet
 *
 * @since 7
 * @FAModelOnly
 */
export interface DataAbilityHelper {
    /**
     * Opens a file in a specified remote path.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the file to open.
     * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
     *             (erasing whatever data is currently in the file), "wt" for write access that truncates any existing
     *             file, "wa" for write-only access to append to any existing data, "rw" for read and write access on
     *             any existing data, or "rwt" for read and write access that truncates any existing file.
     * @param callback Indicates the callback when openfile success
     * @return Returns the file descriptor.
     * @FAModelOnly
     */
    openFile(uri: string, mode: string, callback: AsyncCallback<number>): void;
    openFile(uri: string, mode: string): Promise<number>;

    /**
     * Registers an observer to observe data specified by the given uri.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param type dataChange.
     * @param uri Indicates the path of the data to operate.
     * @param callback Indicates the callback when dataChange.
     * @return -
     * @FAModelOnly
     */
    on(type: 'dataChange', uri: string, callback: AsyncCallback<void>): void;

    /**
     * Deregisters an observer used for monitoring data specified by the given uri.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param type dataChange.
     * @param uri Indicates the path of the data to operate.
     * @param callback Indicates the registered callback.
     * @return -
     * @FAModelOnly
     */
    off(type: 'dataChange', uri: string, callback?: AsyncCallback<void>): void;

    /**
     * Obtains the MIME type of the date specified by the given URI.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the data to operate.
     * @return Returns the MIME type that matches the data specified by uri.
     * @FAModelOnly
     */
    getType(uri: string, callback: AsyncCallback<string>): void;
    getType(uri: string): Promise<string>;

    /**
     * Obtains the MIME types of files supported.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain.
     * @return Returns the matched MIME types Array.
     * @FAModelOnly
     */
    getFileTypes(uri: string,  mimeTypeFilter: string, callback: AsyncCallback<Array<string>>): void;
    getFileTypes(uri: string,  mimeTypeFilter: string): Promise<Array<string>>;

    /**
     * Converts the given uri that refers to the Data ability into a normalized uri.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the uri object to normalize.
     * @return Returns the normalized uri object if the Data ability supports URI normalization or null.
     * @FAModelOnly
     */
    normalizeUri(uri: string, callback: AsyncCallback<string>): void;
    normalizeUri(uri: string): Promise<string>;

    /**
     * Converts the given normalized uri generated by normalizeUri(uri) into a denormalized one.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the uri object to normalize.
     * @return Returns the denormalized uri object if the denormalization is successful.
     * @FAModelOnly
     */
    denormalizeUri(uri: string, callback: AsyncCallback<string>): void;
    denormalizeUri(uri: string): Promise<string>;

    /**
     * Notifies the registered observers of a change to the data resource specified by uri.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the data to operate.
     * @return -
     * @FAModelOnly
     */
    notifyChange(uri: string, callback: AsyncCallback<void>): void;
    notifyChange(uri: string): Promise<void>;

    /**
     * Inserts a single data record into the database.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the data to insert.
     * @param valuesBucket Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     * @return Returns the index of the inserted data record.
     * @FAModelOnly
     */
    insert(uri: string, valuesBucket: rdb.ValuesBucket, callback: AsyncCallback<number>): void;
    insert(uri: string, valuesBucket: rdb.ValuesBucket): Promise<number>;

    /**
     * Inserts multiple data records into the database.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the data to batchInsert.
     * @param valuesBuckets Indicates the data records to insert.
     * @return Returns the number of data records inserted.
     * @FAModelOnly
     */
    batchInsert(uri: string, valuesBuckets: Array<rdb.ValuesBucket>, callback: AsyncCallback<number>): void;
    batchInsert(uri: string, valuesBuckets: Array<rdb.ValuesBucket>): Promise<number>;

    /**
     * Deletes one or more data records from the database.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of the data to delete.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @return Returns the number of data records deleted.
     * @FAModelOnly
     */
    delete(uri: string, predicates: dataAbility.DataAbilityPredicates, callback: AsyncCallback<number>): void;
    delete(uri: string, predicates: dataAbility.DataAbilityPredicates): Promise<number>;

    /**
     * Updates data records in the database.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of data to update.
     * @param valuesBucket Indicates the data to update.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @return Returns the number of data records updated.
     * @FAModelOnly
     */
    update(uri: string, valuesBucket: rdb.ValuesBucket, predicates: dataAbility.DataAbilityPredicates, callback: AsyncCallback<number>): void;
    update(uri: string, valuesBucket: rdb.ValuesBucket, predicates: dataAbility.DataAbilityPredicates): Promise<number>;

    /**
     * Queries data in the database.
     *
     * @devices phone, tablet
     * @since 7
     * @SysCap AAFwk
     * @param uri Indicates the path of data to query.
     * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @return Returns the query result {@link ResultSet}.
     * @FAModelOnly
     */
    query(uri: string, columns: Array<string>, predicates: dataAbility.DataAbilityPredicates, callback: AsyncCallback<ResultSet>): void;
    query(uri: string, columns: Array<string>, predicates: dataAbility.DataAbilityPredicates): Promise<ResultSet>;
}
