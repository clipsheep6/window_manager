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

#include "image_reader.h"
#include <securec.h>

#include "unique_fd.h"

using namespace OHOS::Media;

namespace OHOS {
const int BPP = 4; // bytes per pixel

ImageReader::ImageReader()
{
}

ImageReader::~ImageReader()
{
    DeInit();
}

void ImageReader::DeInit()
{
    if (csurface_ != nullptr) {
        csurface_->UnregisterConsumerListener();
    }
    psurface_ = nullptr;
    csurface_ = nullptr;
}

bool ImageReader::Init()
{
    csurface_ = Surface::CreateSurfaceAsConsumer();
    if (csurface_ == nullptr) {
        return false;
    }

    auto producer = csurface_->GetProducer();
    psurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (psurface_ == nullptr) {
        return false;
    }

    listener_ = new BufferListener(*this);
    SurfaceError ret = csurface_->RegisterConsumerListener(listener_);
    if (ret != SURFACE_ERROR_OK) {
        return false;
    }
    return true;
}

void ImageReader::OnVsync()
{
    printf("ImageReader::OnVsync\n");

    sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp = 0;
    Rect damage;
    auto sret = csurface_->AcquireBuffer(cbuffer, fence, timestamp, damage);
    UniqueFd fenceFd(fence);
    if (cbuffer == nullptr || sret != OHOS::SURFACE_ERROR_OK) {
        printf("ImageReader::OnVsync: surface buffer is null!\n");
        return;
    }

    ProcessBuffer(cbuffer);

    if (cbuffer != prevBuffer_) {
        if (prevBuffer_ != nullptr) {
            SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
            if (ret != SURFACE_ERROR_OK) {
                printf("ImageReader::OnVsync: release buffer error!\n");
                return;
            }
        }

        prevBuffer_ = cbuffer;
    }
}

sptr<Surface> ImageReader::GetSurface() const
{
    return psurface_;
}

void ImageReader::SetHandler(sptr<ImageReaderHandler> handler)
{
    handler_ = handler;
}

void ImageReader::ProcessBuffer(const sptr<SurfaceBuffer> &buf)
{
    if (handler_ == nullptr) {
        printf("ImageReaderHandler not set!\n");
        return;
    }

    BufferHandle *bufferHandle =  buf->GetBufferHandle();
    if (bufferHandle == nullptr) {
        printf("bufferHandle nullptr!\n");
        return;
    }

    uint32_t width = bufferHandle->width;
    uint32_t height = bufferHandle->height;
    uint32_t stride = bufferHandle->stride;
    uint8_t *addr = (uint8_t *)buf->GetVirAddr();

    auto data = (uint8_t *)malloc(width * height * BPP);
    if (data == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < height; i++) {
        memcpy_s(data + width * i * BPP,  width * BPP, addr + stride * i, width * BPP);
    }

    sptr<PixelMap> pixelMap = new PixelMap();
    ImageInfo info;
    info.size.width = width;
    info.size.height = height;
    info.pixelFormat = PixelFormat::RGBA_8888;
    info.colorSpace = ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);

    // data will free by pixelMap->FreePixelMap() in ~PixelMap()
    pixelMap->SetPixelsAddr(data, nullptr, width * height, AllocatorType::HEAP_ALLOC, nullptr);

    handler_->OnImageAvalible(pixelMap);
}
}