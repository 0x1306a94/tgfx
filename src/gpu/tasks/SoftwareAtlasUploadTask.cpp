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

#include "SoftwareAtlasUploadTask.h"
#include "core/AtlasTypes.h"
#include "gpu/Gpu.h"
#include "gpu/Texture.h"

namespace tgfx {
SoftwareAtlasUploadTask::SoftwareAtlasUploadTask(
    UniqueKey uniqueKey, std::vector<std::shared_ptr<Task>> tasks,
    std::map<std::shared_ptr<TextureProxy>, std::vector<AtlasCellData>> cellDatas)
    : ResourceTask(std::move(uniqueKey)), cellTasks(std::move(tasks)),
      cellDatas(std::move(cellDatas)) {
}

bool SoftwareAtlasUploadTask::execute(Context* context) {
  if (cellTasks.empty()) {
    return false;
  }
  for (auto& task : cellTasks) {
    task->wait();
  }
  for (auto& [textureProxy, cellDatas] : cellDatas) {
    if (textureProxy == nullptr || cellDatas.empty()) {
      continue;
    }
    auto texture = textureProxy->getTexture();
    if (texture == nullptr) {
      continue;
    }
    auto gpu = context->gpu();
    for (auto& [data, info, atlasOffset] : cellDatas) {
      if (data == nullptr) {
        continue;
      }
      auto rect = Rect::MakeXYWH(atlasOffset.x, atlasOffset.y, static_cast<float>(info.width()),
                                 static_cast<float>(info.height()));
      gpu->writePixels(texture->getSampler(), rect, data->data(), info.rowBytes());
    }
  }
  return true;
}
}  // namespace tgfx
