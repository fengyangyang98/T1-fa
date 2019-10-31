#ifndef _CSR_H_
#define _CSR_H_

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
typedef struct
{
    unsigned int vertex_number;
    unsigned int edge_number;
    unsigned int row_offsets[500];
    unsigned int col_indices[500];
    char values[500];
    
}csr;

void csr_del(csr * graph);

unsigned int csr_first_edge_index(csr graph, unsigned int vertex_number);
unsigned int csr_last_edge_index(csr graph, unsigned int vertex_number);
char csr_get_edge_value(csr graph, unsigned int edge_number);
unsigned csr_get_next_vertex(csr graph, unsigned edge_number);
bool csr_edge_search(csr graph, char ch, unsigned vertex_number, unsigned * edge_number);


#endif