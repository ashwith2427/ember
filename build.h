#ifndef BUILD_H_
#define BUILD_H_

#include <bits/types.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
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
    RESET_COLOR();                                                             \
    abort();

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
Strings
---------------------------------------
*/

char *string_stream(const char *fmt, ...);
char *get_extension(const char *path);

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
void merge_arrays(Array *dest, Array *src);

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

typedef struct DirectoryInfo {
    Array *sources;
    Array *headers;
} DirectoryInfo;

void create_directory(const char *name, const char *path);

// Deletes the directory or a file.
void delete_directory(const char *path);
/* If flag is REGULAR then it will only delete empty directories
without content. If you have the content you will encounter an error. The
   error is same as in linux
   **The folder is not empty**. So you need to recursively delete the files. */

// Gets the current working directory.
char *get_current_directory();

void change_directory(const char *path);
char *capture_end_file(const char *path);

void delete_file(const char *path);

DirectoryInfo *scan_directory(const char *path);
void free_directory_info(DirectoryInfo *info);

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
typedef struct Dependency{
    char *file;
    Array *files;
}Dependecy;
void __generate_dependencies(const char* folder);

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
    const char *name;
    const char *path;
    Array *sources;
    Array *headers;
    bool is_shared;
} Library;
Library *init_library(const char *name, const char *path, bool shared);
void free_library(Library *library);

typedef struct Executable {
    const char *name;
    const char *root_source_path;
    const char *output_dir;
    Optimization opts;
} Executable;

Executable *init_executable(const char *name, const char *root_source_path,
                            Optimization opts);
void free_executable(Executable *executable);

typedef struct Builder {
    Executable *exe;
    char *command;
    Array *libraries;
    Compiler compiler;
    Architecture arch;
    bool emit_assembly;
    bool emit_dependency_graph;
    void (*generate_library)(struct Builder *builder, Library *lib);
    void (*add_executable)(struct Builder *builder, Executable *exe);
    void (*install)(struct Builder *self);
    void (*link_library)(struct Builder *builder, Library *library);
    void (*add_system_library)(struct Builder *builder, const char *name);
    void (*add_source)(struct Builder *builder, const char* path);
} Builder;

Builder *init_builder();
void free_builder(Builder *builder);
void __generate_library__impl(Builder *self, Library *lib);
void __elink_libraries__impl(Builder *self, Library *library);
void __add_system_library_impl(Builder *self, const char* path);
void __add_source_impl(Builder *self, const char *path);
void __add_executable__impl(Builder *self, Executable *exe);
void __install__impl(Builder *self);

static void __generate_outputs(const char *name, const char *path) {
    char *o_path = string_stream("build/%s_libs", name);
    char *d_path = string_stream("build/%s_deps", name);
    create_directory(o_path, ".");
    create_directory(d_path, ".");
    DirectoryInfo *info = scan_directory(path);
    for (int i = 0; i < get_size(info->sources); i++) {
        char *file_name = *(char **)get_element(info->sources, i);
        char *endfile = capture_end_file(file_name);
        char *compile = string_stream("Compiling %s", file_name);
        PRINT_INFO(compile);
        char *command = string_stream("clang -MMD -MF %s/%s.d -c %s -o %s/%s.o", d_path, endfile, file_name,
                                      o_path, endfile);
        system(command);
        free(compile);
        free(command);
    }
    free(o_path);
    free(d_path);
    free_directory_info(info);
}

static void __trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

static Array *__parse_dependency_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        PRINT_ERROR("Failed to open .d file");
    }

    char line[1024];
    char dependencies[4096] = {0};
    int first_line = 1;

    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len > 0 && line[len - 1] == '\\') {
            line[len - 1] = ' ';
            strcat(dependencies, line);
            continue;
        }

        strcat(dependencies, line);
    }
    fclose(file);
    Array* files = init_array(sizeof(char*));

    char *token = strtok(dependencies, " :");
    while (token) {
        __trim_whitespace(token);
        push(files, token);
        token = strtok(NULL, " :");
    }
    return files;
}

/*
---------------------------------------
*/

#endif // BUILD_H_

#ifdef BUILD_IMPLEMENTATION
#define BUILD_IMPLEMENTATION

/*
---------------------------------------
Strings
---------------------------------------
*/
char *string_stream(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (size < 0)
        return NULL;
    char *buffer = malloc(size + 1);
    va_start(args, fmt);
    vsnprintf(buffer, size + 1, fmt, args);
    va_end(args);
    return buffer;
}

