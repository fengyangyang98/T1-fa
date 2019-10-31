#include "nfa.h"

bool init_have;

unsigned my_index;
unsigned my_prev;
unsigned my_flag;
unsigned no_use;

void read_dfa_file(char *filename, graph_csr *D)
{
  char buff[255];
  FILE *dfa_src = fopen(filename, "r");

  D->char_num = D->state_num = D->acc_num = 0;
  D->move_t.edge_number = D->move_t.vertex_number = 0;
  int v = 0, e1 = 0, e2 = 0;
  char state = 0;
  while (!feof(dfa_src))
  {

    int not_emtpy = fscanf(dfa_src, "%[^(\n|%|,)]", buff);
    char ch = fgetc(dfa_src);

    if (ch == '%')
    {
      state += 1;
    }
    if (not_emtpy == 0)
      continue;

    // printf("%s\n", buff);

    switch (state)
    {
    case 0:
      D->char_t[D->char_num] = buff[0];
      D->char_num++;
      break;

    case 1:
      D->state_t[D->state_num] = (unsigned)atoi(buff);
      D->state_num++;
      break;

    case 2:
      D->init = (unsigned)atoi(buff);
      break;

    case 3:
      D->acc_t[D->acc_num] = (unsigned)atoi(buff);
      D->acc_num++;
      break;

    case 4:
      D->move_t.vertex_number = (unsigned)atoi(buff);
      break;

    case 5:
      D->move_t.edge_number = (unsigned)atoi(buff);
      break;

    case 6:
      if (v > D->move_t.vertex_number)
        break;
      D->move_t.row_offsets[v] = (unsigned)atoi(buff);
      v++;
      break;

    case 7:
      if (e1 > D->move_t.edge_number)
        break;
      D->move_t.col_indices[e1] = (unsigned)atoi(buff);
      e1++;
      break;

    case 8:
      if (e2 > D->move_t.edge_number)
        break;
      D->move_t.values[e2] = buff[0];
      e2++;
      break;

    default:
      break;
    }
  }

  fclose(dfa_src);
}

void read_pre_dfa_file(char *filename, graph_no_csr *D)
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
    else if (condition == '%')
    {
      if (src != dest)
      {
        printf("Error line %d: the src(%d) and the dest(%d) number must be the same when you enter the acceptabel state.\n", lineno, src, dest);
        break;
      }

      D->acc_t[D->acc_num] = src;
      D->acc_num++;
    }
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

  my_index = 0;
  my_prev = 0;

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

bool check_pre_dfa(graph_no_csr *D, graph_csr *std_D)
{
  // the elements can not be doubled
  my_flag = 0;
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
  my_prev = D->acc_t[D->acc_num - 1] + 1;

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

    if (!check_string(temp))
    {
      printf("Error: The state(%d)'s next state is not determined(%s).\n", std_D->state_t[my_index], temp);
      return false;
    }
  }

  return true;
}

bool next_state(graph_csr D, unsigned state, char ch, unsigned *next)
{
  unsigned head = csr_first_edge_index(D.move_t, state);
  unsigned tail = csr_last_edge_index(D.move_t, state);

  unsigned index = 0;
  // unsigned next_index = 0;

  for (index = head; index < tail; index++)
  {
    if (D.move_t.values[index] == ch)
    {
      // printf("%c, %c, %d\n", D.move_t.values[index], ch, index);
      // num_in_arry(D.state_t, D.state_num, D.move_t.col_indices[index], next);
      *next = D.move_t.col_indices[index];
      // printf("next: %d\n", *next);
      return true;
    }
  }

  for (index = head; index < tail; index++)
  {
    if (D.move_t.values[index] == '#')
    {
      num_in_arry(D.state_t, D.state_num, D.move_t.col_indices[index], next);
      return true;
    }
  }

  return false;
}


bool dfa_judge(char *str, graph_csr D, unsigned *end_state)
{
  unsigned index = 0;
  unsigned state = D.init;
  bool right;
  // printf("len %d\n", strlen(str));
  for (index = 0; index < strlen(str); index++)
  {
    right = next_state(D, state, str[index], &state);

    if (!right)
    {
      printf("Error: When handling the char(%c), there is no next state.\n", str[index]);
      return false;
    }
  }

  if (num_in_arry(D.acc_t, D.acc_num, D.state_t[state], &no_use))
  {
    *end_state = state;
    return true;
  }

  printf("Error: At the last char(%c), but not at a accpetable state(%d).\n", str[index], state);
  return false;
}

