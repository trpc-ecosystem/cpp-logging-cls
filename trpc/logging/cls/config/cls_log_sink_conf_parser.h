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

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/printf.h"
#include "trpc/common/config/default_log_conf_parser.h"
#include "yaml-cpp/yaml.h"

#include "trpc/logging/cls/config/cls_log_sink_conf.h"

namespace YAML {

bool GetClsLogSinkNodes(YAML::Node& logger_node, YAML::Node& cls_node) {
  YAML::Node raw_sinks_node;
  // Check if raw_sinks are configured
  if (!trpc::ConfigHelper::GetInstance()->GetNode(logger_node, {"raw_sinks"}, raw_sinks_node)) {
    return false;
  }

  // Check if cls_log is configured
  if (!trpc::ConfigHelper::GetNode(raw_sinks_node, {"cls_log"}, cls_node)) {
    std::cerr << "cls not found!" << std::endl;
    return false;
  }
  return true;
}

template <>
struct convert<trpc::ClsLogSinkConfig> {
  static YAML::Node encode(const trpc::ClsLogSinkConfig& config) {
    YAML::Node node;

    node["topic_id"] = config.topic_id;
    node["cls_region"] = config.cls_region;
    node["secret_id"] = config.secret_id;
    node["secret_key"] = config.secret_key;
    node["key_message"] = config.key_message;
    node["cls_domain"] = config.cls_domain;
    node["endpoint"] = config.endpoint;
    return node;
  }

  static bool decode(const YAML::Node& node, trpc::ClsLogSinkConfig& config) {
    // required fields
    config.topic_id = node["topic_id"].as<std::string>();
    config.cls_region = node["cls_region"].as<std::string>();
    config.secret_id = node["secret_id"].as<std::string>();
    config.secret_key = node["secret_key"].as<std::string>();

    // optional fields
    if (node["request_timeout"]) {
      config.request_timeout = node["request_timeout"].as<unsigned int>();
    }
    if (node["connect_timeout"]) {
      config.connect_timeout = node["connect_timeout"].as<unsigned int>();
    }
    if (node["key_message"]) {
      config.key_message = node["key_message"].as<std::string>();
    }
    if (node["cls_domain"]) {
      config.cls_domain = node["cls_domain"].as<std::string>();
    }

    // generation config
    config.endpoint = fmt::sprintf("%s.cls.%s.com", config.cls_region, config.cls_domain);
    return true;
  }
};

}  // namespace YAML