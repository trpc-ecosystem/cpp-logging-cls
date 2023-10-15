[中文](./README.zh_CN.md)

[![LICENSE](https://img.shields.io/badge/license-Apache--2.0-green.svg)](https://github.com/trpc-ecosystem/cpp-logging-cls/blob/main/LICENSE)
[![Releases](https://img.shields.io/github/release/trpc-ecosystem/cpp-logging-cls.svg?style=flat-square)](https://github.com/trpc-ecosystem/cpp-logging-cls/releases)
[![Build Status](https://github.com/trpc-ecosystem/cpp-logging-cls/actions/workflows/ci.yml/badge.svg)](https://github.com/trpc-ecosystem/cpp-logging-cls/actions/workflows/ci.yml)
[![Coverage](https://codecov.io/gh/trpc-ecosystem/cpp-logging-cls/branch/main/graph/badge.svg)](https://app.codecov.io/gh/trpc-ecosystem/cpp-logging-cls/tree/main)

# Overview

[CLS](https://cloud.tencent.com/product/cls) is a real-time, scalable log service provided by Tencent Cloud, used for collecting, storing, querying, and analyzing log data. CLS can help users quickly locate problems, reduce operation and maintenance costs, and meet multiple scenarios such as monitoring, alarming, auditing, and more. To facilitate users to connect to the CLS system, we have developed a remote log output plugin based on the tRPC-Cpp framework. Users can easily send log data to the CLS platform and perform real-time analysis and queries on the CLS platform.

# Design Overview

## Inheritance Relationship

The ClsSink class inherits from spdlog::sinks::base_sink<Mutex> and serves as a custom spdlog log receiver responsible for sending logs to the Tencent Cloud CLS platform. At the same time, the CLS plugin provides an initialization interface by implementing the Init() function in cls_log_api.h for use in the tRPC-Cpp framework.

## ClsSink Class Details

The ClsSink class is a custom spdlog log receiver responsible for sending logs to the Tencent Cloud CLS platform. It mainly contains the following member functions:

· SetLogCallback(): Sets the log callback function to handle log sending results.
· SetTopicId(): Sets the log topic ID to specify which topic the log data will be sent to.
· SetLogClient(): Sets the log client for communication with the Tencent Cloud CLS platform.
· Start(): Starts the log client.
· Stop(): Stops the log client.

# Usage Instructions

Detailed usage examples can be found in [CLS examples](./examples/)。

## Import Dependencies

### Bazel

1. Import repository

   In the project's WORKSPACE file, import the cpp-cls-logging repository and its dependencies:
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

2. Import plugin

   In the target where CLS is needed, import the "@cpp-cls-logging//trpc/logging/cls:cls_log_api" dependency. For example:
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

Not supported yet.

## Plugin Configuration

To use the Etcd plugin, you must add the plugin configuration to the framework configuration file:
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

The following is a detailed explanation of these configuration items:

| Configuration Item | Type   | Default Value                      | Description                                                  |
| ------------------ | ------ | ---------------------------------- | ------------------------------------------------------------ |
| secret_id          | string | No default value, must be set      | User's Tencent Cloud account SecretId, used for authentication |
| secret_key         | string | No default value, must be set      | User's Tencent Cloud account SecretKey, used for authentication |
| region             | string | Empty string (optional configuration) | CLS service region, e.g., "ap-shanghai"                      |
| topic_id           | string | Empty string (optional configuration) | Log topic ID, used to specify which topic the log data will be sent to |

## Initialization

The CLS plugin provides a plugin registration interface ::trpc::cls::Init(), and users need to call this interface before starting the framework.

For server scenarios, users need to call the TrpcApp::RegisterPlugins function in the service startup:

    ```cpp
    #include "trpc/logging/cls/cls_log_api.h"

    class HelloworldServer : public ::trpc::TrpcApp {
     public:
      ...
      int RegisterPlugins() override {
        ::trpc::logging::cls::Init();
        return 0;
      }
    };
    ```

2. For pure client scenarios, you need to call it after starting the framework configuration initialization and before starting other framework modules:

    ```cpp
    #include "trpc/logging/cls/cls_log_api.h"

    int main(int argc, char** argv) {
      // Initialize the framework configuration.
      // ...
      
      // Initialize the CLS plugin.
      ::trpc::logging::cls::Init();
      
      // Start the framework and other modules.
      // ...
      
      return 0;
    }
    ```

## Precautions

* Before using the CLS plugin, you need to ensure that the CLS service is correctly activated and configured. In addition, you also need to know how to create log topics and configure log delivery rules on the Tencent Cloud console. The following is a brief introduction to these two aspects:

### Activate and configure CLS service
1. Log in to the Tencent Cloud Console [TencentCloud](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fconsole.cloud.tencent.com%2F) and find the Cloud Log Service[CLS](https://cloud.tencent.com/product/cls) in the product list.

2. In the CLS console, create a new log set.

3. In the newly created log set, create a new log topic. During the creation process, you need to specify parameters such as the log topic name, region, and storage period.

4. In the newly created log topic, configure log delivery rules. You can set the delivery target (e.g., COS, CDN, etc.), delivery frequency, and delivery file format according to your needs.

### Get the Topic ID of the log topic
1. Log in to the Tencent Cloud Console [TencentCloud](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fconsole.cloud.tencent.com%2F) and find the Cloud Log Service[CLS](https://cloud.tencent.com/product/cls) in the product list.

2. Enter the page of the log topic you just created.

3. On the basic information page of the log topic, find the Topic ID of the log topic. Add this Topic ID to the plugin configuration to send log data to that topic.

### Special Tips
1. To ensure that log data can be correctly sent to the CLS platform, please make sure your Tencent Cloud account has sufficient permissions. When using the CLS plugin, you need to configure the SecretId and SecretKey in the yaml file. These two parameters can be created and obtained in the API key management page of the Tencent Cloud Console[TencentCloud](https://cloud.tencent.com/login?s_url=https%3A%2F%2Fconsole.cloud.tencent.com%2F).

2. Please note that sending log data to the CLS platform may incur certain costs. For specific cost standards, please refer to the billing instructions in the Tencent Cloud official document [TencentCloud](https://buy.cloud.tencent.com/pricing).

3. To avoid unnecessary costs, please make sure to turn off the CLS plugin when it is no longer needed. You can turn off the plugin by deleting the related configuration of the CLS plugin in the framework configuration file.
