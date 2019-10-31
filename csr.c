#include "csr.h"


unsigned int csr_first_edge_index(csr graph, unsigned int vertex_number)
{
    return graph.row_offsets[vertex_number];
}

// TODO: not the real last
unsigned int csr_last_edge_index(csr graph, unsigned int vertex_number)
{
    return graph.row_offsets[vertex_number + 1];
}

char csr_get_edge_value(csr graph, unsigned int edge_number)
{
    return graph.values[edge_number];
}

unsigned csr_get_next_vertex(csr graph, unsigned edge_number)
{
    return graph.col_indices[edge_number];
}
bool csr_edge_search(csr graph, char ch, unsigned vertex_number, unsigned * edge_number)
{
    unsigned head = csr_first_edge_index(graph, vertex_number);
    unsigned tail = csr_last_edge_index(graph, vertex_number);

    unsigned index = 0;
    for(index = head; index <= tail; index++)
    {
        if(graph.values[index] == ch)
        {
            *edge_number = index;
            return true;
        }
    }
    return false;
}
