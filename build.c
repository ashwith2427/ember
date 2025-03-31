#include <stdio.h>
#define BUILD_IMPLEMENTATION
#include "build.h"
void __add_source_impl(Builder *self, const char *path){
    FILE* file = fopen(path, "r");
    if(!file){
        PRINT_ERROR("No source found.");
    }
    int new_size = strlen(self->command) + strlen(path) + 2;
    self->command = realloc(self->command, new_size);
    strcat(self->command, path);
}
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