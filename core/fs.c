#include "fs.h"

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