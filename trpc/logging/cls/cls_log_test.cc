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

#include "trpc/logging/cls/cls_log.h"

#include "gtest/gtest.h"

#include "trpc/common/config/config_helper.h"
#include "trpc/util/log/default/default_log.h"

namespace trpc::cls {

class ClsLogTest : public ::testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(ConfigHelper::GetInstance()->Init("trpc/logging/cls/testing/cls_log_sink_test.yaml"));
    ASSERT_TRUE(trpc::log::Init());

    auto log = LogFactory::GetInstance()->Get();
    ASSERT_NE(log, nullptr);

    default_log_ = static_pointer_cast<DefaultLog>(log);
    ASSERT_NE(default_log_, nullptr);

    std::vector<ClsLogSinkConfig> config_list;
    if (!GetAllClsLogSinkConfig(config_list)) {
      return;
    }

    cls_default_ = MakeRefCounted<ClsLog>(config_list[0]);
    default_log_->RegisterRawSink(cls_default_);
  }

  void TearDown() override { trpc::log::Destroy(); }

  DefaultLogPtr default_log_;
  ClsLogPtr cls_default_;
};

TEST_F(ClsLogTest, SetLogKVPairTest) {
  // Create a client context
  auto client_context = MakeRefCounted<ClientContext>();

  // Call SetLogKVPair method
  cls_default_->SetLogKVPair(client_context, std::make_pair("custom_key_1", "custom_value_1"));
  cls_default_->SetLogKVPair(client_context, std::make_pair("custom_key_2", "custom_value_2"));
  cls_default_->SetLogKVPair(client_context, std::make_pair("custom_key_3", "custom_value_3"));
  auto logging_data =
      client_context->GetFilterData<std::unordered_map<std::string, std::string>>(cls_default_->GetPluginID());

  // Verify the custom key-value pair in the client context
  ASSERT_EQ((*logging_data)["custom_key_1"], "custom_value_1");
  ASSERT_EQ((*logging_data)["custom_key_2"], "custom_value_2");
  ASSERT_EQ((*logging_data)["custom_key_3"], "custom_value_3");
}

TEST_F(ClsLogTest, WithFieldsTest) {
  // Create a client context
  auto client_context = MakeRefCounted<ClientContext>();

  // Call WithFields with a pair of key-value pairs
  trpc::cls::WithFields(client_context, std::make_pair("key1", "value1"), std::make_pair("key2", "value2"),
                        std::make_pair("key3", "value3"));
  auto logging_data =
      client_context->GetFilterData<std::unordered_map<std::string, std::string>>(cls_default_->GetPluginID());

  // Verify the custom key-value pair in the client context
  ASSERT_EQ((*logging_data)["key1"], "value1");
  ASSERT_EQ((*logging_data)["key2"], "value2");
  ASSERT_EQ((*logging_data)["key3"], "value3");
}

}  // namespace trpc::cls
