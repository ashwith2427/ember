#include "array.h"

Array* init_array(int element_size){
    Array* array = (Array*)malloc(sizeof(Array));
    array->capacity = 10;
    array->size = 0;
    array->element_size = element_size;
    array->data = malloc(element_size * array->capacity);
    return array;
}

void free_array(Array* array) {
    if (!array) return;  // Safety check
    free(array->data);  // Free the array storage
    free(array);        // Free the array struct itself
}


void* get_element(Array* array, int i) {
    if (array == NULL || array->data == NULL) {
        PRINT_ERROR("Invalid array or array data pointer.");
        return NULL;
    }
    PRINT_ERROR("Invalid array or array data pointer.");
    return (char*)array->data + (array->element_size * i);
}


int get_size(Array* array){
    return array->size;
}

void push(Array* array, void* element){
    if(array->size >= array->capacity){
        int previous_cap = array->capacity;
        array->capacity *= 2;
        array->data = realloc(array->data, array->capacity* array->element_size);
    }
    memcpy((char*)array->data + array->size*array->element_size, element, array->element_size);
    array->size++;
}

void pop(Array* array){
    memset((char*)array->data + array->element_size*(array->size - 1), 0, array->element_size);
    array->size--;
}

bool find(Array* array, void* element){
    for(int i=0;i<array->size;i++){
        if(memcmp((char*)array->data + array->element_size*i, element, array->element_size) == 0){
            return true;
        }
    }
    return false;
}

void resize_array(Array* array, size_t size){
    array->data = realloc(array->data, size*array->element_size);
    array->capacity = size;
}

Array* read_file_to_array(const char* file_name, int element_size) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        PRINT_ERROR("Error reading from the file or opening the file...");
        return NULL;
    }

    // Read the size of the file
    size_t size;
    fread(&size, sizeof(size_t), 1, file);

    // Allocate memory for the file information array
    Array* file_infos = init_array(sizeof(FileInfo));

    // Read each FileInfo and populate the file_infos array
    for (size_t i = 0; i < size; i++) {
        FileInfo* info = (FileInfo*)malloc(sizeof(FileInfo));
        fread(info, sizeof(FileInfo), 1, file);
        push_back(file_infos, info);
    }

    // Rebuild the DependencyGraph from the file_infos
    for (size_t i = 0; i < size; i++) {
        FileInfo* info = (FileInfo*)get_element(file_infos, i);
        
        fprintf(stdout, "%s\t%s", info->file_name, info->hash_code);
    }
    fclose(file);
    return file_infos;
}


void print_array(Array* array, void (*print_fn)(void*)){
    for(int i=0;i<array->size;i++){
        print_fn((char*)array->data + i*array->element_size);
    }
    fprintf(stdout, "\n");
}