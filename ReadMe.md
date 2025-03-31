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
    Builder* builder = init_builder();
    
    Executable* exe = init_executable("main", "main.c", OPTIMIZATION_NONE);
    builder->add_executable(builder, exe);
    
    Library* library = init_library("test", "build", true);
    builder->generate_library(builder, library);
    builder->link_library(builder, library);
    
    builder->add_system_library(builder, "vulkan");
    builder->install(builder);
    
    free_library(library);
    free_executable(exe);
    free_builder(builder);
    
    return 0;
}```

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