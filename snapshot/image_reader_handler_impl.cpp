/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: image reader
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

#include "image_reader_handler_impl.h"

namespace OHOS {
bool ImageReaderHandlerImpl::OnImageAvalible(sptr<Media::PixelMap> pixleMap)
{
    if (flag_ == false) {
        flag_ = true;
        pixleMap_ = pixleMap;
        printf("Get an Image!\n");
    }
    return true;
}

bool ImageReaderHandlerImpl::IsImageOk() const
{
    return flag_;
}

sptr<Media::PixelMap> ImageReaderHandlerImpl::GetPixelMap()
{
    return pixleMap_;
}
}