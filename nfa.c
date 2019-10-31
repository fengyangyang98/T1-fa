#include "nfa.h"

static bool init_have;
static unsigned no_use;

int bitcount(unsigned x)
{
  int b;
  for (b = 0; x != 0; x >>= (1))
    if (x & 1)
      ++b;
  return b;
}

/* compasion function for the quick sorting. 
 *
 * move_set_comp:       sort move_set by src.
 * move_set_comp_dest:  sort move_set by dest
 * unsigned_comp:       sort unsigned arry.
 * 
*/
int move_set_comp(const void *a, const void *b)
{
  move_set aa = *(move_set *)a;
  move_set bb = *(move_set *)b;
  return aa.src - bb.src;
}

int unsigned_comp(const void *a, const void *b)
{
  return *(unsigned *)a - *(unsigned *)b;
}

int move_set_comp_dest(const void *a, const void *b)
{
  move_set aa = *(move_set *)a;
  move_set bb = *(move_set *)b;
  return aa.dest - bb.dest;
}

int stop_state_comp(const void *a, const void *b)
{
  stop_state aa = *(stop_state *)a;
  stop_state bb = *(stop_state *)b;
  return aa.condition - bb.condition;
}

/* checking elements in arry
 *
 * num_in_arry:   find a number in a number arry.
 * char_in_arry:  find a char in a char arry.
 * 
*/
bool num_in_arry(unsigned *arry, unsigned len, unsigned num, unsigned *in)
{
  unsigned x = 0;
  for (x = 0; x < len; x++)
  {
    if (arry[x] == num)
    {
      *in = x;
      return true;
    }
  }
  return false;
}

bool char_in_arry(char *arry, unsigned len, unsigned num, unsigned *in)
{
  unsigned x = 0;
  for (x = 0; x < len; x++)
  {
    if (arry[x] == num)
    {
      *in = x;
      return true;
    }
  }
  return false;
}

bool string_in_arry(string * arry, unsigned len, string str, unsigned *in){
  unsigned x = 0;
  for(x = 0; x < len; x++){
    if(strcmp(arry[x].s, str.s) == 0)
    {
      *in = x;
      return true;
    }
  }
  return false;
}

/* find the state in sub state sets */
bool states_set_same(unsigned *a, unsigned *b, unsigned lena, unsigned lenb)
{
  if (lena != lenb)
    return false;

  qsort(a, lena, sizeof(unsigned), unsigned_comp);
  qsort(b, lenb, sizeof(unsigned), unsigned_comp);

  unsigned index;
  for (index = 0; index < lena; index++)
  {
    if (a[index] != b[index])
      return false;
  }
  return true;
}

int find_new_state_index(new_state finding, get_sub subs)
{
  unsigned index = 0;
  for (index = 0; index < subs.sub_index; index++)
  {
    if (states_set_same(subs.sub[index].states, finding.states, subs.sub[index].state_numer, finding.state_numer))
      return index;
  }
  return -1;
}

/* graph heper */

/* get_sub to graph_csr */
bool get_std_graph_csr(get_sub *s, graph_csr *D)
{
  qsort(s->sub_edge, s->edge_index, sizeof(move_set), move_set_comp);
  D->char_num = D->state_num = D->acc_num = 0;

  unsigned index = 0;

  // char table
  for (index = 0; index < s->edge_index; index++)
  {
    // ??
    D->char_t[D->char_num] = '\0';
    if (!char_in_string(D->char_t, s->sub_edge[index].condition))
    {
      D->char_t[D->char_num++] = s->sub_edge[index].condition;
      D->char_t[D->char_num] = '\0';
    }
  }

  // state table
  for (index = 0; index < s->sub_index; index++)
  {
    D->state_t[D->state_num++] = index;
  }

  // csr
  D->move_t.edge_number = s->edge_index;
  D->move_t.vertex_number = s->sub_index;

  for (index = 0; index < s->edge_index; index++)
  {
    D->move_t.col_indices[index] = s->sub_edge[index].dest;
    D->move_t.values[index] = s->sub_edge[index].condition;
  }

  unsigned edge_index = 0;
  for (index = 0; index < s->sub_index; index++)
  {
    if (s->sub[index].is_acc)
      D->acc_t[D->acc_num++] = index;
    if (s->sub[index].is_init)
      D->init = index;
    D->move_t.row_offsets[index] = edge_index;

    if (s->sub_edge[edge_index].src != index)
    {
      continue;
    }

    unsigned prev = s->sub_edge[edge_index].src;
    while (true)
    {
      if (prev != s->sub_edge[edge_index].src)
      {
        break;
      }
      edge_index++;
    }
  }
  D->move_t.row_offsets[index] = edge_index;
}

