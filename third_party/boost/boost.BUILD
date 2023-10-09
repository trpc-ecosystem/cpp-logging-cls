licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "boost",
    hdrs = glob([
        "include/boost/**/*.hpp",
        "include/boost/**/*.h",
    ]),
    includes = ["include"],
    linkopts = ["-lboost_thread"],
)