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
#include <string_view>
#include <unordered_map>

#include "cls_sdk/producerclient.h"
#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "trpc/client/client_context.h"
#include "trpc/common/config/default_log_conf.h"
#include "trpc/log/logging.h"
#include "trpc/server/server_context.h"
#include "trpc/util/log/logging.h"

#include "trpc/common/config/default_value.h"
#include "trpc/logging/cls/callback_result.h"
#include "trpc/logging/cls/cls_sink.h"
#include "trpc/logging/cls/config/cls_log_sink_conf.h"

namespace trpc::cls {

/// @brief Set the plugin ID for a given logger name.
/// @param logger_name The name of the logger.
/// @param plugin_id The ID of the plugin associated with the logger.
void SetLoggerPluginId(const std::string& logger_name, uint32_t plugin_id);

/// @brief Get the plugin ID associated with a given logger name.
/// @param logger_name The name of the logger.
/// @return The plugin ID associated with the logger name.
uint32_t GetPluginIdFromLogger(const std::string& logger_name);

/// @brief Get the map of logger names to plugin IDs.
/// @return A reference to the unordered_map containing logger names as keys and plugin IDs as values.
std::unordered_map<std::string, uint32_t>& GetLoggerPluginIdMap();

/// @brief Adds key-value pairs to the context's filter data for each logger plugin.
/// @tparam T The type of the context object.
/// @tparam Args The parameter pack of key-value pairs (as std::pair objects).
/// @param context The context object, which should have SetFilterData and GetFilterData member functions.
/// @param key_value_pairs The key-value pairs to be added to the context's filter data.
/// @note This function iterates through all logger plugins and adds the key-value pairs to the filter data
///       associated with each plugin's ID. If a key already exists in the filter data, its value will be updated.
///       Otherwise, the new key-value pair will be inserted.
///
/// Example usage:
/// @code{.cpp}
/// trpc::cls::WithFields(context, std::make_pair("key1", "value1"), std::make_pair("key2", "value2"),
/// std::make_pair("key3", "value3"));
/// @endcode
template <typename T, typename... Args>
void WithFields(T& context, Args&&... key_value_pairs) {
  auto plugin_id_map = GetLoggerPluginIdMap();
  for (const auto& [logger_name, plugin_id] : plugin_id_map) {
    auto* data_map = context->template GetFilterData<std::unordered_map<std::string, std::string>>(
        GetPluginIdFromLogger(logger_name.c_str()));
    if (!data_map) {
      std::unordered_map<std::string, std::string> new_data_map;
      (new_data_map.emplace(std::forward<Args>(key_value_pairs)), ...);
      context->SetFilterData(GetPluginIdFromLogger(logger_name.c_str()), std::move(new_data_map));
    } else {
      (data_map->emplace(std::forward<Args>(key_value_pairs)), ...);
    }
  }
}

class ClsLog : public Logging {
 public:
  std::string Name() const override { return "cls-log"; }

  const std::string& LoggerName() const override { return logger_name_; }

  std::string Version() const  { return "1.0.0"; }

  explicit ClsLog(const ClsLogSinkConfig& sink_config)
      : logger_name_(sink_config.logger_name),
        mode_(sink_config.mode),
        topic_id_(sink_config.topic_id),
        endpoint_(sink_config.endpoint),
        secret_id_(sink_config.secret_id),
        secret_key_(sink_config.secret_key) {
    SetLoggerPluginId(logger_name_, this->GetPluginID());
    Init();
  }

  /// @brief Stop the runtime environment of the plugin
  void Stop() noexcept override { cls_sink_->Stop(); }

  /// @brief Initialize the cls-sdk and spdlog
  /// @return int 0: success / non-zero: failure
  int Init() noexcept override;

  /// @brief Output function
  /// @param level The log level
  /// @param filename_in The location information when executing the log operation,
  /// the source code file where the log execution is located
  /// @param line_in The location information when executing the log operation,
  /// the line number in the source code file where the log execution is located
  /// @param funcname_in The location information when executing the log operation,
  /// the method in the source code file where the log execution is located
  /// @param msg The message body to be printed
  /// @param extend_fields_msg The extended fields of the message
  /// @return void No return value
  void Log(const Log::Level level, const char* filename_in, int line_in, const char* funcname_in, std::string_view msg,
           const std::unordered_map<uint32_t, std::any>& extend_fields_msg) override;

  /// @brief Adds or updates a key-value pair in the context's filter data for a specific logger plugin.
  /// @tparam T The type of the context object.
  /// @param context The context object, which should have SetFilterData and GetFilterData member functions.
  /// @param kv The key-value pair (as a std::pair object) to be added or updated in the context's filter data.
  /// @note This function retrieves the existing filter data (as a std::unordered_map) associated with
  ///       the logger plugin's ID.
  ///       If the filter data does not exist, a new map is created with the key-value pair and stored in the context.
  ///       If the filter data exists, the key-value pair is inserted or updated in the map.
  ///
  /// Example usage:
  /// @code{.cpp}
  /// auto context = ...; // context should be a pointer to an object with SetFilterData and GetFilterData functions
  /// SetLogKVPair(context, std::make_pair("key1", "value1"));
  /// SetLogKVPair(context, std::make_pair("key2", "value2"));
  /// SetLogKVPair(context, std::make_pair("key3", "value3"));
  /// @endcode
  template <typename T>
  void SetLogKVPair(T& context, std::pair<std::string, std::string> kv) {
    // Retrieve the existing data
    auto* data_map = context->template GetFilterData<std::unordered_map<std::string, std::string>>(this->GetPluginID());

    // If data_map is nullptr, it means there's no existing data
    if (!data_map) {
      // Create a new map and insert the key-value pair
      std::unordered_map<std::string, std::string> new_data_map{std::move(kv)};
      context->SetFilterData(this->GetPluginID(), std::move(new_data_map));
    } else {
      // If data_map exists, insert or update the key-value pair
      data_map->emplace(std::move(kv));
    }
  }

  // Used for unit tests only, not by users.
  const ::cls::Log& TestGetLastLog() const { return cls_sink_->TestGetClsLog(); }

 private:
  // Initialize the CLS SDK
  void InitSDK();

  // Initialize the spdlog logger instance
  bool InitSpdLogger();

 private:
  // logger configuration
  std::string logger_name_;
  unsigned int mode_;

  // logging configuration
  std::string topic_id_;
  std::string endpoint_;
  std::string secret_id_;
  std::string secret_key_;

  // Spdlog settings
  ClsSinkPtr cls_sink_;
  std::shared_ptr<spdlog::logger> logger_;
  std::shared_ptr<spdlog::details::thread_pool> thread_pool_;
  static constexpr size_t kThreadPoolQueueSize = 10000;

  cls_config::LogProducerConfig sdk_config_;
};

using ClsLogPtr = RefPtr<ClsLog>;

}  // namespace trpc::cls