/* read .nfa file */
void read_pre_nfa_file(char *filename, graph_no_csr *D)
{
  FILE *dfa_src = fopen(filename, "r");

  D->char_num = D->state_num = D->acc_num = D->move_num = 0;
  unsigned src, dest;
  char condition;
  unsigned lineno = 0;
  init_have = false;
  while (!feof(dfa_src))
  {
    int not_emtpy = fscanf(dfa_src, "%d,%c,%d", &src, &condition, &dest);
    lineno++;
    fgetc(dfa_src);
    if (not_emtpy == 0)
      continue;

    // printf( "%d,%c,%d\n", src, condition,dest);

    // inital
    if (condition == ' ')
    {
      if (src != dest)
      {
        printf("Error line %d: the src(%c) and the dest(%c) number must be the same when you enter the init state.\n", lineno, src, dest);
        break;
      }
      D->init = src;
      init_have = true;
    }

    // accept
    else if (condition == '%')
    {
      if (src != dest)
      {
        printf("Error line %d: the src(%d) and the dest(%d) number must be the same when you enter the acceptabel state.\n", lineno, src, dest);
        break;
      }
      if (!num_in_arry(D->acc_t, D->acc_num, src, &no_use))
      {
        D->acc_t[D->acc_num] = src;
        D->acc_num++;
      }
    }

    // edge
    else
    {
      if (!char_in_string(D->char_t, condition))
      {
        D->char_t[D->char_num] = condition;
        D->char_num++;
      }
      D->move_t[D->move_num].src = src;
      D->move_t[D->move_num].dest = dest;
      D->move_t[D->move_num].condition = condition;
      D->move_num++;
    }
  }
  D->char_t[D->char_num] = '\0';

  qsort(D->move_t, D->move_num, sizeof(move_set), move_set_comp_dest);
  check_string(D->char_t);

  unsigned my_index = 0;
  unsigned my_prev = 0;
  unsigned my_flag = 0;

  D->state_t[D->state_num] = D->move_t[my_index].dest;
  my_prev = D->move_t[my_index].dest;
  D->state_num++;
  for (my_index = 1; my_index < D->move_num; my_index++)
  {
    // printf("%d\n", D->move_t[my_index].dest);
    if (D->move_t[my_index].dest != my_prev)
    {
      my_prev = D->move_t[my_index].dest;
      D->state_t[D->state_num] = my_prev;
      D->state_num++;
    }
  }

  qsort(D->move_t, D->move_num, sizeof(move_set), move_set_comp);

  my_index = 0;
  my_flag = 0;
  unsigned end = D->state_num;
  for (my_index = 0; my_index < D->move_num; my_index++)
  {
    if (D->move_t[my_index].src != my_prev || my_index == 0)
    {
      my_prev = D->move_t[my_index].src;

      while (my_flag < end && D->state_t[my_flag] < my_prev)
      {
        if (my_flag == D->state_num - 1)
        {
          my_flag++;
          break;
        }
        my_flag++;
      }

      if (my_flag >= end)
      {
        D->state_t[D->state_num] = my_prev;
        D->state_num++;
      }
      else if (D->state_t[my_flag] == my_prev)
        continue;
      else
      {
        D->state_t[D->state_num] = my_prev;
        D->state_num++;
      }
    }
  }

  qsort(D->state_t, D->state_num, sizeof(unsigned), unsigned_comp);
  qsort(D->acc_t, D->acc_num, sizeof(unsigned), unsigned_comp);
  fclose(dfa_src);
}

