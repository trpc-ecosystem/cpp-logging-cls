# Description: cls
licenses(["notice"])

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "cls_log",
    srcs = ["cls_log.cc"],
    hdrs = ["cls_log.h"],
    deps = [
        ":callback_result",
        ":cls_sink",
        "//trpc/logging/cls/config:cls_log_sink_conf",
        "@com_github_gabime_spdlog//:spdlog",
        "@com_github_tencentcloud_cls_sdk_cpp//:cls_api",
        "@trpc_cpp//trpc/client:client_context",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/server:server_context",
        "@trpc_cpp//trpc/util/log:logging",
        "@trpc_cpp//trpc/util/log/default:default_log",
    ],
)

cc_test(
    name = "cls_log_test",
    srcs = ["cls_log_test.cc"],
    data = ["//trpc/logging/cls/testing:cls_log_sink_test.yaml"],
    deps = [
        ":cls_log",
        "//trpc/logging/cls:cls_log_api",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@trpc_cpp//trpc/common/config:config_helper",
        "@trpc_cpp//trpc/common/config:default_value",
        "@trpc_cpp//trpc/util/log/default:default_log",
    ],
)

cc_library(
    name = "cls_sink",
    hdrs = ["cls_sink.h"],
    deps = [
        ":callback_result",
        "@com_github_gabime_spdlog//:spdlog",
        "@com_github_tencentcloud_cls_sdk_cpp//:cls_api",
    ],
)

cc_test(
    name = "cls_sink_test",
    srcs = ["cls_sink_test.cc"],
    deps = [
        ":cls_sink",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "callback_result",
    hdrs = ["callback_result.h"],
    deps = [
        "@com_github_tencentcloud_cls_sdk_cpp//:cls_api",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)

cc_library(
    name = "cls_log_api",
    srcs = ["cls_log_api.cc"],
    hdrs = ["cls_log_api.h"],
    deps = [
        "//trpc/logging/cls:cls_log",
        "//trpc/logging/cls/config:cls_log_sink_conf",
        "@trpc_cpp//trpc/common:trpc_plugin",
    ],
)
