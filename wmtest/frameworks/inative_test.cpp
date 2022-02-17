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

#include "inative_test.h"

namespace OHOS::Rosen {
void INativeTest::VisitTests(VisitTestFunc func)
{
    auto sortFunc = [](const auto &it, const auto &jt) {
        if (it->GetDomain() == jt->GetDomain()) {
            return it->GetID() < jt->GetID();
        }
        return it->GetDomain() < jt->GetDomain();
    };
    std::sort(nativeTest_.begin(), nativeTest_.end(), sortFunc);

    for (auto &test : nativeTest_) {
        func(test);
    }
}

INativeTest::INativeTest()
{
    nativeTest_.push_back(this);
}
} // namespace OHOS::Rosen