/* get std csr nfa */
bool get_nfa(graph_no_csr *D, graph_csr *std_D)
{
  // the elements can not be doubled
  unsigned my_flag = 0;
  if (D->acc_num == 0)
  {
    printf("Error: There is no acceptable state.\n");
    return false;
  }

  // check init
  if (init_have == false)
  {
    printf("Error: There is no init state.\n");
    return false;
  }

  //check acc
  unsigned my_index = 0;
  unsigned my_prev = D->acc_t[D->acc_num - 1] + 1;

  for (my_index = 0; my_index < D->acc_num; my_index++)
  {
    // printf("1111");
    if (my_prev == D->acc_t[my_index])
    {
      printf("Error: The state (%d) is not unique in the acceptable table.\n", my_prev);
      return false;
    }
    else
    {
      my_prev = D->acc_t[my_index];
    }

    while (my_flag < D->state_num - 1 && D->state_t[my_flag] != my_prev)
    {
      // printf("%d", my_flag);
      if (my_flag == D->state_num - 1)
      {
        my_flag++;
        break;
      }
      my_flag++;
    }

    if (my_flag >= D->state_num)
    {
      printf("Error: The accept state (%d) dose not exsit in state table.\n", my_prev);
      return false;
    }
  }

  for (my_index = 0; my_index < D->state_num; my_index++)
  {
    if (D->state_t[my_index] == D->init)
      break;
  }

  if (my_index == D->state_num)
  {
    printf("Error: The init state (%d) dose not exsit in state table.", my_prev);
    return false;
  }

  // acc unique?
  unsigned acc_index = 0;
  my_index = 0;

  for (acc_index = 0; acc_index < D->acc_num; acc_index++)
  {
    while (my_index != D->state_num && D->state_t[my_index] != D->acc_t[acc_index])
    {
      my_index++;
    }
    if (my_index == D->state_num)
      break;
  }

  if (acc_index != D->acc_num || my_index == D->state_num)
  {
    printf("Error: the acceptable state(%d) is not in the state set.\n", D->acc_t[acc_index]);
    return false;
  }

  std_D->acc_num = D->acc_num;
  std_D->move_t.vertex_number = std_D->state_num = D->state_num;
  std_D->char_num = D->char_num;
  std_D->init = D->init;
  std_D->move_t.edge_number = D->move_num;

  for (my_index = 0; my_index < D->char_num; my_index++)
  {
    std_D->char_t[my_index] = D->char_t[my_index];
  }

  for (my_index = 0; my_index < D->state_num; my_index++)
  {
    std_D->state_t[my_index] = D->state_t[my_index];
  }

  for (my_index = 0; my_index < D->acc_num; my_index++)
  {
    std_D->acc_t[my_index] = D->acc_t[my_index];
  }

  // check col
  my_flag = 0;
  unsigned move_my_flag = 0;
  for (my_index = 0; my_index < D->state_num; my_index++)
  {

    std_D->move_t.row_offsets[my_index] = my_flag;
    if (D->state_t[my_index] != D->move_t[move_my_flag].src || move_my_flag >= D->move_num)
    {
      continue;
    }

    my_prev = D->move_t[move_my_flag].src;
    std_D->move_t.col_indices[my_flag] = D->move_t[move_my_flag].dest;
    std_D->move_t.values[my_flag] = D->move_t[move_my_flag].condition;
    // printf("%d  ", std_D->move_t.col_indices[my_flag]);
    my_flag++;

    move_my_flag++;
    while (move_my_flag < D->move_num && D->move_t[move_my_flag].src == my_prev)
    {

      // printf("%d  --  %d -- %d\n", D->move_t[move_my_flag].src, move_my_flag, my_prev);
      std_D->move_t.col_indices[my_flag] = D->move_t[move_my_flag].dest;
      std_D->move_t.values[my_flag] = D->move_t[move_my_flag].condition;
      // printf("%d  ", std_D->move_t.col_indices[my_flag]);
      move_my_flag++;
      my_flag++;
    }
  }

  std_D->move_t.row_offsets[my_index] = my_flag; // last element
  //  printf("  %d -- \n", std_D->move_t.vertex_number);

  for (my_index = 0; my_index < std_D->move_t.vertex_number; my_index++)
  {

    unsigned head = csr_first_edge_index(std_D->move_t, my_index);
    unsigned tail = csr_last_edge_index(std_D->move_t, my_index);
    // printf("%d, %d, %d\n", my_index, head, tail);

    if (head == tail)
    {
      // printf("%d, %d\n", std_D->acc_num, std_D->state_t[my_index]);
      if (!num_in_arry(std_D->acc_t, std_D->acc_num, std_D->state_t[my_index], &no_use))
      {
        printf("Error: The state (%d) is not acceptable state and has no next state.\n", std_D->state_t[my_index]);
        return false;
      }

      continue;
    }

    char temp[tail - head];
    // memset(temp, '\0', sizeof(temp));
    strncpy(temp, std_D->move_t.values + head, tail - head);
    // printf("%d, %s, %d\n", strlen(temp), temp, tail - head);

    if (!check_string_nfa(temp))
    {
      printf("Error: The state(%d)'s next state is not determined(%s).\n", std_D->state_t[my_index], temp);
      return false;
    }
  }
  return true;
}

