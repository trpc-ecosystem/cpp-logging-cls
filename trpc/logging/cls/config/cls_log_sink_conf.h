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

#include <string>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace trpc {

struct ClsLogSinkConfig {
  // logger name
  std::string logger_name;
  // CLS log output mode
  unsigned int mode;
  // CLS log topic ID
  std::string topic_id;
  // CLS log reporting domain
  std::string cls_region;
  // CLS domain access method, using the internal domain name by default, and "tencentcs" for external access
  std::string cls_domain = "tencentyun";
  // Tencent Cloud secret ID
  std::string secret_id;
  // Tencent Cloud secret key
  std::string secret_key;
  // Number of logs per request
  int logs_per_request = 100;
  // CLS reporting timeout, in seconds
  int request_timeout = 50;
  // CLS client connection timeout, in seconds
  int connect_timeout = 5;
  // Log message field name
  std::string key_message = "msg";
  // Internal domain name is used by default
  std::string endpoint;

  /// @brief Display the configuration parameters of the CLS log sink.
  void Display() const;
};

/// @brief Retrieves all ClsLogSinkConfig objects and stores them in the given vector.
/// @param config_list A reference to a vector of ClsLogSinkConfig objects, which will be filled with the retrieved
/// configurations.
/// @return A boolean value indicating the success of the operation. Returns true if all configurations were retrieved
/// successfully, and false otherwise.
bool GetAllClsLogSinkConfig(std::vector<ClsLogSinkConfig>& config_list);

}  // namespace trpc
