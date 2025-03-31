# Ember
A simple, header-only build system for C, written in C.

🚧 Work in Progress: Currently, Ember only supports the Clang compiler on Linux. Stay tuned for further updates.

## Installation
Include build.h and the ember executable in your project's root folder.

## Usage

```c
#define BUILD_IMPLEMENTATION
#include "build.h"

int main() {
    Builder *builder = init_builder();
    Executable *exe = init_executable("main", "main.c", OPTIMIZATION_NONE);
    builder->add_executable(builder, exe);
    builder->add_source(builder, "math.c");
    Library *library = init_library("test", "test", true);
    builder->generate_library(builder, library);
    builder->link_library(builder, library);
    builder->add_system_library(builder, "vulkan");
    builder->install(builder);
    free_library(library);
    free_executable(exe);
    free_builder(builder);
    return 0;
}
```

## Build
To build the project, run:

```bash
./ember build
```

## Run
To run your project, execute:

```bash
./ember run
```
## Features

These are the long-term goals of this build system. If you’d like to contribute, you can fork the repository, make changes, and submit a pull request (PR).

- [x] Compilation
- [x] Linking System libraries
- [x] Generating shared libraries from source (.c) and header (.h) files
- [x] Linking shared libraries
- [ ] Support for GCC and other compilers
- [ ] Compiler Optimizations
- [ ] Portability(Windows, Mac)
- [ ] Dependency management
- [ ] Parallel compilation
- [ ] External packages from network