/* write .dfa file */
bool write_std_dfa(graph_csr *D, char *filename)
{
  unsigned index = 0;
  FILE *dfa_des = fopen(filename, "w");
  char temp[] = {'0', ','};
  char tran[5];

  // char
  for (index = 0; index < D->char_num; index++)
  {
    fputc(D->char_t[index], dfa_des);
    if (index != D->char_num - 1)
      fputc(',', dfa_des);
  }
  fputs("\n%\n", dfa_des);

  // state
  for (index = 0; index < D->state_num; index++)
  {
    sprintf(tran, "%d", D->state_t[index]);
    fputs(tran, dfa_des);
    if (index != D->state_num - 1)
      fputc(',', dfa_des);
  }
  fputs("\n%\n", dfa_des);

  // init
  sprintf(tran, "%d", D->init);
  fputs(tran, dfa_des);
  fputs("\n%\n", dfa_des);

  // accept
  for (index = 0; index < D->acc_num; index++)
  {
    sprintf(tran, "%d", D->acc_t[index]);
    fputs(tran, dfa_des);
    if (index != D->acc_num - 1)
      fputc(',', dfa_des);
  }
  fputs("\n%\n", dfa_des);

  // vertext
  sprintf(tran, "%d", D->move_t.vertex_number);
  fputs(tran, dfa_des);
  fputs("\n%\n", dfa_des);

  // edge
  sprintf(tran, "%d", D->move_t.edge_number);
  fputs(tran, dfa_des);
  fputs("\n%\n", dfa_des);

  // row
  for (index = 0; index <= D->move_t.vertex_number; index++)
  {
    sprintf(tran, "%d", D->move_t.row_offsets[index]);
    fputs(tran, dfa_des);
    if (index != D->move_t.vertex_number)
      fputc(',', dfa_des);
  }
  fputs("\n%\n", dfa_des);

  // edge
  for (index = 0; index < D->move_t.edge_number; index++)
  {
    sprintf(tran, "%d", D->move_t.col_indices[index]);
    fputs(tran, dfa_des);
    if (index != D->move_t.edge_number - 1)
      fputc(',', dfa_des);
  }
  fputs("\n%\n", dfa_des);

  // value
  for (index = 0; index < D->move_t.edge_number; index++)
  {
    fputc(D->move_t.values[index], dfa_des);
    if (index != D->move_t.edge_number - 1)
      fputc(',', dfa_des);
  }
  fputs("\n%", dfa_des);

  fclose(dfa_des);

  return true;
}

/*
 * eps_closure: dfs to find all the vertexs which can be visited through varepsilon
 * 
 * nfa_to_dfa: nfa to dfa
 * 
*/

get_sub nfa_dfa;

