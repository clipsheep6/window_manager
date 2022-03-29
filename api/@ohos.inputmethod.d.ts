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

import {AsyncCallback} from './basic';

/**
 * inputmethod
 *
 * @since 6
 * @syscap SystemCapability.MiscServices.InputMethodFramework
 */
declare namespace inputMethod {
    /**
     * @since 8
     */
    const MAX_TYPE_NUM: number

    /**
     * @since 8
     */
    function getInputMethodSetting(): InputMethodSetting;

    function getInputMethodController(): InputMethodController;

    /**
     * @since 8
     */
    interface InputMethodSetting {
        listInputMethod(callback: AsyncCallback<Array<InputMethodProperty>>): void;

        listInputMethod(): Promise<Array<InputMethodProperty>>;

        displayOptionalInputMethod(callback: AsyncCallback<void>): void;

        displayOptionalInputMethod(): Promise<void>;
    }

    interface InputMethodController {
        stopInput(callback: AsyncCallback<boolean>): void;

        stopInput(): Promise<boolean>;
    }

    /**
     * @since 8
     */
    interface InputMethodProperty {
        readonly packageName: string;
        readonly methodId: string;
    }
}

export default inputMethod;
