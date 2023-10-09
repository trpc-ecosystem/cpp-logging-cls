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

#pragma once

#include <unistd.h>
#include <iostream>
#include <string>

#include "cls_sdk/result.h"
#include "trpc/util/log/logging.h"

namespace trpc::cls {

class CallbackResult : public tencent_log_sdk_cpp_v2::CallBack {
 public:
  CallbackResult() = default;
  ~CallbackResult() = default;

  void Success(tencent_log_sdk_cpp_v2::PostLogStoreLogsResponse result) override {
    TRPC_FMT_INFO("cls log report success: {}", result.Printf());
  }

  void Fail(tencent_log_sdk_cpp_v2::PostLogStoreLogsResponse result) override {
    TRPC_FMT_ERROR("cls log report fail, reason: {}, debug_msg:{}", result.Printf(),
                   result.loggroup_.ShortDebugString().c_str());
  }
};

using CallbackResultPtr = std::shared_ptr<CallbackResult>;

}  // namespace trpc::cls