void eps_closure(graph_csr *D, unsigned v, new_state *new)
{
  // printf("---%d\n", v);
  unsigned stack_index = 0;
  unsigned stack[MAXSTACK];
  bool is_acc = false, is_init = false;

  bool v_flag[D->state_num];
  for (int index = 0; index < D->state_num; index++)
    v_flag[index] = false;

  // INTERESTING: hahaha? find you bug!!!
  // v_flag[v] = true;
  if (!num_in_arry(new->states, new->state_numer, v, &no_use))
    new->states[new->state_numer++] = v;

  if (num_in_arry(D->acc_t, D->acc_num, v, &no_use))
  {
    is_acc = true;
  }
  if (v == D->init)
    is_init = true;

  stack[stack_index++] = v;

  // new->stop_state_number = new->state_numer = 0;
  unsigned top, head, tail;
  while (stack_index != 0)
  {
    top = stack[--stack_index];
    head = csr_first_edge_index(D->move_t, top);
    tail = csr_last_edge_index(D->move_t, top);

    unsigned i = 0;
    for (i = head; i < tail; i++)
    {
      v = csr_get_next_vertex(D->move_t, i);

      if (!v_flag[v])
      {
        v_flag[v] = true;
        if (csr_get_edge_value(D->move_t, i) == '^')
        {
          stack[stack_index++] = v; // into stack

          if (!num_in_arry(new->states, new->state_numer, v, &no_use))
            new->states[new->state_numer++] = v;

          if (num_in_arry(D->acc_t, D->acc_num, v, &no_use))
          {
            is_acc = true;
          }
          if (v == D->init)
            is_init = true;
        }
        else
        {
          // printf("%d\n",top);
          new->stop[new->stop_state_number].state = top;
          new->stop[new->stop_state_number].condition = csr_get_edge_value(D->move_t, i);
          new->stop[new->stop_state_number].next = v;
          new->stop[new->stop_state_number++].edge_index = i;
        }
      }
    }
  }

  new->is_acc |= is_acc;
  new->is_init |= is_init;

  // if(new->stop_state_number > 1)
  // qsort(new->stop, new->stop_state_number, sizeof(unsigned), unsigned_comp);
  if (new->state_numer > 1)
    qsort(new->states, new->state_numer, sizeof(unsigned), unsigned_comp);
}

bool eps_closure_acc(graph_csr *D, unsigned v)
{
  // printf("---%d\n", v);
  unsigned stack_index = 0;
  unsigned stack[MAXSTACK];
  bool is_acc = false, is_init = false;

  bool v_flag[D->state_num];
  for (int index = 0; index < D->state_num; index++)
    v_flag[index] = false;

  if (num_in_arry(D->acc_t, D->acc_num, v, &no_use))
  {
    is_acc = true;
  }
  if (v == D->init)
    is_init = true;

  stack[stack_index++] = v;

  // new->stop_state_number = new->state_numer = 0;
  unsigned top, head, tail;
  while (stack_index != 0)
  {
    top = stack[--stack_index];
    head = csr_first_edge_index(D->move_t, top);
    tail = csr_last_edge_index(D->move_t, top);

    unsigned i = 0;
    for (i = head; i < tail; i++)
    {
      v = csr_get_next_vertex(D->move_t, i);

      if (!v_flag[v])
      {
        v_flag[v] = true;
        if (csr_get_edge_value(D->move_t, i) == '^')
        {
          stack[stack_index++] = v; // into stack

          if (num_in_arry(D->acc_t, D->acc_num, v, &no_use))
          {
            is_acc = true;
          }
          if (v == D->init)
            is_init = true;
        }
      }
    }
  }
  return is_acc;
}

