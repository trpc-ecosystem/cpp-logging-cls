"""This module contains some dependency"""

# buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def clean_dep(dep):
    return str(Label(dep))

# buildifier: disable=function-docstring-args
def logging_cls_workspace(path_prefix = "", repo_name = "", **kwargs):
    """Build rules for the trpc project

    Note: The main idea is to determine the required version of dependent software during the build process
          by passing in parameters.
    Args:
        path_prefix: Path prefix.
        repo_name: Repository name of the dependency.
        kwargs: Keyword arguments, dictionary type, mainly used to specify the version and sha256 value of
                dependent software, where the key of the keyword is constructed by the `name + version`.
                eg: protobuf_ver,zlib_ver...
    Example:
        trpc_workspace(path_prefix="", repo_name="", protobuf_ver="xxx", protobuf_sha256="xxx", ...)
        Here, `xxx` is the specific specified version. If the version is not specified through the key,
        the default value will be used. eg: protobuf_ver = kwargs.get("protobuf_ver", "3.8.0")
    """

    http_archive(
        name = "com_github_tencentcloud_cls_sdk_cpp",
        sha256 = "008202015efefd287eb6f14e3678ec812918010793641f473c9ee8ee6e8f76dc",
        strip_prefix = "tencentcloud-cls-sdk-cpp-1.0.4",
        urls = [
            "https://github.com/TencentCloud/tencentcloud-cls-sdk-cpp/archive/refs/tags/1.0.4.tar.gz",
        ],
        build_file = "//third_party/com_github_tencentcloud_cls_sdk_cpp:cls_cpp_sdk.BUILD",
        patches = [
            "//third_party/com_github_tencentcloud_cls_sdk_cpp:0001-change-path-with-rebuild-proto.patch",
            "//third_party/com_github_tencentcloud_cls_sdk_cpp:0002-rename-cls-to-cls_sdk.patch",
        ],
        patch_args = ["-p1"],
    )

    native.new_local_repository(
        name = "local_boost",
        build_file = "//third_party/boost:boost.BUILD",
        path = "/usr",
    )

    # com_github_curl_curl
    curl_ver = kwargs.get("curl_ver", "7.81.0")
    curl_path_ver = kwargs.get("curl_path_ver", "7_81_0")
    curl_sha256 = kwargs.get("curl_sha256", "61570dcebdf913c3675c91decd512f9bfe352f257036a86b73dabf2035eefeca")
    curl_urls = [
        "https://github.com/curl/curl/releases/download/curl-{path_ver}/curl-{ver}.zip".format(ver = curl_ver, path_ver = curl_path_ver),
    ]
    http_archive(
        name = "com_github_curl_curl",
        sha256 = curl_sha256,
        strip_prefix = "curl-{ver}".format(ver = curl_ver),
        build_file = "//third_party/com_github_curl_curl:curl.BUILD",
        urls = curl_urls,
        patches = [
            "//third_party/com_github_curl_curl:0001-generate-header-files.patch",
        ],
        patch_args = ["-p1"],
    )
