#include <stdbool.h>
#include "logger.h"
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include <dirent.h>
#endif

typedef enum{
    FORCE = 0,
    REGULAR = 1
}RmFlag;

bool create_directory(const char* path);

// Deletes the directory or a file. 
bool delete_directory(const char* path, RmFlag flag);
/* If flag is REGULAR then it will only delete empty directories
   without content. If you have the content you will encounter an error. The error is same as in linux
   **The folder is not empty**. So you need to recursively delete the files. */

// Gets the current working directory.
char* get_current_directory(const char* path);