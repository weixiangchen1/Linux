libmymath.so:add.o sub.o
	gcc -shared -o $@ $^
%.o:%.c
	gcc -fPIC -c $<

.PHONY:clean
clean:
	rm -f libmymath.so *.o

.PHONY:lib
lib:
	mkdir lib
	cp *.h libmymath.so lib