/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: image reader handler
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

#ifndef IMAGE_READER_HANDLER_H
#define IMAGE_READER_HANDLER_H

#include "pixel_map.h"

namespace OHOS {
class ImageReaderHandler : public RefBase {
public:
    ImageReaderHandler() {}
    virtual ~ImageReaderHandler() noexcept
    {
    }
    virtual bool OnImageAvalible(sptr<Media::PixelMap> pixleMap) = 0;
};
}

#endif // IMAGE_READER_HANDLER_H
