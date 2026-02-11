# Building with Bazel

This project now supports building with Bazel in addition to CMake.

## Prerequisites

Install Bazel: https://bazel.build/install

## Building the Project

### Build all targets
```bash
bazel build //...
```

### Build specific library
```bash
bazel build //:ezlibs
```

### Build and run tests

Build all tests:
```bash
bazel test //Tests/...
```

Run specific test:
```bash
bazel test //Tests/TestMath:TestMath
bazel test //Tests/TestTime:TestTime
bazel test //Tests/TestGeo:TestGeo
bazel test //Tests/TestMisc:TestMisc
bazel test //Tests/TestFile:TestFile
bazel test //Tests/TestApp:TestApp
bazel test //Tests/TestCom:TestCom
bazel test //Tests/TestComp:TestComp
bazel test //Tests/TestOpengl:TestOpengl
```

### Build ezBuildInc tool
```bash
bazel build //Tests/ezBuildInc:ezBuildInc
```

## Configuration

The Bazel configuration includes:
- C++11 standard (matching CMake configuration)
- Clang compiler preference on Linux (matching CMake configuration)
- Header-only library targets for selective inclusion
- Test targets with proper dependencies

## Project Structure

- `WORKSPACE`: Bazel workspace file
- `.bazelrc`: Bazel configuration options
- `BUILD.bazel`: Root build file for ezLibs headers
- `Tests/*/BUILD.bazel`: Build files for each test module

## Notes

- All CMakeLists.txt files remain unchanged and functional
- Bazel uses the same include paths and dependencies as CMake
- Test results are compatible with CMake test infrastructure
