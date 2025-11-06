# Contributing to vulkan-tutorials

## Building and Testing vulkan-tutorials

vulkan-tutorials requires [CMake](https://cmake.org) to build.
The basic build & test process is to configure the build system, build the software, and run the tests:

```bash
cmake -B build/
cmake --build build/
ctest --test-dir build/
```

Check the [`cmake`](https://cmake.org/cmake/help/latest/manual/cmake.1.html) and [`ctest`](https://cmake.org/cmake/help/latest/manual/ctest.1.html) documentation for complete command options.

### CMake Configuration Options

vulkan-tutorials provides CMake configuration options to control the build system.

- `vulkan-tutorials_DEV_BUILD`
  - **Type** `BOOL`
  - **Default Value** `false`
  - **Description**
    This option controls the build system for consumer vs. developer builds.
    When `false`, vulkan-tutorials configures a consumer build.
    Consumer builds disable strict compiler warnings & errors, tests, etc.
    When `true`, vulkan-tutorials configures a developer build.
    Developer builds enable strict compiler warnings, build the tests, etc.
  - **Example** `cmake -D vulkan-tutorials_DEV_BUILD:BOOL=on -B build/`

### Tests

vulkan-tutorials uses [Catch2](https://github.com/catchorg/catch2) as a testing framework and [CTest](https://cmake.org) as a test manager and runner.

At configure time, CMake runs these steps:

1. Download Catch2 and add it to the build system with the [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) module.
1. Register tests with CTest using Catch2's [`catch_discover_tests()`](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#automatic-test-registration) command.

At build time, CMake runs these steps:

1. Build Catch2.
1. Build the tests.

At test time, CTest runs the tests.
