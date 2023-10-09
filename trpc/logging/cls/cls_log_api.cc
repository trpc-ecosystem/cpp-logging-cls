//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include "trpc/common/trpc_plugin.h"

#include "trpc/logging/cls/cls_log_api.h"
#include "trpc/logging/cls/config/cls_log_sink_conf.h"

namespace trpc::cls {

bool Init() {
  std::vector<ClsLogSinkConfig> config_list;
  if (!GetAllClsLogSinkConfig(config_list)) {
    return -1;
  }
  for (const auto& config : config_list) {
    TrpcPlugin::GetInstance()->RegisterLogging(MakeRefCounted<ClsLog>(std::move(config)));
  }

  return true;
}

}  // namespace trpc::cls