void nfa_to_dfa(graph_csr *D, graph_csr *DFA)
{
  nfa_dfa.sub_index = 0;
  nfa_dfa.edge_index = 0;
  nfa_dfa.sub[nfa_dfa.sub_index].stop_state_number = nfa_dfa.sub[nfa_dfa.sub_index] .state_numer = 0; 
  eps_closure(D, D->init, nfa_dfa.sub + nfa_dfa.sub_index);
  nfa_dfa.sub_index++;

  unsigned stack[MAXSTACK];
  unsigned stack_index = 0;
  stack[stack_index++] = 0;
  // printf("acc:  %d\n",D->acc_t[0]);
  unsigned top;
  while (stack_index != 0)
  {
    top = stack[--stack_index];

    qsort(nfa_dfa.sub[top].stop, nfa_dfa.sub[top].stop_state_number, sizeof(stop_state), stop_state_comp);

    unsigned i, previ;
    char prev = 0;
    unsigned new_index;

    for (i = 0; i < nfa_dfa.sub[top].stop_state_number; i++)
    {

      if (i == 0 || prev != nfa_dfa.sub[top].stop[i].condition)
      {
        if (i != 0)
        {
          // printf("++++++%c\n", prev);
          int temp = find_new_state_index(*(nfa_dfa.sub + nfa_dfa.sub_index), nfa_dfa);

          if (temp == -1)
          {
            nfa_dfa.sub_edge[nfa_dfa.edge_index].src = top;
            nfa_dfa.sub_edge[nfa_dfa.edge_index].dest = nfa_dfa.sub_index;
            nfa_dfa.sub_edge[nfa_dfa.edge_index++].condition = prev;

            stack[stack_index++] = nfa_dfa.sub_index;

            nfa_dfa.sub_index++;
          }
          else
          {
            nfa_dfa.sub_edge[nfa_dfa.edge_index].src = top;
            nfa_dfa.sub_edge[nfa_dfa.edge_index].dest = temp;
            nfa_dfa.sub_edge[nfa_dfa.edge_index++].condition = prev;
          }
        }

        // new_index = nfa_dfa.sub_index;
        nfa_dfa.sub[nfa_dfa.sub_index].state_numer = 0;
        nfa_dfa.sub[nfa_dfa.sub_index].stop_state_number = 0;
        nfa_dfa.sub[nfa_dfa.sub_index].is_acc = false;
        nfa_dfa.sub[nfa_dfa.sub_index].is_init = false;
        prev = nfa_dfa.sub[top].stop[i].condition;
        previ = i;
      }

      eps_closure(D,
                  nfa_dfa.sub[top].stop[i].next,
                  nfa_dfa.sub + nfa_dfa.sub_index);
    }

    // TODO: simple
    if (nfa_dfa.sub[top].stop_state_number != 0)
    {
      int temp = find_new_state_index(*(nfa_dfa.sub + nfa_dfa.sub_index), nfa_dfa);
      if (temp == -1)
      {

        nfa_dfa.sub_edge[nfa_dfa.edge_index].src = top;
        nfa_dfa.sub_edge[nfa_dfa.edge_index].dest = nfa_dfa.sub_index;
        nfa_dfa.sub_edge[nfa_dfa.edge_index++].condition = prev;

        stack[stack_index++] = nfa_dfa.sub_index;

        nfa_dfa.sub_index++;
      }
      else
      {
        nfa_dfa.sub_edge[nfa_dfa.edge_index].src = top;
        nfa_dfa.sub_edge[nfa_dfa.edge_index].dest = temp;
        nfa_dfa.sub_edge[nfa_dfa.edge_index++].condition = prev;
      }
    }
  }

  get_std_graph_csr(&nfa_dfa, DFA);

  print_sub(nfa_dfa);
}

/*
 * dfa_to_min: minimize the dfa.
*/
graph_map g;

