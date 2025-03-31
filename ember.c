#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SET_RED() fprintf(stderr, "\033[1;31m")
#define SET_YELLOW() fprintf(stderr, "\x1b[1;33m")
#define RESET_COLOR() fprintf(stderr, "\033[0m")

// Prints error with red color. Only works in linux.
#define PRINT_ERROR(message)                                                   \
    SET_RED();                                                                 \
    fprintf(stderr, "[ERROR]: %s\n", message);                                 \
    RESET_COLOR();                                                             \
    abort();

#define PRINT_INFO(message)                                                    \
    SET_YELLOW();                                                              \
    fprintf(stderr, "[INFO]: %s\n", message);                                  \
    RESET_COLOR();

void delete_directory(const char *path) {
    if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0) {
        PRINT_INFO("You are either deleting project directory or parent dir. "
                   "Check once.");
        return;
    }
    struct dirent *entry;
    DIR *directory = opendir(path);
    if (directory == NULL) {
        PRINT_ERROR("Directory is invalid.");
    }
    char full_path[1024];
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(full_path, 1024, "%s/%s", path, entry->d_name);
        if (entry->d_type == DT_REG) {
            remove(full_path);
        } else if (entry->d_type == DT_DIR) {
            delete_directory(full_path);
        }
    }
    closedir(directory);
    if (rmdir(path) != 0) {
        PRINT_ERROR("Failed to remove directory.");
    }
}

typedef void (*CommandFunc)(void);

typedef struct {
    const char *name;
    CommandFunc func;
    const char *description;
} Command;

void build() {
    printf("Building project...\n");
    system("clang -g -o /tmp/build_runner build.c && /tmp/build_runner");
}

void run() {
    printf("Running project...\n");
    system("./build/main");
}
void clean(){
    printf("Cleaning in progress...\n");
    delete_directory("build");
}

void show_help() {
    printf("Usage: ember <command>\n");
    printf("Available commands:\n");
    
    extern Command commands[];
    for (int i = 0; commands[i].name != NULL; i++) {
        printf("  %-10s %s\n", commands[i].name, commands[i].description);
    }
}

Command commands[] = {
    {"build", build, "Compile the project"},
    {"run", run, "Run the compiled executable"},
    {"clean", clean, "Clean the build folder"},
    {"--help", show_help, "Show this help message"},
    {NULL, NULL, NULL}
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: No command provided. Use 'ember --help' for usage information.\n");
        return 1;
    }

    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(argv[1], commands[i].name) == 0) {
            commands[i].func();
            return 0;
        }
    }

    printf("Error: Unknown command '%s'. Use 'ember --help' for usage information.\n", argv[1]);
    return 1;
}