char *get_extension(const char *path) {
    char *extension;
    char *dot = strrchr(path, '.');
    if (dot && dot != path) {
        extension = dot + 1;
    }
    return extension;
}
/*
---------------------------------------
*/

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
        return; // Safety check
    if (array->element_size == sizeof(char *)) {
        for (int i = 0; i < array->size; i++) {
            free(((char **)array->data)[i]);
        }
    }
    free(array->data); // Free the array storage
    free(array);       // Free the array struct itself
}

void *get_element(Array *array, int i) {
    if (array == NULL || array->data == NULL || i >= array->size) {
        PRINT_ERROR("Invalid array or array data pointer.");
    }
    return (char *)array->data + (array->element_size * i);
}

void push(Array *array, void *element) {
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        void *new_data =
            realloc(array->data, array->capacity * array->element_size);
        if (!new_data) {
            PRINT_ERROR("Memory reallocation failed");
        }
        array->data = new_data;
    }
    if (array->element_size == sizeof(char *)) {
        char *copy = strdup((char *)element);
        memcpy((char **)array->data + array->size, &copy, sizeof(char *));
    } else {
        memcpy((char *)array->data + array->size * array->element_size, element,
               array->element_size);
    }

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

int get_size(Array *array) { return array->size; }

void print_array(Array *array, void (*print_fn)(void *)) {
    for (int i = 0; i < array->size; i++) {
        print_fn((char *)array->data + i * array->element_size);
    }
}

void merge_arrays(Array *dest, Array *src) {
    if (!dest || !src) return;

    if (src->element_size != dest->element_size) {
        PRINT_ERROR("Element size mismatch: Cannot merge arrays of different types");
        return;
    }

    if (src->size == 0) return;

    size_t new_capacity = dest->size + src->size;

    if (dest->capacity < new_capacity) {
        dest->capacity = new_capacity * 2;
        void *new_data = realloc(dest->data, dest->capacity * dest->element_size);
        dest->data = new_data;
    }

    if (dest->element_size == sizeof(char *)) {
        char **dest_data = (char **)dest->data;
        char **src_data = (char **)src->data;

        for (size_t i = 0; i < src->size; i++) {
            dest_data[dest->size + i] = strdup(src_data[i]); 
        }
    } else {
        memcpy((char *)dest->data + (dest->size * dest->element_size), 
               src->data, 
               src->size * src->element_size);
    }

    dest->size += src->size;
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

void create_directory(const char *name, const char *path) {
    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "%s/%s", path, name);
    if (mkdir(new_path, 0700) < 0 && errno == EEXIST) {
        return;
    }
}

char *get_current_directory() {
    char *buf = malloc(1024);
    getcwd(buf, 1024);
    return buf;
}

void change_directory(const char *path) {
    if (chdir(path) != 0) {
        switch (errno) {
        case ENOENT:
            PRINT_ERROR("No such file or directory.");
            break;
        default:
            PRINT_ERROR("Unable to change dir.");
            break;
        }
    }
}

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

char *capture_end_file(const char *path) {
    char *name;
    char *slash = strrchr(path, '/');
    if (slash && slash != path) {
        name = slash + 1;
    }
    return name;
}

void delete_file(const char *path) {
    if (remove(path) != 0) {
        switch (errno) {
        case EISDIR:
            PRINT_ERROR("Not a file. It is a directory.");
            break;
        case ENOENT:
            PRINT_ERROR("No such file or directory.");
            break;
        default:
            PRINT_ERROR("Unable to delete file.");
        }
    }
}

void print_strings(void *data) { printf("%s\n", *(char**)data); }

DirectoryInfo *scan_directory(const char *path) {
    DirectoryInfo *info = (DirectoryInfo *)malloc(sizeof(*info));
    info->headers = init_array(sizeof(char *));
    info->sources = init_array(sizeof(char *));
    struct dirent *entry;
    DIR *directory = opendir(path);
    if (directory == NULL) {
        switch (errno) {
        case ENOENT:
            PRINT_ERROR("No such file or directory.");
            break;
        case ENOTDIR:
            PRINT_ERROR("Not a directory.");
            break;
        default:
            PRINT_ERROR("Recheck the directory.");
        }
    }
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)
            continue;
        char *full_path = string_stream("%s/%s", path, entry->d_name);
        if (entry->d_type == DT_REG) {
            char *extension = get_extension(entry->d_name);
            if (extension && strcmp(extension, "h") == 0) {
                push(info->headers, full_path);
            }
            if (extension && strcmp(extension, "c") == 0) {
                push(info->sources, full_path);
            }
        } else if (entry->d_type == DT_DIR) {
            DirectoryInfo *sub_info = scan_directory(full_path);
            merge_arrays(info->headers, sub_info->headers);
            merge_arrays(info->sources, sub_info->sources);
            free_directory_info(sub_info);
        }
        free(full_path);
    }
    closedir(directory);
    return info;
}

