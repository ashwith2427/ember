#define BUILD_IMPLEMENTATION
#include "build.h"

int main() {
    Builder* builder = init_builder();
    Executable* exe = init_executable("main", "main.c", OPTIMIZATION_NONE);
    builder->add_executable(builder, exe);
    Library *library = init_library("test", "build", true);
    builder->generate_library(builder, library);
    builder->link_library(builder, library);
    builder->add_system_library(builder, "vulkan");
    builder->install(builder);
    free_library(library);
    free_executable(exe);
    free_builder(builder);
    return 0;
}