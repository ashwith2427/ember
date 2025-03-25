#ifndef BUILD_H_
#define BUILD_H_

#include <bits/types.h>
#include <cstdint>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>
#endif

/*
---------------------------------------
Simple Logger
---------------------------------------
*/

// You need to understand ANSI escape code to understand this shit.
#define SET_RED() fprintf(stderr, "\033[1;31m")
#define SET_YELLOW() fprintf(stderr, "\x1b[1;33m")
#define RESET_COLOR() fprintf(stderr, "\033[0m")

// Prints error with red color. Only works in linux.
#define PRINT_ERROR(message)                                                   \
    SET_RED();                                                                 \
    fprintf(stderr, "[ERROR]: %s\n", message);                                 \
    RESET_COLOR();

#define PRINT_INFO(message)                                                    \
    SET_YELLOW();                                                              \
    fprintf(stderr, "[INFO]: %s\n", message);                                  \
    RESET_COLOR();

#define TODO(message) fprintf(stderr, "[TODO]: %s", message);

/*
---------------------------------------
*/

/*
---------------------------------------
Arrays
---------------------------------------
*/
#define push_back(arr, element)                                                \
    do {                                                                       \
        __typeof__(element) temp = (element);                                  \
        _Generic((element),                                                    \
            int: push(arr, &temp),                                             \
            float: push(arr, &temp),                                           \
            double: push(arr, &temp),                                          \
            char *: push(arr, (void *)&temp),                                  \
            default: push(arr, temp));                                         \
    } while (0)

typedef struct FileInfo {
    const char *file_name;
    const char *hash_code;
} FileInfo;

typedef struct Array {
    uint32_t capacity;
    uint32_t size;
    int element_size;
    void *data;
} Array;

Array *init_array(int element_size);
void free_array(Array *array);
void push(Array *array, void *element);
void pop(Array *array);
Array *read_file_to_array(const char *file_name, int element_size);
void *get_element(Array *array, int i);
bool find(Array *array, void *element);
int get_size(Array *array);
void resize_array(Array *array, size_t size);
void print_array(Array *array, void (*print_fn)(void *));

/*
---------------------------------------
*/

/*
---------------------------------------
Hash Table
---------------------------------------
*/

typedef struct map_entry {
    void *key;
    void *value;
    struct map_entry *__next;
} map_entry;

static unsigned long __hash_function(const void *key, size_t key_size) {
    unsigned long hash = 5381;
    const unsigned char *ptr = (const unsigned char *)key;

    for (size_t i = 0; i < key_size; i++)
        hash = ((hash << 5) + hash) + ptr[i];

    return hash;
}

typedef struct hash_map {
    uint32_t key_size;
    uint32_t value_size;
    uint32_t __buckets;
    uint32_t __count;
    map_entry **__heads;
    void (*put)(struct hash_map *self, void *key, size_t key_size, void *value,
                size_t value_size);
    void *(*get)(struct hash_map *self, void *key, size_t key_size);
    uint32_t (*size)(struct hash_map *self);
    void (*dump)(struct hash_map *self, void (*print_fn)(void *element));
    unsigned long (*hash)(const void *key, size_t key_size);
    struct map_iterator *(*iter)(struct hash_map *self);
} hash_map;

typedef struct map_iterator {
    uint32_t __bucket;
    hash_map *__map;
    map_entry *__current;
    map_entry *(*next)(struct map_iterator *self);
    void (*del)(struct map_iterator *self);
} map_iterator;

map_iterator *__hash_map_iter(hash_map *self);
map_entry *__iter_next(map_iterator *self);
void __iter_del(map_iterator *self);

hash_map *hash_map_new(uint32_t key_size, uint32_t value_size);
void hash_map_free(hash_map *map);
void __map_put(hash_map *self, void *key, size_t key_size, void *value,
               size_t value_size);
void *__map_get(hash_map *self, void *key, size_t key_size);
uint32_t __map_size(hash_map *self);
void __map_dump(hash_map *self, void (*print_fn)(void *element));
void __resize_and_rehash(hash_map *self);

