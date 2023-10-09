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

#include "trpc/logging/cls/cls_sink.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace trpc::cls {

class ClsSinkTest : public ::testing::Test {
 public:
  void SetUp() override {
    cls_sink_ = std::make_shared<ClsSinkMt>();
    ASSERT_TRUE(cls_sink_ != nullptr);
  }

  bool HasClsKV(const ::cls::Log& cls_log, const std::string& key, const std::string& value) const {
    for (const auto& content : cls_log.contents()) {
      if (content.key() == key && content.value() == value) {
        return true;
      }
    }
    return false;
  }

  ClsSinkPtr cls_sink_;
};

TEST_F(ClsSinkTest, SinkItTest) {
  // Create a log message
  spdlog::source_loc src_loc{"test_file.cc", 42, "test_func"};
  spdlog::string_view_t logger_name("logger_name");
  spdlog::level::level_enum level = spdlog::level::info;
  spdlog::string_view_t msg("Test message");
  spdlog::details::log_msg log_msg(src_loc, logger_name, level, msg);

  // Call TestSinkIt method
  cls_sink_->TestSinkIt(log_msg);

  // Verify the cls_log_ contents
  const ::cls::Log& cls_log = cls_sink_->TestGetClsLog();
  ASSERT_EQ(cls_log.contents_size(), 3);

  // Print cls_log contents for debugging
  for (const auto& content : cls_log.contents()) {
    std::cout << "Key: " << content.key() << ", Value: " << content.value() << std::endl;
  }

  // Verify the key-value pairs in cls_log_
  ASSERT_TRUE(HasClsKV(cls_log, "level", "info"));
  ASSERT_TRUE(HasClsKV(cls_log, "logger_name", "logger_name"));
  ASSERT_TRUE(HasClsKV(cls_log, "file_line", "test_file.cc:42:test_func"));
}

TEST_F(ClsSinkTest, AddClsKVTest) {
  // Call TestAddClsKV method
  cls_sink_->TestAddClsKV("key", "value");

  // Verify the cls_log_ contents
  const ::cls::Log& cls_log = cls_sink_->TestGetClsLog();
  ASSERT_EQ(cls_log.contents_size(), 1);

  // Verify the key-value pair in cls_log_
  ASSERT_TRUE(HasClsKV(cls_log, "key", "value"));
}

TEST_F(ClsSinkTest, FlushTest) {
  // Add a key-value pair to cls_log_
  cls_sink_->TestAddClsKV("key", "value");

  // Store the current cls_log_ contents
  const ::cls::Log cls_log_before_flush = cls_sink_->TestGetClsLog();

  // Call TestFlush method
  cls_sink_->TestFlush();

  // Verify that the cls_log_ contents have not changed
  const ::cls::Log& cls_log_after_flush = cls_sink_->TestGetClsLog();
  ASSERT_EQ(cls_log_before_flush.contents_size(), cls_log_after_flush.contents_size());
  ASSERT_TRUE(HasClsKV(cls_log_after_flush, "key", "value"));
}

}  // namespace trpc::cls