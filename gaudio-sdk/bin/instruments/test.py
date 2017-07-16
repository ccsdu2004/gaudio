import glob
import io

file = open('out.txt','w')

for filename in glob.glob("*.pat"):
    str = 'Source: \"instruments\\'
    str += filename
    str += '\"; DestDir: \"{app}\instruments\"; Flags: ignoreversion\n'
    file.write(str)

file.close()