#define MAP_PUT(map, key, value)                                               \
    do {                                                                       \
        __typeof__(key) k = (key);                                             \
        __typeof__(value) v = (value);                                         \
        _Generic((key),                                                        \
            int: _Generic((value),                                             \
                int: __map_put(map, &k, sizeof(k), &v, sizeof(v)),             \
                float: __map_put(map, &k, sizeof(k), &v, sizeof(v)),           \
                double: __map_put(map, &k, sizeof(k), &v, sizeof(v)),          \
                default: __map_put(map, &k, sizeof(k), &v, sizeof(v))),        \
            float: _Generic((value),                                           \
                int: __map_put(map, &k, sizeof(k), &v, sizeof(v)),             \
                float: __map_put(map, &k, sizeof(k), &v, sizeof(v)),           \
                double: __map_put(map, &k, sizeof(k), &v, sizeof(v)),          \
                default: __map_put(map, &k, sizeof(k), &v, sizeof(v))),        \
            double: _Generic((value),                                          \
                int: __map_put(map, &k, sizeof(k), &v, sizeof(v)),             \
                float: __map_put(map, &k, sizeof(k), &v, sizeof(v)),           \
                double: __map_put(map, &k, sizeof(k), &v, sizeof(v)),          \
                default: __map_put(map, &k, sizeof(k), &v, sizeof(v))),        \
            default: _Generic((value),                                         \
                int: __map_put(map, &k, sizeof(k), &v, sizeof(v)),             \
                float: __map_put(map, &k, sizeof(k), &v, sizeof(v)),           \
                double: __map_put(map, &k, sizeof(k), &v, sizeof(v)),          \
                default: __map_put(map, &k, sizeof(k), &v, sizeof(v))));       \
    } while (0)

/*
---------------------------------------
*/

/*
---------------------------------------
File Handling
---------------------------------------
*/

typedef enum { FORCE = 0, REGULAR = 1 } RmFlag;

bool create_directory(const char *path);

// Deletes the directory or a file.
bool delete_directory(const char *path, RmFlag flag);
/* If flag is REGULAR then it will only delete empty directories
without content. If you have the content you will encounter an error. The
   error is same as in linux
   **The folder is not empty**. So you need to recursively delete the files. */

// Gets the current working directory.
char *get_current_directory();

bool delete_file(const char *path, const char *file_name);
bool find_file(const char *path, const char *file_name);

/*
---------------------------------------
*/

/*
---------------------------------------
File Utils
---------------------------------------
*/

char *read_line(FILE *file);
void *read_struct(FILE *file, size_t struct_size);
char read_byte(FILE *file);

/*
---------------------------------------
*/

/*
---------------------------------------
Actual Build System
---------------------------------------
*/

/*
    Defines which optimizations to apply. You can choose multiple optimizations
   available using OR; OPTIMIZATION_MEMORY | OPTIMIZATION_SPEED => Will consider
   both mem and speed of the build.
*/
typedef enum Optimization {
    OPTIMIZATION_NONE = 0 << 0,
    OPTIMIZATION_MEMORY = 1 << 0,
    OPTIMIZATION_SPEED = 1 << 1,
    OPTIMIZATION_POWER = 1 << 2,
    OPTIMIZATION_SECURITY = 1 << 3
} Optimization;

typedef enum Compiler { CLANG, GCC } Compiler;

typedef enum Architecture {
    X86,
    ARM64,
    ARM32,
    RISCV,
} Architecture;

typedef struct Library {
    char *name;
    char *path;
    Array *sources;
    Array *headers;
    bool is_shared;
    void (*link_library)(struct Library *library);
} Library;
Library *init_library__impl(const char *name, const char *path, bool is_shared);
void __llink_library__impl(struct Library *library);

typedef struct Executable {
    char *name;
    char *root_source_path;
    const char *version;
    const char *output_dir;
    Optimization opts;
    void (*link_libraries)(Executable *exe, Library *library);
} Executable;

Executable *init_executable(const char *name, const char *root_source_path,
                            Optimization opts);
void __elink_libraries__impl(Executable *exe, Library *library);

