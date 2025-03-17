#ifndef ARRAY_H_
#define ARRAY_H_

#include <bits/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

typedef struct FileInfo{
    const char* file_name;
    const char* hash_code;
}FileInfo;

#define push_back(arr, element) do{ \
    __typeof__(element) temp = (element); \
    _Generic((element), \
        int: push(arr, &temp), \
        float: push(arr, &temp), \
        double: push(arr, &temp), \
        char*: push(arr, (void*)&temp), \
        default: push(arr, &temp) \
    ); \
} while(0)

typedef struct Array{
    __U32_TYPE capacity;
    __U32_TYPE size;
    int element_size;
    void* data;
}Array;

Array* init_array(int element_size);
void free_array(Array* array);
void push(Array* array, void* element);
void pop(Array* array);
Array* read_file_to_array(const char* file_name, int element_size);
void* get_element(Array* array, int i);
bool find(Array* array, void* element);
int get_size(Array* array);
void resize_array(Array* array, size_t size);
void print_array(Array* array, void (*print_fn)(void*));

#endif