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

declare namespace process {

    export interface ChildProcess {
        readonly pid: number;
        readonly ppid: number;
        readonly exitCode: number;
        readonly killed: boolean;

        wait(): Promise<number>;
        /**
        * Buffer the stdout until EOF and return it as 'Uint8Array'
        */
        getOutput(): Promise<Uint8Array>;
        /**
        * Buffer the stderr until EOF and return it as 'Uint8Array'
        */
        getErrorOutput(): Promise<Uint8Array>;
        /**
        * close the target process
        */
        close(): void;
        /**
        * send a signal to process
        */
        kill(signal: number | string): void;
    }
    type EventListener = (evt: Object) => void;
    /**
    * spawns a new ChildProcess to run the command
    */
    function runCmd(command: string,
        options?: { timeout : number, killSignal : number | string, maxBuffer : number }): ChildProcess;

    /**
    *abort current process
    *@return void
    */
    function abort(): void;

    function on(type: string, listener: EventListener): void;
    function off(type: string): boolean;
    function exit(code: number): void;
    /**
    *get current work directory;
    */
    function cwd(): string;

    /**
    *change current  directory
    *@param dir
    */
    function chdir(dir: string): void;

    function uptime(): number;
    /**
     * send some signal to target process
     */
    function kill(signal: number, pid: number): boolean;

    readonly getEgid: number;
    readonly getEuid: number;
    readonly getGid: number;
    readonly getUid: number;
    readonly getGroups: number[];
    readonly getPid: number;
    readonly getPpid: number;
}
export default process;