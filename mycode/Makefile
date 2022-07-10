mycode:mycode.o
		gcc mycode.o -o mycode
mycode.o:mycode.s
		gcc -c mycode.s -o mycode.o
mycode.s:mycode.i
		gcc -S mycode.i -o mycode.s
mycode.i:mycode.c
		gcc -E mycode.c -o mycode.i
.PHONY:clean
clean:
		rm -rf mycode mycode.i mycode.s mycode.o

