server: server.o common.o udp_server.o data.o thread_pool.o sub_reactor.o udp_epoll.o
	gcc -o server server.o common.o udp_server.o data.o thread_pool.o sub_reactor.o udp_epoll.o -lpthread

server.o: server.c ../common/*.h
	gcc -c server.c -I ../common/ -D _D

common.o: ../common/common.c
	gcc -c ../common/common.c

udp_server.o: ../common/udp_server.c
	gcc -c ../common/udp_server.c

data.o: ../common/data.c
	gcc -c ../common/data.c

thread_pool.o: ../common/thread_pool.c
	gcc -c ../common/thread_pool.c

sub_reactor.o: ../common/sub_reactor.c
	gcc -c ../common/sub_reactor.c

udp_epoll.o: ../common/udp_epoll.c
	gcc -c ../common/udp_epoll.c

clean:
	rm server server.o common.o udp_server.o data.o thread_pool.o sub_reactor.o udp_epoll.o