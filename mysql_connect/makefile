mysql_test:mysql_test.cc
	g++ -o $@ $^ -I ./include -L ./lib -lmysqlclient -std=c++11 
.PHONY:clean
clean:
	rm -rf mysql_test
