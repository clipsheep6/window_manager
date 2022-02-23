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

import workScheduler from "./@ohos.workScheduler";

/**
 * Class of the work scheduler extension.
 *
 * @since 9
 * @syscap SystemCapability.ResourceSchedule.WorkScheduler
 * @StageModelOnly
 */
export default class WorkSchedulerExtension {
    /**
     * Called back when a work is started.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.WorkScheduler
     * @StageModelOnly
     * @param work The info of work.
     */
    onWorkStart(work: workScheduler.WorkInfo): void;

    /**
     * Called back when a work is stopped.
     *
     * @since 9
     * @syscap SystemCapability.ResourceSchedule.WorkScheduler
     * @StageModelOnly
     * @param work The info of work.
     */
    onWorkStop(work: workScheduler.WorkInfo): void;
}