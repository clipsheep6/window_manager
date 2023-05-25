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

#include "window_model.h"

namespace OHOS {
namespace Previewer {

PreviewerWindow::PreviewerWindow()
{
}

PreviewerWindow& PreviewerWindow::GetInstance()
{
    static PreviewerWindow instance;
    return instance;
}

void PreviewerWindow::SetWindowParams(const PreviewerWindowModel& windowModel)
{
  this->windowModel = windowModel;
}

PreviewerWindowModel& PreviewerWindow::GetWindowParams()
{
  return this->windowModel;
}
    
void PreviewerWindow::SetWindowObject(const Rosen::WindowImpl *window)
{
  this->window = const_cast<Rosen::WindowImpl*>(window);
}

Rosen::WindowImpl* PreviewerWindow::GetWindowObject()
{
  return this->window;
}

} // namespace Previewer
} // namespace OHOS