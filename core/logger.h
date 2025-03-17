#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>

// Indicates the path characters length. You can change it if you want to.
#define MAX_PATH_SIZE 100
#define ARRAY_LEN(arr) sizeof(arr)/sizeof(arr[0])

// You need to understand ANSI escape code to understand this shit.
#define SET_RED() fprintf(stderr, "\033[1;31m")
#define SET_YELLOW() fprintf(stderr, "\x1b[1;33m")
#define RESET_COLOR() fprintf(stderr, "\033[0m")

// Prints error with red color. Only works in linux.
#define PRINT_ERROR(message) \
    SET_RED(); \
    fprintf(stderr, "%s\n", message); \
    RESET_COLOR(); \

#define PRINT_INFO(message) \
    SET_YELLOW(); \
    fprintf(stderr, "%s\n", message); \
    RESET_COLOR(); \

#define TODO(message) \
    fprintf(stderr, "[TODO]: %s", message);

#endif