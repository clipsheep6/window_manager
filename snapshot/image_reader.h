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

#ifndef IMAGE_READER_H
#define IMAGE_READER_H

#include "refbase.h"
#include "core/ui/rs_surface_node.h"
#include "image_reader_handler.h"

namespace OHOS {
class ImageReader {
public:
    ImageReader();
    virtual ~ImageReader();

    bool Init();
    void DeInit();

    sptr<Surface> GetSurface() const;
    void SetHandler(sptr<ImageReaderHandler> handler);
private:
    class BufferListener : public IBufferConsumerListener {
    public:
        explicit BufferListener(ImageReader &imgReader): imgReader_(imgReader)
        {
        }
        ~BufferListener() noexcept override = default;
        void OnBufferAvailable() override
        {
            imgReader_.OnVsync();
        }

    private:
        ImageReader &imgReader_;
    };
    friend class BufferListener;

    void OnVsync();
    void ProcessBuffer(const sptr<SurfaceBuffer> &buf);

    sptr<IBufferConsumerListener> listener_ = nullptr;
    sptr<Surface> csurface_ = nullptr; // cosumer surface
    sptr<Surface> psurface_ = nullptr; // producer surface
    sptr<SurfaceBuffer> prevBuffer_ = nullptr;
    sptr<ImageReaderHandler> handler_ = nullptr;
};
}

#endif // IMAGE_READER_H
