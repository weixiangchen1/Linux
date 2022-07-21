.PHONY:all
all:cpp_file c_file
cpp_file:cpp_file.cc
		g++ -o $@ $^
c_file:c_file.c
		gcc -o $@ $^
.PHONY:clean
clean:
		rm -rf c_file cpp_file
