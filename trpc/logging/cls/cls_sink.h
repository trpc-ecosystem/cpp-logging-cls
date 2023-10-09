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

#include <chrono>
#include <locale>
#include <mutex>
#include <regex>
#include <sstream>

#include "cls_sdk/cls_logs.pb.h"
#include "cls_sdk/producerclient.h"
#include "spdlog/sinks/base_sink.h"

#include "trpc/logging/cls/callback_result.h"

namespace trpc::cls {

template <typename Mutex>
class ClsSink : public spdlog::sinks::base_sink<Mutex> {
 public:
  /// @brief Set the log_callback_ member variable
  /// @param log_callback The new value to set for log_callback_
  void SetLogCallback(const CallbackResultPtr& log_callback) { log_callback_ = log_callback; }

  /// @brief Set the topic_id_ member variable
  /// @param topic_id The new value to set for topic_id_
  void SetTopicId(const std::string& topic_id) { topic_id_ = topic_id; }

  /// @brief Set the log_client_ member variable
  /// @param sdk_config The new value to set for log_client_, which is of type cls_config::LogProducerConfig
  void SetLogClient(cls_config::LogProducerConfig& sdk_config) {
    log_client_ = std::make_unique<tencent_log_sdk_cpp_v2::ProducerClient>(sdk_config);
  }

  /// @brief Start the cls log client
  void Start() {
    TRPC_ASSERT(log_client_ != nullptr && "cls log client is nullptr!");
    log_client_->Start();
  }

  /// @brief Stop the cls log client
  void Stop() {
    TRPC_ASSERT(log_client_ != nullptr && "cls log client is nullptr!");
    log_client_->LogProducerEnvDestroy();
  }

  // Used for unit tests only, not by users.
  void TestSinkIt(const spdlog::details::log_msg& msg) { sink_it_(msg); }
  void TestAddClsKV(std::string_view key, std::string_view value) { addClsKV(key, value); }
  void TestFlush() { flush_(); }
  const ::cls::Log& TestGetClsLog() const { return cls_log_; }

 private:
  // Override the sink_it_ method of the base_sink class to implement the logging functionality.
  void sink_it_(const spdlog::details::log_msg& msg) override {
    // Report log level, loggername, log line info.
    spdlog::string_view_t level_sv = spdlog::level::to_string_view(msg.level);
    addClsKV("level", std::string_view(level_sv.data(), level_sv.size()));
    addClsKV("logger_name", std::string(msg.logger_name.data(), msg.logger_name.size()));
    addClsKV("file_line", fmt::format("{}:{}:{}", msg.source.filename, msg.source.line, msg.source.funcname));
    spdlog::memory_buf_t formatted;
    this->formatter_->format(msg, formatted);
    std::string formatted_view_str(formatted.begin(), formatted.end());

    // Find the starting position of the "msg=" field
    size_t msg_start_pos = formatted_view_str.find("msg=");

    if (msg_start_pos != std::string::npos) {
      formatted_view_str = formatted_view_str.substr(msg_start_pos);
    }

    std::istringstream iss(formatted_view_str);
    std::string key_value_pair;
    char separator = ';';

    while (std::getline(iss, key_value_pair, separator)) {
      size_t pos = key_value_pair.find('=');
      if (pos != std::string::npos) {
        std::string key = key_value_pair.substr(0, pos);
        std::string value = key_value_pair.substr(pos + 1);
        addClsKV(key, value);
      }
    }
    auto ret = log_client_->PostLogStoreLogs(topic_id_, cls_log_, log_callback_);
    if (ret.statusCode != 0) {
      TRPC_FMT_ERROR("PostLogStoreLogs failed! statusCode: {}", ret.statusCode);
    }
  }

  // Add a key-value pair to the log message before sending it to CLS.
  void addClsKV(std::string_view key, std::string_view value) {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    cls_log_.set_time(static_cast<int64_t>(now_ms));
    auto log_content = cls_log_.add_contents();
    log_content->set_key(key.data());
    log_content->set_value(value.data());
  }

  /// Override the flush_ method of the base_sink class to implement the flushing functionality.
  void flush_() override{};

 private:
  std::unique_ptr<tencent_log_sdk_cpp_v2::ProducerClient> log_client_;
  CallbackResultPtr log_callback_;
  ::cls::Log cls_log_;
  std::string topic_id_;
};

using ClsSinkMt = ClsSink<std::mutex>;
using ClsSinkPtr = std::shared_ptr<ClsSinkMt>;

}  // namespace trpc::cls
