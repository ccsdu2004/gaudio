import os

def do_function(line):
    pass

fn = open("gaudio.h","r");

out = None
if fn != None:
    out = open('gaudio.cpp','w')
    out.write('#include "gaudio.h"\n\n')

while True:
    line = fn.readline()
    if len(line) == 0:
        break 
    else:
        if line.startswith('extern GAPI'):
            line = line[12:len(line)-2]
            line += '{'
            if line.startswith('void* ') or not line.startswith('void'):
                line += 'return 0;}\n'
            else:
                line += '}\n'
            if out != None:
                out.write(line)
            print(line)
                
    
if fn != None:
    fn.close()

if out != None:
    out.close()
