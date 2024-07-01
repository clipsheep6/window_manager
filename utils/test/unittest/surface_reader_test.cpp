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
#include "surface_reader_handler_impl.h"

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
    SurfaceReader* reader = new (std::nothrow) SurfaceReader();
    bool res = reader->Init();
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "SurfaceReaderTest: Init end";
}


/**
 * @tc.name: OnVsync
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceReaderTest, OnVsync, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "SurfaceReaderTest: OnVsync start";
    SurfaceReader* reader = new(std::nothrow) SurfaceReader();
    bool res = reader->Init();
    ASSERT_EQ(res, true);
    reader->OnVsync();
    GTEST_LOG_(INFO) << "SurfaceReaderTest: OnVsync end";
}

/**
 * @tc.name: ProcessBuffer
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SurfaceReaderTest, ProcessBuffer, Function | SmallTest | Level2)
{
    SurfaceReader surfaceReader;
    surfaceReader.Init();
    GTEST_LOG_(INFO) << "SurfaceReaderTest: ProcessBuffer start";
    sptr<SurfaceBuffer> cbuffer = nullptr;
    sptr<SurfaceReaderHandlerImpl> handler = nullptr;
    surfaceReader.SetHandler(handler);
    ASSERT_FALSE(surfaceReader.ProcessBuffer(cbuffer));
    GTEST_LOG_(INFO) << "SurfaceReaderTest: ProcessBuffer end";
}


} // namespace
}
}