typedef struct Builder {
    Executable *exe;
    const char *output_dir;
    Array *libraries;
    Compiler compiler;
    Architecture arch;
    bool emit_assembly;
    bool emit_dependency_graph;
    Library *(*generate_library)(const char *name, const char *path);
    void (*add_executable)(Executable *exe);
    void (*install)(Builder *self);
    void (*run)();
} Builder;

Builder *init_builder();
Library *__generate_library__impl(const char *name, const char *path);
void __add_executable__impl(Executable *exe);
void __install__impl(Builder *self);
void __run__impl();

/*
---------------------------------------
*/

/*
---------------------------------------
Compilation
---------------------------------------
*/

typedef struct {
    Array *sources;
    Array *headers;
} DirectoryInfo;

DirectoryInfo *init_directory_info();
DirectoryInfo *scan_directory();
void generate_dependencies(const char *path);

#endif // BUILD_H_

#ifdef BUILD_IMPLEMENTATION
#define BUILD_IMPLEMENTATION

/*
---------------------------------------
Arrays
---------------------------------------
*/

Array *init_array(int element_size) {
    Array *array = (Array *)malloc(sizeof(Array));
    array->capacity = 10;
    array->size = 0;
    array->element_size = element_size;
    array->data = malloc(element_size * array->capacity);
    return array;
}

void free_array(Array *array) {
    if (!array)
        return;        // Safety check
    free(array->data); // Free the array storage
    free(array);       // Free the array struct itself
}

void *get_element(Array *array, int i) {
    if (array == NULL || array->data == NULL || i >= array->size) {
        PRINT_ERROR("Invalid array or array data pointer.");
        abort();
    }
    return (char *)array->data + (array->element_size * i);
}

int get_size(Array *array) { return array->size; }

void push(Array *array, void *element) {
    if (array->size >= array->capacity) {
        int previous_cap = array->capacity;
        array->capacity *= 2;
        array->data =
            realloc(array->data, array->capacity * array->element_size);
    }
    memcpy((char *)array->data + array->size * array->element_size, element,
           array->element_size);
    array->size++;
}

void pop(Array *array) {
    memset((char *)array->data + array->element_size * (array->size - 1), 0,
           array->element_size);
    array->size--;
}

bool find(Array *array, void *element) {
    for (int i = 0; i < array->size; i++) {
        if (memcmp((char *)array->data + array->element_size * i, element,
                   array->element_size) == 0) {
            return true;
        }
    }
    return false;
}

void resize_array(Array *array, size_t size) {
    array->data = realloc(array->data, size * array->element_size);
    array->capacity = size;
}

Array *read_file_to_array(const char *file_name, int element_size) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        PRINT_ERROR("Error reading from the file or opening the file...");
        return NULL;
    }

    // Read the size of the file
    size_t size;
    fread(&size, sizeof(size_t), 1, file);

    // Allocate memory for the file information array
    Array *file_infos = init_array(sizeof(FileInfo));

    // Read each FileInfo and populate the file_infos array
    for (size_t i = 0; i < size; i++) {
        FileInfo *info = (FileInfo *)malloc(sizeof(FileInfo));
        fread(info, sizeof(FileInfo), 1, file);
        push_back(file_infos, info);
    }

    // Rebuild the DependencyGraph from the file_infos
    for (size_t i = 0; i < size; i++) {
        FileInfo *info = (FileInfo *)get_element(file_infos, i);

        fprintf(stdout, "%s\t%s", info->file_name, info->hash_code);
    }
    fclose(file);
    return file_infos;
}

void print_array(Array *array, void (*print_fn)(void *)) {
    for (int i = 0; i < array->size; i++) {
        print_fn((char *)array->data + i * array->element_size);
    }
}

/*
---------------------------------------
*/

/*
---------------------------------------
Hash Table
---------------------------------------
*/

hash_map *hash_map_new(uint32_t key_size, uint32_t value_size) {
    hash_map *map = (hash_map *)malloc(sizeof(*map));
    map->key_size = key_size;
    map->value_size = value_size;
    map->__buckets = 16;
    map->__heads = (map_entry **)calloc(16, sizeof(map_entry *));
    for (int i = 0; i < map->__buckets; i++) {
        map->__heads[i] = NULL;
    }
    map->__count = 0;
    map->put = __map_put;
    map->get = __map_get;
    map->dump = __map_dump;
    map->size = __map_size;
    map->hash = __hash_function;
    map->iter = __hash_map_iter;
    return map;
}

