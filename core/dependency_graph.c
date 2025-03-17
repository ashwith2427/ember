#include "dependency_graph.h"

bool check_dependency_equal(DependencyNode* n1, DependencyNode* n2){
    CXString n1_s = clang_getFileName(n1->file);
    CXString n2_s = clang_getFileName(n2->file);
    //fprintf(stdout, "%s %s\n", clang_getCString(n1_s), clang_getCString(n2_s));
    return strcmp(clang_getCString(n1_s), clang_getCString(n2_s)) == 0;
}
void expand_graph(DependencyGraph* graph){
    int prev_capacity = graph->capacity;
    graph->capacity *= 2;
    graph->nodes = (DependencyNode*)realloc(graph->nodes, graph->capacity * sizeof(DependencyNode));
    for(int i = prev_capacity; i<graph->capacity;i++){
        graph->nodes[i].file = NULL;
        graph->nodes[i].destinations = NULL;
    }
}

DependencyGraph* init_dependency_graph(){
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

DependencyNode* init_node(DependencyGraph* graph, CXFile file){
    DependencyNode* node = (DependencyNode*)malloc(sizeof(DependencyNode));
    node->capacity = graph->capacity;
    node->size = 0;
    node->destinations = (DependencyNode*)malloc(sizeof(DependencyNode)*node->capacity);
    node->file = file;
    return node;
}

bool add_node(DependencyGraph* graph, DependencyNode* node){
    if(graph->size >= graph->capacity){
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

bool add_edge(DependencyGraph* graph, DependencyNode* source, DependencyNode* destination){
    if(source->size >= source->capacity){
        int prev_capacity = source->capacity;
        source->capacity *= 2;
        source->destinations = (DependencyNode*)realloc(source->destinations, source->capacity*sizeof(DependencyNode));
        for(int i=prev_capacity;i<source->capacity;i++){
            source->destinations = NULL;
        }
    }
    for(int i=0;i<graph->size;i++){
        if(check_dependency_equal(source, &graph->nodes[i])){
            DependencyNode* node = &graph->nodes[i];
            node->destinations[node->size++] = *destination;
            return true;
        }
    }
    return false;
}


void visit(CXFile included_file, CXSourceLocation *inclusion_stack, unsigned include_len, CXClientData client_data){
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
        add_edge(graph, source, destination);
        free(source);
        free(destination);
    }
    inclusion_stack++;
}

void print_graph(DependencyGraph* graph){
    for(int i=0;i<graph->size;i++){
        DependencyNode* node = &graph->nodes[i];
        CXString node_name = clang_getFileName(node->file);
        fprintf(stdout, "%s:\n", clang_getCString(node_name));
        for(int j=0;j<node->size;j++){
            CXString child_name = clang_getFileName(node->destinations[j].file);
            fprintf(stdout, "\t%s\n", clang_getCString(child_name));
            clang_disposeString(child_name);
        }
        fprintf(stdout, "----------------------------------------\n");
        clang_disposeString(node_name);
    }
}