#ifndef DEPENDENCY_GRAPH_H_
#define DEPENDENCY_GRAPH_H_

#include <bits/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __clang__
#include <clang-c/Index.h>
#include <clang-c/CXFile.h>
#include <clang-c/CXSourceLocation.h>
#endif

#include <stdbool.h>
typedef struct DependencyNode{
    __U32_TYPE size;
    __U32_TYPE capacity;
    CXFile file;
    struct DependencyNode* destinations;
    __attribute__((aligned(4))) bool dirty;
}DependencyNode;

typedef struct DependencyGraph{
    __U32_TYPE size;
    __U32_TYPE capacity;
    struct DependencyNode* nodes;
}DependencyGraph;

bool check_dependency_equal(DependencyNode* n1, DependencyNode* n2);
void expand_graph(DependencyGraph* graph);
DependencyGraph* init_dependency_graph();
DependencyNode* init_node(DependencyGraph* graph, CXFile file);
bool add_node(DependencyGraph* graph, DependencyNode* node);
bool add_edge(
    DependencyGraph* graph, 
    DependencyNode* source, 
    DependencyNode* destination
);
void visit(
    CXFile included_file, 
    CXSourceLocation *inclusion_stack, 
    unsigned include_len, 
    CXClientData client_data
);
void print_graph(DependencyGraph* graph);
char* get_sha256sum(const char* file_name);
void save_hash(DependencyGraph* graph);
FILE* generate_dependency_file(const char* path, const char* file_name);
#endif