import io
import sys

def text_get(file):
    fptr = open(file,'r')
    text = fptr.read()
    fptr.close()
    return text

def text_save(file,text):
    fptr = open(file,'w')
    fptr.write(text)
    fptr.close()

def text_make(file,prj):
    text = text_get(file)
    text = text.replace('#PRJ#',prj)
    pr = prj + '.cbp'
    text_save(pr,text)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('bad input paremeter.')
    else:
        text_make('template.cbp',sys.argv[1])
    
