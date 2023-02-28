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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_RS_DISPLAY_NODE_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_RS_DISPLAY_NODE_H
#include <gmock/gmock.h>
#include <ui/rs_display_node.h>

namespace OHOS {
namespace Rosen {
class MockRSDisplayNode : public RSDisplayNode {
public:
    MockRSDisplayNode(const RSDisplayNodeConfig& config) : RSDisplayNode(config) {}
    using RSDisplayNode::AddChild;
    MOCK_METHOD2(AddChild, void(SharedPtr child, int index));
    using RSDisplayNode::RemoveChild;
    MOCK_METHOD1(RemoveChild, void(SharedPtr child));
};
} // namespace Rosen
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_RS_DISPLAY_NODE_H
