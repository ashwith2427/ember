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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __clang__
    #include <clang-c/CXFile.h>
    #include <clang-c/CXString.h>
    #include <clang-c/CXSourceLocation.h>
    #include <clang-c/Index.h>
#endif

typedef enum{
    FORCE = 0,
    REGULAR = 1
}RmFlag;

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

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include <dirent.h>
#endif

typedef struct DependencyNode{
    __U32_TYPE size;
    __U32_TYPE capacity;
    CXFile file;
    struct DependencyNode* destinations;
}DependencyNode;

typedef struct DependencyGraph{
    __U32_TYPE size;
    __U32_TYPE capacity;
    struct DependencyNode* nodes;
}DependencyGraph;

// Creates a directory.
bool create_directory(const char* path);

// Deletes the directory or a file. 
bool delete_directory(const char* path, RmFlag flag);
/* If flag is REGULAR then it will only delete empty directories
   without content. If you have the content you will encounter an error. The error is same as in linux
   **The folder is not empty**. So you need to recursively delete the files. */

// Gets the current working directory.
char* get_current_directory(const char* path);

static DependencyNode* init_node(DependencyGraph* graph, CXFile file);
static DependencyGraph* init_dependency_graph();
static bool add_node(DependencyGraph* graph, DependencyNode* node);
static bool add_edge(DependencyNode* source, DependencyNode* destination);
// Use "." if you want to know where you are in. It is same as the cd command in linux.

// Generates the dependency graph.
// FILE* generate_dependency_file(const char* path, const char* file_name);
static void visit(CXFile included_file, CXSourceLocation *inclusion_stack, unsigned include_len, CXClientData client_data){
    if(include_len > 0 && !clang_Location_isInSystemHeader(*inclusion_stack)){
        CXString file_name = clang_getFileName(included_file);
        // fprintf(stdout, "%s: \n", clang_getCString(file_name));
        CXFile inclusion_file;
        unsigned line, column;
        // Extract the filename, line, and column where the inclusion occurred
        clang_getSpellingLocation(*inclusion_stack, &inclusion_file, &line, &column, NULL);
        DependencyGraph* graph = (DependencyGraph*)(client_data);
        DependencyNode* source = init_node(graph, included_file);
        add_node(graph, source);
        DependencyNode* destination = init_node(graph, inclusion_file);
        add_node(graph, destination);
        CXString source_name = clang_getFileName(source->file);
        CXString destination_name = clang_getFileName(destination->file);
        fprintf(stdout, "Source: %s\tDestination: %s\n", clang_getCString(source_name), clang_getCString(destination_name));
        add_edge(source, destination);
        fprintf(stdout, "Graph Size: %d\n", graph->size);
        fprintf(stdout, "Graph Capacity: %d\n", graph->capacity);
        // Get the name of the file that included the target file
        CXString inclusion_file_name = clang_getFileName(inclusion_file);
        const char* inclusion_cstr = clang_getCString(inclusion_file_name);
        // fprintf(stdout, "%s\n", inclusion_cstr);
        clang_disposeString(inclusion_file_name);
    }
    inclusion_stack++;
}

// FILE* generate_dependency_file(const char* path, const char* file_name);
// FILE* generate_dependency_file(const char* path, const char* file_name){
//     CXIndex index = clang_createIndex(0, 0);
//     CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, file_name, NULL, 0, NULL, 0, CXTranslationUnit_None);

//     if(translationUnit == NULL){
//         PRINT_ERROR("Cannot parse file");
//         return NULL;
//     }
//     CXCursor cursor = clang_getTranslationUnitCursor(translationUnit);

//     clang_getInclusions(translationUnit, visit, NULL);
    
//     return NULL;
// }