void dfa_to_min(graph_csr *D, graph_csr *M)
{
  unsigned index;

  /* init the g */
  g.acc = 0;
  g.non_acc = 0;
  g.all = 0;

  /* init maps */
  g.transition_map = (int **)malloc(MAXSTATE * sizeof(int *));
  g.partition_map = (int **)malloc(MAXSTATE * sizeof(int *));
  for (index = 0; index < MAXSTATE; index++)
  {
    g.transition_map[index] = (int *)malloc(MAXCHAR * sizeof(int));
    g.partition_map[index] = (int *)malloc(MAXCHAR * sizeof(int));
    for (int i = 0; i < MAXCHAR; i++)
    {
      g.transition_map[index][i] = -1;
      g.partition_map[index][i] = -1;
    }
  }

  /* fill the infomation of transitions */
  for (index = 0; index < D->state_num; index++)
  {
    unsigned head = csr_first_edge_index(D->move_t, index);
    unsigned tail = csr_last_edge_index(D->move_t, index);
    for (int i = head; i < tail; i++)
    {
      unsigned char_index;
      char_in_arry(D->char_t, D->char_num, D->move_t.values[i], &char_index); /* find the char's position in transition table */
      // printf("char %c, %d\n",  D->move_t.values[i], char_index);
      g.transition_map[index][char_index] = D->move_t.col_indices[i];

      g.all |= (1 << index);
      g.all |= (1 << D->move_t.col_indices[i]);
    }
  }

  // for (index = 0; index < D->state_num; index++){
  //   for(int i =0; i < D->char_num; i++){
  //     printf("%d ", g.transition_map[index][i]);
  //   }
  //   printf("\n");
  // }

  /* fill the information of init and acc */
  for (index = 0; index < D->acc_num; index++)
  {
    g.acc |= 1 << (D->acc_t[index]);
  }
  g.init = D->init;

  /* TODO: reachable */
  g.reachable = g.all;
  g.all &= g.reachable;
  g.acc &= g.reachable;

  /* init partion sets */
  g.partition = (long int *)malloc(MAXSTATE * sizeof(long int));
  for (index = 0; index < MAXSTATE; index++)
  {
    g.partition[index] = 0;
  }

  /* two partition: acc and non-acc*/
  g.non_acc = g.all & (~g.acc);
  g.partition[0] = g.acc;
  g.partition[1] = g.non_acc;
  // printf("------%x",g.partition[1]);

  g.partion_index = 0;
  if(g.acc != 0) g.partition[g.partion_index ++ ] = g.acc;
  if(g.non_acc != 0) g.partition[g.partion_index++] = g.non_acc;

  for (int partions = 0; partions < MAXSTATE; partions++)
  {

    if (g.partition[partions] == 0)
      break;

    long int new_partion = 0;

    /* find first leader */
    long int leader = MAXSTATE;
    for (leader = MAXSTATE; leader >= 0; leader--)
    {
      // printf("=====%ld, %ld\n",(long int)1 << leader,leader);
      if ((g.partition[partions] & ((long int)1 << leader)) != 0)
        break;
    }
    g.partition_map[partions] = g.transition_map[leader];

    /* kick out the members who are conflict with the leader */
    long int member;
    for (member = leader - 1; member >= 0; member--)
    {
      long int leader_bit = (long int)1 << leader;
      long int member_bit = (long int)1 << member;

      if ((g.partition[partions] & member_bit) == 0)
        continue;

      /* char index */
      for (index = 0; index < D->char_num; index++)
      {
        int leader_next = -1;
        int member_next = -1;

        // INTERESTING: the operation order is so intereting, crying!!!
        for (int p = 0; p < g.partion_index; p++)
        {
          // printf("leader: %d member: %d\n",g.transition_map[leader][index], g.transition_map[member][index]);
          if ((g.partition[p] & (1 << g.transition_map[leader][index]) )!= 0)
            leader_next = p;
          if ((g.partition[p] & (1 << g.transition_map[member][index])) != 0)
            member_next = p;
        }
        // printf("%d\n",partions);

        /* kick out */
        // if is not point to the states contained in the sets
        // and the condition of edges is not the same
        // split!
        // printf("+: %d, %d, %d, %d\n", g.transition_map[leader][index], g.transition_map[member][index], leader_next, member_next);

        if (
          (leader_next != member_next && g.transition_map[member][index] != g.transition_map[leader][index]) 
          //|| (g.transition_map[leader][index] == -1 && g.transition_map[leader][index] != 1) ||
          // (g.transition_map[leader][index] == 1 && g.transition_map[leader][index] != -1)
        )
        {
          // printf("new\n");
          g.partition[partions] &= ~member_bit;
          new_partion |= member_bit;
          break;
        }

      }

      if (new_partion != 0)
      {
        g.partition[g.partion_index++] = new_partion;
      }
    }
  }


  get_sub dfa_to_min;
  dfa_to_min.edge_index = dfa_to_min.sub_index = 0;

  
  for (int i = 0; i < g.partion_index; i++)
  {
    for (int j = 0; j < D->char_num; j++)
    {
      if (g.partition_map[i][j] != -1)
      {
        for (int k = 0; k < g.partion_index; k++)
        {
          if ((g.partition[k] & (1 << g.partition_map[i][j])) != 0)
          {
            // printf("%d %c %d\n", i, D->char_t[j], k);
            dfa_to_min.sub_edge[dfa_to_min.edge_index].src = i;
            dfa_to_min.sub_edge[dfa_to_min.edge_index].dest = k;
            dfa_to_min.sub_edge[dfa_to_min.edge_index++].condition = D->char_t[j];
          }
        }
      }
    }
  }

  for (int i = 0; i < g.partion_index; i++)
  {
    if (g.partition[i] == 0)
      continue;
    dfa_to_min.sub[dfa_to_min.sub_index].state_numer = i;
    dfa_to_min.sub[dfa_to_min.sub_index].state_numer = 0;
    bool is_acc = g.partition[i] & g.acc;
    bool is_init = false;
    for (int j = MAXSTATE; j >= 0; j--)
    {
      long int bit = (long int)1 << j;
      if ((g.partition[i] & bit) != 0)
      {
        dfa_to_min.sub[dfa_to_min.sub_index].states[dfa_to_min.sub[dfa_to_min.sub_index].state_numer++] = j;

        if (j == g.init)
          is_init = true;
      }
    }
    dfa_to_min.sub[dfa_to_min.sub_index].is_acc = is_acc;
    dfa_to_min.sub[dfa_to_min.sub_index].is_init = is_init;
    dfa_to_min.sub_index++;
  }

  print_sub(dfa_to_min);
  get_std_graph_csr(&dfa_to_min, M);
}