void free_directory_info(DirectoryInfo *info) {
    free_array(info->headers);
    free_array(info->sources);
    free(info);
}

/*
---------------------------------------
*/

/*
---------------------------------------
Actual Build System
---------------------------------------
*/

Library *init_library(const char *name, const char *path, bool shared) {
    Library *lib = (Library *)malloc(sizeof(*lib));
    lib->is_shared = shared;
    lib->name = name;
    lib->path = path;
    DirectoryInfo *info = scan_directory(path);
    lib->headers = info->headers;
    lib->sources = info->sources;
    free(info);
    return lib;
}

void free_library(Library *library) {
    free_array(library->headers);
    free_array(library->sources);
    free(library);
}

void __generate_library__impl(Builder *self, Library *lib) {
    __generate_outputs(lib->name, lib->path);
    char *outputs_path = string_stream("build/%s_libs", lib->name);
    char *shared_lib_command =
        string_stream("clang -shared -o build/lib%s.so ", lib->name);
    struct dirent *entry;
    DIR *directory = opendir(outputs_path);
    if (directory == NULL) {
        if (errno == ENOENT) {
            PRINT_ERROR("No such directory.");
        } else {
            PRINT_ERROR("Directory cannot be opened.");
        }
    }

    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_REG) {
            char *full_path =
                string_stream("%s/%s", outputs_path, entry->d_name);
            char *extension = get_extension(full_path);
            size_t new_size =
                strlen(full_path) + strlen(shared_lib_command) + 2;
            shared_lib_command = realloc(shared_lib_command, new_size);
            if (extension && strcmp("o", extension) == 0) {
                strcat(shared_lib_command, full_path);
                strcat(shared_lib_command, " ");
            }
            free(full_path);
        }
    }
    system(shared_lib_command);
    free(outputs_path);
    free(shared_lib_command);
    closedir(directory);
}

Executable *init_executable(const char *name, const char *root_source_path,
                            Optimization opts) {
    Executable *executable = (Executable *)malloc(sizeof(*executable));
    executable->opts = opts;
    executable->root_source_path = root_source_path;
    executable->name = name;
    executable->output_dir = "build";
    return executable;
}

void free_executable(Executable *executable) { free(executable); }

Builder *init_builder() {
    create_directory("build", ".");
    Builder *builder = (Builder *)malloc(sizeof(*builder));
    builder->libraries = init_array(sizeof(Library *));
    builder->generate_library = __generate_library__impl;
    builder->link_library = __elink_libraries__impl;
    builder->add_system_library = __add_system_library_impl;
    builder->add_source = __add_source_impl;
    builder->add_executable = __add_executable__impl;
    builder->arch = X86;
    builder->compiler = CLANG;
    builder->emit_assembly = false;
    builder->emit_dependency_graph = false;
    builder->exe = NULL;
    builder->install = __install__impl;
    return builder;
}

void __add_system_library_impl(Builder *self, const char *name){
    char *lib_comm = string_stream(" -l%s ", name);
    int new_size = strlen(self->command) + strlen(lib_comm) + 2;
    self->command = realloc(self->command, new_size);
    strcat(self->command, lib_comm);
    free(lib_comm);
}

void __add_executable__impl(Builder *self, Executable *exe) {
    self->exe = exe;
    self->command = string_stream("clang -o build/%s %s ", exe->name,
                                  exe->root_source_path);
}

void __elink_libraries__impl(Builder *self, Library *library) {
    char *lib_path = string_stream(" build/lib%s.so ", library->name);
    int new_size = strlen(self->command) + strlen(lib_path) + 2;
    self->command = realloc(self->command, new_size);
    strcat(self->command, lib_path);
    free(lib_path);
}

void __install__impl(Builder *self) {
    create_directory("build", ".");
    system(self->command);
}

void free_builder(Builder *builder) {
    free(builder->command);
    free_array(builder->libraries);
    free(builder);
}

/*
---------------------------------------
*/
#endif