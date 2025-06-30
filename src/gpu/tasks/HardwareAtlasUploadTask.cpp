/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making tgfx available.
//
//  Copyright (C) 2025 THL A29 Limited, a Tencent company. All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "HardwareAtlasUploadTask.h"
#include "tgfx/gpu/Context.h"

namespace tgfx {
HardwareAtlasUploadTask::HardwareAtlasUploadTask(
    UniqueKey uniqueKey, std::vector<std::shared_ptr<Task>> tasks,
    std::map<std::shared_ptr<PixelBuffer>, std::shared_ptr<TextureProxy>> buffers)
    : ResourceTask(std::move(uniqueKey)), cellTasks(std::move(tasks)), buffers(std::move(buffers)) {
}

bool HardwareAtlasUploadTask::execute(Context* context) {
  for (const auto& task : cellTasks) {
    task->wait();
  }
  for (const auto& [buffer, proxy] : buffers) {
    buffer->unlockPixels();
    auto texture = proxy->getTexture();
    if (texture != nullptr) {
      continue;
    }
    texture = Texture::MakeFrom(context, buffer);
    if (texture == nullptr) {
      continue;
    }
    texture->assignUniqueKey(proxy->getUniqueKey());
  }
  return true;
}
}  // namespace tgfx