bool nfa_dfs(graph_csr D, unsigned depth)
{
  string str[100];
  unsigned str_index = 0;

  unsigned v_stack[100];
  new_state st;

  if(depth > 10)
  {
    printf("Error: The depth(%d) is too large.\n", depth);
  }

  printf("number\tstirng\t\n");
  char c_stack[depth + 1];
  unsigned c_pointer = 0;
  unsigned v_pointer = 0;
  unsigned v_top;
  string temp;
  char temp2[depth+1];
  
  unsigned index = 0, number = 0;

  st.state_numer = st.stop_state_number = 0; 
  st.is_acc = st.is_init = false;

  eps_closure(&D, D.init, &st);
  v_stack[v_pointer++] = U_MAX;
  for(index = 0; index < st.stop_state_number; index++)
  {
    v_stack[v_pointer++] = st.stop[index].edge_index;
    // printf("%c\n", D.move_t.values[st.stop[index].edge_index]);
  }

  // printf("v_top: %d\n", v_pointer);
  while(v_pointer != 0)
  {
    v_top = v_stack[--v_pointer];

    while(v_top == U_MAX)
    {
      c_pointer --;
      v_top = v_stack[--v_pointer];
      if(v_pointer == 0) break;
    }
    if(v_stack[v_pointer] == U_MAX && v_pointer == 0) break;

    c_stack[c_pointer++] = D.move_t.values[v_top];
    // printf("v_top: %d\t\%d\n", v_top, c_pointer);
    // if(num_in_arry(D.acc_t, D.acc_num, D.move_t.col_indices[v_top], &no_use))
    if(eps_closure_acc(&D, D.move_t.col_indices[v_top]))
    {

      // number++;
      memset(temp.s, '\0', sizeof(temp2));
      // strncpy(temp2, &c_stack[0], c_pointer );
      strncpy(temp.s, &c_stack[0], c_pointer);
      // printf("%d\t%s\n", strlen(temp.s), temp.s);
      // printf("%d\t%s\n", number,temp.s);
      if(!string_in_arry(str, str_index, temp, &no_use))
      {
        str[str_index++] = temp;
      }
    }
    
    if(c_pointer < depth)
    {
      num_in_arry(D.state_t, D.state_num, D.move_t.col_indices[v_top], &v_top);
      v_stack[v_pointer++] = U_MAX;

      st.state_numer = st.stop_state_number = 0;
      eps_closure(&D, v_top, &st);
      for(index = 0; index < st.stop_state_number; index++)
      {
        v_stack[v_pointer++] = st.stop[index].edge_index;
      }
    }
    else
    {
      c_pointer--;
    }
  }

  for(index = 0; index < str_index; index++)
  {
   printf("%d\t%s\n", index+1, str[index].s);
  }

}


/* debug helper */
void print_sub(get_sub subs)
{
#if DEBUG
  printf("%s\t%s\t%s\t%s\n", "index", "number", "acc", "state");
  unsigned index = 0;
  for (index = 0; index < subs.sub_index; index++)
  {
    unsigned i = 0;
    printf("%d\t%d\t%d\t", index, subs.sub[index].state_numer, subs.sub[index].is_acc);
    for (i = 0; i < subs.sub[index].state_numer; i++)
    {
      printf("%d ", subs.sub[index].states[i]);
    }
    printf("\n");
  }

  // for(index = 0; index < subs.edge_index; index++)
  // {
  //   printf("%d, %c, %d\n", subs.sub_edge[index].src, subs.sub_edge[index].condition, subs.sub_edge[index].dest);
  // }
#else
#endif
}
