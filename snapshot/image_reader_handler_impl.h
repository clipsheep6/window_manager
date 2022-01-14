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

#ifndef IMAGE_READER_HANDLER_IMPL_H
#define IMAGE_READER_HANDLER_IMPL_H

#include "image_reader_handler.h"

namespace OHOS {
class ImageReaderHandlerImpl : public ImageReaderHandler {
public:
    bool OnImageAvalible(sptr<Media::PixelMap> pixleMap) override;
    bool IsImageOk() const;
    sptr<Media::PixelMap> GetPixelMap();

private:
    bool flag_ = false;
    sptr<Media::PixelMap> pixleMap_ = nullptr;
};
}

#endif // IMAGE_READER_HANDLER_IMPL_H
