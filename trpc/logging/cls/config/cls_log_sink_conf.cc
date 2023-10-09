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

#include "trpc/logging/cls/config/cls_log_sink_conf.h"

#include <string>
#include <vector>

#include "trpc/util/log/logging.h"
#include "yaml-cpp/yaml.h"

#include "trpc/logging/cls/config/cls_log_sink_conf_parser.h"

namespace trpc {

/// @brief Get all cls log sink config under the cls.
bool GetAllClsLogSinkConfig(std::vector<ClsLogSinkConfig>& config_list) {
  YAML::Node logger_node_list;
  if (!GetDefaultLogNode(logger_node_list)) {
    return false;
  }
  for (auto logger : logger_node_list) {
    YAML::Node cls_node;
    if (!GetClsLogSinkNodes(logger, cls_node)) {
      continue;
    }
    ClsLogSinkConfig config;

    // Convert node to ClsLogSinkConfig
    if (!YAML::convert<ClsLogSinkConfig>::decode(cls_node, config)) {
      TRPC_FMT_ERROR("Convert cls log sink config error!");
      return false;
    }
    config.logger_name = logger["name"].as<std::string>();
    config.mode = logger["mode"].as<unsigned int>();
    config_list.push_back(config);
  }

  return true;
}

void ClsLogSinkConfig::Display() const {
  TRPC_LOG_DEBUG("--------------------------------");
  TRPC_LOG_DEBUG("topic_id:" << topic_id);
  TRPC_LOG_DEBUG("cls_region:" << cls_region);
  TRPC_LOG_DEBUG("secret_id:" << secret_id);
  TRPC_LOG_DEBUG("secret_key:" << secret_key);
  TRPC_LOG_DEBUG("key_message:" << key_message);
  TRPC_LOG_DEBUG("--------------------------------");
}

}  // namespace trpc