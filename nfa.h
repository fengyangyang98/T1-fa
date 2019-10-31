#ifndef NFA_H
#define NFA_H
#define U_MAX 1 << 32 - 1


#include "csr.h"
#include "util.h"
// #include "set.h"
#include <stdbool.h>

#define MAXSTATE 63
#define MAXCHAR 63

#define MAXSTATESET 100
#define MAXEDGE 100
#define MAXSTACK 100
#define MAXSETS 100

/* the state which is at the border of the sub set */
typedef struct 
{
    unsigned state;
    char condition;
    unsigned next;
    unsigned edge_index;
} stop_state;

/* the states contained in the sub sets*/
typedef struct 
{
    unsigned states[MAXSTATE]; // contante states
    stop_state stop[MAXSTATE]; // stop states
    bool is_acc, is_init;   // jugde
    bool using;
    unsigned state_numer, stop_state_number;    // number flag
} new_state;

/* the edge among sub states */
typedef struct
{
    unsigned src;
    unsigned dest;
    char condition;
    bool using;
}move_set;


/* sub states sets */
typedef struct
{
  new_state sub[MAXSTATE];    // subs
  move_set sub_edge[MAXEDGE]; // edges
  // unsigned acc_t[MAXSTATE];

  unsigned sub_index;
  unsigned edge_index;
} get_sub;

/* graph stored with csr */
typedef struct
{
    char char_t[250];
    
    unsigned state_t[500];
    char init;
    unsigned acc_t[250];
    csr move_t;

    unsigned char_num;
    unsigned state_num;
    unsigned acc_num;
}graph_csr;

/* graph stored without csr */
typedef struct
{
  char char_t[100];
  unsigned state_t[100];
  char init;
  unsigned acc_t[100];

  move_set move_t[100];

  unsigned char_num;
  unsigned state_num;
  unsigned acc_num;
  unsigned move_num;
}graph_no_csr;

/* graph stored with 2-d arry map */
typedef struct
{
  int **transition_map;
  int **partition_map;
  int init;
  long int reachable;
  long int all;
  long int acc;
  long int non_acc;
  long int *partition;

  unsigned partion_index;
} graph_map;

/* string */
typedef struct 
{
  char s[10];
} string;


void read_pre_nfa_file(char * filename, graph_no_csr * D);
void read_pre_dfa_file(char * filename, graph_no_csr * D);
void read_dfa_file(char * filename, graph_csr * D);

bool write_std_dfa(graph_csr * D, char * filename);
bool get_nfa(graph_no_csr * D, graph_csr * std_D);

bool dfa_dfs(graph_csr D, unsigned depth);
bool dfa_judge(char * str, graph_csr D, unsigned * end_state);
bool check_pre_dfa(graph_no_csr * D, graph_csr * std_D);

void nfa_to_dfa(graph_csr *D, graph_csr * DFA);
void dfa_to_min(graph_csr *D, graph_csr *M);
bool nfa_dfs(graph_csr D, unsigned depth);

void print_sub(get_sub subs);

int move_set_comp_dest(const void * a,const void * b);
int move_set_comp(const void * a,const void * b);
int unsigned_comp(const void * a,const void * b);
bool num_in_arry(unsigned * arry, unsigned len, unsigned num, unsigned * in);
bool string_in_arry(string * arry, unsigned len, string str, unsigned *in);
#endif