bool dfa_dfs(graph_csr D, unsigned depth)
{
  unsigned v_stack[100];
  string str[100];
  unsigned str_index = 0;

  if(depth > 10)
  {
    printf("Error: The depth(%d) is too large.\n", depth);
  }

  printf("number\tstirng\t\n");
  char c_stack[depth + 1];
  unsigned c_pointer = 0;
  unsigned v_pointer = 0;

  string temp;
  char temp2[depth+1];
  
  unsigned head, tail, v_top, index;
  unsigned ii = 0, number = 0;
  
  num_in_arry(D.state_t, D.state_num, D.init, &v_top);
  head = csr_first_edge_index(D.move_t, v_top);
  tail = csr_last_edge_index(D.move_t, v_top);
  v_stack[v_pointer++] = U_MAX;
  for(index = head; index < tail; index++)
  {
    (v_stack[v_pointer++]) = index;
  }   

  while(v_pointer != 0)
  {
    // printf("v_top: %d\n", v_pointer);
    v_top = v_stack[--v_pointer];

    while(v_top == U_MAX)
    {
      c_pointer --;
      v_top = v_stack[--v_pointer];
      if(v_pointer == 0) break;
    }
    if(v_stack[v_pointer] == U_MAX && v_pointer == 0) break;

    c_stack[c_pointer++] = D.move_t.values[v_top];
    if(num_in_arry(D.acc_t, D.acc_num, D.move_t.col_indices[v_top], &no_use))
    {
      // number++;
      // memset(temp, '\0', sizeof(temp));
      // strncpy(&temp[0], &c_stack[0], c_pointer );
      // printf("%d\t%s\n", number, temp);
      memset(temp.s, '\0', sizeof(temp2));
      strncpy(temp.s, &c_stack[0], c_pointer);
      if(!string_in_arry(str, str_index, temp, &no_use))
      {
        str[str_index++] = temp;
      }
    }
    
    if(c_pointer < depth)
    {
      num_in_arry(D.state_t, D.state_num, D.move_t.col_indices[v_top], &v_top);
      head = csr_first_edge_index(D.move_t, v_top);
      tail = csr_last_edge_index(D.move_t, v_top);
      v_stack[v_pointer++] = U_MAX;
      for(index = head; index < tail; index++)
      {
        v_stack[v_pointer++] = index;
      }
    }
    else
    {
      c_pointer--;
    }
  }

  for(ii = 0; ii < str_index; ii++)
  {
   printf("%d\t%s\n", ii+1, str[ii].s);
  }

}

// unsigned v_stack[100];
// bool dfa_dfs(graph_csr D, unsigned depth)
// {
//   if(depth > 10)
//   {
//     printf("Error: The depth(%d) is too large.\n", depth);
//   }

//   printf("number\tstirng\t\n");
//   char c_stack[depth + 1];
//   unsigned c_pointer = 0;
//   unsigned v_pointer = 0;
//   char temp[depth + 1];
  
//   unsigned head, tail, v_top, index;
//   unsigned number = 0;
  
//   num_in_arry(D.state_t, D.state_num, D.init, &v_top);
//   head = csr_first_edge_index(D.move_t, v_top);
//   tail = csr_last_edge_index(D.move_t, v_top);
//   v_stack[v_pointer++] = U_MAX;
//   for(index = head; index < tail; index++)
//   {
//     (v_stack[v_pointer++]) = index;
//   }   

//   while(v_pointer != 0)
//   {
//     // printf("v_top: %d\n", v_pointer);
//     v_top = v_stack[--v_pointer];

//     while(v_top == U_MAX)
//     {
//       c_pointer --;
//       v_top = v_stack[--v_pointer];
//       if(v_pointer == 0) break;
//     }
//     if(v_stack[v_pointer] == U_MAX && v_pointer == 0) break;

//     c_stack[c_pointer++] = D.move_t.values[v_top];
//     if(num_in_arry(D.acc_t, D.acc_num, D.move_t.col_indices[v_top], &no_use))
//     {
//       number++;
//       memset(temp, '\0', sizeof(temp));
//       strncpy(&temp[0], &c_stack[0], c_pointer );
//       printf("%d\t%s\n", number, temp);
//     }
    
//     if(c_pointer < depth)
//     {
//       num_in_arry(D.state_t, D.state_num, D.move_t.col_indices[v_top], &v_top);
//       head = csr_first_edge_index(D.move_t, v_top);
//       tail = csr_last_edge_index(D.move_t, v_top);
//       v_stack[v_pointer++] = U_MAX;
//       for(index = head; index < tail; index++)
//       {
//         v_stack[v_pointer++] = index;
//       }
//     }
//     else
//     {
//       c_pointer--;
//     }
//   }

// }