void hash_map_free(hash_map *self) {
    for (int i = 0; i < self->__buckets; i++) {
        map_entry *entry = self->__heads[i];
        while (entry) {
            map_entry *next = entry->__next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(self->__heads);
    free(self);
}

uint32_t __map_size(hash_map *self) { return self->__count; }
void __map_dump(hash_map *self, void (*print_fn)(void *element)) {
    for (int i = 0; i < self->__buckets; i++) {
        if (self->__heads[i]) {
            map_entry *entry = self->__heads[i];
            map_entry *temp;
            for (temp = entry; temp != NULL; temp = temp->__next) {
                print_fn(temp);
            }
        }
    }
}
void __map_put(hash_map *self, void *key, size_t key_size, void *value,
               size_t value_size) {

    if (key_size != self->key_size) {
        printf("[Error]: Key type mismatch.\n");
        abort();
    }
    if (value_size != self->value_size) {
        printf("[Error]: Value type mismatch.\n");
        abort();
    }
    double load_factor = (double)self->__count / (double)self->__buckets;
    if (load_factor >= 0.5) {
        __resize_and_rehash(self);
    }
    unsigned long hash = self->hash(key, key_size) % self->__buckets;
    map_entry *entry = (map_entry *)malloc(sizeof(*entry));
    entry->key = malloc(key_size);
    memcpy(entry->key, key, key_size);
    entry->value = malloc(value_size);
    memcpy(entry->value, value, value_size);
    if (self->__heads[hash]) {
        map_entry *temp;
        for (temp = self->__heads[hash]; temp != NULL; temp = temp->__next) {
            if (memcmp(temp->key, key, key_size) == 0) {
                temp->value = value;
                return;
            }
        }
    }
    entry->__next = self->__heads[hash];
    self->__heads[hash] = entry;
    self->__count++;
}

void *__map_get(hash_map *self, void *key, size_t key_size) {
    unsigned long hash = self->hash(key, key_size) % self->__buckets;
    map_entry *entry = self->__heads[hash];
    map_entry *temp;
    for (temp = entry; temp != NULL; temp = temp->__next) {
        if (memcmp(key, entry->key, key_size) == 0) {
            return temp->value;
        }
    }
    return NULL;
}

void __resize_and_rehash(hash_map *self) {
    map_entry **old_heads = self->__heads;
    int old_size = self->__buckets;

    self->__buckets *= 2;
    self->__heads = calloc(self->__buckets, sizeof(map_entry *));

    for (int i = 0; i < old_size; i++) {
        map_entry *entry = old_heads[i];
        while (entry) {
            map_entry *next = entry->__next;
            int new_index =
                self->hash(entry->key, self->key_size) % self->__buckets;
            entry->__next = self->__heads[new_index];
            self->__heads[new_index] = entry;
            entry = next;
        }
    }

    free(old_heads);
}

map_iterator *__hash_map_iter(hash_map *self) {
    map_iterator *iterator = (map_iterator *)malloc(sizeof(*iterator));
    iterator->__map = self;
    iterator->__bucket = 0;
    iterator->__current = self->__heads[iterator->__bucket];
    iterator->next = &__iter_next;
    iterator->del = &__iter_del;
    return iterator;
}

map_entry *__iter_next(map_iterator *self) {
    map_entry *retval;
    while (self->__current == NULL) {
        self->__bucket++;
        if (self->__bucket >= self->__map->__buckets)
            return NULL;
        self->__current = self->__map->__heads[self->__bucket];
    }

    retval = self->__current;
    if (self->__current != NULL)
        self->__current = self->__current->__next;
    return retval;
}

void __iter_del(map_iterator *self) {
    free(self->__current);
    free(self);
}

/*
---------------------------------------
*/

/*
---------------------------------------
File Handling
---------------------------------------
*/

/*
---------------------------------------
*/
#endif