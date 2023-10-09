cc_library(
    name = "cls_api",
    srcs = glob(
        include = [
            "cls_sdk/*.cpp",
        ],
        exclude = [
            "cls_sdk/syncsample.cpp",
            "cls_sdk/sample.cpp",
            "cls_sdk/logproducerconfig.pb.cc",
            "cls_sdk/cls_logs.pb.cc",
        ],
    ),
    hdrs = glob(
        include = [
            "cls_sdk/*.h",
        ],
        exclude = [
            "cls_sdk/logproducerconfig.pb.h",
            "cls_sdk/cls_logs.pb.h",
        ],
    ),
    copts = [
        "-Wno-deprecated-declarations",
        "-fPIC",
        "-Icls_sdk/",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":cls_logs_cc_proto",
        ":logproducerconfig_cc_proto",
        ":lz4",
        ":rapidjson",
        "@com_google_protobuf//:protobuf",
        "@local_boost//:boost",
        "@com_github_curl_curl//:libcurl",
        "@com_github_openssl_openssl//:libcrypto",
        "@com_github_openssl_openssl//:libssl",
    ],
)

cc_library(
    name = "rapidjson",
    hdrs = glob(
        include = [
            "rapidjson/**/*.h",
        ],
    ),
)

cc_library(
    name = "lz4",
    srcs = ["lz4/lz4.c"],
    hdrs = ["lz4/lz4.h"],
)

# cls-sdk使用的桩代码通过protoc2.6.0生成, 框架默认引入的protobuf版本为3.8.0
# 这里根据cls-sdk提供的proto文件再重新生成一份桩代码
cc_proto_library(
    name = "cls_logs_cc_proto",
    visibility = ["//visibility:public"],
    deps = [":cls_logs_proto"],
)

proto_library(
    name = "cls_logs_proto",
    srcs = [
        "cls_logs.proto",
    ],
    import_prefix = "cls_sdk/",
)

cc_proto_library(
    name = "logproducerconfig_cc_proto",
    visibility = ["//visibility:public"],
    deps = [":logproducerconfig_proto"],
)

proto_library(
    name = "logproducerconfig_proto",
    srcs = [
        "logproducerconfig.proto",
    ],
    import_prefix = "cls_sdk/",
)
