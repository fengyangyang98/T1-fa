# FAM - Finite Automata Machine Tool

## 使用

```shell
Usage: ./fa [-pd] <filename> [-sf] [<string>/<filename>] 
       ./fa [-pd] <filename> [-mwa] [-n] <number>
       ./fa  -e <filename> [-iv]
       ./fa  -r <filename> [-o] <filename>

-h                  Print this help message.

DFA Options:
-p      <filename>  Input the DFA by the <filename>.dfa file.
-d      <filename>  Input the DFA by the <filename>.dfa.fine file.

-s      <string>    Input the string to run the DFA.
-f      <filename>  Input the string from <filename> to run the DFA.

-m                  Minimize the DFA and output to the file min-*.dfa.fine. 
-w                  Chage the <filename>.dfa to <filename>.dfa.fine.

-a                  Output all the possible strings according to the <filename>.
-n      <number>    The max depth of the DFA, default is 5.

NFA Options:
-e      <filename>  Input the NFA by the <filename>.nfa file.
-i                  Convert the NFA to the std NFA and output to the .nfa.fine file.
-v                  Convert the NFA to the DFA and output to the .dfa.fine file.
-r      <filename>  Convert the re to the NFA and store in <name>.nfa
-o      <name>      File name stored.
```

## Todo

- [ ] 完成说明文档
- [ ] 完善图片生成