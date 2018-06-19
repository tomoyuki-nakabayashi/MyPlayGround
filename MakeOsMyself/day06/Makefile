CFLAGS = -c -m32 -march=i486 -nostdlib

default:
	make img

ipl.bin : ipl.asm Makefile
	nasm ipl.asm -o ipl.bin -l ipl.lst

asmhead.bin : asmhead.asm Makefile
	nasm asmhead.asm -o asmhead.bin -l haribote.lst

nasmfunc.o : nasmfunc.asm Makefile
	nasm -g -f elf nasmfunc.asm -o nasmfunc.o

hankaku.c : hankaku.txt convert_hankaku.py
	python convert_hankaku.py

%.o : %.c Makefile
	gcc $(CFLAGS) $^ -o $@

bootpack.hrb : bootpack.c har.ld hankaku.c nasmfunc.o my_sprintf.o graphic.o dsctbl.o int.o Makefile
	gcc -march=i486 -m32 -nostdlib -nostdinc -T har.ld bootpack.c graphic.o dsctbl.o int.o hankaku.c nasmfunc.o my_sprintf.o -o bootpack.hrb

haribote.sys : asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img : ipl.bin haribote.sys Makefile
	mformat -f 1440 -C -B ipl.bin -i haribote.img ::
	mcopy haribote.sys -i haribote.img ::

asm :
	make -r ipl.bin

img :
	make -r haribote.img

run :
	make img
	qemu-system-i386 -fda haribote.img

clean :
	rm *.lst *.bin *.sys *.img *.hrb *.o hankaku.c
