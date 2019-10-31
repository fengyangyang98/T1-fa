from graphviz import Digraph as dg 
import sys

def fa(ob, src, dest, condition):
    if condition == '^': 
        condition = 'ep'
    if condition == ' ':
        return
    
    if condition == '%':
        # print(src)
        ob.attr('node', shape='doublecircle')
        ob.node(src)
    else:
        ob.attr('node', shape='circle', fixedsize='true', width='1.1', fontsize = '10')
        ob.edge(src, dest, condition)


file = sys.argv[1]

dfa = dg('dfa', format='jpg')

f = open(file)

for line in f:
    if ',' in line:
        s = (line.strip('\n')).split(',')
        if s[1] == '%':
            dfa.attr('node', shape='doublecircle')
            dfa.node(name = s[0]) 
f.close()
f = open(file)

for line in f:
    if ',' in line:
        s = (line.strip('\n')).split(',')
        fa(dfa, s[0], s[2], s[1])
f.close()

dfa.attr(rankdir='LR')

dfa.render()