.PHONY:all
all:tcp_client tcp_server

tcp_client:tcp_client.cc
	g++ -o $@ $^ -std=c++11 -static

tcp_server:tcp_server.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf tcp_server tcp_client