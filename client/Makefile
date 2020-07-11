client: common.o udp_client.o data.o client_recv.o client.o 
	gcc -o client common.o udp_client.o data.o client_recv.o client.o -lpthread

client.o: client.c ../common/*.h
	gcc -c client.c -I ../common/ -D _D

common.o: ../common/common.c
	gcc -c ../common/common.c

udp_client.o: ../common/udp_client.c
	gcc -c ../common/udp_client.c

client_recv.o: ../common/client_recv.c
	gcc -c ../common/client_recv.c

data.o: ../common/data.c
	gcc -c ../common/data.c

clean:
	rm client common.o udp_client.o data.o client.o client_recv.o