libmymath.a:sub.o add.o
	ar -rc $@ $^
%.o:%.c
	gcc -c $<

.PHONY:clean
clean:
	rm -rf libmymath.a *.o lib

.PHONY:lib
lib:
	mkdir lib
	cp libmymath.a *.h lib

