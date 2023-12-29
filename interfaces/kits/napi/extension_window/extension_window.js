/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

class ExtensionWindow {
    constructor(obj) {
        this.__extension_window__ = obj;
    }

    getWindowAvoidArea(type) {
        return this.__extension_window__.getWindowAvoidArea(type);
    }

    on(type, callback) {
        return this.__extension_window__.on(type, callback);
    }

    off(type, callback) {
        return this.__extension_window__.off(type, callback);
    }
}

export default ExtensionWindow;
