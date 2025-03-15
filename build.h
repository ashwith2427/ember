/*
    Header-Only Implementation: 
    - All function definitions are inside this .h file.
    - No separate .c file is needed.
    - Just include this header to use the functionality.
    - Uses static inline to avoid multiple definition errors.

    You can see more examples from 
    - https://github.com/nothings/stb
*/


#ifndef BUILD_H_
#define BUILD_H_
#include <bits/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum{
    FORCE = 0,
    REGULAR = 1
}RmFlag;

// Indicates the path characters length. You can change it if you want to.
#define MAX_PATH_SIZE 100

// You need to understand ANSI escape code to understand this shit.
#define SET_RED() fprintf(stderr, "\033[1;31m")
#define RESET_COLOR() fprintf(stderr, "\033[0m")

// Prints error with red color. Only works in linux.
#define PRINT_ERROR(message) \
    SET_RED(); \
    fprintf(stderr, "%s\n", message); \
    RESET_COLOR(); \

#define TODO(message) \
    fprintf(stderr, "[TODO]: %s", message);

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include <dirent.h>
#endif

// Creates a directory.
bool create_directory(const char* path);

// Deletes the directory or a file. 
bool delete_directory(const char* path, RmFlag flag);
/* If flag is REGULAR then it will only delete empty directories
   without content. If you have the content you will encounter an error. The error is same as in linux
   **The folder is not empty**. So you need to recursively delete the files. */

// Gets the current working directory.
char* get_current_directory(const char* path);
// Use "." if you want to know where you are in. It is same as the cd command in linux.

// Generates the dependency graph.
FILE* generate_dependency_file(const char* path, const char* file_name);
// If filename is NULL then it will take current working file as the file_name
// and generate the dependency file.

#endif //BUILD_H

#ifdef  BUILD_IMPLEMENTATION


bool create_directory(const char* path){
    #ifdef _WIN32
        TODO("Need to implement for windows\n");
        return false;
    #else
        // Gets the status of the file or directory.
        struct stat st = {0};
        if(stat(path, &st) == -1){ // If it is positive then the folder is already created.
            if(mkdir(path, 0755) == -1){
                PRINT_ERROR("Failed to create directory!");
            }
        }
    #endif
    return true;
}

char* get_current_directory(const char* path){
    #ifdef _WIN32
        TODO("Not yet implemented for windows\n");
    #else
        int size = 100*sizeof(char);
        char* buff = (char*)malloc(MAX_PATH_SIZE);
        getcwd(buff, MAX_PATH_SIZE);
    #endif
    return buff;
}

bool delete_directory(const char* path, RmFlag flag){
    #ifdef _WIN32
        TODO("Need to implement for windows\n");
        return false;
    #else
        if(rmdir(path) == -1){
            PRINT_ERROR("Cannot delete the directory");
        }
    #endif
    return true;
}

#endif