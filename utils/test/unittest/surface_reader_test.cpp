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

#include "surface_reader.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SurfaceReaderTest : public testing::Test {
  public:
    SurfaceReaderTest() {}
    ~SurfaceReaderTest() {}
};

namespace {
/**
 * @tc.name: Init
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceReaderTest, Init, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SurfaceReaderTest: Init start";
    SurfaceReader* reader = new(std::nothrow) SurfaceReader();
    bool res = reader->Init();
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "SurfaceReaderTest: Init end";
}

/**
 * @tc.name: SetHandler
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceReaderTest, SetHandler, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SurfaceReaderTest: SetHandler start";
    SurfaceReader* reader = new(std::nothrow) SurfaceReader();
    sptr<SurfaceReaderHandler> handler;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        reader->SetHandler(handler);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
    GTEST_LOG_(INFO) << "SurfaceReaderTest: SetHandler end";
}

/**
 * @tc.name: ProcessBuffer
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceReaderTest, ProcessBuffer, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SurfaceReaderTest: ProcessBuffer start";
    SurfaceReader* reader = new(std::nothrow) SurfaceReader();
    sptr<SurfaceBuffer> buf;
    bool res = reader->ProcessBuffer(buf);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "SurfaceReaderTest: ProcessBuffer end";
}

/**
 * @tc.name: ProcessBuffer01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceReaderTest, ProcessBuffer01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SurfaceReaderTest: ProcessBuffer start";
    SurfaceReader* reader = new(std::nothrow) SurfaceReader();
    sptr<SurfaceBuffer> buf;
    sptr<SurfaceReaderHandler> handler;
    reader->SetHandler(handler);
    bool res = reader->ProcessBuffer(buf);
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "SurfaceReaderTest: ProcessBuffer end";
}
} // namespace
}
}
