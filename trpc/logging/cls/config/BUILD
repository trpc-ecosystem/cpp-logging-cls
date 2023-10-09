licenses(["notice"])

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "cls_log_sink_conf",
    srcs = ["cls_log_sink_conf.cc"],
    hdrs = [
        "cls_log_sink_conf.h",
        "cls_log_sink_conf_parser.h",
    ],
    deps = [
        "@com_github_fmtlib_fmt//:fmtlib",
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@trpc_cpp//trpc/common/config:config_helper",
        "@trpc_cpp//trpc/common/config:default_log_conf",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)
