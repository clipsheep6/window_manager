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

#ifndef UNITTEST_MOCK_SINGLETON_MOCKER_H
#define UNITTEST_MOCK_SINGLETON_MOCKER_H

#include "singleton_container.h"
namespace OHOS {
namespace Rosen {
template<class T, class MockT>
class SingletonMocker {
public:
    SingletonMocker()
    {
        SingletonContainer::Set<T>(mock);
    }

    ~SingletonMocker()
    {
        SingletonContainer::Set<T>(T::GetInstance());
    }

    MockT& Mock()
    {
        return mock;
    }

private:
    MockT mock;
};
} // namespace Rosen
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_SINGLETON_MOCKER_H
