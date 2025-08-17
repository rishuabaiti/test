all:tcp_client tcp_server
tcp_client:tcp_client.cpp ../common/common.cpp
	g++ -g -o $@ $^  -std=c++17 -I../common

tcp_server:tcp_server.cpp ../common/common.cpp
	g++ -g -o $@ $^  -std=c++17 -I../common

.PHONY:clean
clean:
	rm -f tcp_client tcp_server

