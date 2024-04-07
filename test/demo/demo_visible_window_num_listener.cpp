/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <iostream>
#include <refbase.h>
#include <unistd.h>

#include "window.h"
#include "wm_common.h"
#include "window_option.h"
#include "window_manager.h"

#include "future.h"

namespace OHOS {
namespace Rosen {
class VisibleWindowNumChangedListenerFuture : public IVisibleWindowNumChangedListener {
public:
    void OnVisibleWindowNumChange(const std::vector<DiffScreenVisibleWindowNum> diffScreenVisibleWindowNum) override
    {
        future_.SetValue(diffScreenVisibleWindowNum);
    };
    RunnableFuture<std::vector<DiffScreenVisibleWindowNum>> future_;
    static constexpr long WAIT_TIME = 20000;
};
}
}

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    std::cout << "===========================Start===========================" << std::endl;

    std::cout << "RegisterVisibleWindowNumChangedListener" << std::endl;
    sptr<VisibleWindowNumChangedListenerFuture> listener = new VisibleWindowNumChangedListenerFuture();
    if (listener == nullptr) {
        return 0;
    }
    WindowManager::GetInstance().RegisterVisibleWindowNumChangedListener(listener);

    std::cout << "You can open a window in during 20s" << std::endl;
	std::vector<DiffScreenVisibleWindowNum> diffScreenVisibleWindowNum;
    diffScreenVisibleWindowNum = listener->future_.GetResult(VisibleWindowNumChangedListenerFuture::WAIT_TIME);
	for (const auto& num : diffScreenVisibleWindowNum) {
         std::cout << "callback displayId = " << num.displayId << "callback visibleWindowNum = " << num.visibleWindowNum << std::endl;
	}

    listener->future_.Reset(diffScreenVisibleWindowNum);
    std::cout << "You can close the window, hide it in during 20s" << std::endl;
    result = listener->future_.GetResult(VisibleWindowNumChangedListenerFuture::WAIT_TIME);
    for (const auto& num : diffScreenVisibleWindowNum) {
         std::cout << "callback displayId = " << num.displayId << "callback visibleWindowNum = " << num.visibleWindowNum << std::endl;
	}

    WindowManager::GetInstance().UnregisterDrawingContentChangedListener(listener);
    std::cout << "============================End============================" << std::endl;
    return 0;
}