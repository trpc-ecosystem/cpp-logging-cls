[English](./README.md)

[![LICENSE](https://img.shields.io/badge/license-Apache--2.0-green.svg)](https://github.com/trpc-ecosystem/cpp-logging-cls/blob/main/LICENSE)
[![Releases](https://img.shields.io/github/release/trpc-ecosystem/cpp-logging-cls.svg?style=flat-square)](https://github.com/trpc-ecosystem/cpp-logging-cls/releases)
[![Build Status](https://github.com/trpc-ecosystem/cpp-logging-cls/actions/workflows/ci.yml/badge.svg)](https://github.com/trpc-ecosystem/cpp-logging-cls/actions/workflows/ci.yml)
[![Coverage](https://codecov.io/gh/trpc-ecosystem/cpp-logging-cls/branch/main/graph/badge.svg)](https://app.codecov.io/gh/trpc-ecosystem/cpp-logging-cls/tree/main)

# 前言

[CLS](https://cloud.tencent.com/product/cls) 是腾讯云提供的一种实时、可扩展的日志服务，用于收集、存储、查询和分析日志数据。CLS可以帮助用户快速定位问题，降低运维成本，同时满足监控、报警、审计等多种场景需求。为了方便用户对接CLS系统，我们基于tRPC-Cpp框架开发了远程日志输出插件，用户可以轻松地将日志数据发送到CLS平台，并在CLS平台上进行实时分析和查询。

# 设计概要介绍

## 继承关系

ClsSink 类继承自 spdlog::sinks::base_sink<Mutex>，作为自定义的 spdlog 日志接收器，负责将日志发送到腾讯云 CLS 平台。同时，CLS 插件通过实现 cls_log_api.h 中的 Init() 函数，提供了一个初始化接口，以便在 tRPC-Cpp 框架中使用。

## ClsSink 类介绍

ClsSink 类是一个自定义的 spdlog 日志接收器，负责将日志发送到腾讯云 CLS 平台。它主要包含以下成员函数：

· SetLogCallback()：设置日志回调函数，用于处理日志发送结果。
· SetTopicId()：设置日志主题 ID，用于指定将日志数据发送到哪个主题。
· SetLogClient()：设置日志客户端，用于与腾讯云 CLS 平台进行通信。
· Start()：启动日志客户端。
· Stop()：停止日志客户端。

# 使用说明

详细的使用例子可以参考：[CLS examples](./examples/)。

## 引入依赖

### Bazel

1. 引入仓库

    在项目的WORKSPACE文件中，引入cpp-cls-logging仓库及其依赖：
    ```
    load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

    git_repository(
        name = "trpc_cpp",
        remote = "https://github.com/trpc-group/trpc-cpp.git",
        branch = "main",
    )
    
    load("@trpc_cpp//trpc:workspace.bzl", "trpc_workspace")
    trpc_workspace()
    
    git_repository(
        name = "cpp-cls-logging",
        remote = "https://github.com/trpc-ecosystem/cpp-logging-cls.git",
        branch = "main",
    )
    
    load("@cpp-cls-logging//trpc:workspace.bzl", "logging_cls_workspace")
    logging_cls_workspace()
    ```

2. 引入插件

   在需要用到CLS的目标中引入“@cpp-cls-logging//trpc/logging/cls:cls_log_api”依赖。例如：
   ```
   cc_binary(
      name = "helloworld_server",
      srcs = ["helloworld_server.cc"],
      deps = [
          "@cpp-cls-plugin//trpc/logging/cls:cls_log_api",
          ...
      ],
    )
    ```

### Cmake

暂不支持。

## 插件配置

要使用Etcd插件，必须在框架配置文件中加上插件配置：
```yaml
plugins:
  log:
    default:
      - name: default
        min_level: 2
        format: "[%Y-%m-%d %H:%M:%S.%e] [thread %t] [%l] [%@] %v"
        mode: 2
        sinks:
        raw_sinks:
          cls_log:
            topic_id: 0***********e    # cls log topic id
            cls_region: a***********u  # cls log reporting domain name
            secret_id: A***********t   # Tencent Cloud secret_id
            secret_key: J***********x  # Tencent Cloud secret_key

```

以下是这些配置项的详细说明：

| 配置项      | 类型   | 默认值            | 说明                                                         |
|-----------| ------ |----------------| ------------------------------------------------------------ |
| secret_id | string | 无默认值，必须设置  | 用户的腾讯云账户 SecretId，用于鉴权                           |
| secret_key| string | 无默认值，必须设置  | 用户的腾讯云账户 SecretKey，用于鉴权                           |
| region    | string | 空字符串（可选配置） | CLS服务所在的地域，例如 "ap-shanghai"                         |
| topic_id  | string | 空字符串（可选配置） | 日志主题的ID，用于指定将日志数据发送到哪个主题                 |

## 初始化

CLS插件提供了插件注册的接口 ::trpc::cls::Init()，用户需要在框架启动前调用该接口进行初始化。

对于服务端场景，用户需要在服务启动的 TrpcApp::RegisterPlugins 函数中调用：

    ```cpp
    #include "trpc/logging/cls/cls_log_api.h"

    class HelloworldServer : public ::trpc::TrpcApp {
     public:
      ...
      int RegisterPlugins() override {
        ::trpc::cls::Init();
        return 0;
      }
    };
    ```

2. 对于纯客户端场景，需要在启动框架配置初始化后，框架其他模块启动前调用：

    ```cpp
    #include "trpc/logging/cls/cls_log_api.h"

    int main(int argc, char** argv) {
      // Initialize the framework configuration.
      // ...
      
      // Initialize the CLS plugin.
      ::trpc::cls::Init();
      
      // Start the framework and other modules.
      // ...
      
      return 0;
    }
    ```

## 注意事项

* 在使用CLS插件之前，您需要确保已正确开通并配置了CLS服务。此外，您还需要了解如何在腾讯云控制台上创建日志主题和配置日志投递规则。以下是有关这两个方面的简要说明：

### 开通并配置CLS服务
1. 登录腾讯云控制台 [TencentCloud](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fconsole.cloud.tencent.com%2F) 在产品列表中找到云日志服务[CLS](https://cloud.tencent.com/product/cls)。

2. 在CLS控制台中，创建一个新的日志集。

3. 在新创建的日志集中，创建一个新的日志主题。在创建过程中，您需要指定日志主题的名称、地域、存储周期等参数。

4. 在新创建的日志主题中，配置日志投递规则。您可以根据需要设置投递的目标（例如COS、CDN等）、投递频率、投递文件格式等参数。

### 获取日志主题的Topic ID
1. 登录腾讯云控制台 [TencentCloud](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fconsole.cloud.tencent.com%2F) 在产品列表中找到云日志服务[CLS](https://cloud.tencent.com/product/cls)。

2. 进入刚刚创建的日志主题页面。

3. 在日志主题的基本信息页面，找到该日志主题的Topic ID。将此Topic ID添加到插件配置中，以便将日志数据发送到该主题。

### 特别提示
1. 为了确保日志数据能够正确地发送到CLS平台，请确保您的腾讯云账户具有足够的权限。在使用CLS插件时，您需要在配置SecretId和SecretKey在yaml文件中，这两个参数可以在腾讯云控制台[TencentCloud](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fconsole.cloud.tencent.com%2F)的API密钥管理页面中创建和获取。

2. 请注意，发送到CLS平台的日志数据可能会产生一定的费用。具体的费用标准，请参考腾讯云官方文档[TencentCloud](https://buy.cloud.tencent.com/pricing)中的计费说明。

3. 为了避免不必要的费用，请确保在不再需要CLS插件时关闭插件。您可以通过在框架配置文件中删除CLS插件的相关配置来关闭插件。
