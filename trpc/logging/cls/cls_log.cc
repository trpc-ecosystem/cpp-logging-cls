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

#include <regex>
#include <unordered_map>
#include <vector>

#include "spdlog/pattern_formatter.h"
#include "trpc/client/client_context.h"
#include "trpc/common/config/trpc_config.h"
#include "trpc/server/server_context.h"

#include "trpc/logging/cls/cls_log.h"

namespace trpc::cls {

std::unordered_map<std::string, uint32_t> logger_plungin_id_map;

void SetLoggerPlunginId(const std::string& logger_name, uint32_t plugin_id) {
  logger_plungin_id_map[logger_name] = plugin_id;
}

uint32_t GetPlunginIdFromLogger(const std::string& logger_name) { return logger_plungin_id_map[logger_name]; }

std::unordered_map<std::string, uint32_t>& GetLoggerPlunginIdMap() { return logger_plungin_id_map; }

void ClsLog::initSDK() {
  // Init cls-sdk client.
  sdk_config_.set_maxsendworkercount(1);
  sdk_config_.set_endpoint(endpoint_);
  sdk_config_.set_acceskeyid(secret_id_);
  sdk_config_.set_accesskeysecret(secret_key_);
  std::string source_ip = trpc::TrpcConfig::GetInstance()->GetGlobalConfig().local_ip;
  sdk_config_.set_source(source_ip);

  cls_sink_->SetLogClient(sdk_config_);
  cls_sink_->SetTopicId(topic_id_);
  cls_sink_->SetLogCallback(std::make_shared<CallbackResult>());
  cls_sink_->Start();
}

bool ClsLog::initSpdLogger() {
  auto formatter =
      std::make_unique<spdlog::pattern_formatter>(spdlog::pattern_time_type::local, spdlog::details::os::default_eol);
  cls_sink_->set_formatter(std::move(formatter));
  std::string name(logger_name_);
  name.append("-");
  name.append(Name());

  // mode 1 synchronous 2 asynchronous 3 extreme
  if (mode_ == 1) {
    logger_ = std::make_shared<spdlog::logger>(name, cls_sink_);
  } else {
    if (mode_ != 2 && mode_ != 3) {
      std::cerr << "ClsLogSink mode: " << mode_ << "is invalid" << std::endl;
      return false;
    }
    auto policy = mode_ == 2 ? spdlog::async_overflow_policy::block : spdlog::async_overflow_policy::overrun_oldest;
    thread_pool_ = std::make_shared<spdlog::details::thread_pool>(kThreadPoolQueueSize, 1);
    logger_ = std::make_shared<spdlog::async_logger>(name, cls_sink_, thread_pool_, policy);
  }
  // register the logger to flush the logs regularly
  logger_->flush_on(spdlog::level::critical);
  logger_->set_level(spdlog::level::trace);
  spdlog::register_logger(logger_);

  return true;
}

int ClsLog::Init() noexcept {
  cls_sink_ = std::make_shared<ClsSinkMt>();

  // Initialize spdlog logger configuration
  if (!initSpdLogger()) {
    TRPC_FMT_ERROR("Init spdlog logger fail!");
    return -1;
  }

  // Initialize the cls-sdk configuration
  initSDK();

  return 0;
}

void ClsLog::Log(const Log::Level level, const char* filename_in, int line_in, const char* funcname_in,
                 const std::string_view msg, const std::unordered_map<uint32_t, std::any>& extend_fields_msg) {
  if (extend_fields_msg.empty()) {
    return;
  }

  // extract custom fields
  auto custom_kv_vec =
      std::any_cast<std::unordered_map<std::string, std::string>>(extend_fields_msg.at(this->GetPluginID()));
  std::string formatted_kv;
  formatted_kv += fmt::format("msg={};", msg);
  for (const auto kv : custom_kv_vec) {
    formatted_kv += fmt::format("{}={};", kv.first, kv.second);
  }
  auto spd_level = static_cast<spdlog::level::level_enum>(static_cast<int>(level));
  logger_->log(spdlog::source_loc{filename_in, line_in, funcname_in}, spd_level, formatted_kv);
}

}  // namespace trpc::cls
