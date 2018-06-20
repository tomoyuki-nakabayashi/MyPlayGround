#coding:utf-8

fin = open('hankaku.txt', 'r')
fout = open('hankaku.c', 'w')

with fin, fout:
    fout.write('char hankaku[4096] =  {')

    for line in fin:
        if '.' in line or '*' in line:
            sum = 0
            for i, s in enumerate(reversed(line.strip())):
                x = 1 if (s == '*') else 0
                sum += x << i
            fout.write('0x{:02x}, '.format(sum))
        elif 'char' in line:
            fout.write("\n    ")

    fout.write('\n};\n')
