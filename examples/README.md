# CLS Log demo

This example demonstrates how to use CLS Log to record the communication between a client and a server. The example includes a simple client and server program.

## File Structure

```shell
$ tree examples/
examples/
├── client
│   ├── BUILD
│   ├── client.cc
│   └── trpc_cpp_fiber.yaml
├── README.md
├── run.sh
└── server
    ├── BUILD
    ├── greeter_service.cc
    ├── greeter_service.h
    ├── helloworld.proto
    ├── helloworld_server.cc
    └── trpc_cpp_fiber.yaml
```

* Compilation

We can run the following command to start the server and proxy program.

```shell
./bazel-bin/examples/server/helloworld_server --config=examples/server/trpc_cpp_fiber.yaml
```

* Run the client program

We can run the following command to start the client program.

```shell
./bazel-bin/examples/client/client --config=examples/server/trpc_cpp_fiber.yaml
```

* View the log data

You need to log in to Tencent Cloud CLS console to check whether the corresponding client/server log has been uploaded successfully.
