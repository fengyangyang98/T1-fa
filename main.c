#include "nfa.h"
#include "re.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

void print_help()
{
    printf("Usage: ./fa [-pd] <filename> [-sf] [<string>/<filename>] \n");
    printf("       ./fa [-pd] <filename> [-mwa] [-n] <number>\n");
    printf("       ./fa  -e <filename> [-iv]\n");
    printf("       ./fa  -r <filename> [-o] <filename>\n\n");
    


    printf("-h                  Print this help message.\n\n");

    printf("DFA Options:\n");
    printf("-p      <filename>  Input the DFA by the <filename>.dfa file.\n");
    printf("-d      <filename>  Input the DFA by the <filename>.dfa.fine file.\n\n");

    printf("-s      <string>    Input the string to run the DFA.\n");
    printf("-f      <filename>  Input the string from <filename> to run the DFA.\n\n");

    printf("-m                  Minimize the DFA and output to the file min-*.dfa.fine. \n");
    printf("-w                  Chage the <filename>.dfa to <filename>.dfa.fine.\n\n");

    printf("-a                  Output all the possible strings according to the <filename>.\n");
    printf("-n      <number>    The max depth of the DFA, default is 5.\n\n");

    printf("NFA Options:\n");
    printf("-e      <filename>  Input the NFA by the <filename>.nfa file.\n");
    printf("-i                  Convert the NFA to the std NFA and output to the .nfa.fine file.\n");
    printf("-v                  Convert the NFA to the DFA and output to the .dfa.fine file.\n");
    printf("-r      <filename>  Convert the re to the NFA and store in <name>.nfa\n");
    printf("-o      <name>      File name stored.\n");
}


int main(int argc, char * argv[])
{
    // printf("11111111");
    if(argc == 1)
    {
        print_help();
        exit(1);
    }

    

    char op_flag = 0;
    char file_flag = 0;
    char ch;

    char dfa_file[100] = "";
    char nfa_file[100] = "";
    char string_file[100] = "";
    char re_file[100] = "";

    char str[100] = "";
    char name[100] = "";
    char re[100] = "";

    FILE * temp;

    graph_no_csr pre_dfa, pre_nfa;
    graph_csr nfa, dfa, min_dfa;
    unsigned depth = 5;

    while ((ch = getopt(argc, argv, "hp:d:c:s:f:w:n:e:aivr:m")) != -1)
    {
        switch(ch)
        {
            case 'p':
                file_flag = 'p';
                strcpy(dfa_file,optarg); 
               strncpy(name, optarg, strlen(optarg) - 4);
                // printf("-----");
                //  printf("%s", dfa_file);
                break;
            case 'd':
                file_flag = 'd';
                strcpy(dfa_file,optarg);
                strncpy(name, optarg, strlen(optarg) - 9);
                break;
            case 'e':
                file_flag = 'e';
                strcpy(nfa_file,optarg); 
                strncpy(name, optarg, strlen(optarg) - 4);
                break;

            case 's':
                op_flag = 's';
                strcpy(str,optarg);
                break;
            case 'f':
                op_flag = 's';
                strcpy(string_file,optarg);

                temp = fopen(string_file, "r");
                if(fscanf(temp, "%s", str) == 0)
                {
                    print_help();
                    exit(0);
                }
                fclose(temp);

                break;

            case 'm':
                op_flag = 'm';
                break;
            case 'c':
                op_flag = 'c';
                break;
            case 'a':
                op_flag = 'a';
                break;
            case 'n':
                depth = atoi(optarg); 
                break;

            case 'i':
                op_flag = 'i';
                break;
            case 'v':
                op_flag = 'v';
                break;

            case 'h':
                print_help();
                break;

            case 'r':
                file_flag = 'r';
                op_flag = 'r';
                strcpy(re_file,optarg);
                strncpy(name, optarg, strlen(optarg) - 3);
                // printf("%s", name);

                temp = fopen(re_file, "r");
                if(fscanf(temp, "%s", re) == 0)
                {
                    print_help();
                    // printf("-----%s", re);
                    exit(0);
                }
                fclose(temp);
                break;

            case 'o':
                strncpy(name, optarg, strlen(optarg));
                break;

            default:
                print_help();
                exit(1);
                break;

        }

    }

    switch (file_flag)
    {
        case 'p':
            //  printf("%s", dfa_file);
            read_pre_dfa_file(dfa_file, &pre_dfa);
            // printf("%s", dfa_file);
            check_pre_dfa(&pre_dfa ,&dfa);
            break;
        case 'd':
            read_dfa_file(dfa_file, &dfa);
            break;
        case 'e':
            read_pre_nfa_file(nfa_file, &pre_nfa);
            get_nfa(&pre_nfa, &nfa);
            nfa_to_dfa(&nfa, &dfa);
            break;
        case 'r': break;
        default:
            print_help();
            exit(1);
            break;
    }

    unsigned end_state;
    
    switch(op_flag){
        case 's':
            if(dfa_judge(str, dfa, &end_state) == true)
            {
                printf("The string (%s) is right, and the end state is %d.\n", str, end_state);
            }
            break;

        case 'm':
            strcat(name, "min.dfa.fine");
            dfa_to_min(&dfa, &min_dfa);
            write_std_dfa(&min_dfa, name);
            break;

        case 'w':
            strcat(name, ".dfa.fine");
            write_std_dfa(&dfa, name);
            break;

        case 'a':
            if(file_flag == 'p' || file_flag == 'd')
                dfa_dfs(dfa, depth);
            else if(file_flag == 'e')
                nfa_dfs(nfa, depth);
            break;

        case 'i':
            strcat(name, ".nfa.fine");
            write_std_dfa(&nfa, name);
            break;

        case 'v':
            strcat(name, ".dfa.fine");
            nfa_to_dfa(&nfa, &dfa);
            write_std_dfa(&dfa, name); 
            break;

        case 'r':
            strcat(name, ".nfa");
            re_to_nfa(re, name);
            break;

        default:
            // print_help();
            exit(0);
    }
}