static bool check_dependency_equal(DependencyNode* n1, DependencyNode* n2){
    CXString n1_s = clang_getFileName(n1->file);
    CXString n2_s = clang_getFileName(n2->file);
    //fprintf(stdout, "%s %s\n", clang_getCString(n1_s), clang_getCString(n2_s));
    return strcmp(clang_getCString(n1_s), clang_getCString(n2_s)) == 0;
}

static void expand_graph(DependencyGraph* graph){
    int prev_capacity = graph->capacity;
    graph->capacity *= 2;
    graph->nodes = (DependencyNode*)realloc(graph->nodes, graph->capacity * sizeof(DependencyNode));
    for(int i = prev_capacity; i<graph->capacity;i++){
        graph->nodes[i].file = NULL;
        graph->nodes[i].destinations = NULL;
    }
}

static bool add_edge(DependencyNode* source, DependencyNode* destination){
    if(source->size >= source->capacity){
        PRINT_INFO("Expanding Node array...");
        int prev_capacity = source->capacity;
        source->capacity *= 2;
        source->destinations = (DependencyNode*)realloc(source->destinations, source->capacity*sizeof(DependencyNode));
        for(int i=prev_capacity;i<source->capacity;i++){
            source->destinations = NULL;
        }
    }
    for(int i=0;i<source->size;i++){
        if(check_dependency_equal(&source->destinations[i], destination)){
            PRINT_INFO("Equal");
            return false;
        }
    }
    source->destinations[source->size++] = *destination;
    return true;
}

static bool add_node(DependencyGraph* graph, DependencyNode* node){
    if(graph->size >= graph->capacity){
        PRINT_INFO("Expanding Graph");
        expand_graph(graph);
    }
    for(int i=0;i<graph->size;i++){
        if(check_dependency_equal(&graph->nodes[i], node)){
            return false;
        }
    }
    graph->nodes[graph->size++] = *node;
    return true;
}

static DependencyNode* init_node(DependencyGraph* graph, CXFile file){
    DependencyNode* node = (DependencyNode*)malloc(sizeof(DependencyNode));
    node->capacity = graph->capacity;
    node->size = 0;
    node->destinations = (DependencyNode*)malloc(sizeof(DependencyNode)*node->capacity);
    node->file = file;
    return node;
}

static DependencyGraph* init_dependency_graph(){
    DependencyGraph* graph = (DependencyGraph*)malloc(sizeof(DependencyGraph));
    graph->capacity = 10;
    graph->size = 0;
    graph->nodes = (DependencyNode*)malloc(graph->capacity*sizeof(DependencyNode));
    for(int i=0;i<graph->capacity;i++){
        graph->nodes[i].file = NULL;
        graph->nodes[i].capacity = graph->capacity;
        graph->nodes[i].size = 0;
        graph->nodes[i].destinations = NULL;
    }
    return graph;
}

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

FILE* generate_dependency_file(const char* path, const char* file_name){
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, file_name, NULL, 0, NULL, 0, CXTranslationUnit_None);

    if(translationUnit == NULL){
        PRINT_ERROR("Cannot parse file");
        return NULL;
    }
    CXCursor cursor = clang_getTranslationUnitCursor(translationUnit);
    DependencyGraph* graph = init_dependency_graph();

    clang_getInclusions(translationUnit, visit, graph);
    // DependencyGraph* graph = init_dependency_graph();
    // DependencyNode* node = init_node(graph, "hello");
    // add_node(graph, node);
    // assert(&graph->nodes[0] != NULL);
    // fprintf(stdout, "Graph Size: %d\n", graph->size);
    // fprintf(stdout, "Graph Capacity: %d\n", graph->capacity);
    // DependencyNode* node1 = init_node(graph, "hallo");
    // fprintf(stdout, "%s\n", clang_getCString(clang_getFileName(node1->file)));
    // assert(node1 != NULL);
    // add_node(graph, node1);
    // fprintf(stdout, "Graph Size: %d\n", graph->size);
    // fprintf(stdout, "Graph Capacity: %d\n", graph->capacity);

    return NULL;
}
#endif